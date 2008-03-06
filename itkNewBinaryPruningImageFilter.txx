/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkNewBinaryPruningImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2004/10/18 21:29:12 $
  Version:   $Revision: 1.4 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkNewBinaryPruningImageFilter_txx
#define _itkNewBinaryPruningImageFilter_txx

#include <iostream>

#include "itkNewBinaryPruningImageFilter.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkNeighborhoodIterator.h"
#include "itkShapedNeighborhoodIterator.h"
#include "itkConstShapedNeighborhoodIterator.h"
#include "itkConnectedComponentAlgorithm.h"
#include "itkSize.h"
#include "itkConstantBoundaryCondition.h"

namespace itk
{

/**
 *    Constructor
 */
template <class TInputImage,class TOutputImage>
NewBinaryPruningImageFilter<TInputImage,TOutputImage>
::NewBinaryPruningImageFilter()
{
  this->SetNumberOfRequiredOutputs( 1 );

  OutputImagePointer pruneImage = OutputImageType::New();
  this->SetNthOutput( 0, pruneImage.GetPointer() );

  m_Iteration = 3;
  m_FullyConnected = true;
}

/**
 *  Return the pruning Image pointer
 */
template <class TInputImage,class TOutputImage>
typename NewBinaryPruningImageFilter<
  TInputImage,TOutputImage>::OutputImageType * 
NewBinaryPruningImageFilter<TInputImage,TOutputImage>
::GetPruning(void)
{
  return  dynamic_cast< OutputImageType * >(
    this->ProcessObject::GetOutput(0) );
}


/**
 *  Prepare data for computation
 */
template <class TInputImage,class TOutputImage>
void 
NewBinaryPruningImageFilter<TInputImage,TOutputImage>
::PrepareData(void) 
{
  
  itkDebugMacro(<< "PrepareData Start");
  OutputImagePointer pruneImage = GetPruning();

  InputImagePointer  inputImage  = 
    dynamic_cast<const TInputImage  *>( ProcessObject::GetInput(0) );

  pruneImage->SetBufferedRegion( pruneImage->GetRequestedRegion() );
  pruneImage->Allocate();

  typename OutputImageType::RegionType region  = pruneImage->GetRequestedRegion();


  ImageRegionConstIterator< TInputImage >  it( inputImage,  region );
  ImageRegionIterator< TOutputImage > ot( pruneImage,  region );

  it.GoToBegin();
  ot.GoToBegin();

  itkDebugMacro(<< "PrepareData: Copy input to output");
 
  while( !ot.IsAtEnd() )
      {
      ot.Set( static_cast< typename OutputImageType::PixelType >( it.Get() ) );
      ++it;
      ++ot;
      }
  itkDebugMacro(<< "PrepareData End");    
}

/**
 *  Post processing for computing thinning
 */
template <class TInputImage,class TOutputImage>
void 
NewBinaryPruningImageFilter<TInputImage,TOutputImage>
::ComputePruneImage() 
{

  itkDebugMacro( << "ComputeThinImage Start");

  // iterator for the output image
  typedef ShapedNeighborhoodIterator<OutputImageType> ShapedNeighborhoodIteratorType;

  OutputImagePointer    pruneImage          =  GetPruning();

  typename OutputImageType::RegionType region  = pruneImage->GetRequestedRegion();

  typename ShapedNeighborhoodIteratorType::RadiusType radius;
  radius.Fill(1);

  ShapedNeighborhoodIteratorType ot(radius, this->GetOutput(), this->GetOutput()->GetRequestedRegion());

  ConstantBoundaryCondition<OutputImageType> bc;
  bc.SetConstant(0);
  ot.OverrideBoundaryCondition(&bc);
  setConnectivity( &ot, m_FullyConnected );
 
  typename ShapedNeighborhoodIteratorType::ConstIterator nIt;
  unsigned int count = 0;
  while(count < m_Iteration)
    {
    ot.GoToBegin();
    while( ! ot.IsAtEnd() )
      {
      if (ot.GetCenterPixel())
       {
         int genus = 0;
	 for (nIt = ot.Begin(); nIt != ot.End(); nIt++)
           {
            genus += (int)(nIt.Get() != 0);
           }
         if (genus < 2)
           {
             ot.SetCenterPixel( 0 );
           }
       }

      ++ot;
  }
    ++count;
    }  
    itkDebugMacro( << "ComputeThinImage End");
}

template <class TInputImage,class TOutputImage>
void 
NewBinaryPruningImageFilter<TInputImage,TOutputImage>
::doErode(typename TOutputImage::Pointer &t1, typename TOutputImage::Pointer &t2) 
{
  typedef ConstShapedNeighborhoodIterator<OutputImageType> ShapedNeighborhoodIteratorType;
  typename ShapedNeighborhoodIteratorType::RadiusType radius;
  radius.Fill(1);
  typename OutputImageType::RegionType region  = this->GetOutput()->GetRequestedRegion();

  ShapedNeighborhoodIteratorType it(radius, t1, region);

  ConstantBoundaryCondition<OutputImageType> bc;
  bc.SetConstant(0);
  it.OverrideBoundaryCondition(&bc);
  setConnectivity( &it, m_FullyConnected );

  typename ShapedNeighborhoodIteratorType::ConstIterator nIt;
   ImageRegionIterator< TOutputImage > otA( t2,  region );

  it.GoToBegin();
  otA.GoToBegin();
    while( ! it.IsAtEnd() )
      {
      OutputPixelType V = it.GetCenterPixel();
      if (V)
       {
         int genus = 0;
	 for (nIt = it.Begin(); nIt != it.End(); nIt++)
           {
            genus += (int)(nIt.Get() != 0);
           }
         if (genus < 2)
           {
             otA.Set( 0 );
           }
         else
           {
	      otA.Set( V );
           }
       }
       else 
       {
         otA.Set( 0 );
       }

      ++it;
      ++otA;
  }
}
/**
 *  Generate PruneImage
 */
template <class TInputImage,class TOutputImage>
void 
NewBinaryPruningImageFilter<TInputImage,TOutputImage>
::GenerateData() 
{

#if 0
  this->PrepareData();

  itkDebugMacro(<< "GenerateData: Computing Thinning Image");
  this->ComputePruneImage();
#endif

    this->AllocateOutputs();
    InputImagePointer  inputImage  = this->GetInput();
    OutputImagePointer outputImage = this->GetOutput();
    // allocate buffer images
    typename OutputImageType::Pointer t1 = OutputImageType::New();
    typename OutputImageType::Pointer t2 = OutputImageType::New();
    typename OutputImageType::RegionType region  = this->GetOutput()->GetRequestedRegion();

    t1->SetRegions(region);
    t2->SetRegions(region);

    t1->Allocate();
    t2->Allocate();
    // Copy and cast input to a buffer image
   ImageRegionConstIterator< TInputImage >  it( inputImage,  region );
   ImageRegionIterator< TOutputImage > ot( t1,  region );

   it.GoToBegin();
   ot.GoToBegin();

  itkDebugMacro(<< "PrepareData: Copy input to output");
 
  while( !ot.IsAtEnd() )
      {
      ot.Set( static_cast< typename OutputImageType::PixelType >( it.Get() ) );
      ++it;
      ++ot;
      }

    // perform erosions
   for (unsigned i = 0; i < m_Iteration; i++)
   {
	doErode(t1, t2);
        std::swap(t1, t2);
   }
   // copy result to output
  ImageRegionConstIterator< TOutputImage >  itA( t1,  region );
  ImageRegionIterator< TOutputImage > otA( outputImage,  region );

  itA.GoToBegin();
  otA.GoToBegin();

  itkDebugMacro(<< "PrepareData: Copy input to output");
 
  while( !otA.IsAtEnd() )
      {
      otA.Set( itA.Get() );
      ++itA;
      ++otA;
      }   
} // end GenerateData()

/**
 *  Print Self
 */
template <class TInputImage,class TOutputImage>
void 
NewBinaryPruningImageFilter<TInputImage,TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  
  os << indent << "Pruning image: " << std::endl;
  os << indent << "Iteration: " << m_Iteration << std::endl;

}



} // end namespace itk

#endif
