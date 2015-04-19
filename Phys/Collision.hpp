#pragma once

#ifndef __COLLISION_HPP__
#define __COLLISION_HPP__

#include "Types.hpp"
#include "Vector.hpp"

class PhysicsObject;

typedef struct
{
	Vector m_vObjectAImpulse;
	Vector m_vObjectBImpulse;
	Scalar m_scCollisionTime;
} CollisionData;


bool GetCollisionTime( PhysicsObject *pObjectA, PhysicsObject *pObjectB, CollisionData &collisionData );


#endif // __COLLISION_HPP__