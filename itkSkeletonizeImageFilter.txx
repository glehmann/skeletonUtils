#ifndef __itkSkeletonizeImageFilter_txx
#define __itkSkeletonizeImageFilter_txx

#include "itkImageFileWriter.h"
#include "itkSkeletonizeImageFilter.h"
#include "itkDanielssonDistanceMapImageFilter.h"
#include "itkSkeletonizeBaseImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"

namespace itk
{
	

template <class TImage, class TOutImage>
SkeletonizeImageFilter<TImage, TOutImage>
::SkeletonizeImageFilter()
{
  m_ForegroundValue = 1;
  //m_BackgroundValue = NumericTraits<OutputPixelType>::Zero;
  // set default foreground connectivity to 0 and background to dim -
  // 1

  this->SetForegroundCellConnectivity(0);
  this->SetBackgroundCellConnectivity(TImage::ImageDimension - 1);

}

template <class TImage, class TOutImage>
void
SkeletonizeImageFilter<TImage, TOutImage>
::GenerateInputRequestedRegion()
{
  // call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();

  // We need all the input.
  InputImagePointer input = const_cast<InputImageType *>(this->GetInput());
  if ( !input )
    { return; }
  input->SetRequestedRegion( input->GetLargestPossibleRegion() );

}

template <class TImage, class TOutImage>
void
SkeletonizeImageFilter<TImage, TOutImage>
::EnlargeOutputRequestedRegion(DataObject *)
{
this->GetOutput()
    ->SetRequestedRegion( this->GetOutput()->GetLargestPossibleRegion() );
}

template <class TImage, class TOutImage>
void
SkeletonizeImageFilter<TImage, TOutImage>
::GenerateData()
{
  // Create a process accumulator for tracking the progress of this minipipeline
  ProgressAccumulator::Pointer progress = ProgressAccumulator::New();
  progress->SetMiniPipelineFilter(this);

  // Allocate the output
  this->AllocateOutputs();

  typedef typename itk::Image< float, TImage::ImageDimension > DistType;

  typedef typename itk::BinaryThresholdImageFilter<TImage, TImage> ThreshType;
  typedef typename itk::DanielssonDistanceMapImageFilter<TImage, DistType> DTType;
  typedef typename itk::SkeletonizeBaseImageFilter<DistType, TOutImage> SkelType;

  typename ThreshType::Pointer thresh = ThreshType::New();
  typename DTType::Pointer disttrans = DTType::New();
  typename SkelType::Pointer skel = SkelType::New();

  progress->RegisterInternalFilter(thresh, 0.1f);
  progress->RegisterInternalFilter(disttrans, 0.6f);
  progress->RegisterInternalFilter(skel, 0.3f);

  // threshold filter needs to select the foreground voxels
  thresh->SetInput(this->GetInput());
  thresh->SetLowerThreshold(m_ForegroundValue);
  thresh->SetUpperThreshold(m_ForegroundValue);
  thresh->SetInsideValue(0);
  thresh->SetOutsideValue(1);

  disttrans->SetInput(thresh->GetOutput());
  disttrans->SetUseImageSpacing(true);
  
  typedef typename itk::ImageFileWriter<DistType> WType;
  typename WType::Pointer wr = WType::New();
  wr->SetInput(disttrans->GetOutput());
  wr->SetFileName("dt.nii.gz");
  wr->Update();
  std::cout << "Finished DT" << std::endl;

  skel->SetInput(disttrans->GetOutput());
  skel->SetForegroundValue(1);
  skel->SetBackgroundValue(0);
  skel->SetForegroundCellConnectivity(m_ForegroundCellConnectivity);
  skel->SetBackgroundCellConnectivity(m_BackgroundCellConnectivity);
  skel->GraftOutput(this->GetOutput());
  skel->Update();
  this->GraftOutput(skel->GetOutput());
}


}
#endif
