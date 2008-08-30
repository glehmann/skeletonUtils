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
	
	typedef itk::SkeletonizeImageFilter<IType, DistType> SkelType;
	SkelType::Pointer pruner = SkelType::New();
	pruner->SetInput(input);
	
	return EXIT_SUCCESS;
}

