#include "./compile.h"
#include "./render.h"
#include "./package.h"
#include "./utils.h"
#include <stdio.h> 
#include <stdlib.h> 

#define USAGE "USAGE: srpt <command> [options] \n\
    commands: compile, render, package\n\
    options: filepath[srpt,sdom]\n"

#define HELP "\033[96;4mHELP\033[0m: srpt <command> [options] \n\
    \033[1msrpt compile <filepath.srpt>\033[0m ; compiles the srpt file given into an sdom binary\n\n\
            --watch,-w ; watches for updates to the srpt source script and recompiles upon change\n\
            --serialize,-s ; toggle serialization, default is TRUE, this will make compile, render pipelines faster if you do them in the same run, but it won't produce a workable sdom, the sdom will only work for the duration of the rendering\
            \n\n\
    \033[1msrpt render <filepath.sdom>\033[0m ; renders a functional UI based on a given sdom binary\n\n\
            --watch,-w ; watches for updates to the sdom binary and rerenders upon change\n\
            \n\n\
    \033[1msrpt package <filepath.sdom>\033[0m ; packages a given sdom binary into an executable along with the scripts it uses and a standalone sdom renderer\n\n\
            --bake,-b ; 'bakes-in' the resources that are referenced in the sdom instead of the default option (storing them in a ./assets folder)\n\
    \n\
    \n\n\
\033[96;4mGeneral\033[0m\n\
    -h ; shows this help message\n\
    -u ; shows a usage message\n"

#define MAX_OPTIONS 6
#define MAX_ARGUMENTS 6

#define NEW_OPTION(OPTION, FUNCTION, ARGUMENT, PTR, LEN) if (*(PTR) == '-' && live_compare((PTR+1), (OPTION), (LEN))) (FUNCTION)((ARGUMENT));
#define NEW_COMMAND(COMMAND, PTR, LEN) if (live_compare((PTR), (COMMAND), (LEN)))
#define COMMAND(COMMAND, PTR) (str_cmp((PTR), (COMMAND)))
#define COMMAND_OPTION(OPTION, PTR, LEN) (*(PTR) == '-' && live_compare((PTR+1), (OPTION), (LEN)))

int main(int argc, char* argv[]) {
    // CLI
    // srpt compile x.srpt -> [x.sdom, ..., z.sdom] : compile.c 
    // srpt render x.sdom -> GUI : render.c
    // srpt package x.sdom -> executable : package.c
    //
    char** arguments = malloc(sizeof(char*)*MAX_ARGUMENTS);
    short /* options_index = 0, */ arguments_index = 0;
    for (int i = 1; i < argc; ++i) {
        //if (argv[i][0] == '-') options[options_index++] = argv[i];
        //else 
        if (argv[i][0] != '-' && str_cmp(argv[i], "compile") != 0 && str_cmp(argv[i], "render") != 0 &&str_cmp(argv[i], "package") != 0){
            arguments[arguments_index++] = argv[i];
        }
    }
    /* options_index = 0; */ arguments_index = 0;
    unsigned int* len = malloc(sizeof(unsigned int));
    for (int i = 1; i < argc; ++i) { 
        // seperate into 2 loops, one for options one for commands
        char* ptr = argv[i];
        bool serialize = TRUE; // global option sets
        // generals;
        NEW_OPTION("h", printf, HELP, ptr, len)
        else NEW_OPTION("u", printf, USAGE, ptr, len)
        NEW_COMMAND("compile", ptr, len) {
            char* output = NULL; bool watch = FALSE; 
            ++i;
            while (i < argc) {
                if (COMMAND_OPTION("o", argv[i], len)) {
                    output = argv[i+1];
                    arguments_index++;
                    ++i; // for output;
                } else if (COMMAND_OPTION("w", argv[i], len) || COMMAND_OPTION("-watch", argv[i], len)) {
                    watch = TRUE;
                } else if (COMMAND_OPTION("s", argv[i], len) || COMMAND_OPTION("-serialize", argv[i], len)) {
                    serialize = FALSE;
                } else if (COMMAND("render", ptr) || COMMAND("package", ptr)) {
                    break;    
                }
                ++i;
            }
            compile(arguments[arguments_index++], output, watch, serialize);

        } else NEW_COMMAND("render", ptr, len) {
            bool watch = FALSE;
            ++i;
            while (i < argc) {
                if (COMMAND_OPTION("w", argv[i], len) || COMMAND_OPTION("-watch", argv[i], len)) {
                    watch = TRUE;
                } else if (COMMAND("package", ptr)) { // no options 
                    break;
                }
                ++i;
            }
            render(arguments[arguments_index++], watch, serialize);
        } else NEW_COMMAND("package", ptr, len) {
            // process options here
            package(arguments[arguments_index++], "output", 0); // to be replaced
        } else NEW_COMMAND("init", ptr, len) {
            // TODO: initialize a new "repo"
        }
    }
    free(arguments);
    free(len);
}
