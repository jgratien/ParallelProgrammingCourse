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
			std::string mode;
			int nb_threads;
			int m_nb_centroid;
			int m_maxiter;
			// vector holding all centroids
			std::vector<uchar*> centroids;
			// vector holding iteration-wise centroids
			std::vector<uchar*> new_centroids;
			// color sum of clusters
			std::vector<double*> clst_colorsum;
			// count of elements of clusters
			std::vector<double> clst_count;
			// matrix for clusters mapping
			uchar* clustered_img;

		public:
			KMeanAlgo(int nb_channels, int nb_centroids, int max_iter,
					  std::string mode, int nb_threads)
				: nb_channels(nb_channels)
				, mode(mode)
				, nb_threads(nb_threads)
				, m_nb_centroid(nb_centroids)
				, m_maxiter(max_iter)
			{
				clst_count.assign(nb_centroids * nb_threads, 0);
				for (int i = 0; i < nb_centroids; i++) {
					centroids.push_back(new uchar[nb_channels]);
					new_centroids.push_back(new uchar[nb_channels]);
					for (int j = 0; j < nb_threads; j++)
						clst_colorsum.push_back(new double[nb_channels]);
				}
			}

			virtual ~KMeanAlgo() {
				delete[] clustered_img;
				for (auto p: clst_colorsum) delete[] p;
				for (auto p: centroids) delete[] p;
			}

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
				double temp_count = 0;
				double temp_sum[nb_channels] = { 0 };
				for (int i = 0; i < m_nb_centroid; i++) {
					for (int th = 0; th < nb_threads; th++) {
						temp_count += clst_count[th * m_nb_centroid + i];
						clst_count[th * m_nb_centroid + i] = 0;
						for (int ch = 0; ch < nb_channels; ch++) {
							temp_sum[ch] += clst_colorsum[th * m_nb_centroid + i][ch];
							clst_colorsum[th * m_nb_centroid + i][ch] = 0;
						}
					}
					for (int ch = 0; ch < nb_channels; ch++) {
						new_centroids[i][ch] =
							(int)(temp_sum[ch] / temp_count);
						temp_sum[ch] = 0;
					}
					temp_count = 0;
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

			void segment(cv::Mat& image) {
				using namespace cv;
				int openmp_active = mode == "omp";
				#pragma omp parallel for collapse(2) if (openmp_active)
				for(int i = 0; i < image.rows; i++)
					for(int j = 0; j < image.cols; j++) {
						// get thread id if on parallel mode
						int th = 0;
						if (mode == "omp") th = omp_get_thread_num();

						int cent_ind = nrst_centroid_index(image, i, j);
						clst_count[th * m_nb_centroid + cent_ind] += 1;
						if (nb_channels == 1)
							clst_colorsum[th * m_nb_centroid + cent_ind][0] +=
								image.at<uchar>(i, j);
						else
							for (int ch = 0; ch < nb_channels; ch++)
								clst_colorsum[th * m_nb_centroid + cent_ind][ch] +=
									image.at<cv::Vec3b>(i, j)[ch];
						clustered_img[i * image.cols + j] = cent_ind;
					}
			}

			void map_segmentation(cv::Mat& image) {
				using namespace cv ;
				int openmp_active = mode == "omp";
				#pragma omp parallel for collapse(2) if (openmp_active)
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
				init_centroids(image);
				while (!converged && iter < m_maxiter) {
					// nearest centroids computing
					segment(image);

					// computing centroids
					compute_centroids();

					// verify convergence by comparing
					// old && new centroids
					converged = true;
					int cent = 0, ch;
					while (converged && cent < m_nb_centroid) {
						ch = 0;
						while (converged && ch < nb_channels) {
							converged = centroids[cent][ch] == new_centroids[cent][ch];
							ch++;
						}
						cent++;
					}
					centroids.swap(new_centroids);
					iter++;
				}
				// change pixels color
				std::cout << "Finished in " << iter << " iterations" << std::endl;
				map_segmentation(image);
			}
	};
}

#endif /* SRC_IMGPROCESSING_KMEANALGO_H_ */
