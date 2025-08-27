#pragma once
#include <utility>

template <typename F> class scope_exit
{
public:
	explicit scope_exit(F&& f) noexcept : func(std::forward<F>(f)), active(true) {}
	~scope_exit() noexcept
	{
		if (active)
			func();
	}

	// non-copyable
	scope_exit(const scope_exit&)            = delete;
	scope_exit& operator=(const scope_exit&) = delete;

	// movable
	scope_exit(scope_exit&& other) noexcept : func(std::move(other.func)), active(other.active) { other.active = false; }

private:
	F    func;
	bool active;
};

// helper to deduce lambda type
template <typename F> scope_exit<F> make_scope_exit(F&& f) { return scope_exit<F>(std::forward<F>(f)); }