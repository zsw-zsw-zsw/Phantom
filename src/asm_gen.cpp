#include <cstdlib>
#include <string>
#include <iostream>
#include <cstdio>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <cassert>
#define NOP "addi x0, x0, 0"
struct Func {
    uint64_t addr;
    std::string ident;
    std::vector<std::string> payload;
    std::string flag;
    Func(){}
    Func(uint64_t addr, std::string ident): 
        addr(addr), ident(ident) {}
    void set_flag(std::string fl) {
        flag = flag + fl + ":\n";
    }
    void append_inst(std::string inst) {
        if (flag != "")
            inst = flag + inst,
            flag = "";
        payload.push_back(inst);
    }
};
void to_asm_file(std::vector<Func> funclist, std::string filepath="asm.S") {
    std::string header = "";
    header.append(".global ");
    for (int i = 0; i < funclist.size(); i++) {
        header.append(funclist[i].ident);
        if (i + 1 != funclist.size())
            header.append(", ");
    }
    header.append("\n");
    header.append(".text\n");
    uint64_t total_size = 0;
    for (int i = 0; i < funclist.size(); i++) {
        total_size = std::max(total_size, funclist[i].addr + funclist[i].payload.size());
    }
    std::cerr << total_size << '\n';
    assert(total_size <= (1 << 26));
    std::vector<std::string> code;
    code.resize(total_size, "");
    for (int i = 0; i < funclist.size(); i++) {
        for (int j = 0; j < funclist[i].payload.size(); j++) {
            auto pos = funclist[i].addr + j;
            if (code[pos] != "") {
                // std::cerr << code[pos] << '\n';
                std::cerr << pos << '\n';
                std::cerr << "Function code overlapped!\n";
                assert(code[pos] == "");
            }
            code[pos] = funclist[i].payload[j] + "\n";
            if (j == 0)
                code[pos] = funclist[i].ident + ":\n" + code[pos];
        }
    }
    for (int i = 0; i < total_size; i++)
        if (code[i] == "")
            code[i] = "addi x0, x0, 0\n";
    freopen(filepath.c_str(), "w", stdout);
    std::cout << header;
    for (int i = 0; i < code.size(); i++)
        std::cout << code[i];
    fclose(stdout);
}
void gen_code() {
    auto func1 = Func(0, "func1");
    uint32_t dist = 31;
    for (int i = 0; i < 8; i++) {
        func1.set_flag("func1_flag" + std::to_string(i));
        func1.append_inst("jal x0, func1_flag" + std::to_string(i + 1));
        for (int j = 0; j < 31; j++)
            func1.append_inst(NOP);
    }
    func1.set_flag("func1_flag" + std::to_string(8));
    func1.append_inst("ret");
    to_asm_file({func1});
}
int main() {
    // Func sum = Func(10, "sum");
    // sum.append_inst("add a0, a1, a0");
    // sum.append_inst("ret");
    // to_asm_file(std::vector<Func>{sum});
    gen_code();
    return 0;
}