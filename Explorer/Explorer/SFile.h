#pragma once
#include <string>
#include <fstream>

extern unsigned hexFunc(const void * key, unsigned len); // функція хексування

class SFile
{
public:
	SFile(std::string* s,bool file, SFile* father = NULL):
	  m_str(*s),m_file(file),m_hex(hexFunc(s->data(),s->length())),m_father(father){};
	SFile(SFile* f,SFile* father):
	  m_str(f->stringValue()),m_hex(f->hexValue()),m_father(father),m_file(f->m_file){};
	SFile(){};
	

	virtual ~SFile(){};

	inline unsigned hexValue(){return m_hex;};
	inline std::string stringValue(){return m_str;};
	inline bool isFile(){return m_file;};
	inline SFile* fatherValue(){return m_father;};
	inline void setFather(SFile* father){m_father = father;};

	virtual void save(std::fstream* file);
	virtual void load(std::fstream* file, SFile* father = NULL);

	std::string way(unsigned length);

protected:
	std::string m_str;
	unsigned m_hex;
	SFile* m_father;
	bool m_file;
	
};
