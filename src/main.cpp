/*
 * main.cpp
 *
 *  Created on: 2017?6?11?
 *      Author: kohil
 */

#include "m4_core.hpp"
int main(int argc,char *argv[]){
	UNUSE(argc);
	UNUSE(argv);
	//H://workspace_py/test/Debug/
	M4_Core x("test.hex");
	cout << x.toString() << endl;
}



