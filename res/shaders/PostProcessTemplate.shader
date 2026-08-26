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
out vec4 color;

void main()
{
	vec3 originColor = texture(screenTexture, TexCoords).rgb;
	vec3 postProcessed = vec3(originColor.r, originColor.g, originColor.b);
	color = vec4(postProcessed,1.0);
}