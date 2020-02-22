#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 resolution;
layout(location = 2) in vec2 fragTextureCoord;
layout(location = 3) in float time;

layout(location = 4) in vec3 inNormal;
layout(location = 5) in vec3 inLightSource;
layout(location = 6) in vec3 inLightVector;
layout(location = 7) in vec3 inEyePos;

layout(location = 8) in vec3 N;    
layout(location = 9) in vec3 v;	

layout(location = 0) out vec4 outColor;

void main()
{
	//ambient lighting is often reffered to as global alumination
	//	we can also change color of the light
	vec3 lightColor = vec3(1.0, 0.0, 1.0);

	float ambientStength = 0.5;
	vec4 ambient = vec4(ambientStength * lightColor, 1.0); 
	
	outColor = ambient * texture(texSampler, fragTextureCoord);	
}