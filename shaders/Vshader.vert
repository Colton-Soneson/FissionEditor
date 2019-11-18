#version 440

#define MAX_ITER	100
#define MAX_DIST	10.0
#define EPSILON		0.001

layout(binding = 0) uniform UniformBufferObject {
	mat4 model;
	mat4 view;
	mat4 proj;
	float aspectRatio;
	float screenHeight;
	float screenWidth;
	float time;
	vec2 uv;
} ubo;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 resolution;
layout(location = 2) out float time;


float sphereRayMarch(vec3 p, float s);
float distanceFunction(vec3 position);


float sphereRayMarch(vec3 p, float s)
{
	return length(p) - s;
}

float distanceFunction(vec3 position)
{
	float t = 0.8;
	float f = sphereRayMarch(position, t);
	return f;
}


void main() {
	
	vec3 camOrigin	= vec3(2.0);
    vec3 camTarget	= vec3(0.0);
    vec3 upDir		= vec3(0.0, 1.0, 0.0);
    
    vec3 camDir		= normalize(camTarget - camOrigin);
    vec3 camRight	= normalize(cross(upDir, camOrigin));
    vec3 camUp		= cross(camDir, camRight);

	//place origin at center of screen & correct aspect ratio
    vec2 screenPos = -1.0 + 2.0 * ubo.uv / vec2(ubo.screenWidth, ubo.screenHeight);
    screenPos.x *= ubo.screenWidth / ubo.screenHeight;
    
    vec3 rayDir = normalize(camRight * screenPos.x + camUp * screenPos.y + camDir);
    
	
    //Raymarching loop
    float totalDist = 0.0;
    vec3 pos = camOrigin;
    float dist = EPSILON;
    
    for (int i = 0; i < MAX_ITER; i++) {
        if (dist < EPSILON || totalDist > MAX_DIST) {
            break;
        }
        
        dist = distanceFunction(pos);
        totalDist += dist;
        pos += dist * rayDir;
        
    }
    if (dist < EPSILON) {				//the ray hit the object
    	vec2 eps = vec2(0.0, EPSILON);
    	vec3 normal = normalize(vec3(
       			distanceFunction(pos + eps.yxx) - distanceFunction(pos - eps.yxx),
       			distanceFunction(pos + eps.xyx) - distanceFunction(pos - eps.xyx),
       			distanceFunction(pos + eps.xxy) - distanceFunction(pos - eps.xxy)));
    
    	float diffuse = max(0.0, dot(-rayDir, normal));
    	float specular = pow(diffuse, 128.0);
        float lighting = diffuse + specular;
        float toonMap;
        
        //map lighting information to discrete values
        if (lighting < 0.256) {
            toonMap = 0.195;
        } else if (lighting < 0.781) {
            toonMap = 0.781;
        } else {
            toonMap = 0.900;
        }
        vec3 color = vec3(toonMap);
        fragColor = color;
    } 
	else {						//the ray didn't hit anything
        fragColor = vec3(0.5, 0.7, 0.7);
    }

    //gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 0.0, 1.0);
}