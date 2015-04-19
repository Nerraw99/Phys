#include "stdafx.h"

#include "Triangle.hpp"


void PhysicsObject::UpdatePosition( Scalar scTimestep, Vector vGravity )
{
	if ( !m_bIsStatic )
	{
		Vector vPositionDelta = m_vVelocity * scTimestep + 0.5 * vGravity * scTimestep * scTimestep;
		
		m_vVelocity = m_vVelocity + vGravity * scTimestep;

		m_vCenterOfMass += vPositionDelta;
	}
	else
	{
		m_vVelocity.Set( 0.0, 0.0 );
	}
}
