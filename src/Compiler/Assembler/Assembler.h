#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <streambuf>
#include "utils.h"
/***
* @Author Joseph Cutler
* @Date May 20, 2016
* @Copyright WTFPL
*/

typedef uint32_t Instruction;
typedef struct Expr{
	std::string op;
	std::vector<std::string> args;
} Expr;
constexpr uint32_t ctrl_mask = 0xF8000000;
constexpr uint32_t data_mask = 0x7FFFFFF;

class Assembler{
public:
	Assembler();
	~Assembler();
	void Assemble(const std::string &fname, std::vector<uint32_t> &assembled);

private:
	std::map<std::string,uint8_t> instruction_ctrls;
	void Parse(const std::string &raw_program, std::vector<Expr> &parsed);

protected:
	const uint8_t add_ctrl = 0b00000;
	const uint8_t sub_ctrl = 0b00001;
	const uint8_t mul_ctrl = 0b00010;
	const uint8_t div_ctrl = 0b00011;
	const uint8_t and_ctrl = 0b00100;
	const uint8_t or_ctrl  = 0b00101;
	const uint8_t not_ctrl = 0b00110;
	const uint8_t loa_ctrl = 0b00111;
	const uint8_t sto_ctrl = 0b01000;
	const uint8_t shr_ctrl = 0b01001;
	const uint8_t shl_ctrl = 0b01010;
	const uint8_t beq_ctrl = 0b01011;
	const uint8_t blt_ctrl = 0b01100;
	const uint8_t ll_ctrl  = 0b01101;
};
