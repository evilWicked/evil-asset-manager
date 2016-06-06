#pragma once

#include "imp/signal-base.h"
#include "imp/slot-base.h"


namespace evil {

	/**
	@brief The thread safe specialisation of the underlying SlotBase class.

	The ThreadSlot class is intended to be used in conjunction with the ThreadSignal class. This version
	provides a threadsafe specialisation of the underlying signals and slots mechanism.  What this guarantees
	is that you can interact with both the signal and slot from mutilple threads simultaneously without conflict.
	
	It does not solve all the problems related to threading.

	In particular if you grab a pointer to associated signal from the slot using slot.signal() and wish to use 
	it you will still need to ensure that no other thread has deleted the slot while you are holding it.

	*/
	template<typename ...Args>
	class ThreadSlot : public SlotBase {

		std::function<void(Args...)> mFunction;

	protected:


	public:
		///Constructor
		///@param name  Optionally provide a name for this slot
		ThreadSlot(const char* name = NO_NAME) :SlotBase(true,name) {
		};

		//attach a function or lambda to the slot,
		///@param f a function with arguments defined by template instantiation. 
		//the signal and associated slots.
		void attach(std::function<void(Args...)> f) {
			std::lock_guard<std::mutex> lock(mMutex);
			mFunction = f;
		};

		//attach a class member to the slot,
		///@param c an instance of class C that will be used to call the member function.
		///@param f a class member function with parameters compatible with the templated parameters
		///used to define the signal and associated slots.
		template <typename C>
		void attach_class(C* c, void(C:: *f)(Args...)) {
			std::lock_guard<std::mutex> lock(mMutex);
			mFunction = util::bind_class(c, f);
		};

		///Triggers the slot passing in the arguments defined in the template. This does not provide lock_guards
		///which are provided by the signal
		///@param args  As defined in the template instantiation.
		void triggerSlot(Args... args) {
			mFunction(args...);
			if(mbFireOnce) {
				mbFireOnceCompleted = true;
				mbActive = false;
			}
		};

		bool hasFunction() { return !!mFunction; }
	public:
		//Hide and disable/default all constructors and destructors unless specifically overridden
		//this is a mnemonic to force me to think about things. If overridden they are commented
		//ThreadSlot() = default;
		~ThreadSlot() = default;
		ThreadSlot(const ThreadSlot& rhs) = delete;
		ThreadSlot& operator=(const ThreadSlot& rhs) = delete;
		ThreadSlot(ThreadSlot&& other) = delete;
		ThreadSlot& operator=(ThreadSlot&& other) = delete;
	};
};

