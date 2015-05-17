#include "stdafx.h"
#include "SFile.h"

#define M 0x5bd1e995
#define R 24
#define SEED 0
#define mmix(h,k) { k *= M; k ^= k >> R; k *= M; h *= M; h ^= k; }

unsigned hexFunc(const void * key, unsigned len)
{
	unsigned l = len;

	const unsigned char * data = (const unsigned char *)key;

	unsigned h = SEED;

	while(len >= 4)
	{
		unsigned k = *(unsigned*)data;

		mmix(h,k);

		data += 4;
		len -= 4;
	}

	unsigned t = 0;

	switch(len)
	{
	case 3: t ^= data[2] << 16;
	case 2: t ^= data[1] << 8;
	case 1: t ^= data[0];
	};

	mmix(h,t);
	mmix(h,l);

	h ^= h >> 13;
	h *= M;
	h ^= h >> 15;

	return h;
}

void SFile::save(std::fstream* file)
{
	file->write((char*)&m_file,sizeof(m_file));

	unsigned size = m_str.length();
	file->write((char*)&size,sizeof(size));
	file->write(m_str.data(),size);
}

void SFile::load(std::fstream* file, SFile* father /* = NULL */)
{
	m_father = father;
	file->read((char*)&m_file,sizeof(m_file));

	unsigned size;
	file->read((char*)&size,sizeof(size));
	m_str.resize(size);
	file->read(&(m_str[0]),size);

	m_hex = hexFunc(m_str.data(),m_str.length());
}

std::string SFile::way(unsigned length)
{
	std::string s= "";
	s.append(m_str);
	

	SFile* curr = m_father;
	std::string str;
	while(curr)
	{
		if (curr->stringValue().length()-1>length)
			break;
		
		s.insert(s.begin(),1,'\\');
		length--;

		str = curr->stringValue();
		s.insert(s.begin(),str.begin(),str.end()-1); // \0 не ключати
		length-=curr->stringValue().length()-1;
		curr = curr->fatherValue();
	}

	if (curr)
	{
		if(length>1)
		{
			s.insert(s.begin(),1,'\\');
			length--;
			str = curr->stringValue();
			s.insert(s.begin(),str.end()-length-1,str.end()-1);// \0 не ключати
		}
		s.insert(s.begin(),3,'.');
	}

	return s;
}