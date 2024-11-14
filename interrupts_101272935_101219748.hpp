#ifndef INTERRUPTS_10272935_101219748_HPP
#define INTERRUPTS_10272935_101219748_HPP

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


struct Partition {
    unsigned int number;   
    unsigned int size;    // in MB
    char code[16];   // status: "free" or assigned program name
};

struct PCB {
    int pid; // process ID
    unsigned int cpuTime; 
    unsigned int ioTime; 
    unsigned int remainingCpuTime; // Remaining CPU time
    int partitionNumber; // partition number assigned
    char programName[20]; // name of the program (programN)
};

struct ExternalFile {
    char name[20]; // program name (20 characters)
    unsigned int size; // size of the program when loaded in memory
};

// function prototypes
void initializePartitions(Partition partitions[]);
void assignPartition(Partition partitions[], const char *programName, unsigned int requiredSize);
void initializePCB(PCB pcbs[], int size);
int loadExternalFiles(ExternalFile externalFiles[], const char *filename);

#endif