/*
 * m4_core.cpp
 *
 *  Created on: 2017?6?11?
 *      Author: kohil
 */
#include "public_header.hpp"
#include "IntelHexAdapter.hpp"
namespace M4_core_thumb16 {
class CORE_REGS : Object{
private:
	uint32_t r[16];
	uint32_t cpsr;
public:
	QString toString(){
		QString rstr = "";
		for (int i = 0; i < 16; i++) {
			rstr = rstr +QString("r%1:").arg(i,0,10)
					+ Int2Hex(r[i]) + " "
					;
//			cout <<r[i] <<endl;
		}
		return rstr;
	}
	CORE_REGS(){
		memset(r, 0, sizeof(r));
		cpsr = 0;
	}
	uint32_t& operator [](uint32_t index) {
		AbortAssert(index < 16, QString("index out of bound.%0").arg(index));
		return r[index];
	}
	uint32_t& operator [](int index) {
		AbortAssert(index < 16, QString("index out of bound.%0").arg(index));
		return r[index];
	}
	void updateCpsrByResult(uint32_t r) {
		cpsr |= (0b1 << 31) & r; //N
		r ? (cpsr |= (1 << 30) & r) : (cpsr &= ~(1 << 30) & r); //Z
		//C And N flags are only for Add and subtract operations.
	}
};
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
uint32_t getBits(uint32_t d,uint32_t mask,uint32_t startbit){
	return (d & (mask << startbit )) >> startbit;
}
uint32_t lsl(CORE_REGS &core_regs, uint16_t instruction) {
	if((instruction & (0b11111 << 11)) == 0){
		uint16_t imm5 = (instruction & (0b11111 << 6)) >> 6;
		uint32_t rm = (instruction & (0b111 << 3)) >> 3;
		uint32_t rd = (instruction & (0b111 << 0)) >> 0;
		uint32_t result = core_regs[rm] << imm5;
		core_regs[rd] = result;
		core_regs.updateCpsrByResult(result);
		return result;
	}//lsr(immediate)
	else if(getBits(instruction, 0b1111111111, 6) == 0b0100000010){
		uint32_t rm = (instruction & (0b111 << 3)) >> 3;
		uint32_t rd = (instruction & (0b111 << 0)) >> 0;
		uint32_t n = core_regs[rm] & 0xff;
		uint32_t result = core_regs[rm] << n;
		core_regs[rd] = result;
		core_regs.updateCpsrByResult(result);
		return result;
	}
	else{
		AbortAssert(1,Int2Hex(instruction));
		return 0;
	}
}


class M4_Core: Object {
public:
	M4_Core(QString filename) :
			intel_hex_adapter(filename),core_regs() {
		unitTest();
	}
private:
	Intel_HEX_Adapter intel_hex_adapter;
	CORE_REGS core_regs;
	void unitTest() {
		//for lsr lsl r0,r1,#2
		core_regs[0] = 2;
		core_regs[1] = 3;
		lsl(core_regs, 0x0088);
		std::cout << "lsr lsl r0,r1,#2 " << (core_regs[0] == 0x0c ? "pass":"not pass") << std::endl;
	}
};

}
;

