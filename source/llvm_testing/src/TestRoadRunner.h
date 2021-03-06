/*
 * TestRoadRunner.h
 *
 *  Created on: Jul 24, 2013
 *      Author: andy
 */

#ifndef TESTROADRUNNER_H_
#define TESTROADRUNNER_H_

#include "rrRoadRunner.h"
#include "rrTestSuiteModelSimulation.h"

namespace rr
{

class TestRoadRunner
{
public:
    TestRoadRunner(const std::string& version, int caseNumber);
    virtual ~TestRoadRunner();

    bool test(const std::string& compiler);

    RoadRunner *rr;

    TestSuiteModelSimulation *simulation;

    std::string version;
    int caseNumber;

    std::string modelFilePath;
    std::string modelFileName;
    std::string settingsFileName;
    std::string home;
    std::string dataOutputFolder;

    void loadSBML(const std::string& compiler);

    void simulate();

    void saveResult();

    void compareReference();


    static void test2();

    static void test3();
};

} /* namespace rr */
#endif /* TESTROADRUNNER_H_ */
