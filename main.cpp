#include <iostream>
#include <fstream>
#include <assert.h>
#include "graph.h"

// store the trace name here
char trace_string[1024];
char gunzip_command[1024];

uint64_t champsim_seed;

// the trace file
FILE *trace_file;

// the profile
FILE *profile;

#define NUM_CPUS 1
#define NUM_INSTR_DESTINATIONS_SPARC 4
#define NUM_INSTR_DESTINATIONS 2
#define NUM_INSTR_SOURCES 4

using namespace std;

class input_instr {
  public:

    // instruction pointer or PC (Program Counter)
    uint64_t ip;

    // branch info
    uint8_t is_branch;
    uint8_t branch_taken;

    uint8_t destination_registers[NUM_INSTR_DESTINATIONS]; // output registers
    uint8_t source_registers[NUM_INSTR_SOURCES]; // input registers

    uint64_t destination_memory[NUM_INSTR_DESTINATIONS]; // output memory
    uint64_t source_memory[NUM_INSTR_SOURCES]; // input memory

    input_instr() {
        ip = 0;
        is_branch = 0;
        branch_taken = 0;

        for (uint32_t i=0; i<NUM_INSTR_SOURCES; i++) {
            source_registers[i] = 0;
            source_memory[i] = 0;
        }

        for (uint32_t i=0; i<NUM_INSTR_DESTINATIONS; i++) {
            destination_registers[i] = 0;
            destination_memory[i] = 0;
        }
    };
};

int read_from_trace() {

    input_instr trace_read_instr;
    char instr[255];
    size_t instr_size = sizeof(input_instr);

    if (!fread(&instr, sizeof(instr), 1, trace_file))
    {
        // reached end of file for this trace
        cout << "*** Reached end of trace " << endl; 

        // close the trace file 
        pclose(trace_file);
        
        // exit
        return 0;
    }
    else
    {
        // cout << hex << "0x" << trace_read_instr.ip << dec << endl;
        printf("%s", instr); 
        return 1;
    }
    
}

uint64_t search_last_occurence(uint64_t miss_pc) {

    input_instr cur_instr;
    input_instr last_occur;
    size_t instr_size = sizeof(input_instr);
    bool found = 0;

    if (!fread(&cur_instr, instr_size, 1, trace_file))
    {
        // reached end of file for this trace 
        // close the trace file 
        pclose(trace_file);
        
        // exit
        if (found)
            return last_occur.ip;
        else
            return 0;

    }
    else
    {
        // cout << hex << "0x" << trace_read_instr.ip << dec << endl;
        if (miss_pc == cur_instr.ip)
        {
            last_occur = cur_instr;
            found = 1;
        }
    }
}



int main(int argc, char** argv)
{

    char miss_instr[255];

    for (int i=0; i<argc; i++) {
        cout << "num of arg: " << i << " arg: " << argv[i] << endl;
    }

    sprintf(trace_string, "%s", argv[2]);

    std::string full_name(argv[2]);
    std::string last_dot = full_name.substr(full_name.find_last_of("."));

    std::string fmtstr;
    std::string decomp_program;

    int status;

    if (full_name.substr(0,4) == "http")
    {
        // Check file exists
        char testfile_command[4096];
        sprintf(testfile_command, "wget -q --spider %s", argv[2]);
        FILE *testfile = popen(testfile_command, "r");
        if (pclose(testfile))
        {
            std::cerr << "TRACE FILE NOT FOUND" << std::endl;
            assert(0);
        }
        fmtstr = "wget -qO- %2$s | %1$s -dc";
    }
    else
    {
        std::ifstream testfile(argv[2]);
        if (!testfile.good())
        {
            std::cerr << "TRACE FILE NOT FOUND" << std::endl;
            assert(0);
        }
        fmtstr = "%1$s -dc %2$s";
    }

    if (last_dot[1] == 'g') // gzip format
        decomp_program = "gzip";
    else if (last_dot[1] == 'x') // xz
        decomp_program = "xz";
    else {
        std::cout << "ChampSim does not support traces other than gz or xz compression!" << std::endl;
        assert(0);
    }

    sprintf(gunzip_command, fmtstr.c_str(), decomp_program.c_str(), argv[2]);

    trace_file = popen(gunzip_command, "r");
    if (trace_file == NULL) {
        printf("\n*** Trace file not found: %s ***\n\n", argv[2]);
        assert(0);
    }

    // if (count_traces != NUM_CPUS) {
    //     printf("\n*** Not enough traces for the configured number of cores ***\n\n");
    //     assert(0);
    // }

    profile = fopen(argv[3], "r");

    while (fgets(miss_instr, sizeof(miss_instr), profile)) {
        /* note that fgets don't strip the terminating \n, checking its
           presence would allow to handle lines longer that sizeof(line) */
        printf("%s", miss_instr); 
    }
    
    fclose(profile);

    int count = 0;

    cout << sizeof(input_instr) << endl;

    while (read_from_trace())
    {
        count += read_from_trace();
    }

    
    return 0;
}
