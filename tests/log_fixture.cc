/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Implementation of a fixture class that enables logging shared among test
 *    suites.
 *
 *****************************************************************************/

// testing header files
#include "log_fixture.hh"

// initialization of the static variable
bool LogFixture::logInitialized_ = false;
