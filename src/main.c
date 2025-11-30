#include "environment.h"

ENTRYPOINT INT32 main(VOID) {
	
	PrintFormatedString("%s\r\n", "Hello world!");
	
	ExitProcess(0);
}

