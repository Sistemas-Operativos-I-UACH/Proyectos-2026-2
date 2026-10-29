#include <stdio.h>

int main(int argc, char *argv[]) {
    char file[] = "/etc/passwd";
    int bytes_read = 0;

    char line[255];
    FILE *fptr;

    fptr = fopen(file, "r");
    if ( fptr == NULL ) {
        printf("No pude abrir el archivo %s\n", file);
        return 1;
    }

    while(bytes_read = fread(line, sizeof(char), 100, fptr)) {
        line[bytes_read] = '\0';
        printf("LINE: %s", line);
    }

    fclose(fptr);

    return 0;

}
