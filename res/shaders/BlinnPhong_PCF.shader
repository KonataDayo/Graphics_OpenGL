#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 u_Normal;
layout(location = 2) in vec2 aTexCoord;
uniform mat4 u_Model;
uniform mat4 u_MVP;
uniform mat4 lightSpaceMatrix;
out vec4 lightSpaceFragPos;
out vec3 u_FragPos;
out vec3 Normal;
out vec2 texCoord;
void main()
{
    texCoord = aTexCoord;
    u_FragPos = vec3(u_Model * vec4(position, 1.0));
    gl_Position = u_MVP * vec4(position.xyz,1.0);
    Normal = transpose(inverse(mat3(u_Model))) * u_Normal;
    lightSpaceFragPos = lightSpaceMatrix * vec4(u_FragPos, 1.0);
}

#shader fragment
#version 330 core

out vec4 color;

in vec3 u_FragPos;
in vec3 Normal;
in vec4 lightSpaceFragPos;
in vec2 texCoord;

uniform sampler2D u_Texture;
uniform sampler2D u_DepthMap;
uniform vec3 u_LightPosition;
uniform vec3 u_CameraLocation;

uniform int dm_width;
uniform int dm_height;
uniform int filter_size;
uniform vec2 poisson_disk[64];

// ============================================================
// Shadow map parameters - these MUST match the C++ side:
//   glm::ortho(-10, 10, -10, 10, 0.1, 20)
// An orthographic projection stores LINEAR depth along the
// light's view axis:
//   depth = (lightViewDist - near) / (far - near)
// so every stored depth converts to a world distance with
// depthToLinear(). (If the C++ ever switches to a perspective
// light projection, this conversion must change too.)
// ============================================================
const float LIGHT_NEAR_PLANE       = 0.1;
const float LIGHT_FAR_PLANE        = 20.0;
const float LIGHT_FRUSTUM_SIZE     = 20.0;  // world width of the ortho frustum (-10..10), spans the full 0..1 UV range
const float LIGHT_SIZE             = 1.0;   // area-light diameter in world units (raise it for softer shadows)
const float MAX_BLOCKER_SEARCH_UV  = 0.025; // cap for the blocker search (25 texels @1024) - keeps it local so the
                                            // receiver's own surface is not mistaken for a blocker
const int   MAX_PCSS_FILTER_TEXELS = 64;
const int   BLOCKER_SEARCH_SAMPLES = 64;    // keep in sync with POISSON_MAX_SAMPLES on the C++ side

// stored depth [0,1] -> world distance from the light (linear because ortho)
float depthToLinear(float depth)
{
    return LIGHT_NEAR_PLANE + depth * (LIGHT_FAR_PLANE - LIGHT_NEAR_PLANE);
}

// slope-scaled bias in shadow-map depth units [0,1]
float shadowBias(vec3 lightDir)
{
    float cosTheta = max(0.0, dot(normalize(Normal), lightDir));
    return max(0.0015, 0.01 * (1.0 - cosTheta));
}

float calculateShadow(vec4 fragPosLightSpace, vec3 lightDir)
{
    vec3 proj = fragPosLightSpace.xyz / fragPosLightSpace.w; // w == 1 for an ortho matrix
    proj = proj * 0.5 + 0.5;
    if (proj.x < 0.0 || proj.x > 1.0 ||
        proj.y < 0.0 || proj.y > 1.0 ||
        proj.z < 0.0 || proj.z > 1.0)
    {
        return 0.0;
    }
    float closestDepth = texture(u_DepthMap, proj.xy).r;
    float currentDepth = proj.z;
    float bias = shadowBias(lightDir);
    return currentDepth - bias > closestDepth ? 1.0 : 0.0;
}

// Grid, No weight
float PCF(sampler2D shadowMap, vec4 lightSpaceFragPos, int filterSize, vec3 lightDir)
{
    // delta unit: pixel (clip space)
	float delta_height = 2.0 / float(dm_height);
	float delta_width = 2.0 / float(dm_width);
    float sum = 0.0;
    vec4 fragPos;
    fragPos.zw = lightSpaceFragPos.zw;

    if (filterSize % 2 == 1) // odd
    {
	    for(int i = -filterSize/2; i <= filterSize/2; i++)
	    {
            fragPos.x = lightSpaceFragPos.x + i * delta_width;
            for(int j = -filterSize/2; j <= filterSize/2; j++)
            {
                fragPos.y = lightSpaceFragPos.y + j * delta_height;
                sum += calculateShadow(fragPos, lightDir);
            }
        }
        return sum / (filterSize * filterSize);
    }
    else // even
    {
        fragPos.x = lightSpaceFragPos.x - (filterSize/2 + 0.5) * delta_width;
        for (int i = 0; i < filterSize; i++)
        {
            fragPos.x += delta_width;
            fragPos.y = lightSpaceFragPos.y - (filterSize/2 + 0.5) * delta_height;
            for(int j = 0; j < filterSize; j++)
            {
                fragPos.y += delta_height;
                sum += calculateShadow(fragPos, lightDir);
            }
        }
        return sum / (filterSize * filterSize);
    }
}

// filterSize is the kernel radius in shadow-map texels
float PoissonDiskSampling_PCF(sampler2D shadowMap, vec4 lightSpaceFragPos, int filterSize, vec3 lightDir)
{
    // a texel in clip space [-1,1]: 2.0 / resolution
    float delta_height = 2.0 / float(dm_height);
	float delta_width = 2.0 / float(dm_width);
    vec4 fragPos;
    fragPos.zw = lightSpaceFragPos.zw;

    // Sample the center first: poisson_disk[0] is always (0,0), while the
    // remaining (0,0) entries are just zero-padding from the C++ side.
    float sum = calculateShadow(lightSpaceFragPos, lightDir);
    float validPoint = 1.0;
    for (int i = 1; i < 64; i++)
    {
        if (poisson_disk[i] == vec2(0.0, 0.0))
        {
            continue;
        }
        fragPos.x = lightSpaceFragPos.x + float(filterSize) * delta_width * poisson_disk[i].x;
        fragPos.y = lightSpaceFragPos.y + float(filterSize) * delta_height * poisson_disk[i].y;
        sum += calculateShadow(fragPos, lightDir);
        validPoint += 1.0;
    }
    return sum / validPoint;
}

// Returns the average depth [0,1] of the blockers around the receiver,
// or -1.0 if no blocker is found.
float BlockerSearch(sampler2D shadowMap, vec4 lightSpaceFragPos, vec3 lightDir)
{
    vec3 proj = lightSpaceFragPos.xyz / lightSpaceFragPos.w;
    proj = proj * 0.5 + 0.5;
    float receiverDepth = proj.z;
    float receiverDist  = depthToLinear(receiverDepth);

    // Similar triangles: blockers that can cast a penumbra onto this fragment
    // live within roughly the light's own footprint at the receiver's distance.
    //   world -> UV : divide by the frustum width (constant for ortho)
    //   (receiverDist - near) / receiverDist : shrinks the search close to the
    //   light, where the penumbra is small.
    float searchUV = (LIGHT_SIZE / LIGHT_FRUSTUM_SIZE)
                   * (receiverDist - LIGHT_NEAR_PLANE) / receiverDist;
    searchUV = clamp(searchUV, 1.0 / float(dm_width), MAX_BLOCKER_SEARCH_UV);

    float bias = shadowBias(lightDir);
    float blockerNum = 0.0;
    float blockerSum = 0.0;
    for (int i = 0; i < BLOCKER_SEARCH_SAMPLES; i++)
    {
        vec2 sampleUV = proj.xy + poisson_disk[i] * searchUV;
        if (sampleUV.x < 0.0 || sampleUV.x > 1.0 ||
            sampleUV.y < 0.0 || sampleUV.y > 1.0)
        {
            continue;
        }
        float sampleDepth = texture(shadowMap, sampleUV).r;
        if (sampleDepth < receiverDepth - bias)
        {
            blockerNum += 1.0;
            blockerSum += sampleDepth;
        }
    }
    if (blockerNum < 0.5)
    {
        return -1.0;
    }
    return blockerSum / blockerNum;
}

float PCSS(sampler2D shadowMap, vec4 lightSpaceFragPos, vec3 lightDir)
{
    vec3 proj = lightSpaceFragPos.xyz / lightSpaceFragPos.w;
    proj = proj * 0.5 + 0.5;

    if (proj.x < 0.0 || proj.x > 1.0 ||
        proj.y < 0.0 || proj.y > 1.0 ||
        proj.z < 0.0 || proj.z > 1.0)
    {
        return 0.0;
    }

    // Step 1: average blocker depth
    float avgBlockerDepth = BlockerSearch(shadowMap, lightSpaceFragPos, lightDir);
    if (avgBlockerDepth < 0.0)
    {
        return 0.0; // no blocker -> fully lit
    }

    // Step 2: penumbra width at the receiver (similar triangles, world units)
    float receiverDist  = depthToLinear(proj.z);
    float blockerDist   = depthToLinear(avgBlockerDepth);
    float penumbraWidth = (receiverDist - blockerDist) / blockerDist * LIGHT_SIZE;

    // Step 3: PCF kernel radius.
    //   world -> UV    : the ortho frustum has a constant world width,
    //                    so divide by LIGHT_FRUSTUM_SIZE
    //   half width     : the penumbra band straddles the shadow edge, so the
    //                    kernel only needs to cover half of it
    //   UV -> texels   : multiply by the shadow-map resolution
    float kernelUV  = penumbraWidth * 0.5 / LIGHT_FRUSTUM_SIZE;
    int filterSize = clamp(int(kernelUV * float(dm_width)), 1, MAX_PCSS_FILTER_TEXELS);

    return PoissonDiskSampling_PCF(shadowMap, lightSpaceFragPos, filterSize, lightDir);
}

void main()
{
    vec4 u_LightColor = vec4(1.0, 1.0, 1.0, 1.0);
    float u_AmbientStrength = 0.3;
    float u_DiffuseCoefficient = 0.4;
    float u_Intensity = 500.0;
    float u_SpecularCoefficient = 0.7;
    float u_SpecularSize = 64;

    vec4 ambient = u_AmbientStrength * u_LightColor;

    float distance = length(u_LightPosition - u_FragPos);
    float energyArrived = u_Intensity / (distance * distance);
    vec3 lightDir = normalize(u_LightPosition - u_FragPos);
    float diff = max(0.0, dot(normalize(Normal), lightDir));
    vec4 diffuse = u_DiffuseCoefficient * energyArrived * diff * u_LightColor;

    vec3 eyeDir = normalize(u_CameraLocation - u_FragPos);
    vec3 bisector = normalize(eyeDir + lightDir);

    float reflectionLobe = max(0.0, dot(normalize(Normal),bisector));

    vec4 specular = u_SpecularCoefficient * energyArrived * pow(reflectionLobe, u_SpecularSize) * u_LightColor;

    //float shadow = calculateShadow(lightSpaceFragPos, lightDir);
    //float shadow = PCF(u_DepthMap, lightSpaceFragPos, filter_size, lightDir);
    //float shadow = PoissonDiskSampling_PCF(u_DepthMap, lightSpaceFragPos, filter_size, lightDir);
    float shadow = PCSS(u_DepthMap, lightSpaceFragPos, lightDir);

    color = (ambient + (diffuse + specular) * (1.0 - shadow)) * vec4(texture(u_Texture, texCoord).rgb, 1.0);
}
