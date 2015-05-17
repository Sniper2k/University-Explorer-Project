#pragma once
#include <string>
#include <fstream>
#include "List.h"
#include "SFile.h"

class SDirectory : public SFile
{
private:
	List m_childs;
	unsigned m_amountOfDirectories;

public:
	inline List* listOfChilds(){return &m_childs;};
	inline unsigned amountOfDirectories(){return m_amountOfDirectories;};

	unsigned createChild(std::string* s, bool isFile); // повертає позицію нового елементу
	unsigned addChild(SFile* son); // повертає позицію нового елементу
	 
	void clear();
	SFile* eraseChild(unsigned index);
	void deleteChild(unsigned index);

	bool alreadyExistsInChilds(std::string* s, bool file);
	bool childOf(SFile* p);

	SFile* childAtPosition(unsigned pos);

	void save(std::fstream* file);
	void load(std::fstream* file, SFile* father = NULL);

	void search(std::string* s,bool file, List* l);
	void find(SFile* f,List* l);

	SDirectory(SDirectory* d,SFile* father);
	SDirectory(void);
	SDirectory(std::string* s,SFile* father = NULL);
	~SDirectory(void);
};

