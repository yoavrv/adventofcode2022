#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// There's probably a better way to do this with a proper filesystem controller
// and everything being smart array-based tree

enum file_type {
    uninitialized = 0,
    directory=1,
    binary_file=2,
    link_bundle=3,
};

// file bundle: a node in a file-system with directors and files
// size should be the sum of all leaves
// a file bundle owns it's data unless it's a link
// a directory has type and data points to a struct of the children
// a binary file has type and data which points to the binary data
typedef struct file_bundle {
    struct file_bundle *parent;
    char name[256];
    long int size;
    int id;
    int type;
    void* data;
} file_bundle;

typedef struct directory_bundle_children {
    int n;
    file_bundle** bundles;
} directory_bundle_children;

int global_id=0;

file_bundle* init_new_empty_bundle(file_bundle* parent, char* name){
    file_bundle* child = malloc(sizeof(file_bundle));
    if (child==NULL) return NULL; // failure
    child->parent = parent;
    child->size=0;
    child->id = global_id++;
    child->type=uninitialized;
    child->data=NULL;
    int i;
    for (i=0; i<256; i++){
        child->name[i] = name[i];
        if (name[i]=='\0') break;
    }
    child->name[255]='\0';
    return child;
}



// place a bundle in the parent directory's children list
// allocate more space provided it is not already there
int push_bundle_to_parent_children(file_bundle *bundle){
    if (bundle->parent==NULL) return -1; // failure
    directory_bundle_children *children =(directory_bundle_children *) bundle->parent->data;
    if (children==NULL) return -1; //failure
    int i=0;
    for (i=0;i<children->n;i++){
        if (children->bundles[i]==bundle) return i; // already there
    }
    children->n+=1;
    children->bundles=realloc(children->bundles,children->n *sizeof(file_bundle*));
    if (children->bundles==NULL) return -1; //failure
    children->bundles[children->n-1]=bundle;
    return 0; //success
}

int remove_bundle_from_parent_children(file_bundle *bundle){
    if (bundle->parent==NULL) return -1; // failure
    directory_bundle_children *children = bundle->parent->data;
    if (children==NULL) return -1; //failure
    int i=0;
    int found=0;
    for (i=0;i+found<children->n;i++){
        if (children->bundles[i]==bundle) found+=1;
        children->bundles[i]=children->bundles[i+found];
    }
    if (!found) return -1; //failure
    children->n-=found;
    children->bundles=realloc(children->bundles,children->n *sizeof(file_bundle*));
    if (children->bundles==NULL && children->n!=0) return -1; //failure
    file_bundle* parent=bundle->parent;
    while (bundle->size!=0 && parent!=NULL) {
        parent->size -= bundle->size;
        parent=parent->parent;
    }
    return 0; //success
}


int init_directory_bundle(file_bundle* bundle){
    file_bundle* parent=bundle->parent;
    bundle->type=directory;
    directory_bundle_children* children=malloc(sizeof(directory_bundle_children));
    bundle->data = children;
    if (children==NULL) {
        bundle->type=uninitialized;
        return -1; //failure
    }
    children->n=0; // no children yet
    children->bundles = NULL;
    bundle->size = 0; // nothing in the director
    while (bundle->size!=0 && parent!=NULL) {
        parent->size += bundle->size;
        parent=parent->parent;
    }
    return push_bundle_to_parent_children(bundle); // success of failure
}

// initialize a binary file with binary data with size. The file bundle now owns
// the data and it will free()ed when the bundle is removed!
int init_binary_file_bundle(file_bundle* bundle, int size, void* data){
    file_bundle* parent=bundle->parent;
    bundle->type=binary_file;
    bundle->data=data;
    bundle->size=size;
    while (size!=0 && parent!=NULL) {
        parent->size += size;
        parent=parent->parent;
    }
    return push_bundle_to_parent_children(bundle); // success of failure
}

// remove bundle as part removal recursion
int remove_bundle_rec(file_bundle* bundle){
    int retval=0;
    if (bundle==NULL) return 0; // "success"
    if (bundle->type==directory) {
        directory_bundle_children *children = bundle->data;
        int i=0;
        for (i=0;i<children->n;i++){
            retval = remove_bundle_rec(children->bundles[i]); // should never have duplicates!
            if (retval<0) return retval; // failure
        }
        free(children->bundles);
    } 
    if (bundle->type!=link_bundle) free(bundle->data);
    free(bundle);
    return 0; //success
}

int remove_bundle(file_bundle* bundle){
    remove_bundle_from_parent_children(bundle);
    remove_bundle_rec(bundle);
}


// print a bundle and all sub-bundles for directories
// if level=-1: print the current bundle only
void print_bundle(file_bundle *bundle, int level){
    if (bundle==NULL){
        fprintf(stdout,"NULL\n");
        return;
    }
    int i=0;
    for (i=0;i<level;i++) fprintf(stdout,"- ");
    fprintf(stdout,"%s ",bundle->name);
    switch (bundle->type) {
        case binary_file:
        fprintf(stdout,"(binary file, %d)\n",bundle->size);
        break;
        case directory:
        fprintf(stdout,"(directory, %d)\n", bundle->size);
        if (level!=-1) {
            directory_bundle_children *children = bundle->data;
            if (children==NULL) break;
            for (i=0;i<children->n;i++) print_bundle(children->bundles[i],level+1);
        }
        break;
        default:
        fprintf(stdout,"(unknown, %d)\n",bundle->size);
        break;
    }
}

void print_bundle_path(file_bundle *bundle, file_bundle* parent){
    if (bundle==NULL){
        return;
    }
    if (bundle==parent){
        fprintf(stdout,"%s",bundle->name);
        return;
    }
    print_bundle_path(bundle->parent,parent);
    fprintf(stdout,"/%s",bundle->name);
}

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

enum commands {
    unknown=0,
    cd=1,
    commands=2,
    mk=3,
    mkdir=4,
    ls=5,
    tree=6,
    touch=7,
    nano=8,
    quit=9,
    verbose=10,
    rm=11,
};

int find_command(char* buff){
    // just a tree of possibilities
    if (buff[0]=='v'){
        return verbose;
    } else if (buff[0]=='r'){
        return rm;
    } else if (buff[0]=='q'){
        return quit;
    } else if (buff[0]=='c'){
        // cd and commands
        if (buff[1]=='d') {
            return cd;
        } else if (buff[1]=='o') {
            return commands;
        }
    } else if (buff[0]=='m'){
        // mk and mkdir
        if (buff[1]=='k') {
            if (buff[2]=='d') {
                return mkdir;
            } else if (buff[2]==' ' || buff[2]=='\0') {
                return mk;
            }
        }
    } else if (buff[0]=='l'){
        // ls
        return ls;
    } else if (buff[0]=='t'){
        // tree or touch
        if (buff[1]=='o') {
            return touch;
        } else if (buff[1]=='r') {
            return tree;
        }
    } else if (buff[0]=='n'){
        // nano
        return nano;
    }
    return unknown;
}

enum program_state{
    active_program=0,
    verbose_active=1,
    quit_program=2,
};


int rec_cd(file_bundle* from_bundle, file_bundle** to_bundle, char* name){
    // .. parent folder
    if (strcmp(name,"..")==0) {
        if (from_bundle->parent==NULL) return -1;
        *to_bundle=from_bundle->parent;
        return 0;
    }
    // ..something folder
    if (strncmp(name,"..",2)==0) {
        if (from_bundle->parent==NULL) return -1;
        if (name[2]=='/') return rec_cd(from_bundle->parent,to_bundle,name+3);
        else return rec_cd(from_bundle->parent,to_bundle,name+2);
    } 
    // . current folder
    if (strcmp(name,".")==0){
        *to_bundle=from_bundle;
        return 0;
    }
    // / home folder
    if (strcmp(name,"/")==0){
        if (strcmp(from_bundle->name,"/")==0){
            *to_bundle = from_bundle;
            return 0;
        }
        if (from_bundle->parent==NULL) return -1;
        return rec_cd(from_bundle->parent,to_bundle,name);
    } 
    // / fundemental folder
    if (strncmp(name,"/",1)==0) {
        if (strcmp(from_bundle->name,"/")==0){
            return rec_cd(from_bundle,to_bundle,name+1); // at / directory, proceed to find the rest
        }
        if (from_bundle->parent==NULL) return -1;
        return rec_cd(from_bundle->parent,to_bundle,name); // go to / directory
    }

    // normal folder or sub folder
    if (strcmp(name, from_bundle->name)==0){
        *to_bundle=from_bundle;
        return 0;
    }
    int i=0;
    int num_names = 0;
    char inner_name[256],outer_name[256];
    num_names=sscanf(name,"%255[^/]/%255s",inner_name,outer_name);
    directory_bundle_children *children = (directory_bundle_children *) from_bundle->data;
    file_bundle* child=NULL;
    if (children==NULL) return -1;
    for (i=0;i<children->n;i++){
        child = children->bundles[i];
        if (child->type==directory) {
            if (strcmp(inner_name,child->name)==0) {
                if (num_names==1) {
                    *to_bundle=child;
                    return 0;
                } else if (num_names==2) {
                    return rec_cd(child,to_bundle,outer_name);
                }
            }
        } else if (strcmp(inner_name,child->name)==0) {
            *to_bundle=child;
            return -2; // file exist but is not a directory!
        }
    }
    // found nothing
    return -1;

}

void print_verbose(char* command, char* buff, int* state){
    char new_buff[256];
    int i=0;
    strncpy(new_buff,buff,255);
    for (i=0;i<255;i++) if (new_buff[i]=='\n') break;
    new_buff[i]='\0';
    if (*state==verbose_active) fprintf(stdout,"read %s. command: %s\n",command, new_buff);
}

void parse_command(file_bundle** curr_dir,int command, char* buff, int *state){
    file_bundle* bundle;
    directory_bundle_children *children = NULL;
    char arg1[256];
    char arg2[256];
    int i=0;
    switch (command) {
        case quit:
            print_verbose("quit",buff,state);
            *state = quit_program;
            fprintf(stdout,"quitting\n");
            break;
        case unknown:
            print_verbose("unknown",buff,state);
            fprintf(stdout,"unknown command\n");
            break;
        case tree:
            print_verbose("tree",buff,state);
            print_bundle(*curr_dir,0);
            break;
        case ls:
            print_verbose("ls",buff,state);
            print_bundle_path(*curr_dir,NULL);
            fprintf(stdout,"\n");
            children = (*curr_dir)->data;
            if (children==NULL) return;
            for (i=0;i<children->n;i++){
                print_bundle(children->bundles[i],-1);
            }
            break;
        case verbose:
            if (*state==verbose_active) {
                fprintf(stdout,"command: verbose ; shutting up\n");
                *state=active_program;
            } else {
                fprintf(stdout,"verbose: start verbalizing\n");
                *state=verbose_active;
            }
            break;
        case touch:
            print_verbose("touch",buff,state);
            if (sscanf(buff,"%255s %255s",arg1,arg2)==EOF) {fprintf(stdout,"b\n"); return;}
            bundle = init_new_empty_bundle(*curr_dir, arg2);
            init_binary_file_bundle(bundle,0,NULL);
            print_bundle(bundle,-1);
            break;
        case mkdir:
            print_verbose("mkdir",buff,state);
            if (sscanf(buff,"%255s %255s",arg1,arg2)==EOF) {fprintf(stdout,"\n"); return;}
            bundle = init_new_empty_bundle(*curr_dir, arg2);
            init_directory_bundle(bundle);
            print_bundle(bundle,-1);
            break;
        case mk:
            print_verbose("mk",buff,state);
            if (sscanf(buff,"%255s %255s %d",arg1,arg2,&i)==EOF) {fprintf(stdout,"\n"); return;}
            bundle = init_new_empty_bundle(*curr_dir, arg2);
            init_binary_file_bundle(bundle,i,NULL);
            print_bundle(bundle,-1);
            break;
        case cd:
            print_verbose("cd",buff,state);
            if (sscanf(buff,"%255s %255s",arg1, arg2)==EOF) {fprintf(stdout,"\n"); return;}
            i = rec_cd(*curr_dir,&bundle, arg2);
            if (i==0) *curr_dir=bundle;
            if (i==-1 && *state==verbose_active) fprintf(stdout,"failed\n");
            if (i==-2) fprintf(stdout,"not a directory!\n");
            break;
        case commands:
            print_verbose("commands",buff,state);
            fprintf(stdout,"Available commands:\n");
            fprintf(stdout,"unknown, cd, commands\n");
            fprintf(stdout,"mk, mkdir, ls\n");
            fprintf(stdout,"tree, touch, nano\n");
            fprintf(stdout,"quit, verbose\n");
            break;
        case rm:
            print_verbose("rm",buff,state);
            if (sscanf(buff,"%255s %255s",arg1, arg2)==EOF) {fprintf(stdout,"\n"); return;}
            i = rec_cd(*curr_dir,&bundle, arg2);
            if (i==-2) {
                remove_bundle(bundle);
            } else if (i==0) {
                if (bundle->parent!=NULL) remove_bundle(bundle);
            }
    }

}

int main(int argc, char **argv) {
    FILE *fp;
    int command=unknown;
    int state=active_program;
    char buff[256];
    file_bundle* origin=init_new_empty_bundle(NULL,"/");
    init_directory_bundle(origin);

    file_bundle* curr_dir = origin;
    // directory_bundle_children* children = (directory_bundle_children*) curr_dir->data;
    // fprintf(stdout,"%d, %p",children->n,children->bundles);

    // fp=ope_file_or_stdin(args,argv);

    // int i=0;// loop variable
    // while (fscanf(fp,"%255s\n",buff)!=EOF && i<100000 ){
    //     if (i<10){
    //         fprintf(stdout,"string: %s\n",buff);
    //     }
        
    //     i++;
    // }
    while (state==active_program || state==verbose_active){
        fprintf(stdout,"%s $",curr_dir->name);
        fgets(buff,255, stdin);
        command=find_command(buff); 
        parse_command(&curr_dir,command,buff,&state);
    }
    fprintf(stdout,"Exiting...");
    remove_bundle(origin);
    fclose(fp);    
    return 0;
}
