
#include <stdio.h>
#include <stdlib.h>

char * filename = NULL, * code = NULL, * memory = NULL, *ptr = NULL, breakpoints = 0;
int ip = 0;

void tick() {
    char c;
    int loop;

    c = code[ip];
    if (c == '>') {
/* asm2bf
 *
 * Copyright (C) Krzysztof Palaiologos Szewczyk, 2019.
 * License: MIT
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 * Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BASE_POINTER 17

char * src;
long lof, ip = -1;

void match(int dir) {
    int level = 1;
    char ipvalue;
    do {
        ip += dir;
        if (ip < 0 || ip >= lof) {
            puts("Mismatched brackets");
            exit(EXIT_FAILURE);
        }
        ipvalue = src[ip];
        if (ipvalue == 91)
            level += dir;
        else if (ipvalue == 93)
            level -= dir;
    } while (level > 0);
}

int main(int argc, char * argv[]) {
    FILE * infile;
    long mp = 0, maxmp = 1023;
    int n;
    unsigned short int * mem;
    if (argc != 2 || argv[1][0] == '-' || argv[1][0] == '/') {
        puts("Usage: bfi src.b");
        return EXIT_FAILURE;
    }
    infile = fopen(argv[1], "rb");
    if (infile == NULL) {
        puts("Error opening input file");
        return EXIT_FAILURE;
    }
    if (fseek(infile, 0L, SEEK_END) != 0) {
        puts("Error determining length of input file");
        return EXIT_FAILURE;
    }
    lof = ftell(infile);
    if (lof == -1) {
        puts("Error determining length of input file");
        return EXIT_FAILURE;
    }
    if (fseek(infile, 0L, SEEK_SET) != 0) {
        puts("Error determining length of input file");
        return EXIT_FAILURE;
    }
    if (lof == 0)
        return EXIT_SUCCESS;
    src = (char *) calloc(lof + 2, sizeof(char));
    if (src == NULL) {
        puts("Program too big to fit in memory");
        return EXIT_FAILURE;
    }
    if (fread(src, sizeof(char), lof, infile) < (unsigned) lof) {
        puts("Error reading input file");
        return EXIT_FAILURE;
    }
    if (fclose(infile) == -1) {
        puts("Error closing input file");
        return EXIT_FAILURE;
    }
    mem = calloc(1024, sizeof(unsigned short int));
    if (mem == NULL) {
        puts("Out of memory");
        return EXIT_FAILURE;
    }
    while (++ip < lof) {
        switch (src[ip]) {
            case '>':
                if (mp >= maxmp) {
                    mem = realloc(mem, (maxmp + 1024) * sizeof(unsigned short int));
                    if (mem == NULL) {
                        puts("Out of memory");
                        return EXIT_FAILURE;
                    }
                    for (n = 1; n <= 1024; n++)
                        mem[maxmp + n] = 0;
                    maxmp += 1024;
                }
                mp++;
                break;
            case '<':
                if (mp <= 0) {
                    printf("Access Violation, ip=%d", ip);
                    return EXIT_FAILURE;
                }
                mp--;
                break;
            case '+':
                mem[mp]++;
                break;
            case '-':
                mem[mp]--;
                break;
            case '.':
                putchar(mem[mp]);
                break;
            case ',':
                n = getchar();
                if (n < 0)
                    n = 0;
                mem[mp] = n;
                break;
            case '[':
                if (src[ip + 1] == '-' && src[ip + 2] == ']') {
                    mem[mp] = 0;
                    ip += 2;
                } else if (mem[mp] == 0)
                    match(1);
                break;
            case ']':
                match(-1);
                ip--;
                break;
			case '*': {
				const char nul[16] = {0};
				char * ptr = mem + BASE_POINTER, a = 0;
				fprintf(stderr, "--- BEGIN REGISTER DUMP ---\n");
				fprintf(stderr, "G  = %02X\t", mem[0]);
				fprintf(stderr, "IP = %02X\n", mem[1]);
				fprintf(stderr, "T0 = %02X\t", mem[2]);
				fprintf(stderr, "T1 = %02X\n", mem[3]);
				fprintf(stderr, "T2 = %02X\t", mem[4]);
				fprintf(stderr, "R1 = %02X\n", mem[5]);
				fprintf(stderr, "R2 = %02X\t", mem[6]);
				fprintf(stderr, "R3 = %02X\n", mem[7]);
				fprintf(stderr, "R4 = %02X\t", mem[8]);
				fprintf(stderr, "IM = %02X\n", mem[9]);
				fprintf(stderr, "T3 = %02X\t", mem[10]);
				fprintf(stderr, "T4 = %02X\n", mem[11]);
				fprintf(stderr, "T5 = %02X\t", mem[12]);
				fprintf(stderr, "T6 = %02X\n", mem[13]);
				fprintf(stderr, "T7 = %02X\t", mem[14]);
				fprintf(stderr, "A  = %02X\n", mem[15]);
				fprintf(stderr, "--- BEGIN STACK DUMP ---\n");
				fprintf(stderr, "BP: %d\n", BASE_POINTER);
				while(*ptr == 1 && ptr[1] == 0) {
					ptr += 2;
					fprintf(stderr, "[%02X] = *(BP + %02X) = %02X\n", ptr - mem, ptr - mem - BASE_POINTER, *ptr);
					ptr += 2;
				}
				fprintf(stderr, "--- BEGIN MEMORY DUMP ---\n");
				while(1) {
					if(!memcmp(nul, ptr, 16))
						break;
					for(a = 0; a < 16; a++)
						fprintf(stderr, "%02X ", *ptr++);
					ptr -= 16;
					for(a = 0; a < 16; a++, ptr++)
						fprintf(stderr, "%c", isprint(*ptr)?*ptr:'.');
					fprintf(stderr, "\n");
				}
				fprintf(stderr, "Found empty block, quitting.\n");
				fprintf(stderr, "--- END ---");
				getchar();
			}
        }
    }
    return EXIT_SUCCESS;
}

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
