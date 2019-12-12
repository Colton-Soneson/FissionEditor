#version 440

layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 2) uniform sampler2D shadowMap;

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

layout(location = 11) in float ambientStrength;
layout(location = 12) in float diffuseStrength;
layout(location = 13) in float specularStrength;

layout(location = 14) in vec4 fragPosLighSpace;

layout(location = 0) out vec4 outColor;

void main()
{
	//ambient lighting is often reffered to as global alumination
	//	we can also change color of the light
	
	vec3 lightColor = vec3(1.0, 1.0, 0.6);
	//vec3 norm = normalize(inNormal);	//normalized normal vector
	vec3 norm = inNormal;	//normalized normal vector
	vec3 lightDir = normalize(inLightSource - fragPos);	//distance between light source position and the actual spot on the object

	//ambient lighting
	vec3 ambient = (ambientStrength * lightColor); 

	//diffuse lighting
	
	float diff = max(dot(norm, lightDir), 0.0);		//darker diffuse component the greater the angle, max is to make sure its not negative
	vec3 diffuse = diffuseStrength * (diff * lightColor);	//remember that we diffuse to the color of incoming light
	//diffuse = clamp(diffuse, 0.0, 1.0);

	//specular lighting
	float shine = 64;	//shininess of specular highlights, higher the shine, the less the light is scattered (think matte vs glossy surfaces), powers of 2, 2 to 256 is good range
	vec3 viewDir = normalize(inEyePos - fragPos);	//view vector from eye to the point we are looking at
	vec3 reflectDir = reflect(-lightDir, norm);		//lD is negative because its pointing FROM light source, in current state it points towards it
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shine);	// positive dot product raised to the power of shine
	vec3 specular = (specularStrength * spec * lightColor);
	//specular = clamp(specular, 0.0, 1.0);


	//shadowmapping
	vec3 projCoords = ((fragPosLighSpace.xyz / fragPosLighSpace.w) * 0.5) + 0.5;	//0,1 range perspective divide
	float depthClosest = texture(shadowMap, projCoords.xy).r;	//closest depth val from lights perspective
	float currentDepth = projCoords.z;	//current frag depth from lights persective
	float shadow = currentDepth > depthClosest ? 1.0 : 0.0;

	//final
	vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * texture(texSampler, fragTextureCoord).xyz; //* texture(texSampler, fragTextureCoord);		
	outColor = vec4(result, 1.0);
}
