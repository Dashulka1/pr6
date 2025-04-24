#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int thread_id;
    int start, end;
    int *array;
    int target;
} ThreadData;

int THREAD_COUNT;
int *local_results;
int global_result = -1;
pthread_barrier_t barrier;

void *search_first(void *arg) {
    ThreadData *d = (ThreadData*)arg;
    int found = -1;
    for (int i = d->start; i < d->end; i++) {
        if (d->array[i] == d->target) {
            found = i;
            break;
        }
    }
    local_results[d->thread_id] = found;

    int rc = pthread_barrier_wait(&barrier);
    if (rc == PTHREAD_BARRIER_SERIAL_THREAD) {
        int min_idx = -1;
        for (int i = 0; i < THREAD_COUNT; i++) {
            if (local_results[i] != -1 &&
                (min_idx == -1 || local_results[i] < min_idx)) {
                min_idx = local_results[i];
            }
        }
        global_result = min_idx;
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

    local_results = malloc(THREAD_COUNT * sizeof(int));
    pthread_barrier_init(&barrier, NULL, THREAD_COUNT);

    pthread_t threads[THREAD_COUNT];
    ThreadData tdata[THREAD_COUNT];
    int chunk = n / THREAD_COUNT;

    for (int i = 0; i < THREAD_COUNT; i++) {
        tdata[i] = (ThreadData){
            .thread_id = i,
            .start = i * chunk,
            .end = (i == THREAD_COUNT - 1 ? n : (i+1)*chunk),
            .array = array,
            .target = target
        };
        pthread_create(&threads[i], NULL, search_first, &tdata[i]);
    }

    for (int i = 0; i < THREAD_COUNT; i++)
        pthread_join(threads[i], NULL);

    if (global_result != -1)
        printf("Первое вхождение %d на индексе %d\n", target, global_result);
    else
        printf("Значение %d не найдено\n", target);

    pthread_barrier_destroy(&barrier);
    free(array);
    free(local_results);
    return EXIT_SUCCESS;
}
