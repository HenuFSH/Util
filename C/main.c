#include <stdio.h>
#include "c_string"

int main(int argc, char *argv[])
{
    char strs[][128] = {
        "",
        " ",
        "          Hello World    d  ",
        " Hello",
        "\t\nHello World ",
        "END"
    };

   int i = 0;
   do{
      printf("trim(\"%s\")=%s.\n", strs[i], trim(strs[i]));
   }while(strcmp(strs[i++], "END"));

   char * t0 = trim(strs[0]);
   char * t1 = trim(strs[1]);
   char * t2 = trim(strs[2]);
   char * t3 = trim(strs[3]);
   char * t4 = trim(strs[4]);

    return 0;

}

