#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
    // Set up files to test
    const char* test_file1 = "./huffman.c";
    const char* test_file2 = "./stack_queue.c";
    
    // Ensure the analyzer executable exists
    const char* analyzer = "./complexity_analyzer";
    
    // Create command strings
    char command1[256];
    char command2[256];
    
    // Format commands
    sprintf(command1, "%s %s", analyzer, test_file1);
    sprintf(command2, "%s %s", analyzer, test_file2);
    
    printf("Running complexity analysis on test files...\n\n");
    
    // Execute analysis on both files
    printf("===== TEST 1: Huffman Coding Implementation =====\n");
    system(command1);
    
    printf("\n===== TEST 2: Stack and Queue Implementation =====\n");
    system(command2);
    
    return 0;
}