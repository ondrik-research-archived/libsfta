/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Source file for sfta program, which is an interface to libSFTA.
 *
 *****************************************************************************/


// Standard library headers
#include <cstdlib>
#include <ctime>
#include <getopt.h>
#include <fstream>
#include <iostream>

// Log4cpp headers
#include <log4cpp/Category.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/BasicLayout.hh>

// SFTA library headers
#include <sfta/bu_tree_automaton_cover.hh>
#include <sfta/convert.hh>
#include <sfta/ta_building_director.hh>
#include <sfta/td_tree_automaton_cover.hh>
#include <sfta/timbuk_bu_ta_builder.hh>
#include <sfta/timbuk_td_ta_builder.hh>



typedef SFTA::BUTreeAutomatonCover<64> BUTreeAutomaton;
typedef SFTA::TDTreeAutomatonCover<64> TDTreeAutomaton;

typedef SFTA::TABuildingDirector<BUTreeAutomaton> BUTABuildingDirector;
typedef SFTA::TABuildingDirector<TDTreeAutomaton> TDTABuildingDirector;

typedef SFTA::AbstractTABuilder<BUTreeAutomaton> AbstractBUTABuilder;
typedef SFTA::AbstractTABuilder<TDTreeAutomaton> AbstractTDTABuilder;

typedef SFTA::TimbukBUTABuilder<BUTreeAutomaton> TimbukBUTABuilder;
typedef SFTA::TimbukTDTABuilder<TDTreeAutomaton> TimbukTDTABuilder;

typedef SFTA::Private::Convert Convert;

enum OperationType
{
	OPERATION_INVALID = 0,
	OPERATION_UNION,
	OPERATION_INTERSECTION,
	OPERATION_LOAD,
	OPERATION_SIMULATION,
	OPERATION_INCLUSION,

	OPERATION_HELP,

	OPERATION_LAST            // just for checking boundary
};

void printHelp(const std::string& programName)
{
	std::cout << "usage: " << programName << " (-l|--load)         <file1>\n";
	std::cout << "   or: " << programName << " (-u|--union)        <file1> <file2>\n";
	std::cout << "   or: " << programName << " (-i|--intersection) <file1> <file2>\n";
	std::cout << "\n";
	std::cout << "    -l, --load             load an automaton from <file1>.\n";
	std::cout << "    -u, --union            create an automaton with language that is the union\n";
	std::cout << "                           of languages of automata from <file1> and <file2>.\n";
	std::cout << "    -i, --intersection     create an automaton with language that is the\n";
	std::cout << "                           intersection of languages of automata from <file1>\n";
	std::cout << "                           and <file2>.\n";
}

void needsArguments(size_t value, size_t needsToBe)
{
	if (value != needsToBe)
	{
		throw std::runtime_error("The operation needs " +
			Convert::ToString(needsToBe) + " arguments.");
	}
}


void specifyOperation(OperationType& oper, OperationType value)
{
	// Assertions
	assert((value > OPERATION_INVALID) && (value < OPERATION_LAST));

	if (oper != OPERATION_INVALID)
	{
		throw std::runtime_error("Cannot specify more operations.");
	}

	oper = value;
}


void performUnion(bool isTopDown, const std::string& lhsFile,
	const std::string& rhsFile)
{
	std::ifstream ifsLhs(lhsFile.c_str());
	if (ifsLhs.fail())
	{
		throw std::runtime_error("Could not open file " + lhsFile);
	}

	std::ifstream ifsRhs(rhsFile.c_str());
	if (ifsRhs.fail())
	{
		throw std::runtime_error("Could not open file " + rhsFile);
	}

	if (!isTopDown)
	{
		std::auto_ptr<AbstractBUTABuilder> builder(new TimbukBUTABuilder());
		BUTABuildingDirector director(builder.get());

		std::auto_ptr<BUTreeAutomaton> taLhs(director.Construct(ifsLhs));
		std::auto_ptr<BUTreeAutomaton> taRhs(director.Construct(ifsRhs));

		std::auto_ptr<BUTreeAutomaton::Operation> op(taLhs->GetOperation());

		std::auto_ptr<BUTreeAutomaton> taUnion(op->Union(taLhs.get(), taRhs.get()));

		std::cout << taUnion->ToString();
	}
	else
	{
		std::auto_ptr<AbstractTDTABuilder> builder(new TimbukTDTABuilder());
		TDTABuildingDirector director(builder.get());

		std::auto_ptr<TDTreeAutomaton> taLhs(director.Construct(ifsLhs));
		std::auto_ptr<TDTreeAutomaton> taRhs(director.Construct(ifsRhs));

		std::auto_ptr<TDTreeAutomaton::Operation> op(taLhs->GetOperation());

		std::auto_ptr<TDTreeAutomaton> taUnion(op->Union(taLhs.get(), taRhs.get()));

		std::cout << taUnion->ToString();
	}
}


void performIntersection(bool isTopDown, const std::string& lhsFile,
	const std::string& rhsFile)
{
	std::ifstream ifsLhs(lhsFile.c_str());
	if (ifsLhs.fail())
	{
		throw std::runtime_error("Could not open file " + lhsFile);
	}

	std::ifstream ifsRhs(rhsFile.c_str());
	if (ifsRhs.fail())
	{
		throw std::runtime_error("Could not open file " + rhsFile);
	}

	if (!isTopDown)
	{
		std::auto_ptr<AbstractBUTABuilder> builder(new TimbukBUTABuilder());
		BUTABuildingDirector director(builder.get());

		std::auto_ptr<BUTreeAutomaton> taLhs(director.Construct(ifsLhs));
		std::auto_ptr<BUTreeAutomaton> taRhs(director.Construct(ifsRhs));

		std::auto_ptr<BUTreeAutomaton::Operation> op(taLhs->GetOperation());


		//clock_t start = clock();
		std::auto_ptr<BUTreeAutomaton> taUnion(op->Intersection(taLhs.get(), taRhs.get()));
		//clock_t finish = clock();
		//SFTA_LOGGER_INFO("Duration: " + Convert::ToString(static_cast<double>(finish - start) / CLOCKS_PER_SEC) + " s");

		std::cout << taUnion->ToString();
	}
	else
	{
		std::auto_ptr<AbstractTDTABuilder> builder(new TimbukTDTABuilder());
		TDTABuildingDirector director(builder.get());

		std::auto_ptr<TDTreeAutomaton> taLhs(director.Construct(ifsLhs));
		std::auto_ptr<TDTreeAutomaton> taRhs(director.Construct(ifsRhs));

		std::auto_ptr<TDTreeAutomaton::Operation> op(taLhs->GetOperation());

		std::auto_ptr<TDTreeAutomaton> taUnion(op->Intersection(taLhs.get(), taRhs.get()));

		std::cout << taUnion->ToString();
	}
}


void performLoad(bool isTopDown, const std::string& file)
{
	std::ifstream ifs(file.c_str());
	if (ifs.fail())
	{
		throw std::runtime_error("Could not open file " + file);
	}

	if (!isTopDown)
	{
		std::auto_ptr<AbstractBUTABuilder> builder(new TimbukBUTABuilder());
		BUTABuildingDirector director(builder.get());

		std::auto_ptr<BUTreeAutomaton> ta(director.Construct(ifs));

		std::cout << ta->ToString();
	}
	else
	{
		std::auto_ptr<AbstractTDTABuilder> builder(new TimbukTDTABuilder());
		TDTABuildingDirector director(builder.get());

		std::auto_ptr<TDTreeAutomaton> ta(director.Construct(ifs));

		std::cout << ta->ToString();
	}
}


void performComputationOfSimulation(bool isTopDown, const std::string& file)
{
	std::ifstream ifs(file.c_str());
	if (ifs.fail())
	{
		throw std::runtime_error("Could not open file " + file);
	}

	if (!isTopDown)
	{
		std::auto_ptr<AbstractBUTABuilder> builder(new TimbukBUTABuilder());
		BUTABuildingDirector director(builder.get());

		std::auto_ptr<BUTreeAutomaton> ta(director.Construct(ifs));

		std::auto_ptr<BUTreeAutomaton::Operation> op(ta->GetOperation());

		typedef BUTreeAutomaton::SimulationRelationType SimulationRelationType;

		SimulationRelationType sim = op->ComputeSimulationPreorder(ta.get());

		std::string resultString = Convert::ToString(sim);

		std::cout << resultString << "\n";
	}
	else
	{
		assert(false);
	}
}


void performCheckingInclusion(bool isTopDown, const std::string& lhsFile,
	const std::string& rhsFile)
{
	std::ifstream ifsLhs(lhsFile.c_str());
	if (ifsLhs.fail())
	{
		throw std::runtime_error("Could not open file " + lhsFile);
	}

	std::ifstream ifsRhs(rhsFile.c_str());
	if (ifsRhs.fail())
	{
		throw std::runtime_error("Could not open file " + rhsFile);
	}

	if (!isTopDown)
	{
		std::auto_ptr<AbstractBUTABuilder> builder(new TimbukBUTABuilder());
		BUTABuildingDirector director(builder.get());

		std::auto_ptr<BUTreeAutomaton> taLhs(director.Construct(ifsLhs));
		std::auto_ptr<BUTreeAutomaton> taRhs(director.Construct(ifsRhs));

		std::auto_ptr<BUTreeAutomaton::Operation> op(taLhs->GetOperation());

		std::cout << (op->DoesLanguageInclusionHold(taLhs.get(), taRhs.get())? "true"
			: "false")  << "\n";
	}
	else
	{
		assert(false);
	}
}


void startLogger()
{
	// create the appender
	log4cpp::Appender* app1  = new log4cpp::OstreamAppender("ClogAppender", &std::clog);

	// log categery
	std::string cat_name = "SFTA";

	// set verbosity level etc.
	log4cpp::Category::getInstance(cat_name).setAdditivity(false);
	log4cpp::Category::getInstance(cat_name).addAppender(app1);
	log4cpp::Category::getInstance(cat_name).setPriority(log4cpp::Priority::INFO);
}


int main(int argc, char* argv[])
{
	// Assertions
	assert(argc >= 1);

	try
	{
		startLogger();

		const char* getoptString = "uihlbtsn";
		option longOptions[] = {
			{"union",                      0, static_cast<int*>(0), 'u'},
			{"intersection",               0, static_cast<int*>(0), 'i'},
			{"help",                       0, static_cast<int*>(0), 'h'},
			{"load",                       0, static_cast<int*>(0), 'l'},
			{"bottom-up",                  0, static_cast<int*>(0), 'b'},
			{"top-down",                   0, static_cast<int*>(0), 't'},
			{"simulation",                 0, static_cast<int*>(0), 's'},
			{"inclusion",                  0, static_cast<int*>(0), 'n'},

			{static_cast<const char*>(0),  0, static_cast<int*>(0), 0}
		};

		OperationType operation = OPERATION_INVALID;
		bool isTopDown = false;

		int opt, optIndex;
		while ((opt = getopt_long(argc, argv,
			getoptString, longOptions, &optIndex)) != -1)
		{
			switch (opt)
			{
				case 'u': specifyOperation(operation, OPERATION_UNION); break;
				case 'i': specifyOperation(operation, OPERATION_INTERSECTION); break;
				case 'h': specifyOperation(operation, OPERATION_HELP); break;
				case 'l': specifyOperation(operation, OPERATION_LOAD); break;
				case 's': specifyOperation(operation, OPERATION_SIMULATION); break;
				case 'n': specifyOperation(operation, OPERATION_INCLUSION); break;
				case 'b': isTopDown = false; break;
				case 't': isTopDown = true; break;
				default: throw std::runtime_error("Invalid command line parameter."); break;
			}
		}

		if (operation == OPERATION_INVALID)
		{
			throw std::runtime_error("Invalid command line parameters.");
		}

		typedef std::vector<std::string> StringVector;
		StringVector inputs;

		for (int i = optind; i < argc; ++i)
		{
			assert(argv[i] != static_cast<const char*>(0));

			inputs.push_back(argv[i]);
		}


		switch (operation)
		{
			case OPERATION_HELP:
				printHelp(argv[0]);
				break;

			case OPERATION_UNION:
				needsArguments(inputs.size(), 2);
				performUnion(isTopDown, inputs[0], inputs[1]);
				break;

			case OPERATION_INTERSECTION:
				needsArguments(inputs.size(), 2);
				performIntersection(isTopDown, inputs[0], inputs[1]);
				break;

			case OPERATION_LOAD:
				needsArguments(inputs.size(), 1);
				performLoad(isTopDown, inputs[0]);
				break;

			case OPERATION_SIMULATION:
				needsArguments(inputs.size(), 1);
				performComputationOfSimulation(isTopDown, inputs[0]);
				break;

			case OPERATION_INCLUSION:
				needsArguments(inputs.size(), 2);
				performCheckingInclusion(isTopDown, inputs[0], inputs[1]);
				break;

			default: throw std::runtime_error("Invalid operation type.");break;
		}
	}
	catch (std::exception& ex)
	{
		std::cerr << "An error occured: " << ex.what() << "\n";
		std::cerr << "Run " << argv[0] << " -h   for detailed help.\n";

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
