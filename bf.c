#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>
typedef uint8_t u8;

const wchar_t ESC = 0x001b;

enum {TAPE_LEN = 30000};
u8 tape[TAPE_LEN];
int ptr = 0;
const char* CODE =
"[[-]+]\0";
int rdr = 0;

typedef struct {
    int* array;
    size_t size;
    size_t max_size;
} Stack;

Stack* initStack(size_t max_size) {
    Stack* stack = malloc(sizeof(Stack));
    stack->size = 0;
    stack->max_size = max_size;
    stack->array = malloc(sizeof(int) * stack->max_size);
    return stack;
}

int pushItem(Stack* stack, int item) {
    stack->size++;
    if (stack->size > stack-> max_size) {
        stack->size--;
        return 0;
    }
    stack->array[stack->size - 1] = item;
    return 1;
}

int popItem(Stack* stack) {
    if (stack->size == 0) {
        return 0;
    }
    stack->array[stack->size - 1] = 0;
    stack->size--;
    return 1;
}

int is_empty_stack(Stack* stack) {
    return stack->size == 0;
}

int getStackTop(Stack* stack) {
    return stack->array[stack->size - 1];
}

int getStackBottom(Stack* stack) {
    return stack->array[0];
}

void printFirstNCells(int n, int columns) {
    //printf("%lc[1E", ESC);
    for (int i = 0; i < n; i++) {
        printf("%d ", tape[i]);
    }
    printf("\n");
    //printf("%lc[1F%lc[%dC", ESC, ESC, columns);
}

void skip(Stack* stack) {
    int skipBreakpoint = stack->size - 1;
    printf("Stack is %ld/%d; rdr=%d on %c\n", stack->size, skipBreakpoint, rdr, CODE[rdr]);
    while (stack->size > skipBreakpoint) {
        rdr++;
        if (CODE[rdr] == '[') {
            pushItem(stack, rdr);
        } else if (CODE[rdr] == ']') {
            popItem(stack);
        }
        printf("Stack is %ld/%d; rdr=%d on %c\n", stack->size, skipBreakpoint, rdr, CODE[rdr]);
        fflush(stdout);
    }
    rdr++;
}

int main(){
    //Zeroing all items on the tape
    for (int i = 0; i < TAPE_LEN; i++) {
        tape[i] = 0;
    }

    Stack* activeBrackets = initStack(500);
    
    //Getting the length of the code
    int CODE_LEN = 0;
    while (CODE[CODE_LEN] != '\0') {
        CODE_LEN++;
    }
    
    //Verifying that all brackets are matched
    for (int i = 0; i < CODE_LEN; i++) {
         if (CODE[i] == '[') {
             pushItem(activeBrackets, i);
         } else if (CODE[i] == ']') {
            if (!popItem(activeBrackets)) {
                printf("Unmatched closing bracket at %d", i);
            }
        }
    }
    if (!is_empty_stack(activeBrackets)) {
        printf("Unmatched opening bracket at %d", getStackTop(activeBrackets));
    }

    int charsPrinted = 0;
    int upperbracketskip = 0;
    while (rdr != CODE_LEN) {
        printFirstNCells(30, charsPrinted);
        //printf("\r");
        fflush(stdout);
        char current = CODE[rdr];
        char input[5];
        int to_write;
        rdr++;
        
        if (current == '[') {
            pushItem(activeBrackets, rdr);
            if (tape[ptr] == 0) {
                skip(activeBrackets);
            }
        } else if (current == ']') {
            if (tape[ptr] != 0) {
                rdr = getStackTop(activeBrackets);
            }
                popItem(activeBrackets);
        } else if (!is_empty_stack(activeBrackets) && skip) {
            continue;        
        } else if (current == '+') {
            if (tape[ptr] < 255) {
            tape[ptr]++;
            } else {
                tape[ptr] = 0;
            }
        } else if (current == '-') {
            if (tape[ptr] > 0) {
            tape[ptr]--;
            } else {
                tape[ptr] = 255;
            }
        } else if (current == '>') {
            ptr++;
            if (ptr >= TAPE_LEN) {
                ptr = 0;
            }
        } else if (current == '<') {
            ptr--;
            if (ptr <= -1) {
                ptr = TAPE_LEN - 1;
            }
        } else if (current == '.') {
            printf("%c", tape[ptr]);
            //printf("Chars printed: %d", charsPrinted);
            fflush(stdout);
        } else if (current == ',') {
            fgets(input, sizeof(input), stdin);
            sscanf(input, "%d", &to_write);
            tape[ptr] = to_write % 256;
        }
        usleep(5000000);
    }
    printFirstNCells(30, charsPrinted);
}