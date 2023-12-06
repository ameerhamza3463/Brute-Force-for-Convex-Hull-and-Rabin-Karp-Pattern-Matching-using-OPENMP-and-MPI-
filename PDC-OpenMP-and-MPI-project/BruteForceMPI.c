#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include<math.h>
#include <mpi.h>

void printarr(int arr[], int n)
{
    int i;
    for (i = 0; i < n; i++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main(int argc, char **argv)
{
    int n, i, j, k, total = 0 , subpoints;

    MPI_Init(&argc, &argv);

    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    {
        printf("Enter the number of points: ");
        scanf("%d", &n);
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int x[n] , y[n] , hull[n];

    if (rank == 0)
    {
        for (i = 0; i < n; i++)
        {
            scanf("%d %d", &x[i], &y[i]);
            hull[i] = -1;
        }
    }

    MPI_Bcast(x, n, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(y, n, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(hull, n, MPI_INT, 0, MPI_COMM_WORLD);

    bool flag;
    int val;

    int local_total = 0;

    if(n <= size)
    {
        if(rank < n)
        {
            subpoints = 1;
            for (j = 0; j < n; j++)
            {
                if (rank != j)
                {
                    flag = true;

                    for (k = 0; k < n; k++)
                    {
                        if (rank != k && j != k)
                        {
                            val = ((x[j] - x[rank]) * (y[k] - y[rank])) - ((y[j] - y[rank]) * (x[k] - x[rank]));
                            if (val <= 0)
                            {
                                flag = false;
                                break;
                            }
                        }
                    }

                    if (flag)
                    {
                        hull[rank] = j;
                        printf("process %d of %d processes found an edge from %d,%d -> %d,%d\n", rank , size , x[rank] , y[rank] , x[j] , y[j]);
                        local_total++;
                    }
                }
            }
        }
        else
        {
            subpoints = 0;
            printf("process %d of %d processes is idle.\n", rank , size);
        }

        printf("process %d subpoints are %d\n", rank , subpoints);
    }
    else if(n > size)
    {
        subpoints = (int)floor((double)n / (double)size);

        int modval = n % size;

        if(rank <= modval - 1)
        {
            subpoints++;
        }
        printf("process %d subpoints are %d\n", rank , subpoints);

        int ultstart , ultend;

        if(rank == 0)
        {
            ultstart = 0;
            int points = ultstart + subpoints;
            MPI_Send(&points, 1, MPI_INT, rank + 1, rank + 1, MPI_COMM_WORLD);
        }

        MPI_Barrier(MPI_COMM_WORLD);

        if(rank != size - 1)
        {
            MPI_Recv(&ultstart, 1, MPI_INT, rank - 1, rank, MPI_COMM_WORLD , MPI_STATUS_IGNORE);
            int points = ultstart + subpoints;
            MPI_Send(&points, 1, MPI_INT, rank + 1, rank + 1, MPI_COMM_WORLD);
        }

        if(rank == size - 1)
        {
            MPI_Recv(&ultstart, 1, MPI_INT, rank - 1, rank, MPI_COMM_WORLD , MPI_STATUS_IGNORE);
        }

        ultend = ultstart + subpoints;

        MPI_Barrier(MPI_COMM_WORLD);

        for (i = ultstart; i < ultend ; i++)
        {
            for (j = 0; j < n; j++)
            {
                if (i != j)
                {
                    flag = true;

                    for (k = 0; k < n; k++)
                    {
                        if (i != k && j != k)
                        {
                            val = ((x[j] - x[i]) * (y[k] - y[i])) - ((y[j] - y[i]) * (x[k] - x[i]));
                            if (val <= 0)
                            {
                                flag = false;
                                break;
                            }
                        }
                    }

                    if (flag)
                    {
                        hull[i] = j;
                        local_total++;
                        printf("process %d of %d processes found an edge from %d,%d -> %d,%d\n", rank , size , x[i] , y[i] , x[j] , y[j]);
                    }
                }
            }
        }
    }

    // Reduce the local_total values from all processes to get the total
    MPI_Reduce(&local_total, &total, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Only combining the valid hull indexes
    int overall_hull[n] , subitr[n] , gathered_hull[n * size];

    MPI_Gather(hull, n, MPI_INT, gathered_hull, n, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Gather(&subpoints, 1, MPI_INT, subitr, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if(rank == 0)
    {
        // printarr(subitr , n);
        int pos , process;
        process = 0;
        pos = 0;
        for(i = 0; i < n * size , process < size; i++)
        {
            for(j = 0 ; j < subitr[process] ; j++)
            {
                overall_hull[pos] = gathered_hull[i];
                i++;
                pos++;
            }
            i += n - 1;
            process++;
        }

        printf("gathered hull\n");
        printarr(gathered_hull , n * size);
        printf("overall hull\n");
        printarr(overall_hull , n);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Finalize();

    if (rank == 0)
    {
        i = 0;
        int count = 0;
        printf("total edges = %d\n", total);
        while (true)
        {
            if (count == total)
            {
                break;
            }

            if (overall_hull[i] != -1)
            {
                printf("%d,%d -> %d,%d\n", x[i], y[i], x[overall_hull[i]], y[overall_hull[i]]);
                i = overall_hull[i];
                count++;
            }
            else
            {
                i++;
            }
        }
    }

    return 0;
}