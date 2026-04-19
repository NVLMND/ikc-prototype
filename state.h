#ifndef STATE_H
#define STATE_H
#include"dyn_buf.h"

typedef struct {
    DynBuf globals;
    DynBuf main_body;
    DynBuf last_globals;
    DynBuf last_main_body;
}BufferState;

void init_state(BufferState *s);
void free_state(BufferState *s);

#endif
