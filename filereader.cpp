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
        for(int c = 0; lines[i].c_str()[0] == 0; i++){
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
                    operations.push_back(RIGHT);
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
    // get handle
    assembly.add_op_imm(0x6A, arg(0xF5, 1));
    assembly.add_op_imm(0xE8, "GetStdHandle", 0x14, argsz(4));
    assembly.add_op_imm(0xA3, "handle", 0x6, argsz(4));

    // What We Need: pointer to string to store output char
    // Use: EDX
    assembly.add_op_rm (0x89, {3, EBP, ESP}, argsz(0)); // MOV r r
    
    for(int i = 0; i < ops.size(); i++){
        bfOp op = ops[i];
        std::cout << "op " << i << ": " << op << std::endl; 
        switch (op)
        {
            case INCREMENT:
                assembly.add_op_rm (0x83, {1, EBP, EAX}, {0x1, 1, pointer, 1});
                break;
            case OUTPUT:
                // Print Instructions
                assembly.add_op_imm(0x6A, arg(0x0, 1));
                assembly.add_op_imm(0x68, "dummy", 0x6, argsz(4));
                assembly.add_op_imm(0x6A, arg(0x1, 1));
                assembly.add_op_rm (0x8D, {1, EBP, ECX}, arg(pointer, 1));
                assembly.add_op_plusr(0x50, ECX);
                assembly.add_op_rm (0xFF, {0, 5, ESI}, "handle", 0x6, argsz(4));
                assembly.add_op_imm(0xE8, "WriteConsoleA", 0x14, argsz(4));
                break;
        }
    }
    assembly.add_op_imm(0x6A, arg(0x0, 1));
    assembly.add_op_imm(0xE8, "ExitProcess", 0x14, argsz(4));
    return assembly;
}