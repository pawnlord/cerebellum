#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "coff_file.h"


class AsmObject {
    public:
    AsmObject() : rt(false) {}

    RelocationTable rt;
    void add_op_imm32();
    std::vector<char> code;
};