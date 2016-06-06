#pragma once


#include <functional>
#include <mutex>

#include "signal-base.h"
#include "../../util/utility.h"

#define NO_NAME "NO NAME"


namespace evil {

	
	class SignalBase;//forward declaration

	/**
	@brief Base Slot Class used by the evil::Slot and evil::ThreadSlot class. 

	Have drawn upon the basic outline of a signal defined in http://simmesimme.github.io/tutorials/2015/09/20/signal-slot/
	As well as some info on function pointer usage http://www.cplusplus.com/forum/general/159730/
	*/
	class SlotBase{

	protected:
		friend class SignalBase;
		
		const bool mbThreadSafe;
		std::mutex mMutex;
		
		const std::string mstrName;		//Optional. Used to give the signal a name - helps with debugging

		SignalBase *mpcSignal;
		int miPriority;
		bool mbActive;
		bool mbFireOnce; //default false
		bool mbFireOnceCompleted;

		void connectSignal(SignalBase *signal,bool fire_once=false,int priority=0,bool active=false);
		void releaseSignal();
		
	public:

		///Constructor
		///@param thread_safe  Optionally enable thread safety on this slot. Defaults to off.
		///@param name  Optionally provide a name for this slot.
		SlotBase(bool thread_safe = false, const char* name = NO_NAME);

		///Along with normal destruction, this has the additional task of removing itself 
		///from the connected signal to stop it attempting to call the callback in the future.
		~SlotBase();
		
		///get the slots name.
		std::string name() const;

		///Allow a slot to be turned off - perhaps during a pause state. Stops it receiving a signal
		///@param x boolean. true is on false is off.
		void setActive(bool x);

		///returns true if the slot has been set to fire once
		bool isFireOnce();

		///returns true if the slot is active
		bool isActive();

		///returns the signal this slot is attached to. Care when used in a threaded environment.
		///If you are moving slots between signals - unlikely perhaps - but the value returned
		///can be changed by other threads. Any sections of code that change the ownership of 
		///a slot should have mutex locks placed around them. The Signal function hasSlot can 
		///be used to test that you have the correct signal - but again this test needs to occur 
		///from a guarded section because it can change the instant after the test.
		SignalBase *signal();

	public:
		//Hide and disable/default all constructors and destructors unless specifically overridden
		//this is a mnemonic to force me to think about things. if overridden they are commented
		//CSlotBase() = default;
		//~CSlotBase() = default;
		SlotBase(const SlotBase& rhs) = delete;
		SlotBase& operator=(const SlotBase& rhs) = delete;
		SlotBase(SlotBase&& other) = delete;
		SlotBase& operator=(SlotBase&& other) = delete;

	};

};
