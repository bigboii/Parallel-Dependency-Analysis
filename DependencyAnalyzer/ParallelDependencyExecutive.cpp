#include "ParallelDependencyExecutive.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ParallelDependencyExecutive.cpp - Collects all of the files matching one or more patterns in a directory tree rooted at  //
//                                  a specified path. On that collection, it conducts a dependency analysis usingthe       //
//                                  services of the ParallelDependencyAnalysis package. It then displays the results by    //
//                                  listing each of the files and their dependencies on two adjacent lines                 //
//                                                                                                                         //
//  ver 1.0                                                                                                                //
//  Application:   Project #3, Requirement #9                                                                              //
//  Author:        Young Kyu Kim, Syracuse University,                                                                     //
//                 (315) 870-8906, ykim127@syr.edu                                                                         //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Command Line Args
// ../Testfiles *.cpp *.h

//#define TEST_PDAEXEC
#ifdef TEST_PDAEXEC

using TypeMap = std::unordered_multimap<std::string, FileScope>;          //TypeName
int main(int argc, char* argv[])
{
	//1a. Check command line Arguments 
	std::cout << "\n argv[]s are: " << std::endl;
	if (argc < 2)
	{
		std::cout
			<< "\n  please enter name of file to process on command line\n\n";
		return 1;
	}

	//2. Setup Parallel Dependency Executive
	ParallelDependencyExecutive<TypeMap>* pde = new ParallelDependencyExecutive<TypeMap>(argv[1]);

	for (int i = 2; i < argc; ++i)           //patterns start from argv[2]
	{
		pde->getAnalyzer()->getFileMgr()->addPattern(argv[i]);
		//std::cout << inputs[i - 1] << std::endl;
	}

	//3. Perform Analysis
	pde->startParallelDependencyAnalysis();

	system("pause");
	return 1;

}
#endif