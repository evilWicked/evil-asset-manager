#pragma once

#include <mutex>

#include <assert.h>
#include <algorithm>
#include "slot-base.h"
#include "signal-base.h"
#include "../../thread/read-write-lock.h"


namespace evil {
	

	void SignalBase::rawRemoveSlot(SlotBase* slot) {
		mlistSlots.remove(slot);
	};


	void SignalBase::rawRemoveAll() {

		for(auto& x : mlistSlots) {
			x->releaseSignal();
		}
		mlistSlots.clear();
	};


	void SignalBase::rawInsertSlot(SlotBase *slot) {

		//gets called once a slot
		if(mlistSlots.empty()) {
			mlistSlots.push_front(slot);
			return;
		}

		//most likely situation - no change to priority so add at the back
		SlotBase *b = mlistSlots.back();
		if(slot->miPriority <= b->miPriority) {
			mlistSlots.push_back(slot);
			return;
		}

		//no easy fix so insert it ahead of the first lower priority slot
		for(auto it = mlistSlots.begin(); it != mlistSlots.end(); ++it) {
			if((*it)->miPriority < slot->miPriority) {
				mlistSlots.insert(it, slot);
				return;
			}
		}

		//couldn't find a slot of lower priority so add it to the end
		//should never get here....
		assert(0);
	};

	void SignalBase::removeCompletedSlots() {

		if(mbThreadSafe) {
			ReadWriteLock lock(&mRWMutex, ReadWriteLock::WRITE);

			type_slot_list_iter it = mlistSlots.begin();
			while(it != mlistSlots.end()) {
				std::lock_guard<std::mutex> slot_lock((*it)->mMutex);
				if((*it)->mbFireOnceCompleted) {
					(*it)->mbFireOnceCompleted = false;
					(*it)->mbActive = false;
					(*it)->mpcSignal = 0;
					it = mlistSlots.erase(it);
				} else {
					++it;
				}
			}

		} else {

			type_slot_list_iter it = mlistSlots.begin();
			while(it != mlistSlots.end()) {
				if((*it)->mbFireOnceCompleted) {
					(*it)->mbFireOnceCompleted = false;
					(*it)->mbActive = false;
					(*it)->mpcSignal = 0;
					it = mlistSlots.erase(it);
				} else {
					++it;
				}
			}
		}
	}

	void SignalBase::addSlot(SlotBase *slot, bool fire_once, int priority, bool active) {

	
		assert(slot->mpcSignal == NULL && "Slot already attached to a signal");

		if(mbThreadSafe) {
			ReadWriteLock lock(&mRWMutex, ReadWriteLock::WRITE);
			slot->connectSignal(this, fire_once, priority, active);
			rawInsertSlot(slot);
		} else {
			slot->connectSignal(this, fire_once, priority, active);
			rawInsertSlot(slot);
		}
	};


	//used to get around the children of a friend aren't friends rule in templated derived classes
	std::mutex& SignalBase::slotMutex(SlotBase *slot) {
		return slot->mMutex;
	}


	//--------------------------------------------------------------------------
	//							Public API
	//
	//	This is the bit that users see - we want this as small as possible. The less
	//  they can do the less can go wrong. There are some not so nice subtleties with
	//  multithread locking and we want to steer as much as possible to the signal so 
	//  that we dont need to consider two alternative paths to change the same thing
	//
	//--------------------------------------------------------------------------

	SignalBase::SignalBase(bool bThreadSafe /*= false*/, const char* name /*= NO_NAME */)
		:mbThreadSafe{ bThreadSafe }, mstrName(name) {
	};

	///Destructor. Also handles informing slots that it has been deleted
	SignalBase::~SignalBase() {
		rawRemoveAll();
	};

	///get the signals name.
	std::string SignalBase::name()const {
		return mstrName;
	};


	void SignalBase::add(SlotBase *slot, int priority/*= 0*/, bool active /*= true*/) {
		addSlot(slot, false, priority, active);
	};

	void SignalBase::addOnce(SlotBase *slot, int priority/* = 0 */, bool active /*= true*/) {
		addSlot(slot, false, priority,active);
	};

	void SignalBase::remove(SlotBase *slot) {

		if(mbThreadSafe) {
			ReadWriteLock lock(&mRWMutex, ReadWriteLock::WRITE);
			rawRemoveSlot(slot);
			slot->releaseSignal();
		} else {
			rawRemoveSlot(slot);
			slot->releaseSignal();
		}
	};

	void SignalBase::removeAll() {

		if(mbThreadSafe) {
			ReadWriteLock lock(&mRWMutex, ReadWriteLock::WRITE);
			rawRemoveAll();
		}else{
			rawRemoveAll();
		}
	};

	bool SignalBase::hasSlot(SlotBase *slot) {

		if(mbThreadSafe) {
			ReadWriteLock lock(&mRWMutex, ReadWriteLock::READ);
			return std::find(mlistSlots.begin(), mlistSlots.end(), slot) != mlistSlots.end();
		}else{
			return std::find(mlistSlots.begin(), mlistSlots.end(), slot) != mlistSlots.end();
		}
	};

	int SignalBase::numSlots() {

		if(mbThreadSafe) {
			ReadWriteLock lock(&mRWMutex, ReadWriteLock::READ);
			return (int)mlistSlots.size();
		} else {
			return (int)mlistSlots.size();
		}
	};
}

//===============================================================================================================