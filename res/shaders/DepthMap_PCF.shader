#shader vertex
#version 330 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoord;
out vec2 v_TexCoord;

void main()
{
	v_TexCoord = texCoord;
	gl_Position = vec4(position.xy, 0.0, 1.0);
}

#shader fragment
#version 330 core

uniform sampler2D depthMap;
in vec2 v_TexCoord;
out vec4 color;

void main()
{

    float rawDepth = texture(depthMap, v_TexCoord).r;
    color = vec4(vec3(rawDepth), 1.0);
}