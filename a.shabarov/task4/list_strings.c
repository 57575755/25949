#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Определение узла связного списка
typedef struct Node {
    char *data;         // Указатель на динамически выделенную строку
    struct Node *next;  // Указатель на следующий элемент списка
} Node;

// Функция добавления строки в конец списка
void append(Node **head, const char *str) {
    // 1. Выделяем память под сам узел
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL) {
        perror("malloc node");
        exit(EXIT_FAILURE);
    }

    // 2. Выделяем память под строку (+1 байт под завершающий нуль '\0')
    new_node->data = (char *)malloc(strlen(str) + 1);
    if (new_node->data == NULL) {
        perror("malloc data");
        free(new_node);
        exit(EXIT_FAILURE);
    }
    strcpy(new_node->data, str);
    new_node->next = NULL;

    // 3. Вставляем узел в список
    if (*head == NULL) {
        *head = new_node;
    } else {
        Node *curr = *head;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = new_node;
    }
}

// Функция освобождения памяти всего списка
void free_list(Node *head) {
    Node *curr = head;
    while (curr != NULL) {
        Node *temp = curr;
        curr = curr->next;
        free(temp->data); // Сначала освобождаем саму строку
        free(temp);       // Затем освобождаем узел
    }
}

int main(void) {
    Node *head = NULL;
    char buffer[1024];

    printf("Введите строки (введите точку '.' для завершения):\n");

    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        // Условие выхода: строка начинается с точки
        if (buffer[0] == '.') {
            break;
        }

        // Удаляем завершающий символ переноса строки '\n'
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        append(&head, buffer);
    }

    // Вывод элементов списка
    printf("\n=== Список введенных строк ===\n");
    Node *curr = head;
    while (curr != NULL) {
        printf("%s\n", curr->data);
        curr = curr->next;
    }

    // Обязательная очистка памяти
    free_list(head);

    return 0;
}
