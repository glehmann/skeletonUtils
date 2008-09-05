#ifndef __itkSkeletonizationBaseImageFilter_h
#define __itkSkeletonizationBaseImageFilter_h

#include <itkImage.h>
#include <itkImageToImageFilter.h>
#include <itkShapedNeighborhoodIterator.h>
#include <itkNeighborhoodIterator.h>

namespace itk
{
	
/**
	 
	 */
template<class TOrderImage, class TImage>
class SkeletonizeBaseImageFilter : public ImageToImageFilter<TOrderImage, TImage>
{
public :
  // standard ITK type definitions
  typedef SkeletonizeBaseImageFilter Self;
  typedef ImageToImageFilter<TOrderImage, TImage> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<Self const> ConstPointer;
		
  itkNewMacro(Self);
  itkTypeMacro(SkeletonizeImageFilter, ImageToImageFilter);
		
		
  //typedef TImage InputImageType;
  typedef TImage OutputImageType;
  typedef TOrderImage OrderingImageType;
		
  //typedef typename InputImageType::ConstPointer InputImageConstPointerType;
  typedef typename OrderingImageType::ConstPointer OrderingImageConstPointerType;
		
  //typedef typename InputImageType::Pointer InputImagePointerType;
  typedef typename OrderingImageType::Pointer OrderingImagePointerType;
		
		
  /** Declaration of pixel type. */
  typedef typename OutputImageType::PixelType OutputPixelType ;
		
  /** Set/Get the foreground value. Defaults to max */
  itkSetMacro(ForegroundValue, OutputPixelType);
  itkGetMacro(ForegroundValue, OutputPixelType);
		
  /** Set/Get the foreground value. Defaults to zero */
  itkSetMacro(BackgroundValue, OutputPixelType);
  itkGetMacro(BackgroundValue, OutputPixelType);
		
  itkSetMacro(ForegroundCellConnectivity, unsigned);
  itkGetMacro(ForegroundCellConnectivity, unsigned);

  itkSetMacro(BackgroundCellConnectivity, unsigned);
  itkGetMacro(BackgroundCellConnectivity, unsigned);
		
//   void SetOrderingImage(OrderingImageType *input)
//   {
//     this->SetNthInput( 1, const_cast<OrderingImageType *>(input) );
//   }
		
//   const OrderingImageType * GetOrderingImage() const
//   {    
//     return static_cast<OrderingImageType*>(
//       const_cast<DataObject *>(this->ProcessObject::GetInput(1)));
//   }
		
		
		
protected :

  // structures to support fast connected component labelling of
  // neighborhoods

  typedef std::vector<int> OffsetVecType;
  typedef std::vector<OffsetVecType> OffsetImType;

  void SetupConnectivity();
  unsigned genNeigCon(unsigned CellConnect);
  // a standard neighbourhood iterator
  typedef typename itk::NeighborhoodIterator<OutputImageType> CubeIteratorType;

  bool ComputeSimplicityTerminality(CubeIteratorType cubeIt,
				    bool *cubeBuffer);

  int countCC(bool cubeIm[],
	      const OffsetImType &ConnectIm,
	      const std::vector<bool> &ConnectivityTest,
	      const std::vector<bool> &NeighConnectivityTest);

  void EnlargeOutputRequestedRegion(DataObject *itkNotUsed(output));
		
  SkeletonizeBaseImageFilter();
  SkeletonizeBaseImageFilter(Self const &); // Purposedly not implemented
  void operator=(Self const &); // Purposedly not implemented
		
  void PrintSelf(std::ostream& os, Indent indent) const;
  void GenerateInputRequestedRegion();
  void GenerateData();
				
  unsigned m_ForegroundCellConnectivity;
  unsigned m_BackgroundCellConnectivity;
		
  OutputPixelType m_ForegroundValue;
  OutputPixelType m_BackgroundValue;


  OffsetImType m_FGConnect;
  OffsetImType m_BGConnect;

  std::vector<bool> m_FGConnectivityTest;
  std::vector<bool> m_BGConnectivityTest;

  std::vector<bool> m_FGNeighConnectivityTest;
  std::vector<bool> m_BGNeighConnectivityTest;


  unsigned CentInd;
		
};
	
} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSkeletonizeBaseImageFilter.txx"
#endif

#endif
