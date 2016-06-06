#pragma once

#include <mutex>
#include <atomic>
#include <condition_variable>

namespace evil {

	/** @brief The internals of the read write lock - not really a mutex although used like one.
		
		The ReadWriteMutex is NOT a mutex!!!  In fact it provides the internals of a read write lock.
		But then again our ReadWriteLock is not a lock its more of a wrapper around this class.  None the less
		this class - our so called ReadWriteMutex wraps up a mutex with condition variables to provide
		a parallel reas single write mechanism
	*/
	class ReadWriteMutex{

		std::mutex mMutex;
		std::condition_variable mReadCond;
		std::condition_variable mWriteCond;
		
		unsigned int muNumReads;
		bool mbIsWriting;

	private:
	
	public:
		ReadWriteMutex() {
			muNumReads = 0;
			mbIsWriting = false;
		};

		///Lock this mutex in read mode. 
		void readLock() { 		
			std::unique_lock<std::mutex> lock(mMutex);
			while (mbIsWriting) {
				mReadCond.wait(lock);
			}
			muNumReads++;
		}

		///Unlock this mutex from read mode. 
		void readUnlock() { 
			std::unique_lock<std::mutex> lock(mMutex);
			muNumReads--;
			if (!muNumReads) {
				mWriteCond.notify_one();
			}
		}

		///Lock this mutex into write mode
		void writeLock() {
			std::unique_lock<std::mutex> lock(mMutex);
			while (mbIsWriting || muNumReads) {
				mWriteCond.wait(lock);
			}
			mbIsWriting = true;
		}

		///Unlock this mutex from write mode
		void writeUnlock() {
			std::unique_lock<std::mutex> lock(mMutex);

			mbIsWriting=false;
			mReadCond.notify_all();
			mWriteCond.notify_one();
		}

	public:
		//Hide and disable/default all constructors and destructors unless specifically overridden
		//this is a mnemonic to force me to think about things. if overridden they are commented
		//CReadWriteMutex() = default;
		~ReadWriteMutex() = default;
		ReadWriteMutex(const ReadWriteMutex& rhs) = delete;
		ReadWriteMutex& operator=(const ReadWriteMutex& rhs) = delete;
		ReadWriteMutex(ReadWriteMutex&& other) = delete;
		ReadWriteMutex& operator=(ReadWriteMutex&& other) = delete;
	};
}