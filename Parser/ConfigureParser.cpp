/////////////////////////////////////////////////////////////////////
//  ConfigureParser.cpp - builds and configures parsers            //
//  ver 2.3                                                        //
//                                                                 //
//  Lanaguage:     Visual C++ 2005                                 //
//  Platform:      Dell Dimension 9150, Windows XP SP2             //
//  Application:   Prototype for CSE687 Pr1, Sp06                  //
//  Author:        Young Kyu Kim, Syracuse University,             //
//                 ykim127@syr.edu                                 //
//  Source:        Jim Fawcett, CST 2-187, Syracuse University     //
//                 (315) 443-3948, jfawcett@twcny.rr.com           //
/////////////////////////////////////////////////////////////////////

#include <fstream>
#include "Parser.h"
#include "../SemiExp/SemiExp.h"
#include "../Tokenizer/Tokenizer.h"
#include "ActionsAndRules.h"
#include "ConfigureParser.h"

using namespace Scanner;

//----< destructor releases all parts >------------------------------

ConfigParseToConsole::~ConfigParseToConsole()
{
	// when Builder goes out of scope, everything must be deallocated

	delete pBeginningOfScope;
	delete pEndOfScope;
	delete pHandlePush;
	delete pHandlePop;

	delete pDetectType;
	delete pStoreType;
	delete pPrintTypeTable;
	delete pScanTypeDeclaration;
	delete pStoreDependency;

	delete pRepo;
	delete pParser;
	delete pSemi;
	delete pToker;
	pIn->close();
	delete pIn;
}
//----< attach toker to a file stream or stringstream >------------

bool ConfigParseToConsole::Attach(const std::string& name, bool isFile)
{
  if(pToker == 0)
    return false;
  pIn = new std::ifstream(name);
  if (!pIn->good())
    return false;
  return pToker->attach(pIn);
}
//----< Here's where alll the parts get assembled >----------------
// WARNING!!! : even Repository will be reset
//Build Parser for Type Analysis
Parser* ConfigParseToConsole::Build()
{
  try
  {
    // add Parser's main parts
    pToker = new Toker;
    pToker->returnComments(false);
    pSemi = new SemiExp(pToker);
    pParser = new Parser(pSemi);         //Originally, Constructor is Parser(ITokCollection* itok), but since pSemi is a derived class of ITokCollection, it works
    
	//PROJECT 3: ONLY MAKE REPOSITORY ONCE
	if (pRepo == NULL)
		pRepo = new Repository(pToker, typeTable);               //Proj 3, updated repository constructor
	else
		pRepo->setToker(pToker);

	/*
		ScopeStack Rules and Actions
	*/
	//ScopeStack Stuff
	pBeginningOfScope = new BeginningOfScope();
	pHandlePush = new HandlePush(pRepo);
	pBeginningOfScope->addAction(pHandlePush);
	pEndOfScope = new EndOfScope();
	pHandlePop = new HandlePop(pRepo, false);
	pEndOfScope->addAction(pHandlePop);

	/*
		Project 3 Rules and Actions
	*/
	pDetectType = new DetectType(pRepo);                   //Rule
	pStoreType = new StoreType(pRepo);                //Action
	
	pDetectType->addAction(pStoreType);

	pParser->addRule(pBeginningOfScope);
	pParser->addRule(pDetectType);
	pParser->addRule(pEndOfScope);
		
	//pParser->addRule(pEndOfScope);
	
    return pParser;
  }
  catch(std::exception& ex)
  {
    std::cout << "\n\n  " << ex.what() << "\n\n";
    return 0;
  }
}

//Similar to Build above, but displays Metric Analysis in the end instead of AST
Parser* ConfigParseToConsole::BuildMetric()
{
	try
	{
		// add Parser's main parts

		pToker = new Toker;
		pToker->returnComments(false);
		pSemi = new SemiExp(pToker);
		pParser = new Parser(pSemi);         //Originally, Constructor is Parser(ITokCollection* itok), but since pSemi is a derived class of ITokCollection, it works
		//pRepo = new Repository(pToker);

		// configure to manage scope
		// these must come first - they return true on match
		// so rule checking continues

		return pParser;
	}
	catch (std::exception& ex)
	{
		std::cout << "\n\n  " << ex.what() << "\n\n";
		return 0;
	}
}

//Build Parser for Dependency Analysis Pass
Parser* ConfigParseToConsole::BuildDependency(std::unordered_multimap<std::string, FileScope>* table)
{
	try
	{
		typeTable = table;

		// add Parser's main parts
		pToker = new Toker;
		pToker->returnComments(false);
		pSemi = new SemiExp(pToker);
		pParser = new Parser(pSemi);         //Originally, Constructor is Parser(ITokCollection* itok), but since pSemi is a derived class of ITokCollection, it works

		//PROJECT 3: ONLY MAKE REPOSITORY ONCE
		if (pRepo == NULL)
			pRepo = new Repository(pToker, typeTable);               //Proj 3, updated repository constructor
		else
		{
			pRepo->setToker(pToker);
			pRepo->setTypeTable(typeTable);
		}

		//ScopeStack stuff
		pBeginningOfScope = new BeginningOfScope();
		pHandlePush = new HandlePush(pRepo);
		pBeginningOfScope->addAction(pHandlePush);
		pEndOfScope = new EndOfScope();
		pHandlePop = new HandlePop(pRepo, false);
		pEndOfScope->addAction(pHandlePop);
		
		//Project 3 Rules and Actions
		pScanTypeDeclaration = new ScanTypeDeclaration(pRepo);  //Rule
		pStoreDependency = new StoreDependency(pRepo, &list_namespace);          //Action
		pScanTypeDeclaration->addAction(pStoreDependency);      //
		pParser->addRule(pBeginningOfScope);
		pParser->addRule(pScanTypeDeclaration);
		pParser->addRule(pEndOfScope);

		return pParser;
	}
	catch (std::exception& ex)
	{
		std::cout << "\n\n  " << ex.what() << "\n\n";
		return 0;
	}
}


//Set filename of rootnode
void ConfigParseToConsole::setFilename(std::string name)
{
	//pRepo->getTree()->getRoot()->setFilename(name);
}

//Get reference to Parser
Parser* ConfigParseToConsole::getParser()
{
	return pParser;
}

//Get reference to Repository
Repository* ConfigParseToConsole::getRepository()
{
	return pRepo;
}

// set reference to TypeAnalyzer's TypeTable
void ConfigParseToConsole::setTypeTable(std::unordered_multimap<std::string, FileScope>* table)
{
	typeTable = table;
}

// set reference to DependencyAnalyzer's TypeTable
void ConfigParseToConsole::setDepTable(std::unordered_multimap<std::string, std::string>* table)
{
	depTable = table;
}

//returns reference to dependency Analyzer
std::vector<std::string>* ConfigParseToConsole::getVectorNamespace()
{
	return &list_namespace;
}


#ifdef TEST_CONFIGUREPARSER

#include <queue>
#include <string>

int main(int argc, char* argv[])
{
  std::cout << "\n  Testing ConfigureParser module\n "
            << std::string(32,'=') << std::endl;

  // collecting tokens from files, named on the command line

  if(argc < 2)
  {
    std::cout 
      << "\n  please enter name of file to process on command line\n\n";
    return 1;
  }

  for(int i=1; i<argc; ++i)
  {
    std::cout << "\n  Processing file " << argv[i];
    std::cout << "\n  " << std::string(16 + strlen(argv[i]),'-');

    ConfigParseToConsole configure;
    Parser* pParser = configure.Build();
    try
    {
      if(pParser)
      {
        if(!configure.Attach(argv[i]))
        {
          std::cout << "\n  could not open file " << argv[i] << std::endl;
          continue;
        }
      }
      else
      {
        std::cout << "\n\n  Parser not built\n\n";
        return 1;
      }
      // now that parser is built, use it

      while(pParser->next())
        pParser->parse();
      std::cout << "\n\n";
    }
    catch(std::exception& ex)
    {
      std::cout << "\n\n    " << ex.what() << "\n\n";
    }
    std::cout << "\n\n";
  }
}

#endif
