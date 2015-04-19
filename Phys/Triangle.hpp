#pragma once

#include "PhysicsObject.hpp"


class Triangle : public PhysicsObject
{
public:

	Triangle() : PhysicsObject( POT_Triangle ) {}

	void SetVector( uint32 uIndex, Vector vector );

	virtual PhysicsObjectTypeEnum GetType() { return POT_Triangle; }
	
protected:
	Vector m_aVectors[ 3 ];
};

