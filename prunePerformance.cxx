#include "ioutils.h"
#include "itkFastBinaryPruningImageFilter.h"
#include "itkNewBinaryPruningImageFilter.h"
#include "itkTimeProbe.h"
#include "itkMultiThreader.h"
#include <iomanip>

int main(int argc, char * argv[])
{

  if( argc != 3 )
    {
    std::cerr << "usage: " << argv[0] << "repeats input" << std::endl;
    std::cerr << " input: the input image" << std::endl;
    std::cerr << " output: the output image" << std::endl;
    // std::cerr << "  : " << std::endl;
    exit(1);
    }

  
  itk::TimeProbe bptime, fptime;

  const int dim = 3;
  
  int repeats = atoi(argv[1]);

  typedef unsigned char PType;
  typedef itk::Image< PType, dim > IType;

  IType::Pointer input = readIm<IType>(argv[2]);


  typedef itk::FastBinaryPruningImageFilter<IType, IType> PruneType;
  PruneType::Pointer pruner = PruneType::New();
  typedef itk::NewBinaryPruningImageFilter<IType, IType> BasicPruneType;
  BasicPruneType::Pointer bpruner = BasicPruneType::New();

  std::cout << "Iterations \t Basic pruner \t fast pruner" << std::endl;

  std::vector<int> Its;
  Its.push_back(1);
  Its.push_back(3);
  Its.push_back(7);
  
  Its.push_back(10);
  Its.push_back(30);
  Its.push_back(70);
  Its.push_back(100);

  for (int P = 0; P < Its.size(); P++)
    {
    int i = Its[P];
    pruner->SetIteration(i);
    bpruner->SetIteration(i);
    pruner->SetInput(input);
    bpruner->SetInput(input);
    itk::TimeProbe fptimer, bptimer;
    for (int k = 0; k < repeats; k++)
      {
      pruner->Modified();
      fptimer.Start();
      pruner->Update();
      fptimer.Stop();

      bpruner->Modified();
      bptimer.Start();
      bpruner->Update();
      bptimer.Stop();
      }

    std::cout << std::setprecision(3) << i << "\t" << bptimer.GetMeanTime() << "\t" << fptimer.GetMeanTime() << std::endl;
    }


  return EXIT_SUCCESS;
}

