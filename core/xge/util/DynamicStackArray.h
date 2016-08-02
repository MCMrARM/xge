#pragma once

#ifdef _WINDOWS
#include <malloc.h>

template <typename T>
class WinStackArray {
	T* arr;
	size_t size;
public:
	WinStackArray(T *arr, size_t size) : arr(arr), size(size) {
		for (size_t i = 0; i < size; i++) {
			new (arr + i)T;
		}
	}
	~WinStackArray() {
		for (size_t i = 0; i < size; i++)
			(&arr[i])->~T();
	}
	operator T*() { return arr; }
	T &operator[](size_t i) {
		return arr[i];
	}
};
#define StackArray(type, name, size) \
	WinStackArray<type> name((type*) alloca(size * sizeof(type)), size);

#else
#define StackArray(type, name, size) \
	type name[size];
#endif