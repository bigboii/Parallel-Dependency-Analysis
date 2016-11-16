#ifndef FILE_NAME_SCOPE      //To Prevent Double inclusion
#define FILE_NAME_SCOPE

///////////////////////////////////////////////////////////////////////////////////
//  FileNameScope.h - Acts as a container for file information when performing   // 
//                    Parallel Dependency Analysi.                               //
//                    Stores namespace, filename, type, and line number          //
//  ver 1.0                                                                      //
//  Application:   Project #3, Requirement #7                                    //
//  Author:        Young Kyu Kim, Syracuse University,                           //
//                 (315) 870-8906, ykim127@syr.edu                               //
///////////////////////////////////////////////////////////////////////////////////
#include<string>
struct FileScope
{
public:
	std::string nameSpace;
	std::string filename;
	std::string type;
	std::string linenum;

private:
};
#endif