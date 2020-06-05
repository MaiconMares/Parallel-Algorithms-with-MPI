#include "mpi.h"
#include <iostream>
#include <string.h>
#include <time.h>
#define SIZE 1000
#define MIN(x,y) ((x) < (y) ? x : y)

using namespace std;

int main(int argc, char *argv[])
{
    void managerCode(int  numprocs);
    void workerCode(void);
    int rank = 0, numprocs = 0;
    clock_t start, end;
    double time_taken;

    start = clock();
    MPI_Init(&argc, &argv);
    
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        managerCode(numprocs);
    } else {
        workerCode();
    }

    MPI_Finalize();
    end = clock();

    time_taken = ((double) (end - start))/CLOCKS_PER_SEC;

    printf("Time taken = %lf\n", time_taken);
    return 0;
}

void managerCode(int numprocs) {
    double a[SIZE][SIZE], c[SIZE], *dotp;
    int i = 0, j = 0, sender = 0, row = 0, numsent = 0, count = 0;
    MPI_Status status;

    //Inicialização de a
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            a[i][j] = (double) j;
        }
    }

    for (i = 1; i < MIN(numprocs, SIZE); i++) {
        MPI_Send(a[i-1], SIZE, MPI_DOUBLE, i, i, MPI_COMM_WORLD);
        numsent++;
    }

    //Recebe o produto dos workers
    for (i = 0; i < SIZE; i++) {
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_DOUBLE, &count);
            
        dotp = (double *) malloc(count*sizeof(double));

        MPI_Recv(dotp, count, MPI_DOUBLE, MPI_ANY_SOURCE, 
                    MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        printf("MESSAGE '%lf' from process %d of %d\n", *dotp, status.MPI_SOURCE, 1000);
        sender = status.MPI_SOURCE;
        row = status.MPI_TAG-1;
        c[row] = *dotp;

        /* Enviar outra linha da matriz para o worker se ainda existir 
        alguma para ser calculada */
        if (numsent < SIZE) {
            //Porque o sender continua sendo o mesmo que envio a última mensagem?
            MPI_Send(a[numsent], SIZE, MPI_DOUBLE, sender, numsent+1, MPI_COMM_WORLD);
            numsent++;
        } else {
            MPI_Send(MPI_BOTTOM, 0, MPI_DOUBLE, sender, 0, MPI_COMM_WORLD);
            for (int i = 0; i < SIZE; i++) {
                cout << c[i] << (i+1 == SIZE ? '\n' : ' ');
            }

            free(dotp);
        }
    }
}

void workerCode(void) {
    double b[SIZE], c[SIZE];
    int i = 0, row = 0, rank = 0;
    double dotp = 0;
    MPI_Status status;
    //Inicialização de c
    for (i = 0; i < SIZE; i++) {
        b[i] = 1.0;
    }

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank <= SIZE) {
        MPI_Recv(c, SIZE, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        while(status.MPI_TAG > 0) {
            row = status.MPI_TAG-1;
            dotp = 0.0;
            for (i = 0; i < SIZE; i++) {
                dotp += c[i] * b[i];
            }
            MPI_Send(&dotp, 1, MPI_DOUBLE, 0, row+1, MPI_COMM_WORLD);
            MPI_Recv(c, SIZE, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        }
    }
}