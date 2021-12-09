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

#include "MatrixVector/DenseMatrix.h"
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
      ("eigen",value<int>()->default_value(0), "use eigen package");
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

  Timer timer ;
  MatrixGenerator generator ;
  int nx = vm["nx"].as<int>() ;
  if(vm["eigen"].as<int>()==1)
  {
    typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> EigenMatrixType ;
    typedef Eigen::Matrix<double, Eigen::Dynamic, 1>              EigenVectorType;


    std::size_t nrows = nx*nx ;
    EigenMatrixType matrix(nrows,nrows) ;

    generator.genLaplacian(nx,matrix) ;

    EigenVectorType x(nrows) ;

    for(std::size_t i=0;i<nrows;++i)
      x(i) = i+1 ;

    EigenVectorType y ;
    {
      Timer::Sentry sentry(timer,"EigenDenseMV") ;
      y = matrix*x ;
    }

    double normy = PPTP::norm2(y) ;
    std::cout<<"||y||="<<normy<<std::endl ;

  }

  std::vector<double> global_y;
  int global_nrows;
  std::vector<int> counts(nb_proc);
  std::vector<int> displacement(nb_proc);

  if(my_rank==0) //master
  {
     //LECTURE DE LA MATRICE
    DenseMatrix matrix ;

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
    std::vector<double> x;
    x.resize(nrows) ;

    global_nrows = nrows;
    global_y.resize(global_nrows*2);

    for(std::size_t i=0;i<nrows;++i)
      x[i] = i+1 ;
    //FIN LECTURE DE LA MATRICE
    std::size_t master_size;
    {

      // SEND GLOBAL SIZE
      MPI_Bcast(
      &nrows,
      1,
      MPI_UNSIGNED_LONG,
      0, //Source
      MPI_COMM_WORLD);


      size_t local_size_rest;
      int local_size = (int)nrows / nb_proc;
      int reste = nrows % nb_proc;

      //double* data = (double*)malloc(((local_size+1)*nrows)*(sizeof(double)));
      double* data = matrix.data();

      std::vector<MPI_Request> requests_local_size(nb_proc-1) ;
      std::vector<MPI_Request> requests_data(nb_proc-1) ;




      for (int i=0; i<nb_proc;++i)
      {
        local_size_rest = local_size;
        // SEND LOCAL SIZE to PROC
        if(i < reste){
            local_size_rest ++;
        }

        counts[i] = local_size_rest;

        if(i == 0){
            master_size = local_size_rest;
            data += local_size_rest*nrows;
            displacement[i] = 0;
            continue; //passe a la boucle suivante
        }

        displacement[i] = 0;
        for(int j=nb_proc; j > 0 ; j--){
            displacement[i] += counts[i];
        }


        // std::cout<<" Sending size " << local_size_rest <<" to proc "<<i<<std::endl ;
        MPI_Isend(
        &local_size_rest,
        1,
        MPI_UNSIGNED_LONG,
        i,
        2,
        MPI_COMM_WORLD,
        &requests_local_size[i-1]);


        // std::cout<<" Sending data to proc "<<i<<std::endl ;
        // for(int j =0; j < nrows; j++){
        //     std::cout << data[j] << " , ";
        // }
        // std::cout << std::endl;

        // SEND MATRIX DATA
        MPI_Isend(
        data,
        local_size_rest*nrows,
        MPI_DOUBLE,
        i,
        3,
        MPI_COMM_WORLD,
        &requests_data[i-1]);

        data += local_size_rest*nrows;
      }
      MPI_Status st;
      for(auto r : requests_local_size){
          MPI_Wait(&r, &st);
      }
      for(auto r : requests_data){
          MPI_Wait(&r, &st);
      }
    }

    {
      // BROAD CAST VECTOR X
      MPI_Bcast(
      x.data(),
      nrows,
      MPI_DOUBLE,
      0, //Source
      MPI_COMM_WORLD);

    }

    {
      std::vector<double> y(nrows);
      {
        Timer::Sentry sentry(timer,"DenseMV") ;
        matrix.mult(x,y) ;
      }
      double normy = PPTP::norm2(y) ;
      std::cout<<"Rank 0 ||y||="<<normy<<std::endl ;
    }


    // COMPUTE LOCAL MATRICE LOCAL VECTOR ON PROC 0
    DenseMatrix local_matrix ;
    std::size_t local_nrows = master_size;
    std::vector<double> master_y(local_nrows);

      double const* matrix_ptr = matrix.data();
      for(std::size_t irow =0; irow<local_nrows;++irow)
      {
        double value = 0 ;
        for(std::size_t jcol =0; jcol<nrows;++jcol)
        {
          value += matrix_ptr[jcol]*x[jcol] ;
        }
        master_y[irow] = value ;
        matrix_ptr += nrows ;
      }



    //GATHER POUR RECREER LE vrai Y
    // MPI_Gather(
    //     &master_y,
    //     local_nrows,
    //     MPI_DOUBLE,
    //     global_y.data(),     //RECEIVE DATA
    //     global_nrows,  //RECEIVE SIZE
    //     MPI_DOUBLE,
    //     0,              //ROOT
    //     MPI_COMM_WORLD); //COMMUNICATOR

        MPI_Gatherv(
            master_y.data(),
            local_nrows,
            MPI_DOUBLE,
            global_y.data(),
            counts.data(),         //counts
            displacement.data(),    //displacement
            MPI_DOUBLE,
            0,
            MPI_COMM_WORLD);

        double normy = PPTP::norm2(global_y) ;
        std::cout<<"GatherV ||y||="<<normy<<std::endl ;
    // std::cout << "\n\n\n######################### GLOBAL Y IS :\n" << std::endl;
    // for(int i = 0; i < global_y.size(); ++i){
    //     std::cout << global_y[i] << " , ";
    // }
    // std::cout << "\n#########################\n\n\n" << std::endl;

  }
  else
  {
    // COMPUTE LOCAL MATRICE LOCAL VECTOR
    // std::cout << "Hello, I am proc number (" << my_rank << "," << nb_proc << ')' << std::endl;

    DenseMatrix local_matrix;
    std::size_t nrows;
    std::size_t local_nrows;

    {
      // RECV DATA FROM MASTER PROC
      // RECV GLOBAL SIZE
      MPI_Bcast(
      &nrows,
      1,
      MPI_UNSIGNED_LONG,
      0, //source
      MPI_COMM_WORLD);

      // nrows = (size_t)global_size;
      // std::cout << "Receive global size from master : " << nrows << std::endl;

      // RECV LOCAL SIZE
      MPI_Status status;
      MPI_Recv(
      &local_nrows,             //Buffer itself
      1,              //Size of buffer
      MPI_UNSIGNED_LONG,        //Type
      0,              //Source
      2,             //Tag - NEED to be the same as sender
      MPI_COMM_WORLD, //Communicator
      &status         //MPI Status
      );
      // std::cout << "Receive local size from master : " << local_nrows << std::endl;

      // RECV MATRIX DATA

      local_matrix.init(local_nrows, nrows); //initializing empty matrix
      // double* ptr =
      MPI_Recv(
      local_matrix.data(),  //Buffer itself
      local_nrows*nrows,    //Size of buffer
      MPI_DOUBLE,           //Type
      0,                    //Source
      3,                    //Tag - NEED to be the same as sender
      MPI_COMM_WORLD,       //Communicator
      &status               //MPI Status
      );

      //Displaying the first row of the data
      // std::cout << "Receive actual matrix from master size : [" << local_nrows << "," << nrows << "]\nVALUES : " << std::endl;
      // for(int i = 0; i < nrows; i += 1){
      //     std::cout << local_matrix(0,i) << " , ";
      // }
      std::cout << std::endl;
    }

    std::vector<double> x(nrows);
    {
      // BROAD CAST VECTOR X
      MPI_Bcast(
        x.data(),
        nrows,
        MPI_DOUBLE,
        0, //Source
        MPI_COMM_WORLD
        );

        // std::cout << "receive vector x size is : " << x.size() << std::endl;
    }

    std::vector<double> local_y(local_nrows);
    {
        local_matrix.mult(x, local_y);
    }
    //GATHER VERS LE 0 BIG TIME
    // MPI_Gather(
    //     &local_y,
    //     local_nrows,
    //     MPI_DOUBLE,
    //     global_y.data(),
    //     global_nrows,
    //     MPI_DOUBLE,
    //     0,              //root
    //     MPI_COMM_WORLD); //communicator

    MPI_Gatherv(local_y.data(),
                local_nrows,
                MPI_DOUBLE,
                NULL,
                NULL,
                NULL,
                MPI_DOUBLE,
                0,
                MPI_COMM_WORLD);

                // global_y.data(),
                // counts,         //counts
                // displacement,    //displacement
                // MPI_DOUBLE,
                // 0,
                // MPI_COMM_WORLD);

  }

  MPI_Finalize();
  if(my_rank == 0){

      timer.printInfo() ;
  }

  return 0 ;
}
