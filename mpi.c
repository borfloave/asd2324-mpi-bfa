#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <omp.h>
#include <stdio.h>

#define ITERATIONS 3000000

int main(int argc, char *argv[]) {
    int rank, size;
    int points_in_circle;
    int pi_total;
    double pi;
    double start_time, end_time;
    double x, y;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    srand(time(NULL) + rank); // Seed rand() with a different value for each process

    if (rank == 0) {
        start_time = MPI_Wtime(); // Start timing
    }

    // Calculate the range of iterations for each process
    int chunk_size = ITERATIONS / size;

    // Perform the computation with OpenMP parallelization
    #pragma omp parallel for private(x, y) reduction(+:points_in_circle)
    for (int i = 0; i < chunk_size; i++) {
        x = (double)rand() / ((double) RAND_MAX);
        y = (double)rand() / ((double) RAND_MAX);
        double distance = x * x + y * y;
        if (distance <= 1.0) {
            points_in_circle++;
        }
    }

    // Sum up the points in circle from each process
    MPI_Reduce(&points_in_circle, &pi_total, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Print the result from the master process
    if (rank == 0) {
        pi = 4 * pi_total / ITERATIONS ; // Multiply by 4 to estimate Pi
	end_time = MPI_Wtime(); // End timing
        printf("Estimated value of PI: %f, Execution time: %f seconds\n", pi, end_time - start_time);
    }

    MPI_Finalize();
    return 0;
}

