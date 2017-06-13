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
 * 	lsl r0,r1,#2 -> 0x0088 -> 0b0 00010 001 000
 * 	                              #2  r1 r0
 * 	It can optionally update the condition flags based on the result.
 * 	0 0 0 0 0 imm5 Rm Rd
 */
uint32_t lsr(CORE_REGS &core_regs, uint16_t instruction) {
	AbortAssert((instruction & (0b11111 << 11)) == 0, "Expect 0b00000.");
	uint16_t imm5 = (instruction & (0b11111 << 6)) >> 6;
	uint32_t rm = (instruction & (0b111 << 3)) >> 3;
	uint32_t rd = (instruction & (0b111 << 0)) >> 0;
	WarnAssert(rd != 13 && rd != 15 && rm != 13 && rm != 13,
			"if d IN {13,15} || m IN {13,15} then UNPREDICTABLE;");
	uint32_t result = core_regs[rm] << imm5;
	core_regs[rd] = result;

	core_regs.updateCpsrByResult(result);
	return result;
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
		lsr(core_regs, 0x0088);
		std::cout << "lsr lsl r0,r1,#2 " << (core_regs[0] == 0x0c ? "pass":"not pass") << std::endl;
	}
};

}
;

