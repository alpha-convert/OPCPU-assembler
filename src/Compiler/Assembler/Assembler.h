#pragma once
#include <string>
#include <vector>
#include <map>
/***
* @Author Joseph Cutler
* @Date May 20, 2016
* @Copyright WTFPL
*/

typedef uint32_t Instruction;
constexpr uint32_t ctrl_mask = 0xF8000000;
constexpr uint32_t data_mask = 0x7FFFFFF;

class Assembler{
public:
	Assembler(std::string p);
	~Assembler();
private:
	std::string program;
	std::map<std::string,uint8_t> instruction_ctrls;
	std::vector<uint32_t> final_bytecode;

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
