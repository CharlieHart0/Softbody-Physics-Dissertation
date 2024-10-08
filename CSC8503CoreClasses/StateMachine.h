#pragma once
#include <map>
#include <vector>

namespace NCL {
	namespace CSC8503 {
		class State;
		class StateTransition;

		typedef std::multimap<State*, StateTransition*> TransitionContainer;
		typedef TransitionContainer::iterator TransitionIterator;

		class StateMachine	{
		public:
			StateMachine();
			virtual ~StateMachine(); //made it virtual!

			void AddState(State* s);
			void AddTransition(StateTransition* t);

			virtual void Update(float dt); //made it virtual!

			bool hasNewState();
			bool hasActiveState(State * s);

		protected:
			State * activeState;
			State * prevState;
			std::vector<State*> allStates;
			TransitionContainer allTransitions;
		};
	}
}