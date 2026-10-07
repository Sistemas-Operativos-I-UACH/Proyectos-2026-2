#include<stdio.h>

void main(void) {
    char buf1[17] = "01234567890ABCDEF";
    char buf2[256];
    char *buf3;
    buf2[0] = 'G';
    printf("BUF1 %s\n", buf1);
    printf("BUF2 %s\n", buf2);
    //printf("BUF1[17] + 1 %p\n", (buf1 - sizeof(char)));
    printf("Dirección de buf1 %p\n", buf1);
    printf("Dirección de buf1 %ld\n", buf1);
    printf("Dirección de buf2 %p\n", buf2);
    printf("Dirección de buf2 %ld\n", buf2);
    printf("offset de buf2 - buf1 %ld\n", buf1 - buf2);
    buf3 = buf1;
    buf3[1] = '\0';
    printf("buf3: -%s-\n", buf3);
    printf("buf1: -%s-\n", buf1);

}
