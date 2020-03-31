#version 440

// (2 verts/axis * 3 axes/basis * (3 vertex bases + 1 face basis) + 4 or 8 wireframe verts = 28 or 32 verts)
// this is dan's, but i need to look into this more, could probably just set it really high 
#define MAX_VERTICES 32

//layout(binding = 0) uniform UniformBufferObject {
//	mat4 model;
//	mat4 view;
//	mat4 proj;
//	float aspectRatio;
//	float screenHeight;
//	float screenWidth;
//	float time;
//	float ambientMod;
//	float diffuseMod;
//	float specularMod;
//	float lightIntensity;
//	vec3 lightSource;
//	vec3 eyePos;
//} ubo;

layout (triangles) in;
//layout (points) in;

layout (line_strip, max_vertices = 6) out;
//layout (line_strip, max_vertices = 2) out;


//layout(location = 0) out vec4 vColor;

void drawWireframe()
{
	//vColor = vec4(1.0, 0.5, 0.0, 1.0);
//	gl_Position = gl_in[0].gl_Position;
//	EmitVertex();
//	gl_Position = gl_in[1].gl_Position;
//	EmitVertex();
//	gl_Position = gl_in[2].gl_Position;
//	EmitVertex();
//	gl_Position = gl_in[0].gl_Position;
//	EmitVertex();
//
//	EndPrimitive();
//
//	gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0);
//	EmitVertex();
//
//	gl_Position = gl_in[0].gl_Position + vec4(0.1, 0.0, 0.0, 0.0);
//	EmitVertex();
//
//	EndPrimitive();
}

void main()
{
   drawWireframe();
}