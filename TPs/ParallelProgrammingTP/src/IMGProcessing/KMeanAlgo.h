/* KMeanAlgo.h
 *
 *  Created on: Nov 4, 2020
 *      Author: gratienj
 */

#ifndef SRC_IMGPROCESSING_KMEANALGO_H_
#define SRC_IMGPROCESSING_KMEANALGO_H_

namespace PPTP
{
	class KMeanAlgo
	{
	public:
		KMeanAlgo(int nb_channels, int nb_centroids)
			: nb_channels(nb_channels),
			  nb_centroids(nb_centroids)
		{
			// Reserve memory space to each vector
			m_total_sum.reserve(nb_channels * nb_centroids);
			m_total_nb.reserve(nb_centroids);
			m_centroids.reserve(nb_centroids * nb_channels);
		}
		virtual ~KMeanAlgo() {}

		void compute_centroids(cv::Mat const &image)
		{
			using namespace cv;

			// INITIATE THE LOOP LIMITS
			int nb_iter = 0;
			double epsilon = 0.00005,
				   norm = 1.0;

			// INITIATE RANDOM CENTROID VALUES

			init_centroids();

			// CONPUTE NEAREST CENTROIDS

			while (nb_iter < 1500 && norm > epsilon)
			{
				// Set the sum and the number vectors to 0
				for (int i = 0; i < nb_centroids * nb_channels; i++)
					m_total_sum[i] = 0;
				for (int i = 0; i < nb_centroids; i++)
					m_total_nb[i] = 0;

				// Fill the sum and number vectors 
				compute_nearest_centroid(m_total_sum, m_total_nb);
				
				// Update the centroids and the norm
				norm = 0.0;
				for (int i = 0; i < nb_centroids; i++)
				{
					if (m_total_nb[i] != 0)
					{
						for (int j = 0; j < nb_channels; j++)
						{
							norm += (abs((double)(m_centroids[nb_channels * i + j] - m_total_sum[nb_channels * i + j])) / 256.00);
							m_centroids[nb_channels * i + j] = m_total_sum[nb_channels * i + j] / m_total_nb[i];
						}
					}
				}

				norm /= (double)(nb_centroids * nb_channels);

				nb_iter++;
			}
			// Update the pixels values in the vector
			update_image_flat(image_flat);
		}

		// COMPUTE SEGMENTATION

		void compute_segmentation(cv::Mat &image)
		{
			// Update the mane image pixels
			using namespace cv;
			switch (nb_channels)
			{
			case 1:
				memcpy(image.data, image_flat.data(), image_flat.size() * sizeof(uchar));
				break;
			case 3:
				Mat_<Vec3b> _I = image;
				memcpy(_I.data, image_flat.data(), image_flat.size() * sizeof(uchar));
				image = _I;
				break;
			}
		}

		void process(cv::Mat &image)
		{
			flatten_image(image);

			compute_centroids(image);
			compute_segmentation(image);

			std::cout << "--------------processed--------------\n";
		}

	private:
		int nb_channels = 3,
			nb_centroids = 1;
		std::vector<uchar> image_flat;
		std::vector<unsigned long> m_total_sum;
		std::vector<int> m_total_nb;
		std::vector<uchar> m_centroids;

		void flatten_image(cv::Mat &image)
		{
			// Transform the image into one single vector
			if (image.isContinuous())
			{
				// If the image data is saved in continious space  
				image_flat.assign((uchar *)image.data,
								  (uchar *)image.data + image.total() * image.channels());
			}
			else
			{
				// If it isn t continiously saved
				for (int i = 0; i < image.rows; ++i)
				{
					image_flat.insert(image_flat.end(), image.ptr<uchar>(i),
									  image.ptr<uchar>(i) + image.cols * image.channels());
				}
			}
		}

		void init_centroids()
		{
			// Allocate random values to initiate the first centroid values
			for (int i = 0; i < nb_centroids * nb_channels; i++)
				m_centroids.push_back((uchar)(rand() % 256));
		}

		double calculate_euclidean_distance(int pixel, int centroid_id)
		{
			// Calculate the euclidean distance between a pixel and a centroid
			double dist = 0;

			for (int i = 0; i < nb_channels; i++)
			{
				dist += pow(image_flat[pixel + i] - m_centroids[centroid_id * nb_channels + i], 2);
			}

			return sqrt(dist);
		}

		void compute_nearest_centroid(std::vector<unsigned long> &sum, std::vector<int> &nb)
		{
			// Sum all the nearest pixels to each centroid in order to recalculate the centroid values
			std::mutex m;
			tbb::parallel_for(tbb::blocked_range<int>(0, image_flat.size() / nb_channels),
							  [&](tbb::blocked_range<int> r) {
								  for (int j = r.begin(); j < r.end(); ++j)
								  {
									  double dist = 999;
									  int centroid_id = 0;
									  //std::cout << "\nWORK TWO\n";

									  for (int i = 0; i < nb_centroids; i++)
									  {
										  double compared_dist = calculate_euclidean_distance(nb_channels * j, i);
										  if (compared_dist < dist)
										  {
											  dist = compared_dist;
											  centroid_id = i;
										  }
									  }
									  m.lock();
									  for (int i = 0; i < nb_channels; i++)
										  sum[centroid_id * nb_channels + i] += image_flat[nb_channels * j + i];
									  nb[centroid_id]++;
									  m.unlock();
								  }
							  }); 
		}

		void update_image_flat(std::vector<uchar> &image_flat)
		{
			tbb::parallel_for(tbb::blocked_range<int>(0, image_flat.size() / nb_channels),
							  [&](tbb::blocked_range<int> r) {
								  for (int j = r.begin(); j < r.end(); ++j)
								  {
									  double dist = 999;
									  int centroid_id = 0;

									  for (int i = 0; i < nb_centroids; i++)
									  {
										  double compared_dist = calculate_euclidean_distance(nb_channels * j, i);
										  if (compared_dist < dist)
										  {
											  dist = compared_dist;
											  centroid_id = i;
										  }
									  }

									  for (int i = 0; i < nb_channels; i++)
										  image_flat[nb_channels * j + i] = m_centroids[centroid_id * nb_channels + i];
								  }
							  });
		}
	};
} // namespace PPTP

#endif /* SRC_IMGPROCESSING_KMEANALGO_H_ */

