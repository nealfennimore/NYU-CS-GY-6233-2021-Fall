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

int main(int argc, char const *argv[])
{
    test_memory_best_fit();
    return 0;
}