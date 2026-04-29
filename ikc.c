#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include"state.h"
#include"classify.h"
//#include"dyn_buf.h"
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
           CMD_MODE"':run', ':clear', ':quit'"C_RESET")\n");
    fflush(stdout);
    while(1){
        const char *prompt=ps.brace_depth ? "\r\033[K...     " : "\r\033[K>>> ";
        int n=ikc_readline(&state, line, sizeof(line), prompt);
        if(n<0){
            printf("\n");
            break;
        }
        LineType type=classify_line(line, &ps);
        switch(type){
            case LINE_EMPTY:
                break;
            case LINE_COMMAND:
                if(strcmp(line, ":run")==0){
                    execute_code(&state);
                }else if(strcmp(line, ":clear")==0){
                    ulist_free(&state.globals);
                    ulist_free(&state.main_body);
                    ulist_free(&state.globals);
                    ulist_free(&state.main_body);
                    ps.brace_depth=0;
                    printf("\r"CMD_MODE"[buffer cleared]"C_RESET "\n");
                }else if(strcmp(line, ":quit")==0){
                    printf("\r" C_MAGENTA "bye!"C_RESET "\n");
                    write(STDOUT_FILENO, "\r\033[K", 4); 
                    term_disable_raw();
                    free_state(&state);
                    return 0;
                }else{
                    printf("\r" IKC_ERROR"[unknown command]"C_RESET "\n");
                }
                break;
            case LINE_GLOBAL:
            case LINE_MAIN:{
                UnitList *target=(type == LINE_GLOBAL)
                    ? &state.globals
                    : &state.main_body;
                int sel=history_get_selected(&state.history);
                if(sel!=-1){
                    int idx=history_get_buffer_index(&state.history, sel);
                    ulist_replace(target, idx, line);
                }else{
                    ulist_add(target, line);
                    history_add(&state.history, line, target->count -1);
                }
                break;
            }
        }
        update_parser_state(line, &ps);
        history_reset_navigation(&state.history);
    }
    free_state(&state);
    return 0;
}
