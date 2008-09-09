#include "ioutils.h"
#include "itkSkeletonizeImageFilter.h"


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

	
  typedef itk::SkeletonizeImageFilter<IType> SkelType;

  SkelType::Pointer skel = SkelType::New();

  skel->SetForegroundCellConnectivity(0);
  skel->SetBackgroundCellConnectivity(1);
 
  skel->SetInput(thresh->GetOutput());
  
  writeIm<IType>(skel->GetOutput(), argv[2]);

  return EXIT_SUCCESS;
}

