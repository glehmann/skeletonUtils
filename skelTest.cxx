#include "ioutils.h"
#include "itkSkeletonizeBaseImageFilter.h"
#include "itkDanielssonDistanceMapImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"


int main(int argc, char * argv[])
{
	
  if( argc != 3 )
    {
    std::cerr << "usage: " << argv[0] << " intput output" << std::endl;
    std::cerr << " input: the input image" << std::endl;
    std::cerr << " output: the output image" << std::endl;
    // std::cerr << "  : " << std::endl;
    exit(1);
    }
	
  const int dim = 2;
	
  typedef unsigned char PType;
  typedef itk::Image< PType, dim > IType;
	
  typedef itk::Image< float, dim > DistType;
	
  IType::Pointer input = readIm<IType>(argv[1]);


  typedef itk::BinaryThresholdImageFilter<IType, IType> ThreshType;

  typedef itk::DanielssonDistanceMapImageFilter<IType, DistType> DTType;

  ThreshType::Pointer thresh = ThreshType::New();
  thresh->SetInput(input);
  thresh->SetUpperThreshold(130);
  thresh->SetOutsideValue(1);
  thresh->SetInsideValue(0);

  // doesn't seem to be any way of telling Danielsson filter to
  // compute the other half of the map, so invert the input
  ThreshType::Pointer thresh2 = ThreshType::New();
  thresh2->SetInput(input);
  thresh2->SetUpperThreshold(130);
  thresh2->SetOutsideValue(0);
  thresh2->SetInsideValue(1);

  writeIm<IType>(thresh->GetOutput(), "thresh.png");
  writeIm<IType>(thresh2->GetOutput(), "thresh2.png");
  DTType::Pointer dt = DTType::New();
  dt->SetInput(thresh2->GetOutput());
  writeIm<DistType>(dt->GetOutput(), "dt.mhd");

  DistType::Pointer dist = dt->GetOutput();
	
  typedef itk::SkeletonizeBaseImageFilter<DistType, IType> SkelType;

  SkelType::Pointer skel = SkelType::New();

  skel->SetForegroundCellConnectivity(0);
  skel->SetBackgroundCellConnectivity(1);
 
  skel->SetInput(dist);
  
  writeIm<IType>(skel->GetOutput(), argv[2]);

  return EXIT_SUCCESS;
}

