#define _CRT_SECURE_NO_WARNINGS    //忽略安全警告
#pragma once
#ifndef MSCANNER_H
#define MSCANNER_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <math.h>
enum Token_Type  //枚举记号的类别
{
	ORIGIN, SCALE, ROT, IS, TO, STEP, DRAW, FOR, FROM,   //保留字
	T,  //参数
	SEMICO, L_BRACKET, R_BRACKET, COMMA,  //分隔符
	PLUS, MINUS, MUL, DIV, POWER,         //运算符
	FUNC,      //函数
	CONST_ID,  //常数
	NONTOKEN,  //空记号
	ERRTOKEN, //出错记号
	COLOR, blue, green, red,
	size
};

typedef double(*MathFuncPtr) (double);

struct Token  //记号的数据结构,记号由类别和属性组成
{
	Token_Type type;         //记号的类别
	char* lexeme;            //属性，原始输入的字符串，是要指向TokenBuffer的指针
	double value;            //属性，为常数的值
	double(*FuncPtr)(double);//属性，函数的指针
};

static Token TokenTab[] =//符号表（字典）：数组元素的类型于记号的类型相同
{//当识别出一个ID时，通过此表来确认具体是哪个记号
{ CONST_ID,    (char*)"PI",         3.1415926,  NULL },
{ CONST_ID,    (char*)"E",          2.71828,    NULL },
{ T,           (char*)"T",          0.0,        NULL },
{ FUNC,        (char*)"SIN",        0.0,        sin  },
{ FUNC,        (char*)"COS",        0.0,        cos  },
{ FUNC,        (char*)"TAN",        0.0,        tan  },
{ FUNC,        (char*)"LN",        0.0,         log  },
{ FUNC,        (char*)"EXP",        0.0,        exp  },
{ FUNC,        (char*)"SQRT",      0.0,        sqrt },
{ ORIGIN,      (char*)"ORIGIN",    0.0,        NULL },
{ SCALE,       (char*)"SCALE",     0.0,        NULL },
{ ROT,         (char*)"ROT",       0.0,        NULL },
{ IS,          (char*)"IS",         0.0,        NULL },
{ FOR,         (char*)"FOR",        0.0,        NULL },
{ FROM,        (char*)"FROM",      0.0,        NULL },
{ TO,          (char*)"TO",        0.0,        NULL },
{ STEP,        (char*)"STEP",       0.0,        NULL },
{ DRAW,        (char*)"DRAW",      0.0,        NULL },
{ COLOR ,      (char*)"COLOR",     0.0,         NULL},
{ blue,        (char*)"BLUE",       0.0,       NULL },
{ green,       (char*)"GREEN",     0.0,         NULL },
{ red,         (char*)"RED",        0.0,        NULL },
{ size,        (char*)"PIXELSIZE",  0.0,       NULL }
};

extern char* GetEnum(int i);
extern unsigned int LineNo;              //跟踪记号所在源文件行号
extern int InitScanner(const char*);     //初始化词法分析器
extern Token GetToken(void);             //获取记号函数
extern void CloseScanner(void);
#endif