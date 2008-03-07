#include "ioutils.h"
#include "itkFastBinaryPruningImageFilter.h"
#include "itkNewBinaryPruningImageFilter.h"

int main(int argc, char * argv[])
{

  if( argc != 5 )
    {
    std::cerr << "usage: " << argv[0] << "iterations intput outputfast outputbasic" << std::endl;
    std::cerr << " input: the input image" << std::endl;
    std::cerr << " output: the output image" << std::endl;
    // std::cerr << "  : " << std::endl;
    exit(1);
    }

  const int dim = 2;
  
  int iterations = atoi(argv[1]);

  typedef unsigned char PType;
  typedef itk::Image< PType, dim > IType;

  IType::Pointer input = readIm<IType>(argv[2]);

  typedef itk::FastBinaryPruningImageFilter<IType, IType> PruneType;
  PruneType::Pointer pruner = PruneType::New();
  pruner->SetInput(input);
  pruner->SetIteration(iterations);
  writeIm<IType>(pruner->GetOutput(), argv[3]);

  typedef itk::NewBinaryPruningImageFilter<IType, IType> BasicPruneType;
  BasicPruneType::Pointer bpruner = BasicPruneType::New();
  bpruner->SetInput(input);
  bpruner->SetIteration(iterations);

  writeIm<IType>(bpruner->GetOutput(), argv[4]);

  return EXIT_SUCCESS;
}

