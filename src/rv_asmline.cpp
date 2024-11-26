#include "rv_asmline.h"
#include <cstdlib>
void RVAsmline::add_stmt(std::string op_name, std::vector<uint32_t> oprd) {
    this->stmt_que.push_back(RVStmt(op_name, oprd));
}
void RVAsmline::assemble() {
    for (auto &stmt : this->stmt_que) {
        auto binary_code = OP_format(stmt.inst, stmt.oprd);
        *this->current++ = binary_code;
    }
}
void* RVAsmline::get_code() {
    return this->buffer;
}
RVAsmlinePtr create_rv_asmline(void* buffer) {
    RVAsmlinePtr ptr = (RVAsmlinePtr)malloc(sizeof(RVAsmline));
    ptr->buffer = ptr->current = (uint32_t*)buffer;
    ptr->stmt_que = std::vector<RVStmt>();
    return ptr;
}
void free_rv_asmline(RVAsmlinePtr ptr) {
    free(ptr);
}