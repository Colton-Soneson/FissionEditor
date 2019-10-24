//from lunarG, gives all funcs enums and structs
//#include <vulkan/vulkan.h>	


#include "HelloTriangleApplication.h"


int main() {
	HelloTriangleApplication app;

	try {
		app.run();
	}
	catch (const std::exception & e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}