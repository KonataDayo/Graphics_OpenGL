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

void main()
{
	const float PI = 3.14159265359;
	vec3 v = normalize(fragPos);
	vec3 fragUV; 
	// theta
	fragUV.x = atan(v.z, v.x) / (2.0 * PI) + 0.5;
	// phi
	fragUV.y = asin(v.y) / PI + 0.5;
	
	color = vec4(texture(envMap, fragUV.xy).rgb, 1.0);
}

