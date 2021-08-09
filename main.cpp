#include <iostream>
#include <fstream>
#include <assert.h>
#include <deque>
#include "graph.h"
#include "instruction.h"

// store the trace name here
char trace_string[1024];
char gunzip_command[1024];

uint64_t champsim_seed;
uint64_t load_op = 69698705567;
uint64_t add_op = 1691269;

// the trace file
FILE *trace_file;

// the profile
FILE *profile;

// ./main -traces ../ChampSim/dpc3_traces/600.perlbench_s-210B.champsimtrace.xz examp.txt
// g++ -g -Wall -std=c++11 -o main main.cpp

struct Source
{
    bool is_mem;
    uint8_t reg;
    uint64_t mem;
};

using namespace std;

int read_from_trace() {

    input_instr trace_read_instr;
    char instr[255];
    size_t instr_size = sizeof(input_instr);

    if (!fread(&trace_read_instr, instr_size, 1, trace_file))
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
        // cout << hex << "0x" << trace_read_instr.ip << dec;
        // cout << " from register: " << trace_read_instr.source_registers[0] << " and " << trace_read_instr.source_registers[1];
        // cout << " from mem loc: " << hex << "0x" << trace_read_instr.source_memory[0] << dec << " and " << trace_read_instr.source_memory[1] << endl;
        return 1;
    }
    
}

ooo_model_instr copy_into_format (input_instr current_instr) {
    
    // copy the instruction into the performance model's instruction format
    ooo_model_instr arch_instr;
    int num_reg_ops = 0, num_mem_ops = 0;

    // arch_instr.instr_id = instr_unique_id;
    arch_instr.ip = current_instr.ip;
    arch_instr.op = current_instr.op;
    arch_instr.is_branch = current_instr.is_branch;
    arch_instr.branch_taken = current_instr.branch_taken;

    // arch_instr.asid[0] = cpu;
    // arch_instr.asid[1] = cpu;

    bool reads_sp = false;
    bool writes_sp = false;
    bool reads_flags = false;
    bool reads_ip = false;
    bool writes_ip = false;
    bool reads_other = false;

    for (uint32_t i=0; i<NUM_INSTR_DESTINATIONS; i++) {
        arch_instr.destination_registers[i] = current_instr.destination_registers[i];
        arch_instr.destination_memory[i] = current_instr.destination_memory[i];
        arch_instr.destination_virtual_address[i] = current_instr.destination_memory[i];

        switch(arch_instr.destination_registers[i]) {
            case 0:
                break;
            case REG_STACK_POINTER:
                writes_sp = true;
                break;
            case REG_INSTRUCTION_POINTER:
                writes_ip = true;
                break;
            default:
                break;
        }

        /*
        if((arch_instr.is_branch) && (arch_instr.destination_registers[i] > 24) && (arch_instr.destination_registers[i] < 28))
            {
        arch_instr.destination_registers[i] = 0;
            }
        */
        
        if (arch_instr.destination_registers[i])
            num_reg_ops++;
        if (arch_instr.destination_memory[i]) {
            num_mem_ops++;

        }
    }
    for (int i=0; i<NUM_INSTR_SOURCES; i++) {
        arch_instr.source_registers[i] = current_instr.source_registers[i];
        arch_instr.source_memory[i] = current_instr.source_memory[i];
        arch_instr.source_virtual_address[i] = current_instr.source_memory[i];

        switch(arch_instr.source_registers[i]) {
            case 0:
                break;
            case REG_STACK_POINTER:
                reads_sp = true;
                break;
            case REG_FLAGS:
                reads_flags = true;
                break;
            case REG_INSTRUCTION_POINTER:
                reads_ip = true;
                break;
            default:
                reads_other = true;
                break;
        }
        
        /*
        if((!arch_instr.is_branch) && (arch_instr.source_registers[i] > 25) && (arch_instr.source_registers[i] < 28))
            {
        arch_instr.source_registers[i] = 0;
            }
        */
        
        if (arch_instr.source_registers[i])
            num_reg_ops++;
        if (arch_instr.source_memory[i])
            num_mem_ops++;
    }
    
    arch_instr.num_reg_ops = num_reg_ops;
    arch_instr.num_mem_ops = num_mem_ops;
    if (num_mem_ops > 0) 
        arch_instr.is_memory = 1;

    // determine what kind of branch this is, if any
    if(!reads_sp && !reads_flags && writes_ip && !reads_other)
    {
        // direct jump
        arch_instr.is_branch = 1;
        arch_instr.branch_taken = 1;
        arch_instr.branch_type = BRANCH_DIRECT_JUMP;
    }
    else if(!reads_sp && !reads_flags && writes_ip && reads_other)
    {
        // indirect branch
        arch_instr.is_branch = 1;
        arch_instr.branch_taken = 1;
        arch_instr.branch_type = BRANCH_INDIRECT;
    }
    else if(!reads_sp && reads_ip && !writes_sp && writes_ip && reads_flags && !reads_other)
    {
        // conditional branch
        arch_instr.is_branch = 1;
        arch_instr.branch_taken = arch_instr.branch_taken; // don't change this
        arch_instr.branch_type = BRANCH_CONDITIONAL;
    }
    else if(reads_sp && reads_ip && writes_sp && writes_ip && !reads_flags && !reads_other)
    {
        // direct call
        arch_instr.is_branch = 1;
        arch_instr.branch_taken = 1;
        arch_instr.branch_type = BRANCH_DIRECT_CALL;
    }
    else if(reads_sp && reads_ip && writes_sp && writes_ip && !reads_flags && reads_other)
    {
        // indirect call
        arch_instr.is_branch = 1;
        arch_instr.branch_taken = 1;
        arch_instr.branch_type = BRANCH_INDIRECT_CALL;
    }
    else if(reads_sp && !reads_ip && writes_sp && writes_ip)
    {
        // return
        arch_instr.is_branch = 1;
        arch_instr.branch_taken = 1;
        arch_instr.branch_type = BRANCH_RETURN;
    }
    else if(writes_ip)
    {
        // some other branch type that doesn't fit the above categories
        arch_instr.is_branch = 1;
        arch_instr.branch_taken = arch_instr.branch_taken; // don't change this
        arch_instr.branch_type = BRANCH_OTHER;
    }

    // total_branch_types[arch_instr.branch_type]++;
    
    // if((arch_instr.is_branch == 1) && (arch_instr.branch_taken == 1))
    // {
    //     arch_instr.branch_target = next_instr.ip;
    // }

    return arch_instr;

}

deque<ooo_model_instr> search_last_occurence(uint64_t miss_pc) {

    // to read in from the trace file
    input_instr current_instr_read;
    input_instr last_occur;
    // structure better for program use
    ooo_model_instr current_instr;
    ooo_model_instr res;
    size_t instr_size = sizeof(input_instr);
    // signaling whether ip is found
    bool found = 0;

    deque<ooo_model_instr> window;
    int max_window_size = 1000;

    while (fread(&current_instr_read, instr_size, 1, trace_file))
    {
        current_instr = copy_into_format(current_instr_read);
        if (window.size() < max_window_size) {
            window.push_front(current_instr);
        }
        else {
            window.pop_back();
            window.push_front(current_instr);
        }
        

        if (miss_pc == current_instr_read.ip) {
            found = 1;
            break;
        }

    }

    pclose(trace_file);
        
    // exit
    if (found) {
        return window;
    } 
    else
        // if the miss_pc was not found, return an empty deque;
        return deque<ooo_model_instr>();

}


// TODO:
uint64_t find_source(input_instr i) {
    return 0;
}

// Search the last occurence of register reg
// if not found, return -1
int traceback_reg(uint8_t reg, deque<ooo_model_instr> trace_window, int index) {
    
    int size = trace_window.size();
    ooo_model_instr cur_instr;
    bool found = -1;

    for (int i = index; i < size; i++)
    {
        cur_instr = trace_window.at(i);
        if(cur_instr.source_registers[0] == reg) {
            found = 1;
            return i;
        }
    }

    return -1;
    
}

int traceback_ea(uint64_t ea, deque<ooo_model_instr> trace_window, int index) {
    
    int size = trace_window.size();
    ooo_model_instr cur_instr;
    bool found = -1;

    for (int i = index; i < size; i++)
    {
        cur_instr = trace_window.at(i);
        if(cur_instr.source_memory[0] == ea) {
            found = 1;
            return i;
        }
    }

    return -1;
    
}

long long int find_const_offset(ooo_model_instr cur_instr) {
    if (cur_instr.offset1 != -1 && cur_instr.offset1 != 0) 
        return cur_instr.offset1;
    else if (cur_instr.offset2 != -1 && cur_instr.offset2 != 0) 
        return cur_instr.offset2;
    else
        return -17;
}

// Builds the graph
// Returns the descriptor of the sink of the graph
vertex_descriptor_t build_graph(deque<ooo_model_instr> trace_window, Graph *g, uint64_t miss_pc) {

    bool complete = false;
    int cur_index = 0;

    ooo_model_instr cur_instr = NULL;
    cout << "got here\n";
    vertex_descriptor_t cur_root_vertex = add_vertex(g, cur_instr.source_memory[0], 0, ADDR);

    vertex_descriptor_t the_root_vertex = cur_root_vertex;

    vertex_descriptor_t load_op_vertex;
    vertex_descriptor_t add_op_vertex;
    vertex_descriptor_t const_vertex;
    vertex_descriptor_t reg_vertex;
    vertex_descriptor_t ea_vertex;

    int reg_index, ea_index;

    long long int offset;
    
    // backtrack in the trace window for another dependence
    while (1) {
        cout << "loop once\n";
        cur_instr = trace_window.at(cur_index);
        
        // the memory op number at which there is a discrepency between ea and reg value
        offset = find_const_offset(cur_instr);

        // if we come across the miss PC again, stop
        if (cur_instr.ip == miss_pc && cur_index != 0) 
            break;
            
        // This means that we found a constant offset
        if (offset != -17) {
            
            // first add the ADD vertex into the graph, because we need to add the offset as 
            // part of the kernel
            add_op_vertex = add_vertex(g, add_op, 0, NONTERM);
            // connect it to the current root
            add_edge(g, cur_root_vertex, add_op_vertex);

            // add the children of ADD vertex, the first one is of course the offset
            const_vertex = add_vertex(g, offset, 0, CONST);
            add_edge(g, add_op_vertex, const_vertex);
            
            // When the current instruction fetch data from memory
            if (cur_instr.is_memory) {
                // find the last occurence of the current effective address
                ea_index = traceback_ea(cur_instr.source_memory[0], trace_window, cur_index);
                
                // set the current root to a new value, notice that it is a
                // load node
                cur_root_vertex = add_vertex(g, load_op, cur_instr.source_memory[0], NONTERM);
                
                // when there does not exist a previous occurence
                if (ea_index == -1) {
                    ea_vertex = add_vertex(g, cur_instr.source_memory[0], 0, ADDR);
                    add_edge(g, cur_root_vertex, ea_vertex);
                    break;
                }
                else {
                    cur_index = ea_index;
                    continue;
                }
            }

            // when the source is not from main memory, search for occurrence 
            // of the register
            else {
                // traceback to find the register
                reg_index = traceback_reg(cur_instr.source_registers[0], trace_window, cur_index);
                
                // set the current root to a new value
                cur_root_vertex = add_vertex(g, load_op, cur_instr.source_registers[0], NONTERM);
                add_edge(g, add_op_vertex, cur_root_vertex);

                // the register cannot be traced back further
                if (reg_index == -1) {
                    reg_vertex = add_vertex(g, cur_instr.source_registers[0], 0, REG);
                    add_edge(g, cur_root_vertex, reg_vertex);
                    break;
                }
                else {
                    cur_index = reg_index;
                    continue;
                }
                
            }
            
        }
        else {
            if (cur_instr.is_memory) {
                ea_index = traceback_ea(cur_instr.source_memory[0], trace_window, cur_index);

                load_op_vertex = add_vertex(g, load_op, cur_instr.source_memory[0], NONTERM);
                add_edge(g, cur_root_vertex, load_op_vertex);
                cur_root_vertex = load_op_vertex;

                if (ea_index == -1)
                {
                    ea_vertex = add_vertex(g, cur_instr.source_memory[0], 0, ADDR);
                    add_edge(g, cur_root_vertex, ea_vertex);
                    break;
                }
                else
                {
                    cur_index = ea_index;
                    continue;
                }
            }
            else {
                // traceback to find the register
                reg_index = traceback_reg(cur_instr.source_registers[0], trace_window, cur_index);

                load_op_vertex = add_vertex(g, load_op, cur_instr.source_registers[0], NONTERM);
                add_edge(g, cur_root_vertex, load_op_vertex);
                cur_root_vertex = load_op_vertex;
                
                // the register cannot be traced back further
                if (reg_index == -1)
                {
                    reg_vertex = add_vertex(g, cur_instr.source_registers[0], 0, REG);
                    add_edge(g, cur_root_vertex, reg_vertex);
                    break;
                }
                else
                {
                    cur_index = reg_index;
                    continue;
                }
            }
            
        }
        
        
    }
    
    return the_root_vertex;
    
}

int main(int argc, char** argv)
{

    char miss_instr[255];
    deque<ooo_model_instr> last_occur_window;

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

    // ------------------------------------------------------------------------ //
    /////////////////////// trace reading complete ///////////////////////////////
    // ------------------------------------------------------------------------ //

    profile = fopen(argv[3], "r");
    uint64_t miss_pc;

    while (fgets(miss_instr, sizeof(miss_instr), profile)) {
        /* note that fgets don't strip the terminating \n, checking its
           presence would allow to handle lines longer that sizeof(line) */
        
        printf("%s", miss_instr); 
        miss_pc = atoi(miss_instr);
        cout << miss_pc;
        last_occur_window = search_last_occurence(miss_pc);

    }
    
    fclose(profile);

    cout << "creating the graph" << endl;

    Graph g = graph_create();

    cout << "building the graph" << endl;

    build_graph(last_occur_window, &g, miss_pc);

    cout << "finish\n";
    
    return 0;
}
