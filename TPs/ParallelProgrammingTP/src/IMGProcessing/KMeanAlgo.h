/*
 * KMeanAlgo.h
 *
 *  Created on: Nov 4, 2020
 *      Author: gratienj
 */

//#ifndef SRC_IMGPROCESSING_KMEANALGO_H_
//#define SRC_IMGPROCESSING_KMEANALGO_H_

/*namespace PPTP
{

	class KMeanAlgo
	{
	public:
		KMeanAlgo(int nb_channels, int nb_centroids)
	        : nb_channels(nb_channels)
		, m_nb_centroid(nb_centroids)
	    {}
		virtual ~KMeanAlgo() {}

		void compute_centroids(cv::Mat const& image)
		{
			using namespace cv ;
			m_centroids.resize(m_nb_centroids*m_nb_channels);


			//INIT INITIAL CENTROID
		


			//COMPUTE NEAREST CENTROID
			for(int i=0; i<image.nrows; i++)

			{
			   for(int j=1;j<image.cols-1;j++)
			   {
			   
			     uchar pixel = image.at<uchar>(i,j) ;
			     
			     // CALCULATE NEAREST CENTROID


			   
			   
			   
			   
			    }
			
			
	


		}

		void compute_segmentation(cv::Mat& image)
		{
                      using namespace cv ;
		      switch(nb_channels)
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
			compute_centroids(image) ;
			compute_segmentation(image) ;
		}

	private :
	    int m_nb_channels = 3 ;
		int m_nb_centroid = 0 ;
		std::vector<uchar> m_centroids ;
	};
}

#endif /* SRC_IMGPROCESSING_KMEANALGO_H_ */
