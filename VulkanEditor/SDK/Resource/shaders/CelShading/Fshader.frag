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
	
	vec3 lightColor = vec3(1.0, 1.0, 1.0);
	vec4 sceneColor = vec4(0.0,0.0,0.0,1.0); //ambient color of the scene
	float celEyeNormMax = 0.5; 
	float celEyeNormTextMult = 0.1; 
	float celLightNormMax = 0.1;
	float celPartialLitTextMult = 0.5;
	float celSpecularMin = 0.5;

	vec3 norm = normalize(inNormal);	//normalized normal vector
	vec3 lightDir = normalize(inLightSource - fragPos);	//distance between light source position and the actual spot on the object

	//ambient lighting
	float ambientStength = 0.1;
	vec4 ambient = vec4(ambientStength * lightColor, 1.0); 

	//diffuse lighting
	float diff = max(dot(norm, lightDir), 0.0);		//darker diffuse component the greater the angle, max is to make sure its not negative
	vec4 diffuse = vec4(diff * lightColor, 1.0);	//remember that we diffuse to the color of incoming light
	diffuse = clamp(diffuse, 0.0, 1.0);

	//specular lighting
	float specularStrength = 0.5;	//how hard of an impact (the brightness) is this highlight going to have
	float shine = 32;	//shininess of specular highlights, higher the shine, the less the light is scattered (think matte vs glossy surfaces), powers of 2, 2 to 256 is good range
	vec3 viewDir = normalize(inEyePos - fragPos);	//view vector from eye to the point we are looking at
	vec3 reflectDir = reflect(-lightDir, norm);		//lD is negative because its pointing FROM light source, in current state it points towards it
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shine);	// positive dot product raised to the power of shine
	vec4 specular = vec4(specularStrength * spec * lightColor, 1.0);
	specular = clamp(specular, 0.0, 1.0);


	//cel shading
	//------------------------
	if(spec > celSpecularMin)	//if specular value is high enough slam our color up max brightness
	{
		outColor = vec4(1.0, 1.0, 1.0, 1.0);
	}
	else if (dot(viewDir, norm) < celEyeNormMax)	//if our eye vs normal angle is lower, the fragcolor is immensely darker
	{
		outColor = texture(texSampler, fragTextureCoord) * celEyeNormTextMult;
	}
	else if (diff >= celLightNormMax)	//if our of light vs normal angle is greater, we see the object for the color it is
	{
		outColor = texture(texSampler, fragTextureCoord);
	}
	else	//everything inbetween these values gets partially lit
	{
		outColor = texture(texSampler, fragTextureCoord) * celPartialLitTextMult;
	}
}