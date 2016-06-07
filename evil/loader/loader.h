


namespace evil {

	namespace loader {

		template <typename C, typename F, typename... Args>
		inline auto run(C&& cinst, F&& func, Args&&... args) {
			getThreadPool().run(std::forward<C>(cinst), std::forward<F>(func), std::forward<Args>(args)...);
		}


		inline Loader& getLoader(void) {
			static Loader loader;
			return loader;
		}
	}

	namespace assets{
	}
}