#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 1024
#define MAX_FUNCTIONS 100
#define MAX_LOOPS 100
#define MAX_VARIABLES 100
#define MAX_TOKENS 1000
#define MAX_FUNC_NAME 64

typedef struct {
    char type[16];
    int line;
    int depth;
} LoopInfo;

typedef struct {
    char name[MAX_FUNC_NAME];
    int start_line;
    int end_line;
    int loop_count;
    int nested_loop_depth;
    int recursive;
    long memory_allocated;
    char time_complexity[32];
    char space_complexity[32];
} FunctionInfo;

typedef struct {
    FunctionInfo functions[MAX_FUNCTIONS];
    int function_count;
    LoopInfo loops[MAX_LOOPS];
    int loop_count;
    char tokens[MAX_TOKENS][64];
    int token_count;
} CodeAnalysis;

void analyze_file(const char* filename, CodeAnalysis* analysis);
void tokenize_line(const char* line, CodeAnalysis* analysis);
void identify_functions(CodeAnalysis* analysis);
void analyze_loops(CodeAnalysis* analysis);
void analyze_recursion(CodeAnalysis* analysis);
void analyze_memory(CodeAnalysis* analysis);
void calculate_complexities(CodeAnalysis* analysis);
void print_results(CodeAnalysis* analysis, const char* filename);
int is_token(const char* token);
int get_current_depth(CodeAnalysis* analysis, int line);

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <c_file1> [c_file2...]\n", argv[0]);
        return 1;
    }
    
    for (int i = 1; i < argc; i++) {
        CodeAnalysis analysis = {0};

        printf("Analyzing %s...\n", argv[i]);
        analyze_file(argv[i], &analysis);
        identify_functions(&analysis);
        analyze_loops(&analysis);
        analyze_recursion(&analysis);
        analyze_memory(&analysis);
        calculate_complexities(&analysis);
        print_results(&analysis, argv[i]); 
    }

    return 0;
}

void analyze_file(const char* filename, CodeAnalysis* analysis) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        exit(1);
    }
    
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        tokenize_line(line, analysis);
    }

    fclose(file);
}

void tokenize_line(const char* line, CodeAnalysis* analysis) {
    char* token = strtok((char*)line, " \t\n\r\f\v(),;{}[]");
    while (token != NULL && analysis->token_count < MAX_TOKENS) {
        if (strlen(token) > 0) {
            strncpy(analysis->tokens[analysis->token_count], token, 63);
            analysis->tokens[analysis->token_count][63] = '\0';
            analysis->token_count++;
        }
        token = strtok(NULL, " \t\n\r\f\v(),;{}[]");
    }
}

void identify_functions(CodeAnalysis* analysis) {
    int in_function = 0;
    int brace_count = 0;
    int func_index = -1;

    for (int i = 0; i < analysis->token_count; i++) {
        if (!in_function && i + 2 < analysis->token_count) {
            if ((strcmp(analysis->tokens[i], "int") == 0 || 
                 strcmp(analysis->tokens[i], "void") == 0 ||
                 strcmp(analysis->tokens[i], "float") == 0 ||
                 strcmp(analysis->tokens[i], "double") == 0 ||
                 strcmp(analysis->tokens[i], "char") == 0) &&
                 strcmp(analysis->tokens[i+2], "(") == 0) {
                    if (analysis->function_count < MAX_FUNCTIONS) {
                        func_index = analysis->function_count;
                        strncpy(analysis->functions[func_index].name, analysis->tokens[i+1], MAX_FUNC_NAME-1);
                        analysis->functions[func_index].name[MAX_FUNC_NAME-1] = '\0';
                        analysis->functions[func_index].start_line = i;
                        analysis->function_count++;
                        in_function = 1;
                    }
            }
        }

        if (in_function) {
            if (strcmp(analysis->tokens[i], "{") == 0) {
                brace_count++;
            } else if (strcmp(analysis->tokens[i], "}") == 0) {
                brace_count--;
                if (brace_count == 0) {
                    analysis->functions[func_index].end_line = i;
                    in_function = 0;
                }
            }
        }
    }
}

void analyze_loops(CodeAnalysis* analysis) {
    int current_depth = 0;

    for (int i = 0; i < analysis->token_count; i++) {
        if (strcmp(analysis->tokens[i], "for") == 0 ||
            strcmp(analysis->tokens[i], "while") == 0) {

            if (analysis->loop_count < MAX_LOOPS) {
                strcpy(analysis->loops[analysis->loop_count].type, analysis->tokens[i]);
                analysis->loops[analysis->loop_count].line = i;
                analysis->loops[analysis->loop_count].depth = current_depth + 1;
                analysis->loop_count++;
                current_depth++;
            }
        }
        else if (strcmp(analysis->tokens[i], "do") == 0 &&
                i + 1 < analysis->token_count &&
                strcmp(analysis->tokens[i+1], "while") == 0) {
            if (analysis->loop_count < MAX_LOOPS) {
                strcpy(analysis->loops[analysis->loop_count].type, "do-while");
                analysis->loops[analysis->loop_count].line = i;
                analysis->loops[analysis->loop_count].depth = current_depth + 1;
                analysis->loop_count++;
                current_depth++;
            }
        }

        if (strcmp(analysis->tokens[i], "}") == 0 && current_depth > 0) {
            current_depth--;
        }
    }

    for (int f = 0; f < analysis->function_count; f++) {
        int max_depth = 0;
        int loop_count = 0;

        for (int l = 0; l < analysis->loop_count; l++) {
            if (analysis->loops[l].line > analysis->functions[f].start_line &&
                analysis->loops[l].line < analysis->functions[f].end_line) {
                    loop_count++;
                    if (analysis->loops[l].depth > max_depth) {
                        max_depth = analysis->loops[l].depth;
                    }
            }
        }

        analysis->functions[f].loop_count = loop_count;
        analysis->functions[f].nested_loop_depth = max_depth;
    }
}    

void analyze_recursion(CodeAnalysis* analysis) {
    for (int f = 0; f < analysis->function_count; f++) {
        analysis->functions[f].recursive = 0;

        for (int i = analysis->functions[f].start_line; i < analysis->functions[f].end_line; i++) {
            if (strcmp(analysis->tokens[i], analysis->functions[f].name) == 0) {
                analysis->functions[f].recursive = 1;
                break;
            }
        }
    }
}

void analyze_memory(CodeAnalysis* analysis) {
    for (int f = 0; f < analysis->function_count; f++) {
        analysis->functions[f].memory_allocated = 0;
        
        for (int i = analysis->functions[f].start_line; i < analysis->functions[f].end_line; i++) {
            if (strcmp(analysis->tokens[i], "malloc") == 0 ||
                strcmp(analysis->tokens[i], "calloc") == 0) {
                    analysis->functions[f].memory_allocated += 1;
            }
        }
    }
}

void calculate_complexities(CodeAnalysis* analysis) {
    for (int f = 0; f < analysis->function_count; f++) {
        if (analysis->functions[f].recursive) {
            strcpy(analysis->functions[f].time_complexity, "O(2^n)");
        } else if (analysis->functions[f].nested_loop_depth > 0) {
            if (analysis->functions[f].nested_loop_depth == 1) {
                strcpy(analysis->functions[f].time_complexity, "O(n)");
            } else if (analysis->functions[f].nested_loop_depth == 2) {
                strcpy(analysis->functions[f].time_complexity, "O(n^2)");
            } else if (analysis->functions[f].nested_loop_depth == 3) {
                strcpy(analysis->functions[f].time_complexity, "O(n^3)");
            } else {
                sprintf(analysis->functions[f].time_complexity, "O(n^%d)", 
                        analysis->functions[f].nested_loop_depth);
            }
        } else {
            strcpy(analysis->functions[f].time_complexity, "O(1)");
        }

        if (analysis->functions[f].recursive) {
            strcpy(analysis->functions[f].space_complexity, "O(n)");
        } else if (analysis->functions[f].memory_allocated > 0) {
            if (analysis->functions[f].nested_loop_depth > 0) {
                strcpy(analysis->functions[f].space_complexity, "O(n)");
            } else {
                strcpy(analysis->functions[f].space_complexity, "O(1)");
            }
        } else {
            strcpy(analysis->functions[f].space_complexity, "O(1)");
        }
    }   
}

void print_results(CodeAnalysis* analysis, const char* filename) {
    printf("\nAnalysis results for %s:\n", filename);
    printf("================================\n");
    printf("%-20s %-20s %-20s %-20s\n", "Function", "Time Complexity", "Space Complexity", "Memory Allocated");
    printf("--------------------------------\n");

    for (int f = 0; f < analysis->function_count; f++) {
        printf("%-20s %-20s %-20s %-20ld\n", 
               analysis->functions[f].name,
               analysis->functions[f].time_complexity, 
               analysis->functions[f].space_complexity,
               analysis->functions[f].memory_allocated);
    }

    printf("================================\n");
    printf("Actual Complexities May Vary Due to Actual Implementation\n");
}

int is_token(const char* token) {
    return strlen(token) > 0 && !isspace((unsigned char)token[0]);
}

int get_current_depth(CodeAnalysis* analysis, int line) {
    int depth = 0;
    for (int i = 0; i < analysis->loop_count; i++) {
        if (analysis->loops[i].line < line) {
            depth = analysis->loops[i].depth;
        }
    }
    return depth;
}