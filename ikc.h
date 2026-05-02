#ifndef IKC_H
#define IKC_H
#include"state.h"
#include"compile.h"
#include"classify.h"

typedef struct{
    BufferState state;
    ParserState ps;
    Compiler compilers[2];
    Compiler *c;
}IKC;
#endif
