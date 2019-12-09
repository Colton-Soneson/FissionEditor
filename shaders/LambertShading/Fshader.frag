#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 resolution;
layout(location = 2) in vec2 fragTextureCoord;
layout(location = 3) in vec3 fragPos;
layout(location = 4) in float time;

layout(location = 5) in vec3 inNormal;
layout(location = 6) in vec3 inLightSource;
layout(location = 7) in vec3 inLightVector;
layout(location = 8) in vec3 inEyePos;

layout(location = 9) in vec3 N;    
layout(location = 10) in vec3 v;	

layout(location = 0) out vec4 outColor;

void main()
{
	//ambient lighting is often reffered to as global alumination
	//	we can also change color of the light
	
	vec3 lightColor = vec3(1.0, 0.0, 1.0);
	vec3 norm = normalize(inNormal);	//normalized normal vector
	vec3 lightDir = normalize(inLightSource - fragPos);	//distance between light source position and the actual spot on the object

	//ambient lighting
	//float ambientStength = 0.5;
	//vec4 ambient = vec4(ambientStength * lightColor, 1.0); 

	//diffuse lighting
	float diff = max(dot(norm, lightDir), 0.0);		//darker diffuse component the greater the angle
	vec4 diffuse = vec4(diff * lightColor, 1.0);	//remember that we diffuse to the color of incoming light
	
	//outColor = (ambient + diffuse)* texture(texSampler, fragTextureCoord);	
	diffuse = clamp(diffuse, 0.0, 1.0);

	outColor = diffuse;
}