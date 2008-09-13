#ifndef __itkSkeletonizeBaseImageFilter_txx
#define __itkSkeletonizeBaseImageFilter_txx

#include <itkImageRegionConstIteratorWithIndex.h>
#include <itkNumericTraits.h>
#include <itkProgressReporter.h>
#include <itkConstantBoundaryCondition.h>

#include "itkSkeletonizeBaseImageFilter.h"
#include "itkSkeletonConnectivity.h"
#include "itkHierarchicalQueue.h"
#include <queue>

//#define SKEL_DEBUG

namespace itk
{
	
	
	
template<class TOrderImage, class TImage>
SkeletonizeBaseImageFilter<TOrderImage, TImage>
::SkeletonizeBaseImageFilter()
{
  this->SetNumberOfRequiredInputs(1);
  m_ForegroundValue = 1;
  m_BackgroundValue = NumericTraits<OutputPixelType>::Zero;
  // set default foreground connectivity to 0 and background to dim -
  // 1

  this->SetForegroundCellConnectivity(0);
  this->SetBackgroundCellConnectivity(TImage::ImageDimension - 1);

}
	
	
template<class TOrderImage, class TImage>
void 
SkeletonizeBaseImageFilter<TOrderImage, TImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os << indent 
		
     << "Cell dimension used for foreground connectivity: " 
		
     <<  m_ForegroundCellConnectivity << std::endl;
  os << indent << "ForegroundValue: " << static_cast<typename NumericTraits<OutputPixelType>::PrintType>(m_ForegroundValue) << std::endl;
  os << indent << "BackgroundValue: " << static_cast<typename NumericTraits<OutputPixelType>::PrintType>(m_BackgroundValue) << std::endl;
}
	
	
template<class TOrderImage, class TImage>
void 
SkeletonizeBaseImageFilter<TOrderImage, TImage>
::GenerateInputRequestedRegion()
	
{
		
  // call the superclass' implementation of this method
		
  Superclass::GenerateInputRequestedRegion();
  // get pointers to the inputs
		
  OrderingImagePointerType orderingPtr = const_cast< OrderingImageType * >(this->GetInput());
		
  if ( !orderingPtr )
    { 
    return; 
    }
		
  // We need to
  // configure the inputs such that all the data is available.
  //
  orderingPtr->SetRequestedRegion(orderingPtr->GetLargestPossibleRegion());
}
	
	
template <class TOrderImage, class TImage>
void 
SkeletonizeBaseImageFilter<TOrderImage, TImage>
::EnlargeOutputRequestedRegion(DataObject *)
{
  this->GetOutput()->SetRequestedRegion(
    this->GetOutput()->GetLargestPossibleRegion() );
}
	
	
	
template<class TOrderImage, class TImage>
void 
SkeletonizeBaseImageFilter<TOrderImage, TImage>
::GenerateData()
{
  this->AllocateOutputs();
  typename OutputImageType::Pointer outputImage = this->GetOutput(0);
		
  outputImage->FillBuffer(m_BackgroundValue);

  OrderingImageConstPointerType orderingImage = this->GetInput();
		
  // set up structures for connected component labelling
  SetupConnectivity();


  HierarchicalQueue<typename OrderingImageType::PixelType,
                     typename OrderingImageType::IndexType,
                     std::less<typename OrderingImageType::PixelType> > hq;

  ProgressReporter progress(this, 0, outputImage->GetRequestedRegion().GetNumberOfPixels()*2);

  // collect nonzero voxels from the ordering image and put in the
  // priority queue
  typedef typename itk::ImageRegionConstIteratorWithIndex<OrderingImageType> ConstItType;

  ConstItType It(orderingImage, this->GetOutput()->GetRequestedRegion());

  // an array to track which voxels are on the queue
  bool* inQueue =
    new bool[outputImage->GetRequestedRegion().GetNumberOfPixels()];

  for (It.GoToBegin(); !It.IsAtEnd();++It)
    {
    typename OrderingImageType::PixelType V = It.Get();
    if (V != itk::NumericTraits<typename OrderingImageType::PixelType>::Zero)
      {
      hq.Push(V, It.GetIndex());
      outputImage->SetPixel(It.GetIndex(), m_ForegroundValue);
      // mark as on the queue
      inQueue[outputImage->ComputeOffset(It.GetIndex()) ] = true;

      }
    else
      {
      inQueue[outputImage->ComputeOffset(It.GetIndex()) ] = false;

      }
    progress.CompletedPixel();
    }


  // set up the shaped iterators
  typedef typename itk::ShapedNeighborhoodIterator<OrderingImageType> OrderingIteratorType;

  typedef typename itk::ShapedNeighborhoodIterator<OutputImageType> OutputIteratorType;
  
  typename OrderingImageType::SizeType radius;
  radius.Fill(1);

  OrderingIteratorType ordIt(radius, orderingImage, outputImage->GetRequestedRegion());
  OutputIteratorType outIt(radius, outputImage, outputImage->GetRequestedRegion());

  CubeIteratorType cubeIt(radius, outputImage, outputImage->GetRequestedRegion());

  setCellConnectivity(&ordIt, m_ForegroundCellConnectivity);
  setCellConnectivity(&outIt, m_ForegroundCellConnectivity);
  ConstantBoundaryCondition<OrderingImageType> bc1;
  ConstantBoundaryCondition<OutputImageType> bc2;
  bc1.SetConstant(0);
  bc2.SetConstant(0);
  
  ordIt.OverrideBoundaryCondition(&bc1);
  outIt.OverrideBoundaryCondition(&bc2);
  cubeIt.OverrideBoundaryCondition(&bc2);

  ordIt.GoToBegin();
  outIt.GoToBegin();
  cubeIt.GoToBegin();

  bool * cubeBuffer = new bool[cubeIt.Size()];

  typename OrderingIteratorType::ConstIterator Nord;
  typename OutputIteratorType::ConstIterator Nout;

  while (!hq.Empty())
    {
    typename OrderingImageType::IndexType const current = hq.FrontValue();
    hq.Pop();
    inQueue[outputImage->ComputeOffset(current)] = false;

    // could optimize slightly with offsets
    typename OrderingImageType::OffsetType shift = current - cubeIt.GetIndex();
    
    cubeIt += shift;
    // evaluate terminality and simplicity criterion
    bool removable = ComputeSimplicityTerminality(cubeIt, cubeBuffer);

    if (removable)
      {
      // this point can safely be removed
      outputImage->SetPixel(current, m_BackgroundValue);
      // different shift because iterators can get out of sync
      typename OrderingImageType::OffsetType shift2 = current - ordIt.GetIndex();
      ordIt += shift2;
      outIt += shift2;
      // add unqueued neighbours
      for (Nord=ordIt.Begin(), Nout=outIt.Begin(); Nord != ordIt.End(); Nord++, Nout++)
	{
	// perhaps not quite optimized because we need to take
	// advantage of the boundary conditions.
	typename OrderingImageType::PixelType P = Nord.Get();
	if ( (P != itk::NumericTraits<typename OrderingImageType::PixelType>::Zero) && (Nout.Get() == m_ForegroundValue))
	  {
	  typename OrderingImageType::IndexType Ind = current + Nord.GetNeighborhoodOffset();
	  unsigned long OO = outputImage->ComputeOffset(Ind);
	  if (!inQueue[OO])
	    {
	    // add the neighbour to the queue
	    inQueue[OO] = true;
	    hq.Push(P, Ind);
	    }
	  }
	
	}

      }
    progress.CompletedPixel();
    }
  delete[] inQueue;
  delete[] cubeBuffer;
}

template<class TOrderImage, class TImage>
void 
SkeletonizeBaseImageFilter<TOrderImage, TImage>
::SetupConnectivity()
{
  // set up our tables of offsets. We'll create a little image amd
  // then use the neighborhood iterator to find out which neighbors
  // are actually in the image

  typedef typename itk::Image<int, OrderingImageType::ImageDimension> ImType;

  typename ImType::SizeType radius;
  typename ImType::SizeType imsize;
  radius.Fill(1);
  imsize.Fill(3);

  typename ImType::Pointer smallIm = ImType::New();
  typename ImType::RegionType region;
  typename ImType::IndexType start;
  start.Fill(0);
  
  region.SetSize(imsize);
  region.SetIndex(start);
  smallIm->SetRegions(region);
  smallIm->Allocate();

  typedef typename itk::ShapedNeighborhoodIterator<ImType> ShapedItType;

  ShapedItType It(radius, smallIm, smallIm->GetLargestPossibleRegion());

  setCellConnectivity(&It, m_ForegroundCellConnectivity);
  ConstantBoundaryCondition<ImType> lcbc;
  lcbc.SetConstant(-1 );
  It.OverrideBoundaryCondition(&lcbc);

  int idx = 0;
  // first fill the image with indexes
  // probably a better way of achieving this, but I can't be bothered
  // figuring it out.
  while (!It.IsAtEnd())
    {
    It.SetCenterPixel(idx);
    ++idx;
    ++It;
    }

  OffsetImType FG, BG;

  It.GoToBegin();
  while (!It.IsAtEnd())
    {
    OffsetVecType OV;
    //int Here = It.GetCenterPixel();
    // check the neighbours
    typename ShapedItType::ConstIterator nIt;
    for (nIt = It.Begin(); nIt != It.End(); nIt++)
      {
      int There = nIt.Get();
      if (There != -1)
	{
	// we are inside the image
	OV.push_back(There);
	}
      }
    FG.push_back(OV);
    ++It;
    }
  CentInd = It.GetCenterNeighborhoodIndex();
  // now for the background
  setCellConnectivity(&It, m_BackgroundCellConnectivity);

  It.GoToBegin();
  while (!It.IsAtEnd())
    {
    OffsetVecType OV;
    //int Here = It.GetCenterPixel();
    // check the neighbours
    typename ShapedItType::ConstIterator nIt;
    for (nIt = It.Begin(); nIt != It.End(); nIt++)
      {
      int There = nIt.Get();
      if (There != -1)
	{
	// we are inside the image
	OV.push_back(There);
	}
      }
    BG.push_back(OV);
    ++It;
    }

  m_FGConnect = FG;
  m_BGConnect = BG;

  {
  // set up the connectivity test structures
  std::vector<bool> F(FG.size(), false);

  for (unsigned K = 0; K < m_FGConnect[CentInd].size(); K++)
    {
    unsigned Idx =  m_FGConnect[CentInd][K];
    F[Idx] = true;
    }

  m_FGConnectivityTest = F;

  std::vector<bool> B(BG.size(), false);

  for (unsigned K = 0; K < m_BGConnect[CentInd].size(); K++)
    {
    unsigned Idx =  m_BGConnect[CentInd][K];
    B[Idx] = true;
    }

  m_BGConnectivityTest = B;
  }
  {
  unsigned fgN = genNeigCon(m_ForegroundCellConnectivity);
  unsigned bgN = genNeigCon(m_BackgroundCellConnectivity);
  setCellConnectivity(&It, fgN);

  smallIm->FillBuffer(0);
  typename ImType::IndexType middle;
  middle.Fill(1);
  
  // set up the neighborhood connectivity test structures
  std::vector<bool> F(FG.size(), false);

  It += middle - It.GetIndex();
  typename ShapedItType::ConstIterator nIt;
  for (nIt = It.Begin(); nIt != It.End(); nIt++)
    {
    typename ImType::IndexType Neighbour = middle + nIt.GetNeighborhoodOffset();
    F[smallIm->ComputeOffset(Neighbour)] = true;
    }


  m_FGNeighConnectivityTest = F;

  std::vector<bool> B(BG.size(), false);
  setCellConnectivity(&It, bgN);

  It += middle - It.GetIndex();
  for (nIt = It.Begin(); nIt != It.End(); nIt++)
    {
    typename ImType::IndexType Neighbour = middle + nIt.GetNeighborhoodOffset();
    B[smallIm->ComputeOffset(Neighbour)] = true;
    }

  m_BGNeighConnectivityTest = B;

  }

}

template<class TOrderImage, class TImage>
bool 
SkeletonizeBaseImageFilter<TOrderImage, TImage>
::ComputeSimplicityTerminality(CubeIteratorType cubeIt,
			       bool *cubeBuffer)
{
#ifdef SKEL_DEBUG
  std::cout << cubeIt.GetIndex() << std::endl;
#endif
  // extract the voxels into the cubeBuffer
  for (unsigned pos = 0; pos < cubeIt.Size(); pos++)
    {
    typename OutputImageType::PixelType P = cubeIt.GetPixel(pos);
    cubeBuffer[pos] = (m_ForegroundValue == P);
    }
#ifdef SKEL_DEBUG
  for (unsigned pos = 0; pos < cubeIt.Size(); pos++)
    {
    std::cout << cubeBuffer[pos] ;
    }
  std::cout << std::endl;
#endif


  // count the neighbors of the centre position to determine
  // terminality
  unsigned int ncount = 0;

  for (unsigned i = 0; i < m_FGConnect[CentInd].size(); i++)
    {
    unsigned idx = m_FGConnect[CentInd][i];
    if (cubeBuffer[idx])
      ++ncount;
    }

#ifdef SKEL_DEBUG
  std::cout << "Terminality " << ncount << std::endl;
#endif

  bool terminality=(ncount == 1);

  // short circuit further work
  if ((terminality))
    {
    // this is a termination point, so we don't need to check any
    // further
    return(false);
    }

  // now to count connected components
  // foreground
  unsigned SZ = m_FGConnect.size();
  
  // remove the centre point
  cubeBuffer[CentInd] = false;
 
  int fgCC = countCC(cubeBuffer, m_FGConnect,
		     m_FGConnectivityTest,
		     m_FGNeighConnectivityTest);
  
#ifdef SKEL_DEBUG
  std::cout << "fgCC " << fgCC << std::endl;
#endif

  if (fgCC != 1) return (false);

  // background this time
  
  SZ = m_BGConnect.size();
  // invert cube
  for (unsigned J = 0; J < SZ; J++)
    {
    cubeBuffer[J] = !cubeBuffer[J];
    }
  cubeBuffer[CentInd] = false;


  int bgCC = countCC(cubeBuffer, m_BGConnect,
		     m_BGConnectivityTest,
		     m_BGNeighConnectivityTest);
  
#ifdef SKEL_DEBUG
  std::cout << "bgCC " << bgCC << std::endl;
#endif
  if (bgCC != 1) return(false);

  return(true);

}


template<class TOrderImage, class TImage>
int
SkeletonizeBaseImageFilter<TOrderImage, TImage>
::countCC(bool cubeBuffer[],
	  const OffsetImType &ConnectIm,
	  const std::vector<bool> &ConnectivityTest,
	  const std::vector<bool> &NeighConnectivityTest)

{
  // searching for first seed
  unsigned SZ = ConnectIm.size();
  // need to change this if we use while loops to find the first seed
  unsigned seed = SZ;
#ifdef SKEL_DEBUG
  std::cout << "Counter ";
  for (unsigned pos = 0; pos < SZ; pos++)
    {
    std::cout << cubeBuffer[pos] ;
    }
  std::cout << std::endl;
#endif

  // set seed to max in case we don't find any start point
  for (unsigned J = 0; J < SZ; J++)
     {
     if (cubeBuffer[J] && ConnectivityTest[J])
       {
       seed = J;
       break;
       }
     }
  
//    while ((seed != SZ) &&
//  	 ((cubeBuffer[seed] == 0) || !ConnectivityTest[seed]))
//      {
//      ++seed;
//      }
#ifdef SKEL_DEBUG
   std::cout << "seed " << seed << std::endl;
#endif
  // now label connected components
  std::vector<bool> processed(SZ, false);
  int nbCC = 0;

  while (seed != SZ)
    {
    ++nbCC;
    processed[seed] = true;

    std::queue<unsigned int, std::list<unsigned int> > q;
    q.push(seed);
    
    while(!q.empty())
      {
      unsigned int const current = q.front();
      q.pop();
      
      if (NeighConnectivityTest[current])
	{
	for(unsigned int nn = 0; nn < ConnectIm[current].size(); ++nn)
	  {
	  unsigned int neighbor = ConnectIm[current][nn];
	  if(!processed[neighbor] && (cubeBuffer[neighbor]))
	    {
	    q.push(neighbor);
	    processed[neighbor] = true;
	    }
	  }
	}
      }
    // Look for next seed
//     while(seed != SZ && 

//           ( processed[seed] || (cubeBuffer[seed] == 0) || !ConnectivityTest[seed] ) 

//          )
//       {
//       ++seed;
//       }

    for (;
	 (seed < SZ) && ( processed[seed] || !cubeBuffer[seed] || !ConnectivityTest[seed] ) ;
	 ++seed);


//     for (;seed < SZ; ++seed)
//       {
//       if (!processed[seed] && cubeBuffer[seed] && ConnectivityTest[seed] )
// 	break;
//       }

    }
   
  return nbCC;

}


template<class TOrderImage, class TImage>
unsigned 
SkeletonizeBaseImageFilter<TOrderImage, TImage>
::genNeigCon(unsigned CellConnect)
{
  // return the neighborhood connectivity corresponding to a cell
  // connectivity - hard coded for 2 and 3 dimensions

  if (TImage::ImageDimension == 2)
    {
    if (CellConnect == 1) return(0);
    } 
  else if (TImage::ImageDimension == 3)
    {
    if (CellConnect == 2) return(1);
    }

  return(CellConnect);
}

} // namespace itk




#endif // itkSkeletonizationImageFilter_txx
