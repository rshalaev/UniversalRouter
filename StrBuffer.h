///////////////////////////////////////////////
// Copyright (c) 2020 Ruslan Shalaev         //
// All Rights Reserved                       //
///////////////////////////////////////////////

#ifndef STRBUFFER_H
#define STRBUFFER_H

//wxWidgets
#include "wx\thread.h"
//STL
#include <queue>
#include <string>
#include <ctime>

template <class T> class CBuffer //prefix sbuf
{//buffer containing std::strings
public:
	            CBuffer(int nBufMax, int nTimeOut, std::string sName,bool bExp,int nExpTime);
	int         GenNumDiscarded();//return # of strings discarded
	void        Add(T);//Add string to the buffer
	T           Get();//Get oldest string in the buffer
   std::vector<T> GetQueue();//Get all the strings in the queue without removing them from the queue
   bool        IsEmpty(){DiscradExpired();return m_bEmpty;};
   volatile bool  m_bEmpty; //used to synchronize reading and writing threads
private:
   std::queue <T> m_quBuf;//queue container object
   std::queue <time_t> m_quTime;//queue container object
	unsigned int   m_nBufMax;//maximum size of the buffer
	int				m_nTimeOut;//time out in miliseconds
	int            m_nDiscarded;//# of strings discarded since last GetNumStrDiscarded
	wxMutex        m_CritSection;
	std::string		m_sName;
   int            m_nExpTime;//Number of seconds before message expires
   bool           m_bExp;//Determines whether to purge expired messages
   void           DiscradExpired();//set to true to delete old strings
};

//wxWidgets
#include "wx\utils.h"


template <class T> CBuffer<T>::CBuffer(int nBufMax, int nTimeOut, std::string sName,bool bExp,int nExpTime)
    : m_nBufMax(nBufMax),m_nTimeOut(nTimeOut),m_nDiscarded(0),
    m_sName(sName), m_bEmpty(true),m_bExp(bExp),m_nExpTime(nExpTime){;};

template <class T> int CBuffer<T>::GenNumDiscarded()
{//return number of strings that were discarded
    int nDiscarded=m_nDiscarded;
    m_nDiscarded=0;
    return nDiscarded;
}

template <class T> void CBuffer<T>::DiscradExpired()
{
   if (m_bExp)
   {
   	 wxMutexLocker singleLock(m_CritSection);
	    if (!singleLock.IsOk())// lock resource 
            throw std::string("CStrBuffer.AddString timed out on Critical Section");
       std::time_t tDiscard=std::time(NULL)-m_nExpTime;//current UTC time - expiration period
       while(!m_quTime.empty() && m_quTime.front()<tDiscard)
       {
            m_quBuf.pop();
            m_quTime.pop();
       }
       m_bEmpty=m_quBuf.empty();
   }
}

template <class T> void CBuffer<T>::Add(T t)
{//Add string to the buffer
	wxMutexLocker singleLock(m_CritSection);
	if (!singleLock.IsOk())// lock resource 
        throw std::string("CStrBuffer.AddString timed out on Critical Section");
	//.usage of shared resource...
	if (m_quBuf.size()+1>=m_nBufMax) //if buffer is full 
	{
		m_quBuf.pop();//discard oldest string
		if(m_bExp)m_quTime.pop();//discard time
		m_nDiscarded++;
	};
   m_quBuf.push(t);
   if(m_bExp) m_quTime.push(time(0));
	m_bEmpty=false;
};//unlock resource

template <class T> T CBuffer<T>::Get()
{//Get oldest string in the buffer
   if (m_bEmpty) throw std::string("CBuffer<T>::Get called on empty buffer");
	wxMutexLocker singleLock(m_CritSection);//.usage of shared resource..
	if (!singleLock.IsOk())// resource locked
        throw std::string("CStrBuffer.GetString timed out on Critical Section");
   T t=m_quBuf.front();
   m_quBuf.pop();//discard oldest string
	if(m_bExp)m_quTime.pop();//discard time
	m_bEmpty=m_quBuf.empty();
	return t;
};//unlocks resource

template <class T> std::vector<T> CBuffer<T>::GetQueue()
{
	wxMutexLocker singleLock(m_CritSection);//.usage of shared resource..
	if (!singleLock.IsOk())// resource locked
        throw std::string("CStrBuffer.GetString timed out on Critical Section");
   std::queue <T> qu=m_quBuf;
   std::vector<T> v;
   while(!qu.empty())
   {
      v.push_back(qu.front());
      qu.pop();
   }
   return v;
}

#endif // STRBUFFER_H