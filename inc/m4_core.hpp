/*
 * m4_core.cpp
 *
 *  Created on: 2017?6?11?
 *      Author: kohil
 */
#include "public_header.hpp"
#include "IntelHexAdapter.hpp"
namespace M4_core_thumb16_instrctions{

	void updateCpsrByResult(uint32_t r,uint32_t &CPSR){
		CPSR |= (0b1 << 31) & r; //N
		r ? (CPSR |= (1 << 30) & r) :(CPSR &= ~(1 << 30) & r); //Z
		//C And N flags are only for Add and subtract operations.
	}
	/*
	 * 	lsl r0,r1,#2
	 * 	It can optionally update the condition flags based on the result.
	 * 	0 0 0 0 0 imm5 Rm Rd
	 */
	uint32_t lsr(uint32_t *r,uint16_t instruction,uint32_t &CPSR){
		LogAssert((instruction & (0b11111 << 11)) == 0,"Expect 0b00000.");
		uint16_t imm5 = instruction & (0b11111 << 6) >> 6;
		uint32_t rm = instruction & (0b111 <<3) >> 3;
		uint32_t rd = instruction & (0b111 <<0) >> 0;
		uint32_t result = r[rm] << imm5;
		r[rd] = result;
		updateCpsrByResult(result, CPSR);
		return r[rd];
	}
};


class M4_Core : Object{
public:
	M4_Core(QString filename) :intel_hex_adapter(filename){

	}
	const QString toString(){
		return intel_hex_adapter.toString();
	}
private:
	Intel_HEX_Adapter intel_hex_adapter;
	union{
		uint32_t r[16];
		uint32_t cpsr;
	}REGS;
};



