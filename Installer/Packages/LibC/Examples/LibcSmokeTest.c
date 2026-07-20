#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int CompareInt(const void *a, const void *b)
{
    int aa = *(const int *)a, bb = *(const int *)b;
    return (aa > bb) - (aa < bb);
}

int main(void)
{
    char *text = malloc(64);
    assert(text != NULL);
    strcpy(text, "OESDK full Newlib libc is active");

    int values[] = { 7, 1, 9, 3 };
    qsort(values, 4, sizeof(values[0]), CompareInt);
    assert(values[0] == 1 && values[3] == 9);
    assert(fabs(sqrt(81.0) - 9.0) < 0.000001);

    printf("%s; sorted=%d,%d,%d,%d\n",
           text, values[0], values[1], values[2], values[3]);
    free(text);
    return 0;
}
