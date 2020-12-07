/*
 *  img_kmean_mpi.cpp
 *
 *  Created on: Nov 30, 2020
 *      Author: Nezar Lheimeur
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

std::vector<uchar> init_centroids(const std::vector<uchar>& image_vect,
                                  int nrows, int ncols,
                                  int nb_centroids, int nb_channels)
{
  srand(time(0));
  
  int row_id, col_id;
  
  std::vector<uchar> centroids(nb_channels * nb_centroids);
  
  switch(nb_channels)
			{
				case 1:
				{
					
					row_id=(int)(rand()%nrows);
					col_id=(int)(rand()%ncols);
					centroids[0]=image_vect[(row_id*ncols + col_id)];
					for(int i=1;i<(int)centroids.size();i++)
					{
						do{
						row_id=(int)(rand()%nrows);
						col_id=(int)(rand()%ncols);
						centroids[i]=image_vect[(row_id*ncols + col_id)];
						}while(abs(centroids[i]-centroids[i-1])>5 && std::find(centroids.begin(), centroids.end(), centroids[i]) != centroids.end());
					}
					break;
				}
				case 3: 
				{
						
					row_id=(int)(rand()%nrows);
					col_id=(int)(rand()%ncols);
					
					double dist=0;
					
					for(int k=0;k<3;k++){
						centroids[k]=image_vect[(row_id * ncols + col_id) * nb_channels + k];
					}
					for(int c=1; c<nb_centroids; c++)
					{	
						do{
							row_id=(int)(rand()%nrows);
							col_id=(int)(rand()%ncols);
							dist=0;
							for(int i=3*c;i<3*c+3;i++)
							{
							
								centroids[i]=image_vect[(row_id * ncols + col_id) * nb_channels + (i-3)];

								dist+=pow(image_vect[(row_id * ncols + col_id) * nb_channels + (i-3)]-centroids[i-3],2);
							}
						}while(sqrt(dist)>5 && std::find(centroids.begin(), centroids.end(), centroids[c]) != centroids.end());
						
					}
					break;
				}
			}
  
  
  return centroids;
}

std::vector<uchar> update_centroids(int nb_centroids, int nb_channels,
                                     std::vector<double>& updated_centroids,
                                     std::vector<double>& centroids_occurences)
{
	std::vector<uchar> centroids(nb_channels * nb_centroids);
 
	switch(nb_channels)
			{
				case 1:
				{
					for(int c=0; c<nb_centroids; c++)
					{
						if(centroids_occurences[c] !=0)
							centroids[c]=(uchar)(updated_centroids[c]/centroids_occurences[c]);
					}
					break;
				}
				case 3:
				{					
					for(int c=0; c<nb_centroids; c++)
					{	
						if(centroids_occurences[c] !=0)
						{
							for(int k=0;k<nb_channels;k++){
							centroids[c*nb_channels+k]=(uchar)(updated_centroids[c*nb_channels+k]/centroids_occurences[c]);
							}
						}	
					}
					break;
				}

			}
	return centroids;
}

int calc_closest_centroid(uchar* pixel, int nb_channels,
                  std::vector<uchar>& centroids, int nb_centroids)
{
	double clust_dist, min_dist = 100000;
	int centroid_id = 0;
	
	for (int c = 0; c < nb_centroids; c++) {
		clust_dist = 0;
		for (int ch = 0; ch < nb_channels; ch++)
			clust_dist += pow((int) pixel[ch] - centroids[c * nb_channels + ch], 2);
		clust_dist = sqrt(clust_dist);
		if (clust_dist <= min_dist) {
			min_dist = clust_dist;
			centroid_id = c;
		}
	}
	
	return centroid_id;
}

void compute_segmentation(std::vector<uchar>& block, std::vector<uchar>& centroids,
             std::vector<uint8_t>& clustered_block,
             long npixels_local, int nb_channels, int nb_centroids,
             std::vector<double>& updated_centroids, std::vector<double>& centroids_occurences)
{
  // reinit cluster reduction buffers
  std::fill(updated_centroids.begin(), updated_centroids.end(), 0);
  std::fill(centroids_occurences.begin(), centroids_occurences.end(), 0);

  uchar* pixel;
  int id_centroid;
  for (int i = 0; i < npixels_local; i++) {
    pixel = block.data() + i * nb_channels;
    id_centroid = calc_closest_centroid(pixel, nb_channels, centroids, nb_centroids);
    clustered_block[i] = id_centroid;
    centroids_occurences[id_centroid] += 1;
    for (int j = 0; j < nb_channels; j++)
      updated_centroids[id_centroid * nb_channels + j] += pixel[j];
  }
}

void paint_segmentation(std::vector<uchar>& image_vect, long img_size,
                        std::vector<uchar>& centroids, int nb_channels,
                        std::vector<uint8_t>& clustered_block)
{
  for (int i = 0; i < img_size; i++)
    for (int j = 0; j < nb_channels; j++)
      image_vect[i * nb_channels + j] = centroids[clustered_block[i] * nb_channels + j];
}

int main( int argc, char** argv )
{
    using namespace boost::program_options;
    options_description desc;
    desc.add_options()
      ("help", "produce help")
      ("file",value<std::string>()->default_value(std::string("")), "image file")
      ("nb-centroids",value<int>()->default_value(0), "KMean k value: number of centroids")
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
      const int nb_centroids = vm["nb-centroids"].as<int>();
      const int nrows = image.rows;
      const int ncols = image.cols;
      const int maxiter = vm["max-iter"].as<int>();
      int iter = 0;
      std::cout << "NB CHANNELS : " << nb_channels << std::endl;
      std::cout << "NROWS       : " << nrows << std::endl;
      std::cout << "NCOLS       : " << ncols << std::endl;

      // Flattening image
      std::vector<uchar> image_vect(nrows * ncols * nb_channels);
      if (image.isContinuous())
        image_vect.assign(image.datastart, image.dataend);

      { // Timer scope
        // Starting timer count
        PPTP::Timer::Sentry sentry(timer, "Kmeans_MPI");

        // Broadcasting sizes
        int buff[2] = { nb_channels, nb_centroids };
        MPI_Bcast(buff, 2, MPI_INT, 0, MPI_COMM_WORLD);

        // Splitting & sending image matrix
        int r = (nrows * ncols) % nb_proc;
		long npixels_local=nrows * ncols/nb_proc;
		if(0<r)
		{
			npixels_local++;
		}
		
        long start = nb_channels * npixels_local;
        
	for (int i = 1; i < nb_proc; i++) {
		npixels_local=nrows * ncols/nb_proc;
		if(i<r)
		{
			npixels_local++;
		}
		MPI_Send(&npixels_local, 1, MPI_LONG, i, 1000, MPI_COMM_WORLD);
		MPI_Send(image_vect.data() + start, npixels_local * nb_channels,
                  MPI_UNSIGNED_CHAR, i, 2000, MPI_COMM_WORLD);
		start += npixels_local * nb_channels;
        }

        // Process initialization
        std::vector<uint8_t> clustered_block(npixels_local);
        std::vector<double> updated_centroids(nb_channels * nb_centroids);
        std::vector<double> centroids_occurences(nb_centroids);
        std::vector<uchar> centroids = init_centroids(image_vect, nrows, ncols,
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
          compute_segmentation(image_vect, centroids, clustered_block, npixels_local,
                  nb_channels, nb_centroids, updated_centroids, centroids_occurences);

          // Receiving clusters elements count reduction
          for (int i = 1; i < nb_proc; i++) {
            MPI_Recv(count_buff, nb_centroids, MPI_DOUBLE,
                    i, 2333, MPI_COMM_WORLD, &status);
            for (int j = 0; j < nb_centroids; j++)
              centroids_occurences[j] += count_buff[j];
          }

          // Receiving clusters colors sum reduction
          for (int i = 1; i < nb_proc; i++) {
            MPI_Recv(color_buff, nb_centroids * nb_channels, MPI_DOUBLE,
                    i, 2666, MPI_COMM_WORLD, &status);
            for (int j = 0; j < nb_centroids * nb_channels; j++)
              updated_centroids[j] += color_buff[j];
          }

          // Computing new centroids
          new_centroids = update_centroids(nb_centroids, nb_channels,
                                            updated_centroids, centroids_occurences);

          // Convergence test
          converged = (new_centroids == centroids) || ((++iter) >= maxiter);
          MPI_Bcast(&converged, 1, MPI_C_BOOL, 0, MPI_COMM_WORLD);
          centroids.swap(new_centroids);
        }

        // Painting image according to clustering result
        paint_segmentation(image_vect, npixels_local,
                          centroids, nb_channels,
                          clustered_block);

        // Receive partial matrix parts
        start = npixels_local * nb_channels;
        for (int i = 1; i < nb_proc; i++) {
			npixels_local=nrows * ncols/nb_proc;
			if(i<r)
			{
				npixels_local++;
			}
			MPI_Recv(image_vect.data() + start, npixels_local * nb_channels,
                	MPI_UNSIGNED_CHAR, i, 3000, MPI_COMM_WORLD, &status);
			start += npixels_local * nb_channels;
        }

        // Reconstruct image
        result = cv::Mat(nrows, ncols,
                         (nb_channels == 1) ? CV_8UC1 : CV_8UC3,
                         image_vect.data());
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
      long npixels_local_receive = 0;
      MPI_Recv(&npixels_local_receive, 1, MPI_LONG, 0, 1000, MPI_COMM_WORLD, &status);
      std::vector<uchar> block(npixels_local_receive * nb_channels);
      std::vector<uint8_t> clustered_block(npixels_local_receive);
      MPI_Recv(block.data(), npixels_local_receive * nb_channels, MPI_LONG, 0, 2000, MPI_COMM_WORLD, &status);


      converged = false;
      std::vector<double> updated_centroids(nb_channels * nb_centroids);
      std::vector<double> centroids_occurences(nb_centroids);
      std::vector<uchar> centroids(nb_channels * nb_centroids);
      while (!converged) {
        // Receiving updated centroids
        MPI_Bcast(centroids.data(), nb_channels * nb_centroids,
                  MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

        // Process own data
        compute_segmentation(block, centroids, clustered_block, npixels_local_receive,
                nb_channels, nb_centroids, updated_centroids, centroids_occurences);

        // Sending reduced clusters data
        MPI_Send(centroids_occurences.data(), nb_centroids, MPI_DOUBLE,
                 0, 2333, MPI_COMM_WORLD);
        MPI_Send(updated_centroids.data(), nb_centroids * nb_channels,
                 MPI_DOUBLE, 0, 2666, MPI_COMM_WORLD);

        // Receiving convergence state
        MPI_Bcast(&converged, 1, MPI_C_BOOL, 0, MPI_COMM_WORLD);
      }

      // Painting image according to clustering result
      paint_segmentation(block, npixels_local_receive,
                         centroids, nb_channels,
                         clustered_block);

      // Sending result image parts
      MPI_Send(block.data(), npixels_local_receive * nb_channels,
               MPI_UNSIGNED_CHAR, 0, 3000, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0 ;
}
