#include <mpi.h>
#include <iostream>
#include <math.h>

/*  This algorithm doesn't use the approach manager/worker
    It makes use of collecive operations instead,
    allowing process 0 does some work as other processes
*/

/*  When executing the compiled file you should to set the 
    number of subintervals as an argument
    by the command line. It gonna be captured by the argv[1].
*/
double f(double x) { return (4.0/(1 + x*x)); }

int main(int argc, char *argv[])
{
    int n, rank, numproces;

    //PI well computed
    double PI25DT = 3.141592653589793238462643;
    double mypi, pi = 0.0, h, sum, x;
    double startWtime = 0.0, endWtime;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numproces);

    if (rank == 0) {
        startWtime = MPI_Wtime();
        n = atoi(argv[1]);
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    h = 1.0/(double) n;
    sum = 0.0;

    for (int i = rank+1; i <=n; i+=numproces) {
        x = h * ((double)i-0.5);
        sum += f(x);
    }

    mypi = (double) h * sum;

    MPI_Reduce(&mypi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        endWtime = MPI_Wtime();
        printf("PI is approximately %.16f, Error is %.16f\n", pi, abs(pi-PI25DT));
        printf("Wall clock time = %f\n", endWtime-startWtime);
    }

    MPI_Finalize();
    return 0;
}