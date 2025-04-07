//Init()->Total size,Base address
//void *malloc(size(16KB),allignment(power of 2,16KB))
//no need of free
//#define MAX_SIZE 1024*1024*1024
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>

typedef struct heap_t
{
    int heap_size;
    int curr_offset;
    pthread_mutex_t heap_lock;
    char memory[];
}heap_t;

heap_t *heap;
void heap_init(heap_t *heap,int heap_size)
{
    heap->heap_size = heap_size;
    heap->curr_offset = 0;
    //Initialize memory to 0
    memset(heap->memory,0,heap_size);
    pthread_mutex_init(&heap->heap_lock, NULL);
    printf("Heap initailized\n");


}


void my_free(void *ptr)
{
    if(NULL == ptr)
        return;
    pthread_mutex_lock(&heap->heap_lock);
    uint8_t* p = (uint8_t * )ptr;
    p  -= 2;
    uint16_t size = *(uint16_t *)p;

   memset(p,0,size+2);
   pthread_mutex_unlock(&heap->heap_lock);

}
void* my_malloc(uint16_t size, int allignment)
{
    if (size == 0 || allignment == 0) return NULL;
    pthread_mutex_lock(&heap->heap_lock);
   /* the idea is to keep the metadata size before the assigned ptr
   current_offfset + metadat(2Byte)*/
    uintptr_t base = (uintptr_t)(heap->memory);
    uintptr_t raw_ptr = base + heap->curr_offset + 2;

    uintptr_t alligned_ptr = (raw_ptr + allignment - 1) & (~(allignment -1));
    uint32_t offset = alligned_ptr - base;
    if(offset + size > heap->heap_size)
    {
        pthread_mutex_unlock(&heap->heap_lock);
        return NULL;
    }
        
    *((uint16_t *)&heap->memory[offset-2]) = size;
    heap->curr_offset = offset + size;
    pthread_mutex_unlock(&heap->heap_lock);
    return (void *)alligned_ptr;


}
typedef struct test
{
    int a;
    char b;
}test;

void test_heap_initialization() {
    char buff[1024];
    heap = (heap_t *)buff;

    heap_init(heap, 1024 - sizeof(heap_t));
    assert(heap->heap_size == 1024 - sizeof(heap_t));
    assert(heap->curr_offset == 0);
    printf("test_heap_initialization passed\n");
}

void test_my_malloc_basic() {
    char buff[1024];
    heap = (heap_t *)buff;

    heap_init(heap, 1024 - sizeof(heap_t));
    test *obj = (test *)my_malloc(sizeof(test), 4);
    assert(obj != NULL);
    obj->a = 42;
    obj->b = 'x';
    assert(obj->a == 42);
    assert(obj->b == 'x');
    printf("test_my_malloc_basic passed\n");
}

void test_my_malloc_alignment() {
    char buff[1024];
    heap = (heap_t *)buff;

    heap_init(heap, 1024 - sizeof(heap_t));
    void *ptr1 = my_malloc(10, 8);
    void *ptr2 = my_malloc(20, 16);
    assert(ptr1 != NULL && ((uintptr_t)ptr1 % 8 == 0));
    assert(ptr2 != NULL && ((uintptr_t)ptr2 % 16 == 0));
    printf("test_my_malloc_alignment passed\n");
}

void test_my_malloc_out_of_memory() {
    char buff[128];
    heap = (heap_t *)buff;

    heap_init(heap, 128 - sizeof(heap_t));
    void *ptr1 = my_malloc(100, 4);
    void *ptr2 = my_malloc(50, 4);
    assert(ptr1 != NULL);
    assert(ptr2 == NULL); // Should fail due to insufficient memory
    printf("test_my_malloc_out_of_memory passed\n");
}

void test_my_free() {
    char buff[1024];
    heap = (heap_t *)buff;

    heap_init(heap, 1024 - sizeof(heap_t));
    test *obj = (test *)my_malloc(sizeof(test), 4);
    assert(obj != NULL);
    obj->a = 42;
    obj->b = 'x';
    my_free(obj);

    // Verify memory is cleared
    uint8_t *raw_ptr = (uint8_t *)obj - 2;
    uint16_t size = *(uint16_t *)raw_ptr;
    for (int i = 0; i < size + 2; i++) {
        assert(raw_ptr[i] == 0);
    }
    printf("test_my_free passed\n");
}

void test_multiple_allocations() {
    char buff[1024];
    heap = (heap_t *)buff;

    heap_init(heap, 1024 - sizeof(heap_t));
    void *ptr1 = my_malloc(16, 4);
    void *ptr2 = my_malloc(32, 8);
    void *ptr3 = my_malloc(64, 16);
    assert(ptr1 != NULL);
    assert(ptr2 != NULL);
    assert(ptr3 != NULL);
    assert((uintptr_t)ptr2 > (uintptr_t)ptr1);
    assert((uintptr_t)ptr3 > (uintptr_t)ptr2);
    printf("test_multiple_allocations passed\n");
}

int main() {
    test_heap_initialization();
    test_my_malloc_basic();
    test_my_malloc_alignment();
    test_my_malloc_out_of_memory();
    test_my_free();
    test_multiple_allocations();

    printf("All tests passed!\n");
    return 0;
}




