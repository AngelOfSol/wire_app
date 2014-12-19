#pragma once
#include <type_traits>
template <typename T> 
int sign(T val) 
{
    return (T(0) < val) - (val < T(0));
}

template <typename T>
T gcd(T a, T b) 
{
    return b == 0 ? a : gcd(b, a % b);
}


template <typename T>
T lcm(T a, T b)
{
	return a * b / gcd(a, b);
}

template <typename Iter>
auto lcm_of(Iter begin, Iter end) -> typename Iter::value_type
{
	Iter::value_type ret = 1;
	for(; begin != end; begin++)
	{
		ret = lcm(ret, *begin);
	}
	return ret;
}