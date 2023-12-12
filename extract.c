#include <stdio.h>
#include <stdlib.h>

#define PAGE_TABLE_SIZE 28
#define TLB_SIZE 16
#define FRAME_SIZE 256
#define TOTAL_FRAMES 256
#define PHYSICAL_MEMORY_SIZE (TOTAL_FRAMES * FRAME_SIZE)
#define ADDRESS_MASK 0xFFFF  // Mask for the rightmost 16 bits

typedef struct {
    int isValid;
    int frameNumber;
} PageTableEntry;

typedef struct {
    int pageNumber;
    int frameNumber;
} TLBEntry;

PageTableEntry pageTable[PAGE_TABLE_SIZE];
TLBEntry tlb[TLB_SIZE];

// Function to extract the page number from a logical address
unsigned int getPageNumber(unsigned int logicalAddress) {
    return (logicalAddress >> 8) & 0xFF;  // Extract upper 8 bits
}

// Function to extract the offset from a logical address
unsigned int getOffset(unsigned int logicalAddress) {
    return logicalAddress & 0xFF;  // Extract lower 8 bits
}

// Function to translate logical to physical address
int translateToPhysicalAddress(unsigned int pageNumber, unsigned int offset) {
    // Implement the translation logic
    // This is a simplified placeholder implementation
    int frameNumber = -1;  // Placeholder for frame number

    // Lookup in Page Table
    if (pageNumber < PAGE_TABLE_SIZE && pageTable[pageNumber].isValid) {
        frameNumber = pageTable[pageNumber].frameNumber;
    }

    // Calculate physical address
    return (frameNumber == -1) ? -1 : (frameNumber * FRAME_SIZE) + offset;
}

void readLogicalAddresses(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    unsigned int logicalAddress;
    while (fscanf(file, "%u", &logicalAddress) != EOF) {
        unsigned int maskedAddress = logicalAddress & ADDRESS_MASK;
        unsigned int pageNumber = getPageNumber(maskedAddress);
        unsigned int offset = getOffset(maskedAddress);
        
        int physicalAddress = translateToPhysicalAddress(pageNumber, offset);
        printf("Logical: %u, Physical: %d\n", logicalAddress, physicalAddress);
    }

    fclose(file);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    // Initialize page table
    for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
        pageTable[i].isValid = 1;
        pageTable[i].frameNumber = i;
    }

    // Read and process logical addresses
    readLogicalAddresses(argv[1]);

    return 0;
}
