#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cstring>

#include <pthread.h>

using namespace std;

#define BUF_SIZE (21)
#define DELAY_LOOP_SIZE (100000000000)
#define OUTPUT_CHAR_NUM (201)

void slowPrintf(const char *str) {
    int j = 0;
    for(int i=1; i<rand()*1.0/RAND_MAX*DELAY_LOOP_SIZE; i++) j = j + (i/i);
    printf("%20s. printf(%d) Done%d\n", str, strlen(str), j);
}

char slowCharGenerator() {
    int j = 0;
    for(int i=1; i<rand()*1.0/RAND_MAX/BUF_SIZE*DELAY_LOOP_SIZE/10; i++) j = j + (i/i);
    return rand()*26.0/RAND_MAX+'a';
}

class Buffer {
    char buffer[BUF_SIZE];
    int bufferTail = 0;
    double filledRatio = 0.8;
    bool _isEnded = false;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cond_isBufferFilled = PTHREAD_COND_INITIALIZER;
    pthread_cond_t cond_isBufferCleared = PTHREAD_COND_INITIALIZER;
public:
    bool isNearlyFilled() const {
        return bufferTail >= filledRatio*(BUF_SIZE);
    }

    bool isFull() const { return bufferTail == BUF_SIZE-1; }

    void writeCharToBuffer(char c) {
        pthread_mutex_lock(&mutex);
        if(isFull()) {
            pthread_cond_signal(&cond_isBufferFilled);
            pthread_cond_wait(&cond_isBufferCleared, &mutex);
        }
        buffer[bufferTail++] = c;
        if(isNearlyFilled()) {
            pthread_cond_signal(&cond_isBufferFilled);
        }
        pthread_mutex_unlock(&mutex);
    }

    void flushWhenNearlyFilled() {
        char _buffer[BUF_SIZE];
        pthread_mutex_lock(&mutex);
        if(!_isEnded && !isNearlyFilled())
            pthread_cond_wait(&cond_isBufferFilled, &mutex);
        buffer[bufferTail] = '\0';
        strcpy(_buffer, buffer);
        bufferTail = 0;
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond_isBufferCleared);
        slowPrintf(_buffer);
    }

    void end() {
        pthread_mutex_lock(&mutex);
        _isEnded = true;
        printf("end.\n");
        pthread_cond_signal(&cond_isBufferFilled);
        pthread_mutex_unlock(&mutex);
    }

    bool isDone() const { return _isEnded && bufferTail == 0; }
};

void *producer(void *args) {
    Buffer &data = *((Buffer *)args);
    for(int i=0; i<OUTPUT_CHAR_NUM; i++) {
        data.writeCharToBuffer(slowCharGenerator());
    }
    data.end();
    return NULL;
}

void *consumer(void *args) {
    Buffer &data = *((Buffer *)args);
    while(!data.isDone()) data.flushWhenNearlyFilled();
    return NULL;
}
/*
int main() {
    srand(time(NULL));
    pthread_t thread_producer, thread_consumer;
    Buffer buffer;

    pthread_create(&thread_producer, NULL, producer, (void*)(&buffer));
    pthread_create(&thread_consumer, NULL, consumer, (void*)(&buffer));
    pthread_join(thread_producer, NULL);
    pthread_join(thread_consumer, NULL);

    return 0;
}

*/
