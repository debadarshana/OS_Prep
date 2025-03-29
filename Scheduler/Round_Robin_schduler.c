#include <stdio.h>
#include <stdlib.h>

typedef struct List_t
{
    struct List_t *prev;
    struct List_t *next;
    int data;
}List_t;

typedef struct TCB
{
    int pid;
    int Quantum;
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
TCB* Create_Task(List_t* Task_Head, int pid, int Quantum)
{
    TCB *task = (TCB *)(malloc(sizeof(TCB)));
    if (!task) 
    {
        perror("Failed to allocate task");
        exit(EXIT_FAILURE);
    }
    task->pid = pid;
    task->Quantum = Quantum;
    //task->TaskList.prev = task->TaskList.next = NULL;
    Initialize_List(&task->TaskList); // Critical fix
    Add_To_Head(Task_Head,&(task->TaskList));
    return task;

}

void Simulate_scheduler(List_t *Task_Head)
{
   
    List_t *currTask = Task_Head->next;
    while(currTask != Task_Head)
    {
        List_t *nextTask = currTask->next;  // Save next task now
       
        TCB *runningTask = LIST_ENTRY(currTask,TCB,TaskList);
        /* execution*/
        printf("%d\t",runningTask->pid);
        runningTask->Quantum -= EACH_QUANTUM;
        if(0 >= runningTask->Quantum)
        {
            Delete_Item_List(Task_Head,&(runningTask->TaskList));
            free(runningTask);

        }
        currTask = nextTask;
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


