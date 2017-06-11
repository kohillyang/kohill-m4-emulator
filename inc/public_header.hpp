/*
 * public_header.h
 *
 *  Created on: 2017?6?11?
 *      Author: kohil
 */

#ifndef INC_PUBLIC_HEADER_HPP_
#define INC_PUBLIC_HEADER_HPP_
#include <stdint.h>
#include <iostream>
#include <QtCore>
#include <qdebug.h>
#include <string>
#include <QException>
using namespace std;
#define UNUSE(x) ((void)x)
ostream& operator << (ostream& fin,QString s){
	fin << s.toStdString();
	return fin;
}
template<typename T>
inline QString Int2Hex(T d){
	return QString("0x%1").arg(d,0,16);
}
class Object : public QObject{

}

#endif /* INC_PUBLIC_HEADER_HPP_ */
