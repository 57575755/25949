#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

void print_uids(const char *stage) {
    printf("[%s] Real UID: %d, Effective UID: %d\n", stage, getuid(), geteuid());
}

void try_open(const char *filename, const char *stage) {
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        fprintf(stderr, "[%s] Ошибка открытия '%s': ", stage, filename);
        perror("");
    } else {
        printf("[%s] Успех! Файл '%s' открыт.\n", stage, filename);
        fclose(f);
    }
}

int main(void) {
    const char *filename = "data.txt";

    printf("=== 1. ДО СБРОСА ПРИВИЛЕГИЙ ===\n");
    print_uids("Старт");
    try_open(filename, "Попытка 1");

    printf("\n--> Сбрасываем привилегии: setuid(getuid())...\n");
    if (setuid(getuid()) == -1) {
        perror("setuid");
    }

    printf("\n=== 2. ПОСЛЕ СБРОСА ПРИВИЛЕГИЙ ===\n");
    print_uids("Финиш");
    try_open(filename, "Попытка 2");

    return 0;
}
