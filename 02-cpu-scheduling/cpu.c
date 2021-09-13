#include <time.h>
#include <stddef.h>
#include <stdio.h>
#include "oslabs.h"

struct PCB NULLPCB = {
    .process_id = 0,
};

struct PCB ready_queue[QUEUEMAX];

struct PCB handle_process_arrival_pp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process, struct PCB new_process, int timestamp) {

    // If there is no currently-running process (i.e., the third argument is the NULLPCB), then the method returns the PCB of the newly-arriving process, indicating that it is the process to execute next. In this case, the PCB of the new process is modified so that the execution start time is set to the current timestamp, the execution end time is set to the sum of the current timestamp and the total burst time and the remaining burst time is set to the total burst time.
    if ( current_process.process_id == NULLPCB.process_id )
    {
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;

        return new_process;
    }

    // If there is a currently-running process, the method compares the priority of the newly-arriving process with the currently-running process. If the new process has equal or lower priority (smaller integers for the priority field in the PCB indicate higher priority), then its PCB is simply added to the ready queue and the return value is the PCB of the currently-running process. As the newly-arriving process is added to the ready queue, its execution start time and execution end time are set to 0, and the remaining burst time is the same as its total burst time.
    if ( new_process.process_priority >= current_process.process_priority ) {
        new_process.execution_endtime = new_process.execution_starttime = 0;
        new_process.remaining_bursttime = new_process.total_bursttime;
        ready_queue[*queue_cnt] = new_process;
        (*queue_cnt)++;
        return current_process;
    }

    // If the new process has a higher priority, then the PCB of the currently-running process is added to the ready queue and the return value is the PCB of the new process. In this case, the PCB of the new process is modified so that the execution start time is set to the current timestamp, the execution end time is set to the sum of the current timestamp and the total burst time and the remaining burst time is set to the total burst time. Also, the PCB of the currently-running process is added to the ready queue after marking its execution end time as 0, and adjusting its remaining burst time.
    if (new_process.process_priority < current_process.process_priority)
    {
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;

        current_process.execution_endtime = 0;
        current_process.remaining_bursttime = current_process.total_bursttime - current_process.execution_starttime;
        ready_queue[*queue_cnt] = current_process;
        (*queue_cnt)++;

        return new_process;
    }
}


struct PCB handle_process_completion_pp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp) {
    //If the ready queue is empty, the method returns the NULLPCB, indicating that there is no process to execute. Otherwise, the method finds the PCB of the process in the ready queue with the highest priority (smaller integers for the priority field in the PCB mean higher priorities), removes this PCB from the ready queue and returns it. Before returning the PCB of the next process to execute, it is modified to set the execution start time as the current timestamp and the execution end time as the sum of the current timestamp and the remaining burst time.

    if(!(*queue_cnt)){
        return NULLPCB;
    }

    int next_idx = 0;
    for (int i = 0; i < *queue_cnt; i++)
    {
        struct PCB current = ready_queue[i];
        struct PCB top = ready_queue[next_idx];
        if (current.process_priority < top.process_priority)
        {
            next_idx = i;
        }
    }
    struct PCB next_process = ready_queue[next_idx];
    next_process.execution_starttime = timestamp;
    next_process.execution_endtime = timestamp + next_process.total_bursttime;

    for (int i = next_idx; i < *queue_cnt; i++)
    {
        ready_queue[i] = ready_queue[i + 1];
    }

    (*queue_cnt)--;

    return next_process;
}
struct PCB handle_process_arrival_srtp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process, struct PCB new_process, int timestamp)
{
    // If there is no currently-running process (i.e., the third argument is the NULLPCB), then the method returns the PCB of the newly-arriving process, indicating that it is the process to execute next. In this case, the PCB of the new process is modified so that the execution start time is set to the current timestamp, the execution end time is set to the sum of the current timestamp and the total burst time and the remaining burst time is set to the total burst time.
    if (current_process.process_id == NULLPCB.process_id)
    {
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;

        return new_process;
    }

    // If there is a currently-running process, the method compares the remaining burst time of the currently-running process and the total burst time of the newly-arriving process. If the new process does not have a shorter burst time, then its PCB is simply added to the ready queue and the return value is the PCB of the currently running process. As the newly-arriving process is added to the ready queue, its execution start time and execution end time are set to 0, and the remaining burst time is set to the total burst time.
    if (new_process.total_bursttime >= current_process.remaining_bursttime)
    {
        new_process.execution_endtime = new_process.execution_starttime = 0;
        new_process.remaining_bursttime = new_process.total_bursttime;
        ready_queue[*queue_cnt] = new_process;
        (*queue_cnt)++;
        return current_process;
    }

    //If the new process has a shorter burst time, then the PCB of the currently-running process is added to the ready queue and the return value is the PCB of the new process. In this case, the PCB of the new process is modified so that the execution start time is set to the current timestamp, the execution end time is set to the sum of the current timestamp and the total burst time and the remaining burst time is set to the total burst time. Also, the PCB of the currently-running process is added to the ready queue, after marking its execution start time and execution end time as 0, and adjusting its remaining burst time
    if (new_process.total_bursttime < current_process.remaining_bursttime)
    {
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;

        current_process.remaining_bursttime = current_process.total_bursttime - current_process.execution_starttime;
        current_process.execution_starttime = current_process.execution_endtime = 0;
        ready_queue[*queue_cnt] = current_process;
        (*queue_cnt)++;

        return new_process;
    }
}
struct PCB handle_process_completion_srtp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp)
{
    // If the ready queue is empty, the method returns the NULLPCB, indicating that there is no process to execute next. Otherwise, the method finds the PCB of the process in the ready queue with the smallest remaining burst time, removes this PCB from the ready queue and returns it. Before returning the PCB of the next process to execute, it is modified to set the execution start time as the current timestamp and the execution end time as the sum of the current timestamp and the remaining burst time.
    if (!(*queue_cnt)) {
        return NULLPCB;
    }

    int next_idx = 0;
    for (int i = 0; i < *queue_cnt; i++)
    {
        struct PCB current = ready_queue[i];
        struct PCB top = ready_queue[next_idx];
        if (current.remaining_bursttime < top.remaining_bursttime)
        {
            next_idx = i;
        }
    }

    struct PCB next_process = ready_queue[next_idx];
    next_process.execution_starttime = timestamp;
    next_process.execution_endtime = timestamp + next_process.remaining_bursttime;

    for (int i = next_idx; i < *queue_cnt; i++)
    {
        ready_queue[i] = ready_queue[i + 1];
    }

    (*queue_cnt)--;
    return next_process;
}
struct PCB handle_process_arrival_rr(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process, struct PCB new_process, int timestamp, int time_quantum)
{
    //If there is no currently-running process (i.e., the third argument is the NULLPCB), then the method returns the PCB of the newly-arriving process, indicating that it is the process to execute next. In this case, the PCB of the new process is modified so that the execution start time is set to the current timestamp, the execution end time is set to the sum of the current timestamp and the smaller of the time quantum and the total burst time. The remaining burst time is set to the total burst time.
    if (current_process.process_id == NULLPCB.process_id)
    {
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + MIN(time_quantum, new_process.total_bursttime);
        new_process.remaining_bursttime = new_process.total_bursttime;

        return new_process;
    }


    // If there is a currently-running process, the method simply adds the PCB of the newly-arriving process to the ready queue and the return value is the PCB of the currently running process. As the newly-arriving process is added to the ready queue, its execution start time and execution end time are set to 0, and the remaining burst time is set to the total burst time.
    new_process.execution_endtime = new_process.execution_starttime = 0;
    new_process.remaining_bursttime = new_process.total_bursttime;
    ready_queue[*queue_cnt] = new_process;
    (*queue_cnt)++;
    return current_process;
}
struct PCB handle_process_completion_rr(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp, int time_quantum)
{
    // If the ready queue is empty, the method returns the NULLPCB, indicating that there is no process to execute next. Otherwise, the method finds the PCB of the process in the ready queue with the earliest arrival time, removes this PCB from the ready queue and returns it. Before returning this PCB, it is modified to set the execution start time as the current timestamp and the execution end time as the sum of the current timestamp and the smaller of the time quantum and the remaining burst time.
    if (!(*queue_cnt)) {
        return NULLPCB;
    }

    int next_idx = 0;
    for (int i = 0; i < *queue_cnt; i++)
    {
        struct PCB current = ready_queue[i];
        struct PCB top = ready_queue[next_idx];
        if (current.arrival_timestamp < top.arrival_timestamp)
        {
            next_idx = i;
        }
    }

    struct PCB next_process = ready_queue[next_idx];
    next_process.execution_starttime = timestamp;
    next_process.execution_endtime = timestamp + MIN(time_quantum, next_process.remaining_bursttime);

    for (int i = next_idx; i < *queue_cnt; i++)
    {
        ready_queue[i] = ready_queue[i + 1];
    }

    (*queue_cnt)--;
    return next_process;
}
