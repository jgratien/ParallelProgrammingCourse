/*
 * main.cpp
 *
 *  Created on: Oct 8, 2019
 *      Author: gratienj
 */

#include <mpi.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <random>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/variables_map.hpp>
#include "opencv2/opencv.hpp"

#include <time.h>
#include <string>

#include <IMGProcessing/KMeanAlgoMpi.h>
#include <Utils/TimerNow.h>

int main(int argc, char **argv)
{
  using namespace boost::program_options;
  using namespace cv;
  using namespace std;

  MPI_Init(&argc, &argv);
  int myRank, nbProc;

  MPI_Comm_size(MPI_COMM_WORLD, &nbProc);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  //General variables
  std::size_t nb_channels, nb_centroids;
  std::vector<u_char> centroids;
  std::vector<float> newCentroids;
  std::vector<long> cluster_sizes;
  std::vector<float> newCentroidsGlobal;
  std::vector<long> cluster_sizes_global;
  std::vector<uint16_t> mapping;
  bool stop;
  std::size_t cols;
  std::size_t slaveLocalNrows;

  //MPI related variables
  MPI_Request req0, req1, req2;
  MPI_Status status0;

  //Root processor
  if (!myRank)
  {
    options_description desc;
    desc.add_options()("help", "produce help")("file", value<std::string>(), "image file")("show", value<int>()->default_value(0), "show image")("seg", value<int>()->default_value(0), "kmean segmentation")("kmean-value", value<int>()->default_value(0), "KMean k value")("rgb", value<int>()->default_value(1), "RGB or GrayScale");
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    if (vm.count("help"))
    {
      std::cout << desc << "\n";
      return 1;
    }

    std::string img_file = vm["file"].as<std::string>();
    Mat image;

    if (vm["rgb"].as<int>() == 1)
    {
      image = imread(img_file.c_str(), CV_LOAD_IMAGE_COLOR); // Read the file in RGB mode
    }
    else
    {
      image = imread(img_file.c_str(), CV_LOAD_IMAGE_GRAYSCALE); // Read the file in GS mode
    }

    if (!image.data) // Check for invalid input
    {
      cout << "Could not open or find the image" << std::endl;
      return -1;
    }

    if (vm["show"].as<int>() == 1)
    {
      namedWindow("Display window", WINDOW_AUTOSIZE); // Create a window for display.
      imshow("Display window", image);                // Show our image inside it.
      waitKey(0);                                     // Wait for a keystroke in the window
      return 0;
    }

    cout << "image found good" << endl;

    cout << "NB CHANNELS : " << image.channels() << std::endl;
    cout << "NROWS       : " << image.rows << std::endl;
    cout << "NCOLS       : " << image.cols << std::endl;

    //Image info
    std::size_t rows;
    rows = image.rows;
    cols = image.cols;
    nb_channels = image.channels();

    double start, end;

    //Kmeans Info
    int nb_centroids = vm["kmean-value"].as<int>();

    if (vm["seg"].as<int>() == 1)
    {
      if (nb_channels == 3)
      {
        cout << "========= RGB Kmeans MPI =========" << endl;
      }
      else
      {
        cout << "========= GrayScale Kmeans MPI =========" << endl;
      }
    }

    int max_iterations = 100;
    int n_iter = 0;
    double epsilon = 1;

    start = now();

    cluster_sizes.resize(nb_centroids);
    cluster_sizes_global.resize(nb_centroids);

    //Data distribution / process info
    std::size_t localNrows;
    std::size_t rest = rows % nbProc;
    slaveLocalNrows = rows / nbProc;
    localNrows = slaveLocalNrows + rest;

    //Setting & Bcasting data distr buffer
    std::size_t base_buff[4] = {std::size_t(nb_centroids), nb_channels, cols, slaveLocalNrows};
    if (nbProc - 1)
    {
      MPI_Ibcast(&base_buff, 4, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD, &req0);
    }

    //Getting centroids ready
    centroids.reserve(nb_channels * nb_centroids);
    newCentroids.resize(nb_channels * nb_centroids);
    newCentroidsGlobal.reserve(nb_channels * nb_centroids);
    std::fill(newCentroidsGlobal.begin(), newCentroidsGlobal.end(), 0.0);

    //Getting flattened image ready for chunck Async sending
    std::vector<uchar> flatImage(rows * cols * nb_channels);
    if (image.isContinuous())
    {
      flatImage.assign(image.datastart, image.dataend);
    }

    for (int proc = 1; proc < nbProc; proc++)
    {
      MPI_Isend(flatImage.data() + localNrows * cols * nb_channels + (proc - 1) * slaveLocalNrows * cols * nb_channels,
                slaveLocalNrows * cols * nb_channels, MPI_UNSIGNED_CHAR, proc, proc * 2, MPI_COMM_WORLD, &req2);
    }

    //Initializing centroids & Async Broadcastin them
    init_centroids(image, nb_channels, nb_centroids, centroids);

    if (nbProc - 1)
    {
      MPI_Ibcast(centroids.data(), nb_channels * nb_centroids, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD, &req1);
    }

    //Preparing Kmeans loop variables
    int total_length = localNrows * cols;
    double max_displ;
    std::vector<double> displacement(nb_centroids, 0);
    double temp = 0;

    //Preparing kmeans output variables : mapping, cluster sizes, new centroids
    mapping.resize(localNrows * cols);

    //Kmeans Update Centroids Loop
    do
    {
      //Compute local kmeans
      std::fill(newCentroids.begin(), newCentroids.end(), 0.0);
      std::fill(cluster_sizes.begin(), cluster_sizes.end(), 0);
      cout << "\n------------ITERATION : " << n_iter + 1 << "-------------" << endl;

      update_centroid(flatImage, total_length, nb_centroids, nb_channels, centroids, cluster_sizes, newCentroids, mapping);

      if ((nbProc - 1) & !n_iter)
      {
        MPI_Wait(&req1, MPI_STATUS_IGNORE); //Making sure centroids were sent ofc
        MPI_Wait(&req2, MPI_STATUS_IGNORE); //Making sure data blocks were sent ofc
      }

      MPI_Reduce(newCentroids.data(), newCentroidsGlobal.data(), nb_channels * nb_centroids, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
      MPI_Reduce(cluster_sizes.data(), cluster_sizes_global.data(), nb_centroids, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

      for (int i = 0; i < nb_centroids; i++)
      {
        temp = 0;
        for (size_t c = 0; c < nb_channels; c++)
        {
          newCentroidsGlobal[i * nb_channels + c] = (float)newCentroidsGlobal[i * nb_channels + c] / (cluster_sizes_global[i] == 0 ? 1.0 : (float)cluster_sizes_global[i]);
          temp += (newCentroidsGlobal[i * nb_channels + c] - (float)centroids[i * nb_channels + c]) * (newCentroidsGlobal[i * nb_channels + c] - (float)centroids[i * nb_channels + c]);
        }
        displacement[i] = sqrt(temp / nb_channels);
      }

      max_displ = 0;
      for (size_t i = 0; i < nb_centroids; i++)
      {
        if (displacement[i] > max_displ)
        {
          max_displ = displacement[i];
        }
      }

      for (int i = 0; i < nb_centroids * nb_channels; i++)
      {
        centroids[i] = (u_char)newCentroidsGlobal[i];
      }

      //Stop conditions
      n_iter += 1;
      stop = (max_displ <= epsilon) | (n_iter == (max_iterations - 1));

      //Notifying the other slave processors
      MPI_Bcast(&stop, 1, MPI_C_BOOL, 0, MPI_COMM_WORLD);
      if (stop)
      {
        std::cout << "stopped after iteration : " << n_iter << std::endl;
      }
      else
      {
        //Bcast newly computed centroids
        MPI_Bcast(centroids.data(), nb_channels * nb_centroids, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
      }
    } while (!stop);

    gen_segmentation(flatImage, nb_channels, total_length, centroids, mapping);

    for (int proc = 1; proc < nbProc; proc++)
    {

      MPI_Recv(flatImage.data() + total_length * nb_channels + (proc - 1) * cols * slaveLocalNrows * nb_channels,
               cols * slaveLocalNrows * nb_channels,
               MPI_UNSIGNED_CHAR,
               proc, proc * 9 + 1,
               MPI_COMM_WORLD, &status0);
    }

    Mat *outPutImage = new Mat(rows, cols, (nb_channels == 1) ? CV_8UC1 : CV_8UC3, flatImage.data());

    end = now();

    std::cout << "Time : " << end - start << std::endl;
    std::cout << "Average Time per cycle : " << (end - start) / n_iter << std::endl;

    img_file.resize(img_file.size() - 4);
    string fileName = img_file + "_MPI_Segmented.jpg";

    imwrite(fileName, *outPutImage);

    delete outPutImage;
  }
  // Other slave processors
  else
  {
    std::vector<uchar> *localFlatImage = new std::vector<uchar>();
    std::size_t base_buff[4];

    //Getting basic info
    MPI_Ibcast(&base_buff, 4, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD, &req0);
    MPI_Wait(&req0, MPI_STATUS_IGNORE);

    nb_centroids = base_buff[0];
    nb_channels = base_buff[1];
    cols = base_buff[2];
    slaveLocalNrows = base_buff[3];

    //Local & Global kmeans tracking variables
    centroids.resize(nb_channels * nb_centroids);
    cluster_sizes.resize(nb_centroids);
    cluster_sizes_global.resize(nb_centroids);
    newCentroids.resize(nb_centroids * nb_channels);
    newCentroidsGlobal.reserve(nb_channels * nb_centroids);
    std::fill(newCentroidsGlobal.begin(), newCentroidsGlobal.end(), 0);

    //Preparing local flat image
    localFlatImage->resize(nb_channels * slaveLocalNrows * cols);

    MPI_Ibcast(centroids.data(), nb_channels * nb_centroids, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD, &req1);

    MPI_Irecv(localFlatImage->data(),
              slaveLocalNrows * cols * nb_channels,
              MPI_LONG, 0, myRank * 2,
              MPI_COMM_WORLD, &req2);

    int total_length = slaveLocalNrows * cols;

    //Preparing kmeans output variables : mapping, cluster sizes, new centroids
    mapping.resize(slaveLocalNrows * cols);

    MPI_Wait(&req1, MPI_STATUS_IGNORE);
    MPI_Wait(&req2, MPI_STATUS_IGNORE);

    do
    {
      std::fill(newCentroids.begin(), newCentroids.end(), 0.0);
      std::fill(cluster_sizes.begin(), cluster_sizes.end(), 0);

      update_centroid(*localFlatImage, total_length, nb_centroids, nb_channels, centroids, cluster_sizes, newCentroids, mapping);

      MPI_Reduce(newCentroids.data(), newCentroidsGlobal.data(), nb_channels * nb_centroids, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
      MPI_Reduce(cluster_sizes.data(), cluster_sizes_global.data(), nb_centroids, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

      MPI_Bcast(&stop, 1, MPI_C_BOOL, 0, MPI_COMM_WORLD);

      //Get new centroids for next iteration (if there is one)
      if (!stop)
      {
        MPI_Bcast(centroids.data(), nb_channels * nb_centroids, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
      }

    } while (!stop);

    gen_segmentation(*localFlatImage, nb_channels, total_length, centroids, mapping);

    MPI_Send(localFlatImage->data(), total_length * nb_channels, MPI_UNSIGNED_CHAR, 0, myRank * 9 + 1, MPI_COMM_WORLD);

    delete localFlatImage;
  }

  MPI_Finalize();
  return 0;
}
