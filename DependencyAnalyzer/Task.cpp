#include "Task.h"
#include "TypeAnalyzer.h"
#include "DependencyAnalyzer.h"
#include "../FileMgr/FileMgr.h"
#include "../DataStore/DataStore.h"

//////////////////////////////////////////////////////////////////////////////
//  Task.cpp - Provides a thread wrapper class around ThreadPool (which holds // 
//                 work item and result queue). The Task and ThreadPool     // 
//                 work together to make the system work in parallel.       //
//                 The Client Code, DependencyAnalyzer, will pass the       //
//                 calleable object into the Task. Then the Task will       //
//                 Execute them in parallel using the services from         //
//                 ThreadPool package.                                      //
//  ver 1.0                                                                 //
//  Application:   Project #2, Requirement #5                               //
//  Author:        Young Kyu Kim, Syracuse University,                      //
//                 (315) 870-8906, ykim127@syr.edu                          //
//  Source:        Jim Fawcett, CST 4-187, Syracuse University              //
//                 (315) 443-3948, jfawcett@twcny.rr.com                    //
//////////////////////////////////////////////////////////////////////////////

/*
	Test Input:
		Namespace1.h X.h X.cpp Y.h Y.cpp Global.h Global.cpp
*/

//#define TEST_TASK
#ifdef TEST_TASK

template<typename ResultType>
ThreadPool<ResultType> Task<ResultType>::globalTP;      //Initialize static variable from Task.h

using TypeMap = std::unordered_multimap<std::string, FileScope>;          //TypeName

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

	//Create Instance of Task
	Task<TypeMap> *task = new Task<TypeMap>();


	//4. Done
	system("pause");
	return 1;
}

#endif