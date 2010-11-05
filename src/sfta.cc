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
#include <sfta/timbuk_ta_builder.hh>



typedef SFTA::BUTreeAutomatonCover<64> TreeAutomaton;

typedef SFTA::TABuildingDirector<TreeAutomaton> TABuildingDirector;
typedef SFTA::AbstractTABuilder<TreeAutomaton> AbstractTABuilder;
typedef SFTA::TimbukTABuilder<TreeAutomaton> TimbukTABuilder;

typedef SFTA::Private::Convert Convert;

enum OperationType
{
	OPERATION_INVALID = 0,
	OPERATION_UNION,
	OPERATION_INTERSECTION,
	OPERATION_LOAD,

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


void performUnion(const std::string& lhsFile, const std::string& rhsFile)
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

	std::auto_ptr<AbstractTABuilder> builder(new TimbukTABuilder());
	TABuildingDirector director(builder.get());

	std::auto_ptr<TreeAutomaton> taLhs(director.Construct(ifsLhs));
	std::auto_ptr<TreeAutomaton> taRhs(director.Construct(ifsRhs));

	std::auto_ptr<TreeAutomaton::Operation> op(taLhs->GetOperation());

	std::auto_ptr<TreeAutomaton> taUnion(op->Union(taLhs.get(), taRhs.get()));

	std::cout << taUnion->ToString();
}


void performIntersection(const std::string& lhsFile, const std::string& rhsFile)
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

	std::auto_ptr<AbstractTABuilder> builder(new TimbukTABuilder());
	TABuildingDirector director(builder.get());

	std::auto_ptr<TreeAutomaton> taLhs(director.Construct(ifsLhs));
	std::auto_ptr<TreeAutomaton> taRhs(director.Construct(ifsRhs));

	std::auto_ptr<TreeAutomaton::Operation> op(taLhs->GetOperation());


	clock_t start = clock();
	std::auto_ptr<TreeAutomaton> taUnion(op->Intersection(taLhs.get(), taRhs.get()));
	clock_t finish = clock();
	SFTA_LOGGER_INFO("Duration: " + Convert::ToString(static_cast<double>(finish - start) / CLOCKS_PER_SEC) + " s");

	std::cout << taUnion->ToString();
}


void performLoad(const std::string& file)
{
	std::ifstream ifs(file.c_str());
	if (ifs.fail())
	{
		throw std::runtime_error("Could not open file " + file);
	}

	std::auto_ptr<AbstractTABuilder> builder(new TimbukTABuilder());
	TABuildingDirector director(builder.get());

	std::auto_ptr<TreeAutomaton> ta(director.Construct(ifs));

	std::cout << ta->ToString();
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

		const char* getoptString = "uihl";
		option longOptions[] = {
			{"union",                      0, static_cast<int*>(0), 'u'},
			{"intersection",               0, static_cast<int*>(0), 'i'},
			{"help",                       0, static_cast<int*>(0), 'h'},
			{"load",                       0, static_cast<int*>(0), 'l'},
			{static_cast<const char*>(0),  0, static_cast<int*>(0), 0}
		};

		OperationType operation = OPERATION_INVALID;

		int opt, optIndex;
		while ((opt = getopt_long(argc, argv,
			getoptString, longOptions, &optIndex)) != -1)
		{
			switch (opt)
			{
				case 'h': specifyOperation(operation, OPERATION_HELP); break;
				case 'u': specifyOperation(operation, OPERATION_UNION); break;
				case 'i': specifyOperation(operation, OPERATION_INTERSECTION); break;
				case 'l': specifyOperation(operation, OPERATION_LOAD); break;
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
				performUnion(inputs[0], inputs[1]);
				break;

			case OPERATION_INTERSECTION:
				needsArguments(inputs.size(), 2);
				performIntersection(inputs[0], inputs[1]);
				break;

			case OPERATION_LOAD:
				needsArguments(inputs.size(), 1);
				performLoad(inputs[0]);
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
