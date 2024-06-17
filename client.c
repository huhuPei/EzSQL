#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "table.h"

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
    PREPARE_NEGATIVE_ID,
    PREPARE_SYNTAX_ERROR,
    PREPARE_STRING_TOO_LONG, 
    PREPARE_UNRECOGNIZED_STATEMENT 
} PrepareResult;

typedef enum { 
    EXECUTE_SUCCESS, 
    EXECUTE_TABLE_FULL 
} ExecuteResult;

typedef enum { 
    STATEMENT_INSERT, 
    STATEMENT_SELECT 
} StatementType;

typedef struct {
  StatementType type;
  Row row_data;
} Statement;

InputBuffer* new_input_buffer();
void close_input_buffer(InputBuffer*);
void read_stdin(InputBuffer*);
void print_prompt();
MetaCommandResult do_meta_command(const char*);
PrepareResult prepare_insert_statement(char*, Statement*);
PrepareResult prepare_statement(char*, Statement*);
ExecuteResult execute_statement(Statement*, Table*);
ExecuteResult execute_insert(Statement*, Table*);
ExecuteResult execute_select(Statement*, Table*);

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

void read_stdin(InputBuffer* input_buffer) {
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

PrepareResult prepare_insert_statement(char* sql_expr, Statement* statement) {
    statement->type = STATEMENT_INSERT;
    char* keyword = strtok(sql_expr, " ");
    char* id_str = strtok(NULL, " ");
    char* username = strtok(NULL, " ");
    char* email = strtok(NULL, " ");
    if (id_str == NULL || username == NULL || email == NULL) {
        return PREPARE_SYNTAX_ERROR;
    }
    if (strlen(username) > COLUMN_USERNAME_SIZE) {
        return PREPARE_STRING_TOO_LONG;
    }
    if (strlen(email) > COLUMN_EMAIL_SIZE) {
        return PREPARE_STRING_TOO_LONG;
    }

    int id = atoi(id_str);
    if (id < 0) { return PREPARE_NEGATIVE_ID; }
    statement->row_data.id = id;
    strcpy(statement->row_data.username, username);
    strcpy(statement->row_data.email, email);
    return PREPARE_SUCCESS;
}

PrepareResult prepare_statement(char* sql_expr, Statement* statement) {
    if (strncmp(sql_expr, "insert", 6) == 0) {
        return prepare_insert_statement(sql_expr, statement);
    }
    if (strcmp(sql_expr, "select") == 0) {
        statement->type = STATEMENT_SELECT; 
        return PREPARE_SUCCESS;
    }
    return PREPARE_UNRECOGNIZED_STATEMENT;
}

ExecuteResult execute_statement(Statement* statement, Table* table) {
    switch (statement->type) {
        case STATEMENT_INSERT:
            return execute_insert(statement, table);
        case STATEMENT_SELECT:
            return execute_select(statement, table);
    }
}

ExecuteResult execute_insert(Statement* statement, Table* table) {
    if (table_full(table)) {
        return EXECUTE_TABLE_FULL;
    }
    Row* row_data = &(statement->row_data);
    void* dest = locate_row(table, table->num_rows);
    serialize_row(row_data, dest);
    table->num_rows += 1;
    return EXECUTE_SUCCESS;
}

ExecuteResult execute_select(Statement* statement, Table* table) {
    Row row;
    for (uint32_t i = 0; i < table->num_rows; i++) {
        deserialize_row(locate_row(table, i), &row);
        print_row(&row);
    }
    return EXECUTE_SUCCESS;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Must supply a database filename.\n");
        exit(EXIT_FAILURE);
    }
    char* filename = argv[1];
    Table* table = open_db(filename);
    InputBuffer* input_buffer = new_input_buffer();
    bool running = true;
    while (running) {
        print_prompt();
        read_stdin(input_buffer);
        if (input_buffer->buffer[0] == ':') {
            switch (do_meta_command(input_buffer->buffer)){
                case META_COMMAND_EXIT:
                    close_input_buffer(input_buffer);
                    close_db(table);
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
                case PREPARE_STRING_TOO_LONG:
                    printf("String is too long.\n");
                    continue;
                case PREPARE_NEGATIVE_ID:
                    printf("ID must be positive.\n");
                    continue;
                case PREPARE_SYNTAX_ERROR:
	                printf("Syntax error. Could not parse statement.\n");
	                continue;
                case PREPARE_UNRECOGNIZED_STATEMENT:
                    printf("Unrecognized keyword at start of '%s'.\n", input_buffer->buffer);
                    continue;
                default: continue;
            }
            ExecuteResult exec_result = execute_statement(&statement, table);
            switch (exec_result) {
                case (EXECUTE_SUCCESS):
        	        printf("Executed.\n");
                    break;
                case (EXECUTE_TABLE_FULL):
	                printf("Error: Table full.\n");
	                break;
            }
        }
    }
}