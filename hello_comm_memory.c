#include "mpi.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int rank = 0, size = 0, numProcesses = 0, nameLen = 0, numChars = 0, src = 0;
    MPI_Status status;
    char processorName[MPI_MAX_PROCESSOR_NAME];
    char *greeting;
    greeting = (char *) malloc(MPI_MAX_PROCESSOR_NAME+80);

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(processorName, &nameLen);

    sprintf(greeting, "Hello World, from process %d of %d on %s\n", 
            rank, numProcesses, processorName);
    if (rank == 0) {
        printf("%s", greeting);
        for (int i = 1; i < numProcesses; i++) {
            MPI_Probe(i, 1, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_CHAR, &numChars);
            greeting = (char *) malloc(numChars);
            src = status.MPI_SOURCE;
            MPI_Recv(greeting, numChars, 
                    MPI_CHAR, src, 1, MPI_COMM_WORLD, &status);
            printf("SOURCE: %d\n", status.MPI_SOURCE);
            printf("TAG MESSAGE: %d\n", status.MPI_TAG);
            printf("%s", greeting);
        }
    } else {
        MPI_Send(greeting, strlen(greeting)+1, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    free(greeting);

    return(0);
}