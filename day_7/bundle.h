#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _BUNDLE_H_GUARD_
#define _BUNDLE_H_GUARD_

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

extern int global_id;

file_bundle* init_new_empty_bundle(file_bundle* parent, char* name);

int push_bundle_to_parent_children(file_bundle *bundle);
int remove_bundle_from_parent_children(file_bundle *bundle);

int init_directory_bundle(file_bundle* bundle);

int init_binary_file_bundle(file_bundle* bundle, int size, void* data);

int remove_bundle_rec(file_bundle* bundle);

int remove_bundle(file_bundle* bundle);

int find_file_rec(file_bundle* from_bundle, file_bundle** to_bundle, char* name);
int find_directory_rec(file_bundle* from_bundle, file_bundle** to_bundle, char* name);
int rec_find(file_bundle* from_bundle, file_bundle** to_bundle, char* name, int prefer);

enum commands {
    unknown=0,
    cd=1,
    commands=2,
    mk=3,
    mkdir=4,
    ls=5,
    tree=6,
    touch=7,
    quit=8,
    verbose=9,
    rm=10,
};

int find_command(char* buff);

enum program_state{
    active_program=0,
    verbose_active=1,
    quit_program=2,
};

void print_bundle(file_bundle *bundle, int level);
void print_bundle_path(file_bundle *bundle, file_bundle* parent);
void print_verbose(char* command, char* buff, int* state);
void parse_command(file_bundle** curr_dir,int command, char* buff, int *state);
int swell(file_bundle* origin);

#endif