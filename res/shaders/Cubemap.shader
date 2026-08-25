#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_proj;
out vec3 v_normal;
out vec3 v_pos;

void main()
{
	v_normal = mat3(transpose(inverse(u_model))) * normal;
	v_pos = vec3(u_model * vec4(position, 1.0));
	gl_Position = u_proj * u_view * u_model * vec4(position, 1.0);
}


#shader fragment
#version 330 core

uniform vec3 u_cameraPos;
uniform samplerCube skybox;
in vec3 v_normal;
in vec3 v_pos;
out vec4 color;

void main()
{
	vec3 I = normalize(v_pos - u_cameraPos);
	vec3 R = reflect(I, normalize(v_normal));
	color = vec4(texture(skybox, R).rgb, 1.0);
}