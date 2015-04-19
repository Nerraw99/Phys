#include "StdAfx.h"

#include "Physics.hpp"

#include <math.h>

#include "PhysicsObject.hpp"

Vector Physics::m_vGravity( 0.0, -500.81 );
//Vector Physics::m_vGravity( 0.0, 0.0 );

Scalar Physics::m_scSimTime = 0.0;



void Physics::Update( Scalar scTimestep )
{
	m_scSimTime += scTimestep;

	UpdateCollisions();
}


void Physics::UpdateCollisions()
{
	static const uint32 kiMaxIterations = 10000;

	uint32 iIteration = 0;
	Scalar scLastCollisionTime = 0.0;

	PairRecordNode *pNode = m_listRecords.GetHeadPtr();

	while ( pNode && ( iIteration < kiMaxIterations ) )
	{
		PairRecord *pPairRecord = pNode->m_data;

		const Scalar scCollisionTime = pPairRecord->collisionData.m_scCollisionTime;
		if ( scCollisionTime < m_scSimTime )
		{
			PhysicsObject *pObjectA = pPairRecord->pObjectA;
			PhysicsObject *pObjectB = pPairRecord->pObjectB;

			pObjectA->UpdatePosition( scCollisionTime - pObjectA->GetTimeOffset(), m_vGravity );
			pObjectB->UpdatePosition( scCollisionTime - pObjectB->GetTimeOffset(), m_vGravity );

			pObjectA->ApplyImpulse( pPairRecord->collisionData.m_vObjectAImpulse );
			pObjectB->ApplyImpulse( pPairRecord->collisionData.m_vObjectBImpulse );

			pObjectA->SetTimeOffset( scCollisionTime );
			pObjectB->SetTimeOffset( scCollisionTime );

			pObjectA->RemoveAllRecords( this );
			pObjectB->RemoveAllRecords( this );

			const Scalar scMinVelocity = 0.001;

			if ( Length( pObjectA->GetVelocity() ) < scMinVelocity )
			{
				pObjectA->SetStatic( true );
			}

			if ( Length( pObjectB->GetVelocity() ) < scMinVelocity )
			{
				pObjectB->SetStatic( true );
			}

			FindCollisionTimes( pObjectA );
			FindCollisionTimes( pObjectB );
			
			iIteration++;
			scLastCollisionTime = scCollisionTime;

			m_uNumCollisions++;

			pNode = m_listRecords.GetHeadPtr();
		}
		else
		{
			break;
		}
	}

	if ( iIteration == kiMaxIterations )
	{
		m_scSimTime = scLastCollisionTime;
	}
}

void Physics::AddObject( PhysicsObject *pObject )
{
	if ( m_uNumObjects < PHYSICS_MAX_OBJECTS )
	{
		m_aObjects[ m_uNumObjects ] = pObject;
		m_uNumObjects++;
	}

	FindCollisionTimes( pObject );
}

void Physics::FindCollisionTimes( PhysicsObject *pObject )
{
	for ( uint32 uIndex = 0; uIndex < m_uNumObjects; uIndex++ )
	{
		PhysicsObject *pOther = m_aObjects[ uIndex ];

		if ( pOther != pObject )
		{
			CollisionData collisionData;

			bool bCollision = GetCollisionTime( pObject, pOther, collisionData );

			if ( bCollision )
			{
				CreateRecord( pObject, pOther, collisionData );
			}
		}
	}
}

void Physics::CreateRecord( PhysicsObject *pObjectA, PhysicsObject *pObjectB, CollisionData &collisionData )
{
	AddPairRecord( pObjectA, pObjectB, collisionData );
}

void Physics::RemoveRecord( PairRecordNode *pNodeToRemove )
{
	PairRecord *pPairRecord = pNodeToRemove->m_data;

	pPairRecord->pObjectA->RemoveObjectA( *pNodeToRemove );
	pPairRecord->pObjectB->RemoveObjectB( *pNodeToRemove );

	RemovePairRecord( pNodeToRemove );
}


void Physics::AddPairRecord( PhysicsObject *pObjectA, PhysicsObject *pObjectB, CollisionData &collisionData )
{
	PairRecord *pNewRecord = new PairRecord( pObjectA, pObjectB, collisionData );

	PairRecordNode *pNewNode = new PairRecordNode( pNewRecord );

	pObjectA->AddObjectA( *pNewNode );
	pObjectB->AddObjectB( *pNewNode );

	PairRecordNode *pCurrentNode = m_listRecords.GetHeadPtr();

	while ( pCurrentNode )
	{
		if ( collisionData.m_scCollisionTime < pCurrentNode->m_data->collisionData.m_scCollisionTime )
		{
			m_listRecords.InsertBefore( *pNewNode, pCurrentNode );
			return;
		}

		pCurrentNode = pCurrentNode->GetNext( PhysList_Main );
	};

	m_listRecords.Add( *pNewNode );
}

void Physics::RemovePairRecord( PairRecordNode *pNodeToRemove )
{
	m_listRecords.Remove( *pNodeToRemove );

	delete pNodeToRemove->m_data;
	delete pNodeToRemove;
}

void Physics::Draw( ID2D1HwndRenderTarget *pRenderTarget )
{
	for ( uint32 uIndex = 0; uIndex < m_uNumObjects; uIndex++ )
	{
		m_aObjects[ uIndex ]->Draw( pRenderTarget );
	}
}


