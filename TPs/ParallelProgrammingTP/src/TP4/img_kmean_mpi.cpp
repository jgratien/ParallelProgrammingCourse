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

std::vector<uchar> init_centroids(const std::vector<uchar>& image_mat,
                                  int nb_rows, int nb_cols,
                                  int nb_centroids, int nb_channels)
{
  srand(time(0));
  int row, col;
  std::vector<uchar> centroids(nb_channels * nb_centroids);
  for (int i = 0; i < (nb_centroids * nb_channels); i += nb_channels) {
    row = rand() % nb_rows;
    col = rand() % nb_cols;
    for (int j = 0; j < nb_channels; j++)
      centroids[i + j] = image_mat[(row * nb_cols + col) * nb_channels + j];
  }
  return centroids;
}

std::vector<uchar> compute_centroids(int nb_centroids, int nb_channels,
                                     std::vector<double>& clst_colorsum,
                                     std::vector<double>& clst_count)
{
  std::vector<uchar> centroids(nb_channels * nb_centroids);
  for (int i = 0; i < nb_centroids; i++)
    for (int j = 0; j < nb_channels; j++)
      centroids[i * nb_channels + j] =
        (int)(clst_colorsum[i * nb_channels + j] / clst_count[i]);
  return centroids;
}

int nrst_centroid(uchar* pixel, int nb_channels,
                  std::vector<uchar>& centroids, int nb_centroids)
{
  double dst, min_dst = 100000;
  int nrst_indx = 0;
  for (int ind = 0; ind < nb_centroids; ind++) {
    dst = 0;
    for (int ch = 0; ch < nb_channels; ch++)
      dst += pow((int) pixel[ch] - centroids[ind * nb_channels + ch], 2);
    dst = sqrt(dst);
    if (dst <= min_dst) {
      min_dst = dst;
      nrst_indx = ind;
    }
  }
  return nrst_indx;
}

void segment(std::vector<uchar>& block, std::vector<uchar>& centroids,
             std::vector<uint8_t>& clustered_block,
             long proc_blocksize, int nb_channels, int nb_centroids,
             std::vector<double>& clst_colorsum, std::vector<double>& clst_count)
{
  // reinit cluster reduction buffers
  std::fill(clst_colorsum.begin(), clst_colorsum.end(), 0);
  std::fill(clst_count.begin(), clst_count.end(), 0);

  uchar* pixel;
  int cent_ind;
  for (int i = 0; i < proc_blocksize; i++) {
    pixel = block.data() + i * nb_channels;
    cent_ind = nrst_centroid(pixel, nb_channels, centroids, nb_centroids);
    clustered_block[i] = cent_ind;
    clst_count[cent_ind] += 1;
    for (int j = 0; j < nb_channels; j++)
      clst_colorsum[cent_ind * nb_channels + j] += pixel[j];
  }
}

void paint_segmentation(std::vector<uchar>& img_mat, long img_size,
                        std::vector<uchar>& centroids, int nb_channels,
                        std::vector<uint8_t>& clustered_block)
{
  for (int i = 0; i < img_size; i++)
    for (int j = 0; j < nb_channels; j++)
      img_mat[i * nb_channels + j] = centroids[clustered_block[i] * nb_channels + j];
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

    PPTP::Timer timer;
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

      cv::Mat result;
      const int nb_channels = image.channels();
      const int nb_centroids = vm["k"].as<int>();
      const int nb_rows = image.rows;
      const int nb_cols = image.cols;
      const int maxiter = vm["max-iter"].as<int>();
      int iter = 0;
      std::cout << "NB CHANNELS : " << nb_channels << std::endl;
      std::cout << "NROWS       : " << nb_rows << std::endl;
      std::cout << "NCOLS       : " << nb_cols << std::endl;

      // Flattening image
      std::vector<uchar> img_mat(nb_rows * nb_cols * nb_channels);
      if (image.isContinuous())
        img_mat.assign(image.datastart, image.dataend);

      { // Timer scope
        // Starting timer count
        PPTP::Timer::Sentry sentry(timer, "Kmeans_MPI");

        // Broadcasting sizes
        int buff[2] = { nb_channels, nb_centroids };
        MPI_Bcast(buff, 2, MPI_INT, 0, MPI_COMM_WORLD);

        // Splitting & sending image matrix
        int r = (nb_rows * nb_cols) % nb_proc;
        long start = nb_channels * (nb_rows * nb_cols / nb_proc + ((r > 0) ? 1 : 0));
        long proc_blocksize = start / nb_channels;
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
        std::vector<uchar> centroids = init_centroids(img_mat, nb_rows, nb_cols,
                                                      nb_centroids, nb_channels);
        std::vector<uchar> new_centroids(nb_centroids * nb_channels);
        converged = false;
        double count_buff[nb_centroids];
        double color_buff[nb_centroids * nb_channels];
        while (!converged) {
          // Sending new centroids
          MPI_Bcast(centroids.data(), nb_channels * nb_centroids,
                    MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

          // Process own data
          segment(img_mat, centroids, clustered_block, proc_blocksize,
                  nb_channels, nb_centroids, clst_colorsum, clst_count);

          // Receiving clusters elements count reduction
          for (int i = 1; i < nb_proc; i++) {
            MPI_Recv(count_buff, nb_centroids, MPI_DOUBLE,
                    i, 2333, MPI_COMM_WORLD, &status);
            for (int j = 0; j < nb_centroids; j++)
              clst_count[j] += count_buff[j];
          }

          // Receiving clusters colors sum reduction
          for (int i = 1; i < nb_proc; i++) {
            MPI_Recv(color_buff, nb_centroids * nb_channels, MPI_DOUBLE,
                    i, 2666, MPI_COMM_WORLD, &status);
            for (int j = 0; j < nb_centroids * nb_channels; j++)
              clst_colorsum[j] += color_buff[j];
          }

          // Computing new centroids
          new_centroids = compute_centroids(nb_centroids, nb_channels,
                                            clst_colorsum, clst_count);

          // Convergence test
          converged = (new_centroids == centroids) || ((++iter) >= maxiter);
          MPI_Bcast(&converged, 1, MPI_C_BOOL, 0, MPI_COMM_WORLD);
          centroids.swap(new_centroids);
        }

        // Painting image according to clustering result
        paint_segmentation(img_mat, proc_blocksize,
                          centroids, nb_channels,
                          clustered_block);

        // Receive partial matrix parts
        start = proc_blocksize * nb_channels;
        for (int i = 1; i < nb_proc; i++) {
          proc_blocksize = nb_rows * nb_cols / nb_proc + ((r > 0) ? 1 : 0);
          MPI_Recv(img_mat.data() + start, proc_blocksize * nb_channels,
                  MPI_UNSIGNED_CHAR, i, 3000, MPI_COMM_WORLD, &status);
          start += proc_blocksize * nb_channels;
        }

        // Reconstruct image
        result = cv::Mat(nb_rows, nb_cols,
                         (nb_channels == 1) ? CV_8UC1 : CV_8UC3,
                         img_mat.data());
      }// Stopping timer

      std::cout << "Finished in " << iter << " iterations" << std::endl;
      std::cout << "Writing image output" << std::endl;
      cv::imwrite("./MPI-out.jpg", result);
      timer.printInfo();
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

      converged = false;
      std::vector<double> clst_colorsum(nb_channels * nb_centroids);
      std::vector<double> clst_count(nb_centroids);
      std::vector<uchar> centroids(nb_channels * nb_centroids);
      while (!converged) {
        // Receiving updated centroids
        MPI_Bcast(centroids.data(), nb_channels * nb_centroids,
                  MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

        // Process own data
        segment(block, centroids, clustered_block, proc_blocksize,
                nb_channels, nb_centroids, clst_colorsum, clst_count);

        // Sending reduced clusters data
        MPI_Send(clst_count.data(), nb_centroids, MPI_DOUBLE,
                 0, 2333, MPI_COMM_WORLD);
        MPI_Send(clst_colorsum.data(), nb_centroids * nb_channels,
                 MPI_DOUBLE, 0, 2666, MPI_COMM_WORLD);

        // Receiving convergence state
        MPI_Bcast(&converged, 1, MPI_C_BOOL, 0, MPI_COMM_WORLD);
      }

      // Painting image according to clustering result
      paint_segmentation(block, proc_blocksize,
                         centroids, nb_channels,
                         clustered_block);

      // Sending result image parts
      MPI_Send(block.data(), proc_blocksize * nb_channels,
               MPI_UNSIGNED_CHAR, 0, 3000, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0 ;
}
