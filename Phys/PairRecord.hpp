#pragma once

#include "List.hpp"

#include "Collision.hpp"

class PairRecord
{
public:
	PairRecord( PhysicsObject *pObjA, PhysicsObject *pObjB, CollisionData &colData )
		: pObjectA( pObjA )
		, pObjectB( pObjB )
		, collisionData( colData )
	{}

	PhysicsObject *pObjectA;
	PhysicsObject *pObjectB;
	CollisionData collisionData;
};


enum PhysListEnum
{
	PhysList_Main,
	PhysList_ObjectA,
	PhysList_ObjectB,
	PhysList_Count
};


typedef Node< PairRecord *, PhysList_Count > PairRecordNode;

