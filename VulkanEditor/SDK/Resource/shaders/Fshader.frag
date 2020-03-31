#version 440

layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 2) uniform sampler2D shadowMap;

layout(location = 0) in vec2 resolution;
layout(location = 1) in vec2 fragTextureCoord;
layout(location = 2) in float time;

layout(location = 3) in vec3 vLightSource;
layout(location = 4) in float vLightIntensity;
layout(location = 5) in vec3 vEyePos;

layout(location = 6) in vec4 vMV_nrm_by_inNorm;
layout(location = 7) in vec4 vMV_pos;

layout(location = 8) in float vAmbientMod;
layout(location = 9) in float vDiffuseMod;
layout(location = 10) in float vSpeculartMod;


layout(location = 0) out vec4 outColor;

void main()
{

	vec4 t_final_dm = texture(texSampler, fragTextureCoord);
	vec4 t_final_sm = texture(texSampler, fragTextureCoord);
	int shine = 4;
	
	vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0) * vLightIntensity;

	
	vec4 L = normalize(vec4(vLightSource, 1.0) - vMV_pos);
	vec4 N = normalize(vMV_nrm_by_inNorm);
	vec4 V = normalize(vMV_pos);					//view vector from eye to the point we are looking at
	vec4 R = reflect(-L, N);						//lD is negative because its pointing FROM light source, in current state it points towards it

	vec4 amb = vec4(vAmbientMod * lightColor);	//0.015

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
