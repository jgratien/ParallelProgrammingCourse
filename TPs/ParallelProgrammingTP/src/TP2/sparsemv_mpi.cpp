#include <mpi.h>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/variables_map.hpp>
#include "omp.h"
#include "tbb/tbb.h"
#include <vector>
#include <fstream>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/LU>
#include "MatrixVector/CSRMatrix.h"
#include "MatrixVector/LinearAlgebra.h"
#include "MatrixVector/MatrixGenerator.h"
#include "Utils/Timer.h"
// #include "utils.h"

//make install &> make.log ; grep error make.log
//mpirun --np 3 ../bin/sparsemv_mpi.exe --nx 2 --debug true --m-type 1

//Ca remarche, cetait un probleme de buffer qui se supprimait avant detre envoye en asycnrhone
//mpirun --np 3 ../bin/sparsemv_mpi.exe --nx 17 --debug true  FONCTIONNE
//mpirun --np 3 ../bin/sparsemv_mpi.exe --nx 18 --debug true   BUG

using namespace PPTP;
int main(int argc, char ** argv) {

    using namespace boost::program_options;
    options_description desc;
    desc.add_options()
        ("help", "produce help")
        ("nx", value < int > () -> default_value(0), "nx grid size")
        ("eigen", value < int > () -> default_value(0), "use eigen package")
        ("debug", value < bool > () -> default_value(false), "use debug")
        ("m-type", value < int > () -> default_value(0), "matrix type : 1 = 4x4, 2 = diagonal, 3 = 5x5");
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }

    Timer timer;
    MatrixGenerator generator;
    int nx = vm["nx"].as < int > ();

    int m_type = vm["m-type"].as<int>();

    bool debug = vm["debug"].as< bool >();

    if (vm["eigen"].as < int > () == 1) {
        typedef Eigen::Matrix < double, Eigen::Dynamic, Eigen::Dynamic > EigenMatrixType;
        typedef Eigen::Matrix < double, Eigen::Dynamic, 1 > EigenVectorType;

        std::size_t nrows = nx * nx;
        EigenMatrixType matrix(nrows, nrows);

        generator.genLaplacian(nx, matrix);

        EigenVectorType x(nrows);

        for (std::size_t i = 0; i < nrows; ++i)
            x(i) = i + 1;

        EigenVectorType y; {
            Timer::Sentry sentry(timer, "EigenSparseMV");
            y = matrix * x;
        }

        double normy = PPTP::norm2(y);
        std::cout << ">> Eigen : ||y||=" << normy << std::endl;
    }

    CSRMatrix matrix;
    generator.genLaplacian(nx, matrix);

    int my_rank = 0;
    int nb_proc = 1;
    MPI_Init( & argc, & argv);
    MPI_Comm_size(MPI_COMM_WORLD, & nb_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, & my_rank);

    std::size_t nrows = matrix.nrows();
    std::vector < double > x, y, y2, yfinal;
    x.resize(nrows);
    y.resize(nrows);
    y2.resize(nrows);
    yfinal.resize(nrows);


    //Declaring here so every proc knows the variable
    std::vector<int>    local_kcol{};
    std::vector<int>    local_col{};
    std::vector<double> local_values{};
    std::vector<int> counts(nb_proc);
    std::vector<int> displacement(nb_proc);

    for (std::size_t i = 0; i < nrows; ++i)
        x[i] = i + 1;


    if(my_rank == 0){
        {
            Timer::Sentry sentry(timer, "SpMV_seq");
            matrix.mult(x, y);
        }

        double normy = PPTP::norm2(y);
        std::cout << "SpMV no MPI ||y||=" << normy << std::endl;

    }


    // Timer::Sentry sentry(timer, "SpMV MPI");
    if(my_rank == 0){
        //to save the first result and store it later in local_variables
        std::vector<int>    save_kcol;
        std::vector<int>    save_col;
        std::vector<double> save_values;
        // std::vector<double> save_vect;

        //Sending stuff to other
        std::vector<MPI_Request> requests_values(nb_proc-1) ;
        std::vector<MPI_Request> requests_col(nb_proc-1) ;
        std::vector<MPI_Request> requests_kcol(nb_proc-1) ;

        for (int num_proc=0; num_proc < nb_proc; ++num_proc)
        {

            /*We simple split the matrix by rows since we are solving a
            laplacian problem don't need to be careful at
            the charge of each proc, the should have about the same size to compute
            */
            size_t local_size_rest;
            int local_size = (int)nrows / nb_proc;
            int rest = nrows % nb_proc;

            local_size_rest = local_size;
            if(num_proc < rest){
                local_size_rest ++;
            }


            int step = 0;
            if(num_proc > 0){
                for(int j = 0; j < counts.size()-1; ++j){
                    step += counts[j];
                }
            }

            displacement[num_proc] = step;

            counts[num_proc]=(int)local_size_rest;
            //####################################################
            //Getting kcol
            for(int j = 0; j < local_size_rest + 1; ++j){
                local_kcol.push_back(matrix.kcol()[j + step]);
            }
            std::vector<int> name;


            //####################################################
            //Getting Values

            /*Le dernier elementde kcol nous indique l'indice de
            fin pour recupere les local_values */
            for(int j = local_kcol[0]; j < local_kcol[local_kcol.size()-1]; ++j){
                local_values.push_back(matrix.values()[j]);
            }


            //####################################################
            //Getting cols
            for(int j = local_kcol[0]; j < local_kcol[local_kcol.size()-1]; ++j){
                local_col.push_back(matrix.cols()[j]);
            }



            int start = *(matrix.kcol().data() +  displacement[num_proc]);
            int end = *(matrix.kcol().data() +  displacement[num_proc] + counts[num_proc]);
            int length = end - start;

            if(num_proc > 0) {
                // std::vector<size_t> sizes = {
                //     local_values.size(),
                //     local_col.size(),
                //     local_kcol.size()
                // };
                std::vector<size_t> sizes{
                    length,
                    length,
                    counts[num_proc]+1
                };

                MPI_Send(
                sizes.data(),
                3,
                MPI_UNSIGNED_LONG,
                num_proc,
                1,
                MPI_COMM_WORLD);

                if(debug)
                {
                    std::cout << num_proc<< " Proc \n";
                    std::cout << length << " = VALUES SIZE\n";
                    std::cout << counts[num_proc]+1 << " = KCOL SIZE\n";

                    std::cout << start << " = START\n";
                    std::cout << end << " = END\n";
                    std::cout << length << " = LENGTH\n";
                }


                MPI_Send(
                local_values.data(),
                local_values.size(),
                MPI_DOUBLE,
                num_proc,
                2,
                MPI_COMM_WORLD);

                MPI_Send(
                local_col.data(),
                local_col.size(),
                MPI_INT,
                num_proc,
                3,
                MPI_COMM_WORLD);

                MPI_Send(
                local_kcol.data(),
                local_kcol.size(),
                MPI_INT,
                num_proc,
                4,
                MPI_COMM_WORLD);

                // MPI_Isend(
                // local_values.data(),
                // local_values.size(),
                // MPI_DOUBLE,
                // num_proc,
                // 2,
                // MPI_COMM_WORLD,
                // &requests_values[num_proc-1]);
                //
                // MPI_Isend(
                // local_col.data(),
                // local_col.size(),
                // MPI_INT,
                // num_proc,
                // 3,
                // MPI_COMM_WORLD,
                // &requests_col[num_proc-1]);
                //
                // MPI_Isend(
                // local_kcol.data(),
                // local_kcol.size(),
                // MPI_INT,
                // num_proc,
                // 4,
                // MPI_COMM_WORLD,
                // &requests_kcol[num_proc-1]);

            }

            else{
                std::cout << "MPI_Wait";
                save_kcol = local_kcol;
                save_col = local_col;
                save_values = local_values;
                // save_vect = local_vect;
            }

            //Since we are clearing the buffer, we can't do async
            //because the buffer gets cleared before getting sent if its too big
            local_kcol.clear();
            local_values.clear();
            local_col.clear();
        }


        // for(auto r : requests_values){
        //   MPI_Wait(&r, MPI_STATUS_IGNORE);
        // }
        // for(auto r : requests_col){
        //   MPI_Wait(&r, MPI_STATUS_IGNORE);
        // }
        // for(auto r : requests_kcol){
        //   MPI_Wait(&r, MPI_STATUS_IGNORE);
        // }


        //We load back our good arrays
        local_kcol = save_kcol;
        local_col = save_col;
        local_values = save_values;

    }
    else{
        std::vector<size_t>sizes(3);
        size_t size_v, size_c, size_kc;

        MPI_Recv(
            sizes.data(),        //Buffer itself
            3,                   //Size of buffer
            MPI_UNSIGNED_LONG,   //Type
            0,                   //Source
            1,                   //Tag - NEED to be the same as sender
            MPI_COMM_WORLD,      //Communicator
            MPI_STATUS_IGNORE    //MPI Status
        );

        size_v = sizes[0];
        size_c = sizes[1];
        size_kc = sizes[2];

        local_values.resize(size_v);
        local_col.resize(size_c);
        local_kcol.resize(size_kc);

        //Recevoir les tableaux
        MPI_Recv(
            local_values.data(),
            size_v,
            MPI_DOUBLE,
            0,
            2,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE
        );

        MPI_Recv(
            local_col.data(),
            size_c,
            MPI_INT,
            0,
            3,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE
        );

        MPI_Recv(
            local_kcol.data(),
            size_kc,
            MPI_INT,
            0,
            4,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE
        );

        int sub = local_kcol[0];
      //we substract the first element of kcol to simulate a new csr to compute
        for(int i = 0; i < local_kcol.size(); ++i){
            local_kcol[i] -= sub;
        }

    }


    //For everyone, do:

    //Create a CSR matric with our arrays
    CSRMatrix my_matrix(local_kcol, local_col, local_values);
    std::vector<double> local_y(local_kcol.size()-1);
    //Faire le calcul
    {
        std::string str("SpMV_MPI_" + std::to_string(my_rank));
        Timer::Sentry sentry(timer, str);
        my_matrix.mult(x, local_y);
    }
    if(debug){
        double normy = PPTP::norm2(local_y);
        std::cout << "|| Norm y ||" << my_rank << " = " << normy << "\n";
    }

    int s = local_y.size();

    //Envoyer/Recuperer les resultats
    if(my_rank == 0){
        MPI_Gatherv(
            local_y.data(),         //buffer send
            s,         //Count send
            MPI_DOUBLE,
            yfinal.data(),
            counts.data(),         //counts receive
            displacement.data(),    //displacement
            MPI_DOUBLE,
            0,
            MPI_COMM_WORLD
            );

            if(debug){
                std::cout << "\nGlobal y size = " << yfinal.size() << "\n";

                std::cout << "COUNTS : ";
                for(int i = 0; i<counts.size(); i++){
                    std::cout << counts[i] << ", ";
                }

                std::cout << "\nDISPLACEMENT : ";
                for(int i = 0; i<displacement.size(); i++){
                    std::cout << displacement[i] << ", ";
                }
                std::cout << "\n" << std::endl;
            }

            if(debug)
            {
                std::cout << "Displaying wrong rows of Y = realY - reconstructedY" << "\n";
                std::vector<double> ysub(nrows);
                for(int i = 0; i < nrows; i++){
                    ysub[i] = y[i] - yfinal[i];
                    if(ysub[i] != 0){
                        std::cout << i << " : " << ysub[i] << "       /// = " << y[i] << " - " << yfinal[i] << "\n";

                    }
                }
            }

            double norm = PPTP::norm2(yfinal);
            std::cout << "SpMV MPI ||y||=" << norm << std::endl;
    }
    else{
        // std::cout << my_rank <<" MY SIZE LOCAL Y IS " << s << "\n";
        MPI_Gatherv(
            local_y.data(),
            s,
            MPI_DOUBLE,
            NULL,
            NULL,
            NULL,
            MPI_DOUBLE,
            0,
            MPI_COMM_WORLD);
    }

    timer.printInfo();

    MPI_Finalize();


    return 0;
}
