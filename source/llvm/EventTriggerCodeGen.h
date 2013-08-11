/*
 * EventTriggerCodeGen.h
 *
 *  Created on: Aug 11, 2013
 *      Author: andy
 */

#ifndef EVENTTRIGGERCODEGEN_H_
#define EVENTTRIGGERCODEGEN_H_

#include "EventCodeGenBase.h"

namespace rr
{

typedef void (*EventTriggerCodeGen_FunctionPtr)(LLVMModelData*, int32_t);

class EventTriggerCodeGen:
        public EventCodeGenBase<EventTriggerCodeGen>
{
public:
    EventTriggerCodeGen(const ModelGeneratorContext &mgc);
    ~EventTriggerCodeGen();

    static const char* FunctionName;
};

} /* namespace rr */
#endif /* EVENTTRIGGERCODEGEN_H_ */
