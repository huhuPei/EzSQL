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

typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_EXIT,
    META_COMMAND_INVALID_COMMAND
} MetaCommandResult;

typedef enum { 
    PREPARE_SUCCESS, 
    PREPARE_UNRECOGNIZED_STATEMENT 
} PrepareResult;

typedef enum { 
    STATEMENT_INSERT, 
    STATEMENT_SELECT 
} StatementType;

typedef struct {
  StatementType type;
} Statement;

InputBuffer* new_input_buffer();
void close_input_buffer(InputBuffer*);
void read(InputBuffer*);
void print_prompt();
MetaCommandResult do_meta_command(const char*);
PrepareResult prepare_statement(const char*, Statement*);
void execute_statement(Statement*);

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

MetaCommandResult do_meta_command(const char* command) {
    if (strcmp(command, ":exit") == 0) {
        return META_COMMAND_EXIT;
    } else {
        return META_COMMAND_INVALID_COMMAND;
    }
}

PrepareResult prepare_statement(const char* source, Statement* statement) {
    if (strncmp(source, "insert", 6) == 0) {
        statement->type = STATEMENT_INSERT;
        return PREPARE_SUCCESS;
    }
    if (strcmp(source, "select") == 0) {
        statement->type = STATEMENT_SELECT; 
        return PREPARE_SUCCESS;
    }
    return PREPARE_UNRECOGNIZED_STATEMENT;
}

void execute_statement(Statement* statement) {
    switch (statement->type) {
        case STATEMENT_INSERT:
            printf("This is where we would do an insert.\n");
            break;
        case STATEMENT_SELECT:
            printf("This is where we would do a select.\n");
            break;
    }
}

int main(int argc, char* argv[]) {
    InputBuffer* input_buffer = new_input_buffer();
    bool running = true;
    while (running) {
        print_prompt();
        read(input_buffer);
        if (input_buffer->buffer[0] == ':') {
            switch (do_meta_command(input_buffer->buffer)){
                case META_COMMAND_EXIT:
                    close_input_buffer(input_buffer);
                    printf("Bye.\n");
                    exit(EXIT_SUCCESS);
                case META_COMMAND_SUCCESS:
                    break;
                case META_COMMAND_INVALID_COMMAND:
                    printf("Invalid command '%s'.\n", input_buffer->buffer);
                    break;  
                default: ;;
            }
        } else {
            Statement statement;
            switch (prepare_statement(input_buffer->buffer, &statement)) {
                case PREPARE_SUCCESS:
                    break;
                case PREPARE_UNRECOGNIZED_STATEMENT:
                    printf("Unrecognized keyword at start of '%s'.\n", input_buffer->buffer);
                    continue;
                default: continue;
            }
            execute_statement(&statement);
            printf("Executed.\n");
        }
    }
}