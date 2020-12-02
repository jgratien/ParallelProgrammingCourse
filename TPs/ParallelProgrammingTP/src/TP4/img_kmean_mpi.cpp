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
#include <IMGProcessing/Kmean_mpi.h>
#include "Utils/Timer.h"

using namespace cv;
using namespace std;



int main( int argc, char** argv )
{
    using namespace boost::program_options ;
    options_description desc;
    desc.add_options()
        ("help", "produce help")
        ("file",value<std::string>(), "image file")
        ("show",value<int>()->default_value(0), "show image")
        ("seg",value<int>()->default_value(1), "kmeans segmentation")
        ("kmean-value",value<int>(), "Kmeans K value")	;
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << "\n";
        return 1;
    }

    using namespace PPTP;    
    Timer timer;
    MPI_Init(&argc,&argv) ;

    int my_rank = 0 ;
    int nb_proc = 1 ;
    MPI_Comm_size(MPI_COMM_WORLD,&nb_proc) ;
    MPI_Comm_rank(MPI_COMM_WORLD,&my_rank) ;

    if(my_rank==0)
    {
      std::string img_file = vm["file"].as<std::string>() ;
      Mat image;
      image = imread(img_file.c_str(), CV_LOAD_IMAGE_COLOR);   // Read the file

      if(! image.data )                              // Check for invalid input
      {
          cout <<  "Could not open or find the image" << std::endl ;
          return -1;
      }

      cout<<"NB CHANNELS : "<<image.channels()<<std::endl ;
      cout<<"NROWS       : "<<image.rows<<std::endl ;
      cout<<"NCOLS       : "<<image.cols<<std::endl ;
      const int nb_channels = image.channels();

      int nb_centroids = vm["kmean-value"].as<int>();
      std::vector<uchar> centroids;
      centroids.reserve(nb_centroids*nb_channels);

      // flatten image
      std::vector<uchar> flat_image; //(image.rows*image.cols*nb_channels);
      if(image.isContinuous())
      {
	flat_image.assign(image.datastart, image.dataend);
      }



      if(vm["seg"].as<int>()==1)
      {
	Timer::Sentry sentry(timer, "KMEANS SEGMENTATION WITH MPI");
	MPI_Status status;
	{
	  // BCAST NB_CHANNELS, NB_COLS AND NB_CENTROIDS TO ALL PROCS
	  std::vector<int> send_info={nb_channels, image.cols, nb_centroids};
	  MPI_Bcast(send_info.data(), 3, MPI_INT, 0, MPI_COMM_WORLD);
	}


	//std::vector<uchar> flat_image2 = flat_image;

	// SEND FLAT IMAGE USING MPI_SCATTERV

	// declare the counts
	//int counts[nb_proc];
	// declare the displacement
	//int displacements[nb_proc];
	//int start = 0;
	//int rows_zero;
	

	// save local rows of each proc in rows_local_info
	int rows_local_info[nb_proc];
	for (int i=0; i<nb_proc; i++)
	{
	  int rows_local = image.rows / nb_proc;
	  int r = image.rows % nb_proc;
	  if(r>i) rows_local ++;
	  rows_local_info[i] = rows_local;

	  //if(i==0) rows_zero = rows_local;
	  //counts[i] = rows_local*image.cols*nb_channels;
	  //displacements[i] = start;
	  //start += counts[i];
	}

	//MPI_Bcast(counts, nb_proc, MPI_INT, 0, MPI_COMM_WORLD);
	//MPI_Bcast(displacements, nb_proc, MPI_INT, 0, MPI_COMM_WORLD);

	//for(int i=0; i<nb_proc; i++)
	//{
	//	std::cout << displacements[i] << " " << counts[i] << std::endl;
	//}
	//std::vector<uchar> flat_image_local(counts[my_rank]);
	//std::cout << "local size " << flat_image_local.size() << std::endl;
	//std::cout << "global size " << flat_image.size() << std::endl;
	//MPI_Scatterv(flat_image.data(), counts, displacements, MPI_UNSIGNED_CHAR, flat_image_local.data(), counts[my_rank], MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD); 

	//std::cout << " scatter: " << (int) flat_image[0] << " " << (int) flat_image_local[0] << std::endl;

	int rows_local_proc_zero = rows_local_info[0];

        {
          // SEND IMAGE DATA
	  // send local image rows

	  int begin = rows_local_proc_zero;
	  for(int i=1; i<nb_proc; i++)
	  {
	    // send local size to proc i
	    int rows_local = rows_local_info[i];
	    MPI_Send(&rows_local, 1, MPI_INT, i, 1000, MPI_COMM_WORLD);

	    // send local part of image
	    uchar* ptr = flat_image.data() + begin*image.cols*nb_channels;
	    MPI_Send(ptr, rows_local*image.cols*nb_channels, MPI_UNSIGNED_CHAR, i, 2000, MPI_COMM_WORLD);
	    begin += rows_local;
	  }
        }

	{
	  // INIT CENTROIDS
	  // determine number of centroids to init by proc 0 i
	  int nb_cent_to_init_zero = nb_centroids / nb_proc;
	  int r = nb_centroids % nb_proc;
	  if(r>0) nb_cent_to_init_zero ++;

	  // init centroids of proc 0
	  for(int i=0; i<nb_cent_to_init_zero; i++)
	  {
	    std::vector<uchar> cent(nb_channels);
	    cent = init_centroid(flat_image, nb_channels);
	    centroids.insert(centroids.end(), cent.begin(), cent.end());
	  }


	  // determine number of centroids to init by proc i and send it
	  for(int i=1; i<std::min(nb_centroids,nb_proc); i++)
	  {
	    int nb_cent_to_init = nb_centroids / nb_proc;
	    int r = nb_centroids % nb_proc;
	    if(r>i) nb_cent_to_init ++;

	    MPI_Send(&nb_cent_to_init, 1, MPI_INT, i, 3000, MPI_COMM_WORLD);

	    // receive initialized centroids from procs
	    std::vector<uchar> centroids_local_to_receive(nb_channels*nb_cent_to_init);
	    MPI_Recv(centroids_local_to_receive.data(), nb_channels*nb_cent_to_init, MPI_UNSIGNED_CHAR, i, 4000, MPI_COMM_WORLD, &status);
	
	    // insert received centroids to the vector centroid
	    centroids.insert(centroids.end(), centroids_local_to_receive.begin(), centroids_local_to_receive.end());
	  }

	}
      

        {
          // COMPUTE CENTROIDS
          std::vector<int> pixel_collection(image.rows*image.cols);
	  int iter = 0;
	  double epsilon = 1;
	  while( (iter<50) && (epsilon>0.05) )
	  {
            std::cout << "iteration number: " << iter << std::endl;


	    // broad cast centroids vectors to all procs
	    MPI_Bcast(centroids.data(), nb_centroids*nb_channels, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

	    // calculate nearest centroid for each pixel
	    // each proc calculates for the pixel in his part of image
	    // nearest centroids of the part of proc 0
	    update_pixel_collection(pixel_collection, nb_channels, rows_local_proc_zero, image.cols, nb_centroids, centroids, flat_image);

	    // receive calculation of nearest centroids by other procs
	    int begin = image.cols*rows_local_proc_zero;
	    for(int i=1; i<nb_proc; i++)
	    {
		int rows_local = rows_local_info[i];

		std::vector<int> pixel_collection_local(image.cols*rows_local);

		MPI_Recv(pixel_collection_local.data(), image.cols*rows_local, MPI_INT, i, 5000, MPI_COMM_WORLD, &status);

		// add the received local pixel collection to the main pixel collection
		std::copy(pixel_collection_local.begin(), pixel_collection_local.end(), pixel_collection.begin()+begin);
		begin += image.cols*rows_local;

	    }


	    // UPDATING CENTROIDS
	    // count pixels for each centroid by proc 0
	    std::vector<double> count_pxl_local;
	    count_pxl_local.reserve((nb_channels+1)*nb_centroids);
	    count_pxl_local = count_pixels(nb_centroids, pixel_collection, flat_image, rows_local_proc_zero, image.cols, nb_channels);

	    // Reduce count from other procs	
	    std::vector<double> count_pxl((nb_channels+1)*nb_centroids);
	    MPI_Reduce(count_pxl_local.data(), count_pxl.data(), (nb_channels+1)*nb_centroids, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	    // save old centroids before updating them to calculate variance
	    std::vector<uchar> old_centroids = centroids;


	    // compute barycentre and update centroids
	    centroids = compute_barycenter_and_update_centroids(count_pxl, nb_centroids, nb_channels);
	
	    epsilon = compute_variance(old_centroids, centroids, nb_channels, nb_centroids);
            std::cout << "epsilon: " << epsilon << std::endl;

	    // send epsilon to other procs
	    MPI_Bcast(&epsilon, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	    
	    iter ++;
	    std::cout << "--------------------------------" << std::endl;
	  }
        }

        {
          // COMPUTE SEGMENTATION
	  // Bcast final centroids
	  MPI_Bcast(centroids.data(), nb_centroids*nb_channels, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
	
	  // compute segmentation by proc 0
	  compute_segmentation(flat_image, rows_local_proc_zero, image.cols, nb_channels, nb_centroids, centroids);

	  // receive segmentation results by other procs
	  int begin = image.cols*rows_local_proc_zero*nb_channels;
	  for(int i=1; i<nb_proc; i++)
	  {
	    // receive flat_image_local after segmentation
	    int rows_local = rows_local_info[i];

	    std::vector<uchar> flat_image_local(rows_local*image.cols*nb_channels);
	    MPI_Recv(flat_image_local.data(), rows_local*image.cols*nb_channels, MPI_UNSIGNED_CHAR, i, 7000, MPI_COMM_WORLD, &status);

	    // add flat_image_local to flat_image
	    std::copy(flat_image_local.begin(), flat_image_local.end(), flat_image.begin()+begin);
	    begin += image.cols*rows_local*nb_channels;
	  }

        }

	// unflat image
	unflat_image(flat_image, image, nb_channels);
	

        imwrite("../MPI_Seg_Image.jpg",image) ;

      }
      timer.printInfo();

    }
    else
    {
      // MY RANK different 0

      MPI_Status status;

      if(vm["seg"].as<int>()==1)
      {
	// RECEIVE NB_CHANNELS, NB_ROWS, NB_COLS AND NB_CENTROIDS FROM PROC 0
	std::vector<int> recv_info(3);
	MPI_Bcast(recv_info.data(), 3, MPI_INT, 0, MPI_COMM_WORLD);
	int nb_channels = recv_info[0];
	int nb_cols = recv_info[1];
	int nb_centroids = recv_info[2];

        
        // RECV IMAGE DATA FROM PROC 0
	int rows_local;
	MPI_Recv(&rows_local, 1, MPI_INT, 0, 1000, MPI_COMM_WORLD, &status);
	
	std::vector<uchar> flat_image_local(rows_local*nb_cols*nb_channels);
	MPI_Recv(flat_image_local.data(), rows_local*nb_cols*nb_channels, MPI_UNSIGNED_CHAR, 0, 2000, MPI_COMM_WORLD, &status);

	// scatter part
	
	//int counts[nb_proc];
	//int displacements[nb_proc];
	//MPI_Bcast(counts, nb_proc, MPI_INT, 0, MPI_COMM_WORLD);
	//MPI_Bcast(displacements, nb_proc, MPI_INT, 0, MPI_COMM_WORLD);
	//for(int i=0; i<nb_proc; i++)
	//{
	//	std::cout << displacements[i] << "-" << counts[i] << std::endl;
	//}

	//std::vector<uchar> flat_image_local(counts[my_rank]);
	//std::cout << " size local: " << flat_image_local.size();
	//MPI_Scatterv(NULL, counts, displacements, MPI_UNSIGNED_CHAR, flat_image_local.data(), counts[my_rank], MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

	// std::cout << "scatter: " << flat_image_local.size() << std::endl;

	{
       	  // INIT CENTROIDS
	  // receive nb_centroids to init
	  if(my_rank<std::min(nb_proc,nb_centroids))
	  {
	    int nb_centroids_to_init;
	    MPI_Recv(&nb_centroids_to_init, 1, MPI_INT, 0, 3000, MPI_COMM_WORLD, &status);
	    std::vector<uchar> centroids_local;
	    centroids_local.reserve(nb_centroids_to_init*nb_channels);

	    for(int i=0; i<nb_centroids_to_init; i++)
	    {
	      std::vector<uchar> cent(nb_channels);
	      cent = init_centroid(flat_image_local, nb_channels);
	      centroids_local.insert(centroids_local.end(), cent.begin(), cent.end());

	    }
	    
	    // send initialized centroids to proc 0 
	    MPI_Send(centroids_local.data(), nb_channels, MPI_UNSIGNED_CHAR, 0, 4000, MPI_COMM_WORLD);

	  }
	}

        {
	  int iter = 0;
	  //double epsilon = 1;
	  while (iter<50) 
	  {
            // receive centroids vector
	    std::vector<uchar> centroids(nb_centroids*nb_channels);
	    MPI_Bcast(centroids.data(), nb_centroids*nb_channels, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

	    // calculate nearest centroid
	    std::vector<int> pixel_collection_local(nb_cols*rows_local);
	    update_pixel_collection(pixel_collection_local, nb_channels, rows_local, nb_cols, nb_centroids, centroids, flat_image_local);

	    // send pixel_collection_local to proc 0
	    MPI_Send(pixel_collection_local.data(), nb_cols*rows_local, MPI_INT, 0, 5000, MPI_COMM_WORLD);


	    // UPDATING CENTROIDS
	    // count pixels
	    std::vector<double> count_pxl_local;
	    count_pxl_local.reserve((nb_channels+1)*nb_centroids);
	    count_pxl_local = count_pixels(nb_centroids, pixel_collection_local, flat_image_local, rows_local, nb_cols, nb_channels);


	    std::vector<double> count_pxl((nb_channels+1)*nb_centroids);
	    MPI_Reduce(count_pxl_local.data(), count_pxl.data(), (nb_channels+1)*nb_centroids, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	    // receive epsilon value
	    double epsilon;
	    MPI_Bcast(&epsilon, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	    if(epsilon<=0.05) break;
	    iter++;
	  }
        }

        {
          // COMPUTE SEGMENTATION
	  // Bcast final centroid vect from proc 0
	  std::vector<uchar> centroids(nb_centroids*nb_channels);
	  MPI_Bcast(centroids.data(), nb_centroids*nb_channels, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
	  // compute segmentation by each proc
	  compute_segmentation(flat_image_local, rows_local, nb_cols, nb_channels, nb_centroids, centroids);
	  // send segmentation results to proc 0
	  MPI_Send(flat_image_local.data(), rows_local*nb_cols*nb_channels, MPI_UNSIGNED_CHAR, 0, 7000, MPI_COMM_WORLD);
        }
      }
    }
    MPI_Finalize();
    return 0 ;
}

