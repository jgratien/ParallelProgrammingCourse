#include <mpi.h>

#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/variables_map.hpp>

#include <string>
#include <vector>
#include <fstream>
#include <typeinfo>

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/LU>


#include "MatrixVector/CSRMatrix.h"
#include "MatrixVector/LinearAlgebra.h"
#include "MatrixVector/MatrixGenerator.h"


#include "Utils/Timer.h"


int main(int argc, char** argv)
{
  using namespace boost::program_options ;
  options_description desc;
  desc.add_options()
      ("help", "produce help")
      ("nrows",value<int>()->default_value(0), "matrix size")
      ("nx",value<int>()->default_value(0), "nx grid size")
      ("file",value<std::string>(), "file input")
      ("eigen",value<int>()->default_value(0), "use eigen package") ;
  variables_map vm;
  store(parse_command_line(argc, argv, desc), vm);
  notify(vm);
  if (vm.count("help"))
  {
      std::cout << desc << "\n";
      return 1;
  }
  
  //____________________________________________________________
  
  MPI_Init(&argc,&argv) ;
  
  int my_rank = 0;
  int nb_proc;
  
  MPI_Comm_size(MPI_COMM_WORLD,&nb_proc) ;
  MPI_Comm_rank(MPI_COMM_WORLD,&my_rank) ;
  
  double mpi_time = 1;  
  double mpi_finalTime;
  
  
  //_______________________________________________________________
  
  
  std::size_t n_nnz;
  std::size_t first_nnz;

  std::size_t local_size;
  
  std::vector<double> local_y;
  
  std::size_t nrows;

  //___________________________________________________________________
  
  using namespace PPTP ;
  
  Timer timer ;
  MatrixGenerator generator ;
  int nx = vm["nx"].as<int>() ;
  
  
  if(vm["eigen"].as<int>()==1)
  {
    typedef Eigen::SparseMatrix<double> MatrixType ;
    typedef Eigen::VectorXd             VectorType ;
	
    MatrixType matrix (nrows,nrows);
    if(vm.count("file"))
    {
      std::string file = vm["file"].as<std::string>() ;
      generator.readFromFile(file,matrix) ;
    }
    else
    {
      int nx = vm["nx"].as<int>() ;
      generator.genLaplacian(nx,matrix) ;
	  std::cout<<" nx "<< nx <<std::endl;
    }
	
	
    std::size_t nrows = matrix.rows();
    VectorType x(nrows);
	
	
    for(std::size_t i=0;i<nrows;++i)
      x(i) = i+1 ;
  
  
    VectorType y;
    {
      Timer::Sentry sentry(timer,"EigenSpMV") ;
       y = matrix*x ;
    }
	
	
    double normy = PPTP::norm2(y) ;

  }
  
  
  
  //_________________________________________________________________________
  
  if (my_rank == 0)
  {
    CSRMatrix matrix ;
	
    if(vm.count("file"))
    {
      std::string file = vm["file"].as<std::string>() ;
      generator.readFromFile(file,matrix) ;
    }
    else
    {
      int nx = vm["nx"].as<int>() ;
      generator.genLaplacian(nx,matrix) ;
    }
	

	std::size_t nrows = matrix.nrows();
    
	int *local_kcol_ptr = matrix.kcol(); // first lines of process 0
    
	first_nnz = local_kcol_ptr[(nrows / nb_proc)] - local_kcol_ptr[0];
    
	local_kcol_ptr = local_kcol_ptr + (nrows / nb_proc);
    
	double *local_values_ptr = matrix.values() + first_nnz;
    
	int *local_cols_ptr = matrix.cols() + first_nnz;
    
	
	std::vector<double> x,y;
	x.resize(nrows) ;
	y.resize(nrows) ;
	
	
	std::cout<<"first nnz = "<< first_nnz <<std::endl ;
	

	for(std::size_t i=0;i<nrows;++i)
      x[i] = i+1 ;
    
	{
	  Timer::Sentry sentry(timer,"SpMV_seq") ;
      matrix.mult(x,y) ;
	  double normy = PPTP::norm2(y) ;
      std::cout<<"Standard value: ||y||="<<normy<<std::endl ;
	 	
	}
	 
	{
      // SEND GLOBAL SIZE
	  
      MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
	  
      std::cout<<"I send nrows: "<<nrows<<std::endl ;
	  
      // SEND MATRIX
	  
	  
      for (int i=1; i<nb_proc;++i)
      {
        std::cout<<" SEND MATRIX DATA to proc "<<i<<std::endl ;
		
		
		local_size = nrows/nb_proc + 1;
		
		/*if (rest > 0) {
          rest -= 1;
          local_nrows += 1;
		}*/

		
		int idx_start = local_kcol_ptr[0];
		
        int idx_end = local_kcol_ptr[ local_size - 1 ] - 1;
		
        n_nnz = idx_end - idx_start + 1;
		
		std::cout<< "Local Size " << local_size <<" idx_start "<<idx_start << " idx_end " << idx_end << " n_nnz " << n_nnz<<std::endl ;
		
		        
		// SEND LOCAL SIZE to PROC I
		
        MPI_Send(&local_size, 1, MPI_UNSIGNED_LONG, i, 10, MPI_COMM_WORLD) ;
		
        
		MPI_Send(&n_nnz, 1, MPI_UNSIGNED_LONG, i, 20, MPI_COMM_WORLD) ;
		
        //tab_local_sizes[i] = (int) local_size - 1;
		
		
		MPI_Send(local_kcol_ptr, local_size, MPI_INT, i, 50, MPI_COMM_WORLD);
       
	    std::cout << "sending n_local_kcol " << n_nnz << std::endl;

        
		MPI_Send(local_cols_ptr, n_nnz, MPI_INT, i, 40, MPI_COMM_WORLD);
        
		MPI_Send(local_values_ptr, n_nnz, MPI_DOUBLE, i, 30, MPI_COMM_WORLD);
		
       
		local_kcol_ptr = local_kcol_ptr + local_size - 1 ;
		
        local_cols_ptr = local_cols_ptr + n_nnz;
		
        local_values_ptr = local_values_ptr + n_nnz;
      }
    }
		
	{
      // BROAD CAST VECTOR X
	  std::cout<<" Broadcasting the vector x"<<std::endl ;
      MPI_Bcast(x.data(), nrows, MPI_DOUBLE,0, MPI_COMM_WORLD) ;
    }
	
	CSRMatrix local_matrix ;

    {
      // EXTRACT LOCAL DATA FROM MASTER PROC
      // COMPUTE LOCAL SIZE
	  
	  //int div = nrows/nb_proc;
	  
	  local_size = (nrows/nb_proc) + 1;
	  
	  // EXTRACT LOCAL MATRIX DATA
	  
	  local_matrix.init(local_size, first_nnz);

	  
	  double *local_values_ptr = local_matrix.values();
      
	  int *local_cols_ptr = local_matrix.cols();
      
	  int *local_kcol_ptr = local_matrix.kcol();
	  
	  
	  for (unsigned int i=0;i<first_nnz;i++)
	  {
		local_values_ptr[i] = matrix.values()[i];
        local_cols_ptr[i] = matrix.cols()[i];
	  }
	  
	  for (unsigned int i=0; i<local_size; i++)
	  {
        local_kcol_ptr[i] = matrix.kcol()[i];
	  }
	  
    }

    local_y.resize(local_size - 1);
	
    {
    std::string s("SpMV_MPI_"+std::to_string(my_rank)); 
	Timer::Sentry sentry(timer, s);
    local_matrix.mult(x, local_y);
    }
	
    // SAVING LOCAL RESULTS TO THE GLOBAL Y
	
	std::vector<double> y_final;
	y_final.resize(nrows);
	
	for (auto i=0;i<local_size;i++)
	{
		y_final[i]=local_y[i];
	}
	
	for (auto i=1;i<nb_proc;i++)
	{
		MPI_Status status;
		// RECV RESULTS FROM OTHER PROCS
		MPI_Recv(y_final.data()+i*(local_size - 1 ), local_size - 1, MPI_DOUBLE, i, 0,MPI_COMM_WORLD,&status);
	}
	
	double normy_final = PPTP::norm2(y_final) ;
    std::cout<<"MPI result ||y_final||=" << normy_final << std::endl;
		
}

else
{
  CSRMatrix local_matrix ;
  {
  // RECV DATA FROM MASTER PROC
   
   MPI_Status status;
  // RECV GLOBAL SIZE
  
  MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
  
  std::cout<<"Received Global size: " << nrows << std::endl ;
  
  // RECV LOCAL SIZE
  
  MPI_Recv(&local_size,1,MPI_UNSIGNED_LONG,0,10,MPI_COMM_WORLD, &status) ;
  
  MPI_Recv(&n_nnz,1,MPI_UNSIGNED_LONG,0,20,MPI_COMM_WORLD, &status) ;
  
  //RECV MATRIX DATA
  
  
  local_matrix.init(local_size, n_nnz);
  
  int *local_kcol_ptr = local_matrix.kcol();
  
  int *local_cols_ptr = local_matrix.cols();
  
  double *local_values_ptr = local_matrix.values();
  
  
  
  MPI_Recv(local_kcol_ptr, local_size, MPI_INT, 0, 50, MPI_COMM_WORLD, &status);
  
  MPI_Recv(local_cols_ptr, n_nnz, MPI_INT, 0, 40, MPI_COMM_WORLD, &status);
  
  MPI_Recv(local_values_ptr, n_nnz, MPI_DOUBLE, 0, 30, MPI_COMM_WORLD, &status);
  
  int offset = local_kcol_ptr[0];
  
  for (int i=0; i<local_size; i++){
   
    local_kcol_ptr[i] = local_kcol_ptr[i] - offset;
    
  }
 
  }
  std::vector<double> x;
  
  {
  // BROAD CAST VECTOR X
  x.resize(nrows) ;
  MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD) ;
  }
   
   local_y.resize(local_size - 1);;
	
  {
    std::string s("SpMV_MPI_"+std::to_string(my_rank)); 
	Timer::Sentry sentry(timer, s);
    local_matrix.mult(x, local_y);
	
  }
  
  	MPI_Send(local_y.data(), local_size - 1 , MPI_DOUBLE, 0,0, MPI_COMM_WORLD) ; 
  
} 

  
  if (my_rank == 0) 
{
	mpi_time = timer.getTime(1);
}
else
{
	mpi_time = timer.getTime(0);
}

MPI_Allreduce(&mpi_time, &mpi_finalTime, 2, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
 
if (my_rank == 0) {
	
  std::ofstream outfile;
  outfile.open("Bench.log",std::ios_base::app);
  outfile << "SPMV  np: " << nb_proc << "  nx: " << nx << "  Time SPMV: "
          << timer.getTime(0) << " Time MPI: " << mpi_finalTime
          << " Acceleration: " << timer.getTime(0)/mpi_finalTime << std::endl;

 
 }

  timer.printInfo() ;  
  MPI_Finalize();
 
  return 0 ;
}