#ifndef __VECTOR_HPP__
#define __VECTOR_HPP__

#include <math.h>

class Vector2
{
public:
	Vector2() {}
	Vector2( Scalar x, Scalar y ) { m_fX = x; m_fY = y; }

	void Set( Scalar scX, Scalar scY ) { m_fX = scX; m_fY = scY; }

	Scalar GetX() const { return m_fX; }
	void SetX( Scalar fX ) { m_fX = fX; }

	Scalar GetY() const { return m_fY; }
	void SetY( Scalar fY ) { m_fY = fY; }

	Vector2 operator+( const Vector2 &other ) const
	{
		Vector2 result( m_fX + other.m_fX, m_fY + other.m_fY );

		return result;
	}

	Vector2 operator-( const Vector2 &other ) const
	{
		Vector2 result( m_fX - other.m_fX, m_fY - other.m_fY );

		return result;
	}

	Vector2 operator-()
	{
		return Vector2( -m_fX, -m_fY );
	}

	const Vector2 &operator+=( const Vector2 &other )
	{
		m_fX += other.m_fX;
		m_fY += other.m_fY;

		return *this;
	}

	const Vector2 &operator *=( const Scalar scMultiplier )
	{
		m_fX *= scMultiplier;
		m_fY *= scMultiplier;

		return *this;
	}
	
	//Vector2 operator *( const Vector2 &v1, const Vector2 &v2 )
	//{
	//	Vector2 result( v1.GetX() * v2.GetX(), v1.GetY() * v2.GetY() );

	//	return result;
	//}
protected:
	Scalar m_fX;
	Scalar m_fY;
};


inline Vector2 operator*( Scalar scalar, const Vector2 &vector )
{
	Vector2 result( vector.GetX() * scalar, vector.GetY() * scalar );

	return result;
}

inline Vector2 operator*( const Vector2 &vector, Scalar scalar )
{
	return operator*( scalar, vector );
}

inline Vector2 operator/( const Vector2 &vector, Scalar scalar )
{
	Vector2 result( vector.GetX() / scalar, vector.GetY() / scalar );

	return result;
}

inline Scalar DotProduct( const Vector2 &vectorA, const Vector2 &vectorB )
{
	return vectorA.GetX() * vectorB.GetX() + vectorA.GetY() * vectorB.GetY();
}

inline Scalar Length( const Vector2 &vector )
{
	return sqrt( vector.GetX() * vector.GetX() + vector.GetY() * vector.GetY() );
}

inline Vector Normalize( const Vector2 &vector )
{
	const Scalar scLength = Length( vector );

	return Vector( vector.GetX() / scLength, vector.GetY() / scLength );
}

inline Scalar Clamp( const Scalar &input, const Scalar &min, const Scalar &max )
{
	return Min( min, Max( input, max ) );
}

#endif // __VECTOR_HPP__