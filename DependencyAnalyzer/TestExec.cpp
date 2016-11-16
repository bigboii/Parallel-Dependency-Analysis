#include "ParallelDependencyExecutive.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  TestExec.cpp - Automated unit test suite that demonstrates you meet all the requirements of Project #3 .               //
//                 User must supply specified path and patterns through command line arguments or Compile.bat              //
//  ver 1.0                                                                                                                //
//  Application:   Project #3, Requirement #10                                                                              //
//  Author:        Young Kyu Kim, Syracuse University,                                                                     //
//                 (315) 443-3948, ykim127@syr.edu                                                                         //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using TypeMap = std::unordered_multimap<std::string, FileScope>;          //TypeName

//Demo Parallel Dependency Executive
void demoReq6789(std::string path)
{
	std::string msg = " Demonstrating Requirement #4, #5, #6, #7, #8, and #9";
	std::cout << "\n +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ " << std::endl;
	std::cout << " + " << msg << std::endl;
	std::cout << " +++++++++++++++++++++++++++++++++++++++++++++++++++++++++ " << std::endl;
	std::cout << " This demo will demonstrate all packages of Parallel Depenency Analysis simultaneously.\n";
	std::cout << " It uses and demos the following packages: \n";
	std::cout << " \t - ThreadPool (Requirement #4) \n";
	std::cout << " \t - Tasks (Requirement #5) \n";
	std::cout << " \t - TypeAnalyzer (Requirement #6) \n";
	std::cout << " \t - DependencyAnalyzer (Requirement #7) \n";
	std::cout << " \t - ParallelDependencyAnalysis (Requirement #8) \n";
	std::cout << " \t - ParallelDependencyExecutive (Requirement #9) \n";
	std::cout << "\n For this analysis, I will using 4 threads in my ThreadPool (Requirement #4) \n";
	std::cout << " My Parallel Dependency Analysis will be slit into 3 tasks: \n";
	std::cout << " \t - File Collection Task";
	std::cout << " \t - Pass 1 Task (Type Analysis) \n";
	std::cout << " \t - Pass 2 Task (Dependency Analysis) \n";
	ParallelDependencyExecutive<TypeMap>* pde = new ParallelDependencyExecutive<TypeMap>(path);

	//3. Perform Analysis
	pde->startParallelDependencyAnalysis();

}

#define TEST_EXEC
#ifdef TEST_EXEC
int main(int argc, char* argv[])
{
	demoReq6789(argv[1]);
	system("pause");
}
#endif