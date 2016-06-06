#pragma once

#include "imp/signal-base.h"
#include "../thread/read-write-lock.h"

namespace evil {

	//we will assume while we are in the evil namespace that other evil things know they are evil and 
	//don't need to be prefixed with evil to be evil. 

	/**
	@brief The single threaded specialisation of the SignalBase class. 
	
	This class is intended to be used in conjunction with the evil::Slot class.
	*/
	template<typename ...Args>
	class Signal :public SignalBase {
	
	public:
		///You can also optionally give the signal a name. It is not used internally but may be useful.
		Signal(const char* name = NO_NAME):SignalBase(false,name) {};
	
		///dispatch a signal that accepts the arguments used in the template instantiation
		void dispatch(Args... args) {

			bool bHasCompletedSlots = false;

			for(auto& x : mlistSlots) {
				Slot<Args...> *slot = static_cast<Slot<Args...>*>(x);
				if(slot->isActive() && slot->hasFunction()) {
					slot->triggerSlot(args...);
					if(slot->isFireOnce()) {
						bHasCompletedSlots = true;
					}
				}
			}
				
			if (bHasCompletedSlots) {
				removeCompletedSlots();
				bHasCompletedSlots = false;
			}

		}

	public:
		//Hide and disable/default all constructors and destructors unless specifically overridden
		//this is a mnemonic to force me to think about things. if overridden they are commented
		//Signal() = default;
		~Signal() = default;
		Signal(const Signal& rhs) = delete;
		Signal& operator=(const Signal& rhs) = delete;
		Signal(Signal&& other) = delete;
		Signal& operator=(Signal&& other) = delete;
	};
};
