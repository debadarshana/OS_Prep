#include <stdio.h>
#include <pthread.h>
#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
/* long __builtin_expect(long exp, long expected); 
“I expect exp to evaluate to expected most of the time.”
Purpose of !!:

The first negation (!x) converts the value of x into a boolean (0 or 1).
If x is non-zero, !x becomes 0.
If x is zero, !x becomes 1.
The second negation (!!x) flips the boolean back:
If x was non-zero, !!x becomes 1.
If x was zero, !!x becomes 0.
This ensures that the result of !!x is always 0 or 1, regardless of the original type or value of x.
The compiler uses this info to:

1. Reorder instructions so the likely path is straight-line (no jump)

2. Avoid CPU pipeline stalls on mispredicted branches

3. Improve instruction cache locality*/

//staically allocated rig buffer
//Read the data, write the data buffer
//isFull(),isEmpty()
//capacity 
//size 
#define BUFFER_SIZE 16 
/* Lets have the buffersize in terms of 2's power. This will help us removing the modulo % to check the boundary condition 
instaed can be used &(BUFFER_SIZE-1)
and also help in structure allignment to the word boundary */

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct RingBuff_t
{
    int buffer[BUFFER_SIZE];
    int front;
    int rear;
    int size;
    

}RingBuff_t;

int isFull(RingBuff_t *RingBuffer)
{
    return RingBuffer->size == BUFFER_SIZE;

}

int isEmpty(RingBuff_t *RingBuffer)
{
    return RingBuffer->size == 0;
}
int size(RingBuff_t *RingBuffer)
{
    return RingBuffer->size;
}
int ReadBufferElement(RingBuff_t *RingBuffer)
{
#if 0
    if(NULL == RingBuffer)
    {
        printf("error\n");
        return -1;
    }
    if(isEmpty(RingBuffer))
    {
        printf("No item\n");
        return -1;
    }
#endif
/* the likely and unlikely helps is branch prediction for modern CPU. when we put unlikely, it means the is going to happen rarely and the code can be optimized*/
    if (unlikely(RingBuffer == NULL)) {
        printf("Error: NULL buffer\n");
        return -1;
    }
    pthread_mutex_lock(&mutex);

    if (unlikely(isEmpty(RingBuffer))) {
        printf("No item to read\n");
        return -1;
    }
    
    int item = RingBuffer->buffer[RingBuffer->front];
    RingBuffer->size--;

    RingBuffer->front = (RingBuffer->front + 1)& (BUFFER_SIZE -1);
    pthread_mutex_unlock(&mutex);
    
    return item;

    
}
int WriteBufferElement(RingBuff_t *RingBuffer,int item)
{
    //null check 
    if(NULL == RingBuffer)
    {
        printf("error\n");
        return 1;
    }
    pthread_mutex_lock(&mutex);
    if(isFull(RingBuffer))
    {
        RingBuffer->front = (RingBuffer->front + 1)& (BUFFER_SIZE -1);
        RingBuffer->size--;


    }
    RingBuffer->buffer[RingBuffer->rear] = item;

    RingBuffer->rear = (RingBuffer->rear + 1)& (BUFFER_SIZE -1);
    
    RingBuffer->size++;
    pthread_mutex_unlock(&mutex);

    return 1;
}

void* writer_thread(void* arg) {
    RingBuff_t* ringBuffer = (RingBuff_t*)arg;
    for (int i = 0; i < 100; i++) {
        WriteBufferElement(ringBuffer, i);
        printf("Writer: Wrote %d to the buffer.\n", i);
    }
    return NULL;
}

void* reader_thread(void* arg) {
    RingBuff_t* ringBuffer = (RingBuff_t*)arg;
    for (int i = 0; i < 100; i++) {
        int item = ReadBufferElement(ringBuffer);
        if (item != -1) {
            printf("Reader: Read %d from the buffer.\n", item);
        }
    }
    return NULL;
}

int main() {
    RingBuff_t ringBuffer = { .front = 0, .rear = 0, .size = 0 };

    pthread_t writer, reader;

    // Create writer and reader threads
    pthread_create(&writer, NULL, writer_thread, &ringBuffer);
    pthread_create(&reader, NULL, reader_thread, &ringBuffer);

    // Wait for both threads to finish
    pthread_join(writer, NULL);
    pthread_join(reader, NULL);

    return 0;
}
