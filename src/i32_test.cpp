#include <iostream>
#include <string>
#include <map>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/ExecutionEngine/GenericValue.h>

#include <llvm/Support/TargetSelect.h>

void jit()
{
    llvm::InitializeNativeTarget();

    // LLVM components
    llvm::LLVMContext& llvm_context = llvm::getGlobalContext();
    llvm::Module* llvm_module = new llvm::Module( "llvm-jit", llvm_context );
    llvm::IRBuilder<> ir_builder( llvm_context );

    // JIT engine
    std::string error_log;
    llvm::ExecutionEngine* jit_engine = llvm::EngineBuilder( llvm_module ).setErrorStr( &error_log ).create();
    if ( jit_engine == nullptr ) {
        std::cerr << "failed to create JIT engine. " << error_log << std::endl;
        return;
    }

    // create pre function
    auto const linkage = llvm::Function::ExternalLinkage;

    auto const& i32_ty = llvm::Type::getInt32Ty( llvm_context );

    std::vector<llvm::Type*> parameter_types = { i32_ty, i32_ty };
    llvm::Type* const return_type = i32_ty;

    llvm::FunctionType* const func_type = llvm::FunctionType::get( return_type, parameter_types, false/*not variable*/ );

    // i32 test( i32, i32 )
    llvm::Function* const func = llvm::Function::Create( func_type, linkage, "test", llvm_module );


    std::map<std::string, llvm::Value*> arg_map;
    std::string const arg_names[] = { "a", "b" };
    for( llvm::Function::arg_iterator ait = func->arg_begin(); ait != func->arg_end(); ++ait ) {
        ait->setName( arg_names[ait->getArgNo()] );
        arg_map[arg_names[ait->getArgNo()]] = ait;
    }
    // body
    llvm::BasicBlock* const basic_brock
        = llvm::BasicBlock::Create( llvm_context, "entry", func );
    ir_builder.SetInsertPoint( basic_brock );

    ir_builder.CreateRet(
        ir_builder.CreateAdd(
            arg_map["a"], arg_map["b"]
            )
        );

    func->dump();



    // JIT!!!
    llvm::GenericValue a;
    a.IntVal = llvm::APInt( 32, 1 );

    llvm::GenericValue b;
    b.IntVal = llvm::APInt( 32, 2 );

    std::vector<llvm::GenericValue> args = { a, b };
    auto res = jit_engine->runFunction( func, args );

    std::cout << *(a.IntVal.getRawData()) << " + "<< *(b.IntVal.getRawData())
              << " = " << *(res.IntVal.getRawData()) << std::endl;
}




int main()
{
    jit();
}
