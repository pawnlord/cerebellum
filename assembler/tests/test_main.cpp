#include "assembler.h"
#include <fstream>

int main(){
    AsmObject assembly;
    assembly.add_op_imm(0x6A, arg(0xF5, 1));
    assembly.add_op_imm(0xE8, "GetStdHandle", 0x14, argsz(4));
    assembly.add_op_imm(0xA3, "handle", 0x6, argsz(4));
    assembly.add_op_plusr(0x50, EBP);
    assembly.add_op_rm (0x89, {3, EBP, ESP}, argsz(0)); // MOV r r
    assembly.add_op_rm (0xC7, {1, EBP, EAX}, {0xA, 4, 0, 1}); // MOV [r] imm
    assembly.add_op_rm (0x8B, {1, EBP, ECX}, arg(0, 1)); // mov r [r]
    assembly.add_op_rm (0x89, {1, EBP, ECX}, arg(0x1, 1));
    assembly.add_op_rm (0x83, {1, EBP, EAX}, {0x30, 1, 0x1, 1});
    assembly.add_op_rm (0xC7, {1, EBP, EAX}, {13, 4, 0x2, 1});
    assembly.add_op_rm (0xC7, {1, EBP, EAX}, {10, 4, 0x3, 1});
    assembly.add_op_rm (0xC7, {1, EBP, EAX}, {0x0, 4, 0x4, 1});
     
    assembly.add_op_imm(0x6A, arg(0x0, 1));
    assembly.add_op_imm(0x68, "dummy", 0x6, argsz(4));
    assembly.add_op_imm(0x6A, arg(0x1, 1));
    assembly.add_op_rm (0x8D, {1, EBP, ECX}, arg(0x1, 1));
    assembly.add_op_plusr(0x50, ECX);
    assembly.add_op_rm (0xFF, {0, 5, ESI}, "handle", 0x6, argsz(4));
    assembly.add_op_imm(0xE8, "WriteConsoleA", 0x14, argsz(4));
    
    
    assembly.add_op_imm(0x6A, arg(0x0, 1));
    assembly.add_op_imm(0xE8, "ExitProcess", 0x14, argsz(4));
    std::cout << "declared a" << std::endl;

    // used symbols
    CoffFile cf("test.asm");  
    cf.add_symbol(".absolut", 0, -1, 0, 0x3, 0, "");
    cf.add_symbol("STD_OUTPUT_HANDLE\0", 0xFFFFFFF5, -1, 0, 0x3, 0, "");
    cf.add_symbol("NULL", 0, -1, 0, 0x3, 0, "");
    cf.add_symbol("ExitProcess", 0, 0, 0, 0x2, 0, "");
    cf.add_symbol("GetStdHandle", 0, 0, 0, 0x2, 0, "");
    cf.add_symbol("WriteConsoleA", 0, 0, 0, 0x2, 0, "");
    cf.add_symbol("msg", 0, 1, 0, 0x3, 0, "");
    cf.add_symbol("msg.len", 0xF, -1, 0, 0x3, 0, "");
    cf.add_symbol("dummy", 0, 2, 0, 0x3, 0, "");
    cf.add_symbol("handle", 4, 2, 0, 0x3, 0, "");
    cf.add_symbol("main", 0, 3, 0, 0x2, 0, "");
    cf.add_symbol("@feat.00", 1, -1, 0, 0x3, 0, "");

    // Data sections 
    RelocationTable null_rt(true);
    cf.add_section(".data\0\0\0", 0xC0300040, null_rt, std::vector<unsigned char>({
        0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64, 0x21, 0x0D, 0x0A, 0x00
    }));
    cf.add_section(".bss\0\0\0\0", 0xC0300080, null_rt, std::vector<unsigned char>({}));

    cf.add_section(".text\0\0\0", 0x60500020, assembly.rt, assembly.code);
    

    cf.compile();
    std::cout << "compiled" << std::endl;

    std::string data = cf.get_compiled();
    std::ofstream out ("test_build.obj", std::ios::binary);
    out.write(data.c_str(), data.size()+1);

    std::cout << "tests complete" << std::endl;
}