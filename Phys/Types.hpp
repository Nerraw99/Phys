#pragma once

#ifndef __TYPES_HPP__
#define __TYPES_HPP__

class Vector2;

typedef double Scalar;
typedef Vector2 Vector;
typedef unsigned int uint32;

template< class Type >
Type Min( const Type &a, const Type &b ) { return ( a < b ) ? a : b; }

template< class Type >
Type Max( const Type &a, const Type &b ) { return ( a > b ) ? a : b; }


static const Scalar CONST_TAU = 3.1415923 * 2.0;




#endif // __TYPES_HPP__