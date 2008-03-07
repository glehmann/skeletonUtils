#include "ioutils.h"
#include "itkFastBinaryPruningImageFilter.h"

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

  IType::Pointer input = readIm<IType>(argv[1]);

  typedef itk::FastBinaryPruningImageFilter<IType, IType> PruneType;
  PruneType::Pointer pruner = PruneType::New();
  pruner->SetInput(input);
  pruner->SetIteration(1);
  writeIm<IType>(pruner->GetOutput(), argv[2]);
  return EXIT_SUCCESS;
}

