/*
 * InitialValueSymbolResolver.cpp
 *
 *  Created on: Jul 25, 2013
 *      Author: andy
 */

#include "InitialValueSymbolResolver.h"
#include "ASTNodeCodeGen.h"
#include "LLVMException.h"
#include <sbml/Model.h>

using namespace std;
using namespace libsbml;
using namespace llvm;

namespace rr
{

InitialValueSymbolResolver::InitialValueSymbolResolver(
        const libsbml::Model* model,
        const LLVMModelDataSymbols& modelDataSymbols,
        const LLVMModelSymbols& modelSymbols,
        llvm::IRBuilder<>& builder) :
        model(model),
        modelDataSymbols(modelDataSymbols),
        modelSymbols(modelSymbols), builder(builder)
{
}

InitialValueSymbolResolver::~InitialValueSymbolResolver()
{
}

llvm::Value* InitialValueSymbolResolver::loadSymbolValue(
        const std::string& symbol)
{
    /*************************************************************************/
    /* AssignmentRule */
    /*************************************************************************/
    {
        SymbolForest::ConstIterator i =
                modelSymbols.getAssigmentRules().find(symbol);
        if (i != modelSymbols.getAssigmentRules().end())
        {
            return ASTNodeCodeGen(builder, *this).codeGen(i->second);
        }
    }

    /*************************************************************************/
    /* Initial Value */
    /*************************************************************************/
    {
        SymbolForest::ConstIterator i =
                modelSymbols.getInitialValues().find(symbol);

        if (i != modelSymbols.getInitialValues().end())
        {
            return ASTNodeCodeGen(builder, *this).codeGen(i->second);
        }
    }

    string msg = "Could not find requested symbol \'";
    msg += symbol;
    msg += "\' in the model";
    throw_llvm_exception(msg);

    return 0;
}

} /* namespace rr */


