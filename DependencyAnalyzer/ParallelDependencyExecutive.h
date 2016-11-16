#ifndef PARALLEL_EXEC_H
#define PARALLEL_EXEC_H

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ParallelDependencyExecutive.h - Collects all of the files matching one or more patterns in a directory tree rooted at  //
//                                  a specified path. On that collection, it conducts a dependency analysis usingthe       //
//                                  services of the ParallelDependencyAnalysis package. It then displays the results by    //
//                                  listing each of the files and their dependencies on two adjacent lines                 //
//                                                                                                                         //
//  ver 1.0                                                                                                                //
//  Application:   Project #3, Requirement #9                                                                              //
//  Author:        Young Kyu Kim, Syracuse University,                                                                     //
//                 (315) 870-8906, ykim127@syr.edu                                                                         //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ParallelDependencyAnalyzer.h"

template<typename ResultType>
class ParallelDependencyExecutive
{
public:
	ParallelDependencyExecutive(std::string path);
	void startParallelDependencyAnalysis();
	ParallelDependencyAnalyzer<ResultType>* getAnalyzer();
private:
	ParallelDependencyAnalyzer<ResultType>* pdAnalyzer;
	std::string path;
	std::vector<std::string> patterns;
};

//Constructor
template<typename ResultType>
ParallelDependencyExecutive<ResultType>::ParallelDependencyExecutive(std::string path)
{
	pdAnalyzer = new ParallelDependencyAnalyzer<ResultType>(path);
}

template<typename ResultType>
void ParallelDependencyExecutive<ResultType>::startParallelDependencyAnalysis()
{
	pdAnalyzer->startParallelDependencyAnalysis();
}

//Returns reference to ResultType
template<typename ResultType>
ParallelDependencyAnalyzer<ResultType>* ParallelDependencyExecutive<ResultType>::getAnalyzer()
{
	return pdAnalyzer;
}
#endif