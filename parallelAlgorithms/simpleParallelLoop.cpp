#include "mpi.h"
#include <iostream>
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
    double startTime, endTime;


    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcess);
    MPI_Comm_rank(MPI_COMM_WORLD, &processID);

    if (numProcess == 0) {
        startTime = MPI_Wtime();
    }

    computeRange(1, 1000000, numProcess, processID, start, end);

    for (int i = start; i <= end; i++) {
        sequence[i-1] = i;
    }

    sum = 0;
    for (int i = start; i <= end; i++) {
        sum += sequence[i-1];
    }

    MPI_Reduce(&sum, &computed, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (processID == 0) {
        endTime = MPI_Wtime();
        cout << "Sum = " << computed << endl;
        cout << "Time taken = " << (endTime-startTime) << endl;
    }

    MPI_Finalize();
    return 0;
}
