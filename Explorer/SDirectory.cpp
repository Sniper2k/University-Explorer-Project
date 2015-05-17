#include "StdAfx.h"
#include "SDirectory.h"

SDirectory::SDirectory(void)
{
}

SDirectory::SDirectory(SDirectory* d,SFile* father):
SFile(d,father),m_amountOfDirectories(d->m_amountOfDirectories)
{
	List* l = d->listOfChilds();  
	l->toStart();
	while (l->currentIsValid())
	{
		SFile* p;
		SFile* current = l->currentValue();
		if (current->isFile())
			p = new SFile(current,this);
		else
			p = new SDirectory((SDirectory*) current,this);
		m_childs.addElementAfter(p);

		l->next();
	}
}

SDirectory::SDirectory(std::string* s,SFile* father /* = NULL */):
SFile(s,false,father),m_amountOfDirectories(0)
{
}

SDirectory::~SDirectory(void)
{
	clear();
}

unsigned SDirectory::createChild(std::string* s, bool isFile)
{
	unsigned hex = hexFunc(s->data(),s->length());
	if (!alreadyExistsInChilds(s,isFile))
	{
		SFile* p;
		if (isFile)
			p = new SFile(s,true,this);
		else
		{
			p = new SDirectory(s,this);
			m_amountOfDirectories++;
		}
			
		return m_childs.addBeforePassingTest(minor,p);
	}

	return -1;
}

unsigned SDirectory::addChild(SFile* son)
{
	SFile* p;
	if (son->isFile())
		p = new SFile(son, this);
	else
	{
		p = new SDirectory((SDirectory*) son, this);
		m_amountOfDirectories++;
	}

	return m_childs.addBeforePassingTest(minor,p);
}

bool SDirectory::alreadyExistsInChilds(std::string* s, bool file)
{

	if (m_childs.isEmpty())
		return false;

	SFile f(s,file);
	SFile* t = m_childs.findElementPassingTest(equal,&f);

	if (t)
		return true;

	return false;
}

void SDirectory::clear()
{
	m_childs.toStart();
	while (m_childs.currentIsValid())
	{
		SFile* p = m_childs.eraseCurrent();
		delete p;
	}
	m_amountOfDirectories = 0;
}

SFile* SDirectory::childAtPosition(unsigned pos)
{
	return m_childs.valueAtPosition(pos);
}

bool SDirectory::childOf(SFile* p)
{
	SFile* current = this;
	while (current!= p && current!= NULL)
		current = current->fatherValue();

	return (current == p);
}

void SDirectory::deleteChild(unsigned index)
{
	SFile* p= eraseChild(index);
	delete p;
}

SFile* SDirectory::eraseChild(unsigned index)
{
	m_childs.setCurrentToPosition(index);
	SFile* p = m_childs.eraseCurrent();
	if (!p->isFile())
		m_amountOfDirectories--;

	return p;
}

void SDirectory::save(std::fstream* file)
{
	SFile::save(file);

	file->write((char*)&m_amountOfDirectories,sizeof(m_amountOfDirectories));
	unsigned size = m_childs.length();
	file->write((char*)&size, sizeof(size));

	m_childs.toStart();
	while(m_childs.currentIsValid())
	{
		SFile* curr= m_childs.currentValue();
		curr->save(file);

		m_childs.next();
	}
}

void SDirectory::load(std::fstream* file, SFile* father /* = NULL */)
{
	SFile::load(file,father);

	file->read((char*)&m_amountOfDirectories,sizeof(m_amountOfDirectories));
	unsigned size;
	file->read((char*)&size, sizeof(size));

	for (unsigned i =0 ;i<size;i++)
	{
		SFile* p;
		if (i<m_amountOfDirectories)
			p = new SDirectory;
		else
			p = new SFile;
		
		p->load(file,this);
		m_childs.addElementAfter(p);
	}
}

void SDirectory::search(std::string* s,bool file, List* l)
{
	SFile f(s,file);
	find(&f,l);
}

void SDirectory::find(SFile* f,List* l)
{
	SFile* t = m_childs.findElementPassingTest(equal,f);
	if (t)
		l->addElementAfter(t);

	m_childs.toStart();
	unsigned i=0;
	while (m_childs.currentIsValid() && i<m_amountOfDirectories)
	{
		SDirectory* dir = (SDirectory*) m_childs.currentValue();
		dir->find(f,l);
		i++;
		m_childs.next();
	}
}