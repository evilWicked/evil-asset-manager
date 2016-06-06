
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <utility>

namespace evil {

	/**
		@brief An implementation of a thread safe queue we are using for threading. 
		
		Technically it can be usedfor anything else as well but we will leave the ambiguity in naming where it is.

		largely (99.99%) taken from http://roar11.com/2016/01/a-platform-independent-thread-pool-using-c14/
		

		by WillP
	*/
	template <typename T>
	class ThreadQueue {

		std::atomic_bool mValid{ true };
		mutable std::mutex mMutex;
		std::queue<T> mQueue;
		std::condition_variable mCondition;

	public:
		ThreadQueue() = default;

		~ThreadQueue(){
			invalidate();
		}

		void push(T value){
			std::lock_guard<std::mutex> lock{ mMutex };
			mQueue.push(std::move(value));
			mCondition.notify_one();
		}

		/**
		* Attempt to get the first value in the queue.
		* Returns true if a value was successfully written to the out parameter, false otherwise.
		*/
		bool tryPop(T& out){
			std::lock_guard<std::mutex> lock{ mMutex };
			if (mQueue.empty() || !mValid)
			{
				return false;
			}
			out = std::move(mQueue.front());
			mQueue.pop();
			return true;
		}

		/**
		* Get the first value in the queue.
		* Will block until a value is available unless clear is called or the instance is destructed.
		* Returns true if a value was successfully written to the out parameter, false otherwise.
		*/
		bool waitPop(T& out){
			std::unique_lock<std::mutex> lock{ mMutex };
			mCondition.wait(lock, [this]()
			{
				return !mQueue.empty() || !mValid;
			});
			/*
			* Using the condition in the predicate ensures that spurious wakeups with a valid
			* but empty queue will not proceed, so only need to check for validity before proceeding.
			*/
			if (!mValid)
			{
				return false;
			}
			out = std::move(mQueue.front());
			mQueue.pop();

			return true;
		}

		/**
		* Check whether or not the queue is empty.
		*/
		bool empty(void) const
		{
			std::lock_guard<std::mutex> lock{ mMutex };
			return mQueue.empty();
		}

		/**
		* Clear all items from the queue.
		*/
		void clear(void)
		{
			std::lock_guard<std::mutex> lock{ mMutex };
			while (!mQueue.empty())
			{
				mQueue.pop();
			}
			mCondition.notify_all();
		}

		/**
		* Invalidate the queue.
		* Used to ensure no conditions are being waited on in waitPop when
		* a thread or the application is trying to exit.
		* The queue is invalid after calling this method and it is an error
		* to continue using a queue after this method has been called.
		*/
		void invalidate(void)
		{
			std::lock_guard<std::mutex> lock{ mMutex };
			mValid = false;
			mCondition.notify_all();
		}

		/**
		* Returns whether or not this queue is valid.
		*/
		bool isValid(void) const
		{
			std::lock_guard<std::mutex> lock{ mMutex };
			return mValid;
		}


	public:
		//Hide and disable/default all constructors and destructors unless specifically overridden
		//this is a mnemonic to force me to think about things. if overridden they are commented
		//CThreadQueue() = default;
		//~CThreadQueue() = default;
		ThreadQueue(const ThreadQueue& rhs) = delete;
		ThreadQueue& operator=(const ThreadQueue& rhs) = delete;
		ThreadQueue(ThreadQueue&& other) = delete;
		ThreadQueue& operator=(ThreadQueue&& other) = delete;

	};

}

