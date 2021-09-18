#include <stdio.h>
#include "oslabs.h"

#define INT_MAX 2147483647
#define FREE 0

struct MEMORY_BLOCK NULLBLOCK = {
    .start_address = 0,
    .end_address = 0,
    .segment_size = 0,
    .process_id = 0
};

// This method allocates memory according to the Best Fit scheme. The method is given the process id of the requesting process, size of the memory being requested, and the memory map. It finds the candidate memory blocks that can be allocated and chooses the one whose size is closest to the requested size. If the free block found is exactly of the same size as the requested size, the method updates the process id to allocate it and returns this memory block. If the free block found is larger than the requested size, the block is split into two pieces - the first piece allocated and the second piece becoming a free block in the memory map. Thus, the method may alter the memory map appropriately. Note that if there is no free block of memory (in the memory map) that is at least as large as the requested size, the method returns the NULLBLOCK.
struct MEMORY_BLOCK best_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, int process_id) {


    int best_fit_idx = -1;
    int best_fit_size = INT_MAX;

    for (int i = 0; i < *map_cnt; i++)
    {
        struct MEMORY_BLOCK block = memory_map[i];
        if (block.process_id == FREE) {
            if (block.segment_size == request_size)
            {
                // printf("block found same size: start %d, end %d, size: %d\n", block.start_address, block.end_address, block.segment_size);
                block.process_id = process_id;
                return block;
            } else if (block.segment_size > request_size && block.segment_size < best_fit_size) {
                // printf("Setting %d, %d, %d, %d\n", block.segment_size, block.process_id, block.start_address, block.end_address);
                best_fit_size = block.segment_size;
                best_fit_idx = i;
            }
        }
    }

    if (best_fit_idx == -1 || *map_cnt + 1 > MAPMAX) {
        return NULLBLOCK;
    }

    struct MEMORY_BLOCK selected_block = memory_map[best_fit_idx];

    // printf("prev block start: %d, end %d, size %d, proc %d\n", selected_block.start_address, selected_block.end_address, selected_block.segment_size, selected_block.process_id);

    int next_segment_size = selected_block.segment_size - request_size;

    selected_block.segment_size -= request_size;
    selected_block.end_address -= request_size;
    selected_block.process_id = process_id;
    memory_map[best_fit_idx] = selected_block;

    int prev_end_address = selected_block.end_address;
    struct MEMORY_BLOCK next_block =
    {
        .start_address = prev_end_address + 1,
        .end_address = prev_end_address + next_segment_size,
        .segment_size = next_segment_size,
        .process_id = FREE,
    };


    // printf("current block start: %d, end %d, size %d, proc %d\n", selected_block.start_address, selected_block.end_address, selected_block.segment_size, selected_block.process_id);
    // printf("stored block start: %d, end %d, size %d, proc %d\n", memory_map[best_fit_idx].start_address, memory_map[best_fit_idx].end_address, memory_map[best_fit_idx].segment_size, memory_map[best_fit_idx].process_id);
    // printf("next block start: %d, end %d, size %d, proc %d\n", next_block.start_address, next_block.end_address, next_block.segment_size, next_block.process_id);

    struct MEMORY_BLOCK former;
    struct MEMORY_BLOCK next = next_block;
    for (int i = best_fit_idx + 1; i < *map_cnt + 1; i++)
    {
        former = memory_map[i];
        memory_map[i] = next;
        next = former;   
    }
    (*map_cnt)++;


    for (int i = 0; i < *map_cnt; i++)
    {
        struct MEMORY_BLOCK current = memory_map[i];
        printf("start: %d, end: %d, size: %d, proc: %d\n", current.start_address, current.end_address, current.segment_size, current.process_id);
    }

    return selected_block;
}

struct MEMORY_BLOCK first_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, int process_id) {}

struct MEMORY_BLOCK worst_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, int process_id) {}

struct MEMORY_BLOCK next_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, int process_id, int last_address) {}

void release_memory(struct MEMORY_BLOCK freed_block, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt) {}


int main(int argc, char const *argv[])
{
    int map_cnt = 0;
    struct MEMORY_BLOCK memory_map[MAPMAX];

    int prev_start = 0;
    int addition = 10;
    for(int i = 0; i < MAPMAX - 1; i++){
        struct MEMORY_BLOCK current = {
            .process_id = FREE,
            .start_address = prev_start + 1,
            .end_address = prev_start + addition,
            .segment_size = addition
        };

        // printf("start: %d, end: %d, size: %d, proc: %d\n", current.start_address, current.end_address, current.segment_size, current.process_id);

        memory_map[i] = current;
        map_cnt++;
        prev_start += addition;
    }


    int request_size = 5;
    best_fit_allocate(request_size, memory_map, &map_cnt, 20);

    return 0;
}
