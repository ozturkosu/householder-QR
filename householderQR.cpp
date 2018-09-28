#include <cmath>
#include <stdlib.h>
#include <stdio.h>
#include "linearalgebra.h"

#define MAXDEGREE 24
#define MAXPOINTS 48
#define MAXMATRIXSIZE  1152
#define MAXRSIZE  576

//Emin
/* ----------------------- householder ----------------------- */
/*  Given a matrix A of dimension m by n (with n <= m) and
    arrays v_i of dimension m-i, for i = 1, ..., m - 1,
    respectively, this algorithm computes n reflection vectors
    and the factor R of a full QR decomposition of A, where R
    is a m by n upper triangular matrix. The n reflection
    vectors are stored in the arrays v_1, ..., v_n and the
    columns of A are overwritten by the columns of R.

    Input variables:
        a: pointer to array of arrays, the ith array of
            which should correspond to the ith column of the
            matrix A. During the algorithm, the columns of R
            will overwrite the columns of A.
        v: pointer to array of arrays in which the ith
            reflection vector of dimension m - i will be
            stored.
        m: number of rows of A.
        n: number of columns of A.

    Features: The number of flops for this implementation is
    ~ 2 * m * n^2 - (2/3) * n^3 and requires O(1) additional
    memory.                                                    */

void householder (double ** a, double ** v,double * b , int m, int n) {
    int i, j;
    double vnorm, vTa, vpartdot;
    double bTa;

    for(i = 0; i < n; i++) {
        /* set v[i] equal to subvector a[i][i : m] */
        partialvec_copy(a[i], v[i], m - i, i);

        /* vpartdot = ||v[i]||^2 - v[i][0] * v[i][0]; since vpartdot
           is unaffected by the change in v[i][0], storing this value
           prevents the need to recalculate the entire norm of v[i]
           after updating v[i][0] in the following step              */
        vpartdot = partialdot_product(v[i], v[i], m - i, 1);

        /* set v[i][0] = v[i][0] + sign(v[i][0]) * ||v[i]|| */
        if(v[i][0] < 0) {
            v[i][0] -= sqrt(v[i][0] * v[i][0] + vpartdot);
        }
        else {
            v[i][0] += sqrt(v[i][0] * v[i][0] + vpartdot);
        }

        /* normalize v[i] */
        vnorm = sqrt(v[i][0] * v[i][0] + vpartdot);
        scalar_div(v[i], vnorm, m - i, v[i]);

        for(j = i; j < n; j++) {
            /* set a[j][i:m] = a[j][i:m] - 2 * (v[i]^T a[j][i:m]) * v[i] */
            vTa = subdot_product(a[j], v[i], m - i, i);
            vTa *= 2;
            partialscalar_sub(v[i], vTa, m - i, i, a[j]);
        }


        // Implicit Calculation of a Product Q*b
        // bk:m  =bk:m - 2vk(vk^Tbk:m)
        bTa = subdot_product(b, v[i] , m-i , i) ;
        bTa *= 2;
        partialscalar_sub(v[i],bTa , m-i ,i  , b) ;





    }
}


int main () {
    int i, j, m, n;
    double x;

    double ts[48];
    double dtemp[24];


    /* Arrays for storing error and timing results */
    double err_svd[MAXDEGREE-2] ;
    double times_svd[MAXDEGREE-2];

    FILE *fid;

    /* let user set the dimension of matrix A */
    //printf("Enter the dimension m (where A is a m by n matrix): ");
    //scanf("%i", &m);
    //printf("Enter the dimension n (where A is a m by n matrix): ");
    //scanf("%i", &n);

    /* check if m < n */
    /*
    if(m < n) {
        printf("For a successful factorization, this implementation "
               "requires n <= m.\nTerminating program.\n");
        return 0;
    }
    */
    for (n = 4; n <=MAXDEGREE; n=n+2) {
      /* code */
        m= 2*n ;

        /* allocate memory for A and vectors v */
        double ** a = new double*[n];
        double ** v = new double*[n];
        for(i = 0; i < n; i++) {
            a[i] = new double[m];
            v[i] = new double[m - i];
        }

        double * b = new double[m];
        double * xm = new double[m];



          /* Compute Vandermonde matrix A and right-hand side b */
          for ( i = 0; i < m; i++) {
              ts[i] = (((double)i)) / (((double)(m - 1)));

              a[0][i] = 1.0 ;
              b[i] = 1.0;
          }

          for (j=1; j<n; j++) {
              for (i=0; i<m; i++) {
                  //aConv[j][i] = aConv[j-1][i] * ts[i];
                  a[j][i] = a[j-1][i]*ts[i] ;
                  b[i] = 1.0 +  b[i] * ts[i];
              }
          }


        /* print the matrix A before calling houheholder */
        printf("A = \n");
        for(i = 0; i < m; i++) {
            for(j = 0; j < n; j++) {

                printf("%9.6g ", a[j][i]);
            }
            printf("\n");
        }
        printf("\n");

        //Print b before householder
        printf("b = \n");
        for(i = 0; i < m; i++) {
                printf("%9.6g ", b[i]);

        }
        printf("\n");

        householder(a, v, b, m, n);

        /* print the matrix R (stored in A) after calling houheholder */
        printf("R = \n");
        for(i = 0; i < m; i++) {
            for(j = 0; j < n; j++) {
                printf("%9.6g ", a[j][i]);
            }
            printf("\n");
        }
        printf("\n");

        //After householder
        printf("b = \n");
        for(i = 0; i < m; i++) {
                printf("%9.6g ", b[i]);

        }
        printf("\n");

        /* print the vectors v after calling householder */
        printf("\n");
        for(i = 0; i < n; i++) {
            printf("v[%i] = ", i);
            for(j = 0; j < m - i; j++) {
                printf("%9.6g ", v[i][j]);
            }
            printf("\n");
        }
        printf("\n");

        /* print numerical evidence that v's are normalized */
        printf("Numerical verification that v_1, ..., v_%i are "
               "normalized:\n", n);
        for(i = 1; i < n; i++) {
            x = dot_product(v[i - 1], v[i - 1], m - i + 1);
            printf("||v[%i]|| = %lg, ", i, x);
            if(i % 5 == 0) {
                printf("\n");
            }
        }
        x = dot_product(v[n - 1], v[n - 1], m - n + 1);
            printf("||v[%i]|| = %lg.", n, x);
        if(n % 5 != 0) printf("\n");
        printf("\n");

        printf("Back Substitution \n" );



        backsolve( xm , a , b , n) ;


        /* print the vectors v after calling householder */
          for(i = 0; i < n; i++) {
              printf("x[%i]=", i);
              printf("%f ", xm[i]);

          }
          printf("\n");

          for (j=0; j<n; j++) {
              dtemp[j] =  xm[j] - 1.0;
          }
          err_svd[n - 3] = norm2_col(dtemp, n);


        /* free memory */
        for(i = 0; i < n; i++) {
            delete[] a[i];
            delete[] v[i];
        }
        delete[] a;
        delete[] v;
        delete[] xm;
        delete[] b;

      }

      fid = fopen("results.m", "w");
      write_vec(fid, "err_svd", err_svd, MAXDEGREE-2);
      //write_vec(fid, "err_eig  ", err_eig, MAXDEGREE-2);
      write_vec(fid, "times_svd", times_svd, MAXDEGREE-2);
      //write_vec(fid, "times_eig", times_eig, MAXDEGREE-2);
      fclose(fid);



    return 0;
}

/*************************************************************
 *
 * FUNCTION: norm2_col
 *
 * Compute 2-norm of a given vector.
 * Note: It is not a perfect implementation as it does not guard against
 *       overflow or underflow.
 *************************************************************/

double norm2_col(double *vec,int vec_dim)
{
    double v_out;
    int i;

    v_out = 0.0;
    for (i=0; i<vec_dim; i++) {
        v_out = v_out +  vec[i] *  vec[i];
    }
    return sqrt(v_out);
}

/*************************************************************
 *
 * FUNCTION: write_vec
 *
 * Write out a vector into file
 *************************************************************/


void  write_vec(FILE *fid, char *name, const double *vec, int n) {
    int i;

    fprintf(fid, "%s=[", name);
    for (i=0; i<n; ++i) {
        fprintf(fid, "%g; ", vec[i]);
    }
    fprintf(fid, "];\n");
}
