#include <stdio.h>

//staically allocated rig buffer
//Read the data, write the data buffer
//isFull(),isEmpty()
//capacity 
//size 
#define BUFFER_SIZE 10
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
    int item = RingBuffer->buffer[RingBuffer->front];
    RingBuffer->size--;

    RingBuffer->front = (RingBuffer->front + 1)%BUFFER_SIZE;
    
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
    if(isFull(RingBuffer))
    {
        RingBuffer->front = (RingBuffer->front + 1)%BUFFER_SIZE;
        RingBuffer->size--;


    }
    RingBuffer->buffer[RingBuffer->rear] = item;

    RingBuffer->rear = (RingBuffer->rear + 1)%BUFFER_SIZE;
    
    RingBuffer->size++;

    return 1;
}

void test_ring_buffer() {
    RingBuff_t ringBuffer = { .front = 0, .rear = 0, .size = 0 };

    // Test 1: Check if the buffer is initially empty
    if (isEmpty(&ringBuffer)) {
        printf("Test 1 Passed: Buffer is initially empty.\n");
    } else {
        printf("Test 1 Failed: Buffer is not empty initially.\n");
    }

    // Test 2: Write an element to the buffer
    WriteBufferElement(&ringBuffer, 10);
    if (!isEmpty(&ringBuffer) && size(&ringBuffer) == 1) {
        printf("Test 2 Passed: Successfully wrote an element to the buffer.\n");
    } else {
        printf("Test 2 Failed: Failed to write an element to the buffer.\n");
    }

    // Test 3: Read the element back
    int item = ReadBufferElement(&ringBuffer);
    if (item == 10 && isEmpty(&ringBuffer)) {
        printf("Test 3 Passed: Successfully read the element from the buffer.\n");
    } else {
        printf("Test 3 Failed: Failed to read the correct element from the buffer.\n");
    }

    // Test 4: Fill the buffer to capacity
    for (int i = 0; i < BUFFER_SIZE; i++) {
        WriteBufferElement(&ringBuffer, i);
    }
    if (isFull(&ringBuffer)) {
        printf("Test 4 Passed: Buffer is full after writing %d elements.\n", BUFFER_SIZE);
    } else {
        printf("Test 4 Failed: Buffer is not full after writing %d elements.\n", BUFFER_SIZE);
    }

  // Test 5: Overwrite an element in the full buffer
    WriteBufferElement(&ringBuffer, 100);
    int last_written_idx = (ringBuffer.rear - 1 + BUFFER_SIZE) % BUFFER_SIZE;
    if (ringBuffer.buffer[last_written_idx] == 100) {
        printf("Test 5 Passed: Successfully overwrote an element in the full buffer.\n");
    } else {
        printf("Test 5 Failed: Failed to overwrite an element in the full buffer.\n");
    }
    // Test 6: Read all elements from the buffer

    int success = 1;
    for (int i = 0; i < BUFFER_SIZE; i++) {
        int val = ReadBufferElement(&ringBuffer);

        if (val == -1) {
            success = 0;
            break;
        }
    }
    if (success && isEmpty(&ringBuffer)) {
        printf("Test 6 Passed: Successfully read all elements from the buffer.\n");
    } else {
        printf("Test 6 Failed: Failed to read all elements from the buffer.\n");
    }
}

int main() 
{
    test_ring_buffer();
    return 0;
}