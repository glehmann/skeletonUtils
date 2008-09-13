#include "ioutils.h"
#include "itkSkeletonizeImageFilter.h"
#include "itkSpecialSkeletonPointsImageFilter.h"


int main(int argc, char * argv[])
{
	
  if( argc != 5 )
    {
    std::cerr << "usage: " << argv[0] << " intput output1 output2 output3" << std::endl;
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
  writeIm<IType>(thresh->GetOutput(), "thresh.png");
	
  typedef itk::SkeletonizeImageFilter<IType> SkelType;

  SkelType::Pointer skel = SkelType::New();

  skel->SetForegroundCellConnectivity(0);
  skel->SetBackgroundCellConnectivity(1);
 
  skel->SetInput(thresh->GetOutput());
  
  typedef itk::SpecialSkeletonPointsImageFilter<IType, IType> BranchType;
  BranchType::Pointer branches = BranchType::New();

  branches->SetInput(skel->GetOutput());
  branches->SetForegroundCellConnectivity(0);
  branches->SetEndPoints(true);

  writeIm<IType>(skel->GetOutput(), argv[2]);
  writeIm<IType>(branches->GetOutput(), argv[3]);

  branches->SetEndPoints(false);
  branches->Modified();
  writeIm<IType>(branches->GetOutput(), argv[4]);

  return EXIT_SUCCESS;
}

