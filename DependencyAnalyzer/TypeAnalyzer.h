#ifndef TYPEANALYZER_H
#define TYPEANALYZER_H

///////////////////////////////////////////////////////////////////////////
//  TypeAnalyzer.h - Finds all the types defined in each of a collection //
//                 of C++ source files. It does this by building rules   //
//                 to detect type definitions: classes, structs, enums,  //
//                 typedefs, and aliases, and capture their fully        //
//                 qualified names and files where they are defined.     //
//  ver 1.0                                                              //
//  Application:   Project #3, Requirement #6                            //
//  Author:        Young Kyu Kim, Syracuse University,                   //
//                 (315) 870-8906, ykim127@syr.edu                       //
///////////////////////////////////////////////////////////////////////////


#include "../Parser/ConfigureParser.h"
#include "FileNameScope.h"

using TypeMap = std::unordered_multimap<std::string, FileScope>;          //TypeName
class TypeAnalyzer
{
public:
	TypeAnalyzer();

	TypeMap startTypeAnalysis(int argc, const char* argv/*, TypeMap* typeTable*/);
	void mergeTables(BlockingQueue<TypeMap>* resultQueue);
	void printMergedTable();
	TypeMap* getMergedTypeTable();

private:
	TypeMap* mergedTypeTable;

};
#endif