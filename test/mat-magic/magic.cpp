#include <bits/stdc++.h>
using namespace std;

const int n = 100;
int a[n][n], b[n][n], c[n][n];

int main()
{
    // Matrix Multiplication
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i - 1 >= 0 && j + 1 < n ) c[i][j] += a[i - 1][j] * b[i][j + 1];
            if (i + 1 < n  && j + 1 < n ) c[i][j] += a[i][j + 1] * b[i + 1][j];
            if (i + 1 < n  && j - 1 >= 0) c[i][j] += a[i + 1][j] * b[i][j - 1];
            if (i - 1 >= 0 && j + 1 >= 0) c[i][j] += a[i][j - 1] * b[i - 1][j];
        }
    }
    return 0;
}