#pragma once

/*
  Utility functions that are needed for something specific but are of a more general nature and may 
  be required elsewhere have been placed under the evil::util namespace.

  Its not ideal but at least I know where to find things

*/

#include <functional>
#include <type_traits>


///need to specialise the standard is_placeholder method
template<int> 
struct placeholder_template
{};

namespace std
{
	///an extension of std to assist with variable placeholder arguments
	template<int N>
	struct is_placeholder< placeholder_template<N> >
		: integral_constant<int, N + 1> // seuqnces are 1 ... N  not 0 ... N-1
	{};
}

namespace evil {

	namespace util{

		///bind a class member with an unknown set of parameters to be filled by placeholders so they can
		///be realized when the function is called rather than when it is bound.

		///an integer sequence
		template<int...> 
		struct int_sequence {};

		///template recursion used to create an integer sequence
		template<int N, int... Is> 
		struct make_int_sequence
			: make_int_sequence<N - 1, N - 1, Is...> {};

		///make a sequence of integers - used to handle variable placeholders.
		template<int... Is> 
		struct make_int_sequence<0, Is...>
			: int_sequence<Is...> {};

		template<class C, class... Args, int... Is>
		auto bind_class(C *c, void(C:: *f)(Args...),int_sequence<Is...>)
		{
			auto x = std::bind(f,c, placeholder_template<Is>{}...);
			return x;
		}


		template<class C, class... Args>
		auto bind_class(C *c,void(C:: *f)(Args...))
		{
			return bind_class(c,f, make_int_sequence< sizeof...(Args) >{});
		}

	}
}