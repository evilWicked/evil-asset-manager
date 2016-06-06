#pragma once

#include "thread-queue.h"
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

namespace evil {

	/**
		@brief A simple threadpool class 
		
		A pool of threads - whatmore can I say? This has largely been taken from 

		http://roar11.com/2016/01/a-platform-independent-thread-pool-using-c14/

		by WillP
	*/
	class ThreadPool {

		/// Base class of our tasks to allow them to be queued - defined as an inner class becuase - well thats 
		/// how WillP did it and we are only refactoring so thats how it is.
		class TaskBase
		{
		public:

			virtual void execute() = 0;
			//delete/default all construction/destruction unless we specifically need them.
			TaskBase(void) = default;
			virtual ~TaskBase(void) = default;
			TaskBase(const TaskBase& rhs) = delete;
			TaskBase& operator=(const TaskBase& rhs) = delete;
			TaskBase(TaskBase&& other) = default;
			TaskBase& operator=(TaskBase&& other) = default;
		};

		/// override the base to allow tasks with different signatures
		template <typename F>
		class ThreadTask : public TaskBase {
			F mFunc;
		public:
			ThreadTask(F&& func) :mFunc{ std::move(func) } {
			}

			void execute() override {
				mFunc();
			}

			//delete/default all construction/destruction unless we specifically need them.
			~ThreadTask(void) override = default;
			ThreadTask(const ThreadTask& rhs) = delete;
			ThreadTask& operator=(const ThreadTask& rhs) = delete;
			ThreadTask(ThreadTask&& other) = default;
			ThreadTask& operator=(ThreadTask&& other) = default;
		};

	public:
		/// defines a future that we can use to wait on the return if needed
		template <typename T>
		class TaskFuture {
			std::future<T> mFuture;
		public:
			TaskFuture(std::future<T>&& future)
				:mFuture{ std::move(future) } {
			}

			~TaskFuture(void) {
				if (mFuture.valid()) {
					mFuture.get();
				}
			}

			auto get(void) {
				return mFuture.get();
			}

			//delete/default all construction/destruction unless we specifically need them.
			TaskFuture(const TaskFuture& rhs) = delete;
			TaskFuture& operator=(const TaskFuture& rhs) = delete;
			TaskFuture(TaskFuture&& other) = default;
			TaskFuture& operator=(TaskFuture&& other) = default;
		};

		/// API below here
	private:
		std::atomic_bool mbDone;
		ThreadQueue<std::unique_ptr<TaskBase>> mWorkQueue;
		std::vector<std::thread> mThreads;

		/// A worker task that waits for tasks and starts them when they arrive. 
		void worker(void) {
			while (!mbDone) {
				std::unique_ptr<TaskBase> pTask{ nullptr };
				if (mWorkQueue.waitPop(pTask)){
					pTask->execute();
				}
			}
		}

		/// cleans up by rejoining threads and invalidating the queue
		void destroy(void) {
			mbDone = true;
			mWorkQueue.invalidate();
			for (auto& thread : mThreads) {
				if (thread.joinable()) {
					thread.join();
				}
			}
			//mWorkQueue.clear();
		}

	public:

		/// default constructor - Automatically create as many threads as the hardware can carry 
		/// allowing for the main thread 
		ThreadPool(void)
			:ThreadPool{ std::max(std::thread::hardware_concurrency(), 2u) - 1u } {
			
			// Always create at least one thread.  If hardware_concurrency() returns 0,
			// subtracting one would turn it to UINT_MAX, so get the maximum of
			// hardware_concurrency() and 2 before subtracting 1.
		}


		/// The actual constructor - allows you to manually define the number of threads. 
		/// Remember more than you have physical threads is a waste of time
		explicit ThreadPool(const std::uint32_t numThreads)
			:mbDone{ false }, mWorkQueue{}, mThreads{} {
			try{
				for (std::uint32_t i = 0u; i < numThreads; ++i){
					//mThreads.emplace_back(std::thread(&CThreadPool::worker,this));
				 mThreads.emplace_back(&ThreadPool::worker, this);
				}

			}catch (...) {
				destroy();
				throw;
			}
		}

		~ThreadPool(void) {
			destroy();
		}


		/// Submit a job to be run by the thread pool. This version is intended for normal functions.
		/// This returns a future. If submitting a reference for an argument, it is important to remember
		/// to wrap it with std::ref or std::cref.
		template <typename F, typename... Args>
		auto calc(F&& func, Args&&... args){
			auto boundTask = std::bind(std::forward<F>(func), std::forward<Args>(args)...);
			using ResultType = std::result_of_t<decltype(boundTask)()>;
			using PackagedTask = std::packaged_task<ResultType()>;
			using TaskType = ThreadTask<PackagedTask>;

			PackagedTask task{ std::move(boundTask) };
			TaskFuture<ResultType> result{ task.get_future() };
			mWorkQueue.push(std::make_unique<TaskType>(std::move(task)));
			return result;
		}

		/// Submit a job to be run by the thread pool. This version is intended for normal functions.
		/// This DOES NOT return a future. If submitting a reference for an argument, it is important to remember
		/// to wrap it with std::ref or std::cref.
		template <typename F, typename... Args>
		void run(F&& func, Args&&... args) {
			auto boundTask = std::bind(std::forward<F>(func), std::forward<Args>(args)...);
			using ResultType = std::result_of_t<decltype(boundTask)()>;
			using PackagedTask = std::packaged_task<ResultType()>;
			using TaskType = ThreadTask<PackagedTask>;

			PackagedTask task{ std::move(boundTask) };
			mWorkQueue.push(std::make_unique<TaskType>(std::move(task)));
		}

		/// Submit a job to be run by the thread pool.  This version is intended for class members.
		/// This returns a future. If submitting a reference for an argument, it is important to remember
		/// to wrap it with std::ref or std::cref.
		template <typename C, typename F, typename... Args>
		auto calc(C&& cinst, F&& func, Args&&... args) {
			auto boundTask = std::bind(std::forward<F>(func), std::forward<C>(cinst), std::forward<Args>(args)...);
			using ResultType = std::result_of_t<decltype(boundTask)()>;
			using PackagedTask = std::packaged_task<ResultType()>;
			using TaskType = CThreadTask<PackagedTask>;

			PackagedTask task{ std::move(boundTask) };
			TaskFuture<ResultType> result{ task.get_future() };
			mWorkQueue.push(std::make_unique<TaskType>(std::move(task)));
			return result;
		}

		/// Submit a job to be run by the thread pool.  This version is intended for class members.
		/// This DOES NOT return a future. If submitting a reference for an argument, it is important to remember
		/// to wrap it with std::ref or std::cref.
		template <typename C, typename F, typename... Args>
		void run(C&& cinst, F&& func, Args&&... args) {
			auto boundTask = std::bind(std::forward<F>(func), std::forward<C>(cinst), std::forward<Args>(args)...);
			using ResultType = std::result_of_t<decltype(boundTask)()>;
			using PackagedTask = std::packaged_task<ResultType()>;
			using TaskType = ThreadTask<PackagedTask>;

			PackagedTask task{ std::move(boundTask) };
			mWorkQueue.push(std::make_unique<TaskType>(std::move(task)));
		}

		//delete/default all construction/destruction unless we specifically need them.
		//~CThreadPool() = default;
		ThreadPool(const ThreadPool& rhs) = delete;
		ThreadPool& operator=(const ThreadPool& rhs) = delete;
		ThreadPool(ThreadPool&& other) = delete;
		ThreadPool& operator=(ThreadPool&& other) = delete;
	};



	namespace thread_pool {

		/**
		This is intended for situations where a thread will return a value for later use.
		It returns a TaskFuture<ResultType> that can be used to access the result of the the
		operation via get();

		Care needs to be taken when using this because the future will wait until the thread has completed.
		This means that if a number of threads need to be created in a loop then the futures need to be
		defined outside the loop or they will wait until the thread returns on each iteration before 
		destroying the return value.

		In other words do this.

				const int numThreads = 10;
				std::vector<evil::thread_pool::TaskFuture<void>> v;
				for (int i = 0; i < numThreads; ++i) {
					v.push_back(
						evil::thread_pool::calc([]() {
						std::this_thread::sleep_for(std::chrono::milliseconds(1000));
					}));
				}

		And don't do this

			const int numThreads = 10;
			for (int i = 0; i < numThreads; ++i) {

				evil::thread_pool::calc([]() {
					std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				 });

				 //because the loop will wait here each iteration before destroying the return.
				 //even if you are not using it
			}

			If submitting a reference for an argument, it is important to remember to wrap it with
			std::ref or std::cref.
		*/
		template <typename F, typename... Args>
		inline auto calc(F&& func, Args&&... args) {
			return getThreadPool().calc(std::forward<F>(func), std::forward<Args>(args)...);
		}

		///the second signature is for use with class members where an instance of the class needs
		///to be included as the first parameter
		template <typename C, typename F, typename... Args>
		inline auto calc(C&& cinst, F&& func, Args&&... args) {
			return getThreadPool().calc(std::forward<C>(cinst), std::forward<F>(func), std::forward<Args>(args)...);
		}

		/**
		  To handle the scenario in which you simply want to fire off the thread and forget about it
		  you can use the run function.	This submits the job to the queue and then moves on. There is no
		  return to block the main thread.

		  If submitting a reference for an argument, it is important to remember to wrap it with
		  std::ref or std::cref.
		*/
		template <typename F, typename... Args>
		void run(F&& func, Args&&... args) {
			getThreadPool().run(std::forward<F>(func), std::forward<Args>(args)...);
		}

		///like calc() the second signature is for use with class members where an instance of the class needs
		///to be included as the first parameter
		///
		/// If submitting a reference for an argument, it is important to remember to wrap it with
		/// std::ref or std::cref.
		template <typename C, typename F, typename... Args>
		inline auto run(C&& cinst, F&& func, Args&&... args) {
			getThreadPool().run(std::forward<C>(cinst), std::forward<F>(func), std::forward<Args>(args)...);
		}

		/// Get the default thread pool for the application.
		/// This pool is created with std::thread::hardware_concurrency() - 1 threads.
		inline ThreadPool& getThreadPool(void) {
			static ThreadPool defaultPool;
			return defaultPool;
		}
	};
}

