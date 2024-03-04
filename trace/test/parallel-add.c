#include <pthread.h>
#include <stdio.h>

const int n = 10000;
const int m = 10;

int a[n], b[n], c[n];

void *sum(void *arg)
{
    int l = *((int *)arg);
    int r = *((int *)arg + 1);
    for (int i = l; i < r; ++i) {
        c[i] = a[i] + b[i];
    }
}

int main()
{
    for (int i = 0; i < n; ++i) {
        a[i] = i;
        b[i] = n - i;
    }
    pthread_t threads[m];
    int thread_args[m][2];
    for (int i = 0; i < m; ++i) {
        thread_args[i][0] = i * n / m;
        thread_args[i][1] = (i + 1) * n / m;
        pthread_create(&threads[i], NULL, sum, (void *)thread_args[i]);
    }
    for (int i = 0; i < m; ++i) {
        pthread_join(threads[i], NULL);
    }
    for (int i = 0; i < n; ++i) {
        if (c[i] != n) {
            return -1;
        }
    }
    return 0;
}