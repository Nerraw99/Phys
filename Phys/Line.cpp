#include "stdafx.h"

#include "Line.hpp"

#include "Render.hpp"

Vector Line::GetNormal()
{
	Vector vDiff = m_aVectors[ 1 ] - m_aVectors[ 0 ];

	Scalar scLen = sqrt( vDiff.GetX() * vDiff.GetX() + vDiff.GetY() * vDiff.GetY() );
	
	Vector vNormal( -vDiff.GetY() / scLen, vDiff.GetX() / scLen );

	return vNormal;
}

Scalar g_scRenderingScale = 1.0;

void Line::Draw( ID2D1HwndRenderTarget *pRenderTarget )
{
    // Create a black brush.
	HRESULT hr;
	ID2D1SolidColorBrush *pBlackBrush;

    hr = pRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::Black),
        &pBlackBrush
        );

    pRenderTarget->DrawLine(
        D2D1::Point2F( (FLOAT) ( ( m_vCenterOfMass.GetX() + m_aVectors[ 0 ].GetX() ) * g_scRenderingScale ), 
					   (FLOAT) ( ( -m_vCenterOfMass.GetY() - m_aVectors[ 0 ].GetY() ) * g_scRenderingScale ) ),
        D2D1::Point2F( (FLOAT) ( ( m_vCenterOfMass.GetX() + m_aVectors[ 1 ].GetX() ) * g_scRenderingScale  ), 
					   (FLOAT) ( ( -m_vCenterOfMass.GetY() - m_aVectors[ 1 ].GetY() ) * g_scRenderingScale  ) ),
					   pBlackBrush,
					   3.0 );

    SafeRelease(&pBlackBrush);
}


