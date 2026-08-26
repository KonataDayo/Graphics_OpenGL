#shader vertex
#version 330 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoord;

out vec2 TexCoords;

void main()
{
	TexCoords = texCoord;
	gl_Position = vec4(position.xy, 0.0, 1.0);
}

#shader fragment
#version 330 core

in vec2 TexCoords;
uniform sampler2D screenTexture;
uniform bool u_HDR;
out vec4 color;

vec3 ACES_ToneMapping(vec3 color)
{
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;

    return clamp((color * (a * color + b)) / (color * (c * color + d) + e), 0.0, 1.0);
}

void main()
{
	vec3 hdrColor = texture(screenTexture, TexCoords).rgb;
	vec3 mapped;
	if (u_HDR)
	{
		// tone mapping
		mapped = ACES_ToneMapping(hdrColor);
	}
	else 
	{
		mapped = clamp(hdrColor, 0.0, 1.0);
	}
	// Gamma Correction
	mapped = pow(mapped, vec3(1.0/2.2));
	color = vec4(mapped,1.0);
}