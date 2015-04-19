#ifndef __PHYSICS_HPP__
#define __PHYSICS_HPP__

#include "Types.hpp"
#include "Vector.hpp"
#include "List.hpp"

#include "PairRecord.hpp"

#define PHYSICS_MAX_OBJECTS 10000
#define PHYSICS_MAX_PAIRS 10000
#define PHYSICS_MAX_COLLISIONS 10000

struct ID2D1HwndRenderTarget;

class PhysicsObject;


class Physics
{
public:
	Physics() : m_uNumObjects( 0 ), m_uNumCollisions( 0 ) {}

	void AddObject( PhysicsObject *pObject );
	void FindCollisionTimes( PhysicsObject *pObject );
	void CreateRecord( PhysicsObject *pObjectA, PhysicsObject *pObjectB, CollisionData &collisionData );
	void RemoveRecord( PairRecordNode *pNodeToRemove );

	void Update( Scalar scTimestep );

	void UpdateCollisions();

	void Draw( ID2D1HwndRenderTarget *pRenderTarget );

	static Vector &GetGravity() { return m_vGravity; }

	static Scalar &GetSimTime() { return m_scSimTime; }

	uint32 GetNumCollisions() { return m_uNumCollisions; }

protected:

	void AddPairRecord( PhysicsObject *pObjectA, PhysicsObject *pObjectB, CollisionData &collisionData );
	void RemovePairRecord( PairRecordNode *pNodeToRemove );

	PhysicsObject *m_aObjects[ PHYSICS_MAX_OBJECTS ];
	uint32 m_uNumObjects;

	uint32 m_uNumCollisions;

	List< PairRecord *, PhysList_Count, PhysList_Main > m_listRecords;

	static Vector m_vGravity;

	static Scalar m_scSimTime;
};



#endif // __PHYSICS_HPP__