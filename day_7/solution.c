#include <stdio.h>


int main(int argc, char **argv) {
    FILE *fp;
    char buff[256];

    if (argc == 1) {
        fp = stdin;
    } else { 
        fprintf(stdout,"argc %d\n", argc) ;
        fprintf(stdout,"Opening %s\n", argv[1]) ;
        fp = fopen(argv[1],"r");
        if (fp==NULL) {
            perror("Error opening file");
        }
    }

    int i=0;// loop variable
    while (fscanf(fp,"%s\n",buff)!=EOF && i<100000 ){
        if (i<10){
            fprintf(stdout,"string: %s\n",buff);
        }
        
        i++;
    }

    fclose(fp);
    fprintf(stdout,"%d steps:\n",i);

    
    return 0;

}
