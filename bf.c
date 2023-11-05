#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>
typedef int8_t i8;

const wchar_t ESC = 0x001b;

enum {TAPE_LEN = 30000};
i8 tape[TAPE_LEN];
int ptr = 0;
const char* CODE =
    "+++++++++++>+>>>>++++++++++++++++++++++++++++++++++++++++++++>++++++++++++++++++++++++++++++++<<<<<<[>[>>>>>>+>+<<<<<<<-]>>>>>>>[<<<<<<<+>>>>>>>-]<[>++++++++++[-<-[>>+>+<<<-]>>>[<<<+>>>-]+<[>[-]<[-]]>[<<[>>>+<<<-]>>[-]]<<]>>>[>>+>+<<<-]>>>[<<<+>>>-]+<[>[-]<[-]]>[<<+>>[-]]<<<<<<<]>>>>>[++++++++++++++++++++++++++++++++++++++++++++++++.[-]]++++++++++<[->-<]>++++++++++++++++++++++++++++++++++++++++++++++++.[-]<<<<<<<<<<<<[>>>+>+<<<<-]>>>>[<<<<+>>>>-]<-[>>.>.<<<[-]]<<[>>+>+<<<-]>>>[<<<+>>>-]<<[<+>-]>[<+>-]<<<-]\0";
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
    if (!stack->size) {
        printf("Could not get item\n");
    }
    return stack->array[stack->size - 1];
}

int getStackBottom(Stack* stack) {
    return stack->array[0];
}

void printFirstNCells(int n, int columns) {
    printf("%lc[1B", ESC);
    for (int i = 0; i < n; i++) {
        printf("%d ", tape[i]);
    }
    printf("%lc[1F%lc[%dC", ESC, ESC, columns);
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

    int charsPrinted = 1;
    int skip = 0;
    int upperbracketskip = 0;
    while (rdr != CODE_LEN) {
        printFirstNCells(30, charsPrinted);
        printf("\r");
        fflush(stdout);
        char current = CODE[rdr];
        char input[5];
        int to_write;
        rdr++;
        
        if (current == '[') {
            pushItem(activeBrackets, rdr);
            if (tape[ptr] == 0) {
                skip = 1;
                upperbracketskip = rdr;
            }
        } else if (current == ']') {
            if (tape[ptr] != 0) {
                rdr = getStackTop(activeBrackets);
            } else {
                if (getStackTop(activeBrackets) == upperbracketskip) {
                    skip = 0;
                }
                popItem(activeBrackets);
            }
        } else if (!is_empty_stack(activeBrackets) && skip) {
            continue;        
        } else if (current == '+') {
            tape[ptr]++;
        } else if (current == '-') {
            tape[ptr]--;
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
            charsPrinted++;
            fflush(stdout);
        } else if (current == ',') {
            fgets(input, sizeof(input), stdin);
            sscanf(input, "%d", &to_write);
            tape[ptr] = to_write % 256;
        }
        usleep(5000);
    }
}