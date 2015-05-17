#include "stdafx.h"
#include "List.h"

bool List::isEmpty()
{
	return !((bool)size);
}

bool List::isElementBefore()
{
	return (begin != current);
}

bool List::isElementAfter()
{
	return (current != end);
}

void List::toStart()
{
	current = begin;
}

void List::toEnd()
{
	current = end;
}

void List::next()
{
	if (current != NULL)
	{
		if(isElementAfter())
			current = current->next;
		else
			current = NULL;
	}

}

void List::prev()
{
	if (current != NULL)
	{
		if(isElementBefore())
			current = current->prev;
		else
			current = NULL;
	}
}

SFile* List::currentValue()
{
	if (current != NULL)
		return current->value;

	return NULL;
}

void List::addElementBefore(SFile* val)
{
	if (current != NULL || isEmpty())
	{
		LElem* p = new LElem(val);

		if (isEmpty())
		{
			begin = p;
			end = p;
		}
		else if (isElementBefore())
		{
			p->next = current;
			p->prev = current->prev;
			current->prev = p;
			p->prev->next = p;
		}
		else
		{
			current->prev = p;
			p->next = current;
			begin = p;
		}

		current = current->prev;
		size++;
	}
}

void List::addElementAfter(SFile* val)
{
	if (current != NULL || isEmpty())
	{
		LElem* p = new LElem(val);

		if (isEmpty())
		{
			begin = p;
			end = p;
		}
		else if (isElementAfter())
		{
			p->prev = current;
			p->next = current->next;
			current->next = p;
			p->next->prev = p;
		}
		else
		{
			p->prev = current;
			current->next = p;
			end = p;
		}

		current = p;
		size++;
	}
}

SFile* List::eraseCurrent()
{
	SFile* retValue = NULL;
	if (current != NULL)
	{
		LElem* p = current;
		retValue = p->value;

		if (!isElementAfter())
		{
			if(!isElementBefore())
			{
				current = NULL;
				begin = NULL;
				end = NULL;
			}
			else
			{
				current =current->prev;
				current->next = NULL;
				end = current;
			}
		}
		else
		{
			if(!isElementBefore())
			{
				current = current->next;
				current->prev = NULL;
				begin = current;
			}
			else
			{
				current->prev->next = current->next;
				current->next->prev = current->prev;
				current = current->next;
			}
		}

		delete p;
		size--;
	}

	return retValue;
}

void List::clear()
{
	toStart();
	while (!isEmpty())
	{
		eraseCurrent();
	}
}

bool List::currentIsValid()
{
	return (bool)current;
}

void List::setCurrentToPosition(unsigned position)
{
	toStart();
	unsigned i=0;
	while (currentIsValid() && i<position)
	{
		i++;
		next();
	}
}

bool  equal(SFile* obj,SFile* comp)
{
	if (obj->isFile() == comp->isFile())
		if (obj->hexValue() == comp->hexValue())
			if (obj->stringValue() == comp->stringValue())
				return true;
	return false;
}

bool minor(SFile* obj,SFile* comp)
{
	if (!obj->isFile() && comp->isFile())
		return false;
	else if (obj->isFile() && !comp->isFile())
		return true;

	std::string str = obj->stringValue();
	std::string s = comp->stringValue();
	unsigned i = 0;
	while (i<s.length())
	{
		if (i == str.length())
			return false;

		if (str[i]>s[i])
			return true;
		else if (str[i]<s[i])
			return false;
		else
			i++;
	}

	return false;
}

SFile* List::findElementPassingTest(compareFunc test,SFile* comp)
{
	toStart();
	while(currentIsValid())
	{
		SFile* curr = currentValue();

		if (test(curr,comp))
			return curr;
		
		next();
	}
	
	return NULL; 
}

unsigned List::length()
{
	return size;
}

unsigned List::addBeforePassingTest(compareFunc test, SFile* val)
{
	if (!(findElementPassingTest(minor,val)))
	{
		toEnd();
		addElementAfter(val);
	}
	else
		addElementBefore(val);

	return currentPosition();
}

unsigned List::currentPosition()
{
	if (!current)
		return 0;

	LElem* p = current;

	unsigned i = 0;
	toStart();
	while (current != p)
	{
		next();
		i++;
	}

	return i;
}

SFile* List::valueAtPosition(unsigned pos)
{
	setCurrentToPosition(pos);

	if (currentIsValid())
		return currentValue();
	else
		return NULL;
}