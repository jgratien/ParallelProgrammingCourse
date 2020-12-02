/*
 *  img_kmean_mpi.cpp
 *
 *  Created on: Nov 30, 2020
 *      Author: ramyk
 */

#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>

#include <mpi.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/variables_map.hpp>

#include "Utils/Timer.h"

std::vector<uchar> init_centroids(const cv::Mat& image, int nb_centroids)
{
  srand(time(0));
  int row, col, chnls = image.channels();
  std::vector<uchar> centroids(chnls * nb_centroids);
  for (int i = 0; i < nb_centroids; i++) {
    row = rand() % image.rows;
    col = rand() % image.cols;
    if (chnls == 1)
      centroids[i][0] = image.at<uchar>(row, col);
    else
      for (int j = 0; j < chnls; j++)
        centroids[i][j] = image.at<cv::Vec3b>(row, col)[j];
  }
  return centroids;
}

std::vector<uchar> compute_centroids(int nb_centroids, int nb_channels,
                                     std::vector<double>& clst_colorsum,
                                     std::vector<double>& clst_count)
{
  std::vector<uchar> centroids(nb_channels * nb_centroids);
  for (int i = 0; i < nb_centroids * nb_channels; i += nb_channels) {
    for (int j = 0; j < nb_channels; j++) {
      centroids[i + j] =
        (int)(clst_colorsum[i + j] / clst_count[i]);
      clst_colorsum[i + j] = 0;
    }
    clst_count[i] = 0;
  }
  return centroids;
}

int main( int argc, char** argv )
{
    using namespace boost::program_options;
    options_description desc;
    desc.add_options()
      ("help", "produce help")
      ("file",value<std::string>()->default_value(std::string("")), "image file")
      ("k",value<int>()->default_value(0), "KMean k value: number of centroids")
      ("max-iter", value<int>()->default_value(10), "Kmean maximum iterations number");
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << "\n";
        return 1;
    }

    MPI_Init(&argc,&argv) ;
    MPI_Status status;

    int rank = 0;
    int nb_proc = 1;
    bool converged = false;
    MPI_Comm_size(MPI_COMM_WORLD,&nb_proc) ;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank) ;

    if (rank == 0) {
      std::string img_file = vm["file"].as<std::string>() ;
      cv::Mat image;
      image = cv::imread(img_file.c_str(), CV_LOAD_IMAGE_COLOR);
      if(!image.data) {
          std::cout <<  "Could not open or find the image" << std::endl;
          MPI_Finalize();
          return -1;
      }

      const int nb_channels = image.channels();
      const int nb_centroids = vm["k"].as<int>();
      const int nb_rows = image.rows;
      const int nb_cols = image.cols;
      const int maxiter = vm["max-iter"].as<int>();
      std::cout << "NB CHANNELS : " << nb_channels << std::endl;
      std::cout << "NROWS       : " << nb_rows << std::endl;
      std::cout << "NCOLS       : " << nb_cols << std::endl;

      // Starting timer count
      PPTP::Timer timer;
      PPTP::Timer::Sentry sentry(timer, "Kmeans_MPI");

      cv::Mat flat = image.reshape(1, image.total() * nb_channels);
      std::vector<uchar> img_mat = image.isContinuous() ? flat : flat.clone();

      // Broadcasting sizes
      int buff[2] = { nb_channels, nb_centroids };
      MPI_Bcast(buff, 4, MPI_INT, 0, MPI_COMM_WORLD);

      // Splitting & sending image matrix
      int r = (nb_rows * nb_cols) % nb_proc;
      long proc_blocksize = 0;
      long start = nb_rows * nb_cols / nb_proc + ((r > 0) ? 1 : 0);
      for (int i = 1; i < nb_proc; i++) {
        proc_blocksize = nb_rows * nb_cols / nb_proc + ((r > 0) ? 1 : 0);
        MPI_Send(&proc_blocksize, 1, MPI_LONG, i, 1000, MPI_COMM_WORLD);
        MPI_Send(img_mat.data() + start, proc_blocksize * nb_channels,
                 MPI_UNSIGNED_CHAR, i, 2000, MPI_COMM_WORLD);
        start += proc_blocksize * nb_channels;
      }

      // Process initialization
      if (r > 0) proc_blocksize++;
      std::vector<uint8_t> clustered_block(proc_blocksize);
      std::vector<double> clst_colorsum(nb_channels * nb_centroids);
      std::vector<double> clst_count(nb_centroids);
      std::vector<uchar> centroids = init_centroids(image, nb_centroids);
      int iter = 0;
      converged = false;
      while (!converged) {
        // Sending new centroids
        MPI_Bcast(centroids.data(), nb_channels * nb_centroids,
                  MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

        // - Process own data
        // - Receive processes clst_colorsum & clst_count
        compute_centroids(nb_centroids, nb_channels, clst_colorsum, clst_count);

        // - Calculate convergence (by old centroids being same as new ones)
        converged = converged || ((++iter) >= maxiter);
        // - Send convergence (so that processes dont stop segmenting
        //   and start by asking for new centroids to be sent)
        //   or just directly send img_mat data
      }

      // Painting image according to clustering result
      for (int i = 0; i < proc_blocksize * nb_channels; i+=nb_channels)
        for (int j = 0; j < nb_channels; j++)
          img_mat[i + j] = centroids[clustered_block[i]][j];

      // Receive partial matrix parts
      start = proc_blocksize * nb_channels;
      for (int i = 1; i < nb_proc; i++) {
        proc_blocksize = nb_rows * nb_cols / nb_proc + ((r > 0) ? 1 : 0);
        MPI_Recv(img_mat.data() + start, proc_blocksize * nb_channels,
                 MPI_UNSIGNED_CHAR, i, 3000, MPI_COMM_WORLD, &status);
        start += proc_blocksize * nb_channels;
      }

      // Reconstruct image
      if (nb_channels == 1)
        cv::Mat result(nb_rows, nb_cols, CV_8UC1, img_mat.data());
      else
        cv::Mat result(nb_rows, nb_cols, CV_8UC3, img_mat.data());

      // Stopping timer
      timer.printInfo();
      std::cout << "Writing image output" << std::endl;
      cv::imwrite("./Seg_Image.jpg", result);
    } else {
      // Receiving sizes
      int buff[2];
      MPI_Bcast(buff, 2, MPI_INT, 0, MPI_COMM_WORLD);
      int nb_channels = buff[0]
        , nb_centroids = buff[1];

      // Receiving image matrix parts
      long proc_blocksize = 0;
      MPI_Recv(&proc_blocksize, 1, MPI_LONG, 0, 1000, MPI_COMM_WORLD, &status);
      std::vector<uchar> block(proc_blocksize * nb_channels);
      std::vector<uint8_t> clustered_block(proc_blocksize);
      MPI_Recv(block.data(), proc_blocksize * nb_channels, MPI_LONG, 0, 2000, MPI_COMM_WORLD, &status);

      // Receiving updated centroids
      std::vector<uchar> centroids(nb_channels * nb_centroids);
      MPI_Bcast(centroids.data(), nb_channels * nb_centroids,
                MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

      // Painting image according to clustering result
      for (int i = 0; i < proc_blocksize * nb_channels; i+=nb_channels)
        for (int j = 0; j < nb_channels; j++)
          block[i + j] = centroids[clustered_block[i]][j];

      // Sending result image parts
      MPI_Send(block.data(), proc_blocksize * nb_channels,
               MPI_UNSIGNED_CHAR, 0, 3000, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0 ;
}
