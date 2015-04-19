#pragma once

#include "Types.hpp"
#include "Vector.hpp"

#include "PairRecord.hpp"

struct ID2D1HwndRenderTarget;
class Physics;


class PhysicsObject
{
public:

	enum PhysicsObjectTypeEnum
	{
		POT_Triangle,
		POT_Line,
		POT_Circle,
		POT_NUMTYPES
	};


	PhysicsObject( PhysicsObjectTypeEnum eType ) : m_fMass( 1.0f ), m_vCenterOfMass( 0.0f, 0.0f ), m_vVelocity( 0.0f, 0.0f ), m_fSpin( 0.0f ), m_eType( eType ), m_bIsStatic( false ), m_scTimeOffset( NULL ), m_uCurrentHistoryIndex( 0 ) {}
	virtual ~PhysicsObject() {}

	virtual void Draw( ID2D1HwndRenderTarget *pRenderTarget ) {}
	virtual void UpdatePosition( Scalar scTimestep, Vector vGravity );

	PhysicsObjectTypeEnum GetType() { return m_eType; }

	void SetCenterOfMass( Vector vCenterOfMass ) { m_vCenterOfMass = vCenterOfMass; }
	Vector GetCenterOfMass() { return m_vCenterOfMass; }

	Vector GetVelocity() { return m_vVelocity; }
	void SetVelocity( Vector vVelocity ) { m_vVelocity = vVelocity; }

	Scalar GetMass() { return m_fMass; }
	void SetMass( Scalar scMass ) { m_fMass = scMass; }

	void SetStatic( bool bIsStatic ) { m_bIsStatic = bIsStatic; }
	bool IsStatic() { return m_bIsStatic; }

	void ApplyImpulse( Vector vImpulse ) { m_vVelocity += vImpulse; }

	Scalar GetTimeOffset() { return m_scTimeOffset; }
	void SetTimeOffset( Scalar scTimeOffset ) { m_scTimeOffset = scTimeOffset; }

	void AddObjectA( PairRecordNode &pairRecordNode ) { m_listObjectA.Add( pairRecordNode ); }
	void RemoveObjectA( PairRecordNode &pairRecordNode ) { m_listObjectA.Remove( pairRecordNode ); }

	void AddObjectB( PairRecordNode &pairRecordNode ) { m_listObjectB.Add( pairRecordNode ); }
	void RemoveObjectB( PairRecordNode &pairRecordNode ) { m_listObjectB.Remove( pairRecordNode ); }

	void RemoveAllRecords( Physics *pPhysics );

protected:
	Vector m_vCenterOfMass;
	Vector m_vVelocity;
	Scalar m_fMass;
	Scalar m_fSpin;

	Vector m_avHistory[10];
	uint32 m_uCurrentHistoryIndex;

	List< PairRecord *, PhysList_Count, PhysList_ObjectA > m_listObjectA;
	List< PairRecord *, PhysList_Count, PhysList_ObjectB > m_listObjectB;

	bool m_bIsStatic;

	Scalar m_scTimeOffset;

	PhysicsObjectTypeEnum m_eType;
};
