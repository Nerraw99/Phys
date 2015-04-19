#include "stdafx.h"

#include "Circle.hpp"

#include "Physics.hpp"

#include "Render.hpp"

Scalar GetColorCircleAtOffset( Scalar scOffset )
{
	static Scalar scaControlPoints[6] =
	{
		1.0,
		1.0,
		0.0,
		0.0,
		0.0,
		1.0
	};

	int indexA = scOffset * 6;
	int indexB = ( indexA + 1 ) % 6;

	Scalar scDistance = 1.0 / 6.0;

	Scalar scDistFromA = fmod( scOffset, scDistance );

	Scalar scAAmount = ( scDistance - scDistFromA ) / scDistance;
	Scalar scBAmount = 1.0 - scAAmount;

	return ( scaControlPoints[ indexA ] * scAAmount + scaControlPoints[ indexB ] * scBAmount );

}

void Circle::Draw( ID2D1HwndRenderTarget *pRenderTarget )
{
    // Create a black brush.
	HRESULT hr;

	{
		ID2D1SolidColorBrush *pBlackBrush;

		Scalar scMaxRadius = 15.0;
		Scalar scMinRadius = 1.5;

		Scalar scSpread = CONST_TAU / 3.0;

		//500.0 / scRadius

		Scalar scRatioToMax = pow( ( ( GetRadius() - scMinRadius ) / ( scMaxRadius - scMinRadius ) ), 2.0 );

		Scalar scRed = GetColorCircleAtOffset( fmod( scRatioToMax, 1.0 ) );
		Scalar scGreen = GetColorCircleAtOffset( fmod( scRatioToMax + 0.3333, 1.0 ) );
		Scalar scBlue = GetColorCircleAtOffset( fmod( scRatioToMax + 0.6666, 1.0 ) );

		//Scalar scRed = cos( scRatioToMax * CONST_TAU ) * 0.5 + 0.5;
		//Scalar scGreen = cos( ( scRatioToMax * CONST_TAU + scSpread ) ) * 0.5 + 0.5;
		//Scalar scBlue = cos( ( scRatioToMax * CONST_TAU + ( scSpread * 2.0 ) ) ) * 0.5 + 0.5;

		Scalar scMaxVelocity = 1000.0;

		Scalar scCurrentVelocity = Length( GetVelocity() + ( Physics::GetGravity() * ( Physics::GetSimTime() - m_scTimeOffset ) ) );
		D2D1::ColorF color( scRed, scBlue, scGreen, 1.0 /*1.0 - Min( scCurrentVelocity / scMaxVelocity, 1.0 )*/ );

		hr = pRenderTarget->CreateSolidColorBrush(
			color,
			&pBlackBrush
			);

		Scalar scBlendTime = 0.03;

		Scalar scNumIterations = 1;

		Scalar scInterpTime = scBlendTime / scNumIterations;
		
		//for ( Scalar scTimestepOffset = 0.0; scTimestepOffset < scBlendTime; scTimestepOffset += scInterpTime )
		{
			Scalar scTimestep = Physics::GetSimTime() - m_scTimeOffset;

			Vector vPositionDelta = m_vVelocity * scTimestep + 0.5 * Physics::GetGravity() * scTimestep * scTimestep;

			Vector vNewCenter = m_vCenterOfMass + vPositionDelta;

			m_avHistory[ m_uCurrentHistoryIndex ] = vNewCenter;

			m_uCurrentHistoryIndex = ( m_uCurrentHistoryIndex + 1 ) % 10;

			//for (uint32 uIndex = 0; uIndex < 10; uIndex++ )
			{
				Vector vDrawPos = vNewCenter;

				pRenderTarget->FillEllipse(
					D2D1::Ellipse(
						D2D1::Point2F( (FLOAT) ( vDrawPos.GetX() + m_vCenter.GetX() ), (FLOAT) ( -vDrawPos.GetY() - m_vCenter.GetY() ) ),
						(FLOAT) m_sRadius,
						(FLOAT) m_sRadius),
					pBlackBrush//,
					//3.0
					);
			}
		}

		SafeRelease(&pBlackBrush);
	}

	//{
	//	ID2D1SolidColorBrush *pRedBrush;
	//	hr = pRenderTarget->CreateSolidColorBrush(
	//		D2D1::ColorF(D2D1::ColorF::Red),
	//		&pRedBrush
	//		);

	//	CollisionData *pNextData = NULL;
	//	Scalar scEarliestCollision = 10e10;

	//	PairRecordNode *pPairRecordNode = m_listObjectA.GetHeadPtr();

	//	while ( pPairRecordNode )
	//	{
	//		if ( pPairRecordNode->m_data->collisionData.m_scCollisionTime < scEarliestCollision )
	//		{
	//			pNextData = &( pPairRecordNode->m_data->collisionData );
	//			scEarliestCollision = pPairRecordNode->m_data->collisionData.m_scCollisionTime;
	//		}

	//		pPairRecordNode = pPairRecordNode->GetNext( PhysList_ObjectA );
	//	}

	//	pPairRecordNode = m_listObjectB.GetHeadPtr();

	//	while ( pPairRecordNode )
	//	{
	//		if ( pPairRecordNode->m_data->collisionData.m_scCollisionTime < scEarliestCollision )
	//		{
	//			pNextData = &( pPairRecordNode->m_data->collisionData );
	//			scEarliestCollision = pPairRecordNode->m_data->collisionData.m_scCollisionTime;
	//		}

	//		pPairRecordNode = pPairRecordNode->GetNext( PhysList_ObjectB );
	//	}

	//	if ( pNextData )
	//	{
	//		Scalar scTimestep = pNextData->m_scCollisionTime - m_scTimeOffset;

	//		Vector vPositionDelta = m_vVelocity * scTimestep + 0.5 * Physics::GetGravity() * scTimestep * scTimestep;

	//		Vector vNewCenter = m_vCenterOfMass + vPositionDelta;

	//		pRenderTarget->DrawEllipse(
	//			D2D1::Ellipse(
	//				D2D1::Point2F( (FLOAT) ( vNewCenter.GetX() + m_vCenter.GetX() ), (FLOAT) ( -vNewCenter.GetY() - m_vCenter.GetY() ) ),
	//				(FLOAT) m_sRadius,
	//				(FLOAT) m_sRadius),
	//			pRedBrush,
	//			3.0
	//			);

	//	}

	//	SafeRelease(&pRedBrush);
	//}
}
