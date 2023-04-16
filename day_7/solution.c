#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bundle.h"

FILE* open_file_or_stdin(int argc,char **argv){
    FILE *fp;
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
    return fp;
}

long int rec_sum_directory_sizes(file_bundle* bundle, long int maximum){
    if (bundle->type!=directory) return 0;
    long int size_sum=0;
    int i=0;
    directory_bundle_children* children = bundle->data;
    for (i=0;i<children->n;i++) size_sum+=rec_sum_directory_sizes(children->bundles[i],maximum);
    if (bundle->size<maximum) size_sum+=bundle->size;
    return size_sum;
}  

int main(int argc, char **argv) {
    FILE *fp;
    file_bundle* origin=init_new_empty_bundle(NULL,"/");
    init_directory_bundle(origin);

    // swell(origin);
    fp=open_file_or_stdin(argc,argv);
    char buff[256];

    file_bundle* curr_dir = origin;
    file_bundle* bundle=NULL;
    int i=0;// loop variable
    int command=unknown;
    int success=0;
    char name[256];
    int size=0;
    // we only have ls or cd commands.
    while (fscanf(fp,"%255[^\n]\n",buff)!=EOF && i<100000 ){
        bundle=NULL;
        success=0;
        size=0;

        if (buff[0]=='$') {
            // read new command
            command = find_command(buff+2);

            if (command==cd) {
                // find where to go
                if (sscanf(buff,"$ cd %s",name)==1) success=find_directory_rec(curr_dir,&bundle,name);
                if (success!=0  && (strcmp(name,"..")!=0) ){
                    // we encountered a new directory
                    bundle=init_new_empty_bundle(curr_dir,name);
                    if (bundle==NULL) fprintf(stdout,"failed allocation at %d",i);
                    init_directory_bundle(bundle);
                    success=0;
                }
                if (success==0) curr_dir=bundle;
            } // else if (command==ls) wait for next line)

        } else if (command==ls){
            if (strncmp(buff,"dir",3)==0){
                //directory in format "dir name"
                if (sscanf(buff,"dir %s",name)==1) success=find_directory_rec(curr_dir,&bundle,name);
                if (success!=0 && (strcmp(name,"..")!=0)){
                    // we encountered a new directory
                    bundle=init_new_empty_bundle(curr_dir,name);
                    if (bundle==NULL) fprintf(stdout,"failed allocation at %d",i);
                    init_directory_bundle(bundle);
                }

            } else { 
                // not a directory in format "size name"
                if (sscanf(buff,"%d %s",&size,name)==2) success=find_file_rec(curr_dir,&bundle,name);
                if (success!=0){
                    // we encountered a new file
                    bundle=init_new_empty_bundle(curr_dir,name);
                    if (bundle==NULL) fprintf(stdout,"failed allocation at %d",i);
                    init_binary_file_bundle(bundle,size,NULL);
                }
            }
        } // else if (command==cd) in the same line as $
        i++;
    }
    fprintf(stdout,"done\n");
    fprintf(stdout,"size is %ld",rec_sum_directory_sizes(origin,100000));
    swell(origin);
    remove_bundle(origin);
    fclose(fp);    
    return 0;
}
