/* 消除左递归，提取左因子，消除二义性后的产生式为
Program->Statement SEMICO Program|ε

Statement->OriginStatement|ScalseStatement|RotStatement|ForStatement|ColorStatement
OriginStatement->ORIGIN IS L_BRACKET Expression COMMA Expression R_BRACKET
ScaleStatement->SCALE IS L_BRACKET Expression COMMA Expression R_BRACKET
RotStatement->ROT IS Expression
ForStatement->FOR T FROM Expression TO Expression TO Expression STEP Expression DRAW L_BRACKET Expression COMMA Expression R_BRACKET
ColorStatement->COLOR IS BLUE|GREEN|RED|(Expression,Expression,Expression)
PixelSizeStatement->PIXELSIZE IS CONST_ID

Expression->Term{ (PLUS|MINUS) Term }
Term->Factor{ (MUL|DIV) Factor}
Factor->PLUS Factor|MINUS Factor|Component
Component->Atom POWER Component|Atom
Atom->CONST_ID | T | FUNC L_BRACKET Expression R_BRACKET | L_BRACKET Expression R_BRACKET

*/

#include "parser.h"
#include "semantic.h"

double Parameter = 0, Origin_x = 0, Origin_y = 0, Scale_x = 1, Scale_y = 1, Rot_angle = 0;//Parameter是参数T的存储空间：记录T每次加一点的变化
int Color_R = 250, Color_G = 250, Color_B = 250;
static Token token;//记号
static void FetchToken();//调用词法分析器的GetToken，把得到的当前记录保存起来。如果得到的记号是errtoken，则指出一个语法错误
static void MatchToken(enum Token_Type AToken);//匹配当前记号
static void SyntaxError(int case_of);//处理语法错误的子程序,打印所在行和对应字符并且终止程序运行。
static void ErrMsg(unsigned LineNo, char* descrip, char* string);  //打印错误信息
static void PrintSyntaxTree(struct ExprNode* root, int indent);  //前序遍历打印树

//非终结符递归子程序声明 有2类
//第1类语法分析，不构造语法树，因此语句的子程序均设计为过程->void类型的函数
static void Program();//递归下降分析
static void Statement();
static void OriginStatement();
static void RotStatement();
static void ScaleStatement();
static void ForStatement();
static void ColorStatement();

//第2类语法分析+构造语法树，因此表达式均设计为返回值为指向语法树节点的指针的函数。
static struct ExprNode* Expression();//处理加减
static struct ExprNode* Term();//引入Term提高MUL和DIV的优先级，处理乘除
static struct ExprNode* Factor();//处理正负
static struct ExprNode* Component();//处理幂
static struct ExprNode* Atom();//处理原子表达式

void call_match(char* x)//打印正在匹配的字符
{
    printf("matchtoken    "); printf(x); printf("\n");
}

void Tree_trace(ExprNode* x)//打印语法树
{
    PrintSyntaxTree(x, 1);
}


//外部接口与语法树构造函数声明
extern void Parser(char* SrcFilePtr);
static struct ExprNode* MakeExprNode(enum Token_Type opcode, ...);//生成语法树的一个节点

static void FetchToken()//调用词法分析器的GetToken，把得到的当前记录保存起来。
{
    token = GetToken();
    if (token.type == ERRTOKEN)
        SyntaxError(1); //如果得到的记号是errtoken，则提示是errtoken
}

//匹配当前的记号
static void MatchToken(enum Token_Type The_Token)
{
    if (token.type != The_Token)
        SyntaxError(2);//如果失败，则提示语法错误
    FetchToken();//若成功，则获取下一个
}

//语法错误处理
static void SyntaxError(int case_of)
{
    switch (case_of)
    {
    case 1: ErrMsg(LineNo, (char*)"ERRTOKEN:", token.lexeme);//需要将const char类型转换为char*类型
        break;
    case 2: ErrMsg(LineNo, (char*)"UNEXPECTED TOKEN:", token.lexeme);
        break;
    }
}

//打印错误信息
void ErrMsg(unsigned LineNo, char* descrip, char* string)
{
    printf("WARNING: LINE%5d:%s %s !\n", LineNo, descrip, string);
    CloseScanner();
    exit(1);
}

//递归先序遍历并打印表达式的语法树：根-->左-->右.其中，CONST_ID和T自己为叶子结点，FUNC有一个子结点，其他的有两个子结点
void PrintSyntaxTree(struct ExprNode* root, int indent)
{
    int temp;
    for (temp = 1; temp <= indent; temp++)//根据树的深度打印制表符
        printf("\t");  //缩进
    switch (root->OpCode)  //查询根结点Token_Type
    {//打印根节点
    case PLUS:  printf("%s\n", "+"); break;
    case MINUS: printf("%s\n", "-"); break;
    case MUL:   printf("%s\n", "*"); break;
    case DIV:   printf("%s\n", "/"); break;
    case POWER: printf("%s\n", "**"); break;
    case FUNC:  printf("%p\n", root->Content.CaseFunc.MathFuncPtr); break;//打印函数的函数地址
    case CONST_ID: printf("%f\n", root->Content.CaseConst); break;//打印常数值
    case T:     printf("%s\n", "T"); break;//打印变量T
    default:    printf("Error Tree Node!\n"); exit(0);
    }
    if (root->OpCode == CONST_ID || root->OpCode == T)  //叶子节点返回
       return;  //常数和参数只有叶子节点 常数：右值；参数：左值地址
    if (root->OpCode == FUNC)  //递归打印一个孩子节点
        PrintSyntaxTree(root->Content.CaseFunc.Child, indent + 1);  //函数有孩子节点和指向函数名的指针
    else  //递归打印两个孩子节点
    {  //二元运算：左右孩子的内部节点
        PrintSyntaxTree(root->Content.CaseOperater.Left, indent + 1);
        PrintSyntaxTree(root->Content.CaseOperater.Right, indent + 1);
    }
}

//绘图语言解释器入口（与主程序的外部接口）
void Parser(char* SrcFilePtr)   //语法分析器的入口
{
    printf("Enter in Parser\n");
    if (!InitScanner(SrcFilePtr))  //初始化词法分析器
    {
        printf("Failed to Open File!\n");
        return;
    }
    FetchToken();  //获取第一个记号
    Program();  //递归下降分析
    CloseScanner();  //关闭词法分析器
    printf("Exit from Parser\n");
    return;
}

//Program的递归子程序
//Program->Statement SEMICO Program|ε
static void Program()
{
    printf("Enter in Program\n");
    while (token.type != NONTOKEN)  //记号类型不是空,即文本没有结束
    {//循环处理绘图语句和分号
        Statement();  //转到每一种文法
        MatchToken(SEMICO);//匹配到分隔符
    }
    printf("Exit from Program\n");
}

//Statement的递归子程序，识别并转到每一种文法
static void Statement()
{
    printf("Enter in Statement\n");
    switch (token.type)
    {
    case ORIGIN:   OriginStatement(); break;
    case SCALE:   ScaleStatement(); break;
    case ROT:   RotStatement(); break;
    case FOR:   ForStatement(); break;
    case COLOR:  ColorStatement(); break;
    default:   SyntaxError(2);//不符合的就提示错误信息
    }
    printf("Exit from Statement");
}


//OriginStatement的递归子程序
//OriginStatement->ORIGIN IS L_BRACKET Expression COMMA Expression R_BRACKET
//eg:origin is (20, 200);
static void OriginStatement(void)
{
    struct ExprNode* tmp;  //语法树节点的类型
    printf("Enter in Original Statement\n");
    MatchToken(ORIGIN);
    MatchToken(IS);
    MatchToken(L_BRACKET);  
    tmp = Expression();
    Origin_x = GetExprValue(tmp);    //获取横坐标点平移距离
    DelExprTree(tmp);    //释放tmp所占空间
    MatchToken(COMMA);   
    tmp = Expression();
    Origin_y = GetExprValue(tmp);   //获取纵坐标的平移距离
    DelExprTree(tmp);   //释放tmp所占空间
    MatchToken(R_BRACKET);    
    printf("Exit from Origin Statement");
}


//ScaleStatement的递归子程序
//ScaleStatement->SCALE IS L_BRACKET Expression COMMA Expression R_BRACKET
//eg:scale is (40, 40);
static void ScaleStatement(void)
{
    struct ExprNode* tmp;
    printf("Enter in Scale Statement\n");
    MatchToken(SCALE);
    MatchToken(IS);
    MatchToken(L_BRACKET);     
    tmp = Expression();
    Scale_x = GetExprValue(tmp);   //获取横坐标的比例因子
    DelExprTree(tmp);//释放参数所占空间
    MatchToken(COMMA);   
    tmp = Expression();
    Scale_y = GetExprValue(tmp);//获取纵坐标的比例因子
    DelExprTree(tmp);//释放参数所占空间
    MatchToken(R_BRACKET);    
    printf("Exit from Scale Statement\n");
}

//RotStatement的递归子程序
//RotStatement->ROT IS Expression
//eg:rot is 0;
static void RotStatement(void)
{
    struct ExprNode* tmp;
    printf("Enter in Rot Statement\n");
    MatchToken(ROT);
    MatchToken(IS);     
    tmp = Expression();
    Rot_angle = GetExprValue(tmp);     //获取旋转角度
    DelExprTree(tmp);//释放参数旋转角度语法树所占空间
    printf("Exit from Rot Statement\n");
}

//ForStatement的递归子程序,对右部文法符号的展开->遇到终结符号直接匹配，遇到非终结符就调用相应子程序
//ForStatement->FOR T FROM Expression TO Expression TO Expression STEP Expression DRAW L_BRACKET Expression COMMA Expression R_BRACKET)
static void ForStatement()
{
    //eg:for T from 0 to 2*pi step pi/50 draw (t, -sin(t));
    double Start, End, Step;  //绘图起点、终点、步长
    struct ExprNode* start_ptr, * end_ptr, * step_ptr, * x_ptr, * y_ptr;  //指向各表达式语法树根节点
    printf("Enter in For Statement\n");
    //遇到非终结符就调用相应子程序
    MatchToken(FOR); call_match((char*)"FOR");
    MatchToken(T); call_match((char*)"T");
    MatchToken(FROM); call_match((char*)"FROM");  
    start_ptr = Expression();   //获得参数起点表达式的语法树
    Start = GetExprValue(start_ptr);   //计算参数起点表达式的值
    DelExprTree(start_ptr);   //释放参数起点语法树所占空间  
    MatchToken(TO); call_match((char*)"TO"); 
    end_ptr = Expression();  //构造参数终点表达式语法树
    End = GetExprValue(end_ptr);  //计算参数终点表达式的值 
    DelExprTree(end_ptr);   //释放参数终点语法树所占空间
    MatchToken(STEP); call_match((char*)"STEP");   
    step_ptr = Expression();   //构造参数步长表达式语法树
    Step = GetExprValue(step_ptr);   //计算参数步长表达式的值  
    DelExprTree(step_ptr);   //释放参数步长语法树所占空间
    MatchToken(DRAW);
    call_match((char*)"DRAW");
    MatchToken(L_BRACKET);
    call_match((char*)"("); 
    x_ptr = Expression();     
    MatchToken(COMMA);
    call_match((char*)",");   
    y_ptr = Expression();   
    MatchToken(R_BRACKET);
    call_match((char*)")");
    DrawLoop(Start, End, Step, x_ptr, y_ptr);       //绘制图形
    DelExprTree(x_ptr);                             //释放横坐标语法树所占空间
    DelExprTree(y_ptr);                             //释放纵坐标语法树所占空间
    printf("Exit from For Statement\n");
}

//COLOR的子程序，遇到BLUE,GREEN,RED修改RGB(r,g,b)的值
//ColorStatement->COLOR IS BLUE|GREEN|RED|(Expression,Expression,Expression)
static void ColorStatement()
{
    Token_Type token_tmp;
    struct ExprNode* value_ptr;
    MatchToken(COLOR); call_match((char*)"COLOR");
    MatchToken(IS); call_match((char*)"IS");
    switch (token.type)
    {//颜色分为两类
     //第一类直接指定颜色（红，绿，蓝）
    case red:Color_R = 255; Color_B = 0; Color_G = 0; MatchToken(token.type);call_match((char*)"RED");break;
    case green:Color_R = 0; Color_B = 0; Color_G = 255;MatchToken(token.type);call_match((char*)"GREEN");break;
    case blue:Color_R = 0; Color_B = 255; Color_G = 0; MatchToken(token.type);call_match((char*)"BLUE");break;
    //第二类通过RGB（r,g,b）定义颜色
    case L_BRACKET:
        MatchToken(L_BRACKET);
        value_ptr = Expression();
        Color_R = GetExprValue(value_ptr);
        MatchToken(COMMA);
        value_ptr = Expression();
        Color_G = GetExprValue(value_ptr);
        MatchToken(COMMA);
        value_ptr = Expression();
        Color_B = GetExprValue(value_ptr);
        MatchToken(R_BRACKET);
        printf(" (%d,%d,%d)\n",Color_R,Color_G,Color_B);
        break;
    default: break;
    }
    printf("Exit from Color Statement\n");
}

//Expression的递归子程序,表达式应该是由正负号或无符号开头、由若干个项以加减号连接而成。
//把函数设计为语法树节点的指针，在函数内引进2个语法树节点的指针变量，分别作为Expression左右操作数（Term）的语法树节点指针
//Expression->Term{ (PLUS|MINUS) Term }
static struct ExprNode* Expression()    //展开右部，并且构造语法树
{
    struct ExprNode* left, * right;      //左右子树节点的指针
    Token_Type token_tmp;     //当前记号种类
    printf("Enter in Expression\n");
    left = Term();     //分析左操作数且得到其语法树
    while (token.type == PLUS || token.type == MINUS)
    {
        token_tmp = token.type;
        MatchToken(token_tmp);
        right = Term();      //分析右操作数且得到其语法树
        left = MakeExprNode(token_tmp, left, right);    //构造运算的语法树，结果为左子树
        Tree_trace(left);   //打印表达式的语法树
    }
    printf("Exit from Expression\n");
    return left;       //返回最终表达式的语法树
}

//Term的递归子程序，项是由若干个因子以乘除号连接而成
//Term->Factor{ (MUL|DIV) Factor }
static struct ExprNode* Term()
{
    struct ExprNode* left, * right;
    Token_Type token_tmp;
    left = Factor();
    while(token.type == MUL || token.type == DIV)
    {
        token_tmp = token.type;
        MatchToken(token_tmp);
        right = Factor();
        left = MakeExprNode(token_tmp, left, right);
        //Tree_trace(left);测试左子树，应该只缺少根结点
    }
    return left;//返回左子树
}

//Factor的递归子程序，因子则可能是一个标识符或一个数字，或是一个以括号括起来的子表达式
//Factor->PLUS Factor|MINUS Factor|Component
static struct ExprNode* Factor()
{
    struct ExprNode* left, * right;
    if (token.type == PLUS)           //匹配一元加运算
    {
        MatchToken(PLUS);
        right = Factor();             //表达式退化为仅有右操作数的表达式
    }
    else if (token.type == MINUS)
    {
        MatchToken(MINUS);//负数开头处理为0.0-正数
        right = Factor();
        left = new ExprNode;
        left->OpCode = CONST_ID;
        left->Content.CaseConst = 0.0;
        right = MakeExprNode(MINUS, left, right);//由Factor产生式知Factor的right不为空，所以通过right生成结点
    }
    else
        right = Component();         //匹配非终结符Component
    return right;
}


//Component->Atom POWER Component|Atom
//Component的递归子程序
static struct ExprNode* Component()//右结合
{
    struct ExprNode* left, * right;
    left = Atom();
    if (token.type == POWER)
    {
        MatchToken(POWER);
        right = Component();         //递归调用Component以实现POWER的右结合
        left = MakeExprNode(POWER, left, right);//这里只能通过left生成Component结点，因为无论是right或者tmp都有可能为空
    }
    return left;
}


//Atom->CONST_ID | T | FUNC L_BRACKET Expression R_BRACKET | L_BRACKET Expression R_BRACKET
//Atom的递归子程序包括分隔符 函数 常数 参数
static struct ExprNode* Atom()
{
    struct Token t = token;
    struct ExprNode* address = nullptr, * tmp;
    switch (token.type)
    {
    case CONST_ID:
        MatchToken(CONST_ID);
        address = MakeExprNode(CONST_ID, t.value);//常数，则结点应储存其右值
        break;
    case T:
        MatchToken(T);
        address = MakeExprNode(T);//参数，则结点应读取Parameter的地址作为参数T的地址
        break;
    case FUNC:
        MatchToken(FUNC);
        MatchToken(L_BRACKET);//sin，cos，tan，ln，exp，sqrt函数名后有左括号
        tmp = Expression();//处理函数表达式
        address = MakeExprNode(FUNC, t.FuncPtr, tmp);//利用获取的tmp为该函数生成结点
        MatchToken(R_BRACKET);//最后还有右括号
        break;
    case L_BRACKET:
        MatchToken(L_BRACKET);
        address = Expression();//处理算术表达式
        MatchToken(R_BRACKET);
        break;
    default:
        SyntaxError(2);
    }
    return address;
}

//生成语法树的一个节点
static struct ExprNode* MakeExprNode(enum Token_Type opcode, ...)
{
    struct ExprNode* ExprPtr = new(struct ExprNode);//申请一个新的结点
    ExprPtr->OpCode = opcode;       //接收记号的类别
    va_list ArgPtr;    //声明一个转换参数的变量
    va_start(ArgPtr, opcode);   //初始化变量，ArgPtr指向最后的参数opcode
    switch (opcode)     //根据记号的类别构造不同的节点
    {
    case CONST_ID:     //常数
        ExprPtr->Content.CaseConst = (double)va_arg(ArgPtr, double);  //va_arg返回右值
        break;
    case T:   //参数
        ExprPtr->Content.CaseParmPtr = &Parameter;  //va_arg返回参数地址
        break;
    case FUNC:  //函数调用
        ExprPtr->Content.CaseFunc.MathFuncPtr = (FuncPtr)va_arg(ArgPtr, FuncPtr);  //返回对应函数名的指针 MathFuncPtr
        ExprPtr->Content.CaseFunc.Child = (struct ExprNode*)va_arg(ArgPtr, struct ExprNode*);  //孩子的内部节点
        break;
    default:  //二元运算
        ExprPtr->Content.CaseOperater.Left = (struct ExprNode*)va_arg(ArgPtr, struct ExprNode*);
        ExprPtr->Content.CaseOperater.Right = (struct ExprNode*)va_arg(ArgPtr, struct ExprNode*);
        break;
    }
    va_end(ArgPtr);  //结束，回收ArgPtr,ArgPtr=NULL
    return ExprPtr;
}
