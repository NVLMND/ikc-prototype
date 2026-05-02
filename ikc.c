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
#include"ikc.h"
#define LINE_MAX 4096

static void cleanup_temp_files(void) {
    (void)remove("temp.c");
    (void)remove("temp.o");
    (void)remove("temp.out");
}

int main(void){
    IKC ikc={
        .compilers={
            {"tcc", NULL, 0},
            {"gcc", NULL, 0}
        }
    };
    ikc.c=&ikc.compilers[0];
    atexit(cleanup_temp_files);
    term_enable_raw();
    init_state(&ikc.state);
    char line[LINE_MAX];
    
    for(int i=0; i<2; i++){
        ikc.compilers[i].path=compiler_path(ikc.compilers[i].name);
        if(ikc.compilers[i].path){
            ikc.compilers[i].a=1;
            printf("\r[%s]=%s\n", ikc.compilers[i].name, ikc.compilers[i].path);
        }else{
            printf("\r[%s]=not found\n", ikc.compilers[i].name);
        }
    }

    printf("\r" IKC_LABEL "IKC" C_RESET "(type: "
           CMD_MODE"':run', ':clear', ':quit'"C_RESET")\n");
    fflush(stdout);
    while(1){
        const char *prompt=ikc.ps.brace_depth ? "\r\033[K...     " : "\r\033[K>>> ";
        int n=ikc_readline(&ikc.state, line, sizeof(line), prompt);
        if(n<0){
            printf("\n");
            break;
        }
        LineType type=classify_line(line, &ikc.ps);
        switch(type){
            case LINE_EMPTY:
                break;
            case LINE_COMMAND:
                if(strcmp(line, ":run")==0){
                    execute_code(&ikc.state, ikc.compilers);
                }else if(strcmp(line, ":clear")==0){
                    ulist_free(&ikc.state.globals);
                    ulist_free(&ikc.state.main_body);
                    ulist_init(&ikc.state.globals);
                    ulist_init(&ikc.state.main_body);
                    history_init(&ikc.state.history);
                    ikc.ps.brace_depth=0;
                    printf("\r"CMD_MODE"[buffer cleared]"C_RESET "\n");
                }else if(strcmp(line, ":quit")==0){
                    printf("\r" C_MAGENTA "bye!"C_RESET "\n");
                    write(STDOUT_FILENO, "\r\033[K", 4); 
                    term_disable_raw();
                    free_state(&ikc.state);
                    free_compiler_path(ikc.compilers);
                    return 0;
                }else{
                    printf("\r" IKC_ERROR"[unknown command]"C_RESET "\n");
                }
                break;
            case LINE_GLOBAL:
            case LINE_MAIN:{
                UnitList *target=(type == LINE_GLOBAL)
                    ? &ikc.state.globals
                    : &ikc.state.main_body;
                int sel=history_get_selected(&ikc.state.history);
                if(sel!=-1){
                    int idx=history_get_buffer_index(&ikc.state.history, sel);
                    ulist_replace(target, idx, line);
                }else{
                    ulist_add(target, line);
                    history_add(&ikc.state.history, line, target->count -1);
                }
                break;
            }
        }
        update_parser_state(line, &ikc.ps);
        history_reset_navigation(&ikc.state.history);
    }
    free_state(&ikc.state);
    return 0;
}
