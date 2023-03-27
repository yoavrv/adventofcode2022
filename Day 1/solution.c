#include <stdio.h>
#include <string.h>

int main() {
    FILE *fp;
    int item=0;
    int value=0;
    int index=0;
    int max_value=0;
    int index_max_value=0;
    char curr_char='\n';
    char prev_char='\n';

    fp = fopen("./input","r");
    if (fp==NULL) {
        perror("Error opening \"input\" file");
    }
    while ( !feof(fp) ){
        // save position if we are starting a new line
        
        curr_char=fgetc(fp);
        // if (curr_char=='\n')  { 
        //     fprintf(stdout,"->%d: \\n ",ftell(fp));
        // } else if (curr_char=='\r') {
        //     fprintf(stdout,"->%d: \\r ",ftell(fp),curr_char);
        // } else {
        //     fprintf(stdout,"->%d: %c ",ftell(fp),curr_char);
        // }

        if ( '0' <= curr_char && curr_char <= '9'){  
                // new digit in number
                item = 10*item + curr_char - '0';
        } else if (curr_char=='\n')  { 
            // number is done or a new blank line
            value+=item;
            item=0;
            if (prev_char=='\n'){ 
                // new blank line: save value and index
                if (value>max_value){
                    max_value=value;
                    index_max_value=index;
                }
                value=0;
                index++;
            } 
        } 
        prev_char=curr_char;
    }
    // end of file, but we might have one last item
    value+=item;
    item=0;
    if (value>max_value){
        max_value=value;
        index_max_value=index;
    }
    
    fclose(fp);
    fprintf(stdout,"Maximum value %d is list number %d\n",max_value, index_max_value);
}

