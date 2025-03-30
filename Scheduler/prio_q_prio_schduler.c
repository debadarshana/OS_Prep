#include <stdio.h>
#include <stdlib.h>

#define MAX_TASKS 100
typedef struct TCB
{
    int pid;
    int prio;
}TCB;

typedef struct PriorityQ
{
    TCB* heap[MAX_TASKS];
    int size;
}PriorityQ;
void swap(TCB** a, TCB** b)
{
    TCB *temp = *a;
    *a = *b;
    *b = temp;
}

void heaqpifyup(PriorityQ* pq,int index)
{
    if(index == 0)
        return;
    int i = index;
    while(i >= 0)
    {
        int parent = (i - 1)/2;
        if(pq->heap[parent]->prio > pq->heap[i]->prio)
        {
            swap(&pq->heap[i],&pq->heap[parent]);
            i = parent;
        }
        else
            break;
        
    }
}
void heaqpifydown(PriorityQ* pq)
{
    int sz = pq->size;
    int i = 0;
    while((2*i + 1) < sz)
    {
        int min = i;
        int left = 2*i + 1;
        int right = 2*i + 2;
        if( (left < sz) && (pq->heap[left]->prio < pq->heap[min]->prio))
            min = left;
        if( (right < sz) && (pq->heap[right]->prio < pq->heap[min]->prio))
            min = right;
        if(min == i)
            break;
        else
        {
            swap(&pq->heap[i],&pq->heap[min]);
            i = min;
        }

    }
}
void heap_insert(PriorityQ* pq, TCB* task)
{
    if(pq->size >= MAX_TASKS)
    {
        printf("Heap is full\n");
        return;
    }
    pq->heap[pq->size] = task;
    heaqpifyup(pq,pq->size);
    pq->size++;

}
TCB* Remove_heap(PriorityQ* pq)
{
    if(pq->size == 0)
        return NULL;
    TCB* removedTask = pq->heap[0];
    pq->heap[0] = pq->heap[--pq->size];
    heaqpifydown(pq);
    return removedTask;

}

void simulate_priority_scheduler(PriorityQ* pq) {
    while (pq->size > 0) {
        TCB* task = Remove_heap(pq);
        printf("Running Task: PID=%d, Priority=%d\n", task->pid, task->prio);
      //  free(task);
    }
    printf("All tasks completed.\n");
}
TCB* Create_Task(PriorityQ* pq, TCB* task)
{

    //task->TaskList.prev = task->TaskList.next = NULL;
    heap_insert(pq,task); // Critical fix

   
   
    return task;

}

int main()
{
    
    PriorityQ pq = { .size = 0 };

    // Create tasks
    int pid = 1;
    heap_insert(&pq, &(TCB){ .pid = pid++, .prio = 10 });
    heap_insert(&pq, &(TCB){ .pid = pid++, .prio = 20 });
    heap_insert(&pq, &(TCB){ .pid = pid++, .prio = 5 });
    heap_insert(&pq, &(TCB){ .pid = pid++, .prio = 30 });
    heap_insert(&pq, &(TCB){ .pid = pid++, .prio = 40 });
    heap_insert(&pq, &(TCB){ .pid = pid++, .prio = 20 });
    heap_insert(&pq, &(TCB){ .pid = pid++, .prio = 10 });


 
    simulate_priority_scheduler(&pq);
    printf("All task completed\n");
    while(1);
    return 1;

}