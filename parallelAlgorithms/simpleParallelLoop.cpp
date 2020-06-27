#include "mpi.h"
#include <iostream>
#include <algorithm>
using lu = unsigned long;

const int MAX = {1000010};

using namespace std;

//Basic algorithm to compute sum from 1 to 1000000
void computeRange
(int firstNumber, int lastNumber, int numProcess, int processID, int &start, int &end) {
    int work1 = (lastNumber-firstNumber+1)/numProcess; //Chunk size
    int work2 = (lastNumber-firstNumber+1)%numProcess; //Rest size

    start = processID*work1 + firstNumber + min(processID, work2);
    end = start+work1-1;

    //Distribute one unit of the rest to some first chunks
    if (work2 > processID) end = end + 1;
}

int main(int argc, char *argv[])
{
    int processID, numProcess, start, end;
    lu sum = 0, computed = 0, sequence[MAX];
    double startTime = 0.0, endTime;


    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcess);
    MPI_Comm_rank(MPI_COMM_WORLD, &processID);

    if (processID == 0) {
        startTime = MPI_Wtime();
    }

    computeRange(1, 1000000, numProcess, processID, start, end);

    sum = 0;
    //Each process sum his chunk accordingly to his range
    for (int i = start; i <= end; i++) {
        sum += i;
    }

    MPI_Reduce(&sum, &computed, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (processID == 0) {
        endTime = MPI_Wtime();
        cout << "Sum = " << computed << endl;
        printf("Time taken = %f\n", endTime-startTime);
    }

    MPI_Finalize();
    return 0;
}