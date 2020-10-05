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

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
    using namespace boost::program_options ;
    options_description desc;
    desc.add_options()
        ("help", "produce help")
        ("nx",value<int>()->default_value(0), "NX")
        ("ny",value<int>()->default_value(0), "NY")
        ("LX",value<int>()->default_value(0), "LX")
        ("LY",value<int>()->default_value(0), "LY")
        ("show",value<int>()->default_value(0), "show image")
        ("filter",value<int>()->default_value(0), "filter value")
        ("compute-nb-cc",value<int>()->default_value(0), "compute cc")
        ("to-gray",value<int>()->default_value(0), "create gray image") ;
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << "\n";
        return 1;
    }

    double PI = 3.1415 ;
    int LX = vm["LX"].as<int>() ;
    int LY = vm["LY"].as<int>() ;
    int nx = vm["nx"].as<int>() ;
    int ny = vm["ny"].as<int>() ;
    Mat image(nx,ny,CV_8UC1,Scalar(0));

    double omegai = 2*PI/LX ;
    double omegaj = 2*PI/LY ;

    for(std::size_t i=1;i<image.rows;++i)
    {
      for(int j=1;j<image.cols;++j)
      {
        image.at<uchar>(i,j) = (int)(255*0.5*(1+std::sin(omegai*i)*cos(omegaj*j)));
      }
    }

    if(vm["show"].as<int>()==1)
    {
      namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
      imshow( "Display window", image );                   // Show our image inside it.
      waitKey(0);                                          // Wait for a keystroke in the window
      return 0 ;
    }

    imwrite("./GraySinCosImage.jpg",image) ;

    if(vm["compute-nb-cc"].as<int>()==1)
    {
      int filter = vm["filter"].as<int>() ;
      Mat filter_image = image ;
      Mat fimage = image ;
      for(std::size_t i=1;i<image.rows;++i)
      {
        for(int j=1;j<image.cols;++j)
        {
          if(image.at<uchar>(i,j) > filter)
          {
            filter_image.at<uchar>(i,j) = 1 ;
            fimage.at<uchar>(i,j) = 255 ;
          }
          else
          {
            filter_image.at<uchar>(i,j) = 0 ;
            fimage.at<uchar>(i,j) = 0 ;
          }
        }
      }

      Mat labels(nx,ny,CV_32S,Scalar(0));
      int nb_component= cv::connectedComponents(filter_image,
                                                labels,
                                                4,
                                                CV_32S,
                                                CCL_DEFAULT) ;
      std::cout<<"NB COMPONENTS : "<<nb_component<<std::endl ;

      namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
      imshow( "Display window", fimage );                   // Show our image inside it.
      waitKey(0);                                          // Wait for a keystroke in the window
    }
    return 0 ;
}

