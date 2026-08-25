#shader vertex
#version 330 core
layout(location = 0) in vec3 position;

uniform mat4 u_mvp;
out vec3 texCoord;

void main()
{
	texCoord = position;
	gl_Position = (u_mvp * vec4(position, 1.0)).xyww;
}


#shader fragment
#version 330 core

in vec3 texCoord;
uniform samplerCube skybox;
out vec4 color;

void main()
{

	color = texture(skybox, texCoord);
}