/* #include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/variables_map.hpp>
#include <mpi.h>
#include <string>
#include <vector>
#include "MatrixVector/CSRMatrix.h"
#include "MatrixVector/LinearAlgebra.h"
#include "MatrixVector/MatrixGenerator.h"
#include "Utils/Timer.h"

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    using namespace boost::program_options;
    using namespace PPTP;

    options_description desc;
    desc.add_options()
        ("help", "produce help")
        ("nx", value<int>()->default_value(50), "nx grid size")
        ("file", value<std::string>(), "file input");

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    if (vm.count("help")) {
        if (rank == 0) {
            std::cout << desc << "\n";
        }
        MPI_Finalize();
        return 1;
    }

    Timer timer;
    MatrixGenerator generator;

    // Generate or read the matrix
    int nx = vm["nx"].as<int>();
    CSRMatrix matrix;
    if (vm.count("file")) {
        std::string file = vm["file"].as<std::string>();
        if (rank == 0) {
            generator.readFromFile(file, matrix);
        }
        MPI_Bcast(&matrix, sizeof(matrix), MPI_BYTE, 0, MPI_COMM_WORLD);
    } else {
        generator.genLaplacian(nx, matrix);
    }

    // Number of rows and global vector x
    std::size_t nrows = matrix.nrows();
    std::vector<double> x(nrows, 1.0); // Initialize vector x with 1.0
    std::vector<double> y(nrows, 0.0); // Result vector

    // Calculate row distribution
    std::size_t rows_per_proc = nrows / size;
    std::size_t start_row = rank * rows_per_proc;
    std::size_t end_row = (rank == size - 1) ? nrows : start_row + rows_per_proc;

    // Local vectors for computation
    std::size_t local_nrows = end_row - start_row;
    std::vector<double> local_y(local_nrows, 0.0);

    // Distribute x (broadcast, as all rows need access to full x)
    MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    {
        Timer::Sentry sentry(timer, "MPI_SpMV");

        // Local SpMV computation
        for (std::size_t irow = start_row; irow < end_row; ++irow) {
            double value = 0;
            for (int k = matrix.m_kcol[irow]; k < matrix.m_kcol[irow + 1]; ++k) {
                value += matrix.m_values[k] * x[matrix.m_cols[k]];
            }
            local_y[irow - start_row] = value;
        }

        // Gather results from all processes
        MPI_Gather(local_y.data(), local_nrows, MPI_DOUBLE, y.data(), local_nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }

    // Calculate and display the norm (only by rank 0)
    if (rank == 0) {
        double normy = PPTP::norm2(y);
        std::cout << "||y||=" << normy << std::endl;
    }

    timer.printInfo(rank);
*/
/*
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/variables_map.hpp>
#include <mpi.h>
#include <vector>
#include "MatrixVector/LinearAlgebra.h"
#include "MatrixVector/CSRMatrix.h"
#include "MatrixVector/MatrixGenerator.h"
#include "Utils/Timer.h"

int main(int argc, char** argv)
{
  using namespace boost::program_options;
  options_description desc;
  desc.add_options()
      ("help", "produce help")
      ("nrows", value<int>()->default_value(0), "matrix size")
      ("nx", value<int>()->default_value(0), "nx grid size")
      ("file", value<std::string>(), "file input")
      ("eigen", value<int>()->default_value(0), "use eigen package");
  
  variables_map vm;
  store(parse_command_line(argc, argv, desc), vm);
  notify(vm);

  if (vm.count("help"))
  {
      std::cout << desc << "\n";
      return 1;
  }

  MPI_Init(&argc, &argv);
  int my_rank = 0;
  int nb_proc = 1;
  MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  using namespace PPTP;

  Timer timer;
  MatrixGenerator generator;
  
  // Charger la matrice
  CSRMatrix matrix;
  
  if(vm.count("file"))
  {
    std::string file = vm["file"].as<std::string>();
    generator.readFromFile(file, matrix);
  }
  else
  {
    int nx = vm["nx"].as<int>();
    generator.genLaplacian(nx, matrix);
  }

  std::size_t nrows = matrix.nrows();
  std::vector<double> x(nrows), y(nrows, 0.0), y2(nrows, 0.0);

  for (std::size_t i = 0; i < nrows; ++i)
    x[i] = i + 1;

  // Répartition des lignes entre les processus
  int rows_per_process = nrows / nb_proc;
  int remainder = nrows % nb_proc;
  int start_row = my_rank * rows_per_process + std::min(my_rank, remainder);
  int end_row = start_row + rows_per_process + (my_rank < remainder ? 1 : 0);

  // Allocation du vecteur local
  std::vector<double> local_y(end_row - start_row, 0.0);

  // Multipliez localement
  {
    Timer::Sentry sentry(timer, "SpMV");
    matrix.mult(x, local_y);
  }

  // Réduire les résultats (somme des parties locales)
  MPI_Gather(local_y.data(), local_y.size(), MPI_DOUBLE, y.data(), rows_per_process, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  // Affichage du résultat sur le processus 0
  if (my_rank == 0)
  {
    double normy = PPTP::norm2(y);
    std::cout << "||y||=" << normy << std::endl;
  }

  // Pour la version OpenMP pour comparaison
  {
    Timer::Sentry sentry(timer, "OMPSpMV");
    matrix.mult(x, y2);
  }
  
  if (my_rank == 0)
  {
    double normy2 = PPTP::norm2(y2);
    std::cout << "||y2||=" << normy2 << std::endl;
  }

  timer.printInfo();
  MPI_Finalize();
  return 0;
}*/

/*
#include <iostream>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/variables_map.hpp>
#include <mpi.h>
#include <vector>
#include <cmath> // Pour std::sqrt

#include "MatrixVector/CSRMatrix.h"
#include "MatrixVector/LinearAlgebra.h"
#include "MatrixVector/MatrixGenerator.h"
#include "Utils/Timer.h"

int main(int argc, char** argv)
{
  using namespace boost::program_options;
  options_description desc;
  desc.add_options()
      ("help", "produce help")
      ("nx", value<int>()->default_value(0), "nx grid size")
      ("file", value<std::string>(), "file input");

  variables_map vm;
  store(parse_command_line(argc, argv, desc), vm);
  notify(vm);

  if (vm.count("help")) {
      std::cout << desc << "\n";
      return 1;
  }

  MPI_Init(&argc, &argv);

  int my_rank = 0;
  int nb_proc = 1;
  MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  using namespace PPTP;

  Timer timer;
  MatrixGenerator generator;

  CSRMatrix matrix;

  // Charger la matrice uniquement dans le processus maître
  if (my_rank == 0) {
    if (vm.count("file")) {
      std::string file = vm["file"].as<std::string>();
      generator.readFromFile(file, matrix);
    } else {
      int nx = vm["nx"].as<int>();
      generator.genLaplacian(nx, matrix);
    }
  }

  // Broadcast du nombre de lignes de la matrice à tous les processus
  std::size_t nrows = 0;
  if (my_rank == 0) nrows = matrix.nrows();
  MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

  // Diffuser la matrice entière à tous les processus
  // Pour cela, nous devons sérialiser/désérialiser la matrice.
  if (nb_proc > 1) {
    if (my_rank == 0) {
      // Le processus maître diffuse la matrice
      MPI_Bcast(matrix.data(), matrix.nnz(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
      // (Cette partie dépend de votre mécanisme de sérialisation, si nécessaire).
    } else {
      // Les autres processus reçoivent la matrice
      // (Reconstruction après sérialisation ici).
      MPI_Bcast(matrix.data(), matrix.nnz(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }
  }

  // Diviser les lignes entre les processus
  int rows_per_process = nrows / nb_proc;
  int remainder = nrows % nb_proc;
  int start_row = my_rank * rows_per_process + std::min(my_rank, remainder);
  int end_row = start_row + rows_per_process + (my_rank < remainder ? 1 : 0);
  int local_nrows = end_row - start_row;

  // Initialiser les vecteurs locaux
  std::vector<double> x(nrows, 0.0);
  std::vector<double> local_y(local_nrows, 0.0);

  // Initialiser x avec des valeurs globales dans le processus maître, puis le diffuser
  if (my_rank == 0) {
    for (std::size_t i = 0; i < nrows; ++i)
      x[i] = i + 1;
  }
  MPI_Bcast(x.data(), nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  // Extraire les indices des lignes locales
  std::vector<double> local_x(local_nrows);
  for (int i = 0; i < local_nrows; ++i) {
    local_x[i] = x[start_row + i];
  }

  // Calcul local du produit matrice-vecteur sur les lignes locales
  for (int i = 0; i < local_nrows; ++i) {
    local_y[i] = 0.0; // Initialisation
  }

  matrix.mult(x, local_y); // Appel à la méthode encapsulée

  // Rassembler les résultats locaux
  std::vector<double> y(nrows, 0.0);
  std::vector<int> recv_counts(nb_proc);
  std::vector<int> displs(nb_proc);

  for (int rank = 0; rank < nb_proc; ++rank) {
    int rank_start_row = rank * rows_per_process + std::min(rank, remainder);
    int rank_end_row = rank_start_row + rows_per_process + (rank < remainder ? 1 : 0);
    recv_counts[rank] = rank_end_row - rank_start_row;
    displs[rank] = rank_start_row;
  }

  MPI_Gatherv(local_y.data(), local_nrows, MPI_DOUBLE, y.data(), recv_counts.data(), displs.data(), MPI_DOUBLE, 0, MPI_COMM_WORLD);

  // Calcul de la norme dans le processus maître
  if (my_rank == 0) {
    double normy = PPTP::norm2(y);
    std::cout << "||y|| = " << normy << std::endl;
  }

  MPI_Finalize();
  return 0;
}*/
/*
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/variables_map.hpp>
#include <mpi.h>
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
    MPI_Init(&argc, &argv);  // Initialisation de MPI

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    using namespace boost::program_options;
    options_description desc;
    desc.add_options()
        ("help", "produce help message")
        ("nrows", value<int>()->default_value(0), "matrix size")
        ("nx", value<int>()->default_value(0), "nx grid size")
        ("file", value<std::string>(), "file input")
        ("eigen", value<int>()->default_value(0), "use Eigen package");

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        MPI_Finalize();
        return 1;
    }

    using namespace PPTP;

    Timer timer;
    MatrixGenerator generator;

    // Début de la génération de la matrice
    if (vm["eigen"].as<int>() == 1) {
        // Utilisation d'Eigen pour la multiplication matrice-vecteur
        typedef Eigen::SparseMatrix<double> MatrixType;
        typedef Eigen::VectorXd VectorType;
        MatrixType matrix;

        if (vm.count("file")) {
            std::string file = vm["file"].as<std::string>();
            generator.readFromFile(file, matrix);
        } else {
            int nx = vm["nx"].as<int>();
            generator.genLaplacian(nx, matrix);
        }

        std::size_t nrows = matrix.rows();
        VectorType x(nrows);
        for (std::size_t i = 0; i < nrows; ++i)
            x(i) = i + 1;

        VectorType y;
        {
            Timer::Sentry sentry(timer, "EigenSpMV");
            y = matrix * x;
        }

        // Affichage de la norme uniquement sur le processus principal
        if (rank == 0) {
            double normy = PPTP::norm2(y);
            std::cout << "||y|| (Eigen) = " << normy << std::endl;
        }

    } else {
        // Utilisation de CSRMatrix pour la multiplication matrice-vecteur
        CSRMatrix matrix;

        if (vm.count("file")) {
            std::string file = vm["file"].as<std::string>();
            generator.readFromFile(file, matrix);
        } else {
            int nx = vm["nx"].as<int>();
            generator.genLaplacian(nx, matrix);
        }

        std::size_t nrows = matrix.nrows();
        std::vector<double> x(nrows), y(nrows), y2(nrows);
	std::vector<double> local_x, local_y;
        std::vector<int> local_kcol, local_cols;
        std::vector<double> local_values;

        for (std::size_t i = 0; i < nrows; ++i)
            x[i] = i + 1;

        // Multiplication matrice-vecteur sur un seul processus
        {
            Timer::Sentry sentry(timer, "SpMV");
            matrix.mult(x, y);
        }

        // Affichage de la norme uniquement sur le processus principal
        if (rank == 0) {
            double normy = PPTP::norm2(y);
            std::cout << "||y|| (CSR) = " << normy << std::endl;
        }

        // Multiplication matrice-vecteur avec MPI
        {
            Timer::Sentry sentry(timer, "MPISpMV");

            // Répartition des lignes entre les processus MPI
            std::size_t local_nrows = nrows / size;
            std::size_t start_row = rank * local_nrows;
            std::size_t end_row = (rank == size - 1) ? nrows : (rank + 1) * local_nrows;

            // Vecteur local pour les résultats de y
            std::vector<double> local_y(local_nrows, 0.0);

            // Accéder aux variables privées m_kcol, m_cols, m_values via des getters
            for (std::size_t irow = start_row; irow < end_row; ++irow) {
                double value = 0.0;
                for (int k = matrix.getKcol()[irow]; k < matrix.getKcol()[irow + 1]; ++k) {
                    value += matrix.getValues()[k] * x[matrix.getCols()[k]];
                }
                local_y[irow - start_row] = value;
            }

            // Collecter les résultats avec MPI_Gather
            MPI_Reduce(local_y.data(), y.data(), nrows, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

            // Calculer la norme sur le processus 0
            if (rank == 0) {
                double normy = PPTP::norm2(y);
                std::cout << "||y|| (MPISpMV) = " << normy << std::endl;
            }
        }
    }


    // Diffusion du nombre total de lignes
    MPI_Bcast(&nrows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

    // Partition des lignes de la matrice entre les processus
    std::size_t local_nrows = nrows / size;
    std::size_t remainder = nrows % size;
    
    std::size_t start_row = rank * local_nrows + std::min(static_cast<std::size_t>(rank), remainder);
    std::size_t end_row = start_row + local_nrows + (rank < remainder ? 1 : 0);

    local_nrows = end_row - start_row;

    // Distribution de la matrice CSR en parties locales
    if (rank == 0) {
        const auto& full_kcol = matrix.getKcol();
        const auto& full_cols = matrix.getCols();
        const auto& full_values = matrix.getValues();

        for (int ranki = 0; ranki < size; ++ranki) {
            std::size_t rank_start = ranki * (nrows / size) + std::min(static_cast<std::size_t>(ranki), remainder);
            std::size_t rank_end = rank_start + (nrows / size) + (ranki < remainder ? 1 : 0);

            if (ranki == rank) {
                local_kcol.assign(full_kcol.begin() + rank_start, full_kcol.begin() + rank_end + 1);

                for (std::size_t i = rank_start; i < rank_end; ++i) {
                    for (int j = full_kcol[i]; j < full_kcol[i + 1]; ++j) {
                        local_cols.push_back(full_cols[j]);
                        local_values.push_back(full_values[j]);
                    }
                }
            }
        }
    }

    // Diffusion des parties locales de la matrice vers tous les processus
    std::size_t local_kcol_size = local_kcol.size();
    std::size_t local_cols_size = local_cols.size();
    MPI_Bcast(&local_kcol_size, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(&local_cols_size, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

    if (rank != 0) {
        local_kcol.resize(local_kcol_size);
        local_cols.resize(local_cols_size);
        local_values.resize(local_cols_size);
    }

    MPI_Bcast(local_kcol.data(), local_kcol_size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(local_cols.data(), local_cols_size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(local_values.data(), local_cols_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Distribution du vecteur global_x aux processus
    local_x.resize(local_nrows);
    local_y.resize(local_nrows, 0.0);

    MPI_Scatterv(x.data(), nullptr, nullptr, MPI_DOUBLE, 
                 local_x.data(), local_nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Produit local matrice-vecteur
    for (std::size_t irow = 0; irow < local_nrows; ++irow) {
        double value = 0;
        for (int k = local_kcol[irow]; k < local_kcol[irow + 1]; ++k) {
            value += local_values[k] * x[local_cols[k]];
        }
        local_y[irow] = value;
    }

    // Collecte des résultats locaux dans le vecteur global_y
    MPI_Gatherv(local_y.data(), local_nrows, MPI_DOUBLE, 
                y.data(), nullptr, nullptr, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Affichage du résultat par le processus 0
    if (rank == 0) {
        double normy = PPTP::norm2(y);
        std::cout << "||y||=" << normy << std::endl;
    }
}

    // Affichage du temps de calcul global
    if (rank == 0) {
        timer.printInfo();
    }

    MPI_Finalize();
    return 0;
}}*/

#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/variables_map.hpp>
#include "omp.h"
#include "tbb/tbb.h"

#include <string>
#include <vector>
#include <fstream>

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/LU>

#include "MatrixVector/CSRMatrix.h"
#include "MatrixVector/LinearAlgebra.h"
#include "MatrixVector/MatrixGenerator.h"
#include <mpi.h>

#include "Utils/Timer.h"

int main(int argc, char** argv)
{
  using namespace boost::program_options;
  options_description desc;
  desc.add_options()
      ("help", "produce help")
      ("nrows", value<int>()->default_value(0), "matrix size")
      ("nx", value<int>()->default_value(0), "nx grid size")
      ("file", value<std::string>(), "file input")
      ("eigen", value<int>()->default_value(0), "use eigen package");
  variables_map vm;
  store(parse_command_line(argc, argv, desc), vm);
  notify(vm);

  if (vm.count("help"))
  {
      std::cout << desc << "\n";
      return 1;
  }

  MPI_Init(&argc, &argv);
  int my_rank = 0;
  int nb_proc = 1;
  MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  using namespace PPTP;

  Timer timer;
  MatrixGenerator generator;

  CSRMatrix matrix;
  if (vm.count("file"))
  {
    std::string file = vm["file"].as<std::string>();
    generator.readFromFile(file, matrix);
  }
  else
  {
    int nx = vm["nx"].as<int>();
    generator.genLaplacian(nx, matrix);
  }

  std::size_t nrows = matrix.nrows();
  std::vector<double> x(nrows), y(nrows), y2(nrows);
  std::vector<double> local_x, local_y;
  std::vector<int> sendcounts(nb_proc), displs(nb_proc);

  for (std::size_t i = 0; i < nrows; ++i)
    x[i] = i + 1;

  // Initialisation des sendcounts et displs
  std::size_t local_nrows = nrows / nb_proc;
  std::size_t remainder = nrows % nb_proc;
  for (int i = 0; i < nb_proc; ++i)
  {
    sendcounts[i] = local_nrows + (i < remainder ? 1 : 0);
    displs[i] = (i > 0) ? displs[i - 1] + sendcounts[i - 1] : 0;
  }

  local_x.resize(sendcounts[my_rank]);
  local_y.resize(sendcounts[my_rank]);

  // Messages de débogage
  std::cout << "Rank " << my_rank << " - sendcounts: " << sendcounts[my_rank] << ", displs: " << displs[my_rank] << std::endl;

  // Distribution du vecteur x
  MPI_Scatterv(x.data(), sendcounts.data(), displs.data(), MPI_DOUBLE, local_x.data(), sendcounts[my_rank], MPI_DOUBLE, 0, MPI_COMM_WORLD);

  // Produit matrice-vecteur local
  {
    Timer::Sentry sentry(timer, "MPISpMV");
    matrix.mult(local_x, local_y);
  }

  // Rassemblement des résultats
  MPI_Gatherv(local_y.data(), sendcounts[my_rank], MPI_DOUBLE, y2.data(), sendcounts.data(), displs.data(), MPI_DOUBLE, 0, MPI_COMM_WORLD);

  if (my_rank == 0)
  {
    double normy2 = PPTP::norm2(y2);
    std::cout << "||y2||=" << normy2 << std::endl;
  }

  timer.printInfo();
  MPI_Finalize();
  return 0;
}


