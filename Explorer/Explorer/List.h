#pragma once
#include "SFile.h"

typedef bool (*compareFunc) (SFile* obj,SFile* comp);

extern bool equal(SFile* obj,SFile* comp);
extern bool minor(SFile* obj,SFile* comp);

class LElem
{
public:
	LElem():value(NULL),next(NULL),prev(NULL){};
	LElem(SFile* val):value(val),next(NULL),prev(NULL){};
	~LElem(){};

	SFile* value;
	LElem* next;
	LElem* prev;
};

class List
{
private:
	LElem* begin;
	LElem* end;
	LElem* current;

	unsigned size;
public:
	bool isEmpty();
	bool isElementBefore();
	bool isElementAfter();
	bool currentIsValid();

	unsigned length();

	void toStart();
	void toEnd();
	void next();
	void prev();
	void setCurrentToPosition(unsigned position);

	SFile* currentValue();
	SFile* valueAtPosition(unsigned pos);

	void addElementBefore(SFile* val);
	void addElementAfter(SFile* val);
	SFile* eraseCurrent();
	void clear();

	SFile* findElementPassingTest(compareFunc test, SFile* comp); 
	unsigned addBeforePassingTest(compareFunc test, SFile* val); // повертає позицію нового елементу
	unsigned currentPosition();

	List():begin(NULL),end(NULL),current(NULL),size(0){};
	~List(){clear();};
};

