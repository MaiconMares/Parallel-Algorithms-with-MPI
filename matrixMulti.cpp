#include <iostream>
#include <mpi.h>
#include <stdlib.h>
#define SIZE 10
#define MIN(A, B) (A < B ? A : B)

using namespace std;

int main(int argc, char *argv[])
{
    int rank = 0, numProces = 0, nameLen = 0;
    char processName[MPI_MAX_PROCESSOR_NAME];
    char *infoMessage;

    void managerCode(int numProces);
    void workerCode(void);

    infoMessage = (char *) malloc(MPI_MAX_PROCESSOR_NAME + 80);

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numProces);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(processName, &nameLen);

    sprintf(infoMessage, "\nMessage from process %d of %d on %s\n", 
        rank, numProces, processName);

    if (rank == 0) {
        printf("%s", infoMessage);
        managerCode(numProces);
    } else {
        printf("%s", infoMessage);
        workerCode();
    }

    MPI_Finalize();
    return 0;
}

void managerCode(int numProces) {
    int a[SIZE][SIZE], c[SIZE][SIZE], numSent = 0, row = 0, sender,
    messageSize = 0, *prd;
    MPI_Status status;

    //Initialize matrix a and c
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            a[i][j] = 2;
            c[i][j] = 0;
        }
    }

    //Sends line by line to workers
    for (int k = 1; k < MIN(numProces, SIZE); k++) {
        MPI_Send(a[k-1], SIZE, MPI_INT, k, k, MPI_COMM_WORLD);
        numSent++;
    }

    //Receives the product of matrix c made by workers
    for (int q = 0; q < SIZE; q++) {
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &messageSize);

        prd = (int *) malloc(messageSize*sizeof(int));

        row = status.MPI_TAG-1;
        sender = status.MPI_SOURCE;

        MPI_Recv(prd, messageSize, MPI_INT, sender, MPI_ANY_TAG, MPI_COMM_WORLD, 
            &status);

        //Store the result of matrix c
        for (int col = 0; col < SIZE; col++)
            c[row][col] = *(prd+col);

        //While there are some lines not sent it will send them to workers
        if (numSent < SIZE) {
            MPI_Send(a[numSent], SIZE, MPI_INT, sender, numSent+1, MPI_COMM_WORLD);
            numSent++;
        } else {

            //It means there aren't lines to send, all lines were sent
            MPI_Send(MPI_BOTTOM, 0, MPI_INT, sender, 0, MPI_COMM_WORLD);

            cout << "RESULT" << endl;
            for (int l = 0; l < SIZE; l++) {
                for (int r = 0; r < SIZE; r++) {
                    printf("%.3d", c[l][r]);
                    cout << (r+1 == SIZE ? '\n' : ' ');
                }
            }

            free(prd);
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