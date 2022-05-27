#include "process.h"
#include "FileSystem.h"
#include "memalloc.h"
#include "memory.h"
int main() {
    FileSystem fs;
    Memalloc* mptr = new Memalloc();
    initMemory();
    run(fs);
    fs.updateTreeFile2();
    return 0;
}