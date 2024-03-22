/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    //for 32*32 m < 300
    if(M == 32 && N == 32)
    {int i, j, k, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8;
    for (i = 0; i < N; i+=8) {
        for (j = 0; j < M; j+=8) {
            for(k = 0; k < 8; ++k) {
                tmp1 = A[i+k][j];
                tmp2 = A[i+k][j+1];
                tmp3 = A[i+k][j+2];
                tmp4 = A[i+k][j+3];
                tmp5 = A[i+k][j+4];
                tmp6 = A[i+k][j+5];
                tmp7 = A[i+k][j+6];
                tmp8 = A[i+k][j+7];
                B[j][i+k] = tmp1;
                B[j+1][i+k] = tmp2;
                B[j+2][i+k] = tmp3;
                B[j+3][i+k] = tmp4;
                B[j+4][i+k] = tmp5;
                B[j+5][i+k] = tmp6;
                B[j+6][i+k] = tmp7;
                B[j+7][i+k] = tmp8;
            }
        }
    } 
    }
    
    //for 64*64 m < 1300
    else if(M == 64  && N == 64)
    {int i, j, k, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8;
    for (i = 0; i < N; i+=8) {
        for (j = 0; j < M; j+=4) {
            if(i != j ) {
                for(k = 0; k < 4; ++k) {
                    B[j+k][i] = A[i+k][j];
                    B[j+k][i+1] = A[i+k][j+1];
                    B[j+k][i+2] = A[i+k][j+2];
                    B[j+k][i+3] = A[i+k][j+3];
                    tmp1 = A[i+k][j+4];
                    tmp2 = A[i+k][j+5];
                    tmp3 = A[i+k][j+6];
                    tmp4 = A[i+k][j+7];
                    B[j+k][i+4] = A[i+k+4][j];
                    B[j+k][i+5] = A[i+k+4][j+1];
                    B[j+k][i+6] = A[i+k+4][j+2];
                    B[j+k][i+7] = A[i+k+4][j+3];
                    B[j+k+4][i] = tmp1;
                    B[j+k+4][i+1] = tmp2;
                    B[j+k+4][i+2] = tmp3;
                    B[j+k+4][i+3] = tmp4;
                    B[j+k+4][i+4] = A[i+k+4][j+4];
                    B[j+k+4][i+5] = A[i+k+4][j+5];
                    B[j+k+4][i+6] = A[i+k+4][j+6];
                    B[j+k+4][i+7] = A[i+k+4][j+7];
                }
             
                j += 4;
                tmp1 = B[j][i+1];
                tmp2 = B[j][i+2];
                tmp3 = B[j][i+3];
                tmp4 = B[j+1][i+2];
                tmp5 = B[j+1][i+3];
                tmp6 = B[j+2][i+3];
                B[j][i+1] = B[j+1][i];
                B[j][i+2] = B[j+2][i];
                B[j][i+3] = B[j+3][i];
                B[j+1][i+2] = B[j+2][i+1];
                B[j+1][i+3] = B[j+3][i+1];
                B[j+2][i+3] = B[j+3][i+2];
                B[j+1][i] = tmp1;
                B[j+2][i] = tmp2;
                B[j+3][i] = tmp3;
                B[j+2][i+1] = tmp4;
                B[j+3][i+1] = tmp5;
                B[j+3][i+2] = tmp6;
                
                tmp1 = B[j][i+5];
                tmp2 = B[j][i+6];
                tmp3 = B[j][i+7];
                tmp4 = B[j+1][i+6];
                tmp5 = B[j+1][i+7];
                tmp6 = B[j+2][i+7];
                B[j][i+5] = B[j+1][i+4];
                B[j][i+6] = B[j+2][i+4];
                B[j][i+7] = B[j+3][i+4];
                B[j+1][i+6] = B[j+2][i+5];
                B[j+1][i+7] = B[j+3][i+5];
                B[j+2][i+7] = B[j+3][i+6];
                B[j+1][i+4] = tmp1;
                B[j+2][i+4] = tmp2;
                B[j+3][i+4] = tmp3;
                B[j+2][i+5] = tmp4;
                B[j+3][i+5] = tmp5;
                B[j+3][i+6] = tmp6;
                
                j -= 4;
                tmp1 = B[j][i+1];
                tmp2 = B[j][i+2];
                tmp3 = B[j][i+3];
                tmp4 = B[j+1][i+2];
                tmp5 = B[j+1][i+3];
                tmp6 = B[j+2][i+3];
                B[j][i+1] = B[j+1][i];
                B[j][i+2] = B[j+2][i];
                B[j][i+3] = B[j+3][i];
                B[j+1][i+2] = B[j+2][i+1];
                B[j+1][i+3] = B[j+3][i+1];
                B[j+2][i+3] = B[j+3][i+2];
                B[j+1][i] = tmp1;
                B[j+2][i] = tmp2;
                B[j+3][i] = tmp3;
                B[j+2][i+1] = tmp4;
                B[j+3][i+1] = tmp5;
                B[j+3][i+2] = tmp6;
                
                tmp1 = B[j][i+5];
                tmp2 = B[j][i+6];
                tmp3 = B[j][i+7];
                tmp4 = B[j+1][i+6];
                tmp5 = B[j+1][i+7];
                tmp6 = B[j+2][i+7];
                B[j][i+5] = B[j+1][i+4];
                B[j][i+6] = B[j+2][i+4];
                B[j][i+7] = B[j+3][i+4];
                B[j+1][i+6] = B[j+2][i+5];
                B[j+1][i+7] = B[j+3][i+5];
                B[j+2][i+7] = B[j+3][i+6];
                B[j+1][i+4] = tmp1;
                B[j+2][i+4] = tmp2;
                B[j+3][i+4] = tmp3;
                B[j+2][i+5] = tmp4;
                B[j+3][i+5] = tmp5;
                B[j+3][i+6] = tmp6;
                
                j += 4;
            }
        else  {
                for(k = 0; k < 3; ++k) {
                    B[j+k][i] = A[i+k+1][j];
                    B[j+k][i+1] = A[i+k+1][j+1];
                    B[j+k][i+2] = A[i+k+1][j+2];
                    B[j+k][i+3] = A[i+k+1][j+3];
                    tmp1 = A[i+k+1][j+4];
                    tmp2 = A[i+k+1][j+5];
                    tmp3 = A[i+k+1][j+6];
                    tmp4 = A[i+k+1][j+7];
                    B[j+k][i+4] = A[i+k+4+1][j];
                    B[j+k][i+5] = A[i+k+4+1][j+1];
                    B[j+k][i+6] = A[i+k+4+1][j+2];
                    B[j+k][i+7] = A[i+k+4+1][j+3];
                    B[j+k+4][i] = tmp1;
                    B[j+k+4][i+1] = tmp2;
                    B[j+k+4][i+2] = tmp3;
                    B[j+k+4][i+3] = tmp4;
                    B[j+k+4][i+4] = A[i+k+4+1][j+4];
                    B[j+k+4][i+5] = A[i+k+4+1][j+5];
                    B[j+k+4][i+6] = A[i+k+4+1][j+6];
                    B[j+k+4][i+7] = A[i+k+4+1][j+7];
                }
                B[j+3][i] = A[i][j];
                B[j+3][i+1] = A[i][j+1];
                B[j+3][i+2] = A[i][j+2];
                B[j+3][i+3] = A[i][j+3];
                tmp1 = A[i][j+4];
                tmp2 = A[i][j+5];
                tmp3 = A[i][j+6];
                tmp4 = A[i][j+7];
                B[j+3][i+4] = A[i + 4][j];
                B[j+3][i+5] = A[i + 4][j+1];
                B[j+3][i+6] = A[i + 4][j+2];
                B[j+3][i+7] = A[i + 4][j+3];
                B[j+7][i] = tmp1;
                B[j+7][i+1] = tmp2;
                B[j+7][i+2] = tmp3;
                B[j+7][i+3] = tmp4;
                B[j+7][i+4] = A[i + 4][j+4];
                B[j+7][i+5] = A[i + 4][j+5];
                B[j+7][i+6] = A[i + 4][j+6];
                B[j+7][i+7] = A[i + 4][j+7];
                
                //now row 1234 -> 2 3 4 1
                j += 4;
                tmp1 = B[j+3][i];
                tmp2 = B[j+3][i+1];
                tmp3 = B[j+3][i+2];
                tmp4 = B[j+3][i+3];
                tmp5 = B[j+3][i+4];
                tmp6 = B[j+3][i+5];
                tmp7 = B[j+3][i+6];
                tmp8 = B[j+3][i+7];
                for(k = 3; k > 0; --k) {
                    B[j+k][i] = B[j+k-1][i];
                    B[j+k][i+1] = B[j+k-1][i+1];
                    B[j+k][i+2] = B[j+k-1][i+2];
                    B[j+k][i+3] = B[j+k-1][i+3];
                    B[j+k][i+4] = B[j+k-1][i+4];
                    B[j+k][i+5] = B[j+k-1][i+5];
                    B[j+k][i+6] = B[j+k-1][i+6];
                    B[j+k][i+7] = B[j+k-1][i+7];
                }
                B[j][i] = tmp1;
                B[j][i+1] = tmp2;
                B[j][i+2] = tmp3;
                B[j][i+3] = tmp4;
                B[j][i+4] = tmp5;
                B[j][i+5] = tmp6;
                B[j][i+6] = tmp7;
                B[j][i+7] = tmp8;
                
                tmp1 = B[j][i+1];
                tmp2 = B[j][i+2];
                tmp3 = B[j][i+3];
                tmp4 = B[j+1][i+2];
                tmp5 = B[j+1][i+3];
                tmp6 = B[j+2][i+3];
                B[j][i+1] = B[j+1][i];
                B[j][i+2] = B[j+2][i];
                B[j][i+3] = B[j+3][i];
                B[j+1][i+2] = B[j+2][i+1];
                B[j+1][i+3] = B[j+3][i+1];
                B[j+2][i+3] = B[j+3][i+2];
                B[j+1][i] = tmp1;
                B[j+2][i] = tmp2;
                B[j+3][i] = tmp3;
                B[j+2][i+1] = tmp4;
                B[j+3][i+1] = tmp5;
                B[j+3][i+2] = tmp6;
                
                tmp1 = B[j][i+5];
                tmp2 = B[j][i+6];
                tmp3 = B[j][i+7];
                tmp4 = B[j+1][i+6];
                tmp5 = B[j+1][i+7];
                tmp6 = B[j+2][i+7];
                B[j][i+5] = B[j+1][i+4];
                B[j][i+6] = B[j+2][i+4];
                B[j][i+7] = B[j+3][i+4];
                B[j+1][i+6] = B[j+2][i+5];
                B[j+1][i+7] = B[j+3][i+5];
                B[j+2][i+7] = B[j+3][i+6];
                B[j+1][i+4] = tmp1;
                B[j+2][i+4] = tmp2;
                B[j+3][i+4] = tmp3;
                B[j+2][i+5] = tmp4;
                B[j+3][i+5] = tmp5;
                B[j+3][i+6] = tmp6;
                
                j -= 4;
                tmp1 = B[j+3][i];
                tmp2 = B[j+3][i+1];
                tmp3 = B[j+3][i+2];
                tmp4 = B[j+3][i+3];
                tmp5 = B[j+3][i+4];
                tmp6 = B[j+3][i+5];
                tmp7 = B[j+3][i+6];
                tmp8 = B[j+3][i+7];
                for(k = 3; k > 0; --k) {
                    B[j+k][i] = B[j+k-1][i];
                    B[j+k][i+1] = B[j+k-1][i+1];
                    B[j+k][i+2] = B[j+k-1][i+2];
                    B[j+k][i+3] = B[j+k-1][i+3];
                    B[j+k][i+4] = B[j+k-1][i+4];
                    B[j+k][i+5] = B[j+k-1][i+5];
                    B[j+k][i+6] = B[j+k-1][i+6];
                    B[j+k][i+7] = B[j+k-1][i+7];
                }
                B[j][i] = tmp1;
                B[j][i+1] = tmp2;
                B[j][i+2] = tmp3;
                B[j][i+3] = tmp4;
                B[j][i+4] = tmp5;
                B[j][i+5] = tmp6;
                B[j][i+6] = tmp7;
                B[j][i+7] = tmp8;
                
                tmp1 = B[j][i+1];
                tmp2 = B[j][i+2];
                tmp3 = B[j][i+3];
                tmp4 = B[j+1][i+2];
                tmp5 = B[j+1][i+3];
                tmp6 = B[j+2][i+3];
                B[j][i+1] = B[j+1][i];
                B[j][i+2] = B[j+2][i];
                B[j][i+3] = B[j+3][i];
                B[j+1][i+2] = B[j+2][i+1];
                B[j+1][i+3] = B[j+3][i+1];
                B[j+2][i+3] = B[j+3][i+2];
                B[j+1][i] = tmp1;
                B[j+2][i] = tmp2;
                B[j+3][i] = tmp3;
                B[j+2][i+1] = tmp4;
                B[j+3][i+1] = tmp5;
                B[j+3][i+2] = tmp6;
                
                tmp1 = B[j][i+5];
                tmp2 = B[j][i+6];
                tmp3 = B[j][i+7];
                tmp4 = B[j+1][i+6];
                tmp5 = B[j+1][i+7];
                tmp6 = B[j+2][i+7];
                B[j][i+5] = B[j+1][i+4];
                B[j][i+6] = B[j+2][i+4];
                B[j][i+7] = B[j+3][i+4];
                B[j+1][i+6] = B[j+2][i+5];
                B[j+1][i+7] = B[j+3][i+5];
                B[j+2][i+7] = B[j+3][i+6];
                B[j+1][i+4] = tmp1;
                B[j+2][i+4] = tmp2;
                B[j+3][i+4] = tmp3;
                B[j+2][i+5] = tmp4;
                B[j+3][i+5] = tmp5;
                B[j+3][i+6] = tmp6;
                
                j += 4;
        }
        
        }
    }
    }

    
    
    //for 61*67 m < 2000
    else if(M == 61 && N == 67)
    {int i, j, k, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8, tmp9, tmp10, tmp11, tmp12;
    int dim = 12;
    // A :67*61
    for(i = 0; i < 60; i+=dim){
        for(j = 0; j < 60; j+=dim){
            for(k = 0; k < dim; ++k){
                tmp1 = A[i+k][j];
                tmp2 = A[i+k][j+1];
                tmp3 = A[i+k][j+2];
                tmp4 = A[i+k][j+3];
                tmp5 = A[i+k][j+4];
                tmp6 = A[i+k][j+5];
                tmp7 = A[i+k][j+6];
                tmp8 = A[i+k][j+7];
                tmp9 = A[i+k][j+8];
                tmp10 = A[i+k][j+9];
                tmp11 = A[i+k][j+10];
                tmp12 = A[i+k][j+11];
                B[j][i+k] = tmp1;
                B[j+1][i+k] = tmp2;
                B[j+2][i+k] = tmp3;
                B[j+3][i+k] = tmp4;
                B[j+4][i+k] = tmp5;
                B[j+5][i+k] = tmp6;
                B[j+6][i+k] = tmp7;
                B[j+7][i+k] = tmp8;
                B[j+8][i+k] = tmp9;
                B[j+9][i+k] = tmp10;
                B[j+10][i+k] = tmp11;
                B[j+11][i+k] = tmp12;
            }
            
        }
    }
    //i = 66;
    int times = 67 - i;
    for(j = 0; j < 60; j+=dim){
        for(k = 0; k < times; ++k){
            tmp1 = A[i+k][j];
            tmp2 = A[i+k][j+1];
            tmp3 = A[i+k][j+2];
            tmp4 = A[i+k][j+3];
            tmp5 = A[i+k][j+4];
            tmp6 = A[i+k][j+5];
            tmp7 = A[i+k][j+6];
            tmp8 = A[i+k][j+7];
            tmp9 = A[i+k][j+8];
            tmp10 = A[i+k][j+9];
            tmp11 = A[i+k][j+10];
            tmp12 = A[i+k][j+11];
            B[j][i+k] = tmp1;
            B[j+1][i+k] = tmp2;
            B[j+2][i+k] = tmp3;
            B[j+3][i+k] = tmp4;
            B[j+4][i+k] = tmp5;
            B[j+5][i+k] = tmp6;
            B[j+6][i+k] = tmp7;
            B[j+7][i+k] = tmp8;
            B[j+8][i+k] = tmp9;
            B[j+9][i+k] = tmp10;
            B[j+10][i+k] = tmp11;
            B[j+11][i+k] = tmp12;
        }
        
    }
    for(i = 0; i < 67; ++i){
        /*B[54][i] = A[i][54];
        B[55][i] = A[i][55];
        B[56][i] = A[i][56];
        B[57][i] = A[i][57];
        B[58][i] = A[i][58];
        B[59][i] = A[i][59];*/
        B[60][i] = A[i][60];
    }
    }

    
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

