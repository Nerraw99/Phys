#include "stdafx.h"

#include "PhysicsObject.hpp"

#include "Physics.hpp"

void PhysicsObject::RemoveAllRecords( Physics *pPhysics )
{
	PairRecordNode *pPairRecordNode = m_listObjectA.GetHeadPtr();

	while ( pPairRecordNode )
	{
		pPhysics->RemoveRecord( pPairRecordNode );

		pPairRecordNode = m_listObjectA.GetHeadPtr();
	}

	pPairRecordNode = m_listObjectB.GetHeadPtr();

	while ( pPairRecordNode )
	{
		pPhysics->RemoveRecord( pPairRecordNode );

		pPairRecordNode = m_listObjectB.GetHeadPtr();
	}
}