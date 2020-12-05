/*
 * KMeanAlgo.h
 *
 *  Created on: Nov 4, 2020
 *      Author: gratienj
 */

#ifndef SRC_IMGPROCESSING_KMEANALGO_H_
#define SRC_IMGPROCESSING_KMEANALGO_H_
#include <cmath>
#include <cstdint>
#define MAX_UCHAR 255
#include <iostream>
#include <time.h>
#include <omp.h>
#include <chrono>
#include <ctime>
#include <limits>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <random>
#include <algorithm>
using namespace std;
using namespace chrono;
using namespace cv;

namespace PPTP
{

	class KMeanAlgo
	{

       private:
	       int nbchannels = 3;
	       int nbcentroids = 1;
	       //std::vector<uchar> centroids;

       public:
		KMeanAlgo(int nb_channels, int nb_centroids)
	        : nbchannels(nb_channels)
		, nbcentroids(nb_centroids)
		{
		  //centroids.reserve(nb_channels*nb_centroids);
		}
		
		
		virtual ~KMeanAlgo() {}

		int nearest_centroid(std::vector<uchar>& pix, std::vector<double>& cent, int index)
		    {
			using namespace cv ;

		      int z=0;
		      double sum=0;
		      double r = fabs((double)pix[index] - cent[0]);
		      double g = fabs((double)pix[index+1] - cent[1]);
		      double b = fabs((double)pix[index+2] - cent[2]);
		      double min = r*r + g*g + b*b;

		      for(int t=1; t<this->nbcentroids;t++)
		      {
		      r = fabs((double)pix[index] - cent[3*t]);
		      g = fabs((double)pix[index+1] - cent[3*t+1]);
		      b = fabs((double)pix[index+2] - cent[3*t+2]);
		      sum = r*r + g*g + b*b;

                               if (sum<min)
			       {
			         min = sum; z = t;}
		       }
		    
		        return z;	
	         }

		
		
		double compute_displacement(std::vector<double>& new_centroids, std::vector<double>& centroids)
		{
                      /*using namespace cv ;
		      switch(nb_channels)
		      {
		        case 1:
		          break ;
		        case 3:
		          break ;
		      }*/
			double rc = (new_centroids[0] - centroids[0]);
                	double gc = (new_centroids[1] - centroids[1]);
                   	double bc = (new_centroids[2] - centroids[2]);
			double disp = rc*rc + gc*gc + bc*bc;
			for(int i=1; i<this->nbcentroids;i++)
			    {
			   	rc = (new_centroids[3*i] - centroids[3*i]);
                	        gc = (new_centroids[3*i+1] - centroids[3*i+1]);
                   	        bc = (new_centroids[3*i+2] - centroids[3*i+2]);
				double max = rc*rc + gc*gc + bc*bc;
				if (max>disp) disp=max;
			     }
                        //disp = sqrt(disp);
                        return disp;

		}

		/*void process(cv::Mat& image)
		{
			//compute_centroids(image) ;
			//compute_segmentation(image) ;
		}*/

	};
}

#endif /* SRC_IMGPROCESSING_KMEANALGO_H_ */
