#include "state.h"
void init_state(BufferState *s){
    dbuf_init(&s->globals);
    dbuf_init(&s->main_body);
    dbuf_init(&s->last_globals);
    dbuf_init(&s->last_main_body);
}

void free_state(BufferState *s){
    dbuf_free(&s->globals);
    dbuf_free(&s->main_body);
    dbuf_free(&s->last_globals);
    dbuf_free(&s->last_main_body);
}
