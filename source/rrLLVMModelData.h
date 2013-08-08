/*
 * rrLLVMModelData.h
 *
 *  Created on: Aug 8, 2013
 *      Author: andy
 */

#ifndef RRLLVMMODELDATA_H_
#define RRLLVMMODELDATA_H_

#include "rrSparse.h"
#include <string>
#include <ostream>

namespace rr {

struct LLVMModelData;

typedef double   (*LLVMEventDelayHandler)(LLVMModelData*);
typedef double*  (*LLVMComputeEventAssignmentHandler)(LLVMModelData*);
typedef void     (*LLVMPerformEventAssignmentHandler)(LLVMModelData*, double*);
typedef void     (*LLVMEventAssignmentHandler)();


/**
 * A data structure that is that allows data to be exchanged
 * with running SBML models. In the case of CExecutableModels, A pointer to
 * this struct is given to the compiled shared library, and the C code
 * there modifies the buffers of this structure.
 *
 * There are some functions in ExecutableModel.h that manage ModelData
 * memory. These would have made more sense here, but in order to prevent
 * any issues with generated code interacting with them, they were placed
 * there.
 *
 * Basic Nomencalture
 * Compartments: A well stirred compartment which contains one or more species.
 * the volume of a compartment can change durring the course of a simulation.
 *
 * Floating Species: these are chemical species who's values (ammount / concentration)
 * change over time.
 *
 * Boundary Species: chemical species that who's values are fixed to their initial conditions,
 * these function as boundary conditions.
 *
 * @see ExecutableModel.h
 */
struct LLVMModelData
{
    /**
     * sizeof this struct, make sure we use the correct
     * size in LLVM land.
     */
    unsigned                            size;                             // 0

    unsigned                            flags;                            // 1

    /**
     * current time.
     */
    double                              time;                             // 2

    /**
     * number of linearly independent rows in the stochiometry matrix.
     */
    unsigned                            numIndependentSpecies;            // 3

    /**
     * number of linerly dependent rows in the stoichiometry matrix.
     *
     * numIndependentVariables + numDependentVariables had better
     * be equal to numFloatingSpecies
     */
    unsigned                            numDependentSpecies;              // 4
    double*                             dependentSpeciesConservedSums;    // 5

    /**
     * number of global parameters
     */
    unsigned                            numGlobalParameters;              // 6
    double*                             globalParameters;                 // 7

    /**
     * number of reactions, same as ratesSize.
     * These are the calcuated reaction rates, not the
     * species rates.
     */
    unsigned                            numReactions;                     // 8
    double*                             reactionRates;                    // 9

    unsigned                            numRateRules;                     // 10

    /**
     * All of the elelments which have a rate rule are stored here.
     *
     * As the integrator runs, this pointer can simply point to an offset
     * in the integrator's state vector.
     *
     * Only used in the LLVM version.
     */
    double*                             rateRuleValues;                   // 11

    /**
     * the rate of change of all elements who's dynamics are determined
     * by rate rules.
     *
     * In the LLVM version, this is just a pointer to a data block
     * owned by the integrator. In the C version, lots of strange
     * stuff goes on here, but the C version does allocate this block,
     * and do all sorts of copying back and forth between the integrator's
     * rate rule block.
     */
    double*                             rateRuleRates;                    // 12

    /**
     * LLVM specific
     * C version does not support local parameters
     * This is the offset, or starting index of the local parameters
     * for reaction i. Length is numReactions.
     *
     * Rationale: It is simple more effecient to store all the local
     * parameters in a single dimensional array with offsets, as the
     * offsets can be computed at compile time, whereas if we used
     * a array of arrays, it would require an additional memory access
     * to determine the location of the parameter.
     */
    unsigned*                           localParametersOffsets;           // 12

    /**
     * the number of local parameters for each reaction,
     * so legnth is numReactions. This is an array of counts,
     * hence it is named differently than the rest of the num*** fields.
     */
    unsigned*                           localParametersNum;               // 14

    /**
     * All local parameters are stored in this array. This has
     * length sum(localParameterNum).
     */
    double*                             localParameters;                  // 15

    /**
     * The total ammounts of the floating species, i.e.
     * concentration * compartment volume.
     * Everything named floatingSpecies??? has length numFloatingSpecies.
     *
     * Note, the floating species consist of BOTH independent AND dependent
     * species. Indexes [0,numIndpendentSpecies) values are the indenpendent
     * species, and the [numIndependentSpecies,numIndendentSpecies+numDependentSpecies)
     * contain the dependent species.
     */
    unsigned                            numFloatingSpecies;               // 16

    /**
     * number of floating species and floating species concentrations.
     */
    double*                             floatingSpeciesConcentrations;    // 17

    /**
     * initial concentration values for floating species.
     */
    double*                             floatingSpeciesInitConcentrations;// 18

    /**
     * amount rates of change for floating species.
     */
    double*                             floatingSpeciesAmountRates;       // 19

    /**
     * The total amount of a species in a compartment.
     */
    double*                             floatingSpeciesAmounts;           // 20

    /**
     * compartment index for each floating species,
     * e.g. the volume of the i'th species is
     * md->compartmentVolumes[md->floatingSpeciesCompartments[i]]
     */
    unsigned*                           floatingSpeciesCompartments;      // 21

    /**
     * number of boundary species and boundary species concentrations.
     * units: either
     * Mass Percent = (Mass of Solute) / (Mass of Solution) x 100%
     * Volume Percent= (Volume of Solute) / (Volume of Solution) x 100%
     * Mass/Volume Percent= (Mass of Solute) / (Volume of Solution) x 100%
     */
    unsigned                            numBoundarySpecies;               // 22
    double*                             boundarySpeciesConcentrations;    // 23
    double*                             boundarySpeciesAmounts;           // 24

    /**
     * compartment index for each boundary species,
     * e.g. the volume of the i'th species is
     * md->compartmentVolumes[md->boundarySpeciesCompartments[i]]
     */
    unsigned*                           boundarySpeciesCompartments;      // 25

    /**
     * number of compartments, and compartment volumes.
     * units: volume
     */
    unsigned                            numCompartments;                  // 26
    double*                             compartmentVolumes;               // 27

    /**
     * stoichiometry matrix
     */
    csr_matrix*                         stoichiometry;                    // 28


    //Event stuff
    unsigned                            numEvents;                        // 29
    unsigned                            eventTypeSize;                    // 30
    bool*                               eventType;                        // 31

    unsigned                            eventPersistentTypeSize;          // 32
    bool*                               eventPersistentType;              // 33

    unsigned                            eventTestsSize;                   // 34
    double*                             eventTests;                       // 35

    unsigned                            eventPrioritiesSize;              // 36
    double*                             eventPriorities;                  // 37

    unsigned                            eventStatusArraySize;             // 38
    bool*                               eventStatusArray;                 // 39

    unsigned                            previousEventStatusArraySize;     // 40
    bool*                               previousEventStatusArray;         // 41

    /**
     * number of items in the state vector.
     */
    unsigned                            stateVectorSize;                  // 42

    /**
     * the state vector, this is usually a pointer to a block of data
     * owned by the integrator.
     */
    double*                             stateVector;                      // 43

    /**
     * the rate of change of the state vector, this is usually a pointer to
     * a block of data owned by the integrator.
     */
    double*                             stateVectorRate;                  // 44

    /**
     * Work area for model implementations. The data stored here is entirely
     * model implementation specific and should not be accessed
     * anywhere else.
     *
     * allocated by allocModelDataBuffers based on the value of workSize;
     */
    unsigned                            workSize;                         // 45
    double*                             work;                             // 46

    LLVMEventDelayHandler*                  eventDelays;                      // 47
    LLVMEventAssignmentHandler*             eventAssignments;                 // 48

    LLVMComputeEventAssignmentHandler*      computeEventAssignments;          // 49
    LLVMPerformEventAssignmentHandler*      performEventAssignments;          // 50

    /**
     * model name
     */
    char*                               modelName;                        // 51

    static void init(LLVMModelData&);

    static void clone(LLVMModelData *dst, LLVMModelData *src);

    static void allocBuffers(LLVMModelData &data, const std::string& modelName);

    static void copyBuffers(LLVMModelData *dst, LLVMModelData *src);

    static void freeBuffers(LLVMModelData&);
};

std::ostream& operator <<(std::ostream& os, const LLVMModelData& data);

}



#endif /* RRLLVMMODELDATA_H_ */
