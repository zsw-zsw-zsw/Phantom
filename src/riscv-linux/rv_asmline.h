#ifndef RV_ASMLINE_H
#define RV_ASMLINE_H



#include <cstdint>
#include <string>
#include <vector>
#include "rv_asm.h"


struct RVStmt {
    std::string inst;
    std::vector<uint32_t> oprd;
    RVStmt() {}
    RVStmt(std::string inst, std::vector<uint32_t> oprd): inst(inst), oprd(oprd) {};
};
struct RVAsmline {
    uint32_t* buffer;
    uint32_t* current;
    std::vector<RVStmt> stmt_que;
    void add_stmt(std::string op_name, std::vector<uint32_t> oprd);
    void assemble();
    void* get_code();
};
typedef RVAsmline* RVAsmlinePtr;;
RVAsmlinePtr create_rv_asmline(void* buffer);
void free_rv_asmline(RVAsmlinePtr ptr);



#endif