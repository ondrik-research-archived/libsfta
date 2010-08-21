/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Implementation file of CUDDFacade. This file uses directly CUDD
 *    functions and provides their actions through the interface of
 *    CUDDFacade.
 *
 *****************************************************************************/


// Standard library headers
#include <stdexcept>

// SFTA headers
#include <sfta/cudd_facade.hh>
#include <sfta/sfta.hh>
#include <sfta/convert.hh>
#include <sfta/fake_file.hh>

// CUDD headers
#include <util.h>
#include <cudd.h>
#include <cuddInt.h>
#include <dddmp.h>

using namespace SFTA::Private;


// A set of functions that converts between SFTA and CUDD manager and node
// types. 
namespace SFTA
{
	namespace Private
	{
		/**
		 * @brief  Converts manager from CUDD format
		 *
		 * Converts a pointer to manager from DdManager type to
		 * CUDDFacade::Manager type.
		 *
		 * @param[in]  cudd_value  CUDD DdManager pointer
		 *
		 * @returns  SFTA Manager pointer
		 */
		CUDDFacade::Manager* fromCUDD(DdManager* cudd_value)
		{
			return reinterpret_cast<CUDDFacade::Manager*>(cudd_value);
		}


		/**
		 * @brief  Converts node from CUDD format
		 *
		 * Converts a pointer to manager from DdNode type to
		 * CUDDFacade::Node type.
		 *
		 * @param[in]  cudd_value  CUDD DdNode pointer
		 *
		 * @returns  SFTA Node pointer
		 */
		CUDDFacade::Node* fromCUDD(DdNode* cudd_value)
		{
			return reinterpret_cast<CUDDFacade::Node*>(cudd_value);
		}


		/**
		 * @brief  Converts manager to CUDD format
		 *
		 * Converts a pointer to manager from CUDDFacade::Manager type to
		 * DdManager type.
		 *
		 * @param[in]  sfta_value  SFTA CUDDFacade::Manager pointer
		 *
		 * @returns  CUDD DdManager pointer
		 */
		DdManager* toCUDD(CUDDFacade::Manager* sfta_value)
		{
			return reinterpret_cast<DdManager*>(sfta_value);
		}


		/**
		 * @brief  Converts node to CUDD format
		 *
		 * Converts a pointer to manager from CUDDFacade::Node type to
		 * DdNode type.
		 *
		 * @param[in]  sfta_value  SFTA CUDDFacade::Node pointer
		 *
		 * @returns  CUDD DdNode pointer
		 */
		DdNode* toCUDD(CUDDFacade::Node* sfta_value)
		{
			return reinterpret_cast<DdNode*>(sfta_value);
		}
	}
}


CUDDFacade::CUDDFacade()
	: manager_(static_cast<Manager*>(0))
{
	// Create the manager
	if ((manager_ = fromCUDD(Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0)))
		== static_cast<Manager*>(0))
	{	// in case the manager could not be created
		std::string error_msg = "CUDD Manager could not be created";
		SFTA_LOGGER_FATAL(error_msg);
		throw std::runtime_error(error_msg);
	}
}


CUDDFacade::Node* CUDDFacade::AddIthVar(int i) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));
	assert(i >= 0);

	Node* res = fromCUDD(Cudd_addIthVar(toCUDD(manager_), i));

	// check the return value
	assert(res != static_cast<Node*>(0));

	return res;
}


SFTA::Private::CUDDFacade::Node* CUDDFacade::AddCmpl(Node* node) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));
	assert(node != static_cast<Node*>(0));

	Node* res = fromCUDD(Cudd_addCmpl(toCUDD(manager_), toCUDD(node)));

	// check the return value
	assert(res != static_cast<Node*>(0));

	return res;
}


CUDDFacade::Node* CUDDFacade::AddConst(CUDDFacade::ValueType value) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));

	Node* res = fromCUDD(Cudd_addConst(toCUDD(manager_), value));

	// check the return value
	assert(res != static_cast<Node*>(0));

	return res;
}


void CUDDFacade::Ref(Node* node) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));
	assert(node != static_cast<Node*>(0));

	Cudd_Ref(toCUDD(node));
}


void CUDDFacade::RecursiveDeref(Node* node) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));
	assert(node != static_cast<Node*>(0));

	Cudd_RecursiveDeref(toCUDD(manager_), toCUDD(node));
}


void CUDDFacade::SetBackground(Node* bck) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));
	assert(bck != static_cast<Node*>(0));

	throw std::logic_error("Setting background value of CUDD manager is not supported!");

	//Cudd_SetBackground(toCUDD(manager_), toCUDD(bck));
}


CUDDFacade::Node* CUDDFacade::ReadBackground() const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));

	Node* res = fromCUDD(Cudd_ReadBackground(toCUDD(manager_)));

	// check the return value
	assert(res != static_cast<Node*>(0));

	return res;
}


unsigned CUDDFacade::GetVarCount() const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));

	return toCUDD(manager_)->size;
}


unsigned CUDDFacade::GetDagSize(Node* node) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));

	int size = Cudd_DagSize(toCUDD(node));
	assert(size >= 0);

	return static_cast<unsigned>(size);
}


CUDDFacade::Node* CUDDFacade::Times(Node* lhs, Node* rhs) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));
	assert(!(cuddIsConstant(toCUDD(lhs)) && cuddIsConstant(toCUDD(rhs))));

// Note: This piece of code was used for testing the speed difference between
// the cached and uncached version of times operation
//
//	class TimesApplyFunctor : public AbstractApplyFunctor
//	{
//	public:
//		virtual ValueType operator()(const ValueType& lhs, const ValueType& rhs)
//		{
//			return lhs * rhs;
//		}
//
//	};
//
//	TimesApplyFunctor timesFunctor;
//
//	Node* res = Apply(lhs, rhs, &timesFunctor);

	Node* res = fromCUDD(Cudd_addApply(toCUDD(manager_), Cudd_addTimes,
		toCUDD(lhs), toCUDD(rhs)));

	// check the return value
	assert(res != static_cast<Node*>(0));

	return res;
}


DdNode* applyCallback(DdManager* dd, DdNode** f, DdNode** g, void* data)
{
	// Assertions
	assert(dd   != static_cast<DdManager*>(0));
	assert(f    != static_cast<DdNode**>(0));
	assert(g    != static_cast<DdNode**>(0));
	assert(data != static_cast<void*>(0));

	// get values of the nodes
	DdNode* F = *f;
	DdNode* G = *g;

	// Further assertions
	assert(F    != static_cast<DdNode*>(0));
	assert(G    != static_cast<DdNode*>(0));

	if (cuddIsConstant(F) && cuddIsConstant(G))
	{	// in case we are at leaves

		// get the functor from the container
		CUDDFacade::AbstractApplyFunctor& func =
			*(static_cast<CUDDFacade::AbstractApplyFunctor*>(data));

		DdNode* res = cuddUniqueConst(dd, func(cuddV(F), cuddV(G)));

		// check the return value
		assert(res != static_cast<DdNode*>(0));

		return res;
	}
	else
	{	// in case we are not at leaves
		return static_cast<DdNode*>(0);
	}
}


DdNode* monadicApplyCallback(DdManager* dd, DdNode* f, void* data)
{
	// Assertions
	assert(dd   != static_cast<DdManager*>(0));
	assert(f    != static_cast<DdNode*>(0));
	assert(data != static_cast<void*>(0));

	if (cuddIsConstant(f))
	{	// in case we are at leaves

		// get the functor from the container
		CUDDFacade::AbstractMonadicApplyFunctor& func =
			*(static_cast<CUDDFacade::AbstractMonadicApplyFunctor*>(data));

		DdNode* res = cuddUniqueConst(dd, func(cuddV(f)));

		// check the return value
		assert(res != static_cast<DdNode*>(0));

		return(res);
	}
	else
	{	// in case we are not at leaves
		return static_cast<DdNode*>(0);
	}
}


CUDDFacade::Node* CUDDFacade::Apply(Node* lhs, Node* rhs,
	AbstractApplyFunctor* func) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));
	assert(lhs != static_cast<Node*>(0));
	assert(rhs != static_cast<Node*>(0));
	assert(func != static_cast<AbstractApplyFunctor*>(0));

	Node* res = fromCUDD(Cudd_addApplyWithData(
		toCUDD(manager_), applyCallback, toCUDD(lhs), toCUDD(rhs), func));

	// check the return value
	assert(res != static_cast<Node*>(0));

	return res;
}


CUDDFacade::Node* CUDDFacade::MonadicApply(Node* root,
	AbstractMonadicApplyFunctor* func) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));
	assert(root != static_cast<Node*>(0));
	assert(func != static_cast<AbstractMonadicApplyFunctor*>(0));

	Node* res = fromCUDD(Cudd_addMonadicApplyWithData(
		toCUDD(manager_), monadicApplyCallback, toCUDD(root), func));

	// check the return value
	assert(res != static_cast<Node*>(0));

	return res;
}


CUDDFacade::Node* CUDDFacade::GetThenChild(Node* node) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));
	assert(!IsNodeConstant(node));

	return fromCUDD(cuddT(toCUDD(node)));
}


CUDDFacade::Node* CUDDFacade::GetElseChild(Node* node) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));
	assert(!IsNodeConstant(node));

	return fromCUDD(cuddE(toCUDD(node)));
}


bool CUDDFacade::IsNodeConstant(Node* node) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));

	return Cudd_IsConstant(node);
}


unsigned CUDDFacade::GetNodeIndex(Node* node) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));
	assert(!IsNodeConstant(node));

	return toCUDD(node)->index;
}


CUDDFacade::Node* CUDDFacade::ChangeVariableIndex(Node* root,
	unsigned oldIndex, unsigned newIndex) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));
	assert(root != static_cast<Node*>(0));

	if (IsNodeConstant(root))
	{	// in case the node is constant
		return root;
	}

	unsigned currentIndex = GetNodeIndex(root);
	if (currentIndex == oldIndex)
	{	// in case we hit a node with the desired variable index
		Node* thenChild = GetThenChild(root);
		Node* elseChild = GetElseChild(root);

		do
		{
			toCUDD(manager_)->reordered = 0;
			root = fromCUDD(cuddUniqueInter(toCUDD(manager_), newIndex,
				toCUDD(thenChild), toCUDD(elseChild)));
		} while (toCUDD(manager_)->reordered == 1);

		return root;
	}
	else
	{
		Node* thenChild = ChangeVariableIndex(GetThenChild(root),
			oldIndex, newIndex);
		Node* elseChild = ChangeVariableIndex(GetElseChild(root),
			oldIndex, newIndex);

		do
		{
			toCUDD(manager_)->reordered = 0;
			root = fromCUDD(cuddUniqueInter(toCUDD(manager_), currentIndex,
				toCUDD(thenChild), toCUDD(elseChild)));
		} while (toCUDD(manager_)->reordered == 1);

		return root;
	}
}


CUDDFacade::Node* CUDDFacade::RemoveVariables(Node* root,
	AbstractNodePredicateFunctor* predicate, AbstractApplyFunctor* merger) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));
	assert(root != static_cast<Node*>(0));
	assert(predicate != static_cast<AbstractNodePredicateFunctor*>(0));
	assert(merger != static_cast<AbstractApplyFunctor*>(0));

	if (IsNodeConstant(root))
	{	// in case the node is constant
		return root;
	}

	Node* thenChild = RemoveVariables(GetThenChild(root), predicate, merger);
	Node* elseChild = RemoveVariables(GetElseChild(root), predicate, merger);

	unsigned index = GetNodeIndex(root);
	if ((*predicate)(index))
	{	// in case the node is to be removed
		root = Apply(elseChild, thenChild, merger);

		return root;
	}
	else
	{	// in case the node is to stay
		do
		{
			toCUDD(manager_)->reordered = 0;
			root = fromCUDD(cuddUniqueInter(toCUDD(manager_), index,
				toCUDD(thenChild), toCUDD(elseChild)));
		} while (toCUDD(manager_)->reordered == 1);

		return root;
	}
}


CUDDFacade::ValueType CUDDFacade::GetNodeValue(Node* node) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));
	assert(IsNodeConstant(node));

	return toCUDD(node)->type.value;
}


std::string CUDDFacade::StoreToString(
	const StringNodeMapType& nodeDictionary) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));

	// arrays to be passed to the CUDD function
	DdNode** arrNodes   = static_cast<DdNode**>(0);
	char** arrRootNames = static_cast<char**>(0);
	char** arrVarNames  = static_cast<char**>(0);

	// fake FILE* manager
	FakeFile ff;

	SFTA_LOGGER_DEBUG("Storing a diagram with "
		+ Convert::ToString(nodeDictionary.size()) + " nodes to string");

	try
	{	// try block due to possible memory errors
		arrRootNames = new char*[nodeDictionary.size()];
		arrNodes = new DdNode*[nodeDictionary.size()];
		unsigned i = 0;

		for (StringNodeMapType::const_iterator itSN = nodeDictionary.begin();
			itSN != nodeDictionary.end(); ++itSN, ++i)
		{	// extract two arrays from the dictionary
			arrRootNames[i] = const_cast<char*>((itSN->first).c_str());
			arrNodes[i] = toCUDD(itSN->second);
		}

		std::vector<std::string> varNames;
		arrVarNames = new char*[GetVarCount()];
		for (size_t i = 0; i < GetVarCount(); ++i)
		{	// fill the array of variables
			varNames.push_back("x" + Convert::ToString(i));
			arrVarNames[i] = const_cast<char*>(varNames[i].c_str());
		}

		if (!Dddmp_cuddAddArrayStore(toCUDD(manager_), static_cast<char*>(0),
			nodeDictionary.size(), arrNodes, arrRootNames, arrVarNames, static_cast<int*>(0),
			DDDMP_MODE_TEXT, DDDMP_VARDEFAULT, static_cast<char*>(0), ff.OpenWrite()))
		{	// in case there was a problem with storing the BDD
			throw std::runtime_error("Could not store BDD to string!");
		}

		// delete the arrays
		delete [] arrNodes;
		arrNodes = static_cast<DdNode**>(0);
		delete [] arrRootNames;
		arrRootNames = static_cast<char**>(0);
		delete [] arrVarNames;
		arrVarNames = static_cast<char**>(0);
	}
	catch (const std::exception& e)
	{	// in case an exception appears, we log it
		if (arrNodes != static_cast<DdNode**>(0))
		{	// in case the array of nodes is still allocated
			delete [] arrNodes;
			arrNodes = static_cast<DdNode**>(0);
		}

		if (arrRootNames != static_cast<char**>(0))
		{	// in case the array of root nodes' names is still allocated
			delete [] arrRootNames;
			arrRootNames = static_cast<char**>(0);
		}

		if (arrVarNames != static_cast<char**>(0))
		{	// in case the array of variable nodes' names is still allocated
			delete [] arrVarNames;
			arrVarNames = static_cast<char**>(0);
		}

		SFTA_LOGGER_ERROR("Error while storing BDD to string: "
			+ Convert::ToString(e.what()));

		throw;
	}

	ff.Close();
	return ff.GetContent();
}


std::pair<CUDDFacade*, CUDDFacade::StringNodeMapType>
	CUDDFacade::LoadFromString(const std::string& str,
	const std::vector<std::string>& rootNames)
{
	CUDDFacade* facade = new CUDDFacade();

	StringNodeMapType nodeDict;

	// fake FILE* manager
	FakeFile ff;

	char** arrRootNames = new char*[rootNames.size()];

	for (unsigned i = 0; i < rootNames.size(); ++i)
	{	// copy root names
		arrRootNames[i] = new char[rootNames[i].length() + 1];
		strncpy(arrRootNames[i], rootNames[i].c_str(), rootNames[i].length());
		arrRootNames[i][rootNames[i].length()] = '\0';
	}

	char** varNames = static_cast<char**>(0);

	DdNode** roots = static_cast<DdNode**>(0);

	int rootCount = Dddmp_cuddAddArrayLoad(toCUDD(facade->manager_),
		DDDMP_ROOT_MATCHNAMES, arrRootNames, DDDMP_VAR_MATCHIDS, varNames,
		static_cast<int*>(0), static_cast<int*>(0), DDDMP_MODE_TEXT,
		static_cast<char*>(0), ff.OpenRead(str), &roots);

	if (rootCount <= 0)
	{	// in case there was an error loading the MTBDD
		throw std::runtime_error("Error loading MTBDD!");
	}

	for (int i = 0; i < rootCount; ++i)
	{	// read root nodes
		nodeDict[rootNames[i]] = fromCUDD(roots[i]);
	}

	for (unsigned i = 0; i < rootNames.size(); ++i)
	{	// delete root names
		delete [] arrRootNames[i];
	}

	delete [] arrRootNames;

	return std::make_pair(facade, nodeDict);
}


void CUDDFacade::DumpDot(const std::vector<Node*>& nodes,
		const std::vector<std::string>& rootNames,
		const std::vector<std::string>& sinkNames,
		const std::string& filename) const
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));
	assert((rootNames.size() == 0) || (nodes.size() == rootNames.size()));

	// arrays to be passed to the CUDD function
	DdNode** arrNodes   = static_cast<DdNode**>(0);
	char** arrRootNames = static_cast<char**>(0);
	char** arrVarNames  = static_cast<char**>(0);
	char** arrSinkNames = static_cast<char**>(0);

	SFTA_LOGGER_DEBUG("Dumping a diagram with "
		+ Convert::ToString(nodes.size()) + " nodes");

	// the file that the BDD should be dumped to
	FILE* outfile = static_cast<FILE*>(0);

	try
	{	// try block due to possible memory errors
		if ((outfile = fopen(filename.c_str(), "w")) == static_cast<FILE*>(0))
		{	// in case the file could not be open
			throw std::runtime_error("Could not open file!");
		}

		// create the array of nodes
		arrNodes = new DdNode*[nodes.size()];
		for (size_t i = 0; i < nodes.size(); ++i)
		{	// insert the nodes to the array
			arrNodes[i] = toCUDD(nodes[i]);
		}

		if (rootNames.size() > 0)
		{	// if there are names of roots
			arrRootNames = new char*[rootNames.size()];

			for (size_t i = 0; i < rootNames.size(); ++i)
			{	// copy names of roots
				arrRootNames[i] = const_cast<char*>(rootNames[i].c_str());
			}
		}

		if (sinkNames.size() > 0)
		{	// if there are names of sink nodes
			arrSinkNames = new char*[sinkNames.size()];

			for (size_t i = 0; i < sinkNames.size(); ++i)
			{	// copy names of sink nodes
				arrSinkNames[i] = const_cast<char*>(sinkNames[i].c_str());
			}
		}

		std::vector<std::string> varNames;
		arrVarNames = new char*[GetVarCount()];
		for (size_t i = 0; i < GetVarCount(); ++i)
		{	// fill the array of variables
			varNames.push_back("x" + Convert::ToString(i));
			arrVarNames[i] = const_cast<char*>(varNames[i].c_str());
		}

		if (!(Cudd_DumpDotWithArbitrarySinkNodes(
			toCUDD(manager_), nodes.size(), arrNodes, arrVarNames,
			arrRootNames, arrSinkNames, outfile)))
		{	// in case there was a problem with dumping the file
			throw std::runtime_error("Could not dump BDD!");
		}

		// delete the arrays
		delete [] arrNodes;
		arrNodes = static_cast<DdNode**>(0);
		delete [] arrRootNames;
		arrRootNames = static_cast<char**>(0);
		delete [] arrSinkNames;
		arrSinkNames = static_cast<char**>(0);
		delete [] arrVarNames;
		arrVarNames = static_cast<char**>(0);

		if (!(fclose(outfile)))
		{	// in case everything was alright
			outfile = static_cast<FILE*>(0);
		}
		else
		{	// in case there was a problem with closing the file
			outfile = static_cast<FILE*>(0);
			throw std::runtime_error("Could not close the dump file!");
		}
	}
	catch (const std::exception& e)
	{	// in case an exception appears, we only log it
		if (outfile != static_cast<FILE*>(0))
		{	// in case the file has not been closed yet
			fclose(outfile);
			outfile = static_cast<FILE*>(0);
		}

		if (arrNodes != static_cast<DdNode**>(0))
		{	// in case the array of nodes is still allocated
			delete [] arrNodes;
			arrNodes = static_cast<DdNode**>(0);
		}

		if (arrRootNames != static_cast<char**>(0))
		{	// in case the array of root nodes' names is still allocated
			delete [] arrRootNames;
			arrRootNames = static_cast<char**>(0);
		}

		if (arrSinkNames != static_cast<char**>(0))
		{	// in case the array of sink nodes' names is still allocated
			delete [] arrSinkNames;
			arrSinkNames = static_cast<char**>(0);
		}

		if (arrVarNames != static_cast<char**>(0))
		{	// in case the array of variable nodes' names is still allocated
			delete [] arrVarNames;
			arrVarNames = static_cast<char**>(0);
		}

		SFTA_LOGGER_ERROR("Error while dumping BDD to file \"" + filename + "\": " + e.what());
	}
}


CUDDFacade::~CUDDFacade()
{
	// Assertions
	assert(manager_ != static_cast<Manager*>(0));

	// Derefence the background
	RecursiveDeref(ReadBackground());

	// Check for nodes with non-zero reference count
	int unrefed = 0;
	if ((unrefed = Cudd_CheckZeroRef(toCUDD(manager_))) != 0)
	{	// in case there are still some nodes unreferenced
		SFTA_LOGGER_WARN("Still " + Convert::ToString(unrefed) + " nodes unreferenced!");
	}

	// Delete the manager
	Cudd_Quit(toCUDD(manager_));
	manager_ = static_cast<Manager*>(0);
}
