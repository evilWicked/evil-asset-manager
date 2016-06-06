#pragma once 

#include "slot-base.h"
#include "signal-base.h"

namespace evil {

	

	void SlotBase::connectSignal(SignalBase *signal,bool fire_once/*=false*/,int priority/*=0*/, bool active/*=false*/) {

		if(mbThreadSafe) {
			std::lock_guard<std::mutex> lock(mMutex);
			mpcSignal = signal;
			mbActive = active;
			mbFireOnce = fire_once;
			mbFireOnceCompleted = false;
		} else {
			mpcSignal = signal;
			mbActive = active;
			mbFireOnce = fire_once;
			mbFireOnceCompleted = false;
		}
	};

	void SlotBase::releaseSignal(){

		if (mbThreadSafe){
			std::lock_guard<std::mutex> lock(mMutex);
			mpcSignal=0;
			mbActive = false;
			mbFireOnce = false;
			mbFireOnceCompleted = false;
		}else{
			mpcSignal = 0;
			mbActive = false;
			mbFireOnce = false;
			mbFireOnceCompleted = false;
		}
	};



	//--------------------------------------------------------------------------
	//							Public API
	//
	//	This is the bit that users see - we want this as small as possible. 
	//  The fewer public entry points we have the simpler it is to thread
	//
	//--------------------------------------------------------------------------

	SlotBase::SlotBase(bool bThreadSafe /*= false*/,const char* name /*= NO_NAME */) 
						:mbThreadSafe{ bThreadSafe }, mstrName(name) {
		mbActive = false;
		mpcSignal = 0;
		miPriority = 0;
		mbFireOnce = false;
		mbFireOnceCompleted = false;
	}

	SlotBase::~SlotBase() {
		if (mpcSignal != NULL) {
			mpcSignal->remove(this);
		}
	};
	
	std::string SlotBase::name() const {
		return mstrName; //constant variable - no thread issues
	};

	bool SlotBase::isFireOnce() { 
		return mbFireOnce; 
	}

	bool SlotBase::isActive() { 
		return mbActive; 
	}

	void SlotBase::setActive(bool x) {
		std::lock_guard<std::mutex> lock(mMutex);
		mbActive = x;
	};

	SignalBase *SlotBase::signal() {
		return mpcSignal;
	};
}
	