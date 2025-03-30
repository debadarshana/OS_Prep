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

/*Additional question :
 Sample Google-style Question
You have a system with multiple tasks arriving at different times with varying priorities. Design and implement a scheduler that ensures no task starves and higher-priority tasks are always scheduled earlier unless a lower-priority task has been waiting for a long time. What data structures would you use and why?

Would you like me to simulate a mock session now — like an actual interviewer asking follow-ups based on your answers? Or you can pick a few and try writing them out, and I’ll review your answers.






You said:
You have a system with multiple tasks arriving at different times with varying priorities. Design and implement a scheduler that ensures no task starves and higher-priority tasks are always scheduled earlier unless a lower-priority task has been waiting for a long time. What data structures would you use and why?

Can you answer for me


ChatGPT said:
Absolutely! Here's how you'd tackle and structure this question in a real interview — clear, concise, and with depth. The goal is to demonstrate understanding of scheduling trade-offs, fairness, and system-level efficiency.

🧠 Restate the Problem (for clarity):
We have tasks arriving over time, each with a priority. The scheduler must:

Always prefer higher-priority tasks

But prevent starvation by allowing lower-priority tasks to run if they’ve waited too long

✅ Key Requirements:
Priority-based scheduling — higher priority tasks should run first.

Arrival time awareness — tasks can arrive at different times.

Fairness — long-waiting low-priority tasks should eventually run (i.e., aging).

🧰 Proposed Data Structures
Concept	Data Structure Used	Why?
Task storage / scheduling	Min-Heap (priority queue)	For O(log n) scheduling by priority
Task metadata	Task struct (TCB) with arrival + wait time	Track waiting time
Time tracking	Global clock or tick counter	Needed to simulate time
📦 TCB Structure
c
Copy
Edit
typedef struct TCB {
    int pid;
    int base_priority;     // Original priority
    int effective_priority; // Adjusted based on aging
    int arrival_time;
    int wait_time;
} TCB;
🔄 Aging Mechanism
Every scheduling tick, update:

c
Copy
Edit
task->wait_time = current_time - task->arrival_time;
task->effective_priority = task->base_priority - (task->wait_time / AGING_FACTOR);
So over time, low-priority tasks get boosted.

Ensure effective_priority >= 0 or clamp within range.

🔧 Scheduler Algorithm
On each tick:

Check all tasks in the heap.

Update their effective_priority.

Rebuild the heap if any priorities changed.

Pop the task with the lowest effective_priority (highest effective priority).

Execute it.

If it’s not done, reinsert with updated arrival_time.

📈 Time Complexities
Operation	Time
Insert new task	O(log n)
Pop highest priority	O(log n)
Aging updates (optional: batch update)	O(n) or amortized if deferred
🤔 Why This Approach Works
The heap ensures efficient scheduling.

The aging mechanism prevents starvation.

We still honor priority but with fairness over time.

🧪 Bonus: Optimizations
Use lazy aging: only recalculate priority when popping or after N ticks.

Separate tasks into multiple priority buckets for faster lookup.

Use a monotonic clock or scheduler tick to avoid clock drift issues.

🧠 Sample Output Behavior
Let’s say:

Task A: priority = 10, arrives at time 0

Task B: priority = 100, arrives at time 1

After a few ticks:

Task B’s effective_priority improves to 50 → 30 → eventually beats A

This avoids starvation even though B started with lower priority

🏁 Summary (What to Tell the Interviewer)
I would use a min-heap to efficiently schedule tasks by effective priority, which is dynamically calculated from the base priority + aging effect. Each task tracks its arrival time and wait time. On every tick, we update priorities based on wait time to ensure fairness and avoid starvation. This gives us a fast, fair, and real-world-suitable scheduler.

Would you like to implement this aging-based priority heap scheduler in code next? We can start from your current heap version and extend it. */