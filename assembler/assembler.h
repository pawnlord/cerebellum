#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "coff_file.h"

typedef unsigned char uchar;
typedef unsigned char uint;

typedef enum {
    EAX = 0, ECX, EDX, EBX, ESP, EBP, ESI, EDI
} register_e;

class AsmObject {
    public:
    AsmObject() : rt(false) {}

    RelocationTable rt;
    void add_op_imm(uchar op, uint data, int size);
    void add_op_imm(uchar op, std::string name, int type, int size);
    void add_op_plusr(uchar op, register_e reg);
    void add_byte(uchar op); // for anything we dont include
    std::vector<uchar> code;
};