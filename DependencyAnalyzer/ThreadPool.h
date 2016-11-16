#ifndef THREADPOOL_H
#define THREADPOOL_H
///////////////////////////////////////////////////////////////////////////
//  ThreadPool.h - Provides facilities to enqueue work items and execute //
//                 them asynchronously each work item will be a callable //
//                 object that is supplied by application code           //
//  ver 1.2                                                              //
//  Application:   Project #2, Requirement #4                            //
//  Author:        Young Kyu Kim, Syracuse University,                   //
//                 (315) 870-8906, ykim127@syr.edu                       //
//  Source:        Jim Fawcett, CST 4-187, Syracuse University           //
//                 (315) 443-3948, jfawcett@twcny.rr.com                 //
///////////////////////////////////////////////////////////////////////////
/*
 Maintenance History:
 ====================
 ver 2.2
	- updated to support parallel dependency analysis
 ver 1.1
	- Origial from CPP Threading Techniques
*/
//** NOTE **
// Parser is not "Thread Safe"! Each thread should have their own Parser

#include<vector>
#include<string>
#include<thread>
#include <functional>
#include "Cpp11-BlockingQueue.h"

template<typename Result>
using WorkItem = std::function<Result()>; //alias
//Workitem class contains functor

// -- ThreadPool Class
// Result: function return type for WorkItem
// CallObj: 1st Callable Object to run
template<typename Result>
class ThreadPool
{
public:
	//Blocking Queue features
	void start();
	void doWork(WorkItem<Result>* pWi);             //Accept any function type
	Result result();
	void wait();
	
	void setThreadCount(int n);
	int getThreadCount();

	//Concurrency Functions
	void resetCountResults();
	void resetIsDone();
	void setFileCount(int n);
	bool getIsDone();
	void resetCountAndDone();

	BlockingQueue<Result>* getResultQueue();

	//void 
	int getCountResults();

	//Other stuff
	~ThreadPool();
private:
	int threadCount = 1;                           //user defined # threads
	int fileCount;                                 //# of files to be analyzed; will be supplied by Client Code
	int countResults = 0;                              //Counts total # of results finished by threads; used for CONCURRENCY
	bool isDone = false;                           //TRUE if countResults == threadCount, FALSE if countResults != threadCount; used for CONCURRENCY
	//std::thread* _pThread;
	BlockingQueue<WorkItem<Result>*> _workItemQueue;
	BlockingQueue<Result> _resultsQueue;
	std::vector<std::thread*> vec_thread;    //Vector of threads max 8
};


#include "../Utilities/Utilities.h"

//----< wait for child thread to terminate >---------------------------
template<typename Result>
void ThreadPool<Result>::wait()
{
	/*
	for (int i = 0; i < threadCount; i++)
	{
	vec_thread[i]->join();
	}
	*/

	std::for_each(vec_thread.begin(), vec_thread.end(), [](std::thread* t)
	{
		t->join();
	});
}
//----< enqueue work item >--------------------------------------------

template<typename Result>
void ThreadPool<Result>::doWork(WorkItem<Result>* pWi)                           //Accepts any function object
{
	_workItemQueue.enQ(pWi);
}
//----< retrieve results with blocking call >--------------------------

template<typename Result>
Result ThreadPool<Result>::result()
{
	return _resultsQueue.deQ();
}
//----< start child thread that dequeus work items >-------------------

template<typename Result>
void ThreadPool<Result>::start()
{
	std::cout << "\n [ThreadPool: " << std::this_thread::get_id() << "] ThreadPool.start()" << std::endl;

	std::function<void()> threadProc =
		[&]() {
		while (true)
		{
			WorkItem<Result>* pWi = _workItemQueue.deQ();
			if (pWi == nullptr)
			{
				std::cout << "\n [T: " << std::this_thread::get_id() << "] shutting down work item processing";
				return;
			}
			Result result = (*pWi)();               //Get result

			if (result.find("DONE") != result.end())                          //Check if final map contains Done signal
			{
				std::cout << "\n  *-----------------------------------------*";
				std::cout << "\n  [ThreadPool] Finished Enqueueing Tables";
				std::cout << "\n  *-----------------------------------------*\n";
				isDone = true;
			}
			else
			{
				_resultsQueue.enQ(result);
				countResults++;                       //YKK: increment result count by 1
			}
		}
	};
	//single threaded version
	//_pThread = new std::thread(threadProc);

	//http://www.bogotobogo.com/cplusplus/C11/3_C11_Threading_Lambda_Functions.php
	//Multithreaded Version

	std::cout << "ThreadPool.start(), threadCount: " << threadCount << std::endl;

	for (int i = 0; i < threadCount; i++)
	{
		vec_thread.push_back(new std::thread(threadProc));
		std::cout << "Executing Thread(" << i << ") [T: "<< vec_thread[i]->get_id() << "] " << std::endl;
		//vec_thread[i]->join();                             //???????????
	}
}

//Set number of threads to run asynchronously
template<typename Result>
void ThreadPool<Result>::setThreadCount(int n)
{
	threadCount = n;
	//vec_thread.reserve(n);
}

template<typename Result>
int ThreadPool<Result>::getCountResults()
{
	return countResults;
}

//Return thread count
template<typename Result>
int ThreadPool<Result>::getThreadCount()
{
	return threadCount;
}

//Reset countResults for next pass/analysis
template<typename Result>
void ThreadPool<Result>::resetCountResults()
{
	countResults = 0;
}

//Reset isDone for next pass/analysis
template<typename Result>
void ThreadPool<Result>::resetIsDone()
{
	isDone = false;
}

//set file count
template<typename Result>
void ThreadPool<Result>::setFileCount(int n)
{
	fileCount = n;
}

//return isDone;
template<typename Result>
bool ThreadPool<Result>::getIsDone()
{
	return isDone;
}

//Reset isDone = false and countResult = 0 for next pass
template<typename Result>
void ThreadPool<Result>::resetCountAndDone()
{
	countResults = 0;
	isDone = false;
}

template<typename Result>
BlockingQueue<Result>* ThreadPool<Result>::getResultQueue()
{
	return &_resultsQueue;
}

//----< clean up heap >------------------------------------------------
template<typename Result>
ThreadPool<Result>::~ThreadPool()
{
	for (int i = 0; i < threadCount; i++)
	{
		vec_thread.pop_back();
	}
}

#endif