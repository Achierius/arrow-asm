#include "codegen.hh"

using namespace codegen;
using namespace bytecode;

int main() {
    BytecodeChunk chunk;
    chunk.code = {
        { kImmByte, std::byte{3} },
        { kImmByte, std::byte{5} },
        { kAddLong, std::byte{0} },
        { kImmByte, std::byte{2} },
        { kMulLong, std::byte{0} },
        { kReturn, std::byte{0} }
    };
    BytecodeExecutable exec;
    exec.chunks = {{chunk, 0}};
    
    Generate(exec, "test.o");
}
