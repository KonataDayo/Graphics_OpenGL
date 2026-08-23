#shader vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

uniform mat4 u_MVP;
uniform mat4 u_Model;

out vec3 v_FragPos;
out vec3 v_Normal;
out vec2 v_TexCoord;

void main()
{
    v_FragPos =
        vec3(
            u_Model *
            vec4(a_Position, 1.0)
        );

    v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;

    v_TexCoord = a_TexCoord;

    gl_Position =
        u_MVP *
        vec4(a_Position, 1.0);
}


#shader fragment
#version 330 core

in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoord;

out vec4 FragColor;

uniform sampler2D u_Texture;

uniform vec3 u_LightPositions[4];
uniform vec3 u_LightColors[4];

uniform bool u_InverseNormals;

void main()
{
    // =========================================================
    // Texture
    // =========================================================

    vec3 albedo =
        texture(
            u_Texture,
            v_TexCoord
        ).rgb;


    // =========================================================
    // Normal
    // =========================================================

    vec3 normal =
        normalize(v_Normal);


    if (u_InverseNormals)
    {
        normal = -normal;
    }


    // =========================================================
    // Lighting
    // =========================================================

    vec3 lighting =
        vec3(0.0);


    for (int i = 0; i < 4; ++i)
    {
        vec3 lightDir =
            u_LightPositions[i] -
            v_FragPos;


        float distance =
            length(lightDir);


        lightDir =
            normalize(lightDir);


        // Lambert
        float diffuse =
            max(
                dot(
                    normal,
                    lightDir
                ),
                0.0
            );


        // Inverse square attenuation
        float attenuation =
            1.0 /
            (
                distance *
                distance
            );


        lighting +=
            albedo *
            diffuse *
            u_LightColors[i] *
            attenuation;
    }


    // Tiny ambient
    lighting +=
        albedo * 0.01;


    // =========================================================
    // IMPORTANT
    //
    // No tone mapping here.
    //
    // lighting is allowed to be > 1.0
    //
    // Example:
    //     (2, 1, 0.5)
    //     (10, 5, 2)
    //     (100, 100, 100)
    // =========================================================

    FragColor =
        vec4(
            lighting,
            1.0
        );
}   