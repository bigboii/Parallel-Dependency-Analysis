#include "ThreadPool.h"

/////////////////////////////////////////////////////////////////////////////
//  ThreadPool.cpp - Provides facilities to enqueue work items and execute //
//                 them asynchronously each work item will be a callable   //
//                 object that is supplied by application code             //
//  ver 1.1                                                                //
//  Application:   Project #2, Requirement #4                              //
//  Author:        Young Kyu Kim, Syracuse University,                     //
//                 (315) 870-8906, ykim127@syr.edu                         //
//  Source:        Jim Fawcett, CST 4-187, Syracuse University             //
//                 (315) 443-3948, jfawcett@twcny.rr.com                   //
/////////////////////////////////////////////////////////////////////////////

//#define TEST_TP
#ifdef TEST_TP

//----< test function for asyncRun >-------------------------------------

using Util = Utilities::StringHelper;
int main()
{
	std::cout << "\n  main thread id = " << std::this_thread::get_id() << std::endl;
	ThreadPool<std::string> threadPool;  //std::function<void()> =>  void func()
	threadPool.setThreadCount(4);        //Use single thread
	threadPool.start();
	//1. Queue 2 work items into ThreadPool
	std::function<std::string()> lamda = []() 
	{
		std::cout << "\n Thread " << std::this_thread::get_id() << " working on lamda1";
		return "I'm a Lamda"; 
	};
	threadPool.doWork(&lamda);       //place WorkItem1(thread) into ThreadPool's BlockingQueue
	std::function<std::string()> lamda2 = []() 
	{
		std::cout << "\n Thread " << std::this_thread::get_id() << " working on lamda2";
		return "I'm another Lamda";
	};
	threadPool.doWork(&lamda2);       //place WorkItem1(thread) into ThreadPool's BlockingQueue
	std::function<std::string()> lamda3 = []()
	{
		std::cout << "\n Thread " << std::this_thread::get_id() << " working on lamda3";
		return "I'm some different Lamda";
	};
	threadPool.doWork(&lamda3);       //place WorkItem1(thread) into ThreadPool's BlockingQueue
	std::function<std::string()> lamda4 = []()
	{
		std::cout << "\n Thread " << std::this_thread::get_id() << " working on lamda4";
		return "I'm am the last Lamda";
	};
	threadPool.doWork(&lamda4);       //place WorkItem1(thread) into ThreadPool's BlockingQueue
	//2. the following calls to result() block until results are enqueued
	std::cout << "Execute Queued Work Items" << std::endl;
	std::cout << "\n  " << threadPool.result();
	std::cout << "\n  " << threadPool.result();
	std::cout << "\n  " << threadPool.result();
	std::cout << "\n  " << threadPool.result();
	threadPool.doWork(nullptr);                      //shutdown thread
	threadPool.doWork(nullptr);                      //shutdown thread
	threadPool.doWork(nullptr);                      //shutdown thread
	threadPool.doWork(nullptr);                      //shutdown thread
	//3. wait for child thread to complete
	threadPool.wait();                           //????????? ISSUES with more than 1 thread
	std::cout << "\n Main Thread "<< std::this_thread::get_id() << " Done!";
	std::cout << "\n\n";
	system("pause");
}

#endif