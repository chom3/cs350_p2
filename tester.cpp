#include <stdio.h>
#include "time_functions.h"
#include <stdlib.h>
#include <iostream>
#include <string>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <fstream>

int main()
{
    FILE *iFP = fopen("test.txt", "r");
    char buf[1024];
    fgets(buf, 1024, iFP);
    printf("%s", buf);
}