#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

/* Markers used to bound trace regions of interest */
volatile char MARKER_START, MARKER_END;

int main(int argc, char* argv[])
{

    #define min(a, b) (((a) < (b)) ? (a) : (b))


    /* Record marker addresses */
    FILE* marker_fp = fopen(".marker","w");
    assert(marker_fp);
    fprintf(marker_fp, "%llx\n%llx",
        (unsigned long long int) &MARKER_START,
        (unsigned long long int) &MARKER_END );
    fclose(marker_fp);

    if (argc!=3) {
        printf("Usage: ./matMul <matrix_a_file> <matrix_b_file>\n");
        exit(EXIT_FAILURE);
    }

    FILE* matrix_a_fp = fopen(argv[1], "r");
    if (!matrix_a_fp) {
        perror("fopen failed");
        return EXIT_FAILURE;
    }
    size_t n;
    int ret = fscanf(matrix_a_fp, "%ld\n", &n);
    assert (ret==1);
    int* a = calloc( n*n, sizeof(int) );
    for ( size_t i=0; i<n; i++ ) {
        for ( size_t k=0; k<n; k++ ) {
            ret = fscanf(matrix_a_fp, "%d ", &a[i*n+k]);
            assert (ret==1);
        }
        ret = fscanf(matrix_a_fp, "\n");
    }
    fclose(matrix_a_fp);

    FILE* matrix_b_fp = fopen(argv[2], "r");
    if (!matrix_b_fp) {
        perror("fopen failed");
        return EXIT_FAILURE;
    }
    size_t m;
    ret = fscanf(matrix_b_fp, "%ld\n", &m);
    assert (ret==1);
    assert( n==m );
    int* b = calloc( n*n, sizeof(int) );
    for ( size_t k=0; k<n; k++ ) {
        for ( size_t j=0; j<n; j++ ) {
            ret = fscanf(matrix_b_fp, "%d ", &b[k*n+j]);
            assert (ret==1);
        }
        ret = fscanf(matrix_b_fp, "\n");
    }
    fclose(matrix_b_fp);

    int* c = calloc( n*n, sizeof(int) );

    MARKER_START = 211;

    double tmp;
    int block_size=4; 
    
    for (int k = 0; k < n; k += block_size){ 
                for (int i = 0; i < n; i += block_size){ 
                        for (int j = 0; j < n; j += block_size){ 
                                for (int kk = k; kk < min(k + block_size, n); kk++){
                                        for (int ii = i; ii < min(i + block_size, n); ii++) {
                                                tmp = a[ii * n + kk];
                                                for (int jj = j; jj < min(j + block_size, n); jj++) { 
                                                       c[ii * n + jj] += tmp * b[kk * n + jj];
                                                }
                                        }
                                }
                        }
                }
    }

    /*// kij
    for ( size_t k=0; k<n; k++ ) {
        for ( size_t i=0; i<n; i++ ) {
            int r = a[i*n+k];
            //printf("r: %d\n", r);
            for ( size_t j=0; j<n; j++ ) {
                c[i*n+j] += r * b[k*n+j];
              //  printf("b: %d\n", b[k*n+j]);
             //   printf("c: %d\n", c[i*n+j]);
            }
        }
    }*/

    // ijk
    // for ( size_t i=0; i<n; i++ ) {
    //     for ( size_t j=0; j<n; j++ ) {
    //         int sum = c[i*n+j];
    //         for ( size_t k=0; k<n; k++ ) {
    //             sum += a[i*n+k] * b[k*n+j];
    //         }
    //         c[i*n+j] = sum;
    //     }
    // }

    // jki
    // for ( size_t j=0; j<n; j++ ) {
    //     for ( size_t k=0; k<n; k++ ) {
    //         int r = b[k*n+j];
    //         for ( size_t i=0; i<n; i++ ) {
    //             c[i*n+j] += a[i*n+k] * r;
    //         }
    //     }
    // }


    MARKER_END = 211;

    for ( size_t i=0; i<n; i++ ) {
        for ( size_t j=0; j<n; j++ ) {
            printf( "%d ", c[i*n+j] );
        }
        printf( "\n" );
    }

    free(c);
    free(b);
    free(a);
    exit(EXIT_SUCCESS);

}
