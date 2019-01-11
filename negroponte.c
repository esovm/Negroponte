
#include <stdio.h>
#include <stdlib.h>

char * filename = NULL, * code = NULL, * memory = NULL, breakpoints = 0;
int ip = 0;

void tick() {
    char c;
    int loop;

    c = code[ip];
    if (c == '>') {
        ++memory;
    } else if (c == '<') {
        --memory;
    } else if (c == '+') {
        ++*memory;
    } else if (c == '-') {
        --*memory;
    } else if (c == '.' ) {
        putchar(*memory);
    } else if (c == ',') {
        *memory = getchar();
    } else if (c == ']' && *memory) {
        loop = 1;
        while (loop > 0) {
            c = code[--ip];
            if (c == '[') {
                loop--;
            } else if (c == ']') {
                loop++;
            }
        }
    }
    
    ++ip;
}

void load(void) {
    long size;
    FILE * f = fopen(filename, "r");
    
    if(!f) {
        printf(" *** Error: Could not load %s!\n", filename);
        return;
    }
    
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    rewind(f);

    code = malloc(size + 1);
    
    if(!code) {
        printf(" *** Error: Out of memory!");
        return;
    }
    
    fread(code, size, 1, f);
    fclose(f);

    code[size] = 0;
    
    if(!(size < 4 ? 0 : strncmp("+>+[", code, 4) == 0)) {
        printf(" *** Error: This brainfuck file wasn't made using asm2bfv1, you may run to trouble!\n");
    }
}

int negroponte(void) {
    char * token = malloc(64);
    
    if(filename != NULL) {
        printf("Loading %s.\n", filename);
        load();
    }
    
    while(1) {
        printf(" [Negroponte] > ");
        scanf("%s", token);
        
        if(!strcmp(token, "show")) {
            
        } else if(!strcmp(token, "run")) {
            while(code[ip] != '@' && code[ip] != 0)
                tick();
            break;
        } else if(!strcmp(token, "step")) {
            while(code[ip] != '!')
                tick();
        } else if(!strcmp(token, "continue")) {
            if(!breakpoints) {
                puts("Error: Breakpoints are not enabled!");
                continue;
            }
            
            while(code[ip] != '#')
                tick();
        } else if(!strcmp(token, "tick")) {
            tick();
        } else if(!strcmp(token, "breakpoints")) {
            scanf("%s", token);
            if(!strcmp(token, "on")) {
                breakpoints = 1;
            } else if(!strcmp(token, "off")) {
                breakpoints = 0;
            } else {
                puts("Expected form: breakpoints [on|off]\n");
            }
        } else if(!strcmp(token, "trace")) {
            
        } else if(!strcmp(token, "load")) {
            fgets(token, 63, stdin);
            filename = token;
            load();
        } else if(!strcmp(token, "quit")) {
            break;
        } else if(!strcmp(token, "reset")) {
            code = NULL;
            filename = NULL;
        } else {
            puts("Unknown command.\n");
        }
    }
    
    free(token);
}

int main(int argc, char * argv[]) {
    puts("Negroponte 1.0\nCopyright (C) 2019 Krzysztof Palaiologos Szewczyk.\nMIT License.\nThis is free software: you are free to change and redistribute it.\nThere is NO WARRANTY, to the extent permitted by law.\nFor help, type \"show help\"\n");
    
    if(argc == 2) {
        filename = argv[1];
    }
    
    return negroponte();
}
