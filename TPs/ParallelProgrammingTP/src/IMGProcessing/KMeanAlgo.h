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
			MPI_Comm_size(MPI_COMM_WORLD,&nb_procs) ;
  			MPI_Comm_rank(MPI_COMM_WORLD,&my_rank) ;
		}
		virtual ~KMeanAlgo() {}

		void compute_centroids(cv::Mat const &image)
		{
			using namespace cv;

			// Initiate the counters
			std::vector<unsigned long> sum_pix_perCentroid(nb_centroids * nb_channels, 0);
			std::vector<int> nb_pix_perCentroid(nb_centroids, 0);
			
			// Gather/Scatter communication size
			int send_size = nb_channels * (((image.cols) * (image.rows)) / nb_procs);
			
			// Initiate the loop parameters
			int nb_iter = 0;
			double epsilon = 0.00005,
				   norm = 1.0;

			// Reserve memory space for each vector
			image_portion.reserve(send_size);
			sum_pix_perCentroid.reserve(nb_channels*nb_centroids);
			nb_pix_perCentroid.reserve(nb_centroids);
			m_total_sum.reserve(nb_channels*nb_centroids);
			m_total_nb.reserve(nb_centroids);
			m_centroids.reserve(nb_centroids*nb_channels);
			
			// Initiate random centroids values
			if (my_rank == 0)
			{
				init_centroids();
			}
			
			// Send to each thread its part of the image vector 'image_flat'
			MPI_Scatter(image_flat.data(), send_size, MPI_UNSIGNED_CHAR, image_portion.data(),
						 send_size, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
			
			// CONPUTE NEAREST CENTROID
			while (nb_iter < 1500 && norm > epsilon)
			{
				// Send the centroids values from root to other processes
				MPI_Bcast(m_centroids.data(), nb_centroids*nb_channels, MPI_UNSIGNED_CHAR,
						  0, MPI_COMM_WORLD);
				//restart the sum and the number counter of the closest pixel of each centroid 
				for (int i=0; i< nb_centroids*nb_channels; i++) sum_pix_perCentroid[i] = 0;
				for (int i=0; i< nb_centroids; i++) nb_pix_perCentroid[i] = 0;
				
				//count the sum and the number of the closest pixel of each centroid 
				compute_nearest_centroid(sum_pix_perCentroid, nb_pix_perCentroid);
				
				// Gather and sum all the partial vectors sent to the different processes
				MPI_Reduce(sum_pix_perCentroid.data(), m_total_sum.data(), nb_centroids * nb_channels,
						   MPI_UNSIGNED_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
				MPI_Reduce(nb_pix_perCentroid.data(), m_total_nb.data(), nb_centroids,
						   MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

				//Update the norm and centroids value on the root
				if (my_rank == 0)
				{
					norm = 0.0;
					for (int i = 0; i < nb_centroids; i++)
					{
						if (m_total_nb[i] != 0)
						{
							for (int j = 0; j < nb_channels; j++)
							{
								norm += (abs(m_centroids[nb_channels * i + j] - m_total_sum[nb_channels * i + j]) / 256.00);
								m_centroids[nb_channels * i + j] = m_total_sum[nb_channels * i + j] / m_total_nb[i];
							}
						}
					}

					norm /= (double)(nb_centroids * nb_channels);
					nb_iter++;
				}

				// Send the new values to the other processes
				MPI_Bcast(&norm, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);	
				MPI_Bcast(&nb_iter, 1, MPI_INT, 0, MPI_COMM_WORLD);
			}

			// Update each process's image portion by the final values
			update_image_portion(image_portion);
			
			// Gather all the sent portions into the whole image vector on the root
			MPI_Gather(image_portion.data(), send_size, MPI_UNSIGNED_CHAR, image_flat.data(),
						send_size, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
			
		}

		void compute_segmentation(cv::Mat &image)
		{
			// update the values of the real image pixels
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
			if (my_rank == 0)
			{
				compute_segmentation(image);
				std::cout << "\n--------------processed--------------\n";
			}
		}

		// the process rank getter
		int get_proc_rank() {return my_rank;}

	private:
		int nb_channels = 3,
			nb_centroids = 1,
			nb_procs = 1,
			my_rank;
		std::vector<uchar> image_portion;
		std::vector<unsigned long> m_total_sum;
		std::vector<int> m_total_nb;
		std::vector<uchar> m_centroids;
		std::vector<uchar> image_flat;

		void flatten_image(cv::Mat &image)
		{
			// Transforms the image into a single vector 
			if (image.isContinuous())
			{
				image_flat.assign((uchar *)image.data,
								  (uchar *)image.data + image.total() * image.channels());
			}
			else
			{
				for (int i = 0; i < image.rows; ++i)
				{
					image_flat.insert(image_flat.end(), image.ptr<uchar>(i),
									  image.ptr<uchar>(i) + image.cols * image.channels());
				}
			}
		}

		void init_centroids()
		{
			// Initiates random centroid values
			for (int i = 0; i < nb_centroids * nb_channels; i++)
				m_centroids.push_back((uchar)(rand() % 256));
		}

		double calculate_euclidean_distance(int pixel, int centroid_id)
		{
			// Calculates the enclidean distance between a pixel and a centroid
			double dist = 0;

			for (int i = 0; i < nb_channels; i++)
			{
				dist += pow(image_portion[pixel + i] - m_centroids[centroid_id * nb_channels + i], 2);
			}

			return sqrt(dist);
		}

		void compute_nearest_centroid(std::vector<unsigned long> &sum, std::vector<int> &nb)
		{

			for (int j = 0; j < (image_flat.size() / (nb_channels*nb_procs)); j++)
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

				// update the sum and the number of pixels close to the centroid
				for (int i = 0; i < nb_channels; i++)
					sum[centroid_id * nb_channels + i] += image_portion[nb_channels * j + i];
				nb[centroid_id]++;
			}
		}

		void update_image_portion(std::vector<uchar> &image_portion)
		{
			// change the image_portion vector depending on the closest pixel
			for (int j = 0; j < (image_flat.size() / (nb_channels*nb_procs)); j++)
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
					image_portion[nb_channels*j + i] = m_centroids[centroid_id * nb_channels + i];
			}
		}
	};
} // namespace PPTP

#endif /* SRC_IMGPROCESSING_KMEANALGO_H_ */

