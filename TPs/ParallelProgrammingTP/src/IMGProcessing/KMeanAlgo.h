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
#include <iostream>
#include <time.h>
#include <omp.h>
#include <chrono>
#include <ctime>
#include <limits>
#include "tbb/tbb.h"
#include <string>

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

		//Setters for centroids
		void setCentroids(std::vector<uchar> centr)
		{

			m_centroids = centr;
			for (size_t i = 0; i < centr.size(); i++)
			{
				m_new_centroids[i] = (double)centr.at(i);
			}
		}

		//Getters for centroids

		std::vector<uchar> getCentroids()
		{
			return (m_centroids);
		}

		//Main kmeans method : iterative computing & update of centroids positions & associated
		//pixels of the image
		int compute_centroids(cv::Mat const &image, int max_iterations, double epsilon, std::string mode)
		{
			using namespace cv;

			m_nb_channels = image.channels(); //Setting image number of channels

			int n = 0;
			std::vector<double> displacement(m_nb_centroid, 0);
			double max_displacement = 0;

			do
			{
				std::cout << "-------------------- ITERATION " << n + 1 << "----------------------------\n"
						  << std::endl;
				displacement = update_centroid(image, mode);

				//We pick the max displacement from the returned per-cluster displacements vector
				//The stop condition checks whether this max displacement is smaller than epsilon
				//Which means that ALL centroids displacements were under this threshold.
				max_displacement = 0;

				for (auto &disp : displacement)
				{
					max_displacement = (disp > max_displacement) ? disp : max_displacement;
				}

				n += 1;

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
			return (n+1);
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
			for (int k = 0; k < m_nb_centroid; k++)
			{
				j = rand() % image.cols;
				i = rand() % image.rows;

				for (int c = 0; c < m_nb_channels; c++)
				{
					pixel = m_nb_channels == 1 ? image.at<uchar>(i, j) : image.at<Vec3b>(i, j)[c];
					m_centroids.push_back(pixel);
					m_new_centroids[k * m_nb_channels + c] = (double)pixel;
				}
			}
		}

		//Updating the centroids based on the surrounding closest pixels,
		//Returns a vector of euclidean differences between old and new position for each centroid

		std::vector<double>
		update_centroid(cv::Mat const &image, std::string mode = "seq")
		{
			using namespace cv;
			using namespace tbb;

			bool omp = (mode == "omp");
			bool tbb = (mode == "tbb");

			int length = m_nb_channels * m_nb_centroid;
			double distance = 0;
			uint32_t c = 0;

			//Initializing pixel-centroid mapping vector and displacement vector
			for (int i = 0; i < m_nb_centroid; i++)
			{
				m_cluster_sizes[i] = 0;
			}

			std::vector<double> displacement(m_nb_centroid, 0.0);

			//OPEN MP OR SEQ MODES
			if (!tbb)
			{
				//Grayscale
				if (m_nb_channels == 1)
				{
#pragma omp parallel for schedule(static, 8) reduction(+                                                \
													   : m_cluster_sizes [0:m_nb_centroid]) reduction(+ \
																									  : m_new_centroids [0:length]) firstprivate(distance, c) shared(image, m_mapping) num_threads(8) if (omp)
					for (auto row = 0; row < image.rows; row++)

					{
						const uchar *pixelsRow = image.ptr<uchar>(row); //Faster access through rows pointer

						for (auto col = 0; col < image.cols; col++)
						{
							distance = std::numeric_limits<double>::max();

							c = argClosest(distance, row, col, image);

							m_mapping[row * image.cols + col] = c;

							//One more pixel belonging to cluster c : increment by 1
							m_cluster_sizes[c] += 1; //Reduction

							//Accumulating pixels value in the c position (helps computing barycentre later)
							m_new_centroids[c] += (double)(pixelsRow[col]);
						}
					}
				}
				//RGB
				else
				{
#pragma omp parallel for schedule(static, 8) reduction(+                                                \
													   : m_cluster_sizes [0:m_nb_centroid]) reduction(+ \
																									  : m_new_centroids [0:length]) firstprivate(distance, c) shared(image, m_mapping) num_threads(8) if (omp)
					for (auto row = 0; row < image.rows; row++)

					{
						const Vec3b *pixelsRow = image.ptr<Vec3b>(row); //Faster access through rows pointer

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
								m_new_centroids[c * m_nb_channels + channel] += (double)(pixelsRow[col][channel]);
							}
						}
					}
				}
			}
			//TBB
			else
			{
				std::vector<double> new_centroids(m_nb_channels * m_nb_centroid, 0.0);
				new_centroids = tbb::parallel_reduce(
					//Range (along rows)
					tbb::blocked_range<size_t>(0, image.rows),
					//Id value
					std::vector<double>(m_nb_channels * m_nb_centroid, 0),
					//Kernel
					[&](tbb::blocked_range<size_t> localRows, std::vector<double> temp_new_centroids) -> std::vector<double> {
						for (auto row = localRows.begin(); row < localRows.end(); row++)
						{
							//Gray Scale
							if (m_nb_channels == 1)
							{

								const uchar *pixelsRow = image.ptr<uchar>(row);
								uint32_t c = 0;

								for (auto col = 0; col < image.cols; col++)
								{
									double distance = std::numeric_limits<double>::max();

									c = argClosest(distance, row, col, image);

									m_mapping[row * image.cols + col] = c;

									//Accumulating pixels value in the c position (helps computing barycentre later)

									temp_new_centroids[c] += (double)(pixelsRow[col]);
								}
							}
							//RGB
							else
							{
								const Vec3b *pixelsRow = image.ptr<Vec3b>(row); //Faster access through rows pointer
								uint32_t c = 0;

								for (auto col = 0; col < image.cols; col++)
								{
									double distance = std::numeric_limits<double>::max();

									c = argClosest(distance, row, col, image);

									m_mapping[row * image.cols + col] = c;

									//Accumulating pixels value in the c position (helps computing barycentre later)

									for (uint16_t channel = 0; channel < m_nb_channels; channel++)
									{
										temp_new_centroids[c * m_nb_channels + channel] += (double)(pixelsRow[col][channel]);
									}
								}
							}
						}

						return (temp_new_centroids);
					},
					//Reduction function
					[](std::vector<double> a, std::vector<double> b) -> std::vector<double> {
						std::vector<double> c(a.size());
						for (size_t i = 0; i < a.size(); i++)
						{
							c[i] = a.at(i) + b.at(i);
						}
						return (c);
					});
				for (size_t i = 0; i < m_nb_centroid * m_nb_channels; i++)
				{
					m_new_centroids[i] = new_centroids.at(i);
				}

				//Reducing on size of clusters
				std::vector<double> cluster_sizes(m_nb_centroid, 0.0);

				cluster_sizes = tbb::parallel_reduce(
					//Range (along rows)
					tbb::blocked_range<size_t>(0, image.rows),
					//Id value
					std::vector<double>(m_nb_centroid, 0),
					//Kernel
					[&](tbb::blocked_range<size_t> localRows, std::vector<double> temp_cluster_sizes) -> std::vector<double> {
                    for (auto row = localRows.begin(); row < localRows.end(); row++)
                    {
                        for (int col = 0; col < image.cols; col++)
                        {
                            temp_cluster_sizes[m_mapping[row * image.cols + col]] += 1;
                        }
                    }
                    return(temp_cluster_sizes); },
					//Reduction
					[](std::vector<double> a, std::vector<double> b) -> std::vector<double> {
						std::vector<double> c(a.size());
						for (size_t i = 0; i < a.size(); i++)
						{
							c[i] = a.at(i) + b.at(i);
						}
						return (c);
					});
				for (size_t i = 0; i < m_nb_centroid; i++)
				{
					m_cluster_sizes[i] = cluster_sizes.at(i);
				}
			}

			//Computing barycentre, measuring displacement between old and new centroid & updating centroid values
			std::fill(displacement.begin(), displacement.end(), 0);

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

			return (displacement);
		}
		//Closest centroid
		uint32_t
		argClosest(double &distance, int row, int col, cv::Mat const &image)
		{
			//No parallelization needed to avoid false sharing / race conditions since loops are relatively small (Nb of centroids)
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
		void compute_segmentation(cv::Mat &image, std::string mode = "seq")
		{
			using namespace cv;
			using namespace tbb;

			bool omp = (mode == "omp");
			bool tbb = (mode == "tbb");

			//OPEN MP OR SEQ MODES
			if (!tbb)
			{
				uint32_t k = 0; //Selected cluster id (retrieved from m_mapping)

				if (m_nb_channels == 1)
				{
					//Grayscale image segmentation

#pragma omp parallel for firstprivate(k) schedule(static, 8) if (omp)
					for (int row = 0; row < image.rows; row++)
					{
						for (int col = 0; col < image.cols; col++)
						{
							k = m_mapping[row * (image.cols) + col];
							uchar &pixel = image.at<uchar>(row, col);
							pixel = (uchar)m_centroids[k];
						}
					}
				}

				//RGB image segmentation
				else
				{
#pragma omp parallel for firstprivate(k) schedule(static, 8) if (omp)
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
			}
			//TBB MODE
			else
			{
				//Grayscale image segmentation
				if (m_nb_channels == 1)
				{
					parallel_for(blocked_range<int>(0, (int)image.rows), [=, &image](blocked_range<int> localRows) {
						for (int row = localRows.begin(); row < localRows.end(); row++)
						{
							parallel_for(blocked_range<int>(0, (int)image.cols), [=, &image](blocked_range<int> localCols) {
								for (int col = localCols.begin(); col < localCols.end(); col++)
								{
									int k = m_mapping[row * (image.cols) + col];

									uchar &pixel = image.at<uchar>(row, col);

									pixel = (uchar)m_centroids[k];
								}
							});
						}
					});
				}
				//RGB image segmentation
				else
				{
					parallel_for(blocked_range<int>(0, (int)image.rows), [=, &image](blocked_range<int> localRows) {
						for (int row = localRows.begin(); row < localRows.end(); row++)
						{
							parallel_for(blocked_range<int>(0, (int)image.cols), [=, &image](blocked_range<int> localCols) {
								for (int col = localCols.begin(); col < localCols.end(); col++)
								{
									int k = m_mapping[row * (image.cols) + col];

									Vec3b &pixel = image.at<Vec3b>(row, col);

									for (int c = 0; c < m_nb_channels; c++)
									{
										pixel[c] = (uchar)m_centroids[k * m_nb_channels + c];
									}
								}
							});
						}
					});
				}
			}
		} //end seg function

		//Kmeans + Image segmentation
		int
		process(cv::Mat &image, int max_iterations = 1000, double epsilon = 1, std::string mode = "seq")
		{
			int nb_iterations = 1;
			std::string myMode = (mode == "seq" ? "Sequential" : (mode == "tbb" ? "TBB" : "OPENMP"));
			std::cout << "Started segmentation, mode : " << myMode << std::endl;
			init_centroids(image);
			nb_iterations = compute_centroids(image, max_iterations, epsilon, mode);
			compute_segmentation(image, mode);
			std::cout << "Output generated : Done" << std::endl;
		}
	};
} // namespace PPTP

#endif /* SRC_IMGPROCESSING_KMEANALGO_H_ */
