
#ifndef __itkSkeletonConnectivity_h
#define __itkSkeletonConnectivity_h

#include "itkImage.h"
#include "itkConstShapedNeighborhoodIterator.h"
#include "itkShapedNeighborhoodIterator.h"

// set up a shaped iterator by specifying the minimum number of zeros in each offset
// a value of 0 corresponds to 8 or 26 connected in 2 or 3 D. A value of 1 corresponds 
// to 4 connected in 2D and 18 connected in 3D. A value of 2 corresponds to 6 connected in 3D

namespace itk
{
	template< class TIterator >
	TIterator*
	setCellConnectivity( TIterator* it, int zeros = 0)
	{
		typename TIterator::OffsetType offset;
		it->ClearActiveList();
		unsigned int centerIndex = it->GetCenterNeighborhoodIndex();
		for( unsigned int d=0; d < centerIndex*2 + 1; d++ )
		{
			offset = it->GetOffset( d );
			int zcount = std::count(offset.begin(), offset.end(), 0);
			if (zcount >= zeros)
			{
				it->ActivateOffset( offset );
			}
		}
		
		offset.Fill(0);
		it->DeactivateOffset( offset );
		
		return it;
	}
	
	
	
}


#endif
