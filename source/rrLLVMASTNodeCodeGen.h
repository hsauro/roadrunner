/*
 * rrLLVMASTNodeCodeGen.h
 *
 * Created on: Jun 16, 2013
 *
 * Author: Andy Somogyi,
 *     email decode: V1 = "."; V2 = "@"; V3 = V1;
 *     andy V1 somogyi V2 gmail V3 com
 */
#ifndef rrLLVMASTNodeCodeGenH
#define rrLLVMASTNodeCodeGenH

#include "rrLLVMIncludes.h"

namespace libsbml
{
class ASTNode;
}


namespace rr
{
/**
 * All of the LLVM IR generation is handled here.
 */
class LLVMASTNodeCodeGen
{
public:
    LLVMASTNodeCodeGen();
    ~LLVMASTNodeCodeGen();

    llvm::Value *Codegen(const libsbml::ASTNode *ast);

    /**
     * sbml ASTNode does not contain as assigment '=' type, assigment
     * is handled by other sbml elements such as initialAssigment.
     *
     */
    llvm::Value *BinaryExprCodegen(const libsbml::ASTNode *ast);

    /**
     * most of the AST types correspond to llvm intrinsic, i.e.
     * cos, sqrt, pow, etc...
     */
    llvm::Value *IntrinsicCallCodegen(const libsbml::ASTNode *ast);

    llvm::IRBuilder<> builder;
};

} /* namespace rr */
#endif /* rrLLVMASTNodeCodeGenH */