#ifndef __itkSpecialSkeletonPointsImageFilter_h
#define __itkSpecialSkeletonPointsImageFilter_h

namespace itk
{
/** \class SpecialSkeletonPointsImageFilter
*
* \brief Finds either end points or branch points by counting
* neighbours
*
* The background is assumed to be zero, skeleton non zero.
* 
* The CellConnectivity must match that of the skeleton
*
* \author Richard Beare
*/

template <class TInputImage,class TOutputImage>
class ITK_EXPORT SpecialSkeletonPointsImageFilter :
    public ImageToImageFilter<TInputImage,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef SpecialSkeletonPointsImageFilter    Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Method for creation through the object factory */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro( SpecialSkeletonPointsImageFilter, ImageToImageFilter );

  /** Type for input image. */
  typedef   TInputImage       InputImageType;

  /** Type for output image: Skelenton of the object.  */
  typedef   TOutputImage      OutputImageType;

  /** Type for the region of the input image. */
  typedef typename InputImageType::RegionType   RegionType;

  /** Type for the index of the input image. */
  typedef typename RegionType::IndexType  IndexType;

  /** Type for the index of the input image. */
  typedef typename InputImageType::PixelType PixelType ;

  /** Type for the size of the input image. */
  typedef typename RegionType::SizeType   SizeType;

  /** Pointer Type for input image. */
  typedef typename InputImageType::ConstPointer InputImagePointer;

  /** Pointer Type for the output image. */
  typedef typename OutputImageType::Pointer OutputImagePointer;
  typedef typename OutputImageType::PixelType OutputPixelType ;

  /** ImageDimension enumeration   */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      TInputImage::ImageDimension );
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      TOutputImage::ImageDimension );

  itkSetMacro(ForegroundCellConnectivity, unsigned);
  itkGetMacro(ForegroundCellConnectivity, unsigned);

  itkSetMacro(EndPoints, bool);
  itkGetConstReferenceMacro(EndPoints, bool);
  itkBooleanMacro(EndPoints);

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Start concept checking */
  itkConceptMacro(SameDimensionCheck,
    (Concept::SameDimension<InputImageDimension, OutputImageDimension>));
#endif

  virtual void GenerateInputRequestedRegion() throw(InvalidRequestedRegionError);
protected:
  SpecialSkeletonPointsImageFilter();
  virtual ~SpecialSkeletonPointsImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  void EnlargeOutputRequestedRegion(DataObject *itkNotUsed(output));

  //typedef typename OutputImageType::IndexType IndexType;

  typedef std::vector<IndexType> IndexVec;
  /** Compute thinning Image. */
  void GenerateData();

private:
  unsigned m_ForegroundCellConnectivity;
  bool m_EndPoints;

};


} // namespace itk
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSpecialSkeletonPointsImageFilter.txx"
#endif


#endif
