#pragma once

class ProceduralLoader
{
public:

	ProceduralLoader() {};
	~ProceduralLoader() {};

	sourced3D createAndStoreIcoSphere(float radius, float definiton);
	sourced3D createAndStoreUVSphere(float radius, int sectors, int stacks);	//http://www.songho.ca/opengl/gl_sphere.html
	sourced3D createAndStoreCube(float height, float width, float depth, float subdivisions);
	sourced3D createAndStorePlane(float height, float width, float subdivisions);

private:
	//std::vector<sourced3D> mOriginals;
};