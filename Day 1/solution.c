#include <stdio.h>
#include <string.h>

int main() {
    FILE *fp;
    char buff[255];
    int value=0;
    int max_val=0;
    char finchar='\n';

    fp = fopen("input","r");
    // todo once c works on windows
    fclose(fp);

}

