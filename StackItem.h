#ifndef _STACK_ITEM_H
#define _STACK_ITEM_H

template<class T> class TStackItem
{
	T Value;
	TStackItem<T>* _Previous;
public:
	TStackItem()
	{
		this->_Previous = NULL;
	}
	TStackItem(T value, TStackItem<T>* prev = nullptr)
	{
		this->Value = value;
		this->_Previous = prev;
	}

	T ReturnTop()
	{
		return this->Value;
	}

	TStackItem* ReturnPrevious()
	{
		return this->_Previous;
	}
};



#endif