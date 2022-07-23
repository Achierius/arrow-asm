#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetMachine.h"

#include <memory>

using namespace llvm;

class Codegen {
private:
    std::unique_ptr<LLVMContext> context;
    std::unique_ptr<Module> module;
    std::unique_ptr<IRBuilder<>> builder;

    Codegen();
    void writeObject(std::string& outputPath);
};

Codegen::Codegen() {
    context = std::make_unique<LLVMContext>();
    module = std::make_unique<Module>("beautiful-asm-test", *context);
    builder = std::make_unique<IRBuilder<>>(*context);
}

void Codegen::writeObject(std::string& outputPath) {
    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();

    auto targetTriple = sys::getDefaultTargetTriple();
    std::string err;
    auto target = TargetRegistry::lookupTarget(targetTriple, err);

    if (!target) {
        // TODO: log `err`
        return;
    }

    // use generic cpu for now
    auto cpu = "generic";
    auto features = "";
    TargetOptions opt;
    auto rm = Optional<Reloc::Model>();
    auto targetMachine = target->createTargetMachine(targetTriple, cpu, features, opt, rm);

    module->setDataLayout(targetMachine->createDataLayout());
    module->setTargetTriple(targetTriple);
    
    std::error_code ec;
    raw_fd_ostream dest(outputPath, ec, sys::fs::OF_None);
    if (ec) {
        // TODO: error: could not open file
        return;
    }

    legacy::PassManager pass;
    auto fileType = CGFT_ObjectFile;

    if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, fileType)) {
        // TODO: error: failed to add emit pass
        return;
    }

    pass.run(*module);
    dest.flush();
}
