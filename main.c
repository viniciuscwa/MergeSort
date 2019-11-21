#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// max number of elements
int MAX;

// number of threads
int THREAD_MAX;

long *array;

// thread control
struct tsk {
    long tsk_no;
    long tsk_low;
    long tsk_high;
    long *tsk_array;
};

// function to merge two parts
void
merge(long low, long mid, long high, long *a)
{
    long n1 = mid - low + 1;  //size of left part
    long n2 = high - mid; //size of right part

    long *left = malloc(n1 * sizeof(long));
    long *right = malloc(n2 * sizeof(long));

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

    // merge left and right in order
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


}

// merge sort function
void
merge_sort(long low, long high, long *a)
{
    // calculating mid point of array
    long mid = low + (high - low) / 2;

    if (low < high) {
        // calling first part
        merge_sort(low, mid, a);

        // calling second part
        merge_sort(mid + 1, high, a);

        // merging the two parts
        merge(low, mid, high, a);
    }
}

// thread function for multi-threading
void *
merge_sort123(void *arg)
{
    struct tsk *tsk = arg;
    long low;
    long high;
    long *a=tsk->tsk_array;
    // calculating low and high
    low = tsk->tsk_low;
    high = tsk->tsk_high;
    // evaluating mid point
    long mid = low + (high - low) / 2;

    if (low < high) {
        merge_sort(low, mid, a);
        merge_sort(mid + 1, high, a);
        merge(low, mid, high, a);
    }

    return 0;
}

// function to create threads and call child merge_sort
long
*merge_sort_threads(long *a, int MAX, int THREAD_MAX)
{
    struct tsk *tsk;
    pthread_t threads[THREAD_MAX];
    struct tsk tsklist[THREAD_MAX];
    long len = MAX / THREAD_MAX;
    long low = 0;

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

    // merging final parts

    struct tsk *tskm = &tsklist[0];
    for (int i = 1; i < THREAD_MAX; i++) {
        struct tsk *tsk = &tsklist[i];
        merge(tskm->tsk_low, tsk->tsk_low - 1, tsk->tsk_high, a);
    }

    // displaying sorted array
    for (int i = 0; i < MAX; i++)
        printf(" %ld\n", a[i]);

    return a; // return sorted
}

int
main(int argc, char *argv[])
{
    FILE *fp[5];
    int size=0;
    int *count[5];
    int j=0;
    int p=0;
    char c;
    THREAD_MAX = 8; // default number of threads

    THREAD_MAX = atoi(argv[1]); 

    for(int i=1;i<6;i++)
    {
    fp[i] = fopen(argv[i+1], "r");   //Open file for read

    if (fp[i] == NULL)
    {
        printf("Error opening file %s\n",argv[i+1]);    //Program prints error message and closes if file is not found            
        exit(0);
    }

    // Extract characters from file and store in character c 
    for (c = getc(fp[i]); c != EOF; c = getc(fp[i])) {
        if (c == '\n') {   // Increment count if this character is newline 
            size = size + 1;
            count[i] = count[i] + 1;
        }
        
    }

    fclose(fp[i]);
    }
 
    array = malloc(10000*sizeof(long)); // Dynamic Memory Allocation
                    
    for(int i=1;i<6;i++)
    {
    fp[i] = fopen(argv[i+1], "r");  
    long num;
    while(fscanf(fp[i], "%ld", &num) > 0) {
        array[p] = num;
        p++;
    }
    fclose(fp[i]);
    }

    merge_sort_threads(array, size, THREAD_MAX);  
    return 0;
}