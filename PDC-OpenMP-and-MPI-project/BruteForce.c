#include<stdio.h>
#include<stdbool.h>

void printarr(int arr[] , int n)
{
    int i;
    for(i = 0 ; i< n ; i++)
    {
        printf("%d " , arr[i]);
    }
    printf("\n");
}

void BruteForce(int x[], int y[], int hull[], int n)
{
    bool flag;
    int val, i, j, k, total = 0;

    for(i = 0 ; i < n ; i++)
    {
        for(j = 0 ; j < n ; j++)
        {
            if(i != j)
            {
                flag = true;

                for(k = 0 ; k < n ; k++)
                {
                    if(i != k && j != k)
                    {
                        val = ((x[j] - x[i])*(y[k] - y[i])) - ((y[j] - y[i])*(x[k] - x[i]));
                        if(val <= 0)
                        {
                            flag = false;
                            break;
                        }
                    }
                }

                if(flag)
                {
                    hull[i] = j;
                    total++;
                }
            }
        }
    }

    i = 0;
    int count = 0;
    printf("total = %d\n" , total);
    while(true)
    {
        if(count == total)
        {
            break;
        }

        if(hull[i] != -1)
        {
            printf("%d,%d -> %d,%d\n" , x[i] , y[i] , x[hull[i]] , y[hull[i]]);
            i = hull[i];
            count++;
        }
        else
        {
            i++;
        }
    }

}

int main ()
{
    int choice, n;
    do
    {
        printf("Enter your choice: \n");
        printf("1. Add points manually \n");
        printf("2. Read from file \n");
        printf("3. Exit \n");
        scanf("%d", &choice);
        if (choice == 1)
        {
            printf("Enter the number of points: ");
            scanf("%d", &n);
            int x[n] , y[n], hull[n];
            printf("Enter the co-ordinates of points: \n");
            for(int i = 0 ; i < n ; i++)
            {
                scanf("%d %d" , &x[i] , &y[i]);
                hull[i] = -1;
            }
            BruteForce(x, y, hull, n);
            break;
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
            break;
        }
        else if (choice == 3)
        {
            return 0;
        }
        else
        {
            printf("Wrong input! Please Enter again. \n");
        }
    } while (choice != 3);

    return 0;
}