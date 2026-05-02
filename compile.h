#ifndef COMPILE_H
#define COMPILE_H
#include"state.h"

typedef struct{
    const char *name;
    char *path;
    int a;
}Compiler;

int build_c_file(BufferState *s, const char *filename);
int run_process(const char *path, char *const argv[]);
int run_binary(void);
int execute_code(BufferState *s, Compiler *c);
char *compiler_path(const char *cmd);
void free_compiler_path(Compiler *c);
#endif
