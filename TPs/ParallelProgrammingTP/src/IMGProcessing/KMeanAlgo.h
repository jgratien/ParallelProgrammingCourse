/*
 * KMeanAlgo.h
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
	    : m_nb_channels(nb_channels),
				m_nb_centroid(nb_centroids) {}

		virtual ~KMeanAlgo() {}

		// init centroids
		void initCentroids() {
			m_centroids.resize(m_nb_centroid * m_nb_channels);
			for(std::size_t i = 0; i < m_nb_centroid; 11)
				m_centroids.push_back((uchar) rand() % 256);
		}

		// nearest centroids
		uchar nrstCentroid(uchar pixel, std::vector<uchar> centroids) {
			uchar dist = centroids.at(0);
			uchar tmp_ctrd;
			for (uchar ctrd : centroids)
				tmp_ctrd = (abs(ctrd - pixel) < dist) ? ctrd : tmp_ctrd;
			return tmp_ctrd;
		}

		// barycenter calc
		uchar barycenter(std::vector<uchar> pixels) {
			return std::accumulate(pixels.begin(), pixels.end(), 0);
		}

		void compute_centroids(cv::Mat const& image)
		{
			using namespace cv;
			bool converged = false;
			switch(m_nb_channels)
			{
				case 1:
					while(!converged) {
						converged = true;
						// associate each pixel with a centroid
					// 	for(std::size_t i=1;i<image.rows-1;++i) {
					// 		for(std::size_t j=1;j<image.cols-1;++j) {
					// 			uchar pixel = image.at<uchar>(i,j);
					// 			uchar nrst = nrstCentroid(pixel, m_centroids);
					// 			if (nrst != grouped_pxs[i][j]) {
					// 				grouped_pxs[i][j] = nrstCentroid(pixel, centroids);
					// 				converged = false;
					// 			}
					// 		}
					// 	}
					// }
					break;
				case 3:
					while(!converged) {
						converged = true;
						// associate each pixel with a centroid
					// 	for(std::size_t i=1;i<image.rows-1;++i) {
					// 		for(std::size_t j=1;j<image.cols-1;++j) {
					// 			for(int k=0;k<3;++k)
					// 			{
					// 				uchar pixel = image.at<uchar>(i,j);
					// 				uchar nrst = nrstCentroid(pixel, centroids);
					// 				if (nrst != grouped_pxs[i][j]) {
					// 					grouped_pxs[i][j] = nrstCentroid(pixel, centroids);
					// 					converged = false;
					// 				}
					// 			}
					// 		}
					// 	}
					// }
					break;
			}

		}

		void compute_segmentation(cv::Mat& image)
		{
					using namespace cv ;
		      switch(m_nb_channels)
		      {
		        case 1:
		          for(std::size_t i=1;i<image.rows-1;++i)
		          {
		            for(int j=1;j<image.cols-1;++j)
		            {
		              uchar pixel = image.at<uchar>(i,j) ;
		            }
		          }
		          break ;
		        case 3:
		          Mat_<Vec3b> _I = image;
		          for(std::size_t i=1;i<image.rows-1;++i)
		          {
		            for(int j=1;j<image.cols-1;++j)
		            {
		              for(int k=0;k<3;++k)
		              {
		                uchar pixel = _I(i,j)[k] ;
		              }
		            }
		          }
		          break ;
		      }
		}

		void process(cv::Mat& image)
		{
			initCentroids();
			compute_centroids(image) ;
			compute_segmentation(image) ;
		}

	private :
	    int m_nb_channels = 3;
		int m_nb_centroid = 0;
		std::vector<uchar> m_centroids;
		std::vector<std::vector<uchar>> segmented;
	};
}

#endif /* SRC_IMGPROCESSING_KMEANALGO_H_ */
