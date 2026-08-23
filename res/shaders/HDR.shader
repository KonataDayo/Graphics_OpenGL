#shader vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

out vec2 v_TexCoord;

void main()
{
    v_TexCoord =
        a_TexCoord;

    // Fullscreen quad is already in NDC.
    gl_Position =
        vec4(
            a_Position,
            1.0
        );
}


#shader fragment
#version 330 core

in vec2 v_TexCoord;

out vec4 FragColor;

uniform sampler2D u_HDRBuffer;
uniform float u_Exposure;

void main()
{
    // =========================================================
    // Read HDR color
    // =========================================================

    vec3 hdrColor =
        texture(
            u_HDRBuffer,
            v_TexCoord
        ).rgb;


    // =========================================================
    // Exposure + tone mapping
    //
    // Exposure changes the perceived brightness:
    //
    // result = 1 - exp(-HDR * exposure)
    // =========================================================

    vec3 mapped =
        vec3(1.0) -
        exp(
            -hdrColor *
            u_Exposure
        );


    // =========================================================
    // Gamma correction
    //
    // Since your display is approximately sRGB,
    // convert linear output to display space.
    // =========================================================

    mapped =
        pow(
            mapped,
            vec3(1.0 / 2.2)
        );


    //FragColor = vec4(hdrColor, 1.0);

    FragColor = vec4(mapped, 1.0);
}