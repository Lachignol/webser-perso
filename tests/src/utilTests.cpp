#include "utilTests.hpp"
#include <iostream>

void expectEqual(bool condition, const std::string& message) {
	if (!condition) {
		std::cerr << "❌ Test failed: " << message << std::endl;
	} else {
		std::cout << "✅ " << message << std::endl;
	}
}
