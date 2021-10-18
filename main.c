#include <stdio.h>
#include "oslabs.h"

void test_memory_best_fit() {
    // Test Failed :
    // 'start: 0, end: 1013, size: 1014, proc: 32
    // start: 1014, end: 2027, size: 1014, proc: 0
    //Missing Block in memory_map for test 1 [0,9,10,32].
    //Here is the memory map.
    //[start_address: 0 end_address: 1013 segment_size: 1014 process_id: 32]
    //[start_address: 1014 end_address: 2027 segment_size: 1014 process_id: 0]'
    int map_cnt = 0;
    struct MEMORY_BLOCK memory_map[MAPMAX];

    int prev_start = 0;
    int addition = 10;
    for (int i = 0; i < MAPMAX - 1; i++)
    {
        struct MEMORY_BLOCK current = {
            .process_id = 0,
            .start_address = prev_start + 1,
            .end_address = prev_start + addition,
            .segment_size = addition};

        memory_map[i] = current;
        map_cnt++;
        prev_start += addition;
    }

    int request_size = 5;
    best_fit_allocate(request_size, memory_map, &map_cnt, 20);
}

void test_memory_best_fit_allocate()
{
    // Test Failed :
    // 'start: 0, end: 1013, size: 1014, proc: 32
    // start: 1014, end: 2027, size: 1014, proc: 0
    //Missing Block in memory_map for test 1 [0,9,10,32].
    //Here is the memory map.
    //[start_address: 0 end_address: 1013 segment_size: 1014 process_id: 32]
    //[start_address: 1014 end_address: 2027 segment_size: 1014 process_id: 0]'
    int map_cnt = 0;
    struct MEMORY_BLOCK memory_map[MAPMAX];

    int prev_start = 0;
    int addition = 10;
    for (int i = 0; i < MAPMAX - 1; i++)
    {
        struct MEMORY_BLOCK current = {
            .process_id = 0,
            .start_address = prev_start + 1,
            .end_address = prev_start + addition,
            .segment_size = addition};

        memory_map[i] = current;
        map_cnt++;
        prev_start += addition;
    }

    int request_size = 5;
    best_fit_allocate(request_size, memory_map, &map_cnt, 20);
}

void test_memory_next_fit()
{
    // Test Failed :
    // 'start: 0, end: 1013, size: 1014, proc: 32
    // start: 1014, end: 2027, size: 1014, proc: 0
    //Missing Block in memory_map for test 1 [0,9,10,32].
    //Here is the memory map.
    //[start_address: 0 end_address: 1013 segment_size: 1014 process_id: 32]
    //[start_address: 1014 end_address: 2027 segment_size: 1014 process_id: 0]'
    int map_cnt = 0;
    struct MEMORY_BLOCK memory_map[MAPMAX];

    int prev_start = 0;
    int addition = 10;
    for (int i = 0; i < MAPMAX - 1; i++)
    {
        struct MEMORY_BLOCK current = {
            .process_id = 0,
            .start_address = prev_start + 1,
            .end_address = prev_start + addition,
            .segment_size = addition};

        memory_map[i] = current;
        map_cnt++;
        prev_start += addition;
    }

    int request_size = 5;
    next_fit_allocate(request_size, memory_map, &map_cnt, 20, 3);
}

void test_virual_memory_page_access_fifo()
{
    // Test Failed : 'Missing entry in page table for test 1 [1,30,12,12,1] page number 0. 
    // Here is the page table. 
    // [is_valid:  0 frame_number: -1 arrival_timestamp: -1 last_access_timestamp: -1 reference_count: -1]
    // [is_valid:  0 frame_number: -1 arrival_timestamp: -1 last_access_timestamp: -1 reference_count: -1]
    // [is_valid:  1 frame_number: 10 arrival_timestamp: 3 last_access_timestamp: 3 reference_count: 1]
    // [is_valid:  0 frame_number: -1 arrival_timestamp: -1 last_access_timestamp: -1 reference_count: -1]
    // [is_valid:  0 frame_number: -1 arrival_timestamp: -1 last_access_timestamp: -1 reference_count: -1]
    // [is_valid:  1 frame_number: 20 arrival_timestamp: 2 last_access_timestamp: 4 reference_count: 2]
    // [is_valid:  0 frame_number: -1 arrival_timestamp: -1 last_access_timestamp: -1 reference_count: -1]
    // [is_valid:  1 frame_number: 30 arrival_timestamp: 12 last_access_timestamp: 12 reference_count: 1]'
    int table_cnt = 8;
    struct PTE page_table[TABLEMAX];

    int prev_start = 0;
    int addition = 10;
    int empty_cnt = 8;
    int frame_cnt = 3;

    for (int i = table_cnt - empty_cnt; i < table_cnt; i++)
    {
        struct PTE current = {
            .is_valid = 0,
            .frame_number = -1,
            .arrival_timestamp = -1,
            .last_access_timestamp = -1,
            .reference_count = -1};

        page_table[i] = current;
        prev_start += addition;
    }

    for (int i = 0; i < table_cnt - empty_cnt; i++)
    {
        struct PTE current = {
            .is_valid = 1,
            .frame_number = i + POOLMAX,
            .arrival_timestamp = prev_start,
            .last_access_timestamp = prev_start,
            .reference_count = 1};

        page_table[i] = current;
        prev_start += addition;
    }

    int page_number = 5;
    int frame_pool[POOLMAX];

    for (int i = 0; i < frame_cnt; i++){
        frame_pool[i] = i;
    }

    int current_timestamp = 200;
    int reference_string[REFERENCEMAX] = {0, 3, 2, 6, 3, 4, 5, 2, 4, 5, 6};
    int reference_cnt = 11;

    count_page_faults_fifo(page_table, table_cnt, reference_string, reference_cnt, frame_pool, frame_cnt);
}

int main(int argc, char const *argv[])
{
    test_virual_memory_page_access_fifo();
    return 0;
}