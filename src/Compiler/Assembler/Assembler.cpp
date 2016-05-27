#include "Assembler.h"
#define DEBUG 1

constexpr const uint8_t Assembler::add_ctrl;
constexpr const uint8_t Assembler::sub_ctrl;
constexpr const uint8_t Assembler::mul_ctrl;
constexpr const uint8_t Assembler::div_ctrl;
constexpr const uint8_t Assembler::and_ctrl;
constexpr const uint8_t Assembler::or_ctrl;
constexpr const uint8_t Assembler::not_ctrl;
constexpr const uint8_t Assembler::loa_ctrl;
constexpr const uint8_t Assembler::sto_ctrl;
constexpr const uint8_t Assembler::shr_ctrl;
constexpr const uint8_t Assembler::shl_ctrl;
constexpr const uint8_t Assembler::beq_ctrl;
constexpr const uint8_t Assembler::blt_ctrl;
constexpr const uint8_t Assembler::ll_ctrl;
constexpr const uint8_t Assembler::dw_ctrl;
std::map<std::string,uint16_t> Assembler::reg_lookup;

/**
 *	Regex for a valid register name
 */
const std::string register_name_regex = 	"^(r|R)\\d{1,3}|(PC|SP|FP|ZR|FR|WR)$";
/**
 *	Regex for a general purpouse register
 *	@note does not validate the validity of the register number
 */
const std::string gp_reg_regex =			"^(r|R)\\d{1,3}$";
/**
 *	Regex for a special purpouse register
 */
const std::string sp_reg_regex = 			"^(PC|SP|FP|ZR|FR|WR)$";

template <typename T>
bool validate(T s, const std::function<bool(T)>& validation, std::string warning = "Error validating."){
	auto valid = validation(s);
	if(valid) return true;
	std::cout << warning << std::endl;
	return false;
}

inline bool validate_on_regex(const std::string &s, const std::string &regex, const std::string &warning = ""){
	return validate<std::string>(s, [=](std::string s) -> bool{
			return std::regex_match(s,std::regex(regex));
			},warning);
}

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

/**
 * @brief set an argument in position pos to the value of iarg
 */
inline void set_arg(Instruction &i, uint8_t pos, uint16_t iarg){
	assert(pos <= 2);
	i |= (iarg & nine_bits) << (18 - (9 * pos));
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
	instruction_ctrls.insert({"dw" , dw_ctrl});
	instruction_ctrls.insert({"DW" , dw_ctrl});

	reg_lookup["PC"] = 0;
	reg_lookup["SP"] = 1;
	reg_lookup["FP"] = 2;
	reg_lookup["ZR"] = 3;
	reg_lookup["FR"] = 4;
	reg_lookup["WR"] = 5;

	for(int r = 1; r < 506;++r){
		reg_lookup["r" + std::to_string(r)] = r + 5;
	}

}

Assembler::~Assembler(){}

/**
 * 
 */
void Assembler::Parse(const std::string &raw_program, std::vector<Expression> &parsed){
	//Split into lines
	std::vector<std::string> lines;
	boost::split(lines,raw_program,boost::is_any_of("\n"));

	parsed.reserve(lines.size());

	//Remove all empty lines
	lines.erase(std::remove_if(lines.begin(),lines.end(),[](const std::string& s){
				return s.empty();
				}),lines.end());


	for(const auto &expr_str: lines){

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
 *	@brief Go from a register's name to the 9 bit code for the data bits
 *	@return Will be a 9 bit value
 */
inline uint16_t Assembler::ParseRegister(std::string register_name){
	//Make sure it's a legit register
	validate_on_regex(register_name,register_name_regex,"Invalid register name");
	return reg_lookup[register_name];
}


inline uint16_t Assembler::ParseConstant9(std::string constant){
	return ParseConstant16(constant) & nine_bits;
}

inline uint16_t Assembler::ParseConstant16(std::string constant){
	uint16_t retval;
	std::stringstream ss;
	ss << std::hex << constant;
	ss >> retval;
	return retval;
}

inline uint16_t Assembler::ParseDirectWord(std::string constant){
	uint32_t retval;
	std::stringstream ss;
	ss << std::hex << constant;
	ss >> retval;
	return retval;
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

	//Go over each instruction
	for(const auto &expr : parsed){
		//Grab the template
		auto instr_template = instruction_ctrls.at(expr.op);

		Instruction final_instruction = 0;
		set_ctrl(final_instruction,instr_template);

		//This part is so ugly
		switch(instr_template){
			case add_ctrl:
			case sub_ctrl:
			case mul_ctrl:
			case div_ctrl:
			case and_ctrl:
			case or_ctrl:
				{
					auto arg0 = ParseRegister(expr.args.at(0));
					auto arg1 = ParseRegister(expr.args.at(1));
					auto arg2 = ParseRegister(expr.args.at(2));
					//args go from left to right
					set_arg(final_instruction,0,arg0);
					set_arg(final_instruction,1,arg1);
					set_arg(final_instruction,2,arg2);

					break;
				}

			case not_ctrl:
			case loa_ctrl:
			case sto_ctrl:
			case shr_ctrl:
			case shl_ctrl:
				{
					auto arg0 = ParseRegister(expr.args.at(0));
					auto arg1 = ParseRegister(expr.args.at(1));
					set_arg(final_instruction,0,arg0);
					set_arg(final_instruction,1,arg1);
					set_arg(final_instruction,2,0);

					break;
				}
			case beq_ctrl:
			case blt_ctrl:
				{
					auto arg0 = ParseRegister(expr.args.at(0));
					auto arg1 = ParseRegister(expr.args.at(1));
					auto arg_const = ParseConstant9(expr.args.at(2));
					set_arg(final_instruction,0,arg0);
					set_arg(final_instruction,1,arg1);
					set_arg(final_instruction,2,arg_const);

					break;
				}

			case ll_ctrl:
				{
					auto arg0 = ParseRegister(expr.args.at(0));
					auto arg_const = ParseConstant16(expr.args.at(1));
					set_arg(final_instruction,0,arg0);
					//this is a 16 bit constant. just drop it in, no mask.
					final_instruction |= arg_const;

					break;
				}

			case dw_ctrl:
				{
					final_instruction = ParseDirectWord(expr.args.at(0));
					break;
				}

			default:
				printf("Something's wrong. Invalid instruction template\n");
				break;
		}
		assembled.push_back(final_instruction);

	}
}

