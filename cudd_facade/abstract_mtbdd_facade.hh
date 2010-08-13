/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Header file for AbstractMTBDDFacade class.
 *
 *****************************************************************************/

#ifndef _SFTA_ABSTRACT_MTBDD_FACADE_HH_
#define _SFTA_ABSTRACT_MTBDD_FACADE_HH_

// insert the class into proper namespace
namespace SFTA
{
	namespace Private
	{
		template <class Leaf>
		class AbstractMTBDDFacade;
	}
}


/**
 * @brief   Abstract class for arbitrary MTBDD facade
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * This class serves as the base class for facade over arbitrary MTBDD
 * package.
 *
 * @tparam  Leaf  Type for leaves of the MTBDD.
 */
template
<
	class Leaf
>
class SFTA::Private::AbstractMTBDDFacade
{
public:   // public data types


	/**
	 * @brief  Leaf type
	 *
	 * The type for leaves of the MTBDD.
	 */
	typedef Leaf LeafType;


	/**
	 * @brief  The abstract class for an MTBDD Apply functor
	 *
	 * This class is an abstract class that defines the interface that all
	 * functors that perform binary Apply operation on an MTBDD need to
	 * implement.
	 */
	class AbstractApplyFunctor
	{
	public:   // Public methods

		/**
		 * @brief  Constructor
		 *
		 * The constructor of the class
		 */
		AbstractApplyFunctor()
		{ }

		/**
		 * @brief  The operator
		 *
		 * The operator of the functor, that is the method that performs the
		 * operation.
		 *
		 * @param[in]  lhs  Left-hand operand of the operation
		 * @param[in]  rhs  Right-hand operand of the operation
		 *
		 * @returns  Result of the operation
		 */
		virtual LeafType operator()(const LeafType& lhs, const LeafType& rhs) = 0;

		/**
		 * @brief  Destructor
		 *
		 * Virtual destructor.
		 */
		virtual ~AbstractApplyFunctor()
		{ }
	};


	/**
	 * @brief  The abstract class for an MTBDD monadic Apply functor
	 *
	 * This class is an abstract class that defines the interface that all
	 * functors that perform monadic Apply operation on an MTBDD need to
	 * implement.
	 */
	class AbstractMonadicApplyFunctor
	{
	public:   // Public methods


		/**
		 * @brief  Constructor
		 *
		 * The constructor of the class
		 */
		AbstractMonadicApplyFunctor()
		{ }


		/**
		 * @brief  The operator
		 *
		 * The operator of the functor, that is the method that performs the
		 * operation.
		 *
		 * @param[in]  val  The operand of the operation
		 *
		 * @returns  Result of the operation
		 */
		virtual LeafType operator()(const LeafType& val) = 0;


		/**
		 * @brief  Destructor
		 *
		 * Virtual destructor
		 */
		virtual ~AbstractMonadicApplyFunctor()
		{ }
	};


protected:// protected methods

	/**
	 * @brief  Constructor
	 *
	 * Protected default contructor. Being protected only enables calling from
	 * derived classes and not creating an instance on its own.
	 */
	AbstractMTBDDFacade()
	{	}

};

#endif
