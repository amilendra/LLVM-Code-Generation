#include "llvm/ADT/ArrayRef.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"    // For ConstantInt.
#include "llvm/IR/DerivedTypes.h" // For PointerType, FunctionType.
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Debug.h" // For errs().

#include <memory> // For unique_ptr

using namespace llvm;

// The goal of this function is to build a Module that
// represents the lowering of the following foo, a C function:
// extern int baz();
// extern void bar(int);
// void foo(int a, int b) {
//   int var = a + b;
//   if (var == 0xFF) {
//     bar(var);
//     var = baz();
//   }
//   bar(var);
// }
//
// The IR for this snippet (at O0) is:
// define void @foo(i32 %arg, i32 %arg1) {
// bb:
//   %i = alloca i32
//   %i2 = alloca i32
//   %i3 = alloca i32
//   store i32 %arg, ptr %i
//   store i32 %arg1, ptr %i2
//   %i4 = load i32, ptr %i
//   %i5 = load i32, ptr %i2
//   %i6 = add i32 %i4, %i5
//   store i32 %i6, ptr %i3
//   %i7 = load i32, ptr %i3
//   %i8 = icmp eq i32 %i7, 255
//   br i1 %i8, label %bb9, label %bb12
//
// bb9:
//   %i10 = load i32, ptr %i3
//   call void @bar(i32 %i10)
//   %i11 = call i32 @baz()
//   store i32 %i11, ptr %i3
//   br label %bb12
//
// bb12:
//   %i13 = load i32, ptr %i3
//   call void @bar(i32 %i13)
//   ret void
// }
//
// declare void @bar(i32)
// declare i32 @baz(...)
std::unique_ptr<Module> myBuildModule(LLVMContext &Ctxt) {
  errs() << "Test"
         << "\n";
  std::unique_ptr<Module> m = std::make_unique<Module>("input.c", Ctxt);
  m->getOrInsertFunction("baz", Type::getInt32Ty(Ctxt));
  m->getOrInsertFunction("bar", Type::getVoidTy(Ctxt), Type::getInt32Ty(Ctxt));
  // AttributeList AL_foo = {Type::getInt32Ty(Ctxt), Type::getInt32Ty(Ctxt)};

  // Define the function type: void (int32, int32)
  FunctionType *fooType = FunctionType::get(
      Type::getVoidTy(Ctxt),                            // return type
      {Type::getInt32Ty(Ctxt), Type::getInt32Ty(Ctxt)}, // argument types
      false                                             // not variadic
  );
  // m->getOrInsertFunction("foo", fooType);
  // Create the function
  Function *fooFunc = Function::Create(fooType, Function::ExternalLinkage,
                                       "foo", // function name
                                       *m // the module it will be inserted into
  );
  // for(Function &MyFunction: MyModule) {
  // // Do something with MyFunction.
  //      errs() << "Test" << "\n";
  // }
  BasicBlock *BB = BasicBlock::Create(Ctxt, "bb", fooFunc);

  IRBuilder<> Builder(Ctxt);

  // Allocate space on the stack for an int32 (local variable)
  AllocaInst *Alloca =
      Builder.CreateAlloca(Type::getInt32Ty(Ctxt), nullptr, "myVar");

  // Optionally store a value into it
  Value *Const42 = ConstantInt::get(Type::getInt32Ty(Ctxt), 42);
  Builder.CreateStore(Const42, Alloca);

  // Insert a return void instruction
  ReturnInst::Create(Ctxt, BB);

  return m;
}
