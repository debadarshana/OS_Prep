#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef struct List_t
{
    struct List_t *prev;
    struct List_t *next;
    int data;
}List_t;

typedef struct TCB
{
    int pid;
    int prio;
    List_t TaskList;
}TCB;

#define LIST_ENTRY(ptr,type,member) \
    ((type *)((char *)(ptr) - ((unsigned long)( &((type *)0)->member ))))
#define EACH_QUANTUM 5

//initialize the list

void Initialize_List(List_t *List)
{
    List->prev = List->next = List;
    
}

void Add_To_Head(List_t *List, List_t *item)
{
        item->next = List->next;
        item->prev = List;
        List->next = item;
        item->next->prev = item;
     
}

void Add_To_Tail(List_t *List, List_t *item)
{
    item->next = List;
    item->prev = List->prev;
    item->prev->next = item;
    List->prev = item;
}

void Delete_Item_List(List_t *List, List_t *item)
{
    item->prev->next = item->next;
    item->next->prev = item->prev;
    item->next = NULL;
    item->prev = NULL;
}
void Traverse_List(List_t* List)
{
    List_t *curr = List->next;
    while(curr != List)
    {
        TCB *runningTask = LIST_ENTRY(curr,TCB,TaskList);
       printf("All Pending Task :%d\n",runningTask->pid);
        curr = curr->next;
       
    }
}
/* Create the TASK Simulation*/
TCB* Create_Task(List_t* Task_Head, int pid,int prio)
{
    TCB *task = (TCB *)(malloc(sizeof(TCB)));
    if (!task) 
    {
        perror("Failed to allocate task");
        exit(EXIT_FAILURE);
    }
    task->pid = pid;
    task->prio = prio;
    //task->TaskList.prev = task->TaskList.next = NULL;
    Initialize_List(&task->TaskList); // Critical fix

    //Adding to tail ,if same priority follow FIFO.
    Add_To_Tail(Task_Head,&(task->TaskList));
    return task;

}

void Simulate_scheduler(List_t *Task_Head)
{
   
    List_t *currTask = Task_Head->next;
    List_t *runningTask = NULL;
    unsigned int best_prio = __INT_MAX__;
    while(currTask != Task_Head)
    {
        
       
        TCB *task  = LIST_ENTRY(currTask,TCB,TaskList);

        if(task->prio < best_prio)
        {
            runningTask = currTask;
            best_prio = task->prio;
        }
        currTask = currTask->next;



    }
            /* execution*/
    if(runningTask)
    {
        TCB *task  = LIST_ENTRY(runningTask,TCB,TaskList);
        printf("Running Task: PID=%d, Priority=%d\n", task->pid, task->prio);
        Delete_Item_List(Task_Head,&(task->TaskList));
        free(task);
    }
  
    printf("\n");

}


int main()
{
    List_t Task_Head;
    Initialize_List(&Task_Head);

    //Create Task
    TCB* task1 = Create_Task(&Task_Head,1,10);
    TCB* task2 = Create_Task(&Task_Head,2,20);
    TCB* task3 = Create_Task(&Task_Head,3,5);
    TCB* task4 = Create_Task(&Task_Head,4,30);
    TCB* task5 = Create_Task(&Task_Head,5,40);
    TCB* task6 = Create_Task(&Task_Head,6,20);
    TCB* task7 = Create_Task(&Task_Head,7,10);

   while(Task_Head.next != &Task_Head)
        Simulate_scheduler(&Task_Head);
    printf("All task completed\n");
    while(1);
    return 1;

}
/* Its need linear search for Scheduling as we need to traverse through the entire list.
We can implement priority queue, to optimize further to log n.
Linux use red black tree*/


