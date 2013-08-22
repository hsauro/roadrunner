/*
 * LLVMExecutableModel.cpp
 *
 * Author: Andy Somogyi,
 *     email decode: V1 = "."; V2 = "@"; V3 = V1;
 *     andy V1 somogyi V2 gmail V3 com
 */

#include "LLVMExecutableModel.h"
#include "LLVMIncludes.h"
#include "rrSparse.h"
#include "rrLogger.h"
#include "rrException.h"
#include "LLVMException.h"
#include <iomanip>
#include <cstdlib>

// static assertion, taken from
// http://msdn.microsoft.com/en-us/library/windows/desktop/ms679289(v=vs.85).aspx
#ifndef C_ASSERT
#define C_ASSERT(e) typedef char __C_ASSERT__[(e)?1:-1]
#endif

static void dump_array(std::ostream &os, int n, const double *p)
{
    os << setiosflags(ios::floatfield) << setprecision(8);
    os << '[';
    for (int i = 0; i < n; ++i)
    {
        os << p[i];
        if (i < n - 1)
        {
            os << ", ";
        }
    }
    os << ']' << endl;
}

namespace rr
{

static int getValues(LLVMModelData* modelData, double (*funcPtr)(LLVMModelData*, int),
        int len, int const *indx, double *values)
{
    for (int i = 0; i < len; ++i)
    {
        int j = indx ? indx[i] : i;
        values[i] = funcPtr(modelData, j);
    }
    return len;
}

LLVMExecutableModel::LLVMExecutableModel() :
    symbols(0),
    context(0),
    executionEngine(0),
    errStr(0),
    evalInitialConditionsPtr(0),
    evalReactionRatesPtr(0),
    getBoundarySpeciesAmountPtr(0),
    getFloatingSpeciesAmountPtr(0),
    getBoundarySpeciesConcentrationPtr(0),
    getFloatingSpeciesConcentrationPtr(0),
    getGlobalParameterPtr(0),
    getCompartmentVolumePtr(0),
    stackDepth(0),
    evalRateRuleRatesPtr(0),
    getEventTriggerPtr(0),
    getEventPriorityPtr(0),
    getEventDelayPtr(0),
    eventTriggerPtr(0),
    eventAssignPtr(0)
{
    // zero out the struct, the generator will fill it out.
    LLVMModelData::init(modelData);

    modelData.time = -1.0;

    std::srand(std::time(0));
}

LLVMExecutableModel::~LLVMExecutableModel()
{
    Log(Logger::PRIO_TRACE) << __FUNC__ << "ModelName: " << modelData.modelName;

    if (errStr->size() > 0)
    {
        Log(Logger::PRIO_WARNING) << "Non-empty LLVM ExecutionEngine error string: " << *errStr;
    }

    LLVMModelData::freeBuffers(modelData);
    delete symbols;
    // the exe engine owns all the functions
    delete executionEngine;
    delete context;
    delete errStr;
}

string LLVMExecutableModel::getModelName()
{
    return modelData.modelName;
}

void LLVMExecutableModel::setTime(double time)
{
    modelData.time = time;
}

double LLVMExecutableModel::getTime()
{
    return modelData.time;
}

int LLVMExecutableModel::getNumIndependentSpecies()
{
    return modelData.numIndependentSpecies;
}

int LLVMExecutableModel::getNumDependentSpecies()
{
    return modelData.numDependentSpecies;
}

int LLVMExecutableModel::getNumFloatingSpecies()
{
    return symbols->getFloatingSpeciesSize();
}

int LLVMExecutableModel::getNumBoundarySpecies()
{
    return symbols->getBoundarySpeciesSize();
}

int LLVMExecutableModel::getNumGlobalParameters()
{
    return symbols->getGlobalParametersSize();
}

int LLVMExecutableModel::getNumCompartments()
{
    return symbols->getCompartmentsSize();
}

int LLVMExecutableModel::getNumReactions()
{
    return modelData.numReactions;
}

int LLVMExecutableModel::getNumLocalParameters(int reactionId)
{
    return 0;
}

void LLVMExecutableModel::convertToAmounts()
{
}

void LLVMExecutableModel::computeConservedTotals()
{
}

double LLVMExecutableModel::getFloatingSpeciesConcentration(int index)
{
    /*
    if (index >= 0 && index < modelData.numFloatingSpecies)
    {
        int compIndex = modelData.floatingSpeciesCompartments[index];
        return modelData.floatingSpeciesAmounts[index] /
                modelData.compartmentVolumes[compIndex];
    }
    else
    {
        Log(Logger::PRIO_ERROR) << "index " << index << "out of range";
        throw Exception(string(__FUNC__) + string(": index out of range"));
    }
    */
    return 0;
}

int LLVMExecutableModel::getFloatingSpeciesConcentrations(int len, int const *indx,
        double *values)
{
    return getValues(&modelData, getFloatingSpeciesConcentrationPtr, len, indx, values);
}

void LLVMExecutableModel::getRateRuleValues(double *rateRuleValues)
{
    memcpy(rateRuleValues, modelData.rateRuleValues, modelData.numRateRules * sizeof(double));
}

void LLVMExecutableModel::setRateRuleValues(const double *rateRuleValues)
{
    memcpy(modelData.rateRuleValues, rateRuleValues, modelData.numRateRules * sizeof(double));
}

void LLVMExecutableModel::convertToConcentrations()
{
}

void LLVMExecutableModel::updateDependentSpeciesValues()
{
}

void LLVMExecutableModel::computeAllRatesOfChange()
{
}

void LLVMExecutableModel::evalModel(double time, const double *y, double *dydt)
{
    modelData.time = time;

    if (y)
    {
        setStateVector(y);
    }

    evalReactionRates();

    csr_matrix_dgemv(1.0, modelData.stoichiometry, modelData.reactionRates,
                     0.0, modelData.floatingSpeciesAmountRates);

    evalRateRuleRatesPtr(&modelData);

    if (dydt)
    {
        memcpy(dydt, modelData.rateRuleRates, modelData.numRateRules * sizeof(double));

        memcpy(dydt + modelData.numRateRules, modelData.floatingSpeciesAmountRates,
                modelData.numIndependentSpecies * sizeof(double));
    }

    /*
    if (Logger::PRIO_TRACE <= rr::Logger::GetLogLevel()) {

        LoggingBuffer log(Logger::PRIO_TRACE, __FILE__, __LINE__);

        log.stream() << __FUNC__ << endl;
        log.stream() << "y: ";
        if (y) {
            dump_array(log.stream(), modelData.numRateRules + modelData.numFloatingSpecies, y);
        } else {
            log.stream() << "null";
        }
        log.stream() << endl << "dydt: ";
        if (dydt) {
            dump_array(log.stream(), modelData.numRateRules + modelData.numFloatingSpecies, dydt);
        } else {
            log.stream() << "null";
        }
        log.stream() << endl << "Model: " << endl << this;
    }
    */
}

void LLVMExecutableModel::testConstraints()
{
}

string LLVMExecutableModel::getInfo()
{
    return "";
}

int LLVMExecutableModel::getFloatingSpeciesIndex(const string& allocator)
{
    Log(Logger::PRIO_FATAL) << "Not Implemented: " << __FUNCTION__;
    throw Exception(string("Not Implemented: ") + __FUNCTION__);
    return 0;
}

string LLVMExecutableModel::getFloatingSpeciesId(int index)
{
    vector<string> ids = symbols->getFloatingSpeciesIds();
    return ids[index];
}

int LLVMExecutableModel::getBoundarySpeciesIndex(const string& id)
{
    return symbols->getBoundarySpeciesIndex(id);
}

string LLVMExecutableModel::getBoundarySpeciesId(int indx)
{
    vector<string> ids = symbols->getBoundarySpeciesIds();
    return ids[indx];
}

int LLVMExecutableModel::getBoundarySpeciesCompartmentIndex(int int1)
{
    Log(Logger::PRIO_FATAL) << "Not Implemented: " << __FUNCTION__;
    throw Exception(string("Not Implemented: ") + __FUNCTION__);
    return 0;
}

int LLVMExecutableModel::getGlobalParameterIndex(const string& allocator)
{
    Log(Logger::PRIO_FATAL) << "Not Implemented: " << __FUNCTION__;
    throw Exception(string("Not Implemented: ") + __FUNCTION__);
    return 0;
}

string LLVMExecutableModel::getGlobalParameterId(int id)
{
    vector<string> ids = symbols->getGlobalParameterIds();
    if (id < ids.size())
    {
        return ids[id];
    }
    else
    {
        throw_llvm_exception("index out of range");
        return "";
    }
}

int LLVMExecutableModel::getCompartmentIndex(const string& allocator)
{
    Log(Logger::PRIO_FATAL) << "Not Implemented: " << __FUNCTION__;
    throw Exception(string("Not Implemented: ") + __FUNCTION__);
    return 0;
}

string LLVMExecutableModel::getCompartmentId(int id)
{
    vector<string> ids = symbols->getCompartmentIds();
    if (id < ids.size())
    {
        return ids[id];
    }
    else
    {
        throw_llvm_exception("index out of range");
        return "";
    }
}

int LLVMExecutableModel::getReactionIndex(const string& allocator)
{
    Log(Logger::PRIO_FATAL) << "Not Implemented: " << __FUNCTION__;
    throw Exception(string("Not Implemented: ") + __FUNCTION__);
    return 0;
}

string LLVMExecutableModel::getReactionId(int id)
{
    vector<string> ids = symbols->getReactionIds();
    if (id < ids.size())
    {
        return ids[id];
    }
    else
    {
        throw_llvm_exception("index out of range");
        return "";
    }
}

void LLVMExecutableModel::evalInitialConditions()
{
    evalInitialConditionsPtr(&modelData);
}

void LLVMExecutableModel::reset()
{
    // eval the initial conditions and rates
    setTime(0.0);
    evalInitialConditions();
    evalReactionRates();

    // this sets up the event system to pull the initial value
    // before the simulation starts.
    setTime(-1.0);

    Log(Logger::PRIO_TRACE) << __FUNC__ << this;
}

bool LLVMExecutableModel::getConservedSumChanged()
{
    return false;
}

void LLVMExecutableModel::setConservedSumChanged(bool val)
{
}

int LLVMExecutableModel::getStateVector(double* stateVector)
{
    if (stateVector == 0)
    {
        Log(Logger::PRIO_TRACE) << __FUNC__ << ", stateVector: null, returning "
                << modelData.numRateRules + modelData.numIndependentSpecies;
        return modelData.numRateRules + modelData.numIndependentSpecies;
    }

    getRateRuleValues(stateVector);

    memcpy(stateVector + modelData.numRateRules,
            modelData.floatingSpeciesAmounts,
            modelData.numIndependentSpecies * sizeof(double));


    if (Logger::PRIO_TRACE <= rr::Logger::GetLogLevel()) {

        LoggingBuffer log(Logger::PRIO_TRACE, __FILE__, __LINE__);

        log.stream() << endl << __FUNC__ <<  ", Model: " << endl << this;

        log.stream() << __FUNC__ << ",  out stateVector: ";
        if (stateVector) {
            dump_array(log.stream(), modelData.numRateRules + modelData.numIndependentSpecies, stateVector);
        } else {
            log.stream() << "null";
        }
    }

    return modelData.numRateRules + modelData.numIndependentSpecies;
}

int LLVMExecutableModel::setStateVector(const double* stateVector)
{
    if (stateVector == 0)
    {
        return -1;
    }

    setRateRuleValues(stateVector);

    memcpy(modelData.floatingSpeciesAmounts,
            stateVector + modelData.numRateRules,
            modelData.numIndependentSpecies * sizeof(double));

    /*
    if (Logger::PRIO_TRACE <= rr::Logger::GetLogLevel()) {

        LoggingBuffer log(Logger::PRIO_TRACE, __FILE__, __LINE__);

        log.stream() << endl << __FUNC__ <<  ", Model: " << endl << this;

        log.stream() << __FUNC__ << ",  stateVector: ";
        if (stateVector) {
            dump_array(log.stream(), modelData.numRateRules + modelData.numIndependentSpecies, stateVector);
        } else {
            log.stream() << "null";
        }
    }
    */

    return modelData.numRateRules + modelData.numIndependentSpecies;
}

void LLVMExecutableModel::print(std::ostream &stream)
{
    stream << "LLVMExecutableModel" << endl;
    stream << "stackDepth: " << stackDepth << endl;
    stream << modelData;
}

LLVMExecutableModel* LLVMExecutableModel::dummy()
{
    return new LLVMExecutableModel();
}

void LLVMExecutableModel::evalReactionRates()
{
    evalReactionRatesPtr(&modelData);
}

int LLVMExecutableModel::getNumRules()
{
    return 0;
}

int LLVMExecutableModel::getFloatingSpeciesAmounts(int len, const int* indx,
        double* values)
{
    return getValues(&modelData, getFloatingSpeciesAmountPtr, len, indx, values);
}

int LLVMExecutableModel::setFloatingSpeciesConcentrations(int len,
        const int* indx, const double* values)
{
    Log(Logger::PRIO_FATAL) << "Not Implemented: " << __FUNCTION__;
    throw Exception(string("Not Implemented: ") + __FUNCTION__);
    return -1;
}

int LLVMExecutableModel::getBoundarySpeciesAmounts(int len, const int* indx,
        double* values)
{
    return getValues(&modelData, getBoundarySpeciesAmountPtr, len, indx, values);
}

int LLVMExecutableModel::getBoundarySpeciesConcentrations(int len,
        const int* indx, double* values)
{
    return getValues(&modelData, getBoundarySpeciesConcentrationPtr, len, indx, values);
}

int LLVMExecutableModel::setBoundarySpeciesConcentrations(int len,
        const int* indx, const double* values)
{
    Log(Logger::PRIO_FATAL) << "Not Implemented: " << __FUNCTION__;
    throw Exception(string("Not Implemented: ") + __FUNCTION__);
    return -1;
}

int LLVMExecutableModel::getGlobalParameterValues(int len, const int* indx,
        double* values)
{
    return getValues(&modelData, getGlobalParameterPtr, len, indx, values);
}

int LLVMExecutableModel::setGlobalParameterValues(int len, const int* indx,
        const double* values)
{
    Log(Logger::PRIO_FATAL) << "Not Implemented: " << __FUNCTION__;
    throw Exception(string("Not Implemented: ") + __FUNCTION__);
    return -1;
}


int LLVMExecutableModel::getCompartmentVolumes(int len, const int* indx,
        double* values)
{
    return getValues(&modelData, getCompartmentVolumePtr, len, indx, values);
}



int LLVMExecutableModel::getReactionRates(int len, const int* indx,
        double* values)
{
    for (int i = 0; i < len; ++i)
    {
        int j = indx ? indx[i] : i;
        if (j < modelData.numReactions)
        {
            values[i] = modelData.reactionRates[j];
        }
        else
        {
            throw Exception("index out of range");
        }
    }
    return len;
}

int LLVMExecutableModel::getNumConservedSums()
{
    return 0;
}

int LLVMExecutableModel::getConservedSumIndex(const string& name)
{
    return 0;
}

string LLVMExecutableModel::getConservedSumId(int index)
{
    return "";
}

int LLVMExecutableModel::getConservedSums(int len, const int* indx,
        double* values)
{
    return 0;
}

int LLVMExecutableModel::setConservedSums(int len, const int* indx,
        const double* values)
{
    return 0;
}

int LLVMExecutableModel::getFloatingSpeciesAmountRates(int len,
        int const *indx, double *values)
{
    /*
    for (int i = 0; i < len; ++i)
    {
        int j = indx ? indx[i] : i;
        if (j < modelData.numFloatingSpecies)
        {
            values[i] = modelData.floatingSpeciesAmountRates[j];
        }
        else
        {
            throw LLVMException("index out of range");
        }
    }
    return len;
    */
    return 0;
}


int LLVMExecutableModel::setFloatingSpeciesAmounts(int len, int const *indx,
        const double *values)
{
    return -1;
}


int LLVMExecutableModel::setCompartmentVolumes(int len, const int* indx,
        const double* values)
{
    return 0;
}

int LLVMExecutableModel::setFloatingSpeciesInitConcentrations(int len,
        const int* indx, const double* values)
{
    return 0;
}

int LLVMExecutableModel::getFloatingSpeciesInitConcentrations(int len,
        const int* indx, double* values)
{
    return 0;
}

double LLVMExecutableModel::getStoichiometry(int index)
{
    return 0;
}

/******************************* Events Section *******************************/
#if (1) /**********************************************************************/
/******************************************************************************/

int LLVMExecutableModel::getNumEvents()
{
    return modelData.numEvents;
}

int LLVMExecutableModel::getEventTriggers(int len, const int *indx, unsigned char *values)
{
    const vector<unsigned char>& attr = symbols->getEventAttributes();

    if (len <= 0)
    {
        return modelData.numEvents;
    }
    else
    {
        for (int i = 0; i < len; ++i)
        {
            int j = indx ? indx[i] : i;
            if (j < modelData.numEvents)
            {
                values[j] = getEventTrigger(j);
            }
            else
            {
                throw Exception("index out of range");
            }
        }
        return len;
    }
}

void LLVMExecutableModel::evalEvents(double timeEnd,
        const unsigned char* previousEventStatus, const double *initialState,
        double* finalState)
{
    modelData.time = timeEnd;
    setStateVector(initialState);

    vector<unsigned char> prevEventState(previousEventStatus,
            previousEventStatus + modelData.numEvents);

    vector<unsigned char> currEventStatus(modelData.numEvents);

    unsigned char *p1 = &prevEventState[0];
    unsigned char *p2 = &currEventStatus[0];

    pendingEvents.make_heap();
    pendingEvents.eraseExpiredEvents();

    while(applyEvents(p1, p2))
    {
        std::swap(p1, p2);
    }

    getStateVector(finalState);
}

/**
 * Each applied event application typically results in a state change.
 *
 * At each state change, we need to re-scan the events and see
 * if any new events become triggered or events expire.
 */
int LLVMExecutableModel::applyPendingEvents(const double *stateVector,
        double timeEnd, double tout)
{
    int assignedEvents = 0;
    modelData.time = timeEnd;
    setStateVector(stateVector);

    vector<unsigned char> prevEventState(modelData.numEvents);
    vector<unsigned char> currEventStatus(modelData.numEvents);

    getEventTriggers(prevEventState.size(), 0, &prevEventState[0]);

    unsigned char *p1 = &prevEventState[0];
    unsigned char *p2 = &currEventStatus[0];

    pendingEvents.make_heap();
    pendingEvents.eraseExpiredEvents();

    while (applyEvents(p1, p2))
    {
        assignedEvents++;
        std::swap(p1, p2);
    }

    return assignedEvents;
}

void  LLVMExecutableModel::evalEventRoots(double time, const double* y, double* gdot)
{
    modelData.time = time;

    if (y)
    {
        setStateVector(y);
    }

    for (uint i = 0; i < modelData.numEvents; ++i)
    {
        unsigned char triggered = getEventTriggerPtr(&modelData, i);

        gdot[i] = triggered ? 1.0 : -1.0;
    }

    return;
}

double LLVMExecutableModel::getNextPendingEventTime(bool pop)
{
    if (pendingEvents.size())
    {
        return pendingEvents.top().assignTime;
    }
    return 0;
}

int LLVMExecutableModel::getPendingEventSize()
{
    return pendingEvents.size();
}

void LLVMExecutableModel::resetEvents()
{
}

bool LLVMExecutableModel::applyEvents(unsigned char* prevEventState,
        unsigned char* currEventState)
{
    for (uint i = 0; i < modelData.numEvents; ++i)
    {
        bool c = getEventTrigger(i);
        currEventState[i] = c;

        Log(Logger::PRIO_DEBUG) << "event " << i << ", previous state: " <<
                (bool)prevEventState[i] << ", current state: " << (bool)c;

        // transition from non-triggered to triggered
        if (c && !prevEventState[i])
        {
            pendingEvents.push(rrllvm::Event(*this, i));
        }
    }

    // fire the highest priority event, this causes state change
    // return true if we incured a state change
    return pendingEvents.applyEvent();
}

bool LLVMExecutableModel::getEventTieBreak(uint eventA, uint eventB)
{
    /*
    C_ASSERT(sizeof(TieBreakKey) == 8 && sizeof(uint) == 4);

    bool result;
    TieBreakKey keyA = eventA;
    TieBreakKey keyB = eventB;
    TieBreakKey key = keyA << 32 | keyB;
    TieBreakMap::iterator i = tieBreakMap.find(key);
    if (i != tieBreakMap.end())
    {
        result = !i->second;
        i->second = result;
    }
    else
    {
        result = true;
        tieBreakMap.insert(std::pair<TieBreakKey,bool>(key, result));
    }

    Log(Logger::PRIO_DEBUG) << "tie break, a: " << std::hex << keyA <<
            ", b: " << keyB << ", key: " << std::hex << key << ", value: " <<
            result;
    return result;
    */
    //bool result =  (tie_break++ % 2) == 0;
    bool result = std::rand() % 2 == 0;
    Log(Logger::PRIO_DEBUG) << "tie break, a" << eventA << ", b: " << eventB << ", result: " << result;
    return result;
}


/******************************* Events Section *******************************/
#endif /***********************************************************************/
/******************************************************************************/


} /* namespace rr */


