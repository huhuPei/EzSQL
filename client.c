#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
    char* buffer;
    size_t buffer_size;
    ssize_t input_size;
    /* data */
} InputBuffer;

InputBuffer* new_input_buffer();
void close_input_buffer(InputBuffer*);
void read(InputBuffer*);
void print_prompt();

InputBuffer* new_input_buffer() {
    InputBuffer* input_buffer = (InputBuffer*) malloc(sizeof(InputBuffer));
    input_buffer->buffer_size = 0;
    input_buffer->input_size = 0;
    input_buffer->buffer = NULL;    
    return input_buffer;
}


void close_input_buffer(InputBuffer* input_buffer) {
    free(input_buffer->buffer);
    free(input_buffer);
}

void read(InputBuffer* input_buffer) {
    ssize_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_size), stdin);
    if (bytes_read <= 0) {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }
    input_buffer->input_size = bytes_read - 1;
    input_buffer->buffer[bytes_read - 1] = '\0';
}
void print_prompt() { printf("db > "); }

int main(int argc, char* argv[]) {
    InputBuffer* input_buffer = new_input_buffer();
    while (true) {
        print_prompt();
        read(input_buffer);         
        if (strcmp(input_buffer->buffer, ":exit") == 0) {
            close_input_buffer(input_buffer);
            printf("Bye.\n");
            exit(EXIT_SUCCESS);
        } else {
            printf("Invalid command '%s'.\n", input_buffer->buffer);
        }  
    }
}