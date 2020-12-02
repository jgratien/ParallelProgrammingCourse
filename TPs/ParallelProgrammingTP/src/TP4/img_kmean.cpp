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
#include <IMGProcessing/KMeanAlgo_parallel_omp.h>
#include <IMGProcessing/KMeanAlgo_parallel_tbb.h>
#include "Utils/Timer.h"
#include "tbb/tbb.h"


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
        ("seg",value<int>()->default_value(0), "kmean segmentation")
        ("kmean-value",value<int>()->default_value(0), "KMean k value") 
        ("nb-centroids",value<int>()->default_value(0), "nb centroids")
        ("nb-iterations",value<int>()->default_value(50), "nb iterations")
	("mode", value<int>()->default_value(0), "0:sequential 1:openmp 2:tbb")
    	("nb-threads", value<int>()->default_value(0), "nb threads");
    	
    	
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    using namespace PPTP;
    Timer timer;

    if (vm.count("help"))
    {
        std::cout << desc << "\n";
        return 1;
    }

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

    int nb_centroids = vm["nb-centroids"].as<int>() ;
    int nb_iterations = vm["nb-iterations"].as<int>();
    
    cout<<"NB CENTROIDS       : "<<nb_centroids<<std::endl ;
    cout<<"NB ITERATIONS MAX       : "<<nb_iterations<<std::endl ;
    
    if(vm["seg"].as<int>()==1)
    {
	const int mode= vm["mode"].as<int>();    
	switch(mode)
	{
		case 0:
		{	
			PPTP::KMeanAlgo algo(channels,nb_centroids, nb_iterations) ;
			Timer::Sentry sentry(timer, "Sequential Kmeans");
			algo.process(image) ;
			break;
		}
		case 1:
		{
			int nb_threads = vm["nb-threads"].as<int>();
			if(nb_threads>0){
				omp_set_num_threads(nb_threads);
			}
			PPTP::KMeanAlgoParOmp algo(channels, nb_centroids, nb_iterations);
			Timer::Sentry sentry(timer, "Parallel OMP Kmeans");
			algo.process_omp(image);
			break;
		}
		case 2:
		{
			int nb_threads = vm["nb-threads"].as<int>();	
			if(nb_threads>0){
				tbb::task_scheduler_init init(nb_threads);
			}
			Timer::Sentry sentry(timer, "Parallel TBB Kmeans");
			cout<<"on est la"<<endl;
			PPTP::KMeanAlgoParTbb algo(channels, nb_centroids, nb_iterations);
			algo.process_tbb(image);
			break;
		}
	}
       	timer.printInfo();	
    	imwrite("./Seg_Image.jpg",image) ;
    }

    return 0 ;
}

