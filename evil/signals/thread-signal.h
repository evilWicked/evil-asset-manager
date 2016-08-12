#pragma once

#include "thread-slot.h"
#include "imp/signal-base.h"
#include "../thread/read-write-lock.h"

namespace evil {

	//we will assume while we are in the evil namespace that other evil things know they are evil and 
	//don't need to be prefixed with evil to access them. 

	/**
	@brief The thread safe specialisation of the underlying SignalBase class.

	The ThreadSignal class is intended to be used in conjunction with the ThreadSlot class. This version
	provides a threadsafe specialisation of the underlying signals and slots mechanism.  What this guarantees
	is that you can interact with both the signal and slot from mutilple threads simultaneously without conflict.

	*/
	template<typename ...Args>
	class ThreadSignal :public SignalBase {

	public:
		///Constructor. ThreadSignal is the thread safe variation of evil::Signal The interface is identical to 
		///evil::Signal and in fact they both derive from the same base class.
		ThreadSignal(const char* name = NO_NAME) :SignalBase(true,name) {};

		void dispatch(Args... args) {

			bool bHasCompletedSlots = false;

			ReadWriteLock lock(&mRWMutex, ReadWriteLock::READ);
			for(auto& x : mlistSlots) {
				ThreadSlot<Args...> *slot = static_cast<ThreadSlot<Args...>*>(x);
				std::lock_guard<std::mutex> slot_lock(slotMutex(slot));

				if(slot->isActive() && slot->hasFunction()) {
					slot->triggerSlot(args...);
					if(slot->isFireOnce()) {
						bHasCompletedSlots = true;
					}
				}
			}

			
			if(bHasCompletedSlots) {
				removeCompletedSlots();
				bHasCompletedSlots = false;
			}

		}

	public:
		//Hide and disable/default all constructors and destructors unless specifically overridden
		//this is a mnemonic to force me to think about things. if overridden they are commented
		//ThreadSignal() = default;
		~ThreadSignal() = default;
		ThreadSignal(const ThreadSignal& rhs) = delete;
		ThreadSignal& operator=(const ThreadSignal& rhs) = delete;
		ThreadSignal(ThreadSignal&& other) = delete;
		ThreadSignal& operator=(ThreadSignal&& other) = delete;
	};
};
