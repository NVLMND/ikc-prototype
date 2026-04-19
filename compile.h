#ifndef COMPILE_H
#define COMPILE_H
#include"state.h"

int build_c_file(BufferState *s, const char *filename);
int run_process(const char *path, char *const argv[]);
int run_binary(void);
int execute_code(BufferState *s);

#endif
