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
#include "utils.h"

int main(void){
	Assembler a = Assembler();
	std::vector<uint32_t> bytecode;
	a.Assemble("testprog.asm",bytecode);
}
