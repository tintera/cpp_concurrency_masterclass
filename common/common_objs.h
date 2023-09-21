#pragma once

#include <vector>
#include <thread>
// ReSharper disable CppClangTidyCppcoreguidelinesAvoidConstOrRefDataMembers

class thread_guard {  // NOLINT(cppcoreguidelines-special-member-functions)

	std::thread& t;

public:
	explicit thread_guard(std::thread& t_) : t(t_)
	{}

	~thread_guard()
	{
		if (t.joinable())
		{
			t.join();
		}
	}

	thread_guard(thread_guard&) = delete;
	thread_guard& operator= (thread_guard&) = delete;
};

class join_threads {  // NOLINT(cppcoreguidelines-special-member-functions)
	std::vector<std::thread>& threads;

public:
	explicit join_threads(std::vector<std::thread>& _threads) :
		threads(_threads)
	{}

	~join_threads()
	{
		for (auto& thread : threads)
		{
			if (thread.joinable())
				thread.join();
		}
	}

};

class function_wrapper {  // NOLINT(cppcoreguidelines-special-member-functions)
	struct impl_base {  // NOLINT(cppcoreguidelines-special-member-functions)
		virtual void call() = 0;
		virtual ~impl_base() = default;
	};

	template<typename F>
	struct impl_type : impl_base
	{
		F f;
		explicit impl_type(F&& f_) : f(std::move(f_)) {}
		void call() override { f(); }
	};

	std::unique_ptr<impl_base> impl;

public:
	template<typename F>
	function_wrapper(F&& f) :
		impl(new impl_type<F>(std::move(f)))
	{}

	void operator()() const { impl->call(); }

	function_wrapper() = default;

	function_wrapper(function_wrapper&& other) noexcept :
		impl(std::move(other.impl))
	{}

	function_wrapper& operator=(function_wrapper&& other) noexcept
	{
		impl = std::move(other.impl);
		return *this;
	}

	function_wrapper(const function_wrapper&) = delete;
	function_wrapper(function_wrapper&) = delete;
};