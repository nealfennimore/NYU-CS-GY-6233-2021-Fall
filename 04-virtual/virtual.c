#include <stdio.h>
#include "oslabs.h"

#define INT_MAX 2147483647
#define DEBUG_VM 0

void print_page_table(struct PTE page_table)
{
#if DEBUG_VM
    printf(
        "valid: %d, frame_num: %d, arrival: %d, last_access: %d, ref_cnt: %d\n",
        page_table.is_valid,
        page_table.frame_number,
        page_table.arrival_timestamp,
        
        page_table.last_access_timestamp,
        page_table.reference_count);
#endif
}

void print_page_tables(struct PTE page_table[TABLEMAX], int *table_cnt)
{
#if DEBUG_VM
    printf("******************************************************************\n");
    for (int i = 0; i < *table_cnt; i++)
    {
        struct PTE current = page_table[i];
        printf("idx: %d, ", i);
        print_page_table(current);
    }
    printf("******************************************************************\n");
#endif
}

int PAGE_FAULTS = 0;
int INVALID_BIT = -1;
struct PTE GLOBAL_TABLE[TABLEMAX];

int is_cached(struct PTE page_table[TABLEMAX], int page_number) {
    return page_table[page_number].is_valid;
}

int get_usable_frame_number(int frame_pool[POOLMAX], int *frame_cnt){
    int frame_number = frame_pool[0];

    for (int i = 0; i < *frame_cnt; i++)
    {
        frame_pool[i] = frame_pool[i + 1];
    }

    (*frame_cnt)--;

    return frame_number;
}

struct PTE invalidate_page(struct PTE page_table[TABLEMAX], int idx)
{
    struct PTE page = page_table[idx];
    page.is_valid = 0;
    page.arrival_timestamp = INVALID_BIT;
    page.last_access_timestamp = INVALID_BIT;
    if(page.frame_number != INVALID_BIT){
        page.frame_number = INVALID_BIT;
    }
    page.reference_count = INVALID_BIT;

    page_table[idx] = page;
    GLOBAL_TABLE[idx] = page;

    return page;
}

struct PTE set_page(struct PTE page_table[TABLEMAX], int idx, int frame_number, int timestamp, int ref_cnt){
    struct PTE page = page_table[idx];
    page.is_valid = 1;
    page.frame_number = frame_number;
    if (page.arrival_timestamp == INVALID_BIT){
        page.arrival_timestamp = timestamp;
    }
    page.last_access_timestamp = timestamp;
    page.reference_count = ref_cnt;

    page_table[idx] = page;
    GLOBAL_TABLE[idx] = page;

    return page;
}

struct PTE swap_page(struct PTE page_table[TABLEMAX], int page_number, int prev_idx, int timestamp, int ref_cnt){
    int frame_number = page_table[prev_idx].frame_number;
    invalidate_page(page_table, prev_idx);
    return set_page(page_table, page_number, frame_number, timestamp, ref_cnt);
}

int process_page_access_fifo(struct PTE page_table[TABLEMAX], int *table_cnt, int page_number, int frame_pool[POOLMAX], int *frame_cnt, int current_timestamp) {
    // First the function checks if the page being referenced is already in memory (i.e., the page-table entry has the valid bit true). If so, it returns the frame number, after modifying the last_access_timestamp and the reference_count fields of the page-table entry.
    if (is_cached(page_table, page_number)) {
        struct PTE current = page_table[page_number];
        set_page(page_table, page_number, current.frame_number, current_timestamp, current.reference_count + 1);
        return current.frame_number;
    }

    // If the page being referenced is not in memory, the function checks if there are any free frames (i.e., the process frame pool is not empty). If so, a frame is removed from the process frame pool and the frame number is inserted into the page-table entry corresponding to the logical page number. In addition, the other fields of the page-table entry are set appropriately. The function returns the frame number associated with the page-table entry.
    if(*frame_cnt > 0){
        int frame_number = get_usable_frame_number(frame_pool, frame_cnt);
        set_page(page_table, page_number, frame_number, current_timestamp, 1);
        PAGE_FAULTS++;
        return frame_number;
    }

    // // If the page being referenced is not in memory and there are no free frames for the process, a page needs to be replaced. The function selects among all the pages of the process that are currently in memory (i.e., they have valid bits as true) the page that has the smallest arrival_timestamp. It marks that page_table entry as invalid, along with setting the frame_number, arrival_timestamp, last_access_timestamp and reference_count to -1. It then sets the frame_number of the page-table entry of the newly-referenced page to the newly freed frame. It also sets the arrival_timestamp, the last_access_timestamp and the reference_count fields of the page-table entry appropriately. Finally, the function returns this frame number.
    int earliest_timestamp = INT_MAX;
    int earliest_idx = 0;
    for(int i = 0; i < *table_cnt; i++) {
        struct PTE current = page_table[i];
        if(current.is_valid && current.arrival_timestamp < earliest_timestamp){
            earliest_timestamp = current.arrival_timestamp;
            earliest_idx = i;
        }
    }

    struct PTE current = swap_page(page_table, page_number, earliest_idx, current_timestamp, 1);
    PAGE_FAULTS++;
    return current.frame_number;
}

int count_page_faults_fifo(struct PTE page_table[TABLEMAX], int table_cnt, int reference_string[REFERENCEMAX], int reference_cnt, int frame_pool[POOLMAX], int frame_cnt){
    int timestamp = 1;
    for (int i = 0; i < table_cnt; i++)
    {
        GLOBAL_TABLE[i] = page_table[i];
        if(page_table[i].arrival_timestamp > timestamp){
            timestamp = page_table[i].arrival_timestamp;
        }
    }

    PAGE_FAULTS = 0;
    for (int i = 0; i < reference_cnt; i++)
    {
        int page_number = reference_string[i];
        process_page_access_fifo(page_table, &table_cnt, page_number, frame_pool, &frame_cnt, timestamp);

        for (int i = 0; i < table_cnt; i++)
        {
            page_table[i] = GLOBAL_TABLE[i];
        }

        timestamp++;
    }
    return PAGE_FAULTS;
}

int process_page_access_lru(struct PTE page_table[TABLEMAX], int *table_cnt, int page_number, int frame_pool[POOLMAX], int *frame_cnt, int current_timestamp){
    // First the function checks if the page being referenced is already in memory (i.e., the page-table entry has the valid bit true). If so, it returns the frame number, after modifying the last_access_timestamp and the reference_count fields of the page-table entry.
    if (is_cached(page_table, page_number))
    {
        struct PTE current = page_table[page_number];
        set_page(page_table, page_number, current.frame_number, current_timestamp, current.reference_count + 1);
        return current.frame_number;
    }

    // If the page being referenced is not in memory, the function checks if there are any free frames (i.e., the process frame pool is not empty). If so, a frame is removed from the process frame pool and the frame number is inserted into the page-table entry corresponding to the logical page number. In addition, the other fields of the page-table entry are set appropriately. The function returns the frame number associated with the page-table entry.
    if (*frame_cnt > 0)
    {
        int frame_number = get_usable_frame_number(frame_pool, frame_cnt);
        set_page(page_table, page_number, frame_number, current_timestamp, 1);
        PAGE_FAULTS++;
        return frame_number;
    }

    // If the page being referenced is not in memory and there are no free frames for the process, a page needs to be replaced. The function selects among all the pages of the process that are currently in memory (i.e., they have valid bits as true) the page that has the smallest last_access_timestamp. It marks that page-table entry as invalifd, along with setting the frame_number, arrival_timestamp, last_access_timestamp and reference_count to -1. It then sets the frame_number of the page-table entry of the newly-referenced page to the newly freed frame. It also sets the arrival_timestamp, the last_access_timestamp and the reference_count fields of the page-table entry appropriately. Finally, the function returns this frame number. 
    int earliest_timestamp = INT_MAX;
    int earliest_idx = 0;
    for (int i = 0; i < *table_cnt; i++)
    {
        struct PTE current = page_table[i];
        if (current.is_valid && current.last_access_timestamp < earliest_timestamp)
        {
            earliest_timestamp = current.last_access_timestamp;
            earliest_idx = i;
        }
    }

    struct PTE current = swap_page(page_table, page_number, earliest_idx, current_timestamp, 1);
    PAGE_FAULTS++;
    return current.frame_number;
};

int count_page_faults_lru(struct PTE page_table[TABLEMAX], int table_cnt, int reference_string[REFERENCEMAX], int reference_cnt, int frame_pool[POOLMAX], int frame_cnt)
{
    int timestamp = 1;
    for (int i = 0; i < table_cnt; i++)
    {
        GLOBAL_TABLE[i] = page_table[i];
        if (page_table[i].arrival_timestamp > timestamp)
        {
            timestamp = page_table[i].arrival_timestamp;
        }
    }

    PAGE_FAULTS = 0;
    for (int i = 0; i < reference_cnt; i++)
    {
        int page_number = reference_string[i];
        process_page_access_lru(page_table, &table_cnt, page_number, frame_pool, &frame_cnt, timestamp);

        for (int i = 0; i < table_cnt; i++)
        {
            page_table[i] = GLOBAL_TABLE[i];
        }

        timestamp++;
    }
    return PAGE_FAULTS;
};
int process_page_access_lfu(struct PTE page_table[TABLEMAX], int *table_cnt, int page_number, int frame_pool[POOLMAX], int *frame_cnt, int current_timestamp) {
    // First the function checks if the page being referenced is already in memory (i.e., the page-table entry has the valid bit true). If so, it returns the frame number, after modifying the last_access_timestamp and the reference_count fields of the page-table entry.
    if (is_cached(page_table, page_number))
    {
        struct PTE current = page_table[page_number];
        set_page(page_table, page_number, current.frame_number, current_timestamp, current.reference_count + 1);
        return current.frame_number;
    }

    // If the page being referenced is not in memory, the function checks if there are any free frames (i.e., the process frame pool is not empty). If so, a frame is removed from the process frame pool and the frame number is inserted into the page-table entry corresponding to the logical page number. In addition, the other fields of the page-table entry are set appropriately. The function returns the frame number associated with the page-table entry.
    if (*frame_cnt > 0)
    {
        int frame_number = get_usable_frame_number(frame_pool, frame_cnt);
        set_page(page_table, page_number, frame_number, current_timestamp, 1);
        PAGE_FAULTS++;
        return frame_number;
    }

    // If the page being referenced is not in memory and there are no free frames for the process, a page needs to be replaced. The function selects among all the pages of the process that are currently in memory (i.e., they have valid bits as true) the page that has the smallest reference_count. If multiple pages have the smallest reference_count, the one with the smallest arrival_timestamp among these is selected. After selecting the page for replacement, the function marks that page-table entry as invalid, and sets the frame_number, arrival_timestamp, last_access_timestamp and reference_count to -1. It then sets the frame_number of the page-table entry of the newly-referenced page to the newly freed frame. It also sets the arrival_timestamp, the last_access_timestamp and the reference_count fields of the page-table entry appropriately. Finally, the function returns this frame number. 
    int earliest_timestamp = INT_MAX;
    int least_ref_cnt = INT_MAX;
    int earliest_idx = 0;
    for (int i = 0; i < *table_cnt; i++)
    {
        struct PTE current = page_table[i];
        if (current.is_valid && current.reference_count <= least_ref_cnt)
        {
            if (current.reference_count < least_ref_cnt){
                least_ref_cnt = current.reference_count;
                earliest_timestamp = current.arrival_timestamp;
                earliest_idx = i;
            } else if (current.arrival_timestamp < earliest_timestamp){
                earliest_timestamp = current.arrival_timestamp;
                earliest_idx = i;
            }
        }
    }

    struct PTE current = swap_page(page_table, page_number, earliest_idx, current_timestamp, 1);
    PAGE_FAULTS++;
    return current.frame_number;
};

int count_page_faults_lfu(struct PTE page_table[TABLEMAX], int table_cnt, int reference_string[REFERENCEMAX], int reference_cnt, int frame_pool[POOLMAX], int frame_cnt)
{
    int timestamp = 1;
    for (int i = 0; i < table_cnt; i++)
    {
        GLOBAL_TABLE[i] = page_table[i];
        if (page_table[i].last_access_timestamp > timestamp)
        {
            timestamp = page_table[i].last_access_timestamp;
        }
    }

    PAGE_FAULTS = 0;
    for (int i = 0; i < reference_cnt; i++)
    {
        int page_number = reference_string[i];
        process_page_access_lfu(page_table, &table_cnt, page_number, frame_pool, &frame_cnt, timestamp);

        for (int i = 0; i < table_cnt; i++)
        {
            page_table[i] = GLOBAL_TABLE[i];
        }

        timestamp++;
    }
    return PAGE_FAULTS;
};
