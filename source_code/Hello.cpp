//===- Hello.cpp - Example code from "Writing an LLVM Pass" ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements two versions of the LLVM "Hello World" pass described
// in docs/WritingAnLLVMPass.html
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "hello"

#include <llvm/Pass.h>
#include <llvm/PassManager.h>
#include <llvm/ADT/SmallVector.h>

#include <llvm/ADT/SmallSet.h>

#include <llvm/Analysis/Verifier.h>
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/InlineAsm.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Support/MathExtras.h>
#include <algorithm>

#include "llvm/Transforms/Utils/ValueMapper.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include <map>
#include <string>

using namespace llvm;



namespace {
    struct Hello :  public ModulePass
    {

        /** Constructor. */
        static char ID;
        Hello() : ModulePass(ID) {}

        //DEFINE_INTPASS_ANALYSIS_ADJUSTMENT(PointerAnalysisPass);

        /**
         * @brief Runs this pass on the given function.
         * @param [in,out] func The function to analyze
         * @return true if the function was modified; false otherwise
        */
        virtual bool runOnModule(llvm::Module &M){
            std::set<StringRef> cloneFunSet;
            cloneFunSet.insert("abc");
            cloneFunSet.insert("xyz");
            if(cloneFunSet.find("xyz")!=cloneFunSet.end()){
                errs() << "find \n";
            } else{
                errs() << "NOT find \n";
            }
            errs() << "<Start: -------------------->" << "\n";
            Function* func_pop_direct_branch;
            // Create a new global variable
            AllocaInst* newInst = new AllocaInst(IntegerType::get(M.getContext(),32));
            GlobalVariable* gvar_int32_glob = new GlobalVariable(/*Module=*/M,
                    /*Type=*/IntegerType::get(M.getContext(), 32),
                    /*isConstant=*/false,
                    /*Linkage=*/GlobalValue::ExternalLinkage,
                    /*Initializer=*/0, // has initializer, specified below
                    /*Name=*/"globbbbbbbbbb");
            gvar_int32_glob->setAlignment(4);
            ConstantInt* const_int32_4 = ConstantInt::get(M.getContext(), APInt(32, StringRef("3"), 10));
            gvar_int32_glob->setInitializer(const_int32_4);

            if(!M.getFunction("pow2")){
                errs() << "\n\n\nget null func\n";
            } else{
                errs() << "get func\n";
            }

            for (Module::iterator func = M.begin(), e = M.end(); func != e; ++func) {
                // Print out the name of the basic block if it has one, and then the
                // number of instructions that it contains
                if (func->getName()=="pop_direct_branch"){
                    func_pop_direct_branch = func;
                    func++;
                }
                errs() << "Function (name=" << func->getName() << ") has "
                       << func->size() << " basic blocks.\n";
                for (Function::iterator blk = func->begin(), e = func->end(); blk != e; ++blk) {
                    errs() << "Basic block (name=" << blk->getName() << ") has "
                           << blk->size() << " instructions.\n";
                    for (BasicBlock::iterator i = blk->begin(), e = blk->end(); i != e; ++i) {
                        errs() << *i << "\n";
                        if (isa<CallInst>(i)) {
                            errs() << "yes: " << "\n";
                            Function* getCall = cast<CallInst>(i)->getCalledFunction();
                            errs() << "Called Function name: " << getCall->getName() << "\n";
                            if (getCall->getName().startswith("p") && (getCall->getFunctionType()->getReturnType()==IntegerType::get(M.getContext(), 32)) && getCall->getName()!="printf"){
                                Function* NewF;
                                if(cloneFunSet.find(getCall->getName())==cloneFunSet.end()) {
                                    errs() << "\n\n\nFunction NOT cloned\n\n\n";
                                    cloneFunSet.insert(getCall->getName());

                                    errs() << "\nstart with p && return type is int && not system function\n\n";
                                    ValueToValueMapTy VMap;
                                    errs() << "\nFunction pow2 type: " << *getCall->getFunctionType();
                                    errs() << "\nFunction pow2 return type: " <<
                                           *getCall->getFunctionType()->getReturnType() << "\n";

                                    std::vector<Type *> ArgTypes;

                                    // The user might be deleting arguments to the function by specifying them in
                                    // the VMap.  If so, we need to not add the arguments to the arg ty vector
                                    //
                                    for (Function::const_arg_iterator I = getCall->arg_begin(), E = getCall->arg_end();
                                         I != E; ++I)
                                        if (VMap.count(I) == 0)  // Haven't mapped the argument to anything yet?
                                            ArgTypes.push_back(I->getType());

                                    // Create a new function type...
                                    FunctionType *FTy = FunctionType::get(Type::getVoidTy(M.getContext()),
                                                                          ArgTypes,
                                                                          getCall->getFunctionType()->isVarArg());

                                    // Create the new function...
                                    Function *NewFF = Function::Create(FTy, getCall->getLinkage(), getCall->getName());

                                    // Loop over the arguments, copying the names of the mapped arguments over...
                                    Function::arg_iterator DestI = NewFF->arg_begin();
                                    for (Function::const_arg_iterator I = getCall->arg_begin(), E = getCall->arg_end();
                                         I != E; ++I)
                                        if (VMap.count(I) == 0) {   // Is this argument preserved?
                                            DestI->setName(I->getName()); // Copy the name over...
                                            VMap[I] = DestI++;        // Add mapping to VMap
                                        }

                                    SmallVector < ReturnInst * , 8 > Returns;  // Ignore returns cloned.

                                    CloneFunctionInto(NewFF, getCall, VMap, true, Returns, "");
                                    StringRef name = getCall->getName();
                                    NewFF->setName(name + "_cloned");
                                    getCall->getParent()->getFunctionList().push_back(NewFF);
                                    NewF = NewFF;
                                } else{
                                    errs() << "\n\n\nFunction cloned\n\n\n";
                                    StringRef ClonedName=getCall->getName();
                                    std::string tmp_name = ClonedName;
                                    tmp_name += "_cloned";
                                    ClonedName = tmp_name;
                                    NewF = M.getFunction(ClonedName);
                                }

//                                Function* callChange;
//                                callChange = M.getFunction("pow2_cloned");
                                errs() << "# OPRD called ------- : " << i->getNumOperands() << "\n";
                                for (int count=0; count < i->getNumOperands(); count++){
                                    errs() << "OPRD called ------- : " << *i->getOperand(count) << "\n";
                                }
                                errs() << "output *i again: " << *i << "\n";

                                // new call site
                                CallInst* void_63 = CallInst::Create(NewF, i->getOperand(0), "");
                                void_63->setCallingConv(CallingConv::C);
                                void_63->setTailCall(false);
                                AttributeSet void_63_PAL;
                                void_63->setAttributes(void_63_PAL);
                                blk->getInstList().insert(i, void_63);


                                // find the next ins after calling
                                i++;
                                errs() << "(i+1)->getOperand(0): " << *i->getOperand(0) << "\n";
                                errs() << "(i+1)->getOperand(1): " << *i->getOperand(1) << "\n";

//                                  insert:  %3 = load i32* @globbbbbbbbbb, align 4
                                // get %p (the second OPRD)
                                // alloca another int (%n)
                                // load glob to that int
//                                    Value* val_temp = i->getOperand(1);
                                LoadInst* int32_19 = new LoadInst(gvar_int32_glob, "", false);
                                int32_19->setAlignment(4);
                                blk->getInstList().insert(i, int32_19);

                                // store %n to %p
//                                  insert:  store i32 %3, i32* %p, align 4
                                StoreInst* void_28 = new StoreInst(int32_19, i->getOperand(1), false);
                                void_28->setAlignment(4);
                                blk->getInstList().insert(i, void_28);

                                // delete ins: store i32 %3(this %3 is the previous var), i32* %p, align 4
                                Instruction* tmp = i;
                                i--;
                                errs() << "\n\n\nins: " << *tmp;
                                tmp->eraseFromParent();
                                errs() << "  was removed\n";

                                // find the old call ins
                                i--;i--;
                                tmp = i;
                                //erase the old call ins
                                i--;
                                errs() << "ins: " << *tmp;
                                tmp->eraseFromParent();
                                errs() << "  was removed\n";
                                errs() << "now iter is point to: " << *i << "\n\n";


                                // Iter over cloned func
                                errs() << "Function (name=" << NewF->getName() << ") has "
                                       << NewF->size() << " basic blocks.\n";
                                for (Function::iterator blk = NewF->begin(), e = NewF->end(); blk != e; ++blk) {
                                    errs() << "Basic block (name=" << blk->getName() << ") has "
                                           << blk->size() << " instructions.\n";
                                    for (BasicBlock::iterator i = blk->begin(), e = blk->end(); i != e; ++i) {
                                        errs() << *i << "\n";
                                        if (i->getOpcode()==1&&i->getNumOperands()==1) { //return && not return void
                                            errs() << "return ins\n";
                                            Value *retVal;
                                            retVal = i->getOperand(0);
                                            errs() << "return value: " << *retVal << "\n";
                                            errs() << "------------pow2_cloned------------\n";

                                            //let glob store the value
                                            StoreInst *void_27 = new StoreInst(retVal, gvar_int32_glob, false);
                                            void_27->setAlignment(4);
                                            blk->getInstList().insert(i, void_27);

                                            //call pop_direct_br
                                            CallInst *void_62 = CallInst::Create(func_pop_direct_branch, "");
                                            void_62->setCallingConv(CallingConv::C);
                                            void_62->setTailCall(false);
                                            AttributeSet void_62_PAL;
                                            void_62->setAttributes(void_62_PAL);

                                            blk->getInstList().insert(i, void_62);

                                            // change return instruction

                                            Instruction *tmp = i;
                                            errs() << "ins: "<< *tmp;
                                            i--;
                                            tmp->eraseFromParent();//remove the old return instruction
                                            errs() << "   was removed\n";
                                            errs() << "now iter is point to: "<< *i << "\n";
                                            ReturnInst::Create(M.getContext(), blk);
                                            errs() << "new ret ins created\n";
                                        }
                                    }
                                }
                            }
                        } else {
                            errs() << "no: " << "\n";
                        }
                    }
                }
            }

            errs() << "loop finished\n";

            return false;
        }

    };
}

char Hello::ID = 0;
static RegisterPass<Hello> X("hello", "Hello World Pass", false, false);



