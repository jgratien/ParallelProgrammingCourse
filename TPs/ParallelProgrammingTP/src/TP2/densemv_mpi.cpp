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
#include <chrono> 
#include "MatrixVector/DenseMatrix.h"
#include "MatrixVector/CSRMatrix.h"
#include "MatrixVector/LinearAlgebra.h"
#include "MatrixVector/MatrixGenerator.h"

#include "Utils/Timer.h"
using namespace std::chrono;

int main(int argc, char** argv)
{
	using namespace boost::program_options ;
	options_description desc;
	desc.add_options()
		("help", "produce help")
		("nrows",value<int>()->default_value(0), "matrix size")
		("nx",value<int>()->default_value(0), "nx grid size")
		("zero",value<double>()->default_value(1.0), "MASTER_AMOUNT_WORK")

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
	double norm =0.0;
	double normT = 0.0;

	using namespace PPTP ;
	using namespace std;

	Timer timer ;
//	MatrixGenerator generator ;
	//int nx = vm["nx"].as<int>() ;
	//if(vm["eigen"].as<int>()==1)
		/*{
		  typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> EigenMatrixType ;
		  typedef Eigen::Matrix<double, Eigen::Dynamic, 1>              EigenVectorType;


		  std::size_t nrows = nx*nx ;
		  EigenMatrixType matrix(nrows,nrows) ;

		  generator.genLaplacian(nx,matrix) ;

		  EigenVectorType x(nrows) ;
		  for(std::size_t i=0;i<nrows;++i)
		  x(i) = i+1 ;;

		  EigenVectorType y ;
		  {
		  Timer::Sentry sentry(timer,"EigenDenseMV") ;
		  y = matrix*x ;
		  }

		  double normy = PPTP::norm2(y) ;
		  std::cout<<"||y||="<<normy<<std::endl ;

		  }*/

		auto s = std::to_string(nb_proc);
	std::ofstream benchmark;
	benchmark.open (s+"_nbproc_report_DENSEMV_MPI_.csv");
	benchmark << "NX(rows),Distribution_Time(ms),Calculation_Time(ms),Total_Time(ms),Nb_Proc\n";
	int nxc[12] = {10,20,40,60,80,100,120,140,160,180,200,220};
	for(int iter =0; iter<17; iter++)
	{ 

		if(my_rank==0)
		{
			//auto start = high_resolution_clock::now();
			int nx = nxc[iter] ;
	MatrixGenerator generator ;

			DenseMatrix matrix ;
			MPI_Request request;
			if(vm.count("file"))
			{
				std::string file = vm["file"].as<std::string>() ;
				generator.readFromFile(file,matrix) ;
			}
			else
			{
				generator.genLaplacian(nx,matrix) ;
			}

			std::size_t nrows = nx*nx ;
			std::vector<double> x;
			x.resize(nrows) ;
			std::vector<double> const &values = matrix.get_values();
			for(std::size_t i=0;i<nrows;++i)
				x[i] = i+1 ;


			{
				std::vector<double> y(nrows);
				{
					Timer::Sentry sentry(timer,"DenseMV") ;
					matrix.mult(x,y) ;
				
				double normy = PPTP::norm2(y) ;
				std::cout<<"||y||="<<normy<<std::endl ;}
			}

			auto start = high_resolution_clock::now();
			auto start2 = high_resolution_clock::now();



			// SEND GLOBAL SIZE
			int nrows_int = nrows;
			/*int nrows_local = (nrows/nb_proc)/2;
			  int r = nrows % nb_proc;*/ 
			MPI_Bcast(&nrows_int, 1, MPI_INT, 0, MPI_COMM_WORLD);

			// SEND MATRIX

			int begin = 0;
			double zero = vm["zero"].as<double>() ;

			int nrows_local_proc_zero =  (int)((nrows/nb_proc)/zero);
	int nrows_remain = nrows - nrows_local_proc_zero;

			if(nb_proc>1)

			// if(r>0) nrows_local_proc_zero++; 

			{	
			int r = nrows_remain%(nb_proc-1);

			cout<<"\nProcessor ZERO is dealing with "<<nrows_local_proc_zero<<" rows"<<std::endl;
			begin += nrows_local_proc_zero;


			for (int i=1; i<nb_proc; ++i)
			{
				std::cout<<" SEND MATRIX DATA to proc "<<i<<std::endl ;

				// SEND LOCAL SIZE to PROC I
				int nrows_local = nrows_remain/(nb_proc-1);

				if(i<r+1) { nrows_local++;}

				MPI_Send(&nrows_local, 1, MPI_INT, i, 1000, MPI_COMM_WORLD);
				double const * ptr = values.data() + begin*nrows;

				MPI_Send(ptr, nrows_local*nrows, MPI_DOUBLE, i, 2000, MPI_COMM_WORLD);
				begin += nrows_local;
			}



			// BROAD CAST VECTOR X

			//std::vector<double> x_to_sendi;
			//MPI_Ibarrier(MPI_COMM_WORLD, &request);
			MPI_Bcast(x.data(), nrows_int, MPI_DOUBLE, 0, MPI_COMM_WORLD); }
			auto stop = high_resolution_clock::now();
			auto duration1 = duration_cast<milliseconds>(stop - start); 

			cout <<"\n\nThe distribution part took "<< duration1.count()<<" milliseconds\n" << std::endl; 
			start = high_resolution_clock::now();
			double s ;
			int beg = 0 ;
			std::vector<double> result(nrows,0.0);
			for(int i = 0; i<nrows_local_proc_zero; i++)

			{   
				s = 0 ;
				for(int j=0; j<nrows; j++)
				{
					double const * ptr1 = values.data() + beg;
					s += (*ptr1) * x.at(j) ;
					beg++;
				}

				result.at(i) = s; 
				norm += s*s;    
			}
			beg =  nrows_local_proc_zero;
			MPI_Status status;
			// COMPUTE LOCAL SIZE
			for (int t=1; t<nb_proc; t++)

			{    
				int size_to_receive = nrows/nb_proc;
				int r = nrows_remain%(nb_proc-1);

				if (t<r+1) size_to_receive++;

				std::vector<double> result_to_receive(size_to_receive,0.0);


				MPI_Recv(&result_to_receive[0], size_to_receive, MPI_DOUBLE, t, 3000, MPI_COMM_WORLD, &status);
				//cout<<"\nReceived result from processor "<<t<<" and the first element is "<<result_to_receive[0]<<"  "<<result_to_receive[1]<<"   "<<result_to_receive[2];
				//cout<<"\nResult received from processor "<<t<<std::endl;
				std::copy(result_to_receive.begin(), result_to_receive.end(), result.begin()+beg);
				beg += size_to_receive;
			}
			//MPI_Waitall(4, reqs, stats);
			//MPI_Barrier(MPI_COMM_WORLD);
			MPI_Reduce(&norm, &normT, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
			normT = sqrt(normT);
			std::cout<<"\nNorm of the resulting vector = "<<normT<<std::endl;
			stop = high_resolution_clock::now(); 
			auto duration2 = duration_cast<milliseconds>(stop - start); 
			cout <<"\n\nThe calculation part took "<< duration2.count()<<" milliseconds\n" << std::endl; 
			auto stop2 = high_resolution_clock::now(); 
			auto duration3 = duration_cast<milliseconds>(stop2 - start2); 

			cout <<"\n\nThe whole process of multiplication took "<< duration3.count()<<" milliseconds\n" << std::endl; 

			benchmark<<nx<<","<<duration1.count()<<","<<duration2.count()<<","<<duration3.count()<<","<<nb_proc<<"\n";	

		}
		else
		{
			// COMPUTE LOÂii²CAL MATRICE LOCAL VECTOR

			DenseMatrix local_matrix ;
			int nrows = 0 ;
			int local_nrows=0 ;
			// RECV DATA FROM MASTER PROC
			//MPI_Request request;

			// RECV Global Size
			MPI_Bcast(&nrows, 1, MPI_INT, 0, MPI_COMM_WORLD);

			// RECV LOCAL SIZE

			MPI_Status status;
			MPI_Recv(&local_nrows, 1, MPI_INT, 0, 1000, MPI_COMM_WORLD, &status);
			cout<<"\nProcessor "<<my_rank<<" is dealing with "<<local_nrows<<" rows"<<std::endl;


			std::vector<double> local_vector(nrows*local_nrows);
			//RECV MATRIX DATA
			MPI_Recv(local_vector.data(), nrows*local_nrows, MPI_DOUBLE, 0, 2000, MPI_COMM_WORLD, &status);




			// BROAD CAST VECTOR X
			std::vector<double> x_to_receive(nrows);
			//MPI_Ibarrier(MPI_COMM_WORLD, &request);
			MPI_Bcast(x_to_receive.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);
			//MPI_Waitall(4, reqs, stats);

			//MPI_Barrier(MPI_COMM_WORLD);




			std::vector<double> result_to_send(local_nrows, 0.0);
			for(int i = 0; i<local_nrows; i++)
			{   
				double s = 0 ;
				for(int j=0; j<nrows; j++)
				{

					s += local_vector[i*nrows+j] * x_to_receive.at(j);
				}

				result_to_send.at(i) = s;
				norm += s*s;
			}

			MPI_Send(&result_to_send[0], local_nrows, MPI_DOUBLE, 0, 3000, MPI_COMM_WORLD);
			// MPI_Waitall(4, reqs, stats);
			MPI_Reduce(&norm, &normT, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);



		} 
	}


	MPI_Finalize(); 
	return 0 ;
}
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
