/*
 * m4_core.cpp
 *
 *  Created on: 2017Äê6ÔÂ14ÈÕ
 *      Author: kohil
 */
#include "../inc/m4_core.hpp"


namespace M4_core_thumb16{
static uint32_t mask_table(uint32_t i){
	return (1<<i) -1;
}
static uint32_t BITS(uint16_t d,uint32_t startbit,uint32_t stopbit){
	AbortAssert(startbit <=16,"");
	AbortAssert(stopbit <=16,"");
	return (d & (mask_table(stopbit-startbit+1) << startbit )) >> startbit;
}

/*
 * * lsr(immediate)
 * 	lsl r0,r1,#2 -> 0x0088 -> 0b0 00010 001 000
 * 	                              #2  r1 r0
 * 	It can optionally update the condition flags based on the result.
 * 	0 0 0 0 0 imm5 Rm Rd
 * 	* lsr(register)
 * 	Logical Shift Right (register) shifts a register value right by a variable number of bits,
 * 	shifting in zeros, and writes the result to the destination register.
 * 	 The variable number of bits is read from the bottom byte of a register.
 * 	 It can optionally update the condition flags based on the result.
 *
 */
uint32_t lsl(CORE_REGS &core_regs, uint16_t instruction) {
	if(LSL_I_CON(instruction)){
		uint16_t imm5 = (instruction & (0b11111 << 6)) >> 6;
		uint32_t rm = (instruction & (0b111 << 3)) >> 3;
		uint32_t rd = (instruction & (0b111 << 0)) >> 0;
		uint32_t fromData = core_regs[rm];
		uint32_t result = fromData << imm5;
		core_regs[rd] = result;
		core_regs.updateCpsrByResult(result);
		((1 << (32 - imm5)) & fromData) ? core_regs.updateCpsrCarryBit(true) :
				core_regs.updateCpsrCarryBit(false);
		return result;
	}//lsr(immediate)
	else if(LSL_R_CON(instruction)){
		uint32_t rm = (instruction & (0b111 << 3)) >> 3;
		uint32_t rd = (instruction & (0b111 << 0)) >> 0;
		uint32_t n = core_regs[rm] & 0xff;
		uint32_t fromData = core_regs[rm];
		uint32_t result = fromData << n;
		core_regs[rd] = result;
		core_regs.updateCpsrByResult(result);
		((1 << (32 - n)) & fromData) ? core_regs.updateCpsrCarryBit(true) :
					core_regs.updateCpsrCarryBit(false);

		return result;
	}
	else{
		AbortAssert(1,Int2Hex(instruction));
		return 0;
	}
}

};



