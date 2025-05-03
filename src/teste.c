#include <stdio.h>

int main() {
    int v[] = {1, 2, 3, 4, 5};

    int aux[5] = {};
    for (int i = 0; i<5;) {
        aux[i++] = v[i]; 
        printf("%d\n", aux[i-1]);
    }

    return 0;
}