#include "interrupts_101272935_101219748.hpp"

void initializePartitions(Partition partitions[]) { // initialize the partitions
    int sizes[6] = {40, 25, 15, 10, 8, 2};
    for (int i = 0; i < 6; i++) {
        partitions[i].number = i + 1;
        partitions[i].size = sizes[i];
        strcpy(partitions[i].code, "free"); // initially everything is free 
    }
}

void assignPartition(Partition partitions[], const char *programName, unsigned int requiredSize) { // find best fit
    int bestIndex = -1;
    unsigned int bestSize = 1000000; //arbitrary max value

    for (int i = 0; i < 6; i++) { //find best fit
        if (strcmp(partitions[i].code, "free") == 0 && partitions[i].size >= requiredSize) {
            if (partitions[i].size < bestSize) {
                bestSize = partitions[i].size; // update best size
                bestIndex = i; // update best index
            }
        }
    }

    if (bestIndex != -1) { // partition found
        strcpy(partitions[bestIndex].code, programName); // mark the partition as occupied by program name
    }
}

void initializePCB(PCB pcbs[], int size) { 
    for (int i = 0; i < size; i++) {
        pcbs[i].pid = -1; // -1 for unused PCB's
        pcbs[i].cpuTime = 0;
        pcbs[i].ioTime = 0;
        pcbs[i].remainingCpuTime = 0;
        pcbs[i].partitionNumber = 0; // or any default value
        strcpy(pcbs[i].programName, ""); // empty
    }

    // init process
    pcbs[0].pid = 11;
    pcbs[0].cpuTime = 0;
    pcbs[0].ioTime = 0;
    pcbs[0].remainingCpuTime = 1; // Assuming it uses 1 MB of CPU time
    pcbs[0].partitionNumber = 6; // Uses Partition 6
    strcpy(pcbs[0].programName, "init"); // Set program name for init process
}

int loadExternalFiles(ExternalFile externalFiles[], const char *filename) { // load external files into externalFiles array
    FILE *file = fopen(filename, "r");

    int count = 0;
    while (fscanf(file, "%20[^,], %u\n", externalFiles[count].name, &externalFiles[count].size) == 2) {
        count++;
    }

    fclose(file);
    return count; // return the number of files loaded
}

void saveSystemStatus(PCB pcbs[], int currentTime) {
    FILE *statusFile = fopen("system_status_10272935_101219748.txt", "a");

    fprintf(statusFile, " ---------------------------------------------!\n");
    fprintf(statusFile, "Save Time: %d ms\n", currentTime);
    fprintf(statusFile, "+--------------------------------------------+\n");
    fprintf(statusFile, "| PID | Program Name | Partition Number| size |\n");
    fprintf(statusFile, "+--------------------------------------------+\n");

    for (int i = 0; i < 16; i++) { // assuming a maximum of 16 PCBs
        if (pcbs[i].pid != -1) {// output the PID on 1 line, with appropriate spacy
            fprintf(statusFile, "| %d | %-14s | %14d | %4d |\n", pcbs[i].pid, pcbs[i].programName, pcbs[i].partitionNumber, pcbs[i].remainingCpuTime);
        }
    }

    fprintf(statusFile, "!---------------------------------------------!\n");
    fclose(statusFile);
}

int main(int argc, char *argv[]) { // ./sim external_files_10272935_101219748.txt trace_10272935_101219748.txt vector_table_10272935_101219748.txt 
    srand(time(NULL));
    char line[24];
    char *word;
    char *comm;
    int duration;
    int mem;
    int tempPartition;
    int tempSize;
    int context_duration;
    int program_count = 0;
    FILE *log = fopen("execution_10272935_101219748.txt", "w"); // output of execution
    FILE *trace = fopen(argv[2], "r"); // trace with Fork and Exec calls
    FILE *file; // this is to access programN.txt
    FILE *statusFile = fopen("system_status_10272935_101219748.txt", "w");
    char vector_table[32][16];
    char *programName;

    int time_elapsed = 0;
    int i = 0;
    int temp_duration;

    Partition partitions[6]; //create partition array
    initializePartitions(partitions); // initialize according to assignment specifics

    PCB pcbs[32]; // array of 32 PCBS
    initializePCB(pcbs, 32); // create the PCB's

    ExternalFile externalFiles[100]; // list of at most 100 external programs
    int numExternalFiles = loadExternalFiles(externalFiles, argv[1]); // load from input external_files_10272935_101219748.txt

    FILE *table = fopen(argv[3], "r"); //vector table

    // load vector table from file
    while (fgets(vector_table[i], sizeof(vector_table[i]), table) != NULL) {
        vector_table[i][strcspn(vector_table[i], "\n")] = '\0';
        i++;
    }
    fclose(table);
    saveSystemStatus(pcbs, time_elapsed);
    
    // trace file processing

    while (fgets(line, sizeof(line), trace) != NULL) {
        line[strcspn(line, "\n")] = 0; //delete new lines

        char *command = strtok(line, ", ");
        char *col2 = strtok(NULL, ", ");

        if (strcmp(command, "FORK") == 0) {
            // handle FORK command
            duration = atoi(col2);
            if (duration <= 0) {
                fprintf(stderr, "Error: Invalid duration for FORK.\n");
                continue;
            }
            fprintf(log, "%u, 1, Switch to kernel mode\n", time_elapsed);
            time_elapsed += 1; 

            context_duration = rand() % 3 + 1;
            fprintf(log, "%u, %u, Context saved\n", time_elapsed, context_duration);
            time_elapsed += context_duration;

            fprintf(log, "%u, 1, Find vector 2 in the memory location 0x0004\n", time_elapsed);
            time_elapsed += 1;

            fprintf(log, "%u, 1, Load address %s into the PC\n", time_elapsed, vector_table[2]);
            time_elapsed += 1;

            duration = rand() % 10 + 1;
            fprintf(log, "%u, %u, FORK: copy parent PCB to child PCB\n", time_elapsed, duration);
            time_elapsed += duration;

            duration = rand() % 10 + 1;
            fprintf(log, "%u, %u, Sceduler called\n", time_elapsed, duration);
            time_elapsed += duration;

            fprintf(log, "%u, %u, IRET\n", time_elapsed, 1);
            time_elapsed += 1;


            PCB temp;
            program_count += 1;
            temp.pid = 11 + program_count;
            temp.cpuTime = 0;
            temp.ioTime = 0;
            temp.remainingCpuTime = 1; // init uses 1MB
            temp.partitionNumber = 6; // uses Partition 6
            strcpy(temp.programName, "init"); // set program name for init process
            pcbs[program_count] = temp;
            saveSystemStatus(pcbs, time_elapsed);

        } 
        else if (strcmp(command, "EXEC") == 0) {
        // handle EXEC command
        programName = col2; // get the program name from column 2
        duration = atoi(strtok(NULL, ", ")); // get duration from next token

        unsigned int programSize = 0;
        for (int j = 0; j < numExternalFiles; j++) {
            if (strcmp(programName, externalFiles[j].name) == 0) { // find the program in the external files table
                programSize = externalFiles[j].size; // get the size of the program
                break;
            }
        }
        fprintf(log, "%u, 1, switch to kernel mode\n", time_elapsed);
        time_elapsed += 1; 

        context_duration = rand() % 3 + 1; 
        fprintf(log, "%u, %u, context saved\n", time_elapsed, context_duration);
        time_elapsed += context_duration; 

        fprintf(log, "%u, 1, find vector 3 in memory position 0x0006\n", time_elapsed);
        time_elapsed += 1;

        fprintf(log, "%u, 1, load address %s into the PC\n", time_elapsed, vector_table[3]);
        time_elapsed += 1; 

        assignPartition(partitions, programName, programSize);

        duration = rand() % 10 + 1;
        fprintf(log, "%u, %u, EXEC: load %s of size %uMB\n", time_elapsed, duration, programName, programSize);
        tempSize = programSize;
        time_elapsed += duration;
        for (int k = 0; k < 6; k++) { // get partition
            if (strcmp(partitions[k].code, programName) == 0) {
                duration = rand() % 10 + 1;
                fprintf(log, "%u, %u, found partition %u with %uMB of space\n", time_elapsed, duration, partitions[k].number, partitions[k].size);
                time_elapsed += duration; 
                tempPartition = partitions[k].number;
                duration = rand() % 10 + 1;
                fprintf(log, "%u, %u, partition %u marked as occupied\n", time_elapsed, duration, partitions[k].number);
                time_elapsed += duration; 
                break; 
            }
        }

        for (int k = 0; k < 16; k++) { // assuming up to 16 processes
            if (pcbs[k].pid == 11 + program_count) { // set PID
                pcbs[k].remainingCpuTime = tempSize; // update with program size
                pcbs[k].partitionNumber = tempPartition; // assign the partition
                strcpy(pcbs[k].programName, programName); // set name
                break;
            }
        }
        duration = rand() % 10 + 1;
        fprintf(log, "%u, %u, updating PCB with new information\n", time_elapsed, duration);
        time_elapsed += duration;

        duration = rand() % 10 + 1;
        fprintf(log, "%u, %u, scheduler called\n", time_elapsed, duration);
        time_elapsed += duration;

        fprintf(log, "%u, %u, IRET\n", time_elapsed, 1);
        time_elapsed += 1;

        saveSystemStatus(pcbs, time_elapsed);

        //after EXEC returns it loads the associated program from externalFiles 
        //and begins processing the instructions within the programN.txt file

        char programFileName[30]; // string buffer for filename
        snprintf(programFileName, sizeof(programFileName), "%s.txt", programName); // construct the file name with .txt
 
        file = fopen(programFileName, "r"); // open the programN.txt file

        
        while(fgets(line, sizeof(line), file) != NULL){ // each loop fully analysis 1 line
        word = strtok(line, ",");
        duration = atoi(strtok(NULL, ","));

        if(strcmp(word, "CPU")){
            comm = strtok(word, " ");
            mem = atoi(strtok(NULL, ","));
        }

        if(!strcmp(word, "CPU")){
            fprintf(log,"%u, %u, CPU execution\n", time_elapsed, duration);
            time_elapsed += duration;
        }

        else if(!strcmp(comm, "SYSCALL")){
            fprintf(log, "%u, 1, Switch to kernel mode\n", time_elapsed);
            time_elapsed += 1; 

            context_duration = rand() % 3 + 1;
            fprintf(log, "%u, %u, Context saved\n", time_elapsed, context_duration);
            time_elapsed += context_duration;

            fprintf(log, "%u, 1, Find vector %u in the memory location 0x000%X\n", time_elapsed, mem, 2*mem);
            time_elapsed += 1;

            fprintf(log, "%u, 1, Load address %s into the PC\n", time_elapsed, vector_table[mem]);
            time_elapsed += 1;

            temp_duration = rand() % (duration/2) + (duration/2);
            fprintf(log, "%u, %u, SYSCALL run the ISR\n", time_elapsed, temp_duration);
            time_elapsed += temp_duration;
            duration -= temp_duration;

            temp_duration = rand() % (2*duration/3);
            fprintf(log, "%u, %u, Transfer data\n", time_elapsed, temp_duration);
            time_elapsed += temp_duration;
            duration -= temp_duration;

            fprintf(log, "%u, %u, Check for errors\n", time_elapsed, duration);
            time_elapsed += duration;

            fprintf(log, "%u, %u, IRET\n", time_elapsed, 1);
            time_elapsed += 1;
        }

        else if(!strcmp(comm, "END_IO")){
            fprintf(log, "%u, %u, Check interupt priority\n", time_elapsed, 1);
            time_elapsed += 1;

            fprintf(log, "%u, %u, Check if masked\n", time_elapsed, 1);
            time_elapsed += 1;

            fprintf(log, "%u, 1, Switch to kernel mode\n", time_elapsed);
            time_elapsed += 1;

            context_duration = rand() % 3 + 1;
            fprintf(log, "%u, %u, Context saved\n", time_elapsed, context_duration);
            time_elapsed += context_duration;

            fprintf(log, "%u, 1, Find vector %u in the memory location 0x000%X\n", time_elapsed, mem, 2*mem);
            time_elapsed += 1;

            fprintf(log, "%u, 1, Load address %s into the PC\n", time_elapsed, vector_table[mem]);
            time_elapsed += 1;

            fprintf(log,"%u, %u, END_IO\n", time_elapsed, duration);
            time_elapsed += duration;

            fprintf(log, "%u, %u, IRET\n", time_elapsed, 1);
            time_elapsed += 1;
        }    
        }
        }
    }
    
    fclose(trace);
    fclose(log);
    return 0;
}
