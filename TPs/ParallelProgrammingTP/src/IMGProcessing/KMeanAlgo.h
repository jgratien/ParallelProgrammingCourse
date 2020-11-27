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

namespace PPTP
{

	class KMeanAlgo
	{
	private:
		uint16_t m_nb_channels = 3;
		uint16_t m_nb_centroid = 1;
		std::vector<uchar> m_centroids;		//Of size m_nb_centroid*m_nb_channels
		double *m_new_centroids{nullptr};	//Stores temporary sums of pixels to compute barycentre later and evaluate centroid displacement
		uint32_t *m_mapping{nullptr};		//Of size rows * cols, indicates for each flattened index the corresponding centroid (or cluster) [0,0,1,1, etc..]
		uint32_t *m_cluster_sizes{nullptr}; //Of size m_nb_centroid : contains number of pixels for each cluser. Helps computing barycentre with m_new_centroids

	public:
		//Constructor
		KMeanAlgo(uint16_t nb_channels, uint16_t nb_centroids)
			: m_nb_channels(nb_channels), m_nb_centroid(nb_centroids)
		{
			m_centroids.reserve(nb_centroids * nb_channels);
			m_cluster_sizes = new uint32_t[nb_centroids];
			m_new_centroids = new double[m_nb_centroid * m_nb_channels];
			std::cout << "KMeans algo constructed for nb channels = " << m_nb_centroid << std::endl;
		}
		//Destructor
		virtual ~KMeanAlgo()
		{
			delete[] m_cluster_sizes;
			delete[] m_mapping;
			delete[] m_new_centroids;
		}

		//Main kmeans method : iterative computing & update of centroids positions & associated
		//pixels of the image
		void compute_centroids(cv::Mat const &image, int max_iterations, double epsilon, bool parallel)
		{
			using namespace cv;

			m_nb_channels = image.channels(); //Setting image number of channels

			init_centroids(image);

			std::cout << "Init done" << std::endl;

			int n = 0;
			std::vector<double> displacement(m_nb_centroid, 0);
			double max_displacement = 0;

			do
			{
				std::cout << "-------------------- ITERATION " << n + 1 << "----------------------------\n"
						  << std::endl;
				displacement = update_centroid(image, parallel);

				//We pick the max displacement from the returned per-cluster displacements vector
				//The stop condition checks whether this max displacement is smaller than epsilon
				//Which means that ALL centroids displacements were under this threshold.
				max_displacement = 0;

				for (auto &disp : displacement)
				{
					max_displacement = (disp > max_displacement) ? disp : max_displacement;
				}

				n += 1;
				std::cout << "+ Displacement at iteration " << n << " = " << max_displacement << std::endl;

				for (int i = 0; i < m_nb_centroid; i++)
				{
					std::cout << "++ Nb pixels for cluster" << i << " = " << m_cluster_sizes[i] << std::endl;
				}

				std::cout << "----------------------------------------------------------\n"
						  << std::endl;

			} while ((n < max_iterations) & (max_displacement > epsilon));

			//Recap of the training phase
			//	1 - Displaying the nb of iterations the algorithm had before stopping
			if (n == (max_iterations - 1))
			{
				std::cout << "Optimal centroids computed after reaching max iterations : " << n + 1 << std::endl;
			}
			else
			{
				std::cout << "Optimal centroids computed after reaching minimum displacement, and " << n << " iterations." << std::endl;
			}
			//	2 - Displaying the resulting centroids values
			for (int k = 0; k < m_nb_centroid; k++)
			{
				std::cout << "Centroid [ " << k << " ] : \t (";
				if (m_nb_channels == 1)
				{
					std::cout << (int)m_centroids[k] << ")" << std::endl;
				}
				else
				{
					for (int c = 0; c < m_nb_channels - 1; c++)
					{
						std::cout << (int)m_centroids[k * m_nb_channels + c] << " , ";
					}
					std::cout << (int)m_centroids[k * m_nb_channels + m_nb_channels - 1] << ") " << std::endl;
				}
			}
		}

		//Random centroids initialization based on image dimensions
		void init_centroids(cv::Mat const &image)
		{
			using namespace cv;

			m_mapping = new uint32_t[image.rows * image.cols];

			std::cout << "Initialization started for " << m_nb_channels << " channels" << std::endl;

			//Random seed for centroids random generation
			time_t t;
			srand((unsigned)time(&t));

			int i, j;
			uchar pixel;

			switch (m_nb_channels)
			{
			//Grays Scale
			case 1:

				for (int k = 0; k < m_nb_centroid; k++)
				{
					j = rand() % image.cols;
					i = rand() % image.rows;
					pixel = image.at<uchar>(i, j);
					m_centroids.push_back(pixel);
					m_new_centroids[k] = (double)pixel;
				}
				break;

			//RGB
			case 3:
				Mat_<Vec3b> *_I = new Mat_<Vec3b>();
				*_I = image;
				std::cout << "Image copied by pointer" << std::endl;
				for (int k = 0; k < m_nb_centroid; k++)
				{

					j = rand() % (image.cols - 2) + 1;
					i = rand() % (image.rows - 2) + 1;
					for (int c = 0; c < 3; c++)
					{
						pixel = (*_I)(i, j)[c];
						m_centroids.push_back(pixel);
						m_new_centroids[k * m_nb_channels + c] = (double)pixel;
					}
				}
				delete _I;

				break;
			}
		}

		//Updating the centroids based on the surrounding closest pixels,
		//Returns a vector of euclidean differences between old and new position for each centroid
		std::vector<double> update_centroid(cv::Mat const &image, bool parallel)
		{
			using namespace cv;

			uchar pixel = 0;
			double distance = 0;
			uint32_t c = 0;

			//Initializing pixel-centroid mapping vector and displacement vector
			for (int i = 0; i < m_nb_centroid; i++)
			{
				m_cluster_sizes[i] = 0;
			}

			std::vector<double> displacement(m_nb_centroid, 0.0);

			switch (m_nb_channels)
			{
			//Grayscale images
			case 1:
				for (auto row = 0; row < image.rows; row++)
				{
					for (auto col = 0; col < image.cols; col++)
					{
						distance = std::numeric_limits<double>::max();

						c = argClosest(distance, row, col, image);

						pixel = image.at<uchar>(row, col);

						m_mapping[row * image.cols + col] = c; //Pixel's Corresponding centroid
						m_cluster_sizes[c] += 1;			   //One more pixel belonging to cluster c : increment by 1
						m_new_centroids[c] += (double)pixel;   //Accumulating pixels value in the c position (helps computing barycentre later)
					}
				}

				//Computing barycentre, measuring displacement between old and new centroid & updating centroid values
				std::fill(displacement.begin(), displacement.end(), 0);
				for (uint16_t k = 0; k < m_nb_centroid; k++)
				{
					m_new_centroids[k] /= ((double)m_cluster_sizes[k] != 0 ? m_cluster_sizes[k] : 1);
					displacement[k] = fabs(m_new_centroids[k] - (double)m_centroids[k]);
					m_centroids[k] = (uchar)m_new_centroids[k];
				}
				break;

			//RGB images
			case 3:

#pragma omp parallel if (parallel)
			{
#pragma omp for collapse(2)                                \
	firstprivate(pixel, distance)                    \
		schedule(static, 32)                               \
			reduction(+                                    \
					  : m_cluster_sizes [0:m_nb_centroid]) \
				reduction(-                                \
						  : m_new_centroids [0:(m_nb_centroid * m_nb_channels)])

				for (auto row = 0; row < image.rows; row++)

				{
					for (auto col = 0; col < image.cols; col++)
					{
						distance = std::numeric_limits<double>::max();

						c = argClosest(distance, row, col, image);

						m_mapping[row * image.cols + col] = c;

						//One more pixel belonging to cluster c : increment by 1
						m_cluster_sizes[c] += 1; //Reduction

						//Accumulating pixels value in the c position (helps computing barycentre later)
						for (uint16_t channel = 0; channel < m_nb_channels; channel++)
						{
							pixel = (m_nb_channels == 1) ? image.at<uchar>(row, col) : image.at<Vec3b>(row, col)[channel];
							m_new_centroids[c * m_nb_channels + channel] += (double)pixel;
						}
					}
				}
#pragma omp single
				//Computing barycentre, measuring displacement between old and new centroid & updating centroid values
				std::fill(displacement.begin(), displacement.end(), 0);

#pragma omp for schedule(static, 8)
				for (uint16_t k = 0; k < m_nb_centroid; k++)
				{
					for (uint16_t channel = 0; channel < m_nb_channels; channel++)
					{
						m_new_centroids[k * m_nb_channels + channel] /= (double)(m_cluster_sizes[k] != 0 ? (double)m_cluster_sizes[k] : 1.0);
						displacement[k] += (m_new_centroids[k * m_nb_channels + channel] - (double)m_centroids[k * m_nb_channels + channel]) * (m_new_centroids[k * m_nb_channels + channel] - (double)m_centroids[k * m_nb_channels + channel]);

						m_centroids[k * m_nb_channels + channel] = (uchar)m_new_centroids[k * m_nb_channels + channel];
					}
					//displacement contains doubles = > chuncks of 8 (8*64 = 64bytes) is enough to avoid false sharing
					displacement[k] = sqrt(displacement[k] / m_nb_channels); //Divide by nb_channels since displacement accumulates sums for each channel
				}
			}
			break;
			}
			return (displacement);
		}

		//Closest centroid
		uint32_t argClosest(double &distance, int row, int col, cv::Mat const &image)
		{
			using namespace cv;

			uchar pixel = 0;
			double temp = 0;
			uint32_t c = 0;

			for (uint16_t k = 0; k < m_nb_centroid; k++)
			{
				temp = 0;

				for (uint16_t channel = 0; channel < m_nb_channels; channel++)
				{
					pixel = (m_nb_channels == 1) ? image.at<uchar>(row, col) : image.at<Vec3b>(row, col)[channel];
					temp += ((int)pixel - (int)m_centroids[k * m_nb_channels + channel]) * ((int)pixel - (int)m_centroids[k * m_nb_channels + channel]);
				}

				if (temp <= distance)
				{
					distance = temp;
					c = k;
				}
			}

			return (c);
		}

		//Segmentation
		void compute_segmentation(cv::Mat &image, bool parallel)
		{
			using namespace cv;

			uint32_t k = 0; //Selected cluster id (retrieved from m_mapping)

			switch (m_nb_channels)
			{
			//Grayscale image segmentation
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

			//RGB image segmentation
			case 3:

#pragma omp parallel if (parallel)
			{
#pragma omp for collapse(2) schedule(static, 64)
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
			}
			break;
			}
		}

		//Kmeans + Image segmentation
		void process(cv::Mat &image, int max_iterations = 1000, double epsilon = 1, bool parallel = false)
		{
			const clock_t begin_time = clock();
			compute_centroids(image, max_iterations, epsilon, parallel);
			compute_segmentation(image, parallel);
			const clock_t end_time = clock();
			std::cout << "Elapsed time (in clock ticks) : " << (float)(end_time - begin_time) / (CLOCKS_PER_SEC / 1000) << std::endl;
			std::cout << "Output generated : Done" << std::endl;
		}
	};
} // namespace PPTP

#endif /* SRC_IMGPROCESSING_KMEANALGO_H_ */

