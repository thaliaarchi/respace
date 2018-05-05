#include <stdlib.h>
#include <stdio.h>

void main() {
    int* heap = (int*)malloc(sizeof(int) * 10);
    heap[0] = ' ';
    heap[1] = 'b';
    heap[2] = 'o';
    heap[3] = 't';
    heap[4] = 't';
    heap[5] = 'l';
    heap[6] = 'e';
    heap[7] = 's';
    heap[8] = '\n';
    heap[9] = 0;

    int count = 99;
    label_0:
        printf("%d", count);

        int i = 0;
        label_1:
            if (heap[i] == 0)
                goto label_2;
            printf("%c", heap[i]);
            i++;
        goto label_1;

        label_2:

        count--;
        if (count < 0)
            goto label_3;
    goto label_0;

    label_3:
    return;
}
