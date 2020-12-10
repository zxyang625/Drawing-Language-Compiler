#ifndef PARSER_H
#define PARSER_H
#include "scanner.h"

typedef double(*FuncPtr)(double);
struct ExprNode        //语法树节点类型
{
	enum Token_Type OpCode;  //scanner中的操作
	union//共用体，每一结点只能是下面的四种之一
	{
		struct { 
			ExprNode* Left, * Right;
		}CaseOperater;//二元运算：有左右孩子的内部节点
		struct { 
			ExprNode* Child; 
			FuncPtr MathFuncPtr;
		}CaseFunc;//函数调用：只有一个孩子的内部节点，还有一个指向对应函数名的指针 MathFuncPtr

		double CaseConst;  //常数：叶子节点  右值
		double* CaseParmPtr;  //参数T   左值：存放T值的地址
	}Content;
};
extern void Parser(char* FILE);   //调用语法分析器

#endif // !PARSER_H