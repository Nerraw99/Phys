#pragma once

#include "PhysicsObject.hpp"

class Circle : public PhysicsObject
{
public:

	Circle( const Vector2 &vCenter, const Scalar sRadius )
		: PhysicsObject( POT_Circle )
		, m_vCenter( 0.0f, 0.0f )
		, m_sRadius( sRadius )
	{
		SetCenterOfMass( vCenter );
	}

	void SetCenter( const Vector2 &vCenter );
	Vector2 &GetCenter() { return m_vCenter; }

	virtual void Draw( ID2D1HwndRenderTarget *pRenderTarget );

	virtual PhysicsObjectTypeEnum GetType() { return POT_Circle; }

	Scalar GetRadius() { return m_sRadius; }
	

protected:
	Vector2 m_vCenter;
	Scalar m_sRadius;
};
