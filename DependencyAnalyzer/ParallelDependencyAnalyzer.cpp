#include "ParallelDependencyAnalyzer.h"
#include<iostream>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ParallelDependencyAnalyzer.cpp - Evaluates the dependency graph for all the packages in the specified file location.    //
//                                 For each file the analyses run asynchronously, using the facilities of the Task class. //
//                                 This will be done in 2 passes. The first pass will perform Type Analysis.              //
//                                 The second pass will perform Dependency Analysis.                                      //
//                                                                                                                        //
//  ver 1.0                                                                                                               //
//  Application:   Project #3, Requirement #8                                                                             //
//  Author:        Young Kyu Kim, Syracuse University,                                                                    //
//                 (315) 870-8906, ykim127@syr.edu                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#define TEST_PARALLEL
#ifdef TEST_PARALLEL

using TypeMap = std::unordered_multimap<std::string, FileScope>;          //TypeName
//Command Line Arguments: ../TestFiles2
int main(int argc, char* argv[])
{
	std::cout << "\n  main thread id = " << std::this_thread::get_id() << std::endl;

	//1. collecting tokens from files, named on the command line
	if (argc < 2)
	{
		std::cout
			<< "\n  please enter name of file to process on command line\n\n";
		return 1;
	}

	//2. Initialize Parallel Dependency Analyzer
	ParallelDependencyAnalyzer<TypeMap>* parallelDepAnalyzer = new ParallelDependencyAnalyzer<TypeMap>(argv[1]);

	parallelDepAnalyzer->startParallelDependencyAnalysis();

	//3. Execute
	

	//4. Done
	system("pause");
	return 1;


}

#endif