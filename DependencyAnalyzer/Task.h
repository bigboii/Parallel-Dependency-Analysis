#ifndef TASK_H
#define TASK_H

//////////////////////////////////////////////////////////////////////////////
//  Task.h - Provides a thread wrapper class around ThreadPool (which holds // 
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

#include "ThreadPool.h"

template<typename ResultType>      
class Task
{
public:

	Task() 
	{
		typeThreadPool = new ThreadPool<ResultType>();            //Don't make on stack, use heap
		depThreadPool = new ThreadPool<ResultType>();
	}

	void setLamda(std::function<ResultType()>* lamda);             //perform type analysis

	static ThreadPool<ResultType>* getTP()
	{
		return &globalTP;
	}

private:
	ThreadPool<ResultType> *typeThreadPool; //= new ThreadPool<ResultType>();            //Don't make on stack, use heap
	ThreadPool<ResultType> *depThreadPool;// = new ThreadPool<ResultType2>();
	static ThreadPool<ResultType> globalTP;
};

//Get Calleable object from client code
template<typename ResultType>
void Task<ResultType>::setLamda(std::function<ResultType()>* lamda)
{
	//1. Queue WorkItems into
	Task<ResultType>::globalTP.doWork(lamda);
}


#endif