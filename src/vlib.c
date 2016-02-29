#include "vlib.h"
#include "vio.h"

void __assert_fail(const char *__assertion, const char *__file,
	unsigned int __line, const char *__function)
{
	printf("Assertion failed: [");
    printf(__assertion);
    printf("] in : ");
    printf(__file);
    printf(" : ");
    printf(__function);
    printf(" # ");
    printint(__line);
    printf("\n");

    for(;;) { }
}
