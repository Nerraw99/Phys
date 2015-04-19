#pragma once

#include "PhysicsObject.hpp"

class Line : public PhysicsObject
{
public:
	Line( Vector vA, Vector vB, Vector vCenterOfMass ) : PhysicsObject( POT_Line ) { m_aVectors[ 0 ] = vA; m_aVectors[ 1 ] = vB; SetCenterOfMass( vCenterOfMass ); }

	void SetVector( uint32 uIndex, Vector vector );

	virtual void Draw( ID2D1HwndRenderTarget *pRenderTarget );

	virtual PhysicsObjectTypeEnum GetType() { return POT_Triangle; }

	Vector &GetPosition( uint32 uIndex ) { return m_aVectors[ uIndex ]; }

	Vector GetNormal();

protected:
	Vector m_aVectors[ 2 ];
};
