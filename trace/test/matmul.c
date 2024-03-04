const int n = 20;
int a[n][n], b[n][n], c[n][n];

int main()
{
    // Matrix Multiplication
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            for (int k = 0; k < n; ++k) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    return 0;
}