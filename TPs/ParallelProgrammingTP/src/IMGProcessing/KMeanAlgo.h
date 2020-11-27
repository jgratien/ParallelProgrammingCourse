/*
 * KMeanAlgo.h
 *
 *  Created on: Nov 4, 2020
 *      Author: gratienj
 */

#ifndef SRC_IMGPROCESSING_KMEANALGO_H_
#define SRC_IMGPROCESSING_KMEANALGO_H_

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cstdlib>
#include <tuple>

namespace PPTP
{

	class KMeanAlgo
	{
		private :
			int nb_channels;
			int m_nb_centroid;
			int m_maxiter;
			// vector holding all centroids
			std::vector<uchar*> centroids;
			// represents channels sum & count
			// of pixels for each cluster
			std::vector<
				std::tuple<int*, int>
				> reduced_clusters;
			// matrix for clusters mapping
			uchar* clustered_img;

		public:
			KMeanAlgo(int nb_channels, int nb_centroids, int max_iter)
				: nb_channels(nb_channels)
				, m_nb_centroid(nb_centroids)
				, m_maxiter(max_iter)
			{
				for(int i = 0; i < nb_centroids; i++) {
					centroids.push_back(new uchar[nb_channels]);
					reduced_clusters.push_back(
						std::make_tuple(new int[nb_channels], 0));
				}
			}
			virtual ~KMeanAlgo() {}

			void init_centroids(cv::Mat& image) {
				srand(time(0));
				int row, col;
				for (int i = 0; i < m_nb_centroid; i++) {
					row = rand() % image.rows;
					col = rand() % image.cols;
					if (nb_channels == 0)
						centroids[i][0] = image.at<uchar>(row, col);
					else
						for (int j = 0; j < nb_channels; j++)
							centroids[i][j] = image.at<cv::Vec3b>(row, col)[j];
				}
				// init clustered_img matrix
				clustered_img = new uchar[image.rows * image.cols];
			}

			void compute_centroids() {
				int* colorsum;
				int clustercount;
				for (int i = 0; i < m_nb_centroid; i++) {
					colorsum = std::get<0>(reduced_clusters[i]);
					clustercount = std::get<1>(reduced_clusters[i]);
					for (int j = 0; j < nb_channels; j++) {
						centroids[i][j] = (int)(colorsum[j]/clustercount);
						colorsum[j] = 0;
					}
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
						std::get<1>(reduced_clusters[cent_ind]) += 1;
						if (nb_channels == 1)
							std::get<0>(reduced_clusters[cent_ind])[0] += image.at<uchar>(i, j);
						else
							for (int ch = 0; ch < nb_channels; ch++)
								std::get<0>(reduced_clusters[cent_ind])[ch] += image.at<cv::Vec3b>(i, j)[ch];
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
					}
					else {
						std::cout << "Computing new centroids..." << std::endl;
						compute_centroids();
					}
					// nearest centroids computing
					std::cout << "Starting segmentation n." << iter << "..." << std::endl;
					converged = segment(image);
					for(int i=0; i < m_nb_centroid;i++)
						std::cout << (int)centroids[i][0] << " " << (int)centroids [i][1] << " " << (int)centroids[i][2] << std::endl;
				}
				// change pixels color
				map_segmentation(image);
			}
	};
}

#endif /* SRC_IMGPROCESSING_KMEANALGO_H_ */
