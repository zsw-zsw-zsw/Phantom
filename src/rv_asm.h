#include <string>
#include <cstdint>
#include <vector>
enum OPType {
    R_Type,
    I_Type,
    S_Type,
    B_Type,
    J_Type,
    U_Type
};
struct OPInfo {
    std::string inst;
    OPType type;
    uint8_t op_code;
    uint8_t func3_code;
    uint8_t func7_code;
    OPInfo() {}
    OPInfo(std::string inst, OPType type, uint8_t op_code, uint8_t func3_code, uint8_t func7_code) :
        inst(inst), type(type), op_code(op_code), func3_code(func3_code), func7_code(func7_code) {}
};
typedef OPInfo* OPInfoPtr;
void init_op_pool();
uint32_t OP_format(OPInfo op, uint32_t rs1, uint32_t rs2, uint32_t rd, uint32_t imm);
uint32_t OP_format(std::string op_name, std::vector<uint32_t> arg_list);