#include <cstdio>
#include <cstring>
#include <unistd.h>

#include <pthread.h>
/*
using namespace std;

#define N_THREAD (10)

class share_data {
    int counter{0};
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
public:
    void incr() {
        pthread_mutex_lock(&mutex);
        counter++;
        pthread_mutex_unlock(&mutex);
    }

    int getCount() const { return counter; }
};

void *incr(void *args) {
    share_data &data = *((share_data *)args);
    for(int i=0; i<100000; i++) {
        data.incr();
    }
    return NULL;
}

int main() {
    pthread_t *threads = new pthread_t[N_THREAD];
    share_data data;

    for(int i=0; i<N_THREAD; i++) {
        pthread_create(&threads[i], NULL, incr, (void*)(&data));
    }

    for(int i=0; i<N_THREAD; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("%d\n", data.getCount());
    return 0;
}


*/
