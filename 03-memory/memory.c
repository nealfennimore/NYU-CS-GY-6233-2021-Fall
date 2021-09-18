#include <stdio.h>
#include "oslabs.h"

#define INT_MAX 2147483647
#define FREE 0

void print_memory_map(struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt)
{
    #if DEBUG
        printf("******************************************************************\n");
        for (int i = 0; i < *map_cnt; i++)
        {
            struct MEMORY_BLOCK current = memory_map[i];
            printf("idx: %d, start: %d, end: %d, size: %d, proc: %d\n", i, current.start_address, current.end_address, current.segment_size, current.process_id);
        }
        printf("******************************************************************\n");
    #endif
}

struct MEMORY_BLOCK split_memory_block(int idx, int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, int process_id)
{
    struct MEMORY_BLOCK selected = memory_map[idx];

    int next_segment_size = selected.segment_size - request_size;

    selected.segment_size = request_size;
    selected.end_address = selected.start_address + request_size - 1;
    selected.process_id = process_id;
    memory_map[idx] = selected;

    struct MEMORY_BLOCK next_block =
        {
            .start_address = selected.end_address + 1,
            .end_address = selected.end_address + next_segment_size,
            .segment_size = next_segment_size,
            .process_id = FREE,
        };

    struct MEMORY_BLOCK former;
    struct MEMORY_BLOCK next = next_block;
    (*map_cnt)++;
    for (int i = idx + 1; i < *map_cnt; i++)
    {
        former = memory_map[i];
        memory_map[i] = next;
        next = former;
    }

    return selected;
}

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
                block.process_id = process_id;
                memory_map[i] = block;
                return block;
            } else if (block.segment_size > request_size && block.segment_size < best_fit_size) {
                best_fit_size = block.segment_size;
                best_fit_idx = i;
            }
        }
    }

    if (best_fit_idx == -1 || *map_cnt + 1 > MAPMAX) {
        return NULLBLOCK;
    }

    print_memory_map(memory_map, map_cnt);

    return split_memory_block(best_fit_idx, request_size, memory_map, map_cnt, process_id);
}

// This method allocates memory according to the First Fit scheme. The method is given the process id of the requesting process, size of the memory being requested, and the memory map. It finds the first (lowest starting address) free memory block whose size is at least as large as the requested size. If the free block found is exactly of the same size as the requested size, the method updates the process id to allocate it and returns this memory block. If the free block found is larger than the requested size, the block is split into two pieces - the first piece allocated and the second piece becoming a free block in the memory map. Thus, the method may alter the memory map appropriately. Note that if there is no free block of memory (in the memory map) that is at least as large as the requested size, the method returns the NULLBLOCK.
struct MEMORY_BLOCK first_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, int process_id) {
    int first_fit_idx = -1;

    for (int i = 0; i < *map_cnt; i++)
    {
        struct MEMORY_BLOCK block = memory_map[i];
        if (block.process_id == FREE) {
            if (block.segment_size == request_size)
            {
                block.process_id = process_id;
                memory_map[i] = block;
                return block;
            } else if (block.segment_size > request_size) {
                first_fit_idx = i;
                break;
            }
        }
    }

    if (first_fit_idx == -1 || *map_cnt + 1 > MAPMAX) {
        return NULLBLOCK;
    }

    print_memory_map(memory_map, map_cnt);
    struct MEMORY_BLOCK first_fit_block = split_memory_block(first_fit_idx, request_size, memory_map, map_cnt, process_id);
    print_memory_map(memory_map, map_cnt);
    return first_fit_block;
}

// This method allocates memory according to the Worst Fit scheme. The method is given the process id of the requesting process, size of the memory being requested, and the memory map. It finds the candidate memory blocks that can be allocated and chooses the largest among these blocks. If the free block found is exactly of the same size as the requested size, the method updates the process id to allocate it and returns this memory block. If the free block found is larger than the requested size, the block is split into two pieces - the first piece allocated and the second piece becoming a free block in the memory map. Thus, the method may alter the memory map appropriately. Note that if there is no free block of memory (in the memory map) that is at least as large as the requested size, the method returns the NULLBLOCK.
struct MEMORY_BLOCK worst_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, int process_id) {
    int worst_fit_idx = -1;
    int worst_fit_size = 0;

    for (int i = 0; i < *map_cnt; i++)
    {
        struct MEMORY_BLOCK block = memory_map[i];
        if (block.process_id == FREE) {
            if (block.segment_size == request_size)
            {
                block.process_id = process_id;
                memory_map[i] = block;
                return block;
            } else if (block.segment_size > request_size && block.segment_size > worst_fit_size) {
                worst_fit_size = block.segment_size;
                worst_fit_idx = i;
            }
        }
    }

    if (worst_fit_idx == -1 || *map_cnt + 1 > MAPMAX) {
        return NULLBLOCK;
    }

    print_memory_map(memory_map, map_cnt);

    return split_memory_block(worst_fit_idx, request_size, memory_map, map_cnt, process_id);
}

// This method allocates memory according to the Next Fit scheme. The method is given the process id of the requesting process, size of the memory requested, the memory map, and the address of the last block allocated. It finds the first (lowest starting address) free memory block, greater than or equal to the previously allocated block address, whose size is at least as the requested size. If the free block found is exactly of the same size as the requested size, the method updates the process id to allocate it and returns this memory block. If the free block found is larger than the requested size, the block is split into two pieces - the first piece allocated and the second piece becoming a free block in the memory map. Thus, the method may alter the memory map appropriately. Note that if there is no free block of memory (in the memory map) that is at least as large as the requested size, the method returns the NULLBLOCK.
struct MEMORY_BLOCK next_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, int process_id, int last_address) {
    int next_fit_idx = -1;

    for (int i = 1; i < *map_cnt; i++)
    {
        int j = (i + last_address) % *map_cnt;
        struct MEMORY_BLOCK block = memory_map[j];
        if (block.process_id == FREE) {
            if (block.segment_size == request_size)
            {
                block.process_id = process_id;
                memory_map[j] = block;
                return block;
            } else if (block.segment_size > request_size) {
                next_fit_idx = j;
                break;
            }
        }
    }

    if (next_fit_idx == -1 || *map_cnt + 1 > MAPMAX) {
        return NULLBLOCK;
    }

    print_memory_map(memory_map, map_cnt);
    struct MEMORY_BLOCK next_fit_block = split_memory_block(next_fit_idx, request_size, memory_map, map_cnt, process_id);
    print_memory_map(memory_map, map_cnt);
    return next_fit_block;
}

// This method releases a memory block. Accordingly, it modifies the memory map passed in. Specifically, it marks the released block of memory as free and then it merges that block with adjacent free blocks if any. That is, if the memory block adjacent to the newly released block is free, the memory map is altered to reduce the number of memory blocks by one and the ending address (and the size) of the previous free block extended. Note that the method does not have any explicit return value and instead modifies the memory map passed in.
void release_memory(struct MEMORY_BLOCK freed_block, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt) {
    int block_to_free_idx = -1;

    for (int i = 0; i < *map_cnt; i++){
        struct MEMORY_BLOCK current = memory_map[i];
        if(current.start_address == freed_block.start_address && current.end_address == freed_block.end_address){
            block_to_free_idx = i;
            break;
        }
    }

    if(block_to_free_idx >= 0){
        int prev_idx = block_to_free_idx - 1;
        int next_idx = block_to_free_idx + 1;

        int start_address = freed_block.start_address;
        int end_address = freed_block.end_address;
        int segment_size = freed_block.segment_size;
        int start_merge_idx = block_to_free_idx;
        int end_merge_idx = block_to_free_idx;

        if(prev_idx >= 0 && memory_map[prev_idx].process_id == FREE){
            struct MEMORY_BLOCK prev = memory_map[prev_idx];
            start_address = prev.start_address;
            segment_size += prev.segment_size;
            start_merge_idx = block_to_free_idx;
        }

        if(next_idx < *map_cnt && memory_map[next_idx].process_id == FREE){
            struct MEMORY_BLOCK next = memory_map[next_idx];
            end_address = next.start_address;
            segment_size += next.segment_size;
            end_merge_idx = block_to_free_idx;
        }

        int blocks_to_merge_cnt = end_merge_idx - start_merge_idx;

        struct MEMORY_BLOCK merged_block = {
            .start_address = start_address,
            .end_address = end_address,
            .segment_size = segment_size,
            .process_id = FREE
        };

        for(int i = start_merge_idx + 1; i < *map_cnt - blocks_to_merge_cnt; i++){
            memory_map[i] = memory_map[i + blocks_to_merge_cnt];
        }
    }
}


