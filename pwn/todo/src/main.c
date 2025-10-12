#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_STRING_LENGTH 128
#define MAX_TODOS 32

typedef struct {
    char   str[MAX_STRING_LENGTH];
    size_t length;
} String;

typedef struct {
    String todos[MAX_TODOS];
    size_t count;
} TodoList;



void ignoreBuffering(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stdin,  NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
}

void menu(void) {
    puts("\n==== TODO LIST MENU ====");
    puts("[0] Add a new todo");
    puts("[1] View a todo");
    puts("[2] Delete a todo");
    puts("[3] Edit a todo");
    puts("[4] Show all todos");
    puts("[5] Exit");
}

void printString(const String *s) {
    if (!s) return;
    if (s->length) write(STDOUT_FILENO, s->str, s->length);
}

long readInt(const char *prompt) {
    char buf[64];
    printf("%s", prompt);
    if (!fgets(buf, sizeof buf, stdin)) return -1;
    char *end = NULL;
    long v = strtol(buf, &end, 10);
    return v;
}

void getInput(String* s, ssize_t size) {
    if (!s) return;
    for (int i = 0; i <= size; i++) {
        s->length = i;
        ssize_t n = read(STDIN_FILENO, s->str + i, 1);
        if (n <= 0 || s->str[i] == '\n') return;
    }
}

void createTodoItem(TodoList *list) {
    if (!list) return;
    if (list->count >= MAX_TODOS) {
        puts("Your todo list is full. Please delete an item before adding more.");
        return;
    }
    String newTodo = {0};
    printf("Enter your new todo (max %d chars): ", MAX_STRING_LENGTH);
    getInput(&newTodo, MAX_STRING_LENGTH);
    list->todos[list->count++] = newTodo;
    puts("Added! Your todo was saved.");
}

void printTodoItem(const TodoList *list) {
    if (!list) return;
    if (list->count == 0) {
        puts("You have no todos yet!");
        return;
    }
    long idx = readInt("Enter the index of the todo to view: ");
    if (idx < 0 || (size_t)idx >= list->count) {
        puts("Invalid index. Please try again.");
        return;
    }
    printf("\nTodo #%ld: ", idx);
    printString(&list->todos[idx]);
    puts("");
}

void deleteTodoItem(TodoList *list) {
    if (!list) return;
    if (list->count == 0) {
        puts("No todos to delete. Your list is empty!\n");
        return;
    }
    long idx = readInt("Enter the index of the todo to delete: ");
    if (idx < 0 || (size_t)idx >= list->count) {
        puts("Invalid index. Please try again.\n");
        return;
    }
    for (size_t i = (size_t)idx; i + 1 < list->count; i++) {
        list->todos[i] = list->todos[i + 1];
    }
    list->count--;
    puts("Todo deleted successfully!\n");
}

void editTodoItem(TodoList *list) {
    if (!list) return;
    if (list->count == 0) {
        puts("No todos to edit. Your list is empty!\n");
        return;
    }
    long idx = readInt("Enter the index of the todo to edit: ");
    if (idx < 0 || (size_t)idx >= list->count) {
        puts("Invalid index. Please try again.\n");
        return;
    }
    String edited = {0};
    edited.length = list->todos[(size_t)idx].length;
    printf("Enter the new text for todo #%ld (max %ld chars): ", idx, edited.length);
    getInput(&edited, edited.length);
    list->todos[idx] = edited;
    puts("Todo updated.");
}

void printTodoList(const TodoList *list) {
    if (!list) return;
    if (list->count == 0) {
        puts("You have no todos yet! Add one to get started.\n");
        return;
    }
    puts("\n==== ALL TODOS ====");
    for (size_t i = 0; i < list->count; i++) {
        printf("[%zu] ", i);
        printString(&list->todos[i]);
        puts("");
    }
}

int main(void) {
    ignoreBuffering();
    TodoList list = {0};
    while (1)
    {
        menu();
        long choice = readInt("Enter your choice: ");
        switch (choice) {
            case 0: createTodoItem(&list);  break;
            case 1: printTodoItem(&list);   break;
            case 2: deleteTodoItem(&list);  break;
            case 3: editTodoItem(&list);    break;
            case 4: printTodoList(&list);   break;
            case 5: _exit(0);
            default: puts("Invalid choice. Please try again.");
        }
    }
}
