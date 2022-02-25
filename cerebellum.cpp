#include "filereader.h"
#include <fstream>

struct {
    bool version_flag;
    bool verbose_flag;
    std::string input_name;
    std::string output_name;
} cl_arguments;

int main(int argc, char **argv){
    cl_arguments.input_name = "";
    cl_arguments.output_name = "a.obj";
    cl_arguments.version_flag = false;
    cl_arguments.verbose_flag = false;
    bool has_flag = false;
    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "-V") == 0 || strcmp(argv[i], "--version") == 0){
            has_flag = false;
            cl_arguments.version_flag = true;
            break;
        } else if(strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0){
            has_flag = false;
            cl_arguments.verbose_flag = true;
            break;
        } else if(strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0){
            has_flag = true;
        } else {
                std::cout << argv[i] << has_flag << "\n";
            if (has_flag){
                cl_arguments.output_name = argv[i];
            } else {
                cl_arguments.input_name = argv[i];
                std::cout << cl_arguments.input_name << "\n";
            }
            has_flag = false;
        }
    }

    std::cout << "cerebellum: brainfuck compiler for windows" << std::endl;
    std::cout << "version 1.1.0" << std::endl;

    if(cl_arguments.version_flag){
        return 0;
    }

    if(cl_arguments.input_name == ""){
        std::cerr << "[cerebellum: COMPILE ERROR] No input file given." << std::endl;
        return 1;
    }

    std::vector<bfOp> ops = read_bf_file(cl_arguments.input_name);
        std::cout << "op " << ops.size() << std::endl;
    AsmObject assembly = translate_bf(ops);

    CoffFile cf(cl_arguments.input_name);  
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
    std::ofstream out (cl_arguments.output_name, std::ios::binary);
    out.write(data.c_str(), data.size()+1);
}