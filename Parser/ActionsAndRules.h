#ifndef ACTIONSANDRULES_H
#define ACTIONSANDRULES_H
/////////////////////////////////////////////////////////////////////
//  ActionsAndRules.h - declares new parsing rules and actions     //
//  ver 2.3                                                        //
//  Language:      Visual C++ 2008, SP1                            //
//  Platform:      Dell Precision T7400, Vista Ultimate SP1        //
//  Application:   Prototype for CSE687 Pr1, Sp09                  //
//  Author:        Young Kyu Kim, Syracuse University,             //
//                 (315) 487-9907, ykim127@syr.edu                 //
//  Source:        Jim Fawcett, CST 4-187, Syracuse University     //
//                 (315) 443-3948, jfawcett@twcny.rr.com           //
/////////////////////////////////////////////////////////////////////
/*
  Module Operations: 
  ==================
  This module defines several action classes.  Its classes provide 
  specialized services needed for specific applications.  The modules
  Parser, SemiExpression, and Tokenizer, are intended to be reusable
  without change.  This module provides a place to put extensions of
  these facilities and is not expected to be reusable. 

  Public Interface:
  =================
  Toker t(someFile);              // create tokenizer instance
  SemiExp se(&t);                 // create a SemiExp attached to tokenizer
  Parser parser(se);              // now we have a parser
  Rule1 r1;                       // create instance of a derived Rule class
  Action1 a1;                     // create a derived action
  r1.addAction(&a1);              // register action with the rule
  parser.addRule(&r1);            // register rule with parser
  while(se.getSemiExp())          // get semi-expression
    parser.parse();               //   and parse it

  Build Process:
  ==============
  Required files
    - Parser.h, Parser.cpp, ScopeStack.h, ScopeStack.cpp,
      ActionsAndRules.h, ActionsAndRules.cpp, ConfigureParser.cpp,
      ItokCollection.h, SemiExpression.h, SemiExpression.cpp, tokenizer.h, tokenizer.cpp
  Build commands (either one)
    - devenv CodeAnalysis.sln
    - cl /EHsc /DTEST_PARSER parser.cpp ActionsAndRules.cpp \
         semiexpression.cpp tokenizer.cpp /link setargv.obj

  Maintenance History:
  ====================
  ver 2.3 : 7 April, 2016
  - Updated Rules and Actions to handle Project 3 requirements
  ver 2.2 : 12 March, 2016
  - Updated Beginning of scope rule and Handle Push to acommodate Abstract Syntax Tree
  - Updated End of Scope rule and Handle Pop to accommodate Abstract Syntax Tree
  - Added Metric Analyzer to Repository
  ver 2.1 : 15 Feb 16
  - small functional change to a few of the actions changes display strategy
  - preface the (new) Toker and SemiExp with Scanner namespace
  ver 2.0 : 01 Jun 11
  - added processing on way to building strong code analyzer
  ver 1.1 : 17 Jan 09
  - changed to accept a pointer to interfaced ITokCollection instead
    of a SemiExpression
  ver 1.0 : 12 Jan 06
  - first release

*/
//
#include <queue>
#include <string>
#include <sstream>
#include "Parser.h"
#include "../SemiExp/itokcollection.h"
#include "../ScopeStack/ScopeStack.h"
#include "../Tokenizer/Tokenizer.h"
#include "../SemiExp/SemiExp.h"
#include "ASTree.h"
#include "MetricAnalyzer.h"
#include "../DependencyAnalyzer/Cpp11-BlockingQueue.h"
#include <unordered_map>
#include "../DependencyAnalyzer/FileNameScope.h"

///////////////////////////////////////////////////////////////
// Repository instance is used to share resources
// among all actions.
/*
 * ToDo:
 * - add AST Node class
 * - provide field to hold root of AST
 * - provide interface to access AST
 * - provide helper functions to build AST, perhaps in Tree class
 */

class Repository               // application specific
{
private:
	//Project 2 Variables
	ScopeStack<ASNode*> stack;   //Scope Stack
	Scanner::Toker* p_Toker;
	ASTree* tree;                //Q: Tree that holds scope nodes
	MetricAnalyzer* mAnalyzer;   //Q: Analyzer for metric analysis

	//Project 3 Variables
	std::unordered_multimap<std::string, FileScope>* tTable;                          // Temporary Table to store types
	std::unordered_multimap<std::string, FileScope>* typeTable;                       // shared reference to TypeAnalyzer's typetable
	std::unordered_multimap<std::string, FileScope>* depTable;

	std::vector<std::string>* tVector;                      // Temporary vector to hold types contained in a certain file
	FileScope* tScope;                     //Temporary FileNameScope that holds namespace and fileinformation 
	std::string fileName;

public:
  Repository(Scanner::Toker* pToker, std::unordered_multimap<std::string, FileScope>* table)      //Constructor
  {
    p_Toker = pToker;

	//Initialize Root node for AST
	ASNode * root = new ASNode();          //Global Name Space
	root->name = "Global";
	root->type = "namespace";
	root->lineStart = 0;

	//stack.push(root);                       //Push root node into ScopeStack, PROJ 3
	tree = new ASTree(root);                //update tree's root
	mAnalyzer = new MetricAnalyzer(tree);   //Analyzer has reference to tree
	tTable = new std::unordered_multimap<std::string, FileScope>();                   //proj 3
	typeTable = table;                                      //shared from configParser
	depTable = new std::unordered_multimap<std::string, FileScope>();
	tVector = new std::vector<std::string>(); //proj3
	tScope = new FileScope();             //proj3
  }
  ScopeStack<ASNode*>& scopeStack()             //returns reference to scopeStack
  {
    return stack;
  }
  Scanner::Toker* Toker()
  {
    return p_Toker;
  }
  size_t lineCount()
  {
    return (size_t)(p_Toker->currentLineCount());   //fawcett's version: return toker's linecount
  }

  //get Tree
  ASTree* getTree() 
  { 
	  return tree; 
  }

  /*
  //Proj 3: returns reference to _resultQueue for Type Analysis
  BlockingQueue<std::unordered_multimap<std::string, FileScope>>* getResultQueue()
  {
	  return _resultsQueue;
  }

  //Proj 3: returns reference to _resultQueue for Dependency Analysis
  BlockingQueue<std::unordered_multimap<std::string, FileScope>>* getResultQueue2()
  {
	  return _resultsQueue2;
  }
  */

  //Proj 3: sets or returns filename
  void setFileName(std::string file)
  {
	  fileName = file;
  }

  std::string getFileName()
  {
	  return fileName;
  }

  //Proj 3: gets reference to table
  std::unordered_multimap<std::string, FileScope>* getTable()
  {
	  return tTable;
  }

  //Proj 3: get reference to vector
  std::vector<std::string>* getVector()
  {
	  return tVector;
  }

  FileScope* getScope()
  {
	  return tScope;
  }

  //Proj 3: Update reference to Toker when ConfigParser reBuilds()
  void setToker(Scanner::Toker* pToker)
  {
	  p_Toker = pToker;
  }
  //Proj 3: update reference to shard TypeTable from TypeAnalyzer
  void setTypeTable(std::unordered_multimap<std::string, FileScope>* table)
  {
	  typeTable = table;
  }

  //Proj 3: Reset TypeTable and Vector
  void resetTypeAnalysis()
  {
	  delete tTable;
	  delete tVector;

	  tTable = new std::unordered_multimap<std::string, FileScope>();                   //proj 3
	  tScope->filename = "";
	  tScope->nameSpace = "";
	  tVector = new std::vector<std::string>(); //proj3
  }
  
  void resetDepAnalysis()
  {
	  delete tTable;
	  delete tVector;
	  delete depTable;

	  tTable = new std::unordered_multimap<std::string, FileScope>();                   //proj 3
	  depTable = new std::unordered_multimap<std::string, FileScope>();
	  tScope->filename = "";
	  tScope->nameSpace = "";
	  tVector = new std::vector<std::string>(); //proj3
  }

  //Proj 3: get reference to shared TypeTable
  std::unordered_multimap<std::string, FileScope>* getTypeTable()
  {
	  return typeTable;
  }

  //Proj 3: get reference to shared DepTable
  std::unordered_multimap<std::string, FileScope>* getDepTable()
  {
	  return depTable;
  }

  //Perform Metric Analysis and Print tree
  void analyzeTree()
  {
	  mAnalyzer->printTree(mAnalyzer->getTree()->getRoot(), 0);
  }

};


/***************************************
	ScopeStack Rules and Actions
****************************************/

///////////////////////////////////////////////////////////////
// rule to detect beginning of anonymous scope

class BeginningOfScope : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		if (pTc->find("{") < pTc->length())
		{
			doActions(pTc);
			return true;
		}
		return true;
	}
};

///////////////////////////////////////////////////////////////
// action to handle scope stack at end of scope

class HandlePush : public IAction
{
	Repository* p_Repos;                                  //Points to repository located in ConfigureParser
public:
	HandlePush(Repository* pRepos)
	{
		p_Repos = pRepos;
	}

	//check if loop
	bool isLoop(const std::string& tok)
	{
		const static std::string keys[] = { "for", "while", "switch", "if", "catch" };
		for (int i = 0; i<5; ++i)
			if (tok == keys[i])
				return true;
		return false;
	}

	//check if conditional
	bool isConditional(const std::string& tok)
	{
		const static std::string keys[] = { "if", "else if", "else", "switch" };
		for (int i = 0; i<5; ++i)
			if (tok == keys[i])
				return true;
		return false;
	}

	//check if struct
	bool isStruct(const std::string& tok)
	{
		if (tok == "struct")
			return true;
		else
			return false;
	}

	//check if exception
	bool isException(const std::string& tok)
	{
		const static std::string keys[] = { "try", "catch", "finally", "throw" };
		for (int i = 0; i<4; ++i)
			if (tok == keys[i])
				return true;
		return false;
	}

	void doAction(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;               //get reference to semi expression
												 //1. Create Elem
		ASNode* elem = new ASNode();                       //*******(HEAP)    elem.type = "unknown";                               //initialize type	 
		std::string name = "Anonymous";
		std::string type = "Anonymous";
		int i = 0;
		while (tc[i] == "\n" || tc[i] == " ")        //1. Eat any \n or white lines if needed
			i++;
		if (tc.find("{") < tc.length())              //2. Double check that { is there
		{
			size_t len = tc.find("(");               //3. Check scope type         
			if (isLoop(tc[len - 1]))
				type = "loop";
			else if (isConditional(tc[i]))
				type = "conditional";
			else if (isStruct(tc[i]))
				type = "struct";
			else if (isException(tc[i]))
				type = "exception";
			else if(tc.find("(") < tc.length())                                    //function
			{
				name = (*pTc)[pTc->find("(") - 1];
				type = "function";
			}
			else if(tc[i] == "namespace")    //namespace scope
			{                     
				name = tc[i + 1];
				type = "namespace";
			}
			else if (tc[i] == "class")       //class scope
			{
				name = tc[i + 1];
				type = "class";
			}
		}
		elem->name = name;                         //2a) Add to Scope Stack
		elem->type = type;
		elem->lineStart = p_Repos->lineCount();    //initialize line count									   
		checkScopeThenAddChild(elem);
		//2c) Add to scopestack : add element to top element of scope stack
		p_Repos->scopeStack().push(elem);                    //get reference to Repository's stack, and push the element
	}

	void checkScopeThenAddChild(ASNode* elem)
	{
		if (p_Repos->scopeStack().size() != 0)     //2b) Make Child: Get current top, and add child
			p_Repos->scopeStack().top()->addChild(elem);
	}
};


///////////////////////////////////////////////////////////////
// rule to detect end of scope

class EndOfScope : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		if (pTc->find("}") < pTc->length())
		{
			doActions(pTc);
			return true;
		}
		return true;
	}
};

///////////////////////////////////////////////////////////////
// action to handle scope stack at end of scope: Here we update a scope's size

class HandlePop : public IAction
{
	Repository* p_Repos;
	bool isMetric;

public:
	HandlePop(Repository* pRepos, bool isprint)
	{
		p_Repos = pRepos;
		isMetric = isprint;
	}
	void doAction(ITokCollection*& pTc)
	{
		//std::cout << "\n--EndOfScope rule";
		if (p_Repos->scopeStack().size() == 0)
			return;

		p_Repos->scopeStack().top()->lineEnd = p_Repos->lineCount() + 1; //update lineEnd before we pop
		ASNode* elem = p_Repos->scopeStack().pop();          //save copy of currest scope node after popping
															 //std::cout << "Popping " << elem->name << std::endl;
															 //elem->lineEnd = p_Repos->lineCount();

	}
};

/*************************************
	Project 3 Rules and Actions
**************************************/

///////////////////////////////////////////////////////////////
// Q's New rule: Detects aliases and typenames
class DetectType : public IRule
{
	Repository* p_Repos;                                  //Points to repository located in ConfigureParser
public:
	DetectType(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;               //get reference to semi expression

		//1. Eat any \n or white lines if needed
		int i=0;
		while (tc[i] == "\n" || tc[i] == " ")
		{
			i++;
		}

		//2. Check for namespaces
		/*
		if (tc[i] == "namespace")
		{
			//curr namespace
			p_Repos->getScope()->nameSpace = tc[i + 1];
		}
		*/

		
		//3. Check for types
		if (tc[i] == "class" || tc[i] == "struct" || tc[i] == "enum" || tc[i] == "typedef" || tc[i] == "using" || tc[i] == "namespace" || tc.find("(") < tc.length())
		{
			if (tc[i] != "for" && tc[i] != "if" && tc[i] != "else" && tc[i] != "while" && tc[i] != "try" && tc[i] != "catch")
			{
				doActions(pTc);
				return true;
			}
		}
		return true;
	}
};

///////////////////////////////////////////////////////////////
// Q's New Action: store type and corresponding filename into table
class StoreType : public IAction
{
	Repository* p_Repos;                                  //Points to repository located in ConfigureParser
public:
	StoreType(Repository* pRepos)
	{
		p_Repos = pRepos;
	}


	void doAction(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;                          //get reference to semi expression
		std::string filename = p_Repos->getFileName();      //get filename of file being analyzed
		int i = 0;                                       	//1. Eat any \n or white lines if needed
		while (tc[i] == "\n" || tc[i] == " ")
			i++;
		std::string name;                 		//3. Get type definition's name
		std::string type;
		if (tc[i] == "typedef")           //SPECIAL CASE: typedef
		{
			size_t len = tc.find(";");
			name = tc[len - 1];
			type = "typedef";
		}
		else if(tc[i] == "using")         //SPECIAL CASE: using
		{
			checkUsing(pTc, name, type, i);
		}
		else if (tc[i] == "class" || tc[i] == "struct" || tc[i] == "enum")                  //class, struct, enum
		{
			name = tc[i + 1];
			type = tc[i];
		}
		else         		//Check if tc is a function
		{
			if (tc[tc.length() - 1] == "{" || tc[tc.length() - 1] == ";")      //if function definition, e.g., int funct() { };
			{
				if (checkFunction(pTc, name, type, i))
					return;
			}
		}
		if (p_Repos->scopeStack().size() != 0)                                                    //4. pushback type into vector
			p_Repos->getScope()->nameSpace = p_Repos->scopeStack().getStack()->front()->name;     //get current scope's name
		else
			p_Repos->getScope()->nameSpace = "global";
		if (containsOtherExitCases(pTc, i))                       //exit if there are other exit cases
			return;
		p_Repos->getScope()->filename = p_Repos->getFileName();   //otherwise, just add it
		p_Repos->getScope()->type = type;
		std::pair<std::string, FileScope> typePair(name, *p_Repos->getScope());   //3. Create Map Pair and insert into typetable
		p_Repos->getTable()->insert(typePair);
	}

	//Exit if we find lamdas and namespaces, and other cases
	bool containsOtherExitCases(ITokCollection*& pTc, int i)
	{
		ITokCollection& tc = *pTc;
		if (tc[i] == "namespace")    //            return if namespace after updating scopestack
			return true;
		if (tc.find("(") < tc.length())             //is it a function?
		{
			if (tc[tc.find("(") - 1] == "]")        //don't handle lamdas
			{
				return true;
			}
		}
		if (tc[i + 1] == "=")                       //function assignment?   x = std::function<> ();
			return true;
		return false;
	}

	//Check "using"
	void checkUsing(ITokCollection*& pTc, std::string &name , std::string &type, int i)
	{
		ITokCollection& tc = *pTc;                          //get reference to semi expression
		type = "using";
		if (tc[i + 1] == "namespace")			//define using
		{
			size_t len = tc.find(";");
			name = tc[len - 1];
		}
		else			//declare using
		{
			name = tc[i + 1];
		}
	}
	
	//check if its a function; returns true if global function found. Returns false if global func not found
	bool checkFunction(ITokCollection*& pTc, std::string &name, std::string &type, int i)
	{
		ITokCollection& tc = *pTc;                          //get reference to semi expression
		size_t len_f = tc.find("(");
		if (len_f < tc.length())                            //function declaration found
		{
			if (p_Repos->scopeStack().size() == 0)          //Only Add global functions 1)Global Scope   2)No ::
			{
				type = "function";
				if (tc[len_f - 2] != ".")               // function call? e.g.     x.funct(); 
				{
					if (tc[len_f - 2] != "::")          // no namespace before it? e.g., X::funct();
					{
						//get function name               
						if (tc[len_f - 1] != "main" && tc[len_f - 1] != "]")      //Dont include int main()s and lamdas in type table
							name = tc[len_f - 1];
						else
							return false;
					}
				}
				else
				{
					return true;                           //return if function call
				}
			}
			else
				return true;
		}

		return false;         // Correct?
	}
};

///////////////////////////////////////////////////////////////
// Q's New Action: print scope info
class PrintTypeTable : public IAction
{
	Repository* p_Repos;                                  //Points to repository located in ConfigureParser
public:
	PrintTypeTable(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{
		std::unordered_multimap<std::string, FileScope> tTable;
		/*
		while (p_Repos->getResultQueue()->size() != 0)
		{
			tTable = p_Repos->getResultQueue()->deQ();

			std::cout << "\n<--- Contents of merged table: --->" << std::endl;
		}
		*/
	}
};

///////////////////////////////////////////////////////////////
// Q's New Proj 3 Rule : Simple Scan for Type Declaration for Dependency Analysis
class ScanTypeDeclaration : public IRule
{
	Repository* p_Repos;                                  //Points to repository located in ConfigureParser
public:
	ScanTypeDeclaration(Repository* pRepos)
	{
		p_Repos = pRepos;
	}

	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;               //get reference to semi expression

		std::vector<std::string> keywords = {"int", "double", "float", "bool", "char", "long",                    //Handle Primitive Types
											 "std",                                                               //Handle std::
											 "public", "private", "protected",                                    //Handle access
											 "for"
		};

		//1. Eat any \n or white lines if needed
		int i = 0;
		while (tc[i] == "\n" || tc[i] == " ")
		{
			i++;
		}

		//2. Check if first token contains keyword that we can ignore
		doActions(pTc);

		return true;
	}
};

///////////////////////////////////////////////////////////////
// Q's New Proj 3 Action : Simple Run for Dependency Analysis
class StoreDependency : public IAction
{
	Repository* p_Repos;                                  //Points to repository located in ConfigureParser
	std::vector<std::string>* list_namespace;
public:
	StoreDependency(Repository* pRepos, std::vector<std::string>* namespaces)
	{
		p_Repos = pRepos;
		list_namespace = namespaces;
	}

	void doAction(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;                         //get reference to semi expression
		if (static_cast<int>(tc.length()) < 3)
			return;
		std::string filename = p_Repos->getFileName();     //get filename of file being analyzed
		int i = 0;
		while (tc[i] == "\n" || tc[i] == " ")        //1. Eat any \n or white lines if needed
			i++;
		checkNameSpace(pTc, i);
		isInTypeTable(pTc, i);                       //Simple check to see if pTc contains any elements from typetable
	}
	
	//Check if tokens in pTc is inside typetable
	
	void isInTypeTable(ITokCollection*& pTc, int i)
	{
		ITokCollection& tc = *pTc;
		for (auto it = p_Repos->getTypeTable()->begin(); it != p_Repos->getTypeTable()->end(); ++it)
		{
			size_t len = tc.find(it->first);      //len: location of token
			if (len < tc.length() && it->second.filename != p_Repos->getFileName())       //if found is not the same filename
			{
				FileScope* scope = new FileScope();                      // NAMESPACE::MEMBER                 tc[i]   is namespace;   Type Name is in tc[i+2]
				if (tc[len - 1] == "::" && tc[len - 2] == it->second.nameSpace)				//Check Namespace //1. Check for :: behind token for namespace
				{
					scope->filename = it->second.filename;
					std::pair<std::string, FileScope> depPair(p_Repos->getFileName(), *scope);
					p_Repos->getDepTable()->insert(depPair);                                          //store dependency
				}
				else if(list_namespace->size() == 0) 				//2. is is a global namespace??     //using namespace list empty?
				{
					if (p_Repos->scopeStack().getStack()->size() == 0) //scopestack empty?
					{
						scope->filename = it->second.filename;
						std::pair<std::string, FileScope> depPair(p_Repos->getFileName(), *scope);
						p_Repos->getDepTable()->insert(depPair);                                          //store dependency
					}
					else if (p_Repos->scopeStack().getStack()->front()->type != "namespace")
					{
						scope->filename = it->second.filename;
						std::pair<std::string, FileScope> depPair(p_Repos->getFileName(), *scope);
						p_Repos->getDepTable()->insert(depPair);                                          //store dependency
					}
					else { }
				}
				else if(p_Repos->scopeStack().getStack()->front()->name == it->second.nameSpace)			//3. Check ScopeStack
				{
					scope->filename = it->second.filename;                          
					std::pair<std::string, FileScope> depPair(p_Repos->getFileName(), *scope);
					p_Repos->getDepTable()->insert(depPair);                                          //store dependency
				}
				else                                      //scopestack contains namespace
				{
					for (size_t i = 0; i < list_namespace->size(); i++)  {                    //iterate through namespace list
						if ((*list_namespace)[i] == it->second.nameSpace) {
							scope->filename = it->second.filename;
							std::pair<std::string, FileScope> depPair(p_Repos->getFileName(), *scope);
							p_Repos->getDepTable()->insert(depPair);                                          //store dependency
						}
					}
				}
			}
		}
	}

	//check current namespace
	void checkNameSpace(ITokCollection*& pTc, int i)
	{
		ITokCollection& tc = *pTc;                         //get reference to semi expression
		if (tc[i] == "using")                        //Keep track of which namespace the file is using
		{
			if (tc[i + 1] == "namespace")			//define using
			{
				size_t len = tc.find(";");
				list_namespace->push_back(tc[len - 1]);
				//name_space = tc[len - 1];           //store namespace's name
			}
		}
	}

	//Checks functional dependencies for Global Functions
	void checkFunctionalDependency(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;                         //get reference to semi expression
		for (auto it = p_Repos->getTypeTable()->begin(); it != p_Repos->getTypeTable()->end(); ++it)
		{
			size_t len = tc.find("(");          //Find index of '('
			if (len < tc.length())              //if function declaration
			{
				if (tc[len - 1] == it->first && it->second.filename != p_Repos->getFileName())        //If token declaration is found
				{
					if (p_Repos->scopeStack().size() != 0)                                  //if there is a namespace
					{
						//p_Repos->getScope()->nameSpace = p_Repos->scopeStack().getStack()->front()->name;      //get current scope's name
						if (p_Repos->scopeStack().getStack()->front()->name == it->second.nameSpace)
						{
							FileScope* scope = new FileScope();
							scope->filename = it->second.filename;
							scope->nameSpace = "";

							std::pair<std::string, FileScope> depPair(p_Repos->getFileName(), *scope);
							p_Repos->getDepTable()->insert(depPair);      //store dependency
						}
					}
					else                                                                     //if there is no namespace -> global namespace
					{
						FileScope* scope = new FileScope();
						scope->filename = it->second.filename;
						scope->nameSpace = "";

						std::pair<std::string, FileScope> depPair(p_Repos->getFileName(), *scope);
						p_Repos->getDepTable()->insert(depPair);      //store dependency
					}
				}
			}
		}
	}
};

///////////////////////////////////////////////////////////////
// Q's New + Last rule : Last rule to display ASTree

class EndOfFile : public IRule
{
	Repository* p_Repos;                                  //Points to repository located in ConfigureParser
public:
	EndOfFile(Repository* pRepos)
	{
		p_Repos = pRepos;
	}

	bool doTest(ITokCollection*& pTc)
	{
		//std::cout << "Checking EndOfFile, length = " << p_Repos->scopeStack().size() <<std::endl;
		if (p_Repos->scopeStack().size() == 1)        //if we find last scope
		{
			doActions(pTc);
			return true;
		}
		//doActions(pTc);
		return true;
	}
};
#endif
