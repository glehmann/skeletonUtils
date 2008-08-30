#ifndef itkSkeletonizationImageFilter_h
#define itkSkeletonizationImageFilter_h

#include <itkImage.h>
#include <itkImageToImageFilter.h>

namespace itk
{
	
	/**
	 
	 */
	template<class TImage, class TOrderImage>
	class SkeletonizeImageFilter : public ImageToImageFilter<TImage, TImage>
	{
		public :
		// standard ITK type definitions
		typedef SkeletonizeImageFilter Self;
		typedef ImageToImageFilter<TImage, TImage> Superclass;
		typedef SmartPointer<Self> Pointer;
		typedef SmartPointer<Self const> ConstPointer;
		
		itkNewMacro(Self);
		itkTypeMacro(SkeletonizeImageFilter, ImageToImageFilter);
		
		
		typedef TImage InputImageType;
		typedef TImage OutputImageType;
		typedef TOrderImage OrderingImageType;
		
		typedef typename InputImageType::ConstPointer InputImageConstPointerType;
		typedef typename OrderingImageType::ConstPointer OrderingImageConstPointerType;
		
		typedef typename InputImageType::Pointer InputImagePointerType;
		typedef typename OrderingImageType::Pointer OrderingImagePointerType;
		
		
		/** Declaration of pixel type. */
		typedef typename InputImageType::PixelType InputPixelType ;
		
		/** Set/Get the foreground value. Defaults to max */
		itkSetMacro(ForegroundValue, InputPixelType);
		itkGetMacro(ForegroundValue, InputPixelType);
		
		/** Set/Get the foreground value. Defaults to zero */
		itkSetMacro(BackgroundValue, InputPixelType);
		itkGetMacro(BackgroundValue, InputPixelType);
		
		itkSetMacro(ForegroundCellConnectivity, unsigned);
		itkGetMacro(ForegroundCellConnectivity, unsigned);

		itkSetMacro(BackgroundCellConnectivity, unsigned);
		itkGetMacro(BackgroundCellConnectivity, unsigned);
		
		void SetOrderingImage(OrderingImageType *input)
		{
			this->SetNthInput( 1, const_cast<OrderingImageType *>(input) );
		}
		
		const OrderingImageType * GetOrderingImage() const
		{    
			return static_cast<OrderingImageType*>(
												   const_cast<DataObject *>(this->ProcessObject::GetInput(1)));
		}
		
		
		
		protected :
		void EnlargeOutputRequestedRegion(DataObject *itkNotUsed(output));
		
		SkeletonizeImageFilter();
		SkeletonizeImageFilter(Self const &); // Purposedly not implemented
		void operator=(Self const &); // Purposedly not implemented
		
		void PrintSelf(std::ostream& os, Indent indent) const;
		void GenerateInputRequestedRegion();
		void GenerateData();
				
		unsigned m_ForegroundCellConnectivity;
		unsigned m_BackgroundCellConnectivity;
		
		InputPixelType m_ForegroundValue;
		InputPixelType m_BackgroundValue;
		
	};
	
} // namespace itk

#include "itkSkeletonizeImageFilter.txx"

#endif
