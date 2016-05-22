#include "Assembler.h"
#define DEBUG 1

/**
 * @brief Return the control bits of instruction i
 */

inline uint8_t ctrl_of(Instruction i){
	return (i & ctrl_mask) >> 27;
}

/**
 * @brief Return the data bits of instruction i
 */
inline uint32_t data_of(Instruction i){
	return i & data_mask;
}

/**
 * @brief Set the control segment of an instruction
 * @param ctrl A 5 bit number (one of the control bits specified in the datasheet)
 * @param i The instruction to be modified
 * @note The upper 3 bits of "ctrl" will be cleared to ensure that nothing bad happens
 */
inline void set_ctrl(Instruction &i, uint8_t ctrl){
	i |= (ctrl & 0b00011111)  << 27; 
}

/**
 * @brief Set the data segment of an instruction
 * @param data A 27 bit number (specified by the datasheet)
 * @param i The instruction to be modified
 * @note The upper 5 bits of "data" will be cleared in order to ensure that the ctrl bits will not be effected
 */
inline void set_data(Instruction &i, uint32_t data){
	i |= (data & (0b00000 << 27)); //make sure the upper 5 bits are clear as not to change the ctrl
}


Assembler::Assembler(){
	instruction_ctrls.insert({"add",add_ctrl});
	instruction_ctrls.insert({"sub",sub_ctrl});
	instruction_ctrls.insert({"mul",mul_ctrl});
	instruction_ctrls.insert({"div",div_ctrl});
	instruction_ctrls.insert({"and",and_ctrl});
	instruction_ctrls.insert({"or" , or_ctrl});
	instruction_ctrls.insert({"not",not_ctrl});
	instruction_ctrls.insert({"loa",loa_ctrl});
	instruction_ctrls.insert({"sto",sto_ctrl});
	instruction_ctrls.insert({"shr",shr_ctrl});
	instruction_ctrls.insert({"shl",shl_ctrl});
	instruction_ctrls.insert({"beq",beq_ctrl});
	instruction_ctrls.insert({"blt",blt_ctrl});
	instruction_ctrls.insert({"ll" , ll_ctrl});

}

Assembler::~Assembler(){}

/**
 * 
 */
void Assembler::Parse(const std::string &raw_program, std::vector<Expression> &parsed){
	//Split into lines
	std::vector<std::string> lines;
	boost::split(lines,raw_program,boost::is_any_of("\n"));

	//Remove all empty lines
	lines.erase(std::remove_if(lines.begin(),lines.end(),[](const std::string& s){
		return s.empty();
	}),lines.end());


	for(const auto &expr_str: lines){
		std::cout << "Parsing instruction: \"" <<  expr_str << "\"" <<std::endl;

		//Split into tokens
		std::vector<std::string> tokens;
		boost::split(tokens,expr_str,boost::is_any_of(" \t"));

		//Make sure it's one of the valid operations
		bool valid_op = !(instruction_ctrls.find(tokens[0]) == instruction_ctrls.end());
		assert(valid_op);

		Expression expr;
		//Take the first token (the op)
		expr.op = tokens.at(0);
		//Remove the first token
		tokens.erase(tokens.begin());
		//Set the rest to the args
		expr.args = tokens;
		//Add it to the list
		parsed.push_back(expr);

	}

}

/**
 *	Assemble the program
 */
void Assembler::Assemble(const std::string &fname, std::vector<uint32_t> &assembled){
	USE(fname);
	USE(assembled);
	printf("Assembling program from file %s\n",fname.c_str());

	//Read file from disk into memory
	std::string raw_program;
	read_entire_file(fname,raw_program);
	printf("Loaded text from file %s\n",fname.c_str());

	//Parse the program into expressions
	std::vector<Expression> parsed;
	Parse(raw_program,parsed);

	for(const auto &expr : parsed){
		printf("%s:\n",expr.op.c_str());
	}

}
