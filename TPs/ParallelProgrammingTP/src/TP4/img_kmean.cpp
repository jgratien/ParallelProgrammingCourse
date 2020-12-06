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
#include <chrono>
#include <map>
#include <mpi.h>
#include <mutex>
#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"
#include "tbb/task_scheduler_init.h"
#include "omp.h"
#include <boost/lexical_cast.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/variables_map.hpp>
#include <IMGProcessing/KMeanAlgo.h>

using namespace cv;
using namespace std;
using namespace std::chrono;

int main(int argc, char **argv)
{
  using namespace boost::program_options;
  options_description desc;
  desc.add_options()
    ("help", "produce help")
    ("file", value<std::string>(), "image file")
    ("show", value<int>()->default_value(0), "show image")
    ("seg", value<int>()->default_value(0), "kmean segmentation")
    ("nb-centroids", value<int>()->default_value(1), "centroids number (k value)");
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
  image = imread(img_file.c_str(), CV_LOAD_IMAGE_COLOR); // Read the file

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

  cout << "NB CHANNELS : " << image.channels() << std::endl;
  cout << "NROWS       : " << image.rows << std::endl;
  cout << "NCOLS       : " << image.cols << std::endl;
  const int channels = image.channels();

  int nb_centroids = vm["nb-centroids"].as<int>();


  if (vm["seg"].as<int>() == 1)
  {
    PPTP::KMeanAlgo algo(channels, nb_centroids);
    
    auto start = high_resolution_clock::now();
    
    // Process the image
    algo.process(image);
    
    auto stop = high_resolution_clock::now();
    // Calculate Time in microsecs
    auto exec_time = duration_cast<microseconds>(stop - start);
    
    // Save result
    imwrite("./Seg_Image.jpg", image);
    
    // Print exec time
    cout << "-----------------"
     	<< "\nMETHOD : "
      	<< " SERIAL "
       	<< "\nEXECUTION TIME: "
     	<< exec_time.count()
       	<< " MicroSecs"
       	<< "\n-----------------"
       	<< std::endl;
  }

  return 0;
}

