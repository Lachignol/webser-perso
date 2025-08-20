#include "unitTests.hpp"
#include <string>
#include <vector>
#include <iostream>
#include "stringUtils.hpp"

void testSplit() {
	std::string str = "foo   bar  ";
	std::vector<std::string> tokens = stringUtils::split(str, ' ');

	for (std::vector<std::string>::const_iterator it = tokens.begin();
	it != tokens.end(); ++it) {
		std::cout << *it << std::endl;
	}
}
