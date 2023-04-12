#include <stdio.h>
#include <strings.h>

char* duplicated_char_mid_str(char* str){
    int len=0;
    char *p1,*p2,*midstr;
    len = strlen(str);
    midstr=str+len/2;
    for (p1=str; p1<midstr; p1++){
        for (p2=midstr; *p2!='\0'; p2++){
            if (*p1==*p2){
                return p1;
            }
        }
    }
    return 0; // error
}

char* duplicated_char_3_str(char* str0,char* str1, char* str2){
    char *p0,*p1,*p2;
    for (p0=str0; *p0!='\0'; p0++){
        for (p1=str1; *p1!='\0'; p1++){
            if (*p0==*p1){
                for (p2=str2; *p2!='\0'; p2++){
                    if (*p1==*p2 ){
                        return p1;
                    }
                }
            }
        }
    }
    return 0; // error
}

int score_char(char c){
    if (c<='Z')  return c-'A'+27; // uppercase
    return c-'a'+1; //lowercase
}


int main(int argc, char **argv) {
    FILE *fp;
    char buff0[256];
    char buff1[256];
    char buff2[256];
    int i=0;
    int item_score=0;
    int score_singles=0;
    int score_triplets=0;
    char* c;

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

    i=0;// count number of turns in t
    while (fscanf(fp,"%s\n%s\n%s",buff0,buff1,buff2)!=EOF && i<100000 ){
        c = duplicated_char_mid_str(buff0);
        item_score = score_char(*c);
        if (i<10){
            fprintf(stdout,"string: %s, char: %c, value: %d\n",buff0, *c ,item_score);
        }
        score_singles+=item_score;
        c = duplicated_char_mid_str(buff1);
        item_score = score_char(*c);
        if (i<10){
            fprintf(stdout,"string: %s, char: %c, value: %d\n",buff1, *c, item_score);
        }
        score_singles+=item_score;
        c = duplicated_char_mid_str(buff2);
        item_score = score_char(*c);
        if (i<10){
            fprintf(stdout,"string: %s, char: %c, value: %d\n",buff2, *c,item_score);
        }
        score_singles+=item_score;

        c = duplicated_char_3_str(buff0,buff1,buff2);
        item_score = score_char(*c);
        if (i<10){
            fprintf(stdout,"group: %d, common value: %d\n",i,item_score);
        }
        score_triplets+=item_score;
        i++;
    }

    fclose(fp);
    fprintf(stdout,"%d sacks were checked:\n",3*i);
    fprintf(stdout,"singles mid-split score: %d\n",score_singles);
    fprintf(stdout,"triplets score: %d\n",score_triplets);
    
    return 0;

}
