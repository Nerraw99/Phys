#include "stdafx.h"

#include "Collision.hpp"

#include "Line.hpp"
#include "Circle.hpp"
#include "Triangle.hpp"

#include "Physics.hpp"

template< class TypeA, class TypeB >
bool GetCollisionTime( PhysicsObject *pObjectA, PhysicsObject *pObjectB, CollisionData &collisionData )
{
	return false;
}


#define DEFINE_GET_COLLISION_TIME_SINGLE( Type )													\
bool GetCollisionTime##Type##Type( Type *, Type *, CollisionData &collisionData );	\
																									\
template<>																							\
bool GetCollisionTime< Type, Type >( PhysicsObject *pObjectA, PhysicsObject *pObjectB, CollisionData &collisionData )	\
{																									\
	Type *pObjTypeA = static_cast< Type * >( pObjectA );											\
	Type *pObjTypeB = static_cast< Type * >( pObjectB );											\
	return GetCollisionTime##Type##Type( pObjTypeA, pObjTypeB, collisionData );			\
}																									\
																									\

#define DEFINE_GET_COLLISION_TIME( TypeA, TypeB )													\
bool GetCollisionTime##TypeA##TypeB( TypeA *, TypeB *, CollisionData &collisionData );	\
																									\
template<>																							\
bool GetCollisionTime< TypeA, TypeB >( PhysicsObject *pObjectA, PhysicsObject *pObjectB, CollisionData &collisionData )	\
{																									\
	TypeA *pObjTypeA = static_cast< TypeA * >( pObjectA );											\
	TypeB *pObjTypeB = static_cast< TypeB * >( pObjectB );											\
	return GetCollisionTime##TypeA##TypeB( pObjTypeA, pObjTypeB, collisionData );		\
}																									\
																									\
template<>																							\
bool GetCollisionTime< TypeB, TypeA >( PhysicsObject *pObjectA, PhysicsObject *pObjectB, CollisionData &collisionData )	\
{																									\
	TypeA *pObjTypeA = static_cast< TypeA * >( pObjectB );											\
	TypeB *pObjTypeB = static_cast< TypeB * >( pObjectA );											\
	bool bResult = GetCollisionTime##TypeA##TypeB( pObjTypeA, pObjTypeB, collisionData );	\
	Vector vTemp = collisionData.m_vObjectAImpulse;													\
	collisionData.m_vObjectAImpulse = collisionData.m_vObjectBImpulse;								\
	collisionData.m_vObjectBImpulse = vTemp;														\
	return bResult;																					\
}


DEFINE_GET_COLLISION_TIME( Line, Circle );

bool GetCollisionTimeLineCircle( Line *pLine, Circle *pCircle, CollisionData &collisionData )
{
	Scalar scRelativeCollisionTime = -1.0;

	Vector vRelativeVelocity = pCircle->GetVelocity() - pLine->GetVelocity();

	Vector vLineNormal = pLine->GetNormal();

	const Vector vLineToCircle = ( pCircle->GetCenterOfMass() + pCircle->GetCenter() ) - ( pLine->GetCenterOfMass() + pLine->GetPosition( 0 ) );

	const Scalar scDistToLine = DotProduct( vLineNormal, vLineToCircle );

	/*if ( scDistToLine < pCircle->GetRadius() )
	{
		if ( DotProduct( vLineNormal, vRelativeVelocity ) <= 0.0 )
		{
			scRelativeCollisionTime = 0.0; // HACK HACK HACK... if penetrating and moving toward the line, register a collision...
		}
		else
		{
			return false;
		}
	}
	else*/
	{
		const Scalar sc2A = DotProduct( Physics::GetGravity(), vLineNormal );

		if ( abs( sc2A ) > 0.0 )
		{
			const Scalar scB = DotProduct( vRelativeVelocity, vLineNormal );

			const Vector vLineToCircle = ( pCircle->GetCenterOfMass() + pCircle->GetCenter() ) - ( pLine->GetCenterOfMass() + pLine->GetPosition( 0 ) );
			const Scalar scC = DotProduct( vLineToCircle, vLineNormal ) - pCircle->GetRadius();

			const Scalar scDiscriminant = sqrt( ( scB * scB ) - ( 2.0 * sc2A * scC ) );

			const Scalar scT1 = ( -scB + scDiscriminant ) / sc2A;
			const Scalar scT2 = ( -scB - scDiscriminant ) / sc2A;

			const Vector vVelocityT1 = pCircle->GetVelocity() + Physics::GetGravity() * scT1;
			const Vector vVelocityT2 = pCircle->GetVelocity() + Physics::GetGravity() * scT2;

			const Scalar scDotT1 = DotProduct( vLineNormal, vVelocityT1 );
			const Scalar scDotT2 = DotProduct( vLineNormal, vVelocityT2 );

			if ( ( scT1 > 0.0 ) && ( scDotT1 < 0.0 ) )
			{
				if ( ( scT2 > 0.0 ) && ( scDotT2 < 0.0 ) )
				{
					scRelativeCollisionTime = Min( scT1, scT2 );
				}
				else
				{
					scRelativeCollisionTime = scT1;
				}
			}
			else if ( ( scT2 > 0.0 ) && ( scDotT2 < 0.0 ) )
			{
				scRelativeCollisionTime = scT2;
			}
		}
		else
		{
			//Scalar scCollisionTime = pCircle->GetRadius() - DotProduct( ( pCircle->GetCenterOfMass() + pCircle->GetCenter() + ( pLine->GetCenterOfMass() + pLine->GetPosition( 0 ) ) ), vLineNormal ) / 
				//DotProduct( vRelativeVelocity, vLineNormal );
			Scalar scCollisionTime = ( -DotProduct( pCircle->GetCenterOfMass() + pCircle->GetCenter(), vLineNormal ) + DotProduct( pLine->GetCenterOfMass() + pLine->GetPosition( 0 ), vLineNormal ) + pCircle->GetRadius() ) / 
									 DotProduct( vRelativeVelocity, vLineNormal );

			const Vector vVelocityAtIntersect = pCircle->GetVelocity() + Physics::GetGravity() * scCollisionTime;

			const Scalar scDot = DotProduct( vLineNormal, vVelocityAtIntersect );

			if ( ( scCollisionTime > 0.0 ) && ( scDot < 0.0 ) )
			{
				scRelativeCollisionTime = scCollisionTime;
			}
		}
	}

	if ( scRelativeCollisionTime > 0.0 )
	{
		// assume line has infinite mass
		// circle will reflect 100% in the normal
		collisionData.m_scCollisionTime = pCircle->GetTimeOffset() + scRelativeCollisionTime;

		collisionData.m_vObjectAImpulse.Set( 0.0, 0.0 );

		Vector vVelocityAtImpact = pCircle->GetVelocity() + Physics::GetGravity() * scRelativeCollisionTime;

		collisionData.m_vObjectBImpulse = ( DotProduct( vVelocityAtImpact, vLineNormal ) * -2.0 ) * vLineNormal;

		return true;
	}

	return false;
}

DEFINE_GET_COLLISION_TIME_SINGLE( Circle );


bool GetCollisionTimeCircleCircle( Circle *pCircleA, Circle *pCircleB, CollisionData &collisionData )
{
	Scalar scCollisionTime = -1.0;

	const bool bAIsFirst = ( pCircleA->GetTimeOffset() < pCircleB->GetTimeOffset() );

	Circle *pFirst, *pSecond;

	if ( bAIsFirst )
	{
		pFirst = pCircleA;
		pSecond = pCircleB;
	}
	else
	{
		pFirst = pCircleB;
		pSecond = pCircleA;
	}

	// find all values relative to the second or later time obstacle.
	const Scalar scTotalRadius = pFirst->GetRadius() + pSecond->GetRadius();

	const Scalar scRelativeTime = pSecond->GetTimeOffset() - pFirst->GetTimeOffset();

	const Vector vFirstPosition = ( pFirst->GetCenterOfMass() + pFirst->GetCenter() );
	const Vector vVelocityFirstRelative = pFirst->GetVelocity() + Physics::GetGravity() * scRelativeTime;
	const Vector vPositionFirstRelative = vFirstPosition + pFirst->GetVelocity() * scRelativeTime + 
									  0.5 * Physics::GetGravity() * scRelativeTime * scRelativeTime;

	const Vector vRelativeVelocity = vVelocityFirstRelative - pSecond->GetVelocity();

	const Vector vRelativePosition = ( pSecond->GetCenterOfMass() + pSecond->GetCenter() ) - vPositionFirstRelative;
	

	/*if ( Length( vRelativePosition ) < scTotalRadius )
	{
		if ( DotProduct( vRelativePosition, vRelativeVelocity ) > 0.0f )
		{
			collisionData.m_scCollisionTime = 0.0; // HACK -- if already penetrating and moving towards eachother register a collision.  Effectively prevents further penetration...
		}
		else
		{
			return false;
		}
	}
	else*/
	{
		const Scalar sc2A = DotProduct( vRelativeVelocity, vRelativeVelocity ) * 2.0;

		if ( abs( sc2A ) > 0.0 )
		{
			const Scalar scB = -2.0 * DotProduct( vRelativePosition, vRelativeVelocity );

			const Scalar scC = DotProduct( vRelativePosition, vRelativePosition ) - ( scTotalRadius * scTotalRadius );

			const Scalar scDiscriminant = sqrt( ( scB * scB ) - ( 2.0 * sc2A * scC ) );

			const Scalar scT1 = ( -scB + scDiscriminant ) / sc2A;
			const Scalar scT2 = ( -scB - scDiscriminant ) / sc2A;

			const Vector vRelativePositionT1 = vRelativeVelocity * scT1;
			const Vector vRelativePositionT2 = vRelativeVelocity * scT2;

			const Vector vNormalT1 = Normalize( vRelativePositionT1 - vRelativePosition );
			const Vector vNormalT2 = Normalize( vRelativePositionT2 - vRelativePosition );

			const Scalar scDotT1 = DotProduct( vNormalT1, vRelativeVelocity );
			const Scalar scDotT2 = DotProduct( vNormalT2, vRelativeVelocity );

			if ( ( scT1 > 0.0 ) && ( scDotT1 < 0.0 ) )
			{
				if ( ( scT2 > 0.0 ) && ( scDotT2 < 0.0 ) )
				{
					scCollisionTime = Min( scT1, scT2 );
				}
				else
				{
					scCollisionTime = scT1;
				}
			}
			else if ( ( scT2 > 0.0 ) && ( scDotT2 < 0.0 ) )
			{
				scCollisionTime = scT2;
			}
		}
	}

	if ( scCollisionTime > 0.0 )
	{
		collisionData.m_scCollisionTime = pSecond->GetTimeOffset() + scCollisionTime;

		Vector vRelativeIntersection = vRelativeVelocity * scCollisionTime;

		Vector vNormal = Normalize( vRelativeIntersection - vRelativePosition );

		Vector vRelativeImpulse = DotProduct( vRelativeVelocity, vNormal ) * vNormal * 2.00;

		Scalar scTotalMass = pFirst->GetMass() + pSecond->GetMass();

		if ( bAIsFirst )
		{
			collisionData.m_vObjectAImpulse = vRelativeImpulse * -( pCircleB->GetMass() / scTotalMass );
			collisionData.m_vObjectBImpulse = vRelativeImpulse * ( pCircleA->GetMass() / scTotalMass );
		}
		else
		{
			collisionData.m_vObjectAImpulse = vRelativeImpulse * ( pCircleB->GetMass() / scTotalMass );
			collisionData.m_vObjectBImpulse = vRelativeImpulse * -( pCircleA->GetMass() / scTotalMass );
		}

		return true;
	}

	return false;
}


typedef bool ( *fnGetCollisionTime )( PhysicsObject *, PhysicsObject *, CollisionData &collisionData );

fnGetCollisionTime aGetCollisionTimeFns[ PhysicsObject::POT_NUMTYPES ][ PhysicsObject::POT_NUMTYPES ] =
{
	{
		GetCollisionTime< Triangle, Triangle >,
		GetCollisionTime< Triangle, Line >,
		GetCollisionTime< Triangle, Circle >,
	},
	{
		GetCollisionTime< Line, Triangle >,
		GetCollisionTime< Line, Line >,
		GetCollisionTime< Line, Circle >,
	},
	{
		GetCollisionTime< Circle, Triangle >,
		GetCollisionTime< Circle, Line >,
		GetCollisionTime< Circle, Circle >,
	}
};

bool GetCollisionTime( PhysicsObject *pObjectA, PhysicsObject *pObjectB, CollisionData &collisionData )
{
	fnGetCollisionTime GetCollisionTimeFn = aGetCollisionTimeFns[ pObjectA->GetType() ][ pObjectB->GetType() ];
	
	return GetCollisionTimeFn( pObjectA, pObjectB, collisionData );
}


