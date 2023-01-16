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
        ("file",value<std::string>(), "image file")
        ("show",value<int>()->default_value(0), "show image")
        ("add-noise",value<int>()->default_value(0), "add noise")
        ("noise-density",value<int>()->default_value(0), "noise density")
        ("filter",value<int>()->default_value(0), "filtre median")
        ("to-gray",value<int>()->default_value(0), "create gray image") ;
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << "\n";
        return 1;
    }

    std::string img_file = vm["file"].as<std::string>() ;
    Mat image;
    //image = imread(img_file.c_str(), CV_LOAD_IMAGE_COLOR);   // Read the file
    image = imread(img_file.c_str(), cv::IMREAD_COLOR);   // Read the file

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

    if(vm["add-noise"].as<int>()==1)
    {
      int nsample = vm["noise-density"].as<int>() ;
      std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_int_distribution<> dis(0,image.cols);
      std::uniform_int_distribution<> dis_pixel(0,255);

      switch(channels)
      {
        case 1:
          for(std::size_t i=0;i<image.rows;++i)
          {
            for(int k=0;k<nsample;++k)
            {
              int j = dis(gen) ;
              image.at<uchar>(i,j) = (uchar) dis_pixel(gen) ;
            }
          }
          break ;
        case 3:
          Mat_<Vec3b> _I = image;
          for(std::size_t i=0;i<image.rows;++i)
          {
            for(int k=0;k<nsample;++k)
            {
              int j = dis(gen) ;
              _I(i,j)[0] = (uchar) dis_pixel(gen) ;
              _I(i,j)[1] = (uchar) dis_pixel(gen) ;
              _I(i,j)[2] = (uchar) dis_pixel(gen) ;
              //_I(i,j)[0] = 255 ;
              //_I(i,j)[1] = 255 ;
              //_I(i,j)[2] = 255 ;
            }
          }
          break ;
      }

      imwrite("./Noisy_Image.jpg",image) ;
    }

    if(vm["filter"].as<int>()==1)
    {
      switch(channels)
      {
        case 1:
          for(std::size_t i=1;i<image.rows-1;++i)
          {
            for(int j=1;j<image.cols-1;++j)
            {
              array<uchar,5> buffer ;
              buffer[0] = image.at<uchar>(i-1,j) ;
              buffer[1] = image.at<uchar>(i,j-1) ;
              buffer[2] = image.at<uchar>(i,j) ;
              buffer[3] = image.at<uchar>(i,j+1) ;
              buffer[4] = image.at<uchar>(i+1,j) ;
              std::sort(buffer.begin(),buffer.end()) ;
              image.at<uchar>(i,j) = buffer[2] ;
            }
          }
          break ;
        case 3:
          Mat_<Vec3b> _I = image;
          for(std::size_t i=1;i<image.rows-1;++i)
          {
            for(int j=1;j<image.cols-1;++j)
            {
              for(int k=0;k<3;++k)
              {
                array<uchar,5> buffer ;
                buffer[0] = _I(i-1,j)[k] ;
                buffer[1] = _I(i,j-1)[k] ;
                buffer[2] = _I(i,j)[k] ;
                buffer[3] = _I(i,j+1)[k] ;
                buffer[4] = _I(i+1,j)[k] ;
                std::sort(buffer.begin(),buffer.end()) ;
                _I(i,j)[k] = buffer[2] ;
              }
            }
          }
          break ;
      }

      imwrite("./Filter_Image.jpg",image) ;
    }



    if(vm["to-gray"].as<int>()==1)
    {

      Mat gray_image;
      //cvtColor(image,gray_image,CV_BGR2GRAY);
      cvtColor(image,gray_image,cv::COLOR_BGR2RGB);
      imwrite("./Gray_Image.jpg",gray_image) ;

      cout<<"NB CHANNELS : "<<gray_image.channels()<<std::endl ;
      cout<<"NROWS       : "<<gray_image.rows<<std::endl ;
      cout<<"NCOLS       : "<<gray_image.cols<<std::endl ;
    }
    return 0 ;
}

