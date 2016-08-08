#pragma once

#if defined(__GNUC__) && !defined(__clang__)
#define StackArray(type, name, size) \
	type name[size];
#else
#include <malloc.h>

template <typename T>
class _DynamicStackArray {
	T* arr;
	size_t size;
public:
	_DynamicStackArray(T *arr, size_t size) : arr(arr), size(size) {
		for (size_t i = 0; i < size; i++) {
			new (arr + i)T;
		}
	}
	~_DynamicStackArray() {
		for (size_t i = 0; i < size; i++)
			(&arr[i])->~T();
	}
	operator T*() { return arr; }/*
	T &operator[](size_t i) {
		return arr[i];
	}*/
};
#define StackArray(type, name, size) \
	_DynamicStackArray<type> name((type*) alloca(size * sizeof(type)), size);
#endif
