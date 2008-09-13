#ifndef _itkSpecialSkeletonPointsImageFilter_txx
#define _itkSpecialSkeletonPointsImageFilter_txx

#include "itkSpecialSkeletonPointsImageFilter.h"
#include "itkImageRegionConstIterator.h"
#include "itkConstShapedNeighborhoodIterator.h"
#include "itkConstantBoundaryCondition.h"

namespace itk
{

template <class TInputImage,class TOutputImage>
SpecialSkeletonPointsImageFilter<TInputImage,TOutputImage>
::SpecialSkeletonPointsImageFilter()
{
  this->SetNumberOfRequiredOutputs( 1 );

  m_ForegroundCellConnectivity = 0;
  m_EndPoints = true; // find endpoints, not branch points
}

template <class TInputImage,class TOutputImage>
void 
SpecialSkeletonPointsImageFilter<TInputImage,TOutputImage>
::GenerateInputRequestedRegion() throw(InvalidRequestedRegionError)
{
  // call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();

  // We need all the input.
  typename Superclass::InputImagePointer input = const_cast<TInputImage *>(this->GetInput());
  if( !input )
    {
    return;
    }
  input->SetRequestedRegion( input->GetLargestPossibleRegion() );
}

template <class TInputImage,class TOutputImage>
void 
SpecialSkeletonPointsImageFilter<TInputImage,TOutputImage>
::EnlargeOutputRequestedRegion(DataObject *)
{
  this->GetOutput()
    ->SetRequestedRegion( this->GetOutput()->GetLargestPossibleRegion() );
}

template <class TInputImage,class TOutputImage>
void 
SpecialSkeletonPointsImageFilter<TInputImage,TOutputImage>
::GenerateData() 
{
  this->AllocateOutputs();
InputImagePointer  inputImage  = this->GetInput();
  OutputImagePointer outputImage = this->GetOutput();
  outputImage->FillBuffer(0);
  typename OutputImageType::RegionType region  = this->GetOutput()->GetRequestedRegion();
  
  ProgressReporter progress(this, 0, region.GetNumberOfPixels());
  ImageRegionConstIterator< TInputImage >  it( inputImage,  region );

  
  typedef typename std::vector<IndexType> IndVecType;
  IndVecType AllIndexes;

  for (it.GoToBegin(); !it.IsAtEnd(); ++it)
    {
    if (it.Get() != 0)
      {
      AllIndexes.push_back(it.GetIndex());
      }
    progress.CompletedPixel();
    }
  
  typedef ConstShapedNeighborhoodIterator<OutputImageType> ShapedNeighborhoodIteratorType;
  typename ShapedNeighborhoodIteratorType::RadiusType radius;
  radius.Fill(1);
  ShapedNeighborhoodIteratorType sIt(radius, inputImage, region);
  typename ShapedNeighborhoodIteratorType::ConstIterator nIt;

  ConstantBoundaryCondition<OutputImageType> bc;
  bc.SetConstant(0);
  sIt.OverrideBoundaryCondition(&bc);
  setCellConnectivity( &sIt, m_ForegroundCellConnectivity);

  if (m_EndPoints)
    {
    // now check the list of indexes
    for (typename IndVecType::const_iterator V=AllIndexes.begin();
	 V != AllIndexes.end(); V++)
      {
      sIt += (*V) - sIt.GetIndex();
    
      // now count the neighbours
      unsigned ncount = 0;
      for (nIt = sIt.Begin(); nIt != sIt.End(); nIt++)
	{
	ncount += (nIt.Get() != 0);
	if (ncount > 1) break;
	}

      if (ncount == 1)
	{
	outputImage->SetPixel((*V), 1);
	}
      }
    }
  else 
    {
    // I think we need a two pass process here
    for (typename IndVecType::const_iterator V=AllIndexes.begin();
	 V != AllIndexes.end(); V++)
      {
      sIt += (*V) - sIt.GetIndex();
      // now count the neighbours
      unsigned ncount = 0;
      for (nIt = sIt.Begin(); nIt != sIt.End(); nIt++)
	{
	ncount += (nIt.Get() != 0);
	}
      if (ncount >= 3)
	{
	outputImage->SetPixel((*V), 1);
	}
      }
    }
}

template <class TInputImage,class TOutputImage>
void 
SpecialSkeletonPointsImageFilter<TInputImage,TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  
  os << indent << "Special Skeleton Points: " << std::endl;
  os << indent << "Connectivity: " << m_ForegroundCellConnectivity << std::endl;
  os << indent << "EndPoints: " << m_EndPoints << std::endl;

}





}

#endif
