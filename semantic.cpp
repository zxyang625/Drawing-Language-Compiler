#include "semantic.h"
extern  double Parameter,Origin_x, Origin_y,Scale_x, Scale_y,Rot_angle;

double GetExprValue(struct ExprNode* root);//获得表达式的值
void DrawLoop(double Start, double End, double Step, struct ExprNode* HorPtr, struct ExprNode* VerPtr);//图形绘制
void DelExprTree(struct ExprNode* root);//删除一棵树
static void CalcCoord(struct ExprNode* Hor_Exp, struct ExprNode* Ver_Exp, double& Hor_x, double& Ver_y);//计算点的坐标

//计算被绘制点的坐标
static void CalcCoord(struct ExprNode* Hor_Exp,  //横坐标表达式语法树的根节点
	struct ExprNode* Ver_Exp,  //纵坐标表达式语法树的根节点
	double& Hor_x,  //点横坐标值，起返回值的作用
	double& Ver_y)  //点纵坐标值，起返回值的作用
{
	double HorCord, VerCord, Hor_tmp;
	HorCord = GetExprValue(Hor_Exp);
	VerCord = GetExprValue(Ver_Exp);  //根据表达式的语法树计算原始坐标
	HorCord *= Scale_x;
	VerCord *= Scale_y;   //先进行比例变换
	Hor_tmp = HorCord * cos(Rot_angle) + VerCord * sin(Rot_angle);//需要引入中间变量Hor_tmp，否则HorCord的值会提前改变
	VerCord = VerCord * cos(Rot_angle) - HorCord * sin(Rot_angle);
	HorCord = Hor_tmp;    //再进行旋转变换
	HorCord += Origin_x;
	VerCord += Origin_y;   //最后进行平移变换
	Hor_x = HorCord;
	Ver_y = VerCord;    //将新的x，y值赋给Hor_x,Ver_y
}

//循环绘制点的坐标
void DrawLoop(double Start,   //起始
	double End,               //终止
	double Step,              //步长
	struct ExprNode* HorPtr,  //横坐标表达式语法树的根节点
	struct ExprNode* VerPtr)  //纵坐标表达式语法树的根节点
{
	double x, y;
	for (Parameter = Start; Parameter <= End; Parameter += Step)//把T在范围内的每一个值带入计算
	{
		CalcCoord(HorPtr, VerPtr, x, y);  //计算要绘制点的实际坐标
		putpixel((int)x, (int)y, RGB(250, 250, 250));  //绘制这个点
	}
}

//计算表达式的值
double  GetExprValue(struct ExprNode* root)  //参数是表达式的根
{  //后续遍历语法树  根据不同的节点类型计算当前根节点的值
	if (root == NULL)
		return 0.0;
	switch (root->OpCode)
	{
		//二元运算符
	case PLUS:
		return GetExprValue(root->Content.CaseOperater.Left) + GetExprValue(root->Content.CaseOperater.Right);
	case MINUS:
		return GetExprValue(root->Content.CaseOperater.Left) - GetExprValue(root->Content.CaseOperater.Right);
	case MUL:
		return GetExprValue(root->Content.CaseOperater.Left) * GetExprValue(root->Content.CaseOperater.Right);
	case DIV:
		return GetExprValue(root->Content.CaseOperater.Left) / GetExprValue(root->Content.CaseOperater.Right);
	case POWER:
		return pow(GetExprValue(root->Content.CaseOperater.Left), GetExprValue(root->Content.CaseOperater.Right));
	case FUNC: return (*root->Content.CaseFunc.MathFuncPtr)(GetExprValue(root->Content.CaseFunc.Child));//函数调用，函数只有一个结点Child
	case CONST_ID: return root->Content.CaseConst;//常数
	case T: return *(root->Content.CaseParmPtr);//参数的值
	default: return 0.0;
	}  //返回值是表达式的值
}

//递归删除一颗语法树
void DelExprTree(struct ExprNode* root)
{
	if (root == NULL)//遇到根结点代表子树删除结束
		return;
	switch (root->OpCode)
	{
	case PLUS:       //二元：两个孩子的内部节点
	case MINUS:
	case MUL:
	case DIV:
	case POWER:
		DelExprTree(root->Content.CaseOperater.Left);
		DelExprTree(root->Content.CaseOperater.Right);
		break;
	case FUNC:       //一元：：一个孩子的内部节点
		DelExprTree(root->Content.CaseFunc.Child);
		break;
	default:  break;       //叶子节点
	}
	delete(root);          //删除根节点
}