#include <stdio.h>        // printf, fprintf, scanf
#include <stdlib.h>       // malloc, realloc, free, exit
#include <unistd.h>       // read, lseek, close
#include <fcntl.h>        // open, O_RDONLY

typedef struct {
    long offset;   // Позиция начала строки в файле (байт от начала)
    int length;    // Длина строки (количество символов)
} LineInfo;

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    int capacity = 16;
    int num_lines = 0;
    LineInfo *table = malloc(capacity * sizeof(LineInfo));
    if (table == NULL) {
        perror("malloc");
        close(fd);
        return 1;
    }

    long offset = 0; //начало текущей строки
    int length = 0; //длина текущей строки
    char c;

    while (read(fd, &c, 1) == 1) {
        if (c == '\n') {
            //Конец строки - записываем в таблицу
            if (num_lines >= capacity) {
                capacity *= 2;
                LineInfo *new_table = realloc(table, capacity * sizeof(LineInfo));
                if (new_table == NULL) {
                    perror("realloc");
                    free(table);
                    close(fd);
                    return 1;
                }
                table = new_table;
            }
            table[num_lines].offset = offset;
            table[num_lines].length = length;
            num_lines++;

            //Следующая строка начинается после \n 
            offset = lseek(fd, 0L, SEEK_CUR);
            length = 0;
        }
        else {
            length++;
        }
    }

    //Последняя строка без \n 
    if (length > 0) {
        if (num_lines >= capacity) {
            capacity++;
            LineInfo *new_table = realloc(table, capacity * sizeof(LineInfo));
            if (new_table == NULL) {
                perror("realloc");
                free(table);
                close(fd);
                return 1;
            }
            table = new_table;
        }
        table[num_lines].offset = offset;
        table[num_lines].length = length;
        num_lines++;
    }

    printf("--- Debug: Line Table ---\n");
    for (int i = 0; i < num_lines; i++) {
        printf("Line %d: Offset = %ld, Length = %d\n",
               i + 1, table[i].offset, table[i].length);
    }
    printf("-------------------------\n");

    int n;
    while (1) {
        printf("Enter line number (0 to quit): ");

        if (scanf("%d", &n) != 1) {
            break;
        }

        if (n == 0) {
            break;
        }

        if (n < 1 || n > num_lines) {
            printf("Invalid line number (1..%d)\n", num_lines);
            continue;
        }

        int idx = n - 1; //Индекс в массиве

        if (lseek(fd, table[idx].offset, SEEK_SET) == -1) {
            perror("lseek");
            continue;
        }

        //Читаем ровно length байт
        char *buf = malloc(table[idx].length + 1);
        if (buf == NULL) {
            perror("malloc");
            break;
        }

        ssize_t got = read(fd, buf, table[idx].length);
        if (got == -1) {
            perror("read");
            free(buf);
            continue;
        }

        buf[got] = '\0';
        printf("%s\n", buf);

        free(buf);
    }
    
    free(table);
    close(fd);

    return 0;
}