#include "filereader.h"

std::vector<bfOp> read_bf_file(std::string path){
    std::ifstream file;
    std::string line;
    std::vector<std::string> lines;
    file.open(path);
    std::string parsed_line = "";
    if(file.is_open()){
        while(std::getline(file, line)){
            parsed_line = "";
            for(int c = 0; line.c_str()[c] != 0; c++){
                if(line[c] == '#'){
                    break;
                }
                parsed_line += line[c];
            }
            lines.push_back(parsed_line);
        }
    }
    else{
        std::cerr << "[cerebellum: COMPILE ERROR] file " << path << " not found." << std::endl;
        exit(1);
    }
    std::vector<bfOp> operations;
    for(int i = 0; i < lines.size(); i++){
        for(int c = 0; lines[i].c_str()[c] != 0; c++){
            switch(lines[i][c]){
                case '+':
                    operations.push_back(INCREMENT);
                break;
                case '-':
                    operations.push_back(DECREMENT);
                break;
                case '>':
                    operations.push_back(RIGHT);
                break;
                case '<':
                    operations.push_back(LEFT);
                break;
                case '.':
                    operations.push_back(OUTPUT);
                break;
                case ',':
                    operations.push_back(INPUT);
                break;
                case '[':
                    operations.push_back(BEGIN_LOOP);
                break;
                case ']':
                    operations.push_back(END_LOOP);
                break;
            }
        }
    }
    return operations;
}

AsmObject translate_bf(std::vector<bfOp> ops){
    AsmObject assembly;
    int pointer = 0;
    int allocated_memory = 0;
    std::vector<int> loop_positions;
    // get handle
    assembly.add_op_imm(0x6A, arg(0xF6, 1));
    assembly.add_op_imm(0xE8, "GetStdHandle", 0x14, argsz(4));
    assembly.add_op_imm(0xA3, "in_handle", 0x6, argsz(4));

    assembly.add_op_imm(0x6A, arg(0xF5, 1));
    assembly.add_op_imm(0xE8, "GetStdHandle", 0x14, argsz(4));
    assembly.add_op_imm(0xA3, "out_handle", 0x6, argsz(4));

    assembly.add_op_plusr(0x50, EBP);
    // Allocate 255 bytes
    assembly.add_op_rm (0x81, {3, ESP, EBP}, arg(30000, 4)); // MOV r r
    
    assembly.add_op_rm (0x89, {3, EBP, ESP}, argsz(0)); // MOV r r
    

    // initialize value 
    assembly.add_op_rm (0xC7, {1, EBP, EAX}, {0x0, 4, 0x0, 1}); // MOV [r] imm

    for(int i = 0; i < ops.size(); i++){
        bfOp op = ops[i];
        switch (op)
        {
            case INCREMENT:
                assembly.add_op_rm (0x80, {1, EBP, EAX}, {0x1, 1, 0, 1});
                break;
            case DECREMENT:
                assembly.add_op_rm (0x80, {1, EBP, EAX}, {-0x1, 1, 0, 1});
                break;
            case OUTPUT:
                // Print Instructions
                assembly.add_op_imm(0x6A, arg(0x0, 1));
                assembly.add_op_imm(0x68, "dummy", 0x6, argsz(4));
                assembly.add_op_imm(0x6A, arg(0x1, 1));
                assembly.add_op_rm (0x8D, {1, EBP, ECX}, arg(0, 1));
                assembly.add_op_plusr(0x50, ECX);
                assembly.add_op_rm (0xFF, {0, EBP, ESI}, "out_handle", 0x6, argsz(4));
                assembly.add_op_imm(0xE8, "WriteConsoleA", 0x14, argsz(4));
                break;
            case INPUT:
                // Input Instructions
                assembly.add_op_imm(0x6A, arg(0x0, 1));
                assembly.add_op_imm(0x68, "dummy", 0x6, argsz(4));
                assembly.add_op_imm(0x6A, arg(0x1, 1));
                assembly.add_op_rm (0x8D, {1, EBP, ECX}, arg(0, 1));
                assembly.add_op_plusr(0x50, ECX);
                assembly.add_op_rm (0xFF, {0, EBP, ESI}, "in_handle", 0x6, argsz(4));
                assembly.add_op_imm(0xE8, "ReadConsoleA", 0x14, argsz(4));

                // clean up input buffer
                assembly.add_op_rm (0xFF, {0, EBP, ESI}, "in_handle", 0x6, argsz(4));
                assembly.add_op_imm(0xE8, "FlushConsoleInputBuffer", 0x14, argsz(4));
                break;
            case RIGHT:
                assembly.add_op_plusr(0x40, EBP);
                pointer += 1;
                if(pointer > allocated_memory) {
                    assembly.add_op_rm (0xC7, {1, EBP, EAX}, {0x0, 4, 0, 1}); // MOV [r] imm
                }
                allocated_memory += 1;
                break;
            case LEFT:
                assembly.add_op_plusr(0x48, EBP);
                
                if(pointer != 0 || pointer == 0){
                    pointer -= 1;
                } else {
                    std::cerr << "[cerebellum: COMPILE WARNING] Shifted to far left." << std::endl;
                }
                break;
            case BEGIN_LOOP:
                assembly.add_op_imm(0xE9, arg(0, 4));
                loop_positions.push_back(assembly.code.size()); // get current position to refer back to later
                                                                // change the number later
                break;
            case END_LOOP:

                int loop_pos = loop_positions.back();
                loop_positions.pop_back();
                // send the begin loop here first
                int current_pos = assembly.code.size();
                int offset_from = current_pos-loop_pos;
                assembly.change_number(loop_pos-4, offset_from, 4);

                assembly.add_op_rm(0x80, {1, EBP, EDI}, {0, 1, 0, 1}); // CMP

                current_pos = assembly.code.size();
                int offset_to = loop_pos-(current_pos+6); // account for added bytes
                assembly.add_byte(0x0f);
                assembly.add_op_imm(0x85, arg(offset_to, 4)); // JMP
                break;
        }
    }
    assembly.add_op_rm (0xFF, {3, EAX, EAX}, argsz(0));
    assembly.add_op_imm(0xE8, "ExitProcess", 0x14, argsz(4));
    return assembly;
}