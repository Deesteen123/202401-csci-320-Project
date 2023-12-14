#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "final.h"

int **pagetable;
char **frames;
int free_frame;
int **TLB;
int queue_idx, pagetable_hits, TLB_hits, total_addrs;

void initialize_pagetable()
{
    pagetable = (int **)calloc(pow(2, 8), sizeof(int *));
    for (int i = 0; i < 256; i++)
    {
        pagetable[i] = (int *)calloc(2, sizeof(int));
    }

    pagetable_hits = 0;
}

void initialize_frames()
{
    frames = (char **)calloc(pow(2, 8), sizeof(char *));
    for (int i = 0; i < 256; i++)
    {
        frames[i] = (char *)calloc(pow(2, 8), sizeof(char));
    }

    free_frame = 0;
}

void initialize_TLB()
{
    TLB = (int **)calloc(16, sizeof(int *));
    for (int i = 0; i < 16; i++)
    {
        TLB[i] = (int *)calloc(2, sizeof(int));
    }

    queue_idx = 0;
    TLB_hits = 0;
    total_addrs = 0;
}

void cleanup()
{
    for (int i = 0; i < 256; i++)
    {
        free(pagetable[i]);
        free(frames[i]);
    }

    for (int i = 0; i < 16; i++)
    {
        free(TLB[i]);
    }

    free(pagetable);
    free(frames);
    free(TLB);
}

unsigned char extract_page_number(int address)
{
    return (address >> 8) & 0x00FF;
}

unsigned char extract_offset(int address)
{
    return address & 0x00FF;
}

int check_pagetable(unsigned char page_number)
{
    if (pagetable[page_number][1] == 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int check_TLB(unsigned char page_number)
{
    for (int i = 0; i < 16; i++)
    {
        if (TLB[i][0] == page_number)
        {
            return TLB[i][1];
        }
    }

    return -1;
}

void process_address(unsigned char offset, unsigned char page_number, int address)
{

    int frame, phys_addr;

    // do TLB check here

    int TLB_res = check_TLB(page_number);

    if (TLB_res == -1)
    {
        // TLB miss occured
        // consult the page table
        if (check_pagetable(page_number))
        {
            // if page table hits then reslove stored physical address

            frame = pagetable[page_number][0];
            phys_addr = (frame * 256) + offset;
            printf("(%d, %d) %d, %d, %d\n", page_number, offset, address, phys_addr, frames[frame][offset]);
            pagetable_hits++;
        }
        else
        {
            // if page table misses go into page fault

            page_fault(offset, page_number);
            frame = free_frame - 1;
            phys_addr = ((frame) * 256) + offset;

            printf("(%d, %d) %d, %d, %d\n", page_number, offset, address, phys_addr, frames[frame][offset]);
        }

        if (queue_idx == 16) {
            queue_idx = 0;
        }

        TLB[queue_idx][0] = page_number;
        TLB[queue_idx][1] = frame;
    }
    else {
        frame = TLB_res;
        phys_addr = (frame * 256) + offset;
        printf("(%d, %d) %d, %d, %d\n", page_number, offset, address, phys_addr, frames[frame][offset]);
        TLB_hits++;
    }

    total_addrs++;
}

void page_fault(unsigned char offset, unsigned char page_number)
{
    char buffer[256];

    FILE *fp = fopen("BACKING_STORE.bin", "rb");
    if (fp == NULL)
    {
        printf("Error: could not open file BACKING_STORE.bin");
    }

    fseek(fp, (page_number * 256), SEEK_SET);
    fread(buffer, 256, 1, fp);

    // for (int i = 0; i < 256; i++) {
    //     printf("%d: %d\n", i, buffer[i]);
    // }

    for (int i = 0; i < 256; i++)
    {
        frames[free_frame][i] = buffer[i];
    }

    pagetable[page_number][0] = free_frame;
    pagetable[page_number][1] = 1;

    free_frame++;
}