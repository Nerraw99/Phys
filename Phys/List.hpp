#pragma once

#ifndef __LIST_HPP__
#define __LIST_HPP__

#include "Types.hpp"

template< class Type, int NumLists = 1 >
class Node
{
public:

	typedef Node< Type, NumLists > * NodePointer;

	Node() {}
	Node( Type data ) 
		: m_data( data )
	{}

	Type m_data;

	NodePointer GetPrev( int iListIndex = 0 ) { return m_pPrev[ iListIndex ]; }
	NodePointer GetNext( int iListIndex = 0 ) { return m_pNext[ iListIndex ]; }

	void SetPrev( NodePointer pPrev, int iListIndex = 0 ) { m_pPrev[ iListIndex ] = pPrev; }
	void SetNext( NodePointer pNext, int iListIndex = 0 ) { m_pNext[ iListIndex ] = pNext; }

private:
	NodePointer m_pPrev[ NumLists ];
	NodePointer m_pNext[ NumLists ];
};

template< class Type, int NumLists = 1, int ListIndex = 0 >
class List
{
public:
	List() : m_pHead( NULL ), m_pTail( NULL ), m_uNodeCount( 0 ) {}

	inline void Add( Node< Type, NumLists > &nodeToAdd );

	inline void InsertAfter( Node< Type, NumLists > &nodeToAdd, Node< Type, NumLists > *pPrevNode );
	inline void InsertBefore( Node< Type, NumLists > &nodeToAdd, Node< Type, NumLists > *pNextNode );

	inline void Remove( Node< Type, NumLists > &nodeToRemove );

	inline Node< Type, NumLists > *GetHeadPtr() { return m_pHead; }

protected:
	Node< Type, NumLists > *m_pHead;
	Node< Type, NumLists > *m_pTail;

	uint32 m_uNodeCount;
};


template< class Type, int NumLists, int ListIndex >
void List< Type, NumLists, ListIndex >::Add( Node< Type, NumLists > &nodeToAdd )
{
	if ( m_pTail )
	{
		nodeToAdd.SetPrev( m_pTail, ListIndex );
		nodeToAdd.SetNext( NULL, ListIndex );
		m_pTail->SetNext( &nodeToAdd, ListIndex );
		m_pTail = &nodeToAdd;
	}
	else
	{
		nodeToAdd.SetPrev( NULL, ListIndex );
		nodeToAdd.SetNext( NULL, ListIndex );
		m_pHead = &nodeToAdd;
		m_pTail = &nodeToAdd;
	}

	m_uNodeCount++;
}

template< class Type, int NumLists, int ListIndex >
void List< Type, NumLists, ListIndex >::InsertAfter( Node< Type, NumLists > &nodeToAdd, Node< Type, NumLists > *pPrevNode )
{
	Node< Type > *pNext = pPrevNode.m_pNext;
	nodeToAdd.SetPrev( pPrevNode, ListIndex );
	nodeToAdd.SetNext( pNext, ListIndex );

	if ( pPrevNode )
	{
		pPrevNode->SetNext( &nodeToAdd, ListIndex );
	}
	else
	{
		m_pHead = &nodeToAdd;
	}

	if ( pNext )
	{
		pNext->SetPrev( &nodeToAdd, ListIndex );
	}
	else
	{
		m_pTail = &nodeToAdd;
	}
	
	m_uNodeCount++;
}

template< class Type, int NumLists, int ListIndex >
void List< Type, NumLists, ListIndex >::InsertBefore( Node< Type, NumLists > &nodeToAdd, Node< Type, NumLists > *pNextNode )
{
	Node< Type, NumLists > *pPrev = pNextNode->GetPrev( ListIndex );
	nodeToAdd.SetPrev( pPrev, ListIndex );
	nodeToAdd.SetNext( pNextNode, ListIndex );

	if ( pPrev )
	{
		pPrev->SetNext( &nodeToAdd, ListIndex );
	}
	else
	{
		m_pHead = &nodeToAdd;
	}

	if ( pNextNode )
	{
		pNextNode->SetPrev( &nodeToAdd );
	}
	else
	{
		m_pTail = &nodeToAdd;
	}
	
	m_uNodeCount++;
}

template< class Type, int NumLists, int ListIndex >
void List< Type, NumLists, ListIndex >::Remove( Node< Type, NumLists > &nodeToRemove )
{
	Node< Type, NumLists > *pNext = nodeToRemove.GetNext( ListIndex );
	Node< Type, NumLists > *pPrev = nodeToRemove.GetPrev( ListIndex );

	if ( pNext )
	{
		pNext->SetPrev( pPrev, ListIndex );
	}
	else
	{
		m_pTail = pPrev;
	}

	if ( pPrev )
	{
		pPrev->SetNext( pNext, ListIndex );
	}
	else
	{
		m_pHead = pNext;
	}
	
	m_uNodeCount--;
}


#endif // __LIST_HPP__