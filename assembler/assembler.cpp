#include "assembler.h"



void AsmObject::add_op_imm(uchar op, int data, int size){
    code.push_back(op);
    push_back_number(code, data, size);
}
void AsmObject::add_op_imm(uchar op, std::string name, int type, int size){
    code.push_back(op);
    rt.add_future_relocation(code.size(), name, type);
    push_back_number(code, 0, size);
}
void AsmObject::add_op_rm(uchar op, uint rmbyte, int extra, uint size){
    code.push_back(op);
    code.push_back(rmbyte);
    push_back_number(code, extra, size);
}
void AsmObject::add_op_rm(uchar op, uint rmbyte, std::string name, int type, int size){
    code.push_back(op);
    code.push_back(rmbyte);
    rt.add_future_relocation(code.size(), name, type);
    push_back_number(code, 0, size);
}
void AsmObject::add_byte(uchar op){
    code.push_back(op);
}
void AsmObject::add_op_plusr(uchar op, register_e reg){
    code.push_back(op+reg);
}
