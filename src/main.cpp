#include <string>
#include <algorithm>
#include <vector>
#include <array>
#include <array>
#include <iostream>
#include <fstream>
#include <sstream>
#include <streambuf>
#include "assembler.h"


void read_entire_file(std::string fname, std::string &contents){
	std::ifstream t(fname);
	std::stringstream buffer;
	buffer << t.rdbuf();
	contents = buffer.str();
}

int main(void){
	std::string program;
}
