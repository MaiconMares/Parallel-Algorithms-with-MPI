#include <iostream>
#include <mpi.h>
#include <stdlib.h>
#define SIZE 10
#define MIN(A, B) (A < B ? A : B)

using namespace std;

int main(int argc, char *argv[])
{
    int rank = 0, numProces = 0;

    void managerCode(int numProces);
    void workerCode(void);

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numProces);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        managerCode(numProces);
    } else {
        workerCode();
    }

    MPI_Finalize();
    return 0;
}

void managerCode(int numProces) {
    int a[SIZE][SIZE], c[SIZE][SIZE], prd[SIZE], numSent = 0, row = 0, sender;
    MPI_Status status;

    //Initialize matrix a with all values equal to 2
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            a[i][j] = 2;
        }
    }

    //Sends line by line to workers
    for (int k = 1; k < MIN(numProces, SIZE); k++) {
        MPI_Send(a[k-1], SIZE, MPI_INT, k, k, MPI_COMM_WORLD);
        numSent++;
    }

    //Receives the product of matrix c made by workers
    for (int q = 0; q < SIZE; q++) {
        MPI_Recv(&prd, SIZE, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        row = status.MPI_TAG-1;
        sender = status.MPI_SOURCE;

        //Store the result of matrix c
        for (int col = 0; col < SIZE; col++)
            c[row][col] = prd[col];

        //While there are some lines not sent it will send them to workers
        if (numSent < SIZE) {
            MPI_Send(a[numSent], SIZE, MPI_INT, sender, numSent+1, MPI_COMM_WORLD);
            numSent++;
        } else {

            //It means there aren't lines to send, all lines were sent
            MPI_Send(MPI_BOTTOM, 0, MPI_INT, sender, 0, MPI_COMM_WORLD);

            cout << "RESULT" << endl;
            for (int l = 0; l < SIZE; l++) {
                for (int r = 0; r < SIZE; r++)
                    cout << c[l][r] << (r+1 == SIZE ? '\n' : ' ');
            }
        }
        
    }
}

void workerCode() {
    int b[SIZE][SIZE], c[SIZE], result[SIZE] {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
        rank = 0, line = 0;
    MPI_Status status;

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            b[i][j] = 3;
        }
    }

    //Initialize workers' processes
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank <= SIZE) {

        //Receives line by line from the manager
        MPI_Recv(c, SIZE, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        while(status.MPI_TAG > 0) {
            line = status.MPI_TAG-1;

            //Do multiply 
            for (int col = 0; col < SIZE; col++) {
                for (int k = 0; k < SIZE; k++)
                    result[col] += c[k]*b[k][col];
            }
            
            //Sends result to manager
            MPI_Send(result, SIZE, MPI_INT, 0, line+1, MPI_COMM_WORLD);

            //Clear auxiliar matrix to next step
            for (int index = 0; index < SIZE; index++)
                result[index] = 0;
                
            //Receives the last non sent lines
            MPI_Recv(c, SIZE, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        }
    }
}