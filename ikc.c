#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include"state.h"
#include"classify.h"
#include"dyn_buf.h"
#include"compile.h"
#include"term.h"
#include"readline.h"
#include"colors.h"
#define LINE_MAX 4096

static void cleanup_temp_files(void) {
    (void)remove("temp.c");
    (void)remove("temp.o");
    (void)remove("temp.out");
}

int main(void){
    atexit(cleanup_temp_files);
    term_enable_raw();
    BufferState state;
    ParserState ps={0};
    init_state(&state);
    char line[LINE_MAX];

    printf(IKC_LABEL "IKC" C_RESET "(type: "
           CMD_MODE"'run', 'clear', 'quit'"C_RESET")\n");
    while(1){
        int n=ikc_readline(line, sizeof(line));
        if(n<0){
            printf("\n");
            break;
        }
        LineType type=classify_line(line, &ps);
        switch(type){
            case LINE_EMPTY:
                break;
            case LINE_COMMAND:
                if(strcmp(line, "run")==0){
                    execute_code(&state);
                }else if(strcmp(line, "clear")==0){
                    dbuf_reset(&state.globals);
                    dbuf_reset(&state.main_body);
                    ps.brace_depth=0;
                    printf("\r"CMD_MODE"[buffer cleared]"C_RESET "\n");
                }else if(strcmp(line, "quit")==0){
                    printf("\r" C_MAGENTA "bye!"C_RESET "\n");
                    write(STDOUT_FILENO, "\r\033[k", 4); 
                    term_disable_raw();
                    free_state(&state);
                    return 0;
                }else{
                    printf("\r" IKC_ERROR"[unknown command]"C_RESET "\n");
                }
                break;
            case LINE_GLOBAL:
                //update_brace_depth(line, &brace_depth);
                dbuf_append_line(&state.globals, line);
                break;
            case LINE_MAIN:
                //update_brace_depth(line, &brace_depth);
                dbuf_append_line(&state.main_body, line);
                break;
        }
        update_parser_state(line, &ps);
    }
    free_state(&state);
    return 0;
}
