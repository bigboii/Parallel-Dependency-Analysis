#ifndef DEPANALYZER_H
#define DEPANALYZER_H

///////////////////////////////////////////////////////////////////////////////////
//  DependencyAnalyzer.h - Finds, for each file in a specified file collection,  // 
//                         all other files from the file collection on which     //
//                         they depend. File A depends on file B, if and only    //
//                         if, it uses the name of any type or global function   //
//                         defined in file B. It might do that by calling a      //
//                         function or method of a type or by inheriting the     //
//                         type. Note that this intentionally does not record    //
//                         dependencies of a file on files outside the file set, //
//                         e.g., language and platform libraries.                //
//  ver 1.0                                                                      //
//  Application:   Project #3, Requirement #7                                    //
//  Author:        Young Kyu Kim, Syracuse University,                           //
//                 (315) 870-8906, ykim127@syr.edu                               //
///////////////////////////////////////////////////////////////////////////////////
#include<unordered_map>
#include<string>
#include"../Parser/ConfigureParser.h"

using TypeMap = std::unordered_multimap<std::string, FileScope>;          //TypeName
//using DepMap = std::unordered_map<std::string, std::string>;

class DependencyAnalyzer
{
public:
	DependencyAnalyzer(TypeMap* map);      //Constructor

	TypeMap startDependencyAnalysis(int argc, const char* argv, std::unordered_multimap<std::string, FileScope>* table);
	void mergeDepTables(BlockingQueue<TypeMap>* resultQueue);
	void displayDepTable();
	TypeMap* getDepTable();

private:
	//ConfigParseToConsole* configParser;       //shared configParser with TypeAnalyzer
	std::vector<std::string> vec_filenames;
	TypeMap* typeTable;        //Reference to MergedTable from TypeAnalyzer
	TypeMap* mergedDepTable;    //Dependency Table of files
};
#endif