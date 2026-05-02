#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/wait.h>
#include<unistd.h>
#include"state.h"
#include"colors.h"
#include"compile.h"
static char *find_path(const char *cmd){
    char *path = getenv("PATH");
    if (!path) return NULL;

    char *path_copy=strdup(path);
    char *saveptr;
    char *dir=strtok_r(path_copy, ":", &saveptr);

    while(dir){
        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", dir, cmd);

        if (access(fullpath, X_OK)==0) {
            free(path_copy);
            return strdup(fullpath);
        }
        dir = strtok_r(NULL, ":", &saveptr);
    }
    free(path_copy);
    return NULL;
}

void free_compiler_path(Compiler *c){
    for(int i=0; i<2; i++){
        free(c[i].path);
        c[i].path=NULL;
    }
}
int build_c_file(BufferState *s, const char *filename){
    FILE *f=fopen(filename, "w");
    if(!f){
        perror("fopen");
        return-1;
    }
    for(int i=0; i<s->globals.count; i++){
        fprintf(f, "%s\n", s->globals.items[i]);
    }
    fprintf(f, "int main() {\n");
    for(int i=0; i<s->main_body.count; i++){
        fprintf(f, "%s", s->main_body.items[i]);
    }
    fprintf(f, "return 0;\n");
    fprintf(f, "}\n");
    fclose(f);
    return 0;
}

int run_process(const char *path, char *const argv[]){
    pid_t pid=fork();
    if(pid==0){
        execv(path, argv);
        perror("exec failed");
        _exit(127);
    }else if(pid>0){
        int status;
        waitpid(pid, &status, 0);
        if(WIFEXITED(status))
            return WEXITSTATUS(status);
        else return -1;
    }else{
        perror("fork failed");
        return -1;
    }
}

int run_binary(void){
    char *args[]={
        "./temp.out",
        NULL
    };
    return run_process("./temp.out", args);
}

/*static int run_with_tcc(const char *file){
    char *path=find_path("tcc");
    if(!path) return -1;
    char *args[]={
        "tcc",
        "-run",
        (char *)file,
        NULL
    };
    int ret=run_process(path, args);
    free(path);
    return ret;
}
*/
/*static int run_with_gcc(const char *file){
    char *path=find_path("gcc");
    if(!path) return -1;

    char *compile_args[]={
        path,
        "-c",
        (char *)file,
        "-o",
        "temp.o",
        NULL
    };
    if(run_process(path, compile_args)!=0){
        free(path);
        return -1;
    }
    char *link_args[]={
        path,
        "temp.o",
        "-o",
        "temp.out",
        NULL
    };
    if(run_process(path, link_args)!=0){
        free(path);
        return -1;
    }
    free(path);
    return run_binary();
}*/
char *compiler_path(const char *cmd){
    return find_path(cmd);
}

int run_with_compiler(Compiler *c, const char *file){
    if(strcmp(c->name, "tcc")==0){
        char *args[]={
            "tcc",
            "-run",
            (char *)file,
            NULL
        };
        int ret=run_process(c->path, args);
        return ret;

    }else if(strcmp(c->name, "gcc")==0){
        char *compile_args[]={
            c->path,
            "-c",
            (char *)file,
            "-o",
            "temp.o",
            NULL
        };
        if(run_process(c->path, compile_args)!=0){
            return -1;
        }
        char *link_args[]={
            c->path,
            "temp.o",
            "-o",
            "temp.out",
            NULL
        };
        if(run_process(c->path, link_args)!=0){
            return -1;
        }
        return run_binary();
        }
    return -1;
}

int execute_code(BufferState *s, Compiler *c){
    if(build_c_file(s, "temp.c")!=0) return -1;
    write(STDOUT_FILENO, "\r", 1);
    run_with_compiler(c, "temp.c");
    return 0;
}
