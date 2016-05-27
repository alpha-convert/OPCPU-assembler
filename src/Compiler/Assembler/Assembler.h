#pragma once
#include <string>
#include <regex>
#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <streambuf>
#include <algorithm>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "utils.h"
/***
* @Author Joseph Cutler
* @Date May 20, 2016
* @Copyright WTFPL
*/

typedef uint32_t Instruction;
typedef struct Expression{
	std::string op;
	std::vector<std::string> args;
} Expression;
constexpr uint32_t ctrl_mask = 0xF8000000;
constexpr uint32_t data_mask = 0x7FFFFFF;
constexpr uint16_t nine_bits = 0x1FF;

class Assembler{
public:
	Assembler();
	~Assembler();
	void Assemble(const std::string &fname, std::vector<uint32_t> &assembled);

private:
	std::map<std::string,uint8_t> instruction_ctrls;
	void Parse(const std::string &raw_program, std::vector<Expression> &parsed);
	inline uint16_t ParseRegister(std::string register_name);
	inline uint16_t ParseConstant9(std::string constant);
	inline uint16_t ParseConstant16(std::string constant);
	inline uint16_t ParseDirectWord(std::string constant);

protected:
	static constexpr const uint8_t add_ctrl = 0b00000;
	static constexpr const uint8_t sub_ctrl = 0b00001;
	static constexpr const uint8_t mul_ctrl = 0b00010;
	static constexpr const uint8_t div_ctrl = 0b00011;
	static constexpr const uint8_t and_ctrl = 0b00100;
	static constexpr const uint8_t or_ctrl  = 0b00101;
	static constexpr const uint8_t not_ctrl = 0b00110;
	static constexpr const uint8_t loa_ctrl = 0b00111;
	static constexpr const uint8_t sto_ctrl = 0b01000;
	static constexpr const uint8_t shr_ctrl = 0b01001;
	static constexpr const uint8_t shl_ctrl = 0b01010;
	static constexpr const uint8_t beq_ctrl = 0b01011;
	static constexpr const uint8_t blt_ctrl = 0b01100;
	static constexpr const uint8_t ll_ctrl  = 0b01101;
	static constexpr const uint8_t dw_ctrl  = 0b01110;

	static std::map<std::string,uint16_t> reg_lookup;

};

