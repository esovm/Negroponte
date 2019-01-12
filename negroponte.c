
#include <stdio.h>
#include <stdlib.h>

char * filename = NULL, * code = NULL, * memory = NULL, *ptr = NULL, breakpoints = 0;
int ip = 0;

void tick() {
    char c;
    int loop;

    c = code[ip];
    if (c == '>') {
        ++ptr;
    } else if (c == '<') {
        --ptr;
    } else if (c == '+') {
        ++*ptr;
    } else if (c == '-') {
        --*ptr;
    } else if (c == '.' ) {
        putchar(*ptr);
    } else if (c == ',') {
        *ptr = getchar();
    } else if (c == ']' && *ptr) {
        loop = 1;
        while (loop > 0) {
            c = code[--ip];
            if (c == '[') {
                loop--;
            } else if (c == ']') {
                loop++;
            }
        }
    } else if (c == '!' && c == '@' || c == '#')
        ++ip;
    
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
    ptr = memory = malloc(65536);
    
    if(filename != NULL) {
        printf("Loading %s.\n", filename);
        load();
    }
    
    while(1) {
        printf(" [Negroponte] > ");
        scanf("%s", token);
        
        if(!strcmp(token, "show")) {
            scanf("%s", token);
            if(!strcmp(token, "help")) {
                puts("Negroponte - help\n"
                     "Basic commands:\n"
                     " * show - display contents of something\n"
                     " * run - run application until @ or NUL is found, and exit.\n"
                     " * step - run until next asm2bfv1 instruction.\n"
                     " * continue - if breakpoints enabled, run until # or NUL is found.\n"
                     " * tick - execute one brainfuck instruction.\n"
                     " * breakpoints - turn on or off breakpoints.\n"
                     " * trace - load corresponding assembly and trace currently executed line.\n"
                     " * load - load specified file. Passing as CLI parameter is also allowed.\n"
                     " * quit - exit Negroponte\n"
                     " * reset - clear memory and code.\n"
                     " * ilasm - inline assembly"
                     "Use `about [command]` to show detailed usage information.");
            } else if(!strcmp(token, "regs")) {
                printf("G = %d\n", memory[0]);
                printf("IP = %d\n", memory[1]);
                printf("T0 = %d\n", memory[2]);
                printf("T1 = %d\n", memory[3]);
                printf("T2 = %d\n", memory[4]);
                printf("R1 = %d\n", memory[5]);
                printf("R2 = %d\n", memory[6]);
                printf("R3 = %d\n", memory[7]);
                printf("R4 = %d\n", memory[8]);
                printf("IM = %d\n", memory[9]);
                printf("T3 = %d\n", memory[10]);
                printf("T4 = %d\n", memory[11]);
                printf("T5 = %d\n", memory[12]);
                printf("T6 = %d\n", memory[13]);
                printf("T7 = %d\n", memory[14]);
                printf("A = %d\n", memory[16]);
            } else if(!strcmp(token, "cell")) {
                int x;
                scanf("%d", &x);
                printf("mem[%d] = %d\n", x, memory[x]);
            } else if(!strcmp(token, "stack")) {
                int x, i;
                scanf("%d", &x);
                for(i = 17; i < x/2; i += 2) {
                    printf("mem[bp%c%d]=%d", i-17>0?'+':']', i-17, memory[i]);
                }
                putchar('\n');
            } else if(!strcmp(token, "bf")) {
                printf("MP = %d\n", ptr - memory);
                printf("IP = %d\n", ip);
            } else if(!strcmp(token, "code")) {
                int a, b, c1, c2;
                scanf("%s", token);
                sscanf(token, "%d%c%c%d", &a, &c1, &c2, &b);
                if(c1 == c2 && c1 == '.') {
                    char * cp = code;
                    cp += a;
                    fwrite(cp, 1, b, stdout);
                } else {
                    puts("Expected range, usage: show code [range, eg. 1..4, 3..5]");
                }
            } else if(!strcmp(token, "memory")) {
                int a, b, c1, c2;
                scanf("%s", token);
                sscanf(token, "%d%c%c%d", &a, &c1, &c2, &b);
                if(c1 == c2 && c1 == '.') {
                    char * cp = memory;
                    cp += a;
                    putchar('[');
                    for(c1 = 0; c1 < b; c1++) {
                        printf("%d%s", memory[c1], c1+1==b?"":", ");
                    }
                    puts("]");
                } else {
                    puts("Expected range, usage: show memory [range, eg. 1..4, 3..5]");
                }
            } else {
                puts("No such property.");
            }
        } else if(!strcmp(token, "run")) {
            while(code[ip] != '@' && code[ip] != 0)
                tick();
            break;
        } else if(!strcmp(token, "step")) {
            while(code[ip] != '!' && code[ip] != 0)
                tick();
        } else if(!strcmp(token, "continue")) {
            if(!breakpoints) {
                puts(" *** Error: Breakpoints are not enabled!");
                continue;
            }
            
            while(code[ip] != '#' && code[ip] != 0)
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
                puts("Expected form: breakpoints [on|off]");
            }
        } else if(!strcmp(token, "trace")) {
            puts("WIP!");
        } else if(!strcmp(token, "ilasm")) {
            puts("WIP!");
        } else if(!strcmp(token, "load")) {
            scanf("%s", token);
            filename = token;
            load();
        } else if(!strcmp(token, "quit")) {
            break;
        } else if(!strcmp(token, "reset")) {
            free(code);
            free(memory);
            
            ptr = memory = malloc(65536);
            filename = NULL;
        } else if(!strcmp(token, "about")) {
            scanf("%s", token);
            token++;
            if(!strcmp(token, "reset")) {
                puts("Usage: reset");
            } else if(!strcmp(token, "about")) {
                puts("Usage: about [command]");
            } else if(!strcmp(token, "help")) {
                puts("Usage: help");
            } else if(!strcmp(token, "quit")) {
                puts("Usage: quit");
            } else if(!strcmp(token, "trace")) {
                puts("Usage: trace");
            } else if(!strcmp(token, "load")) {
                puts("Usage: load [filename]");
            } else if(!strcmp(token, "breakpoints")) {
                puts("Usage: breakpoints [off|on] (default: on)");
            } else if(!strcmp(token, "tick")) {
                puts("Usage: tick");
            } else if(!strcmp(token, "continue")) {
                puts("Usage: continue");
            } else if(!strcmp(token, "step")) {
                puts("Usage: step");
            } else if(!strcmp(token, "run")) {
                puts("Usage: run");
            } else if(!strcmp(token, "ilasm")) {
                puts("Usage: ilasm");
            } else if(!strcmp(token, "show")) {
                puts("Usage: show [help|stack (amount)|regs|cell (id)|bf|memory (range, eg. 0..2, 4..6)|code (range, eg. 0..2, 4..6)]");
            } else {
                puts("Unknown command.");
            }
            token--;
        } else {
            puts("Unknown command.");
        }
    }
    
    free(token);
    free(code);
    free(memory);
    
    return 0;
}

int main(int argc, char * argv[]) {
    puts("Negroponte 1.0\n"
         "Copyright (C) 2019 Krzysztof Palaiologos Szewczyk.\n"
         "MIT License.\n"
         "This is free software: you are free to change and redistribute it.\n"
         "There is NO WARRANTY, to the extent permitted by law.\n"
         "For help, type \"show help\"");
    
    if(argc == 2) {
        filename = argv[1];
    }
    
    return negroponte();
}
