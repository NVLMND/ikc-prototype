#ifndef IKC_LINE_BUF_H
#define IKC_LINE_BUF_H

#include<stddef.h>
#include"state.h"

int ikc_readline(BufferState *state, char *out, size_t maxlen, const char *prompt);
#endif
