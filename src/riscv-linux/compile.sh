# gcc  -O0 -w main.c phantom.c utils.c perf_event_wrapper.c -lassemblyline -lmastik -o main 
g++ -O0 test_rv_asm.cpp rv_asmline.cpp utils.c rv_asm.cpp -o main