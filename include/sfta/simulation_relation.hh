/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Header file for class with simulation relation.
 *
 *****************************************************************************/

#ifndef _SIMULATION_RELATION_HH_
#define _SIMULATION_RELATION_HH_

// Standard library headers
#include <set>
#include <tr1/unordered_map>




// insert the class into proper namespace
namespace SFTA
{
	template
	<
		typename State
	>
	class SimulationRelation;
}


template
<
	typename State
>
class SFTA::SimulationRelation
{
private:  // Private data types


	/**
	 * Data type of states.
	 */
	typedef State StateType;


	/**
	 * Data type for a row of the relation matrix.
	 */
	typedef std::set<StateType> RowType;


	/**
	 * Data type for the relation matrix.
	 */
	typedef std::tr1::unordered_map<StateType, RowType> MatrixType;

public:   // Public data types

	typedef SimulationRelation<StateType> Type;

	typedef std::pair<const StateType, StateType> value_type;


private:  // Private data members


	MatrixType matrix_;


private:  // Private methods

	RowType& getRow(const StateType& column)
	{
		typename MatrixType::iterator itMatrix;
		if ((itMatrix = matrix_.find(column)) == matrix_.end())
		{	// in case the value for given state has not been initialized yet
			itMatrix = matrix_.insert(matrix_.begin(),
				std::make_pair(column, RowType()));
		}

		return itMatrix->second;
	}

public:   // Public methods

	inline void insert(const value_type& value)
	{
		getRow(value.first).insert(value.second);
	}

	inline void erase(const value_type& value)
	{
		getRow(value.first).erase(value.second);
	}

	bool is_in(const value_type& value) const
	{
		typename MatrixType::const_iterator itMatrix;
		if ((itMatrix = matrix_.find(value.first)) == matrix_.end())
		{
			return false;
		}

		return (itMatrix->second).find(value.second) != (itMatrix->second).end();
	}

	const std::set<StateType>& GetSimulators(const StateType& state) const
	{
		return const_cast<Type*>(this)->getRow(state);
	}
};

#endif
