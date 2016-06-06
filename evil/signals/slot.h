#pragma once

#include "imp/signal-base.h"
#include "imp/slot-base.h"


namespace evil {
	
	/**	@brief The single threaded specialisation of the SlotBase class. 
		
		This class is intended to be used in conjunction with the evil::Signal class. Creates a slot that
		you attach callbacks to that with accept the arguments defined in the template instantiation.
	*/
	template<typename ...Args>
	class Slot : public SlotBase {
		
		std::function<void(Args...)> mFunction;

	protected:
		

	public:
		///Constructor
		///@param name  Optionally provide this slot with a name.
		Slot(const char* name = NO_NAME):SlotBase(false,name){
		};

		//attach a function or lambda to the slot,
		///@param f a function with arguments defined by template instantiation. 
		//the signal and associated slots.
		void attach(std::function<void(Args...)> f) {
			mFunction = f;
		};

		//attach a class member to the slot,
		///@param c an instance of class C that will be used to call the member function.
		///@param f a class member function with parameters compatible with the templated parameters
		///used to define the signal and associated slots.
		template <typename C>
		void attach_class(C* c, void(C:: *f)(Args...)) {
			mFunction=util::bind_class(c,f);
		};
		
		///Triggers the slot passing in the arguments defined in the template. Even though not denoted
		///as such this is a raw function that expects lock guards to be wrapped around it by the caller.
		///It is not normally called by user code but is public so that it can be accessed by the slot.
		///@param args  As defined in the template instantiation.
		void triggerSlot(Args... args) {
			mFunction(args...);
			if (mbFireOnce) {
				mbFireOnceCompleted = true;
				mbActive = false;
			}
		};

		bool hasFunction() { return !!mFunction; }
	public:
		//Hide and disable/default all constructors and destructors unless specifically overridden
		//this is a mnemonic to force me to think about things. If overridden they are commented
		//CSlot() = default;
		~Slot() = default;
		Slot(const Slot& rhs) = delete;
		Slot& operator=(const Slot& rhs) = delete;
		Slot(Slot&& other) = delete;
		Slot& operator=(Slot&& other) = delete;
	};
};

