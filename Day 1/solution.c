#include <stdio.h>
#include <string.h>

int update_max_value(int new_value, int new_index,int* max_value, int* index_max_value, int len){
    int i=0;
    int new=0;
    for (i=0;i<len;i++){
        if (new_value>max_value[i]){
            new=1;
            if (i!=0){
                max_value[i-1]=max_value[i];
                index_max_value[i-1]=index_max_value[i];
            }
            max_value[i]=new_value;
            index_max_value[i]=new_index;
        }
    }
    return new;
}

int print_max_values(int* max_value, int* index_max_value, int len){
    int i=0;
    int total=0;
    for (i=0;i<len;i++){
        fprintf(stdout,"%d: %d\n",index_max_value[i],max_value[i]);
        total+=max_value[i];
        }
    return total;
}


int main() {
    FILE *fp;
    int item=0;
    int value=0;
    int index=0;
    int max_value[3]={0,0,0};
    int index_max_value[3]={0,0,0};
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
                update_max_value(value,index,max_value,index_max_value,3);
                value=0;
                index++;
            } 
        } 
        prev_char=curr_char;
    }
    // end of file, but we might have one last item
    value+=item;
    item=0;
    update_max_value(value,index,max_value,index_max_value,3);

    fclose(fp);
    fprintf(stdout,"Top three:\n");
    item=print_max_values(max_value, index_max_value, 3);
    fprintf(stdout,"total: %d\n",item);
}

