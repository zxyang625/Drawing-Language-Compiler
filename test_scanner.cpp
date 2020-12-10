
/*
#include"scanner.h"//测试词法分析器的正确性
using namespace std;
void main()
{
    Token token;
    char file[] = "draw2.txt";
    if (!InitScanner(file))                                    //初始化词法分析器
    {
        printf("Open Sorce File Error !\n");
        return;
    }
    printf("记号类别         字符串        常数值           函数指针\n");
    printf("--------------------------------------------\n");
    while (true)
    {
        token = GetToken();//输出一个记号
        if (token.type != NONTOKEN)//记号的类别不是错误，就打印出他的内容
            printf("%12s,%12s,%12f,%12x\n", GetEnum(token.type), token.lexeme, token.value, token.FuncPtr);
        else
            break;
    }
    printf("-------------------------------------------\n");
    CloseScanner();//关闭词法分析器
    system("pause");
}
*/