#ifndef _itkFastBinaryPruningImageFilter_txx
#define _itkFastBinaryPruningImageFilter_txx

#include <iostream>

#include "itkFastBinaryPruningImageFilter.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkNeighborhoodIterator.h"
#include "itkShapedNeighborhoodIterator.h"
#include "itkConstShapedNeighborhoodIterator.h"
#include "itkSkeletonConnectivity.h"
#include "itkSize.h"
#include "itkConstantBoundaryCondition.h"


namespace itk
{

/**
 *    Constructor
 */
template <class TInputImage,class TOutputImage>
FastBinaryPruningImageFilter<TInputImage,TOutputImage>
::FastBinaryPruningImageFilter()
{
  this->SetNumberOfRequiredOutputs( 1 );

  OutputImagePointer pruneImage = OutputImageType::New();
  this->SetNthOutput( 0, pruneImage.GetPointer() );

  m_Iteration = 3;
  m_ForegroundCellConnectivity = 0;
}


template <class TInputImage,class TOutputImage>
void 
FastBinaryPruningImageFilter<TInputImage,TOutputImage>
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
FastBinaryPruningImageFilter<TInputImage,TOutputImage>
::EnlargeOutputRequestedRegion(DataObject *)
{
  this->GetOutput()
    ->SetRequestedRegion( this->GetOutput()->GetLargestPossibleRegion() );
}

template <class TInputImage,class TOutputImage>
void 
FastBinaryPruningImageFilter<TInputImage,TOutputImage>
::doErode(typename TOutputImage::Pointer &t1, IndexVec &v1, IndexVec &v2, ProgressReporter *progress) 
{
  // in this version we iterate over the voxels known to be members of
  // the skeleton.
  typedef ConstShapedNeighborhoodIterator<OutputImageType> ShapedNeighborhoodIteratorType;
  typename ShapedNeighborhoodIteratorType::RadiusType radius;
  radius.Fill(1);
  typename OutputImageType::RegionType region  = this->GetOutput()->GetRequestedRegion();

  ShapedNeighborhoodIteratorType it(radius, t1, region);
  IndexVec deletedPoints;

  ConstantBoundaryCondition<OutputImageType> bc;
  bc.SetConstant(0);
  it.OverrideBoundaryCondition(&bc);
  setCellConnectivity( &it, m_ForegroundCellConnectivity );

  typename ShapedNeighborhoodIteratorType::ConstIterator nIt;

  it.GoToBegin();

  typedef typename IndexVec::const_iterator vecItType;
  for (vecItType vecIt = v1.begin(); vecIt != v1.end(); vecIt++)
    {
    // get the index
    IndexType Ind = *vecIt;
    // move the iterators to this index
    it += Ind - it.GetIndex();
    OutputPixelType V = it.GetCenterPixel();

    int genus = 0;
    for (nIt = it.Begin(); nIt != it.End(); nIt++)
      {
      genus += (int)(nIt.Get() != 0);
      }
    if (genus < 2)
      {
      // this point is being removed
       deletedPoints.push_back(Ind);
      }
    else
      {
      // this point is being retained. Copy the index to the index buffer
      v2.push_back(Ind);
      }
    progress->CompletedPixel();
    }
  // now we need to remove the endpoints from the input
  for (vecItType vecIt = deletedPoints.begin(); vecIt != deletedPoints.end();
       vecIt++)
    {
    t1->SetPixel(*vecIt, 0);
    }
}
/**
 *  Generate PruneImage
 */
template <class TInputImage,class TOutputImage>
void 
FastBinaryPruningImageFilter<TInputImage,TOutputImage>
::GenerateData() 
{
  this->AllocateOutputs();
  InputImagePointer  inputImage  = this->GetInput();
  OutputImagePointer outputImage = this->GetOutput();
  typename OutputImageType::RegionType region  = this->GetOutput()->GetRequestedRegion();
  
  ProgressReporter progress(this, 0, region.GetNumberOfPixels()*2);

  IndexVec v1, v2;

  ImageRegionConstIterator< TInputImage >  it( inputImage,  region );
  ImageRegionIterator< TOutputImage > ot( outputImage,  region );
  
  it.GoToBegin();
  ot.GoToBegin();
  itkDebugMacro(<< "PrepareData: Copy input to output");
  
  while( !ot.IsAtEnd() )
    {
    OutputPixelType P = static_cast< typename OutputImageType::PixelType >(it.Get());
    ot.Set(P);
    if (P)
      {
      IndexType here = ot.GetIndex();
      v1.push_back(here);
      }
    ++it;
    ++ot;
    progress.CompletedPixel();
    }
  
  // perform erosions
  for (unsigned i = 0; i < m_Iteration; i++)
    {
    doErode(outputImage, v1, v2, &progress);
    std::swap(v1, v2);
    v2.clear();
    }
} // end GenerateData()

/**
 *  Print Self
 */
template <class TInputImage,class TOutputImage>
void 
FastBinaryPruningImageFilter<TInputImage,TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  
  os << indent << "Pruning image: " << std::endl;
  os << indent << "Iteration: " << m_Iteration << std::endl;

}



} // end namespace itk

#endif
