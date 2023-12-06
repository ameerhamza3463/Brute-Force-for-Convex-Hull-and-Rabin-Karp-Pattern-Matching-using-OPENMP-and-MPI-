#include <stdio.h>
#include <stdbool.h>
#include <omp.h>

// void printarr(int arr[], int n)
// {
//     int i;
//     for (i = 0; i < n; i++)
//     {
//         printf("%d ", arr[i]);
//     }
//     printf("\n");
// }

void BruteForce(int x[], int y[], int hull[], int n)
{
    bool flag;
    int val, i, j, k, total = 0;

    omp_set_num_threads(n);

    #pragma omp parallel for private(j, k, flag, val) reduction(+ : total)
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            if (i != j)
            {
                flag = true;

                #pragma omp parallel for private(k) shared(flag) schedule(static)
                for (k = 0; k < n; k++)
                {
                    if (i != k && j != k)
                    {
                        val = ((x[j] - x[i]) * (y[k] - y[i])) - ((y[j] - y[i]) * (x[k] - x[i]));
                        if (val <= 0)
                        {
                            flag = false;
                        }
                    }
                }

                #pragma omp critical
                {
                    if (flag)
                    {
                        hull[i] = j;
                        total++;
                        printf("Thread %d found hull point: (%d,%d) -> (%d,%d) out of %d threads\n",omp_get_thread_num(), x[i], y[i], x[j], y[j] , omp_get_max_threads());
                    }
                }
            }
        }
    }

    i = 0;
    int count = 0;
    printf("total = %d\n", total);
    // while (true)
    // {
    //     if (count == total)
    //     {
    //         break;
    //     }

    //     if (hull[i] != -1)
    //     {
    //         i = hull[i];
    //         count++;
    //     }
    //     else
    //     {
    //         i++;
    //     }
    // }

}


int main()
{
    int choice, n;
    printf("--------------------------------------------------------\n");
    printf("\t\t BRUTE FORCE ALGORITHM OPENMP \t\t\n");
    printf("--------------------------------------------------------\n");
    do
    {
        printf("\nEnter your choice: \n");
        printf("1. Add points manually \n");
        printf("2. Read from file \n");
        printf("3. Exit \n");
        scanf("%d", &choice);
        if (choice == 1)
        {
            printf("\nEnter the number of points: ");
            scanf("%d", &n);
            int x[n] , y[n], hull[n];
            printf("\nEnter the co-ordinates of points: \n");
            for(int i = 0 ; i < n ; i++)
            {
                scanf("%d %d" , &x[i] , &y[i]);
                hull[i] = -1;
            }
            BruteForce(x, y, hull, n);
            printf("--------------------------------------------------------\n");
        }
        else if (choice == 2)
        {
            FILE *filePointer;
            char fileName[100];

            printf("Enter the file name: ");
            scanf("%s", fileName);

            filePointer = fopen(fileName, "r");

            if (filePointer == NULL) {
                printf("Unable to open the file %s.\n", fileName);
                return 1;
            }
            fscanf(filePointer, "%d", &n);
            int x[n], y[n], hull[n];
            for (int i = 0; i < n; i++) {
                fscanf(filePointer, "%d,%d", &x[i], &y[i]);
                hull[i] = -1;
            }
            fclose(filePointer);
            BruteForce(x, y, hull, n);
            printf("--------------------------------------------------------\n");
        }
        else if (choice == 3)
        {
            return 0;
        }
        else
        {
            printf("Wrong input! Please Enter again. \n");
            printf("--------------------------------------------------------\n");
        }
    } while (choice != 3);

    return 0;
}