#pragma once

#include "read-write-mutex.h"

namespace evil {

	/**
	@brief a Read Write implementation to allow parallel reads and sequential writes.

	The ReadWriteLock enables multiple reader to exist concurrently while restricting writes to to 
	sequential access.  The particular scenario is a list that has readers progressing down it at 
	those times we are not making changes to the structure of the list.  

	The CReadWriteLock has the following behaviours.

		1. Reads can occur in parallel.
		2. Writes can only occur when no other read or write is in progress.
		3. Reads can not occur when a write is in progress.

	*/
	class ReadWriteLock {
	public:
		enum enumType {READ,WRITE};
		private:
		enumType menumType;
		ReadWriteMutex *mpRWmutex;
	public:

			ReadWriteLock(ReadWriteMutex *pRWmutex,enumType type){
				menumType = type;
				mpRWmutex = pRWmutex;

				switch (menumType) {
					case READ: {
						mpRWmutex->readLock();
					}; break;
					case WRITE:{
						mpRWmutex->writeLock();
					}; break;
				}
			};

			
			~ReadWriteLock(){

				switch (menumType) {
					case READ:{
						mpRWmutex->readUnlock();
					};break;
					case WRITE:{
						mpRWmutex->writeUnlock();
					};break;
				}
			};


	public:
		//Hide and disable/default all constructors and destructors unless specifically overridden
		//this is a mnemonic to force me to think about things. if overridden they are commented
		//CReadWriteLock() = default;
		//~CReadWriteLock() = default;
		ReadWriteLock(const ReadWriteLock& rhs) = delete;
		ReadWriteLock& operator=(const ReadWriteLock& rhs) = delete;
		ReadWriteLock(ReadWriteLock&& other) = delete;
		ReadWriteLock& operator=(ReadWriteLock&& other) = delete;
	};
}