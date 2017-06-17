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
		core_regs.updateCpsrNZByResult(result);
		((1 << (32 - imm5)) & fromData) ? core_regs.updateCpsrCarryBit(true) :
				core_regs.updateCpsrCarryBit(false);
		return result;
	}//lsr(immediate)
	else if(LSL_R_CON(instruction)){
		uint32_t rm = (instruction & (0b111 << 3)) >> 3;
		uint32_t rd = (instruction & (0b111 << 0)) >> 0;
		uint32_t n = core_regs[rm] & 0xff;
		uint32_t fromData = core_regs[rd];
		uint32_t result = fromData << n;
		core_regs[rd] = result;
		core_regs.updateCpsrNZByResult(result);
		((1 << (32 - n)) & fromData) ? core_regs.updateCpsrCarryBit(true) :
					core_regs.updateCpsrCarryBit(false);

		return result;
	}
	else{
		AbortAssert(1,Int2Hex(instruction));
		return 0;
	}
}
uint32_t lsr(CORE_REGS &core_regs, uint16_t instruction) {

	if(LSR_I_CON(instruction)){
		uint16_t n = BITS(instruction,6,10);//rm max:31
		uint16_t rm = BITS(instruction,3,5);
		uint16_t rd = BITS(instruction,0,2);
		uint32_t fromData = core_regs[rm];
		uint32_t result = fromData >> n;
		core_regs[rd] = result;
		core_regs.updateCpsrNZByResult(result);
		((1 << n) & (fromData << 1)) ? core_regs.updateCpsrCarryBit(true) :
				core_regs.updateCpsrCarryBit(false);
		//it's OK when the highest bit of fromData is 1.
		return result;
	}else if(LSR_R_CON(instruction)){
		uint16_t rm = BITS(instruction,3,5);
		uint16_t rd = BITS(instruction,0,2);
		uint16_t n = core_regs[rm] & 0xff;
		uint32_t fromData = core_regs[rd];
		uint32_t result = fromData >> n;
		core_regs[rd] = result;
		core_regs.updateCpsrNZByResult(result);
		((1 << n) & (fromData << 1)) ? core_regs.updateCpsrCarryBit(true) :
				core_regs.updateCpsrCarryBit(false);
		//it's OK when the highest bit of fromData is 1.
		return result;
	}else{
		AbortAssert(1,Int2Hex(instruction));
		return 0;
	}
}


/*
 * Add with Carry (immediate)
 * adds an immediate value and the carry flag value to a register value,
 * and writes the result to the destination register.
 * It can optionally update the condition flags based on the result.
 * Note: ARM_Cortex M4 does not support adc(immediate)
 *
 * Add with Carry (register) adds a register value,
 * the carry flag value, and an optionally-shifted register value,
 * and writes the result to the destination register.
 * It can optionally update the condition flags based on the result.
 * Example: adc r3,r1,r3 (r1+r3+carry)->r3
 * 			adc r1,r1,r3 (r1+r3+carry)->r1
 * 	Note:dest must overlap one source register.
 * cpsr: N,Z,C,V
 *
 * carry is unsigned overflow,and V is signed overflow.
 * For example, for a 8bit 8 MCU,
 * the signed intergar range is -128-127
 *	so it'll cause overflow that 127+127 or -128-128
 */
uint32_t adc(CORE_REGS &core_regs, uint16_t instruction){
	if(ADC_R_CON(instruction)){
		uint16_t rm = BITS(instruction,3,5);
		uint16_t rdn = BITS(instruction,0,2);
		uint32_t op1 = core_regs[rm];
		uint32_t op2 = core_regs[rdn];
		uint64_t result = (uint64_t )op1 + (uint64_t )op2;
		uint32_t result32 = (uint32_t)result;
		core_regs[rdn] = result32;
		core_regs.updateCpsrNZByResult(result32);
		core_regs.updateCpsrCarryBit(result & ((uint64_t)1<<32));

		int64_t signed_result= int64_t((int32_t)op1) + int64_t((int32_t)op2);

		core_regs.updateCpsrOverflowBit(
				  (signed_result > (int64_t)(int32_t)(0x7FFFFFFF)) ||
				  (signed_result < (int64_t)(int32_t)(0x80000000))
				  );
		core_regs.updateCpsrNZByResult(result32);
		return result32;
	}else{
		AbortAssert(1,Int2Hex(instruction));
		return 0;
	}
}
};



