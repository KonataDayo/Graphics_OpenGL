#shader vertex
#version 330 core

layout(location = 0) in vec3 a_Position;

uniform mat4 u_MVP;

void main()
{
    gl_Position =
        u_MVP *
        vec4(
            a_Position,
            1.0
        );
}


#shader fragment
#version 330 core

uniform vec3 u_LightColor;

out vec4 FragColor;

void main()
{
    FragColor =
        vec4(
            u_LightColor,
            1.0
        );
}