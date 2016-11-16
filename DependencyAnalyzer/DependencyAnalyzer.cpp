#include "DependencyAnalyzer.h"
#include "../Utilities/Utilities.h"
#include "../Parser/ConfigureParser.h"
#include "TypeAnalyzer.h"
#include <iostream>

///////////////////////////////////////////////////////////////////////////////////
//  DependencyAnalyzer.cpp - Finds, for each file in a specified file collection,  // 
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
//                 (315) 487-8907, ykim127@syr.edu                               //
///////////////////////////////////////////////////////////////////////////////////

using namespace Scanner;
using namespace Utilities;
#define Util StringHelper
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"

//Constructor
DependencyAnalyzer::DependencyAnalyzer(TypeMap* map)
{
	typeTable = map;
	mergedDepTable = new TypeMap();
}

//Performs Dependency Analysis and stores itinto table,
TypeMap DependencyAnalyzer::startDependencyAnalysis(int argc, const char* argv, std::unordered_multimap<std::string, FileScope>* table)
{
	ConfigParseToConsole configParser;
	std::string fileSpec = FileSystem::Path::getFullFileSpec(argv);
	std::string msg = "Processing file " + fileSpec;
	//Util::title(msg);
	putline();
	Parser* pParser = configParser.BuildDependency(table);       //configure initializes parser with Toker, SemiExpression, and adds Rules and Actions
	pParser->updateFileName(argv);         //Proj 3, update File Info
	configParser.getRepository()->setFileName(argv);
	TypeMap outputMap;
	//try
	//{
		if (pParser)
		{
			if (!configParser.Attach(argv))
			{
				std::cout << "\n  could not open file " << argv << std::endl;
				return outputMap;
			}
		}
		else
		{
			std::cout << "\n\n  Parser not built\n\n";
			return outputMap;
		}
		// now that parser is built, use it
		while (pParser->next())                  //1. get One SemiExpression
		{
			pParser->parse();                    //2. Parse SemiExpression, returns true?
			//configParser.getVectorNamespace()->clear();//Reset list_namespace for next iteration
		}
		/*std::cout << "\n<--- Contents of Dependency Table at \"" << argv << "\" --->" << std::endl;
		for (auto it = configParser.getRepository()->getDepTable()->begin(); it != configParser.getRepository()->getDepTable()->end(); ++it)
		{
			std::cout << " " << it->first << " depends on " << it->second.filename << std::endl;
		}*/

		//configParser->getRepository()->getResultQueue2()->enQ(*configParser->getRepository()->getDepTable());   //4. Store table into ResultsQueue
		outputMap = *configParser.getRepository()->getDepTable();
		configParser.getRepository()->resetDepAnalysis();                                                 //5. Reset Type Analysis data for next iteration
		std::cout << "\n";
	//}
	//catch (std::exception& ex)
	//{
	//	std::cout << "\n\n    " << ex.what() << "\n\n";
	//}
	return outputMap;
}

//Merge Dep Tables
void DependencyAnalyzer::mergeDepTables(BlockingQueue<TypeMap>* resultQueue)
{
	TypeMap map;

	//Initial Loop based on keys
	while (resultQueue->size() > 0)     //while there is always at least 2 maps in Result Queue...
	{
		map = resultQueue->deQ();       //get 1 table from Result Queue

		if (mergedDepTable->size() > 0)      //if this isn't first iteration
		{
			for (auto it = map.begin(); it != map.end(); ++it)
			{
				bool keyFound = false;
				for (auto it2 = mergedDepTable->begin(); it2 != mergedDepTable->end(); ++it2)
				{
					if (it->first == it2->first)                                                  //1. Check duplicate
					{
						keyFound = true;
						
						if (it->second.filename != it2->second.filename)   //2. check Key then filename
						{
						keyFound = false;
						}
						
					}
				}
				if (!keyFound)                               //if duplicate key not found
				{
					std::pair<std::string, FileScope> depPair(it->first, it->second);   //3. Create Map Pair and insert into typetable
					mergedDepTable->insert(depPair);
				}
			}
		}
		else         //if mergedTypeTable is initially empty
		{
			mergedDepTable->insert(map.begin(), map.end());
		}
	}
}

//Print merged Dependency Table
void DependencyAnalyzer::displayDepTable()
{
	std::cout << "\n <--- Contents of merged Dependency Table: --->" << std::endl;

	for (auto it = mergedDepTable->begin(); it != mergedDepTable->end(); ++it)
	{
		std::cout << " " << it->first << " depends on " << it->second.filename << std::endl;
	}
	
}

//Returns reference to DependencyTable
TypeMap* DependencyAnalyzer::getDepTable()
{
	return mergedDepTable;
}


//----< test stub >--------------------------------------------
//#define TEST_DEPANALYZER
#ifdef TEST_DEPANALYZER

#include <queue>
#include <string>
#define Util StringHelper

int main(int argc, char* argv[])
{
	Util::Title("Testing Parser Class", '=');
	putline();

	// collecting tokens from files, named on the command line
	if (argc < 2)
	{
		std::cout
			<< "\n  please enter name of file to process on command line\n\n";
		return 1;
	}

	ConfigParseToConsole configParser;
	//1st Pass: Type Analysis
	std::cout << "<<< TYPE ANALYSIS >>>" << std::endl;
	TypeAnalyzer* typeAnalyzer = new TypeAnalyzer(&configParser);
	typeAnalyzer->startTypeAnalysis(argc, argv);

	//2nd Pass: Dependency Analysis
	std::cout << "<<< DEPENDENCY ANALYSIS >>>" << std::endl;
	DependencyAnalyzer* depAnalyzer = new DependencyAnalyzer(&configParser, &typeAnalyzer->getMergedTypeTable());
	depAnalyzer->startDependencyAnalysis(argc, argv, &typeAnalyzer->getMergedTypeTable());
	//depAnalyzer->mergeDepTables();
	//depAnalyzer->displayDepTable();

	std::cout << "\n";
	system("pause");           //needed for some reason
}

#endif
