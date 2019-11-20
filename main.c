#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// number of elements in array
//#define MAX 15
//#define MAX 16
int MAX;

// number of threads
//#define THREAD_MAX 4
int THREAD_MAX;

// array of size MAX
int *aa;
int *array;
int *b;
int *c;
int *d;
int *e;
// thread control parameters
struct tsk {
    int tsk_no;
    int tsk_low;
    int tsk_high;
    int *tsk_array;
};

// merge function for merging two parts
void
merge(int low, int mid, int high, int *a)
{
    // n1 is size of left part and n2 is size of right part
    int n1 = mid - low + 1;
    int n2 = high - mid;

    int *left = malloc(n1 * sizeof(int));
    int *right = malloc(n2 * sizeof(int));

    int i;
    int j;

    // storing values in left part
    for (i = 0; i < n1; i++)
        left[i] = a[i + low];

    // storing values in right part
    for (i = 0; i < n2; i++)
        right[i] = a[i + mid + 1];

    int k = low;

    i = j = 0;

    // merge left and right in ascending order
    while (i < n1 && j < n2) {
        if (left[i] <= right[j])
            a[k++] = left[i++];
        else
            a[k++] = right[j++];
    }

    // insert remaining values from left
    while (i < n1)
        a[k++] = left[i++];

    // insert remaining values from right
    while (j < n2)
        a[k++] = right[j++];

    free(left);
    free(right);
}

// merge sort function
void
merge_sort(int low, int high, int *a)
{
    // calculating mid point of array
    int mid = low + (high - low) / 2;

    if (low < high) {
        // calling first half
        merge_sort(low, mid, a);

        // calling second half
        merge_sort(mid + 1, high, a);

        // merging the two halves
        merge(low, mid, high, a);
    }
}

// thread function for multi-threading
void *
merge_sort123(void *arg)
{
    struct tsk *tsk = arg;
    int low;
    int high;
    int *a=tsk->tsk_array;
    // calculating low and high
    low = tsk->tsk_low;
    high = tsk->tsk_high;
    // evaluating mid point
    int mid = low + (high - low) / 2;

    if (low < high) {
        merge_sort(low, mid, a);
        merge_sort(mid + 1, high, a);
        merge(low, mid, high, a);
    }

    return 0;
}

int
*merge_sort_threads(int *a, int MAX, int THREAD_MAX)
{
    struct tsk *tsk;
    pthread_t threads[THREAD_MAX];
    struct tsk tsklist[THREAD_MAX];
    int len = MAX / THREAD_MAX;
    int low = 0;

    for (int i = 0; i < THREAD_MAX; i++, low += len) {
        tsk = &tsklist[i];
        tsk->tsk_no = i;
        tsk->tsk_low = low;
        tsk->tsk_high = low + len - 1;
        if (i == (THREAD_MAX - 1))
            tsk->tsk_high = MAX - 1;
    }

    // creating threads
    for (int i = 0; i < THREAD_MAX; i++) {
        tsk = &tsklist[i];
        tsk->tsk_array = a;
        pthread_create(&threads[i], NULL, merge_sort123, tsk);
    }

    // joining all threads
    for (int i = 0; i < THREAD_MAX; i++)
        pthread_join(threads[i], NULL);

    // merging the final parts

    struct tsk *tskm = &tsklist[0];
    for (int i = 1; i < THREAD_MAX; i++) {
        struct tsk *tsk = &tsklist[i];
        merge(tskm->tsk_low, tsk->tsk_low - 1, tsk->tsk_high, a);
    }

    // displaying sorted array
    //printf("\n\nSorted array:\n");
    //for (int i = 0; i < MAX; i++)
    //    printf(" %d\n", a[i]);

    return a;
}

int
main(int argc, char *argv[])
{
    FILE *fp[5];
    int size=0;
    int *count[5];
    int j=0;
    int k=0;
    char c;
    THREAD_MAX = 20;

    THREAD_MAX = atoi(argv[1]);

    for(int i=1;i<3;i++)
    {
    fp[i] = fopen(argv[i+1], "r");                         //Open file for read.


    if (fp[i] == NULL)
    {
        printf("Error opening file %s\n",argv[i+1]);    //Program prints error message and closes if file is not found            
        exit(0);
    }

// Extract characters from file and store in character c 
    for (c = getc(fp[i]); c != EOF; c = getc(fp[i])) {
        if (c == '\n') {// Increment count if this character is newline 
            size = size + 1;
            count[i] = count[i] + 1;
        }
        
    }

    fclose(fp[i]);

    }
    array = malloc(sizeof(int) * (size));

    for(int i=1;i<3;i++)
    {
    fp[i] = fopen(argv[i+1], "r");  
    int num;
    fp[i] = fopen(argv[i+1], "r");
    while(fscanf(fp[i], "%d", &num) > 0) {
        array[k] = num;
        printf ("%d\n", array[k]);
        k++;
    }
    fclose(fp[i]);
    }


    //printf("\n\nUnsorted array:\n");
    //for (int i = 0; i < size; i++)
    //    printf(" %d\n", array[i]);


    merge_sort_threads(array, size, THREAD_MAX);

    return 0;
}