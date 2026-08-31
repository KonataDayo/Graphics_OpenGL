#shader vertex
#version 330 core
layout(location = 0)in vec3 aPos;

uniform mat4 proj;
uniform mat4 view;

out vec3 fragPos;

void main()
{
	fragPos = aPos;
	gl_Position = proj * view * vec4(aPos,1.0);
}

#shader fragment
#version 330 core
uniform sampler2D envMap;
in vec3 fragPos;
out vec4 color;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
	const float PI = 3.14159265359;
	vec3 v = normalize(fragPos);
	vec2 fragUV = SampleSphericalMap(v); 
	color = vec4(texture(envMap, fragUV).rgb, 1.0);
}



