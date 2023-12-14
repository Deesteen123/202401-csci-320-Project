#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "final.h"

extern int TLB_hits, pagetable_hits, total_addrs;

int main(int argc, char *argv[]) {

    char *str = argv[1];
    unsigned char offset, page_number;
    int address;

    initialize_pagetable();
    initialize_frames();
    initialize_TLB();

    FILE *fp = fopen(str, "r");

    if (fp == NULL) {
        printf("Error: could not open file %s", str);
        return 0;
    }

    char curr_address[7];

    while (fgets(curr_address, sizeof(curr_address), fp)) {
        address = atoi(curr_address);
        offset = extract_offset(address);
        page_number = extract_page_number(address);
        process_address(offset, page_number, address);
    }

    printf("TLB Hits: %d\nPage Table Hits: %d\nTotal Addresses Translated: %d\n", TLB_hits, pagetable_hits, total_addrs);

    fclose(fp);
    cleanup();
}