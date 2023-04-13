#include <stdio.h>

// There's probably a better way to do this with a proper filesystem controller
// and everything being smart array-based tree

enum file_type {
    uninitialized = 0,
    directory = 1,
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
    directory_bundle_children *children = bundle->parent->data;
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
    if (children->bundles==NULL) return -1; //failure
    return 0; //success
}


int init_directory_bundle(file_bundle* bundle){
    file_bundle* parent=bundle->parent;
    bundle->type=directory;
    directory_bundle_children* children=malloc(sizeof(directory_bundle_children));
    if (children==NULL) {
        bundle->type=uninitialized;
        return -1; //failure
    }
    children->n=0; // no children yet
    children->bundles = NULL;
    bundle->size = 0; // nothing in the director
    while (bundle->size!=0 && parent!=NULL) {
        parent->size += bundle->size;
        parent=bundle->parent;
    }
    return push_bundle_to_childrens_array(bundle); // success of failure
}

// initialize a binary file with binary data with size. The file bundle now owns
// the data and it will free()ed when the bundle is removed!
int init_binary_file_bundle(file_bundle* bundle, int size, void* data){
    file_bundle* parent=bundle->parent;
    bundle->type=binary_file;
    bundle->data=data;
    bundle->size = size;
    while (size!=0 && parent!=NULL) {
        parent->size += size;
        parent=bundle->parent;
    }
    return push_bundle_to_childrens_array(bundle); // success of failure
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
    while (fscanf(fp,"%255s\n",buff)!=EOF && i<100000 ){
        if (i<10){
            fprintf(stdout,"string: %s\n",buff);
        }
        
        i++;
    }

    fclose(fp);
    fprintf(stdout,"%d steps:\n",i);

    
    return 0;

}
