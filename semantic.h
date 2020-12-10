#ifndef SEMANTIC_H
#define SEMANTIC_H

#include <graphics.h>
#include <conio.h>
#include "parser.h"

//----------外部函数声明
extern double GetExprValue(struct ExprNode* root);//根据结点获得表达式的值
extern void DrawLoop(double Start, double End, double Step, struct ExprNode* HorPtr, struct ExprNode* VerPtr);//图形绘制
extern void DelExprTree(struct ExprNode* root);//删除一棵树

#endif // !SEMANTIC_H