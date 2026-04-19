#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/wait.h>
#include<unistd.h>
#include"state.h"
#include"colors.h"
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

int build_c_file(BufferState *s, const char *filename){
    FILE *f=fopen(filename, "w");
    if(!f){
        perror("fopen");
        return-1;
    }

    if(s->globals.data && s->globals.len>0){
        fprintf(f, "%s\n", s->globals.data);
    }
    fprintf(f, "int main() {\n");
    if(s->main_body.data && s->main_body.len>0){
        fprintf(f, "%s", s->main_body.data);
    }
    if(s->main_body.len>0 && s->main_body.data[s->main_body.len-1]!='\n'){
        fprintf(f, "\n");
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

static int run_with_tcc(const char *file){
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

static int run_with_gcc(const char *file){
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
}

int execute_code(BufferState *s){
    if(build_c_file(s, "temp.c")!=0) return -1;
    write(STDOUT_FILENO, "\r", 1);
    if(run_with_tcc("temp.c")==0){
        //write(STDOUT_FILENO, "\r", 1);
        printf("\r" C_GREEN"[ran with tcc]"C_RESET "\n");
        return 0;
    }
    printf("\r" C_YELLOW"[tcc failed or not found, using gcc]"C_RESET "\n");

    if(run_with_gcc("temp.c")==0){
        printf("\r[ran with gcc]\n");
        return 0;
    }
    printf("\r" IKC_ERROR"[no working compiler found]"C_RESET "\n");
    return -1;
}
