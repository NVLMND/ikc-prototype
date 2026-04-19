#ifndef CLASSIFY_H
#define CLASSIFY_H
typedef struct{
    int brace_depth;
    int in_global_block;
    int in_function;
    int pending_function;
}ParserState;

typedef enum{
    LINE_EMPTY,
    LINE_COMMAND,
    LINE_GLOBAL,
    LINE_MAIN
}LineType;

LineType classify_line(const char *line, ParserState *ps);
void update_parser_state(const char *line, ParserState *ps);
#endif
