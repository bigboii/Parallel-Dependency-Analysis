/////////////////////////////////////////////////////////////////////
//  Parser.cpp - Analyzes C++ language constructs                  //
//  ver 1.4                                                        //
//  Language:      Visual C++ 2008, SP1                            //
//  Platform:      Dell Precision T7400, Vista Ultimate SP1        //
//  Application:   Prototype for CSE687 Pr1, Sp09                  //
//  Author:        Jim Fawcett, CST 4-187, Syracuse University     //
//                 (315) 443-3948, jfawcett@twcny.rr.com           //
/////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>
#include "../Utilities/Utilities.h"
#include "../Tokenizer/Tokenizer.h"
#include "../SemiExp/SemiExp.h"
#include "Parser.h"
#include "ActionsAndRules.h"
#include "ConfigureParser.h"

using namespace Scanner;
using namespace Utilities;

//----< register parsing rule >--------------------------------

void Parser::addRule(IRule* pRule)
{
  rules.push_back(pRule);
}
//----< get next ITokCollection >------------------------------

bool Parser::next() 
{ 
  // default operation of doFold(pTokColl) is to simply return
  // - code analysis provides specific rules for handling
  //   for statements and operator expressions which are
  //   bound in ConfigureParser.cpp
  bool succeeded = pTokColl->get();            //get one SemiExpression
  if(!succeeded)
    return false;
  return true;
}

//----< parse the SemiExp by applying all rules to it >--------

bool Parser::parse()
{
  bool succeeded = false;
  for(size_t i=0; i<rules.size(); ++i)
  {
    if(rules[i]->doTest(pTokColl))          //doTest() is implemented in every derived classes of IRule
      succeeded = true;
  }
  return succeeded;
}

//----< Proj 3: Update Filename the parser will be working on >--------
void Parser::updateFileName(std::string fileSpec)
{
	fileName = fileSpec;
}

//----< Proj 3: Get filename the parser will be working on >--------
std::string Parser::getFileName()
{
	return fileName;
}

//----< register action with a rule >--------------------------

void IRule::addAction(IAction *pAction)
{
  actions.push_back(pAction);
}
//----< invoke all actions associated with a rule >------------

void IRule::doActions(ITokCollection*& pTokColl)
{
  if(actions.size() > 0)
    for(size_t i=0; i<actions.size(); ++i)
      actions[i]->doAction(pTokColl);
}

//----< test stub >--------------------------------------------

#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"

//#define TEST_PARSER
#ifdef TEST_PARSER

#include <queue>
#include <string>
#define Util StringHelper

int main(int argc, char* argv[])
{
  Util::Title("Testing Parser Class", '=');
  putline();

  // collecting tokens from files, named on the command line
  if(argc < 2)
  {
    std::cout 
      << "\n  please enter name of file to process on command line\n\n";
    return 1;
  }

  for(int i=1; i<argc; ++i)
  {
    std::string fileSpec = FileSystem::Path::getFullFileSpec(argv[i]);
    std::string msg = "Processing file" + fileSpec;
    Util::title(msg);
    putline();
    ConfigParseToConsole configure;
    Parser* pParser = configure.Build();       //configure initializes parser with Toker, SemiExpression, and adds Rules and Actions
	pParser->updateFileName(argv[i]);         //Proj 3, update File Info
    try
    {
      if(pParser)
      {
        if(!configure.Attach(argv[i]))
        {
          std::cout << "\n  could not open file " << fileSpec << std::endl;
          continue;
        }
      }
      else
      {
        std::cout << "\n\n  Parser not built\n\n";
        return 1;
      }
      // now that parser is built, use it
      while(pParser->next())                  //1. get One SemiExpression
        pParser->parse();                     //2. Parse SemiExpression, returns true?
      std::cout << "\n";
    }
    catch(std::exception& ex)
    {
      std::cout << "\n\n    " << ex.what() << "\n\n";
    }
  }
  std::cout << "\n";
  system("pause");           //needed for some reason
}

#endif
