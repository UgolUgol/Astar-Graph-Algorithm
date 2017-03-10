#ifndef _STACK_H
#define _STACK_H
#include <iostream>
#include "StackItem.h"

template<class T> class TStack
{ 
	size_t Size;
	TStackItem<T>* _Top;
public:
	TStack()
	{
		this->Size = 0;
	}
	
	bool Push(T value)
	{
		if (this->Empty())
		{
			this->_Top = new TStackItem<T>(value);
			this->Size++;
			return true;
		}
		this->_Top = new TStackItem<T>(value, this->_Top);
		this->Size++;
		return true;
	}

	T Pop()
	{
		if (!this->Empty())
		{
			T value = this->Top();
			TStackItem<T>* removing = this->_Top;
			this->_Top = this->_Top->ReturnPrevious();
			delete removing;
			this->Size--;
			return value;
		}
	}

	T Top()
	{
		return this->_Top->ReturnTop();
	}

	bool Empty()
	{
		return (this->Size == 0);
	}
};


#endif