#ifndef __itkFastBinaryPruningImageFilter_h
#define __itkFastBinaryPruningImageFilter_h

#include <itkImageToImageFilter.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkNeighborhoodIterator.h>

namespace itk
{
/** \class FastBinaryPruningImageFilter
 *
 * \brief This filter removes "spurs" of less than a certain
 * length in the input image.
 *
 * This class is parametrized over the type of the input image
 * and the type of the output image.
 *
 * The background is assumed to be zero and skeleton non zero
 *
 * This version of the filter is more efficient, extendable to
 * arbitary dimensions and has the novel benefit of producing correct
 * answers.
 * 
 * Rafael C. Gonzales and Richard E. Woods. 
 * Digital Image Processing. 
 * Addison Wesley, 491-494, (1993).
 *
 * \sa MorphologyImageFilter
 * \sa BinaryErodeImageFilter
 * \sa BinaryDilateImageFilter
 * \sa BinaryThinningImageFilter
 * \ingroup ImageEnhancement MathematicalMorphologyImageFilters
 */

template <class TInputImage,class TOutputImage>
class ITK_EXPORT FastBinaryPruningImageFilter :
    public ImageToImageFilter<TInputImage,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef FastBinaryPruningImageFilter    Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Method for creation through the object factory */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro( FastBinaryPruningImageFilter, ImageToImageFilter );

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
 
  /** Neighborhood iterator type */
  typedef NeighborhoodIterator<TInputImage> NeighborhoodIteratorType ;
  
  /** Set/Get the iteration value */
  itkSetMacro(Iteration, unsigned int);
  itkGetMacro(Iteration, unsigned int);

  /** ImageDimension enumeration   */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      TInputImage::ImageDimension );
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      TOutputImage::ImageDimension );
  /**
   * Set/Get whether the connected components are defined strictly by
   * face connectivity or by face+edge+vertex connectivity.  Default is
   * FullyConnectedOff.  For objects that are 1 pixel wide, use
   * FullyConnectedOn.
   */
  itkSetMacro(FullyConnected, bool);
  itkGetConstReferenceMacro(FullyConnected, bool);
  itkBooleanMacro(FullyConnected);
#ifdef ITK_USE_CONCEPT_CHECKING
  /** Start concept checking */
  itkConceptMacro(SameDimensionCheck,
    (Concept::SameDimension<InputImageDimension, OutputImageDimension>));
  itkConceptMacro(SameTypeCheck,
    (Concept::SameType<PixelType, typename TOutputImage::PixelType>));
  itkConceptMacro(AdditiveOperatorsCheck,
    (Concept::AdditiveOperators<PixelType>));
  itkConceptMacro(IntConvertibleToPixelTypeCheck,
    (Concept::Convertible<int, PixelType>));
  itkConceptMacro(PixelLessThanIntCheck,
    (Concept::LessThanComparable<PixelType, int>));
  /** End concept checking */
#endif
  virtual void GenerateInputRequestedRegion() throw(InvalidRequestedRegionError);

protected:
  FastBinaryPruningImageFilter();
  virtual ~FastBinaryPruningImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  void EnlargeOutputRequestedRegion(DataObject *itkNotUsed(output));

  //typedef typename OutputImageType::IndexType IndexType;

  typedef std::vector<IndexType> IndexVec;
  /** Compute thinning Image. */
  void GenerateData();

  void doErode(typename TOutputImage::Pointer &t1, typename TOutputImage::Pointer &t2, IndexVec &v1, IndexVec &v2);
private:   
  FastBinaryPruningImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  bool m_FullyConnected;
  unsigned int                  m_Iteration;

}; // end of BinaryThinningImageFilter class

} //end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkFastBinaryPruningImageFilter.txx"
#endif

#endif
