#ifndef TEST_PARALLELANALYZER
#define TEST_PARALLELANALYZER

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ParallelDependencyAnalyzer.h - Evaluates the dependency graph for all the packages in the specified file location.    //
//                                 For each file the analyses run asynchronously, using the facilities of the Task class. //
//                                 This will be done in 2 passes. The first pass will perform Type Analysis.              //
//                                 The second pass will perform Dependency Analysis.                                      //
//                                                                                                                        //
//  ver 1.0                                                                                                               //
//  Application:   Project #3, Requirement #8                                                                             //
//  Author:        Young Kyu Kim, Syracuse University,                                                                    //
//                 (315) 870-8906, ykim127@syr.edu                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../FileMgr/FileMgr.h"
#include "Task.h"
#include "TypeAnalyzer.h"
#include "DependencyAnalyzer.h"

using TypeMap = std::unordered_multimap<std::string, FileScope>;          //TypeName"

//Sample Commandline Argument:
//   C:/Users/YQ/Desktop/CSE687_OOD/Project2HelperCode/TestFiles *.h *.cpp
template<typename ResultType>
class ParallelDependencyAnalyzer
{
public:
	ParallelDependencyAnalyzer(std::string path);
	Task<ResultType>* getTask();

	void startParallelDependencyAnalysis();
	void startFileCollection();
	void startPass1();
	void startPass2(TypeMap map);


	FileMgr* getFileMgr();

private:
	TypeAnalyzer* typeAnalyzer;
	DependencyAnalyzer* depAnalyzer;
	Task<ResultType>* task;
	FileMgr* fm;                        //finds files and directories at a specified path
	DataStore* ds;
	std::string specifiedPath;                //specified path
	TypeMap fileMap;
};

//ThreadPool<ResultType> Task<ResultType>::globalTP;      //Initialize static variable from Task.h

template<typename ResultType>
ThreadPool<ResultType> Task<ResultType>::globalTP;      //Initialize static variable from Task.h

//Constructor
template<typename ResultType>
ParallelDependencyAnalyzer<ResultType>::ParallelDependencyAnalyzer(std::string path)
{
	specifiedPath = path;
	typeAnalyzer = new TypeAnalyzer();
	depAnalyzer = new DependencyAnalyzer(typeAnalyzer->getMergedTypeTable());
	task = new Task<TypeMap>();                 //no constructors
	ds = new DataStore();
	fm = new FileMgr(specifiedPath, *ds);           //no default construtors
}

//Return reference to Task
template<typename ResultType>
Task<ResultType>* ParallelDependencyAnalyzer<ResultType>::getTask()
{
	return task;
}

//Start Entire Parallel Dependency Analysis
template<typename ResultType>
void ParallelDependencyAnalyzer<ResultType>::startParallelDependencyAnalysis()
{
	std::cout << "\n ////////////////////////////////////////////" << std::endl;
	std::cout << " //   Starting Parallel Dependency Analysis" << std::endl;
	std::cout << " //   Main Thread ID [T: " << std::this_thread::get_id() <<"]" << std::endl;
	std::cout << " /////////////////////////////////////////////" << std::endl;

	Task<TypeMap>::getTP()->setThreadCount(4);

	std::cout << "   # of THREADS: " << Task<TypeMap>::getTP()->getThreadCount() << std::endl;
	//1. Start ThreadPool
	Task<TypeMap>::getTP()->start();                 //	Queue WorkItems into static ThreadPool

	//2. Perform Analysis
	startFileCollection();
}

//Start file collection; called by startParallelDependencyAnalyzer()
template<typename ResultType>
void ParallelDependencyAnalyzer<ResultType>::startFileCollection()
{
	std::function<TypeMap()> fileTask = [&]()
	{
		std::cout << "[" << std::this_thread::get_id() << "] Performing File Collection" << std::endl;
		std::cout << "----------------------------------------------\n";
		fm->addPattern("*.cpp");
		fm->addPattern("*.h");
		fm->search();

		TypeMap filemap;
		for (auto fs : *ds)
		{
			FileScope someScope;                                         //empty...
			std::pair<std::string, FileScope> typePair(fs, someScope);   //3. Create Map Pair and insert into typetable
			filemap.insert(typePair);
		}

		std::cout << "\n\n  contents of files in " << specifiedPath;          //Verify fileMap
		std::cout << "\n -----------------------\n";
		for (auto it = filemap.begin(); it != filemap.end(); ++it)
			std::cout << "\t " << it->first << std::endl;

		std::cout << "\n\n";
		return filemap;
	};

	//task->setLamda(&fileTask);
	Task<TypeMap>::getTP()->doWork(&fileTask);

	startPass1();
}

//Start Type Analysis; called by startParallelDependencyAnalyzer()
template<typename ResultType>
void ParallelDependencyAnalyzer<ResultType>::startPass1()
{
	BlockingQueue<ResultType>* resultQT = Task<TypeMap>::getTP()->getResultQueue();//DEBUGGING
	std::cout << "\n [Main Thread: " << std::this_thread::get_id() << "] DeQing fileMap from ResultQueue" << std::endl;   //Getinput from result Queue
	fileMap = Task<TypeMap>::getTP()->result();            //get fileMap Result
	for (auto it = fileMap.begin(); it != fileMap.end(); ++it)
	{
		std::string* s = new std::string();
		*s = "";
		*s += specifiedPath;
		*s += "/";
		*s += it->first;
		std::function<TypeMap()>* TypeAnalysisTask = new std::function<TypeMap()>();        		//Setup Lambda and execute it
		*TypeAnalysisTask = [=]()           //create lamda statement like the following
		{                                                           //Capture variables in main
			std::cout << "\n [T: " << std::this_thread::get_id() << "] Starting Type Analaysis" << std::endl;
			return typeAnalyzer->startTypeAnalysis(1, (*s).c_str());			//Perform TypeAnalysis
		};
		std::cout << "\n [Main Thread: " << std::this_thread::get_id() << "] ENQing \""<< (*s).c_str() <<"\" Type Analysis Lamda into Tasks" << std::endl;
		Task<TypeMap>::getTP()->doWork(TypeAnalysisTask);
	}
	std::function<TypeMap()> finishTypeAnalysisTask = []()                      //Send message to ThreadPool that notify that Type Analysis is done
	{
		std::cout << "\n [T: " << std::this_thread::get_id() << "] Sending Done signal on Type Analysis" << std::endl;
		TypeMap finishMap;
		FileScope scope;
		std::pair<std::string, FileScope>pair("DONE", scope);
		finishMap.insert(pair);
		return finishMap;     //return Null Table
	};
	std::cout << "\n [Main Thread: " << std::this_thread::get_id() << "] ENQing FINISH Type Analysis Lamda into Tasks" << std::endl;
	Task<TypeMap>::getTP()->doWork(&finishTypeAnalysisTask);

	std::cout << "\n [T: " << std::this_thread::get_id() << "] Merging Type Tables" << std::endl;
	//return *typeAnalyzer->getMergedTypeTable();

	while (Task<TypeMap>::getTP()->getCountResults() != fileMap.size() + 1 && Task<TypeMap>::getTP()->getIsDone() != true)  	//Check if All TypeTables have been enQ'd
	{
			//break;
	}
	std::cout << "\n [Main Thread: " << std::this_thread::get_id() << "] ENQing Merge Type Table Lamda into Tasks" << std::endl;
	//Task<TypeMap>::getTP()->doWork(&MergeTypeTableTask);
	typeAnalyzer->mergeTables(Task<TypeMap>::getTP()->getResultQueue());                //Or here??
	typeAnalyzer->printMergedTable();
	startPass2(*typeAnalyzer->getMergedTypeTable());
}

// Start Type Analysis; called by startParallelDependencyAnalyzer()
template<typename ResultType>
void ParallelDependencyAnalyzer<ResultType>::startPass2(TypeMap map)
{
	BlockingQueue<ResultType>* resultQ = Task<TypeMap>::getTP()->getResultQueue();//DEBUGGING
	Task<TypeMap>::getTP()->resetCountAndDone();                  	//Reset Threadpool's countResults and isDone
	for (auto it = fileMap.begin(); it != fileMap.end(); ++it)     	//4. Setup Task for Dependency Analysis
	{
		std::string* s = new std::string();
		*s = "";
		*s += specifiedPath;
		*s += "/";
		*s += it->first;
		std::function<TypeMap()>* DepAnalysisTask = new std::function<TypeMap()>();
		*DepAnalysisTask = [=]()
		{
			std::cout << "\n [T: " << std::this_thread::get_id() << "] Starting Dependency Analaysis" << std::endl;
			return depAnalyzer->startDependencyAnalysis(1, (*s).c_str(), typeAnalyzer->getMergedTypeTable());     //Deq Item from result queue
		};
		std::cout << "\n [Main Thread: " << std::this_thread::get_id() << "] ENQing \"" << (*s).c_str() << "\" Dependency Analysis Lamda into Tasks" << std::endl;
		Task<TypeMap>::getTP()->doWork(DepAnalysisTask);
	}
	std::function<TypeMap()> finishDepAnalysisTask = []()                      //Send message to ThreadPool that Type Analysis is done
	{
		std::cout << "\n [T: " << std::this_thread::get_id() << "] Sending Done signal on Type Analysis" << std::endl;
		TypeMap finishMap;
		FileScope scope;
		std::pair<std::string, FileScope>pair("DONE", scope);
		finishMap.insert(pair);
		return finishMap;     //return Null Table
	};
	std::cout << "\n [Main Thread: " << std::this_thread::get_id() << "] ENQing FINISH Dependency Analysis Lamda into Tasks" << std::endl;
	Task<TypeMap>::getTP()->doWork(&finishDepAnalysisTask);                                    //Send done signal to Thread Pool?
	while (Task<TypeMap>::getTP()->getCountResults() != fileMap.size() + 1 && Task<TypeMap>::getTP()->getIsDone() != true)        //Merge Dependency Table
	{
	}
	std::cout << "\n [Main Thread: " << std::this_thread::get_id() << "] ENQing Merge Dependency Table Lamda into Tasks" << std::endl;
	std::cout << "\n [" << std::this_thread::get_id() << "] Merging Dependency Tables" << std::endl;
	depAnalyzer->mergeDepTables(Task<TypeMap>::getTP()->getResultQueue());                //Or here??
	depAnalyzer->displayDepTable();
	//return *depAnalyzer->getDepTable();
	std::this_thread::sleep_for(std::chrono::milliseconds(100));        //100ms; sleep a lil bit so I can print merged dependency table
	for (int i = 0; i < Task<TypeMap>::getTP()->getThreadCount(); i++)   	//3. Signal to shutdown threads and Join all threads from thread pool
		Task<TypeMap>::getTP()->doWork(nullptr);                      //shutdown thread
	Task<TypeMap>::getTP()->wait();
}

template<typename ResultType>
FileMgr* ParallelDependencyAnalyzer<ResultType>::getFileMgr()
{
	return fm;
}


//

#endif