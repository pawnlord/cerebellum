#include "filereader.h"
#include <fstream>

int main(){
    std::cout << "cerebellum: brainfuck compiler for windows" << std::endl;
    std::cout << "version 0.2.0" << std::endl;

    std::vector<bfOp> ops = read_bf_file("test.bf");
        std::cout << "op " << ops.size() << std::endl;
    AsmObject assembly = translate_bf(ops);

    CoffFile cf("test.asm");  
    cf.add_symbol(".absolut", 0, -1, 0, 0x3, 0, "");
    cf.add_symbol("STD_OUTPUT_HANDLE\0", 0xFFFFFFF5, -1, 0, 0x3, 0, "");
    cf.add_symbol("NULL", 0, -1, 0, 0x3, 0, "");
    cf.add_symbol("ExitProcess", 0, 0, 0, 0x2, 0, "");
    cf.add_symbol("GetStdHandle", 0, 0, 0, 0x2, 0, "");
    cf.add_symbol("WriteConsoleA", 0, 0, 0, 0x2, 0, "");
    cf.add_symbol("ReadConsoleA", 0, 0, 0, 0x2, 0, ""); 
    cf.add_symbol("FlushConsoleInputBuffer", 0, 0, 0, 0x2, 0, ""); 
    cf.add_symbol("msg", 0, 1, 0, 0x3, 0, "");
    cf.add_symbol("msg.len", 0xF, -1, 0, 0x3, 0, "");
    cf.add_symbol("dummy", 0, 2, 0, 0x3, 0, "");
    cf.add_symbol("out_handle", 4, 2, 0, 0x3, 0, "");
    cf.add_symbol("in_handle", 8, 2, 0, 0x3, 0, "");
    cf.add_symbol("main", 0, 3, 0, 0x2, 0, "");
    cf.add_symbol("@feat.00", 1, -1, 0, 0x3, 0, ""); // TODO: automate

    // Data sections 
    RelocationTable null_rt(true);
    cf.add_section(".data\0\0\0", 0xC0300040, null_rt, std::vector<unsigned char>({}));
    cf.add_section(".bss\0\0\0\0", 0xC0300080, null_rt, std::vector<unsigned char>({}));

    cf.add_section(".text\0\0\0", 0x60500020, assembly.rt, assembly.code);
    

    cf.compile();

    std::string data = cf.get_compiled();
    std::ofstream out ("test_build.obj", std::ios::binary);
    out.write(data.c_str(), data.size()+1);
}