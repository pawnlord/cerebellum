#include "assembler.h"



void AsmObject::add_op_imm(uchar op, arg_t args){
    code.push_back(op);
    push_back_number(code, args.data, args.datasz);
}
void AsmObject::add_op_imm(uchar op, std::string name, int type, arg_t arg){
    code.push_back(op);
    rt.add_future_relocation(code.size(), name, type);
    push_back_number(code, 0, arg.datasz);
}
void AsmObject::add_op_rm(uchar op, modrm_t rm, arg_t arg){
    code.push_back(op);
    uchar rmbyte = rm.rm;
    rmbyte += rm.reg << 3;
    rmbyte += rm.mod << 6;
    code.push_back(rmbyte);
    push_back_number(code, arg.disp, arg.dispsz);
    push_back_number(code, arg.data, arg.datasz);
}
void AsmObject::add_op_rm(uchar op, modrm_t rm, sib_t sib, arg_t arg){
    code.push_back(op);
    uchar rmbyte = rm.rm;
    rmbyte += rm.reg << 3;
    rmbyte += rm.mod << 6;
    uchar sibbyte = sib.base;
    sibbyte += sib.index << 3;
    sibbyte += sib.ss << 6;
    code.push_back(rmbyte);
    code.push_back(sibbyte);
    push_back_number(code, arg.disp, arg.dispsz);
    push_back_number(code, arg.data, arg.datasz);
}
void AsmObject::add_op_rm(uchar op, modrm_t rm, std::string name, int type, arg_t arg){
    code.push_back(op);
    uchar rmbyte = rm.rm;
    rmbyte += rm.reg << 3;
    rmbyte += rm.mod << 6;
    code.push_back(rmbyte);
    rt.add_future_relocation(code.size(), name, type);
    push_back_number(code, 0, arg.datasz);
}
void AsmObject::add_byte(uchar op){
    code.push_back(op);
}
void AsmObject::add_op_plusr(uchar op, register_e reg){
    code.push_back(op+reg);
}
