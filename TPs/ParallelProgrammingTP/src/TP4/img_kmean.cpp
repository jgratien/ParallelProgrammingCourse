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
#include "sys/time.h"
#include <ctime>

using namespace cv;
using namespace std;
double now()
{
    struct timeval t;
    double f_t;
    gettimeofday(&t, NULL);
    f_t = t.tv_usec;
    f_t = f_t / ((float)1E6);
    f_t += t.tv_sec;
    return (f_t);
}
int main(int argc, char **argv)
{

  using namespace boost::program_options;
  options_description desc;
  desc.add_options()("help", "produce help")("file", value<std::string>(), "image file")("show", value<int>()->default_value(0), "show image")("seg", value<int>()->default_value(0), "kmean segmentation")("kmean-value", value<int>()->default_value(0), "KMean k value");
  variables_map vm;
  store(parse_command_line(argc, argv, desc), vm);
  notify(vm);

  if (vm.count("help"))
  {
    std::cout << desc << "\n";
    return 1;
  }

  cout << "just entered" << endl;
  std::string img_file = vm["file"].as<std::string>();
  cout << "my image file : " << img_file << endl;
  Mat image, image2;
  image = imread(img_file.c_str(), CV_LOAD_IMAGE_COLOR); // Read the file

  image2 = imread(img_file.c_str(), CV_LOAD_IMAGE_COLOR); // Read the file
  cout << "just entered" << endl;
  if (!image.data) // Check for invalid input
  {
    cout << "Could not open or find the image" << std::endl;
    return -1;
  }

  cout << "image found good" << endl;
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

  double startSeq, endSeq;
  double startPar, endPar;

  int nb_centroids = vm["kmean-value"].as<int>();

  if (vm["seg"].as<int>() == 1)
  {
    if (channels == 3)
    {
      cout << "=========RGB Kmeans=========" << endl;
    }
    else
    {
      cout << "=========GrayScale Kmeans=========" << endl;
    }
    PPTP::KMeanAlgo algoSeq(channels, nb_centroids);
    PPTP::KMeanAlgo algoPar(channels, nb_centroids);
    algoSeq.init_centroids(image);
    algoPar.init_centroids(image);
    algoPar.setCentroids(algoSeq.getCentroids());

    startSeq = now();
    algoSeq.process(image, 100,1,false);
    endSeq = now();

    startPar = now();
    algoPar.process(image2, 100,1,true);
    endPar = now();

    std::cout << "Acceleration OpenMP / Sequential : " << (endSeq - startSeq) / (endPar - startPar) << std::endl;
    //algo.process(image, false);
  }
  
  cout << "Writing image" << endl;
  img_file.resize(img_file.size() - 4);
  string fileName = img_file + "_Segmented.jpg";
  imwrite(fileName, image);
  cout << "Image written at : " << fileName << endl;

return 0;
}
