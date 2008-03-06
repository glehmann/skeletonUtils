/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkNewBinaryPruningImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2006/04/23 04:12:03 $
  Version:   $Revision: 1.6 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkNewBinaryPruningImageFilter_h
#define __itkNewBinaryPruningImageFilter_h

#include <itkImageToImageFilter.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkNeighborhoodIterator.h>

namespace itk
{
/** \class NewBinaryPruningImageFilter
 *
 * \brief This filter removes "spurs" of less than a certain
 * length in the input image.
 *
 * This class is parametrized over the type of the input image
 * and the type of the output image.
 *
 * The input is assumed to be a binary image.  
 *
 * This filter is a sequential pruning algorithm and known to be computational time
 * dependable of the image size.  The algorithm is the N-dimensional version
 * of that given for two dimensions in:
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
class ITK_EXPORT NewBinaryPruningImageFilter :
    public ImageToImageFilter<TInputImage,TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef NewBinaryPruningImageFilter    Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;

  /** Method for creation through the object factory */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro( NewBinaryPruningImageFilter, ImageToImageFilter );

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
  
  /** Get Skelenton by thinning image. */
  OutputImageType * GetPruning(void);

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

protected:
  NewBinaryPruningImageFilter();
  virtual ~NewBinaryPruningImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Compute thinning Image. */
  void GenerateData();

  /** Prepare data. */
  void PrepareData();

  /**  Compute thinning Image. */
  void ComputePruneImage();

  void doErode(typename TOutputImage::Pointer &t1, typename TOutputImage::Pointer &t2);
private:   
  NewBinaryPruningImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  bool m_FullyConnected;
  unsigned int                  m_Iteration;

}; // end of BinaryThinningImageFilter class

} //end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkNewBinaryPruningImageFilter.txx"
#endif

#endif
