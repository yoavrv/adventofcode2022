#include <stdio.h>
#include <string.h>

int main() {
    FILE *fp;
    int ind=0;
    int start_line=0;
    int end_line=0;
    int item=0;
    int value=0;
    int index=0;
    int max_value=0;
    int index_max_value=0;
    char curr_char='\n';
    char prev_char='\n';

    fp = fopen("./input","r");
    // todo once c works on windows
    if (fp==NULL) {
        perror("Error opening \"input\" file");
    }
    while ( !feof(fp) && ind<100){
        ind++;
        // save position if we are starting a new line
        if (prev_char=='\n'){ 
            start_line=ftell(fp);
            fprintf(stdout,"start line: %d\n",start_line);
        }
        
        // advance to next character
        curr_char=fgetc(fp);
        fprintf(stdout,"%d: %c\n",ind,curr_char);
        if (curr_char==NULL){
            perror("Error: Null character in file");
        }

        // encounter a newline
        if (curr_char=='\n')  { 
            // number is done or a new blank line

            if (prev_char=='\n'){ 
                // new blank line: save value and index
                if (value>max_value){
                    max_value=value;
                    index_max_value=index;
                }
                fprintf(stdout,"list: index %d: value:%d\n",index,value);
                value=0;
                index++;
            } else {  
                // end of number, record the value
                end_line = ftell(fp);
                fseek(fp, start_line, SEEK_SET); // return to start of number
                fscanf(fp,"%d",&item); // read the number into an integer
                value += item;
                fprintf(stdout,"item: %d:\n",item);
                fseek(fp, end_line, SEEK_SET); // return to end of number
                fprintf(stdout,"at: %d\n", ftell(fp));
            }
        }
        prev_char=curr_char;
    }
    // end of file, but we might have one last item
    if (prev_char!='\n'){
        fseek(fp, start_line, SEEK_SET); // return to start of number
        fscanf(fp,"%d",&item); // read the number into an integer
        value += item;
    }
    if (value>max_value){
        max_value=value;
        index_max_value=index;
    }
    
    fclose(fp);
    fprintf(stdout,"Maximum value %d is list number %d\n",max_value, index_max_value);
}
