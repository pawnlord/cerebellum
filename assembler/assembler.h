#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "coff_file.h"
#define arg(data, sz) {data, sz, 0, 0}
#define argsz(sz) {0,sz,0,0}

typedef unsigned char uchar;
typedef unsigned char uint;

typedef enum {
    EAX = 0, ECX, EDX, EBX, ESP, EBP, ESI, EDI
} register_e;

typedef struct {
    uchar mod;
    uchar rm;
    register_e reg;
} modrm_t;

typedef struct {
    uchar ss;
    uchar index;
    register_e base;
} sib_t;

typedef struct {
    int data;
    int datasz;
    int disp;
    int dispsz;
} arg_t;
class AsmObject {
    public:
    AsmObject() : rt(false) {}

    RelocationTable rt;
    void add_op_imm(uchar op, arg_t args);
    void add_op_imm(uchar op, std::string name, int type, arg_t argsz);
    void add_op_rm(uchar op, modrm_t rm, arg_t arg);
    void add_op_rm(uchar op, modrm_t rm, sib_t sib, arg_t arg);
    void add_op_rm(uchar op, modrm_t rm, std::string name, int type, arg_t argsz);
    void add_op_plusr(uchar op, register_e reg);
    void add_byte(uchar op); // for anything we dont include
    std::vector<uchar> code;
};