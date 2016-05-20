#include "utils.h"
void read_entire_file(std::string fname, std::string &contents){
	std::ifstream t(fname);
	std::stringstream buffer;
	buffer << t.rdbuf();
	contents = buffer.str();
}
