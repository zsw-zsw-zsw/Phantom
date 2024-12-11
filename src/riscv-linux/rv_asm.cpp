

#include "rv_asm.h"
#include <vector>
#include <map>
#include <assert.h>
#define ECHO_ERROR(s) \
    (fprintf(stderr, "Fatal error: %s!\n", s),\
    exit(0), 0)

#define CHECK_BIT_LEN_U(x, r) \
    ((x) < (1 << (r)))

#define CHECK_BIT_LEN_S(x, r) \
    ((x) < (1 << (r - 1))) || ((~x) < (1 << (r - 1)))


#define BIT_TEST(x, p) \
    (((x) >> (p)) & 1)

#define BIT_MASK(x, p) \
    ((x) & (((1 << (p)) - 1)))

OPInfo op_list[] = {
    OPInfo("add", R_Type, 0b0110011, 0x0, 0x00),
    OPInfo("sub", R_Type, 0b0110011, 0x0, 0x20),
    OPInfo("xor", R_Type, 0b0110011, 0x4, 0x00),
    OPInfo("or", R_Type, 0b0110011, 0x6, 0x00),
    OPInfo("and", R_Type, 0b0110011, 0x7, 0x00),
    OPInfo("sll", R_Type, 0b0110011, 0x1, 0x00),
    OPInfo("srl", R_Type, 0b0110011, 0x5, 0x00),
    OPInfo("sra", R_Type, 0b0110011, 0x5, 0x20),
    OPInfo("slt", R_Type, 0b0110011, 0x2, 0x00),
    OPInfo("sltu", R_Type, 0b0110011, 0x3, 0x00),
    OPInfo("addi", I_Type, 0b0010011, 0x0, 0),
    OPInfo("xori", I_Type, 0b0010011, 0x4, 0),
    OPInfo("ori", I_Type, 0b0010011, 0x6, 0),
    OPInfo("andi", I_Type, 0b0010011, 0x7, 0),
    OPInfo("slli", I_Type, 0b0010011, 0x1, 0x00),
    OPInfo("srli", I_Type, 0b0010011, 0x5, 0x00),
    OPInfo("srai", I_Type, 0b0010011, 0x5, 0x20),
    OPInfo("slti", I_Type, 0b0010011, 0x2, 0),
    OPInfo("sltiu", I_Type, 0b0010011, 0x3, 0),
    OPInfo("lb", I_Type, 0b0000011, 0x0, 0),
    OPInfo("lh", I_Type, 0b0000011, 0x1, 0),
    OPInfo("lw", I_Type, 0b0000011, 0x2, 0),
    OPInfo("lbu", I_Type, 0b0000011, 0x4, 0),
    OPInfo("lhu", I_Type, 0b0000011, 0x5, 0),
    OPInfo("sb", S_Type, 0b0100011, 0x0, 0),
    OPInfo("sh", S_Type, 0b0100011, 0x1, 0),
    OPInfo("sw", S_Type, 0b0100011, 0x2, 0),
    OPInfo("beq", B_Type, 0b1100011, 0x0, 0),
    OPInfo("bne", B_Type, 0b1100011, 0x1, 0),
    OPInfo("blt", B_Type, 0b1100011, 0x4, 0),
    OPInfo("bge", B_Type, 0b1100011, 0x5, 0),
    OPInfo("bltu", B_Type, 0b1100011, 0x6, 0),
    OPInfo("bgeu", B_Type, 0b1100011, 0x7, 0),
    OPInfo("jal", J_Type, 0b1101111, 0, 0),
    OPInfo("jalr", I_Type, 0b1100111, 0x0, 0),
    OPInfo("lui", U_Type, 0b0110111, 0, 0),
    OPInfo("auipc", U_Type, 0b0010111, 0, 0),
    OPInfo("ecall", I_Type, 0b1110011, 0x0, 0x0),
    OPInfo("ebreak", I_Type, 0b1110011, 0x0, 0x1)
};

std::map<std::string, OPInfo> op_pool;

void init_op_pool() {
    for (auto op : op_list) {
        op_pool[op.inst] = op;
    }
}

uint32_t OP_format_R(OPInfo op, uint32_t rs1, uint32_t rs2, uint32_t rd) {
    CHECK_BIT_LEN_U(rs1, 5) && CHECK_BIT_LEN_U(rs2, 5) && CHECK_BIT_LEN_U(rd, 5) || ECHO_ERROR("R_Type arg error");
    return op.op_code | rd << 7 | op.func3_code << 12 | rs1 << 15 | rs2 << 20 | op.func7_code << 25;
}
uint32_t OP_format_I(OPInfo op, uint32_t rs1, uint32_t rd, uint32_t imm) {
    //CHECK_BIT_LEN_S(imm, 12) &&
    CHECK_BIT_LEN_U(rs1, 5) &&  CHECK_BIT_LEN_U(rd, 5) || ECHO_ERROR ("I_Type arg error");
    if (op.inst == "slli" || op.inst == "srli" || op.inst == "srai") {
        CHECK_BIT_LEN_U(imm, 5) || ECHO_ERROR("I_type arg error");
    }
    return op.op_code | rd << 7 | op.func3_code << 12 | rs1 << 15 | BIT_MASK(imm, 12) << 20;
}
uint32_t OP_format_S(OPInfo op, uint32_t rs1, uint32_t rs2, uint32_t imm) {
    //CHECK_BIT_LEN_S(imm, 12) && 
    CHECK_BIT_LEN_U(rs1, 5) && CHECK_BIT_LEN_U(rs2, 5) || ECHO_ERROR("S_Type arg error");
    return op.op_code | (BIT_MASK(imm, 5)) << 7 | op.func3_code << 12 | rs1 << 15 | rs2 << 20 | (BIT_MASK((imm >> 5), 7)) << 25;
}
uint32_t OP_format_B(OPInfo op, uint32_t rs1, uint32_t rs2, uint32_t imm) {
    //CHECK_BIT_LEN_S(imm, 13) && 
    CHECK_BIT_LEN_U(rs1, 5) && CHECK_BIT_LEN_U(rs2, 5) && 
        (!(imm & 1)) || ECHO_ERROR("B_Type arg error");
    return op.op_code | (BIT_TEST(imm, 11) << 7) | (BIT_MASK(imm, 5) << 7) | 
        op.func3_code << 12 | rs1 << 15 | rs2 << 20 | (BIT_MASK((imm >> 5), 6)) << 25 | BIT_TEST(imm, 12) << 31;
}
uint32_t OP_format_U(OPInfo op, uint32_t rd, uint32_t imm) {
    // CHECK
    CHECK_BIT_LEN_U(rd, 5) || ECHO_ERROR("U_Type arg error");
    return op.op_code | (BIT_MASK(imm, 20) << 7);
}
uint32_t OP_format_J(OPInfo op, uint32_t rd, uint32_t imm) {
    CHECK_BIT_LEN_U(rd, 5) || (!(imm & 1)) || ECHO_ERROR("J_Type arg error");
    return op.op_code | rd << 7 | BIT_MASK(imm >> 12, 8) << 12 | BIT_TEST(imm, 11) << 20 | BIT_MASK(imm >> 1, 10) << 21 | BIT_TEST(imm, 20) << 31;
}

uint32_t OP_format(OPInfo op, uint32_t rs1, uint32_t rs2, uint32_t rd, uint32_t imm) {
    switch (op.type) {
        case R_Type: 
            return OP_format_R(op, rs1, rs2, rd);
        case I_Type:
            return OP_format_I(op, rs1, rd, imm);
        case S_Type:
            return OP_format_S(op, rs1, rs2, imm);
        case B_Type:
            return OP_format_B(op, rs1, rs2, imm);
        case J_Type:
            return OP_format_J(op, rd, imm);
        case U_Type:
            return OP_format_U(op, rd, imm);
        default:
            ECHO_ERROR("Undefined op");
    }
    assert(false);
}
uint32_t OP_format(std::string op_name, std::vector<uint32_t> arg_list) {
    if (!op_pool.count(op_name)) {
        ECHO_ERROR("undefined op");
    }
    OPInfo op = op_pool[op_name];
    switch (op.type) {
        case R_Type:
            arg_list.size() == 3 || ("R_Type arg error");
            return OP_format_R(op, arg_list[0], arg_list[1], arg_list[2]);
        case I_Type:
            arg_list.size() == 3 || ECHO_ERROR("I_Type arg error");
            return OP_format_I(op, arg_list[0], arg_list[1], arg_list[2]);
        case S_Type:
            arg_list.size() == 3 || ECHO_ERROR("S_Type arg error");
            return OP_format_S(op, arg_list[0], arg_list[1], arg_list[2]);
        case B_Type:
            arg_list.size() == 3 || ECHO_ERROR("B_Type arg error");
            return OP_format_S(op, arg_list[0], arg_list[1], arg_list[2]);
        case J_Type:
            arg_list.size() == 2 || ECHO_ERROR("J_Type arg error");
            return OP_format_J(op, arg_list[0], arg_list[1]);
        case U_Type:
            arg_list.size() == 2 || ECHO_ERROR("U_Type arg error");
            return OP_format_U(op, arg_list[0], arg_list[1]);
        default:
            ECHO_ERROR("Undefined op");
    }
    assert(false);
}

