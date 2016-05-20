#include "Assembler.h"
inline uint32_t ctrl_of(Instruction i){
	return (i & ctrl_mask) >> 27;
}

inline uint32_t data_of(Instruction i){
	return i & data_mask;
}

/**
 *	ctrl must be a 5 bit number (one of the 
 */
inline void set_ctrl(Instruction &i, uint8_t ctrl){
	i |= (ctrl & 0b00011111)  << 27; //make sure the upper 3 bits of the uint8 aren't set
}

inline void set_data(Instruction &i, uint32_t data){
	i |= (data & (0b00000 << 27)); //make sure the upper 5 bits are clear as not to change the ctrl
}


Assembler::Assembler(std::string p){
	{instruction_ctrls.insert({"add",add_ctrl});
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
	instruction_ctrls.insert({"ll" , ll_ctrl});}

	program = p;
}

