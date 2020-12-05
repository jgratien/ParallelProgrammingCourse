/*
 *  img_kmean.cpp
 *
 *  Edited on: Nov 27, 2020
 *     Author: ramyk
 */

#include <iostream>
#include <random>
#include <algorithm>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/variables_map.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <omp.h>
#include <tbb/tbb.h>

#include "IMGProcessing/KMeanAlgo.h"
#include "Utils/Timer.h"

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
    using namespace boost::program_options;
    options_description desc;
    desc.add_options()
        ("help", "produce help")
        ("file",value<std::string>()->default_value(std::string("")), "image file")
        ("mode",value<std::string>()->default_value(std::string("seq")), "Computing mode <seq|omp|tbb>")
        ("nb-threads",value<int>()->default_value(1), "Number of threads: Useful only for 'omp' and 'tbb' moodes")
        ("k",value<int>()->default_value(2), "KMean k value")
        ("max-iter", value<int>()->default_value(10), "Kmean maximum iterations number");
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }

    std::string img_file = vm["file"].as<std::string>();
    Mat image;
    image = imread(img_file.c_str(), CV_LOAD_IMAGE_COLOR);   // Read the file

    // Check for invalid input
    if(! image.data ) {
        cout <<  "Could not open or find the image" << std::endl;
        return -1;
    }

    // Check for invalid mode
    std::string mode = vm["mode"].as<std::string>();
    if (mode != "seq" && mode != "omp" && mode != "tbb") {
        cout <<  "Could not select requested mode" << std::endl;
        return -1;
    }

    // Setup threads number
    int nb_threads = vm["nb-threads"].as<int>();
    omp_set_num_threads(nb_threads);
    tbb::task_scheduler_init init(nb_threads);

    cout<<"NB CHANNELS : "<<image.channels()<<std::endl;
    cout<<"NROWS       : "<<image.rows<<std::endl;
    cout<<"NCOLS       : "<<image.cols<<std::endl;
    const int channels = image.channels();

    int nb_centroids = vm["k"].as<int>();
    int maxiter = vm["max-iter"].as<int>();
    PPTP::Timer timer;

    {   // Timer scope
        // Starting timer count
        PPTP::Timer::Sentry sentry(timer, "Kmeans_" + mode);
        PPTP::KMeanAlgo algo(channels, nb_centroids, maxiter, nb_threads);
        if (mode == "seq")      algo.process(image);
        else if (mode == "omp") algo.omp_process(image);
        else                    algo.tbb_process(image);
    }// Stopping timer

    boost::to_upper(mode);
    std::cout << "Writing image output" << std::endl;
    imwrite("./" + mode + "-out.jpg", image);

    timer.printInfo();
    return 0;
}
