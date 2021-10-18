#include <stdio.h>
#include "oslabs.h"

#define INT_MAX 2147483647
#define DEBUG_VM 1

void print_request_block(struct RCB request_block)
{
#if DEBUG_VM
    printf(
        "req_id: %d, cylinder: %d, arrival: %d, addr: %d, proc_id: %d\n",
        request_block.request_id,
        request_block.cylinder,
        request_block.arrival_timestamp,

        request_block.address,
        request_block.process_id);
#endif
}

void print_request_queue(struct RCB request_block[QUEUEMAX], int *queue_cnt)
{
#if DEBUG_VM
    printf("******************************************************************\n");
    for (int i = 0; i < *queue_cnt; i++)
    {
        struct RCB current = request_block[i];
        printf("idx: %d, ", i);
        print_request_block(current);
    }
    printf("******************************************************************\n");
#endif
}

struct RCB NULLRCB = {
    .process_id = 0,
    .cylinder = 0,
    .process_id = 0,
    .address = 0,
    .request_id = 0
};

struct RCB remove_request(struct RCB request_queue[QUEUEMAX], int *queue_cnt, int idx)
{
    struct RCB removed = request_queue[idx];

    for (int i = idx; i < *queue_cnt; i++)
    {
        request_queue[i] = request_queue[i + 1];
    }

    (*queue_cnt)--;

    return removed;
}

struct RCB handle_request_arrival_fcfs(struct RCB request_queue[QUEUEMAX], int *queue_cnt, struct RCB current_request, struct RCB new_request, int timestamp) {

    if (current_request.process_id == NULLRCB.process_id){
        return new_request;
    }

    request_queue[(*queue_cnt)++] = new_request;
    
    return current_request;
}
struct RCB handle_request_completion_fcfs(struct RCB request_queue[QUEUEMAX], int *queue_cnt){

    if(*queue_cnt == 0){
        return NULLRCB;
    }

    int earliest_arrival = INT_MAX;
    int earliest_idx = INT_MAX;
    for(int i = 0; i < *queue_cnt; i++){
        struct RCB current = request_queue[i];
        if(current.arrival_timestamp < earliest_arrival){
            earliest_arrival = current.arrival_timestamp;
            earliest_idx = i;    
        }
    }

    return remove_request(request_queue, queue_cnt, earliest_idx);
}
struct RCB handle_request_arrival_sstf(struct RCB request_queue[QUEUEMAX], int *queue_cnt, struct RCB current_request, struct RCB new_request, int timestamp){
    if (current_request.process_id == NULLRCB.process_id)
    {
        return new_request;
    }

    request_queue[(*queue_cnt)++] = new_request;

    return current_request;
}
struct RCB handle_request_completion_sstf(struct RCB request_queue[QUEUEMAX], int *queue_cnt, int current_cylinder){
    if (*queue_cnt == 0)
    {
        return NULLRCB;
    }

    int earliest_arrival = INT_MAX;
    int shortest_distance = INT_MAX;
    int earliest_idx = INT_MAX;
    for (int i = 0; i < *queue_cnt; i++)
    {
        struct RCB current = request_queue[i];
        int distance = abs(current.cylinder -  current_cylinder);
        if (distance <= shortest_distance)
        {
            if( distance < shortest_distance){
                shortest_distance = distance;
                earliest_arrival = current.arrival_timestamp;
                earliest_idx = i;
            } else if (current.arrival_timestamp < earliest_arrival){
                earliest_arrival = current.arrival_timestamp;
                earliest_idx = i;
            }
        }
    }

    return remove_request(request_queue, queue_cnt, earliest_idx);
}
struct RCB handle_request_arrival_look(struct RCB request_queue[QUEUEMAX], int *queue_cnt, struct RCB current_request, struct RCB new_request, int timestamp) {

    if (current_request.process_id == NULLRCB.process_id)
    {
        return new_request;
    }

    request_queue[(*queue_cnt)++] = new_request;

    return current_request;
}
struct RCB handle_request_completion_look(struct RCB request_queue[QUEUEMAX], int *queue_cnt, int current_cylinder, int scan_direction){
    if (*queue_cnt == 0)
    {
        return NULLRCB;
    }

    // If there are requests in the queue with the same cylinder as the current cylinder, the method picks the one among these requests with the earliest arrival time.Otherwise, if the scan direction is 1 and there are requests with cylinders larger than the current cylinder, the method picks the one among these whose cylinder is closest to the current cylinder.Otherwise, if the scan direction is 1 and there are no requests with cylinders larger than the current cylinder, the method picks the request whose cylinder is closest to the current cylinder.Otherwise, if the scan direction is 0 and there are requests with cylinders smaller than the current cylinder, the method picks the one among these whose cylinder is closest to the current cylinder.Otherwise, if the scan direction is 0 and there are requests with cylinders larger than the current cylinder, the method picks the request whose cylinder is closest to the current cylinder.

    int earliest_arrival = INT_MAX;
    int earliest_idx = -1;
    for (int i = 0; i < *queue_cnt; i++)
    {
        struct RCB current = request_queue[i];
        if (current.cylinder == current_cylinder && current.arrival_timestamp < earliest_arrival)
        {
            earliest_arrival = current.arrival_timestamp;
            earliest_idx = i;
        }
    }
    if (earliest_idx != -1){
        return remove_request(request_queue, queue_cnt, earliest_idx);
    }


    int has_request_along_direction = 0;
    int shortest_distance = INT_MAX;
    int next_idx = -1;
    for (int i = 0; i < *queue_cnt; i++)
    {
        struct RCB current = request_queue[i];
        int distance = abs(current.cylinder - current_cylinder);
        if(
            (scan_direction == 1 && current_cylinder < current.cylinder) ||
            (scan_direction == 0 && current_cylinder > current.cylinder)
        ){
            if(! has_request_along_direction){
                has_request_along_direction = 1;
                shortest_distance = distance;
                next_idx = i;
            } else if (distance < shortest_distance){
                shortest_distance = distance;
                next_idx = i;
            }
        } else if ( ! has_request_along_direction && distance < shortest_distance)
        {
            shortest_distance = distance;
            next_idx = i;
        }
    }
    return remove_request(request_queue, queue_cnt, next_idx);
}