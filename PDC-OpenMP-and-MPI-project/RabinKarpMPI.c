#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include<mpi.h>

char word[1000], pattern[1000];

int CalculateHash(char s[], int start, int end)
{
    int i, val = 0, power = 1, j = 0, diff;
    diff = end - start + 1;

    for (i = end; j < diff; i--)
    {
        val += (s[i] - 'a' + 1) * power;
        power *= 10;
        j++;
    }

    return val;
}

int main(int argc, char** argv)
{
    int i, j, start, end, curr_val, act_val;
    bool found , idle = false;

    int rank , size;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(rank == 0)
    {
        printf("Enter the string: ");
        scanf("%s", word);

        printf("Enter the pattern to be found: ");
        scanf("%s", pattern);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Bcast(word, sizeof(word), MPI_CHAR, 0, MPI_COMM_WORLD);
    MPI_Bcast(pattern, sizeof(pattern), MPI_CHAR, 0, MPI_COMM_WORLD);

    int len = strlen(word);
    int sublen = strlen(pattern);

    if(rank == 0)
    {
        if(len == 0 || sublen == 0 || sublen > len)
        {
            printf("Invalid Inputs.\n");
            MPI_Finalize();
        }    
    }

    act_val = CalculateHash(pattern, 0, sublen - 1);

    int iterations = len - sublen + 1;
    // if(rank == 0)
    // {
    //     printf("iterations = %d\n" , iterations);
    // }

    found = false;
    if(size >= iterations)
    {
        start = rank;
        end = rank + sublen - 1;

        if(end >= len)
        {
            printf("Process %d of %d is idle\n" , rank , size);
            idle = true;
        }
        else
        {
            curr_val = CalculateHash(word , start ,end);

            if(curr_val == act_val)
            {
                printf("The pattern exists from index %d to %d in the string found by process %d of %d\n", start, end , rank , size);
                found  = true;
            }
        }
    }
    else if(size < iterations)
    {
        int subiterations = (int)floor((double)iterations / (double)size);

        int modval = iterations % size;

        if(rank <= modval - 1)
        {
            subiterations++;
        }
        printf("process %d subiterations %d\n", rank , subiterations);

        int ultstart , ultend;

        if(rank == 0)
        {
            ultstart = 0;
            int num = ultstart + subiterations;
            MPI_Send(&num, 1, MPI_INT, rank + 1, rank + 1, MPI_COMM_WORLD);
        }

        MPI_Barrier(MPI_COMM_WORLD);

        if(rank != size - 1)
        {
            MPI_Recv(&ultstart, 1, MPI_INT, rank - 1, rank, MPI_COMM_WORLD , MPI_STATUS_IGNORE);
            int num = ultstart + subiterations;
            MPI_Send(&num, 1, MPI_INT, rank + 1, rank + 1, MPI_COMM_WORLD);
        }

        if(rank == size - 1)
        {
            MPI_Recv(&ultstart, 1, MPI_INT, rank - 1, rank, MPI_COMM_WORLD , MPI_STATUS_IGNORE);
        }

        ultend = ultstart + subiterations;

        MPI_Barrier(MPI_COMM_WORLD);

        for(i = ultstart ; i < ultend ; i++)
        {
            start = i;
            end = i + sublen - 1;

            // printf("process=%d start=%d end=%d limit=%d\n", rank , start , end , (rank * subiterations) + subiterations);

            curr_val = CalculateHash(word , start , end);

            if(end >= len)
            {
                printf("Process %d of %d is idle\n" , rank , size);
                break;
            }

            if(curr_val == act_val)
            {
                printf("process %d The pattern exists from index %d to %d in the string\n", rank ,start, end);
                found  = true;
            }
            // else
            // {
            //     printf("Process %d did not find the pattern in this iteration\n", rank);
            // }
        }
    }

    if(!found && !idle)
    {
        printf("process %d did not found the pattern in the string\n", rank);
    }

    MPI_Finalize();

    return 0;
}