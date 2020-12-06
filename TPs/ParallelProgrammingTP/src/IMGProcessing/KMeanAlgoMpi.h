#include <cmath>
#include <cstdint>
#include <iostream>
#include <time.h>
#include <chrono>
#include <ctime>
#include <limits>
#include <vector>
#include <cuchar>
#include <opencv2/opencv.hpp>

//Random centroids initialization based on image dimensions
void init_centroids(cv::Mat const &image, int nb_channels, int nb_centroid, std::vector<uchar> &centroids)
{
    using namespace cv;

    std::cout << "Initialization started for " << nb_centroid << " centroids" << std::endl;

    //Random seed for centroids random generation

    time_t t;
    srand((unsigned)time(&t));

    int i, j;
    uchar pixel;

    for (int k = 0; k < nb_centroid; k++)
    {
        j = rand() % image.cols;
        i = rand() % image.rows;

        for (int c = 0; c < nb_channels; c++)
        {
            pixel = nb_channels == 1 ? image.at<uchar>(i, j) : image.at<Vec3b>(i, j)[c];
            centroids.push_back(pixel);
        }
    }
}

//Closest centroid
uint32_t argClosest(double &distance, int index, std::vector<u_char> const &flat_image, int nb_channels, int nb_centroids, std::vector<u_char> centroids)
{
    using namespace cv;

    uchar pixel = 0;
    double temp = 0;
    uint32_t c = 0;

    for (uint16_t k = 0; k < nb_centroids; k++)
    {
        temp = 0;

        for (uint16_t channel = 0; channel < nb_channels; channel++)
        {
            pixel = flat_image.at(index + channel);
            temp += ((int)pixel - (int)centroids[k * nb_channels + channel]) * ((int)pixel - (int)centroids[k * nb_channels + channel]);
        }

        if (temp <= distance)
        {
            distance = temp;
            c = k;
        }
    }

    return (c);
}

//Updating centroid based on pixels proximity (partial operation since we don't have the whole image)
void update_centroid(std::vector<u_char> const &flat_image,
                     int total_length,
                     int nb_centroid,
                     int nb_channels,
                     std::vector<u_char> centroids,
                     std::vector<long> &cluster_sizes,
                     std::vector<float> &newCentroids,
                     std::vector<uint16_t> &mapping)
{
    using namespace cv;

    uchar pixel = 0;
    double distance = 0;
    uint32_t c = 0;

    //Initializing pixel-centroid mapping vector and displacement vector
    for (int i = 0; i < nb_centroid; i++)
    {
        cluster_sizes[i] = 0;
    }

    //Grayscale images & RGB images

    for (int index = 0; index < total_length; index++)
    {
        distance = std::numeric_limits<double>::max();

        //Closest centroid
        c = argClosest(distance, index * nb_channels, flat_image, nb_channels, nb_centroid, centroids);

        //One more pixel belonging to cluster c : increment by 1
        cluster_sizes[c] += 1;

        //Update mapping
        mapping[index] = c;

        //Accumulating pixels value in the c position (helps computing barycentre later)
        for (uint16_t channel = 0; channel < nb_channels; channel++)
        {
            newCentroids[c * nb_channels + channel] += (long)(flat_image.at(index * nb_channels + channel));
        }
    }
}

//Segmentation
void gen_segmentation(std::vector<u_char> &flat_image,
                      int nb_channels,
                      int total_length,
                      std::vector<u_char> const &centroids,
                      std::vector<uint16_t> const &mapping)
{
    using namespace cv;

    for (int index = 0; index < total_length; index++)
    {
        for (int channel = 0; channel < nb_channels; channel++)
        {
            flat_image[index * nb_channels + channel] = (int)centroids.at(mapping[index] + (channel + 2) % nb_channels);
        }
    }
}