/*
 *  KMeanAlgo.h
 *
 *  Edited on: Nov 27, 2020
 *     Author: ramyk
 */

#ifndef SRC_IMGPROCESSING_KMEANALGO_H_
#define SRC_IMGPROCESSING_KMEANALGO_H_

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cstdlib>

namespace PPTP
{

	class KMeanAlgo
	{
		private :
			int nb_channels;
			int nb_threads;
			int m_nb_centroid;
			int m_maxiter;
			// vector holding all centroids
			std::vector<uchar*> centroids;
			// color sum of clusters
			std::vector<double*> clst_colorsum;
			// count of elements of clusters
			std::vector<double> clst_count;
			// matrix for clusters mapping
			uchar* clustered_img;

		public:
			KMeanAlgo(int nb_channels, int nb_centroids,
					  int max_iter, int nb_threads)
				: nb_channels(nb_channels)
				, nb_threads(nb_threads)
				, m_nb_centroid(nb_centroids)
				, m_maxiter(max_iter)
			{
				clst_count.reserve(nb_centroids);
				for(int i = 0; i < nb_centroids; i++) {
					centroids.push_back(new uchar[nb_channels]);
					clst_colorsum.push_back(new double[nb_channels]);
					clst_count[i] = 0;
				}
			}
			virtual ~KMeanAlgo() {}

			void init_centroids(cv::Mat& image) {
				srand(time(0));
				int row, col;
				for (int i = 0; i < m_nb_centroid; i++) {
					row = rand() % image.rows;
					col = rand() % image.cols;
					if (nb_channels == 1)
						centroids[i][0] = image.at<uchar>(row, col);
					else
						for (int j = 0; j < nb_channels; j++)
							centroids[i][j] = image.at<cv::Vec3b>(row, col)[j];
				}
				// init clustered_img matrix
				clustered_img = new uchar[image.rows * image.cols];
			}

			void compute_centroids() {
				double* colorsum;
				for (int i = 0; i < m_nb_centroid; i++) {
					colorsum = clst_colorsum[i];
					std::cout << "FOUND " << clst_count[i] << " elements in cluster n." << i << std::endl;
					for (int j = 0; j < nb_channels; j++) {
						centroids[i][j] = (int)(colorsum[j]/clst_count[i]);
						colorsum[j] = 0;
					}
					clst_count[i] = 0;
				}
			}

			int nrst_centroid_index(cv::Mat& image, int i, int j) {
				double min_dst = 1000, dst = 0;
				int nrst_indx = 0;
				for (int ind = 0; ind < m_nb_centroid; ind++) {
					if (nb_channels == 1) {
						dst = fabs(image.at<uchar>(i, j) - centroids[ind][0]);
					}
					else {
						for (int ch = 0; ch < nb_channels; ch++)
							dst += pow((int) image.at<cv::Vec3b>(i, j)[ch] - centroids[ind][ch], 2);
						dst = sqrt(dst);
					}
					if (dst <= min_dst) {
						min_dst = dst;
						nrst_indx = ind;
					}
				}
				return nrst_indx;
			}

			bool segment(cv::Mat& image) {
				using namespace cv;
				bool converged = true;
				int cent_ind;
				for(int i = 0; i < image.rows; i++)
					for(int j = 0; j < image.cols; j++) {
						cent_ind = nrst_centroid_index(image, i, j);
						clst_count[cent_ind] += 1;
						if (nb_channels == 1)
							clst_colorsum[cent_ind][0] += image.at<uchar>(i, j);
						else
							for (int ch = 0; ch < nb_channels; ch++)
								clst_colorsum[cent_ind][ch] += image.at<cv::Vec3b>(i, j)[ch];
						if (clustered_img[i * image.cols + j] != cent_ind) {
							clustered_img[i * image.cols + j] = cent_ind;
							converged = false;
						}
					}
				return converged;
			}

			void map_segmentation(cv::Mat& image) {
				using namespace cv ;
				for(int i = 0; i < image.rows; i++)
					for(int j = 0; j < image.cols; j++)
						if (nb_channels == 1)
							image.at<uchar>(i,j) =
								centroids[clustered_img[i * image.cols + j]][0];
						else
							for(int k = 0; k < nb_channels; k++)
								image.at<Vec3b>(i,j)[k] =
									centroids[clustered_img[i * image.cols + j]][k];
			}

			void process(cv::Mat& image) {
				bool converged = false;
				int iter = 0;
				while(!converged && iter < m_maxiter) {
					iter++;
					converged = true;
					// computing centroids
					if (iter == 1) {
						std::cout << "Randomly initializing centroids..." << std::endl;
						init_centroids(image);
					} else {
						std::cout << "Computing new centroids..." << std::endl;
						compute_centroids();
					}
					// nearest centroids computing
					std::cout << "Starting segmentation n." << iter << "..." << std::endl;
					converged = segment(image);
				}
				// change pixels color
				map_segmentation(image);
			}

			void omp_process(cv::Mat& image) {
				bool converged = false;
				int iter = 0;
				while(!converged && iter < m_maxiter) {
					iter++;
					converged = true;
					// computing centroids
					if (iter == 1) {
						std::cout << "Randomly initializing centroids..." << std::endl;
						init_centroids(image);
					} else {
						std::cout << "Computing new centroids..." << std::endl;
						compute_centroids();
					}
					// nearest centroids computing
					std::cout << "Starting segmentation n." << iter << "..." << std::endl;
					converged = segment(image);
				}
				// change pixels color
				map_segmentation(image);
			}

			void tbb_process(cv::Mat& image) {
				bool converged = false;
				int iter = 0;
				while(!converged && iter < m_maxiter) {
					iter++;
					converged = true;
					// computing centroids
					if (iter == 1) {
						std::cout << "Randomly initializing centroids..." << std::endl;
						init_centroids(image);
					} else {
						std::cout << "Computing new centroids..." << std::endl;
						compute_centroids();
					}
					// nearest centroids computing
					std::cout << "Starting segmentation n." << iter << "..." << std::endl;
					converged = segment(image);
				}
				// change pixels color
				map_segmentation(image);
			}
	};
}

#endif /* SRC_IMGPROCESSING_KMEANALGO_H_ */
