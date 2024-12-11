#include "rv_asmline.h"
#include "rv_asm.h"
#include "utils.h"
int main() {
    puts("test");
    void* buf = create_buffer(random_addr(), PAGE_SIZE);
    puts("test");
    auto asmline = create_rv_asmline(buf);
    init_op_pool();

    asmline->add_stmt("add", {1, 2, 33});
    puts("test");
    asmline->assemble();
    puts("test");
    uint32_t* func = (uint32_t*)asmline->get_code();
    printf("%08u\n", *func);
    return 0;
}