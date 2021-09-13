#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void main(int argc, char const *argv[])
{
    unsigned long year = strtol(argv[1], NULL, 10);
    int is_leap_year = 0;
    char qualifier[4] = "";

    if( year % 400 == 0 || ((year % 4 == 0) && (year % 100 != 0)) ){
        is_leap_year = 1;
    }

    if(! is_leap_year){
        strcat(qualifier, " not");
    }

    printf("%lu was%s a leap year", year, qualifier);
}
