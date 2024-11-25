#include <cstring>
#include <cstdint>
enum OP_Type {
    R_Type,
    I_Type,
    S_Type,
    B_Type,
    J_Type,
    U_Type
};
struct OP_Node {
    string inst;
    OP_Type type;
    uint8_t op_code;
    uint8_t func3_code;
    uint8_t func7_code;
    OP_Type() {}
    OP_Type(string inst, OP_Type type, uint8_t op_code, uint8_t func3_code, uint8_t func7_code) :
        inst(inst), type(type), op_code(op_code), func3_code(func3_code), func7_code(func7_code) {}
};
OP_Node op_list = {
    OP_Node("add", R_Type, 0b0110011, 0x0, 0x00),
    OP_Node("sub", R_Type, 0b0110011, 0x0, 0x20),
    OP_Node("xor", R_Type, 0b0110011, 0x4, 0x00),
    OP_Node("or", R_Type, 0b0110011, 0x6, 0x00),
    OP_Node("and", R_Type, 0b0110011, 0x7, 0x00),
    OP_Node("sll", R_Type, 0b0110011, 0x1, 0x00),
    OP_Node("srl", R_Type, 0b0110011, 0x5, 0x00),
    OP_Node("sra", R_Type, 0b0110011, 0x5, 0x20),
    OP_Node("slt", R_Type, 0b0110011, 0x2, 0x00),
    OP_Node("sltu", R_Type, 0b0110011, 0x3, 0x00),
    OP_Node("addi", I_Type, 0b0010011, 0x0, 0),
    OP_Node("xori", I_Type, 0b0010011, 0x4, 0),
    OP_Node("ori", I_Type, 0b0010011, 0x6, 0),
    OP_Node("andi", I_Type, 0b0010011, 0x7, 0),
    OP_Node("slli", I_Type, 0b0010011, 0x1, 0x00),
    OP_Node("srli", I_Type, 0b0010011, 0x5, 0x00),
    OP_Node("srai", I_Type, 0b0010011, 0x5, 0x20),
    OP_Node("slti", I_Type, 0b0010011, 0x2, 0),
    OP_Node("sltiu", I_Type, 0b0010011, 0x3, 0),
    OP_Node("lb", I_Type, 0b0000011, 0x0, 0),
    OP_Node("lh", I_Type, 0b0000011, 0x1, 0),
    OP_Node("lw", I_Type, 0b0000011, 0x2, 0),
    OP_Node("lbu", I_Type, 0b0000011, 0x4, 0),
    OP_Node("lhu", I_Type, 0b0000011, 0x5, 0),
    OP_Node("sb", S_Type, 0b0100011, 0x0, 0),
    OP_Node("sh", S_Type, 0b0100011, 0x1, 0),
    OP_Node("sw", S_Type, 0b0100011, 0x2, 0),
    OP_Node("beq", B_Type, 0b1100011, 0x0, 0),
    OP_Node("bne", B_Type, 0b1100011, 0x1, 0),
    OP_Node("blt", B_Type, 0b1100011, 0x4, 0),
    OP_Node("bge", B_Type, 0b1100011, 0x5, 0),
    OP_Node("bltu", B_Type, 0b1100011, 0x6, 0),
    OP_Node("bgeu", B_Type, 0b1100011, 0x7, 0),
    OP_Node("jal", J_Type, 0b1101111, 0, 0),
    OP_Node("jalr", I_Type, 0b1100111, 0x0, 0),
    OP_Node("lui", U_Type, 0b0110111, 0, 0),
    OP_Node("auipc", U_Type, 0b0010111, 0, 0),
    OP_Node("ecall", I_Type, 0b1110011, 0x0, 0x0),
    OP_Node("ebreak", I_Type, 0b1110011, 0x0, 0x1)
}