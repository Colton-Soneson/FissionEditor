#version 440

// (2 verts/axis * 3 axes/basis * (3 vertex bases + 1 face basis) + 4 or 8 wireframe verts = 28 or 32 verts)
// this is dan's, but i need to look into this more, could probably just set it really high 
#define MAX_VERTICES 32

layout (triangles) in;

layout (line_strip, max_vertices = MAX_VERTICES) out;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 resolution;
layout(location = 2) out vec2 fragTextureCoord;
layout(location = 3) out float time;

layout(location = 4) out vec3 vNormal;
layout(location = 5) out vec3 vLightSource;
layout(location = 6) out vec3 vLightVector;
layout(location = 7) out vec3 vEyePos;

layout(location = 8) out vec4 vMV_nrm_by_inNorm;
layout(location = 9) out vec4 vMV_pos;


void drawWireframe()
{
	fragColor = vec3(1.0, 0.5, 0.0);
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();

	EndPrimitive();
}

void main()
{
    drawWireframe();
}