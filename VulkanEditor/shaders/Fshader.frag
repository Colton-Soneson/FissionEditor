#version 440

layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 2) uniform sampler2D shadowMap;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 resolution;
layout(location = 2) in vec2 fragTextureCoord;
layout(location = 3) in float time;

layout(location = 4) in vec3 vNormal;
layout(location = 5) in vec3 vLightSource;
layout(location = 6) in vec3 vLightVector;
layout(location = 7) in vec3 vEyePos;

layout(location = 8) in vec4 vMV_nrm_by_inNorm;
layout(location = 9) in vec4 vMV_pos;

layout(location = 0) out vec4 outColor;

void main()
{
	//ambient lighting is often reffered to as global alumination
	//	we can also change color of the light
	
//	vec3 lightColor = vec3(1.0, 1.0, 0.6);
//	//vec3 norm = normalize(inNormal);	//normalized normal vector
//	vec3 norm = inNormal;	//normalized normal vector
//	vec3 lightDir = normalize(inLightSource - fragPos);	//distance between light source position and the actual spot on the object
//
//	//ambient lighting
//	vec3 ambient = (ambientStrength * lightColor); 
//
//	//diffuse lighting
//	
//	float diff = max(dot(norm, lightDir), 0.0);		//darker diffuse component the greater the angle, max is to make sure its not negative
//	vec3 diffuse = diffuseStrength * (diff * lightColor);	//remember that we diffuse to the color of incoming light
//	//diffuse = clamp(diffuse, 0.0, 1.0);
//
//	//specular lighting
//	float shine = 64;	//shininess of specular highlights, higher the shine, the less the light is scattered (think matte vs glossy surfaces), powers of 2, 2 to 256 is good range
//	vec3 viewDir = normalize(inEyePos - fragPos);	//view vector from eye to the point we are looking at
//	vec3 reflectDir = reflect(-lightDir, norm);		//lD is negative because its pointing FROM light source, in current state it points towards it
//	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shine);	// positive dot product raised to the power of shine
//	vec3 specular = (specularStrength * spec * lightColor);
//	//specular = clamp(specular, 0.0, 1.0);
//
//
//	//shadowmapping
//	vec3 projCoords = ((fragPosLighSpace.xyz / fragPosLighSpace.w) * 0.5) + 0.5;	//0,1 range perspective divide
//	float depthClosest = texture(shadowMap, projCoords.xy).r;	//closest depth val from lights perspective
//	float currentDepth = projCoords.z;	//current frag depth from lights persective
//	float shadow = currentDepth > depthClosest ? 1.0 : 0.0;
//
//	//final
//	vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * texture(texSampler, fragTextureCoord).xyz; //* texture(texSampler, fragTextureCoord);		
//	outColor = vec4(result, 1.0);


	vec4 t_final_dm = texture(texSampler, fragTextureCoord);
	vec4 t_final_sm = texture(texSampler, fragTextureCoord);
	int shine = 4;
	
	vec4 lightColor = vec4(10.0, 10.0, 10.0, 1.0);

	
	vec4 L = normalize(vec4(vLightSource, 1.0) - vMV_pos);
	vec4 N = normalize(vMV_nrm_by_inNorm);
	vec4 V = normalize(vMV_pos);					//view vector from eye to the point we are looking at
	vec4 R = reflect(-L, N);						//lD is negative because its pointing FROM light source, in current state it points towards it

	vec4 amb = vec4(0.015 * lightColor);

	float diff = max(0.0, dot(N, L));
	vec4 diffuse = vec4((diff * t_final_dm) * lightColor);

	float spec = max(0.0, dot(V, R));
	for(int i = 0; i < shine; i++)
	{
		spec *= spec;
	}
	vec4 specular = vec4((spec * t_final_sm) * lightColor);
	vec4 temp = (amb + diffuse + specular) * t_final_dm;

	outColor = temp;
}
