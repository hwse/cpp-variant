#include<array>
#include<algorithm>
#include<cassert>
#include<cstdint>
#include<iostream>
#include<type_traits>

/**
 * struct to get them maximal sizeof multiple types
 */
template<typename T, typename... Types>
struct max_sizeof;

template<typename T>
struct max_sizeof<T>
{
	constexpr static size_t value = sizeof(T);
};

template<typename T0, typename T1, typename... Types>
struct max_sizeof<T0, T1, Types...>
{
	constexpr static size_t value = sizeof(T0) > sizeof(T1) ?
		max_sizeof<T0, Types...>::value :
		max_sizeof<T1, Types...>::value;
	
};

template<typename SEARCH_T, typename T, typename... Types>
struct type_index;

template<typename SEARCH_T, typename T>
struct type_index<SEARCH_T, T>
{
	constexpr static bool found = std::is_same<SEARCH_T, T>::value;
	constexpr static size_t value = 0;
};

template<typename SEARCH_T, typename T0, typename T1, typename... Types>
struct type_index<SEARCH_T, T0, T1, Types...>
{
	constexpr static bool found = 
		std::is_same<SEARCH_T, T0>::value || type_index<SEARCH_T, T1, Types...>::found;
	constexpr static size_t value = std::is_same<SEARCH_T, T0>::value ?
		0 : 
		type_index<SEARCH_T, T1, Types...>::value + 1;
};

/**
 * Get the type index of SEARCH_T for a list of at least one type
 */
template<typename SEARCH_T, typename T, typename... Types>
constexpr size_t get_type_index()
{
	using Result = type_index<SEARCH_T, T, Types...>;
	static_assert(Result::found, "type not found!");
	return Result::value;
}

template<typename... Types>
class Variant
{
public:
	template<typename T>
	Variant(const T & t);
	
	Variant(const Variant & other);
	
	template<typename T>
	bool holds() const;
	
	template<typename T>
	const T & get() const;
	
	void print() const;
private:
	constexpr static size_t blobSize = max_sizeof<Types...>::value;
	
	uint8_t which;
	std::array<uint8_t, blobSize> blob;
};

template<typename... Types>
template<typename T>
Variant<Types...>::Variant(const T & t)
	: which(get_type_index<T, Types...>())
{
	new (blob.data()) T(t);
}

template<typename... Types>
Variant<Types...>::Variant(const Variant & t)
	: which(other.which)
{
	// TODO: copy
}

template<typename... Types>
template<typename T>
bool Variant<Types...>::holds() const
{
	return get_type_index<T, Types...>() == which;
}

template<typename... Types>
template<typename T>
const T & Variant<Types...>::get() const
{
	assert(holds<T>());
	const T * t = reinterpret_cast<const T *>(blob.data());
	return *t;
}

template<typename... Types>
void Variant<Types...>::print() const
{
	std::cout << "which is: " << static_cast<size_t>(which) << std::endl;
	std::cout << "blob has size: " << blobSize << std::endl;
}


int main()
{
	const char * s = "trest";
	Variant<int, const char*> x = s;
	std::cout << "sizeof(Variant<...>):" << sizeof(x) << std::endl;
	x.print();
	if (x.holds<const char *>())
	{
		std::cout << x.get<const char *>() << std::endl;
	}
	else
	{
		std::cout << x.get<int>() << std::endl;
	}
	auto y = x;
	y.print();
	return 0;
}