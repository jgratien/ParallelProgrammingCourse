/*
 * main.cpp
 *
 *  Created on: Oct 8, 2019
 *      Author: gratienj
 */



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
#include <IMGProcessing/KMeanAlgo.h>
#include <IMGProcessing/KMeanAlgoOpenMP.h>
#include <IMGProcessing/KMeanAlgoTbb.h>
#include <Utils/Timer.h>
#include "omp.h"
#include "tbb/tbb.h"

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
    using namespace boost::program_options ;
    options_description desc;
    desc.add_options()
        ("help", "produce help")
	("nb-threads",value<int>()->default_value(0), "nb threads")
        ("file",value<std::string>(), "image file")
        ("show",value<int>()->default_value(0), "show image")
        ("seg",value<int>()->default_value(0), "kmean segmentation")
	("seg-openmp",value<int>()->default_value(0), "Kmean segmentation using openmp")
	("seg-tbb",value<int>()->default_value(0), "Kmean segmentation using tbb")
        ("kmean-value",value<int>()->default_value(0), "KMean k value")
        ("max_iter",value<int>()->default_value(50), "max iterations");
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << "\n";
        return 1;
    }

    int nb_threads = vm["nb-threads"].as<int>();
    if(nb_threads>0)
    {
	omp_set_num_threads(nb_threads);
	tbb::task_scheduler_init init(nb_threads);
    }

    using namespace PPTP;
    Timer timer;

    std::string img_file = vm["file"].as<std::string>() ;
    Mat image;
    image = imread(img_file.c_str(), CV_LOAD_IMAGE_COLOR);   // Read the file

    if(! image.data )                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    if(vm["show"].as<int>()==1)
    {
      namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
      imshow( "Display window", image );                   // Show our image inside it.
      waitKey(0);                                          // Wait for a keystroke in the window
      return 0 ;
    }

    cout<<"NB CHANNELS : "<<image.channels()<<std::endl ;
    cout<<"NROWS       : "<<image.rows<<std::endl ;
    cout<<"NCOLS       : "<<image.cols<<std::endl ;
    const int channels = image.channels();

    int nb_centroids = vm["kmean-value"].as<int>() ;
    int max_iter = vm["max_iter"].as<int>();
    
    if(vm["seg"].as<int>()==1)
    {
      switch(channels)
      {
        case 1:
          {
		  Timer::Sentry sentry(timer, "KMEANS SEGMENTATION SEQUENTIAL");
		  KMeanAlgo algo(1, nb_centroids, max_iter) ;
		  std::cout << "starting processing ... " << std::endl;
        	  algo.process(image) ;
          }
          break ;
        case 3:
          {
		  Timer::Sentry sentry(timer, "KMEANS SEGMENTATION SEQUENTIAL");
		  KMeanAlgo algo(3, nb_centroids, max_iter) ;
		  std::cout << "starting processing ... " << std::endl;
        	  algo.process(image) ;
          }
          break ;
      }
      timer.printInfo();
      imwrite("../Seg_Image.jpg",image) ;
    }

    if(vm["seg-openmp"].as<int>()==1)
    {
      switch(channels)
      {
        case 1:
          {
		  Timer::Sentry sentry(timer, "KMEANS SEGMENTATION WITH OPENMP");
		  KMeanAlgoOpenMP algo(1, nb_centroids, max_iter) ;
		  std::cout << "starting processing ... " << std::endl;
        	  algo.process(image) ;
          }
          break ;
        case 3:
          {
		  Timer::Sentry sentry(timer, "KMEANS SEGMENTATION WITH OPENMP");
		  KMeanAlgoOpenMP algo(3, nb_centroids, max_iter) ;
		  std::cout << "starting processing ... " << std::endl;
        	  algo.process(image) ;
          }
          break ;
      }
      timer.printInfo();
      imwrite("../OpenMp_Seg_Image.jpg",image) ;
    }

    if(vm["seg-tbb"].as<int>()==1)
    {
      switch(channels)
      {
        case 1:
          {
		  Timer::Sentry sentry(timer, "KMEANS SEGMENTATION WITH TBB");
		  KMeanAlgoTbb algo(1, nb_centroids, max_iter) ;
		  std::cout << "starting processing ... " << std::endl;
        	  algo.process(image) ;
          }
          break ;
        case 3:
          {
		  Timer::Sentry sentry(timer, "KMEANS SEGMENTATION WITH TBB");
		  KMeanAlgoTbb algo(3, nb_centroids, max_iter) ;
		  std::cout << "starting processing ... " << std::endl;
        	  algo.process(image) ;
          }
          break ;
      }
      timer.printInfo();
      imwrite("../Tbb_Seg_Image.jpg",image) ;
    }

    return 0 ;
}

