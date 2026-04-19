#include<ctype.h>
#include<string.h>
#include"classify.h"

static int starts_with(const char *s, const char *prefix){
    while(*prefix){
        if(*s++!=*prefix++) return 0;
    }
    return 1;
}

static int is_control_keyword(const char *line){
    return (starts_with(line, "if")     && !isalnum(line[2])) ||
           (starts_with(line, "while")  && !isalnum(line[5])) ||
           (starts_with(line, "for")    && !isalnum(line[3])) ||
           (starts_with(line, "switch") && !isalnum(line[6])) ||
           (starts_with(line, "else")   && !isalnum(line[4])) ||
           (starts_with(line, "do")     && !isalnum(line[2]));
}

static int looks_like_function(const char *line){
    if(!strchr(line, '(') || !strchr(line, ')') || !strchr(line, '{'))
        return 0;

    if(is_control_keyword(line))
        return 0;

    if(starts_with(line, "printf"))
        return 0;
    /*const char *p = strchr(line, '(');
    if (p == line) return 0;
    const char *q = p;
    while (q > line && isspace((unsigned char)*(q - 1))) q--;
    if (q == line) return 0;*/

    return 1;
}

static int is_global_declaration(const char *line){
    return starts_with(line, "typedef") ||
           starts_with(line, "struct")  ||
           starts_with(line, "enum")    ||
           starts_with(line, "union")   ||
           starts_with(line, "static")  ||
           starts_with(line, "extern")  ||
           starts_with(line, "const");
}

LineType classify_line(const char *line, ParserState *ps){
    if(!line) return LINE_EMPTY;
    while(isspace((unsigned char)*line)) line++;

    if(*line=='\0')
        return LINE_EMPTY;

    if(strcmp(line, "run")==0    ||
        strcmp(line, "clear")==0 ||
        strcmp(line, "quit")==0)
        return LINE_COMMAND;

    if(*line=='#')
        return LINE_GLOBAL;

    if(ps->in_function)
        return LINE_GLOBAL;

    if(looks_like_function(line)){
        if(strchr(line, '{')){
            ps->in_function=1;
            return LINE_GLOBAL;
        }else{
            ps->pending_function=1;
            return LINE_GLOBAL;
        }
    }
    if(ps->pending_function && strchr(line, '{')){
        ps->pending_function=0;
        ps->in_function=1;
        return LINE_GLOBAL;
    }
    if(is_global_declaration(line))
        return LINE_GLOBAL;

    return LINE_MAIN;
}

void update_parser_state(const char *line, ParserState *ps){
    for(const char *p=line; *p; p++) {
        if(*p=='{') ps->brace_depth++;
        else if(*p=='}') ps->brace_depth--;
    }

    if (ps->brace_depth<0)
        ps->brace_depth=0;

    if (ps->in_function && ps->brace_depth==0) {
        ps->in_function=0;
    }
}


    

