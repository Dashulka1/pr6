#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int thread_id;
    int start, end;
    int *array;
    int target;
    int *counts;
    int **idxs;
} ThreadData;

int THREAD_COUNT;
pthread_barrier_t barrier;

void *search_all(void *arg) {
    ThreadData *d = (ThreadData*)arg;
    int local_count = 0;
    int capacity = d->end - d->start;
    int *local_idxs = malloc(capacity * sizeof(int));

    for (int i = d->start; i < d->end; i++) {
        if (d->array[i] == d->target)
            local_idxs[local_count++] = i;
    }
    d->counts[d->thread_id] = local_count;
    d->idxs[d->thread_id] = local_idxs;

    int rc = pthread_barrier_wait(&barrier);
    if (rc == PTHREAD_BARRIER_SERIAL_THREAD) {
        // общее число вхождений
        int total = 0;
        for (int i = 0; i < THREAD_COUNT; i++)
            total += d->counts[i];

        printf("Всего найдено: %d\n", total);
        if (total > 0) {
            printf("Индексы (возрастание): ");
            for (int i = 0; i < THREAD_COUNT; i++) {
                for (int j = 0; j < d->counts[i]; j++)
                    printf("%d ", d->idxs[i][j]);
                free(d->idxs[i]);
            }
            printf("\n");
        }
    }
    return NULL;
}

int main() {
    int n, target;
    printf("Введите N и искомое значение: ");
    if (scanf("%d %d", &n, &target) != 2) {
        fprintf(stderr, "Ошибка ввода\n");
        return EXIT_FAILURE;
    }
    int *array = malloc(n * sizeof(int));
    printf("Введите %d элементов массива: ", n);
    for (int i = 0; i < n; i++) scanf("%d", &array[i]);

    printf("Введите число потоков: ");
    scanf("%d", &THREAD_COUNT);

    // структуры
    int *counts = calloc(THREAD_COUNT, sizeof(int));
    int **idxs   = malloc(THREAD_COUNT * sizeof(int*));
    pthread_barrier_init(&barrier, NULL, THREAD_COUNT);

    pthread_t threads[THREAD_COUNT];
    ThreadData tdata[THREAD_COUNT];
    int chunk = n / THREAD_COUNT;

    for (int i = 0; i < THREAD_COUNT; i++) {
        tdata[i] = (ThreadData){
            .thread_id = i,
            .start = i*chunk,
            .end = (i == THREAD_COUNT-1 ? n : (i+1)*chunk),
            .array = array,
            .target = target,
            .counts = counts,
            .idxs = idxs
        };
        pthread_create(&threads[i], NULL, search_all, &tdata[i]);
    }

    for (int i = 0; i < THREAD_COUNT; i++)
        pthread_join(threads[i], NULL);

    pthread_barrier_destroy(&barrier);
    free(array);
    free(counts);
    free(idxs);
    return EXIT_SUCCESS;
}
