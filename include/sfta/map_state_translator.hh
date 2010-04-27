/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    The file with the MapStateTranslator class.
 *
 *****************************************************************************/

#ifndef _SFTA_MAP_STATE_TRANSLATOR_HH_
#define _SFTA_MAP_STATE_TRANSLATOR_HH_

// Standard library header files
#include <map>


// insert the class into proper namespace
namespace SFTA
{
	namespace Private
	{
		template
		<
			typename AutomatonState,
			typename TransitionFunctionState
		>
		class MapStateTranslator;
	}
}


template
<
	typename AutomatonState,
	typename TransitionFunctionState
>
class SFTA::Private::MapStateTranslator
{
private:  // Private data types

	typedef TransitionFunctionState TFStateType;
	typedef AutomatonState AStateType;


	typedef std::map<AStateType, TFStateType>
		A2TFTableType;
	typedef std::map<TFStateType, AStateType>
		TF2ATableType;

	typedef std::set<TFStateType> SetOfTFStatesType;

private:  // Private data members

	A2TFTableType a2tf_;
	TF2ATableType tf2a_;

	SetOfTFStatesType finalStates_;

	/**
	 * @brief  The name of the Log4cpp category for logging
	 *
	 * The name of the Log4cpp category used for logging messages from this
	 * class.
	 */
	static const char* LOG_CATEGORY_NAME;


protected:// Protected methods

	MapStateTranslator()
		: a2tf_(),
		  tf2a_(),
			finalStates_()
	{ }

public:   // Public methods


	void AddTranslation(const AStateType& autState, const TFStateType& tfState)
	{
		if (!a2tf_.insert(std::make_pair(autState, tfState)).second
			|| !tf2a_.insert(std::make_pair(tfState, autState)).second)
		{	// in case there was an attempt to overwrite previous value
			throw std::runtime_error("An attempt to overwrite existing state.");
		}

		SFTA_LOGGER_DEBUG("Added state translation: " + Convert::ToString(autState)
			+ " <-> " + Convert::ToString(tfState));
	}


	AStateType TranslateTF2A(const TFStateType& tfState) const
	{
		typename TF2ATableType::const_iterator it;
		if ((it = tf2a_.find(tfState)) == tf2a_.end())
		{	// in case the translation could not be found
			assert(false);
			throw std::runtime_error("Cannot find translation TF2A.");
		}

		return it->second;
	}


	TFStateType TranslateA2TF(const AStateType& aState) const
	{
		typename A2TFTableType::const_iterator it;
		if ((it = a2tf_.find(aState)) == a2tf_.end())
		{	// in case the translation could not be found
			throw std::runtime_error("Cannot find translation A2TF.");
		}

		return it->second;
	}


	inline bool ContainsTFState(const TFStateType& tfState) const
	{
		return tf2a_.find(tfState) != tf2a_.end();
	}

	inline size_t Size() const
	{
		return a2tf_.size();
	}


	void SetTFStateFinal(const TFStateType& tfState)
	{
		finalStates_.insert(tfState);
	}


};


// Setting the logging category name for Log4cpp
template
<
	typename AS,
	typename TFS
>
const char* SFTA::Private::MapStateTranslator<AS, TFS>::LOG_CATEGORY_NAME
	= "map_state_translator";

#endif
