# Drawing-language-Compiler
西电编译原理大作业——用C语言简单的绘图语言解释器
================================================
最终产生式:
------------
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


其他说明
---------
scanner.cpp  词法分析实现  
parser.cpp   语法分析实现  
semantic.cpp 语义分析实现  
test_scanner.cpp  test_parser.cpp  test_semantic.cpp均为测试所用的main()函数  
为了实现完整功能，应该只使得test_semantic.cpp有效，其他两个注释掉。  
添加了极其详细的注释和设置颜色的功能，详情请见代码  
