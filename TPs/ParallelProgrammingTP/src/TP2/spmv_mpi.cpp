/*
 * helloworld.cpp
 *
 *  Created on: Aug 16, 2018
 *      Author: gratienj
 */
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
      ("nb-threads",value<int>()->default_value(0), "nb threads")
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
  MPI_Init(&argc,&argv) ;

  int my_rank = 0 ;
  int nb_proc = 1 ;
  MPI_Comm_size(MPI_COMM_WORLD,&nb_proc) ;
  MPI_Comm_rank(MPI_COMM_WORLD,&my_rank) ;

  using namespace PPTP ;

  Timer timer_seq, timer_mpi ;
  MatrixGenerator generator ;
  int nx = vm["nx"].as<int>() ;
  if(vm["eigen"].as<int>()==1)
  {
    typedef Eigen::SparseMatrix<double> MatrixType ;
    typedef Eigen::VectorXd             VectorType ;
    MatrixType matrix ;
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

    std::size_t nrows = matrix.rows();
    VectorType x(nrows);

    for(std::size_t i=0;i<nrows;++i)
      x(i) = i+1 ;

    VectorType y ;
    {
      Timer::Sentry sentry(timer_seq,"EigenSpMV") ;
       y = matrix*x ;
    }

    double normy = PPTP::norm2(y) ;
    std::cout<<"||y||="<<normy<<std::endl ;
  }
  else
  {	
	if (my_rank==0)
	{	
		//INITIALISATION
			
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
	
		std::cout<<"SPMV MPI with np="<<nb_proc<<" and nx="<<nx<<std::endl;

		std::size_t nrows = matrix.nrows();
		std::vector<int> kcol = matrix.kcol();
		std::vector<int> cols = matrix.cols();
		std::vector<double> values = matrix.values();
		
		std::vector<double> x,y,y2 ;
		x.resize(nrows) ;
		y.resize(nrows) ;
		y2.resize(nrows) ;
		
		std::size_t index_kcol_end_0 = 0;
		int kcol_number;
		std::size_t index_kcol_start;
		std::size_t index_kcol_end;

		for(std::size_t i=0;i<nrows;++i)
		  x[i] = i+1 ;
		
		//std::cout<<"Matrix 0"<<std::endl ;
		//matrix.print();
		
		{//SEQUENTIAL PART

		{
			Timer::Sentry sentry(timer_seq,"SpMV Sequential") ;
			matrix.mult(x,y) ;		
		}
		
		//printf("\nSequential - Vector Y\n\n");
		/*
		for (std::size_t i = 0; i<nrows; i++) {
			std::cout << y[i] << std::endl ;
		}
		*/
		std::cout << std::endl ;
		double normy = PPTP::norm2(y) ;
		std::cout<<"Sequential ||y||="<<normy<<std::endl ;
			
		}
		
		{//MPI Part
		
			Timer::Sentry sentry(timer_mpi, "SpMV MPI");
			
			{//SEND INFORMATION
			
				// SEND GLOBAL SIZE
				
				//MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD) ;
				
				std::size_t local_kcol_number = values.size()/nb_proc ; //gets the number of values and divide by number of proc
				
				
				{// SEND MATRIX DATA
				
					// SEND GLOBAL SIZE
					MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD) ;
					
					//will split data evenly according to the number of values and not the rows
					do {// CALCULATE END INDEX of KCOL for PROC 0
						index_kcol_end_0+=1;
						kcol_number=kcol[index_kcol_end_0];
					} 
					while(kcol_number<local_kcol_number);			
					
					//std::cout<<"index_kcol_end_0 = "<<index_kcol_end_0<<std::endl;
					//std::cout<<"kcol[index_kcol_end_0] = "<<kcol[index_kcol_end_0]<<std::endl;
					
					index_kcol_start = index_kcol_end_0;
					index_kcol_end = index_kcol_end_0;
					
					for (int destination=1; destination<nb_proc;++destination)
					{	
						
						do {// CALCULATE END DATA PROC I
							index_kcol_end+=1;
							kcol_number=kcol[index_kcol_end];
						} 
						while(kcol_number<local_kcol_number*(destination+1));
						
						//std::cout<< destination << " ; index_kcol_end = " << index_kcol_end <<std::endl;
						
						//std::cout<<" SEND MATRIX DATA to proc "<<destination<<std::endl ;
						
						if(destination==nb_proc-1){//To make sure that the last proc calculate all the remaining data
							index_kcol_end = nrows;				
						}
						
						//SEND LENGHT KCOL to PROC I
						std::size_t lenght_index_kcol = index_kcol_end - index_kcol_start;
						MPI_Send(&lenght_index_kcol, 1, MPI_UNSIGNED_LONG, destination, 0, MPI_COMM_WORLD) ;
						
						//SEND LENGHT VALUES to PROC I
						std::size_t lenght_values = kcol[index_kcol_end]-kcol[index_kcol_start];
						MPI_Send(&lenght_values, 1, MPI_UNSIGNED_LONG, destination, 0, MPI_COMM_WORLD) ;
						
						// SEND KCOL SPLIT to PROC I
						//double const* kcol_ptr = matrix.data_kcol() + (*index_kcol_start) ;
						int const* kcol_ptr = matrix.data_kcol() + index_kcol_start;
						MPI_Send(kcol_ptr, lenght_index_kcol+1, MPI_INT, destination, 0, MPI_COMM_WORLD) ;
						
						// SEND COL SPLIT to PROC I
						int const* col_ptr = matrix.data_cols() + kcol[index_kcol_start] ;
						MPI_Send(col_ptr, lenght_values, MPI_INT, destination, 0, MPI_COMM_WORLD) ;

						// SEND VALUES to PROC I
						double const* matrix_ptr = matrix.data() + kcol[index_kcol_start] ;
						MPI_Send(matrix_ptr, lenght_values, MPI_DOUBLE, destination, 0, MPI_COMM_WORLD) ;
						
						index_kcol_start = index_kcol_end;
						
					}
				}
				
				{
				  // BROAD CAST VECTOR X
				  MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD) ;
				}
			
			}
			
			{// COMPUTE ON PROC 0
				
				//Initialisation
				
				CSRMatrix local_matrix ;			
				std::vector<int> local_kcol;
				std::vector<int> local_cols;
				std::vector<double> local_values;
				std::vector<double> z(nrows);
				std::vector<double> local_y(index_kcol_end_0); //changed local_nrows to number of line used in mult (lenght_index_kcol)
				
				
				
				{// EXTRACT LOCAL MATRIX DATA
				
					local_kcol.insert(local_kcol.begin(), kcol.begin(), kcol.begin() + index_kcol_end_0 + 1);
					local_kcol[index_kcol_end_0]=kcol[index_kcol_end_0];
					//std::cout<<"local_kcol[index_kcol_end_0] = "<<local_kcol[index_kcol_end_0]<<std::endl;
					
					
					local_cols.insert(local_cols.begin(), cols.begin(), cols.begin() + local_kcol[index_kcol_end_0]);
					//std::cout<<"0 ; local_length_value = "<<local_cols.size()<<std::endl;
					local_values.insert(local_values.begin(), values.begin(), values.begin() + local_kcol[index_kcol_end_0]);
					local_matrix = CSRMatrix(index_kcol_end_0, kcol[index_kcol_end_0], local_kcol, local_cols, local_values);
					//local_matrix.setSparse(local_kcol, local_cols, local_values);
					//std::cout<<"Printing Matrix PROC 0\n";
					//local_matrix.print();
					
				}
			
				{// COMPUTE PROC 0
				  
				  local_matrix.mult(x, local_y);
				  
				}
				
				{//GATHER DATA
				
					for (std::size_t i = 0; i<index_kcol_end_0; i++) {
						z[i] = local_y[i] ;
					}
					
					// GATHER
					//MPI_Gatherv(local_y.data(), local_lenght_index_kcol, MPI_DOUBLE, z.data(), recvcounts.data(), displs.data(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
					
					std::size_t cumulated_length = index_kcol_end_0;
					
					for (int source=1; source<nb_proc; ++source)
					{	
						std::size_t local_lenght_index_kcol;
						//std::cout << " RECEIVE VECTOR FROM proc " << source << std::endl ;
						
						MPI_Status status ;
						MPI_Recv(&local_lenght_index_kcol, 1, MPI_UNSIGNED_LONG, source, 1, MPI_COMM_WORLD, &status);
						
						std::vector<double> local_y(local_lenght_index_kcol);
						
						MPI_Recv(local_y.data(), local_lenght_index_kcol, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, &status);
						
						
						
						for (std::size_t i = 0; i<local_lenght_index_kcol; i++) {
							z[cumulated_length+i] = local_y[i] ;
						}
						
						cumulated_length+=local_lenght_index_kcol;
						  
					}		
					
					double normz = PPTP::norm2(z);
					std::cout << "MPI ||y_MPI||=" << normz << std::endl;
				}
			
			}
		
		}
		
		{//Timer info
				
			timer_seq.printInfo();
			timer_mpi.printInfo();

			std::cout<<" Timer seq = "<< timer_seq.sum_time() <<"\n";
			std::cout<<" Timer mpi = "<< timer_mpi.sum_time() <<"\n";
				
				
		}
		
	}
	else
	{
		{// COMPUTE ON PROC I
			
			//Initialisation
			
			CSRMatrix local_matrix ;			

			std::size_t local_lenght_index_kcol;
			std::size_t local_lenght_values;
				
			
			
			{// RECEIVE LOCAL MATRIX DATA
				
				MPI_Status status ;
				int source = 0;
				
				//RECEIVE LENGHT KCOL to PROC I
				MPI_Recv(&local_lenght_index_kcol, 1, MPI_UNSIGNED_LONG, source, 0, MPI_COMM_WORLD, &status) ;
				
				//RECEIVE LENGHT VALUES to PROC I
				MPI_Recv(&local_lenght_values, 1, MPI_UNSIGNED_LONG, source, 0, MPI_COMM_WORLD, &status) ;
				//std::cout<< my_rank <<" ; local_lenght_values "<< local_lenght_values <<"\n";
				
				// RECEIVE KCOL SPLIT to PROC I
				std::vector<int> local_kcol(local_lenght_index_kcol+1);
				MPI_Recv(local_kcol.data(), local_lenght_index_kcol+1, MPI_INT, source, 0, MPI_COMM_WORLD, &status) ;
				
				// RECEIVE COL SPLIT to PROC I
				std::vector<int> local_cols(local_lenght_values);
				MPI_Recv(local_cols.data(), local_lenght_values, MPI_INT, source, 0, MPI_COMM_WORLD, &status) ;
				//std::cout<< my_rank <<" ; local_cols.size "<< local_cols.size() <<"\n";

				// RECEIVE VALUES to PROC I
				std::vector<double> local_values(local_lenght_values);
				MPI_Recv(local_values.data(), local_lenght_values, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, &status) ;
				
				local_matrix = CSRMatrix(local_lenght_index_kcol, local_kcol[local_lenght_index_kcol], local_kcol, local_cols, local_values);
				
				//std::cout<< my_rank <<" ; local_matrix.local_cols.size "<< local_matrix.cols().size() <<"\n";
				
				//std::cout<<"Printing Matrix PROC "<< my_rank <<"\n";
				//local_matrix.print();
				
			}
			
			// RECV GLOBAL SIZE
			std::size_t nrows ;
			MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD) ;
			
			std::vector<double> x(nrows);
			{// BROAD CAST VECTOR X			  
			  MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD) ;
			}
		
			std::vector<double> local_y(local_lenght_index_kcol);
			{// COMPUTE PROC I
			  
			  
			  local_matrix.mult(x,local_y);
			}

			//MPI_Gatherv(local_y.data(), local_lenght_index_kcol, MPI_DOUBLE, NULL, NULL, NULL, MPI_DOUBLE, 0, MPI_COMM_WORLD);
			
			// SEND BACK THE LOCAL VECTOR
			int destination = 0;
			MPI_Send(&local_lenght_index_kcol, 1, MPI_UNSIGNED_LONG, destination, 1, MPI_COMM_WORLD);
			MPI_Send(local_y.data(), local_lenght_index_kcol, MPI_DOUBLE, destination, 1, MPI_COMM_WORLD);	
			
		}

	}

  }
  
  MPI_Finalize() ;
  
  return 0 ;
}
