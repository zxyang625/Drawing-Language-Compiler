#include "semantic.h"
#include <graphics.h>
int main()
{
	initgraph(720, 640);
	Parser((char*)"draw.txt");
	Sleep(3000);
	closegraph();
	return 0; 
}
