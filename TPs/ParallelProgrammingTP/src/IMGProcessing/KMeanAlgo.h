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

namespace PPTP
{

	class KMeanAlgo
	{
	private:
		uint16_t m_nb_channels = 3;
		uint16_t m_nb_centroid = 1;
		std::vector<uchar> m_centroids;   //Of size m_nb_centroid*m_nb_channels
		std::vector<double> m_new_centroids;
		std::vector<uint64_t> m_mapping;
		std::vector<uint64_t> m_cluster_sizes;

	public:
		//Constructor
		KMeanAlgo(uint16_t nb_channels, uint16_t nb_centroids)
			: m_nb_channels(nb_channels), m_nb_centroid(nb_centroids)
		{
			m_centroids.reserve(nb_centroids * nb_channels);
			m_cluster_sizes.reserve(nb_centroids);
			m_new_centroids.reserve(nb_centroids * nb_channels);
			std::cout<<"KMeans algo constructed for nb channels = "<<m_nb_centroid <<std::endl;	
		}
		//Destructor
		virtual ~KMeanAlgo() {}

		//Main kmeans method : iterative computing & update of centroids positions & associated
		//pixels of the image
		void compute_centroids(cv::Mat const &image, int max_iterations, double epsilon)
		{
			using namespace cv;
			

			Mat_<Vec3b> _I = image;
			//Testing whether the image is rgb or gs
			bool isGS = false;
			//for(int c = 0;c<((int)image.channels() - 1);c++)
			//{
			//	isGS &= ((int)_I(0,0)[c] == (int)_I(0,0)[c+1] ); 
			//}
			
			m_nb_channels =isGS? 1 :  image.channels();

			std::cout<<"Initialization started for "<<m_nb_channels <<" channels" <<std::endl;

			init_centroids(image);

			int n = 0;
			std::vector<double> displacement(m_nb_centroid, 0);
			double max_displacement = 0;
			
			do
			{
				displacement = update_centroid(image);
				max_displacement = 0;
				for(auto &disp:displacement){
					max_displacement = (disp>max_displacement)? disp :max_displacement;
				}
				n += 1;
				std::cout<<"+ Displacement at iteration " <<n <<" = "<<max_displacement <<std::endl;
					
				for(int i=0;i<m_nb_centroid;i++){
					std::cout<<"++ Nb pixels for cluster" <<i <<" = " <<m_cluster_sizes[i] <<std::endl;
				}

				std::cout<<"----------------------------------------------------------\n"<<std::endl;

			} while ((n<max_iterations) & (max_displacement> epsilon));
			if(n==(max_iterations-1)){
				std::cout<<"Optimal centroids computed after reaching max iterations : " <<n+1 <<std::endl;
			}
			else{
				std::cout<<"Optimal centroids computed after reaching minimum displacement, and "<< n <<" iterations." <<std::endl;
			}
			for(int k=0;k<m_nb_centroid;k++){
				std::cout<<"Centroid [ " <<k <<" ] : \t (";
				if(m_nb_channels==1){
					std::cout<<(int)m_centroids[k] <<")" <<std::endl;
				}	
				else{
					for(int c = 0;c<m_nb_channels-1;c++){
						std::cout<<(int)m_centroids[k*m_nb_channels + c] << " , ";
					}
					std::cout<<(int)m_centroids[k*m_nb_channels + m_nb_channels - 1] << ") "<< std::endl;
				}
			}


		}

		//Random centroids initialization based on image dimensions
		void init_centroids(cv::Mat const &image)
		{
			using namespace cv;
			
			time_t t;

    			srand((unsigned)time(&t));
			
			int i, j;
			uchar pixel;

			switch (m_nb_channels)
			{
			case 1:

				for (int k = 0; k < m_nb_centroid; k++)
				{
					i = rand() % image.cols;
					j = rand() % image.rows;
					pixel = image.at<uchar>(i, j);
					m_centroids.push_back(pixel);
					m_new_centroids.push_back((double)pixel);
				}
				break;
			case 3:
				Mat_<Vec3b> _I = image;

				for (int k = 0; k < m_nb_centroid; k++)
				{
					i = rand() % image.cols;
					j = rand() % image.rows;
					for (int c = 0; c < 3; c++)
					{
						pixel = _I(i, j)[c];
						m_centroids.push_back(pixel);
						m_new_centroids.push_back((double)pixel);
					}

				}

				break;
			}
			std::cout<<"Initialization done" <<std::endl;
			
		}

		//Updating the centroid based on the surrounding closest pixels, returns the
		//euclidean difference between old and new position (sum for all clusters)
 		std::vector<double> update_centroid(cv::Mat const &image)
		{
			using namespace cv;

			uchar pixel;
			double distance = MAX_UCHAR * MAX_UCHAR; //To be privatized
			double temp = 0;						 //To be privatized
			uint32_t c = 0;							 //To be privatized

			//Computing distances and selecting the closest centroid
			std::fill(m_cluster_sizes.begin(), m_cluster_sizes.end(), 0);
			std::fill(m_mapping.begin(), m_mapping.end(), 0);
			std::vector<double> displacement(m_nb_centroid, 0);

			switch (m_nb_channels)
			{
			case 1:


				for(int i=0;i<m_nb_centroid;i++){
					m_cluster_sizes[i] = 0; 
				}
				//Grayscale images
				for (auto row = 0; row < image.rows; row++)
				{
					for (auto col = 0; col< image.cols; col++)
					{
						pixel = image.at<uchar>(row, col);

						distance = ((int)pixel - (int)m_centroids[0]) * ((int)pixel - (int)m_centroids[0]);
						
						for (uint16_t k = 0; k < m_nb_centroid; k++)
						{
							temp = ((int)pixel -(int)m_centroids[k]) * ((int)pixel -(int)m_centroids[k]);
							if (temp < distance)
							{
								distance = temp;
								c = k;
							}
						}

						m_mapping[row * image.cols + col] = c; //Pixel's Corresponding centroid
						m_cluster_sizes[c] += 1;
						m_new_centroids[c] += (double)pixel;
					}
				}

				//Compute sum of displacements of all centroids (will serve as a stop condition)

				std::fill(displacement.begin(), displacement.end(), 0);
				for (uint16_t k = 0; k < m_nb_centroid; k++)
				{
					m_new_centroids[k] /= ((double)m_cluster_sizes[k] != 0 ? m_cluster_sizes[k] : 1);
					displacement[k] = fabs(m_new_centroids[k] - (double)m_centroids[k]);
					m_centroids[k] = (uchar)m_new_centroids[k];
				}

			case 3:
				//RGB images
				Mat_<Vec3b> _I = image;
				for(int i=0;i<m_nb_centroid;i++){
					m_cluster_sizes[i] = 0; 
				}

				for (auto row = 0; row < image.rows; row++)
				{
					for (auto col = 0; col<image.cols; col++)
					{
						for (uint16_t k = 0; k < m_nb_centroid; k++)
						{
							temp = 0;
							distance = 0;

							for (uint16_t channel = 0; channel < m_nb_channels; channel++)
							{
								pixel = _I(row, col)[channel];
								distance += ((int)pixel - (int)m_centroids[0 * m_nb_channels + channel]) * ((int)pixel - (int)m_centroids[0 * m_nb_channels + channel]);
							}
							for (uint16_t channel = 0; channel < m_nb_channels; channel++)
							{
								pixel = _I(row, col)[channel];
								temp += ((int)pixel - (int)m_centroids[k * m_nb_channels + channel]) * ((int)pixel - (int)m_centroids[k * m_nb_channels + channel]);
							}

							if (temp <= distance)
							{
								distance = temp;
								c = k;
							}
						
						}
						
						m_mapping[row * image.cols + col] = c; //Pixel's Corresponding centroid
						m_cluster_sizes[c] += 1;
						for (uint16_t channel = 0; channel < m_nb_channels; channel++)
						{
							pixel = _I(row, col)[channel];
							m_new_centroids[c * m_nb_channels + channel] += (double)pixel;
						}
					}
				}
				std::fill(displacement.begin(), displacement.end(), 0);
				//Compute sum of displacements of all centroids (will serve as a stop condition)
				for (uint16_t k = 0; k < m_nb_centroid; k++)
				{
					for (uint16_t channel = 0; channel < m_nb_channels; channel++)
					{

						m_new_centroids[k * m_nb_channels + channel] /= ((double)m_cluster_sizes[k] != 0 ? m_cluster_sizes[k] : 1);
						displacement[k] += (m_new_centroids[k * m_nb_channels + channel] - (double)m_centroids[k * m_nb_channels + channel]) * (m_new_centroids[k * m_nb_channels + channel] - (double)m_centroids[k * m_nb_channels + channel]);
				
						m_centroids[k* m_nb_channels + channel] = (uchar)m_new_centroids[k*m_nb_channels + channel]; 
					}
				
					displacement[k] = sqrt(displacement[k]/m_nb_channels);
				}

				break;
			}
				return (displacement);
			}

		//Segmentation 
		void compute_segmentation(cv::Mat &image)
		{
			using namespace cv;

			uint64_t k = 0; //Selected cluster id (retrieved from m_mapping)

			switch (m_nb_channels)
			{
			case 1:
				for (int row = 0; row < image.rows; row++)
				{
					for (int col = 0; col < image.cols; col++)
					{
						k = m_mapping[row * (image.cols) + col];
						uchar &pixel = image.at<uchar>(row, col);
						pixel = (uchar)m_centroids[k];
					}
				}
			case 3:

				for (int row = 0; row < image.rows; row++)
				{
					for (int col = 0; col < image.cols; col++)
					{
						k = m_mapping[row * (image.cols) + col];

						Vec3b &pixel = image.at<Vec3b>(row, col);

						for (int c = 0; c < m_nb_channels; c++)
						{
							pixel[c] = (uchar)m_centroids[k * m_nb_channels + c];
						}
					}
				}

				break;
			}

		}

		//Kmeans + Image segmentation
		void process(cv::Mat &image, int max_iterations = 1000, double epsilon = 1)
		{
			m_mapping.reserve(image.rows * image.cols);
			compute_centroids(image, max_iterations, epsilon);
			std::cout<<"Optimal centroids computed : Done" <<std::endl;
			compute_segmentation(image);
			std::cout<<"Output generated : Done" <<std::endl;
		}
	};
} // namespace PPTP

#endif /* SRC_IMGPROCESSING_KMEANALGO_H_ */
