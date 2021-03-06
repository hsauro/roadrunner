/*
 * TestBase.cpp
 *
 *  Created on: Jul 20, 2013
 *      Author: andy
 */

#include "TestBase.h"
#include "tests.h"
#include "rrModelGeneratorFactory.h"
#include "rrUtils.h"
#include "rrLogger.h"

#include <stdexcept>
#include <fstream>

namespace rr
{
using namespace std;

TestBase::TestBase(const std::string& compiler, const std::string& version, int caseNumber) :
    model(0),
    version(version),
    caseNumber(caseNumber),
    compiler(compiler)
{
    fileName = getModelFileName(version, caseNumber);

    if(!fileExists(fileName))
    {
        stringstream msg;
        msg<<"File: "<<fileName<<" does not exist";
        Log(lError)<<msg.str();

        throw std::runtime_error(msg.str());
    }

    ifstream ifs(fileName.c_str());
    if(!ifs)
    {
        stringstream msg;
        msg<<"Failed opening file: "<<fileName;
        Log(lError)<<msg.str();

        throw std::runtime_error(msg.str());
    }

    std::string sbml((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

    ifs.close();
    Log(Logger::PRIO_INFORMATION)
        << "/*** SBML " << fileName << " ***/" << endl
        << sbml << endl
        << "/****************************************************************************************/"
        << endl;


    string home = getenv("HOME");
    string tmp = home + string("/tmp");
    string support = home + "/local/rr_support/";
    ModelGenerator *generator = ModelGeneratorFactory::createModelGenerator(compiler, tmp, support);

    model = generator->createModel(sbml, 0);

    delete generator;
}

TestBase::~TestBase()
{
    Log(lInfo) << "deleting model, " << __FUNC__;
    delete model;
}

} /* namespace rr */
