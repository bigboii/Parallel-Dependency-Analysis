#include "TypeAnalyzer.h"
#include "../Utilities/Utilities.h"
#include "../Parser/ConfigureParser.h"
#include <iostream>

///////////////////////////////////////////////////////////////////////////
//  TypeAnalyzer.cpp - Finds all the types defined in each of a collection //
//                 of C++ source files. It does this by building rules   //
//                 to detect type definitions: classes, structs, enums,  //
//                 typedefs, and aliases, and capture their fully        //
//                 qualified names and files where they are defined.     //
//  ver 1.0                                                              //
//  Application:   Project #3, Requirement #6                            //
//  Author:        Young Kyu Kim, Syracuse University,                   //
//                 (315) 870-8906, ykim127@syr.edu                       //
///////////////////////////////////////////////////////////////////////////

using namespace Scanner;
using namespace Utilities;
#define Util StringHelper
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"

//Constructor
TypeAnalyzer::TypeAnalyzer()
{
	mergedTypeTable = new TypeMap();
	//this->argc = argc;
	//this->argv = argv;
}

/*
//return reference to parser
Parser* TypeAnalyzer::getParser() 
{ 
	return configParser->getParser(); 
};

//return reference to repository
Repository* TypeAnalyzer::getRepository()
{
	return configParser->getRepository();
}
*/

//Starts Type Analysis
TypeMap TypeAnalyzer::startTypeAnalysis(int argc, const char* argv/*, TypeMap* typeTable*/)
{
	ConfigParseToConsole configParser;
	std::string fileSpec = FileSystem::Path::getFullFileSpec(argv);
	std::string msg = "Processing file " + fileSpec;
	//Util::title(msg);
	putline();
	Parser* pParser = configParser.Build();       //configure initializes parser with Toker, SemiExpression, and adds Rules and Actions
	pParser->updateFileName(argv);         //Proj 3, update File Info
	configParser.getRepository()->setFileName(argv);
	TypeMap outputMap;
	try
	{
		if (pParser)
		{
			if (!configParser.Attach(argv /*upName*/))
			{
				std::cout << "\n  could not open file " << fileSpec << std::endl;
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
			pParser->parse();                    //2. Parse SemiExpression, returns true?
			
		/*
		std::cout << "\n<--- Contents of Type Table at \"" << argv << "\" --->" << std::endl;
		for (auto it = configParser.getRepository()->getTable()->begin(); it != configParser.getRepository()->getTable()->end(); ++it)
		{
			std::cout << " " << it->first << " : \n\t" << it->second.filename << "\n\t namespace: \"" << it->second.nameSpace << "\"\n\t type:" << it->second.type << std::endl;
		}
		*/
		//configParser->getRepository()->getResultQueue()->enQ(*configParser->getRepository()->getTable());   //4. Display and store table into ResultsQueue
		outputMap = *configParser.getRepository()->getTable();                  //2 threads mess up here...................................
		configParser.getRepository()->resetTypeAnalysis();                                                 //5. Reset Type Analysis data for next iteration
		std::cout << "\n";
	}
	catch (std::exception& ex)
	{
		std::cout << "\n\n    " << ex.what() << "\n\n";
	}
	
	return outputMap;
}

//set filename of file being type analyzed
/*
void TypeAnalyzer::setFileName()
{    //error here?
	std::cout << "filename: " << name << std::endl;
	std::string name = configParser.getParser()->getFileName();      
	configParser->getRepository()->setFileName(name);
}*/

/*
//return filename of file being type analyzed
std::string TypeAnalyzer::getFileName()
{
	return configParser->getParser()->getFileName();
}
*/

/*
//get reference to result queue
BlockingQueue<TypeMap>* TypeAnalyzer::getResultQueue()
{
	return getRepository()->getResultQueue();
}

//Get size of ResultQueue
int TypeAnalyzer::getResultQueueSize()
{
	return getRepository()->getResultQueue()->size();
}
*/

//Merge Tables from Repository's ResultQueue
void TypeAnalyzer::mergeTables(BlockingQueue<TypeMap>* resultQueue)
{
	TypeMap map;

	//Initial Loop based on keys
	while (resultQueue->size() > 0)     //while there is always at least 2 maps in Result Queue...
	{
		map = resultQueue->deQ();       //get 1 table from Result Queue

		if (mergedTypeTable->size() > 0)      //if this isn't first iteration
		{
			for (auto it = map.begin(); it != map.end(); ++it)
			{
				bool keyFound = false;
				for (auto it2 = mergedTypeTable->begin(); it2 != mergedTypeTable->end(); ++it2)
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
					std::pair<std::string, FileScope> typePair(it->first, it->second);   //3. Create Map Pair and insert into typetable
					mergedTypeTable->insert(typePair);
				}
			}
		}
		else         //if mergedTypeTable is initially empty
		{
			mergedTypeTable->insert(map.begin(), map.end());
		}
	}
}

//Print Merged Type Table
void TypeAnalyzer::printMergedTable()
{
	std::cout << "\n<--- Contents of merged Type Table: --->" << std::endl;
	
	for (auto it = mergedTypeTable->begin(); it != mergedTypeTable->end(); ++it)
	{
		std::cout << " " << it->first << " : \n\t file: " << it->second.filename << "\n\t namespace: \"" << it->second.nameSpace << "\"\n\t type: " << it->second.type << std::endl;
	}
}

TypeMap* TypeAnalyzer::getMergedTypeTable()
{
	return mergedTypeTable;
}

//----< test stub >--------------------------------------------
//#define TEST_TYPEANALYZER
#ifdef TEST_TYPEANALYZER

#include <queue>
#include <string>
#define Util StringHelper

int main(int argc, char* argv[])
{
	Util::Title("Testing Parser Class", '=');
	putline();
	// Namespace1.h X.h X.cpp Y.h Y.cpp Global.h Global.cpp
	// collecting tokens from files, named on the command line
	if (argc < 2)
	{
		std::cout
			<< "\n  please enter name of file to process on command line\n\n";
		return 1;
	}

	ConfigParseToConsole configParser;
	TypeAnalyzer* typeAnalyzer = new TypeAnalyzer(&configParser);
	typeAnalyzer->startTypeAnalysis(argc, argv);
	//std::cout << "ResultQueue Size: " << typeAnalyzer->getResultQueueSize() << std::endl;
	//typeAnalyzer->mergeTables();
	//typeAnalyzer->printMergedTable();

	std::cout << "\n";
	system("pause");           //needed for some reason
}

#endif
