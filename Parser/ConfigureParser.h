#ifndef CONFIGUREPARSER_H
#define CONFIGUREPARSER_H
/////////////////////////////////////////////////////////////////////
//  ConfigureParser.h - builds and configures parsers              //
//  ver 2.3                                                        //
//                                                                 //
//  Application:   Prototype for CSE687 Pr1, Sp06                  //
//  Author:        Young Kyu Kim, ykim127@syr.edu, (315) 870-8906  //
//  Source:        Jim Fawcett, CST 2-187, Syracuse University     //
//                 (315) 443-3948, jfawcett@twcny.rr.com           //
/////////////////////////////////////////////////////////////////////
/*
  Module Operations: 
  ==================
  This module builds and configures parsers.  It builds the parser
  parts and configures them with application specific rules and actions.

  Public Interface:
  =================
  ConfigParseToConsole conConfig;
  conConfig.Build();
  conConfig.Attach(someFileName);

  Build Process:
  ==============
  Required files
    - ConfigureParser.h, ConfigureParser.cpp, Parser.h, Parser.cpp,
      ActionsAndRules.h, ActionsAndRules.cpp,
      SemiExpression.h, SemiExpression.cpp, tokenizer.h, tokenizer.cpp
  Build commands (either one)
    - devenv Project1HelpS06.sln
    - cl /EHsc /DTEST_PARSER ConfigureParser.cpp parser.cpp \
         ActionsAndRules.cpp \
         semiexpression.cpp tokenizer.cpp /link setargv.obj

  Maintenance History:
  ====================
  ver 2.3 : 7 April, 2016
  - Supports different build configurations for Type and Dependency Analyzer
  ver 2.2 : 12 Mar, 2016
  - Deleted all rules and actions except for BeginScope, HandlePush, EndScope, HandlePop
  ver 2.1 : 19 Feb 16
  - Added PrintFunction action to FunctionDefinition rule
  ver 2.0 : 01 Jun 11
  - Major revisions to begin building a strong code analyzer
  ver 1.1 : 01 Feb 06
  - cosmetic changes to ConfigureParser.cpp
  ver 1.0 : 12 Jan 06
  - first release

*/

//
#include <fstream>
#include "Parser.h"
#include "../SemiExp/SemiExp.h"
#include "../Tokenizer/Tokenizer.h"
#include "ActionsAndRules.h"
//#include "../DependencyAnalyzer/TypeAnalyzer.h"//Proj3

///////////////////////////////////////////////////////////////
// build parser that writes its output to console

class ConfigParseToConsole : IBuilder
{
public:
  ConfigParseToConsole() {};
  ~ConfigParseToConsole();
  bool Attach(const std::string& name, bool isFile=true);
  Parser* Build();          
  Parser* BuildMetric();
  Parser* BuildType();                     //Parser Configuration for Type Analysis
  Parser* BuildDependency(std::unordered_multimap<std::string, FileScope>* table);               //Parser Configuration for Dependency Analysis
  
  void setFilename(std::string name);      //set filename of file being analyzed

	//Proj 3 File related function
  Parser* getParser();
  Repository* getRepository();
  void setTypeTable(std::unordered_multimap<std::string, FileScope>* table);
  void setDepTable(std::unordered_multimap<std::string, std::string>* table);

  std::vector<std::string>* getVectorNamespace();

private:
  // Builder must hold onto all the pieces

  std::ifstream* pIn;
  Scanner::Toker* pToker;
  Scanner::SemiExp* pSemi;
  Parser* pParser;
  Repository* pRepo;

  //ScopeStack Stuff
  BeginningOfScope* pBeginningOfScope;
  EndOfScope* pEndOfScope;
  HandlePush* pHandlePush;
  HandlePop* pHandlePop;

  //Project 3 TypeAnalysis stuff
  DetectType* pDetectType;
  StoreType* pStoreType;
  PrintTypeTable* pPrintTypeTable;
  std::unordered_multimap<std::string, FileScope>* typeTable;      //Reference to TypeAnalyzer's typetable

  //Project 3 Dependency Analysis stuff
  ScanTypeDeclaration* pScanTypeDeclaration;
  StoreDependency* pStoreDependency;
  std::unordered_multimap<std::string, std::string>* depTable;       //Reference to Dependency Analyzer's typeTable
  std::vector<std::string> list_namespace;

  // prohibit copies and assignments
  ConfigParseToConsole(const ConfigParseToConsole&) = delete;
  ConfigParseToConsole& operator=(const ConfigParseToConsole&) = delete;
};


#endif
