#pragma once


#include <list>
#include <mutex>

#include "slot-base.h"
#include "../../thread/read-write-mutex.h"

#define NO_NAME "NO NAME"

//implementation included by the include at the bottom of this file.

namespace evil {

	
	class SlotBase;//forward declaration

	/**
	@brief  the base class used by both the evil::Signal and evil::ThreadSignal class. 

	Have drawn upon the basic outline of a signal defined in http://simmesimme.github.io/tutorials/2015/09/20/signal-slot/
	As well as some info on function pointer usage http://www.cplusplus.com/forum/general/159730/
	*/
	class SignalBase {
	protected:

		friend class SlotBase;

		typedef std::list<SlotBase *> type_slot_list;
		typedef type_slot_list::iterator type_slot_list_iter;
		
		/// Optional. Used to give the signal a name - helps with debugging
		const std::string mstrName;
		const bool mbThreadSafe;
		ReadWriteMutex mRWMutex;
		type_slot_list mlistSlots;


		void rawRemoveSlot(SlotBase* slot);
		void rawRemoveAll();
		void rawInsertSlot(SlotBase *slot);

		///used internally to remove fireOnce slots
		void removeCompletedSlots();
		void addSlot(SlotBase* slot, bool fire_once, int priority = 0, bool active = true);
		
		//helper to get past children of a friend aren't friend rules
		//when called by the final templated signal class
		std::mutex& slotMutex(SlotBase *slot);

	public:

		///Constructor - you can optionally name this signal. Ths signal can be created to either be threadsafe 
		///uninsg mutex locks or not by setting the bThreadSafe flag.
		SignalBase(bool bThreadSafe = false,const char* name = NO_NAME);
	
		///needs to remove all slots
		virtual ~SignalBase();

		///get the signals name.
		std::string name()const;

		///Add a slot to this signal. 
		///@param slot  The slot you want to add to this signal. Care that the template definition matches this one.
		///@param priority   Optionally assign a priortiy to the slot. Priority zero is normal. Higher fires earlier.
		///@param active   Optionally deactivate the slot on creation by setting active to false.
		void add(SlotBase *slot, int priority = 0, bool active = true);

		///Add a slot to this signal to fire once and be removed. 
		///@param slot  The slot you want to add to this signal. Care that the template definition matches this one.
		///@param priority   Optionally assign a priortiy to the slot. Priority zero is normal. Higher fires earlier.
		///@param active   Optionally deactivate the slot on creation by setting active to false.
		void addOnce(SlotBase *slot, int priority = 0, bool active = true);

		///Remove a single slot from this signal. Priority, active state and fire_once state are not preserved.
		void remove(SlotBase *slot);

		///Remove all slots from this signal. Priority, active state and fire_once state are not preserved.
		void removeAll();

		///return how many slots are connected
		int numSlots();
		
		///return true is this signal contains a slot
		bool hasSlot(SlotBase *slot);

	public:
		//Hide and disable/default all constructors and destructors unless specifically overridden
		//this is a mnemonic to force me to think about things. if overridden they are commented
		//SignalBase() = default;
		//~SignalBase() = default;
		SignalBase(const SignalBase& rhs) = delete;
		SignalBase& operator=(const SignalBase& rhs) = delete;
		SignalBase(SignalBase&& other) = delete;
		SignalBase& operator=(SignalBase&& other) = delete;
	};

};
