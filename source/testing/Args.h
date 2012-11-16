#ifndef CommandLineParametersH
#define CommandLineParametersH
#include <string>
using std::string;

string Usage(const string& prg);
class Args
{
    public:
                                        Args();
        virtual                        ~Args(){}
		string                          RRInstallFolder;     	                  	//option i:
        string                          SBMLModelsFilePath;                       	//option m:
		string                          CompilerLocation;                         	//option l:
        string                          ResultOutputFile;                         	//option r:
        string                          TempDataFolder;                           	//option t:
		string                          SupportCodeFolder;                        	//option s:
		bool                            EnableLogging;                    	     	//option v:
};

#endif