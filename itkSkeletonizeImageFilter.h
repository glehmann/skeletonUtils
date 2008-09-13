#ifndef __itkSkeletonizeImageFilter_h
#define __itkSkeletonizeImageFilter_h

#include "itkImageToImageFilter.h"

namespace itk {
/** \class SkeletonizeImageFilter
 *  \brief A class using SkeletonizeBaseImageFilter in a minipipeline
 *  to skeletonize a mask
 *
 *  This is a convenience filter that skeletonizes a mask. Internally
 *  it uses the DanielssonDistanceTransformImageFilter with floating
 *  point pixel types. The distance transform step can dominate the
 *  computation time, so don't use this filter if you've computed the
 *  distance transform for other reasons or don't want this level of
 *  precision.
 *
 * \author Richard Beare
 */
template <class TImage, class TOutImage=TImage>
class ITK_EXPORT SkeletonizeImageFilter : public ImageToImageFilter<TImage, TOutImage>
{
public :
  // standard ITK type definitions
  typedef SkeletonizeImageFilter Self;
  typedef ImageToImageFilter<TImage, TOutImage> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<Self const> ConstPointer;
		
  itkNewMacro(Self);
  itkTypeMacro(SkeletonizeBaseImageFilter, ImageToImageFilter);
  
  typedef TImage InputImageType;
  typedef typename TImage::Pointer InputImagePointer;

  typedef typename TImage::PixelType InputPixelType;
  typedef typename TOutImage::PixelType OutputPixelType;
		  
  /** Set/Get the foreground value. Defaults to max */
  itkSetMacro(ForegroundValue, OutputPixelType);
  itkGetMacro(ForegroundValue, OutputPixelType);
		
  /** Set/Get the foreground value. Defaults to zero */
  //itkSetMacro(BackgroundValue, OutputPixelType);
  //itkGetMacro(BackgroundValue, OutputPixelType);
		
  itkSetMacro(ForegroundCellConnectivity, unsigned);
  itkGetMacro(ForegroundCellConnectivity, unsigned);

  itkSetMacro(BackgroundCellConnectivity, unsigned);
  itkGetMacro(BackgroundCellConnectivity, unsigned);
		
protected:
  SkeletonizeImageFilter();
  SkeletonizeImageFilter(Self const &); // Purposedly not implemented
  void operator=(Self const &); // Purposedly not implemented

  void GenerateInputRequestedRegion();
  void GenerateData();
  void EnlargeOutputRequestedRegion(DataObject *itkNotUsed(output));

  InputPixelType m_ForegroundValue;
  //OutputPixelType m_BackgroundValue;

  unsigned m_ForegroundCellConnectivity;
  unsigned m_BackgroundCellConnectivity;



};
} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSkeletonizeImageFilter.txx"
#endif

#endif
