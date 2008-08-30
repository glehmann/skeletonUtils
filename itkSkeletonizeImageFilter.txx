#ifndef itkSkeletonizationImageFilter_txx
#define itkSkeletonizationImageFilter_txx

#include <itkImageRegionConstIteratorWithIndex.h>
#include <itkNumericTraits.h>
#include <itkProgressReporter.h>

#include "itkSkeletonizeImageFilter.h"


namespace itk
{
	
	
	
	template<class TImage, class TOrderImage>
	SkeletonizeImageFilter<TImage, TOrderImage>
	::SkeletonizeImageFilter()
	{
		this->SetNumberOfRequiredInputs(2);
		m_ForegroundValue = NumericTraits<InputPixelType>::max();
		m_BackgroundValue = NumericTraits<InputPixelType>::Zero;
		// need some code to set up default connectivities
	}
	
	
	template<class TImage, class TOrderImage>
	void 
	SkeletonizeImageFilter<TImage, TOrderImage>
	::PrintSelf(std::ostream& os, Indent indent) const
	{
		Superclass::PrintSelf(os,indent);
		os << indent 
		
		<< "Cell dimension used for foreground connectivity: " 
		
		<<  m_ForegroundCellConnectivity << std::endl;
		os << indent << "ForegroundValue: " << static_cast<typename NumericTraits<InputPixelType>::PrintType>(m_ForegroundValue) << std::endl;
		os << indent << "BackgroundValue: " << static_cast<typename NumericTraits<InputPixelType>::PrintType>(m_BackgroundValue) << std::endl;
	}
	
	
	template<class TImage, class TOrderImage>
	void 
	SkeletonizeImageFilter<TImage, TOrderImage>
	::GenerateInputRequestedRegion()
	
	{
		
		// call the superclass' implementation of this method
		
		Superclass::GenerateInputRequestedRegion();
		// get pointers to the inputs
		
		OrderingImagePointerType orderingPtr = const_cast< OrderingImageType * >(this->GetOrderingImage());
		InputImagePointerType inputPtr = const_cast<InputImageType *>(this->GetInput());
		
		if ( !orderingPtr || !inputPtr )
		{ 
			return; 
		}
		
		// We need to
		// configure the inputs such that all the data is available.
		//
		orderingPtr->SetRequestedRegion(orderingPtr->GetLargestPossibleRegion());
		inputPtr->SetRequestedRegion(inputPtr->GetLargestPossibleRegion());
	}
	
	
	template <class TImage, class TOrderImage>
	void 
	SkeletonizeImageFilter<TImage, TOrderImage>
	::EnlargeOutputRequestedRegion(DataObject *)
	{
		this->GetOutput()->SetRequestedRegion(
											  this->GetOutput()->GetLargestPossibleRegion() );
	}
	
	
	
	template<class TImage, class TOrderImage>
	void 
	SkeletonizeImageFilter<TImage, TOrderImage>
	::GenerateData()
	{
		this->AllocateOutputs();
		
		OrderingImageConstPointerType orderingImage = this->GetOrderingImage();
		
		// set up structures for connected component labelling
		
	}
	
} // namespace itk

#endif // itkSkeletonizationImageFilter_txx
