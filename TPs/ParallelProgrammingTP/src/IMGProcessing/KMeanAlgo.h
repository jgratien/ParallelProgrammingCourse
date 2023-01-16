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
	    : m_nb_channels(nb_channels)
		, m_nb_centroids(nb_centroids)
	    {}
		virtual ~KMeanAlgo() {}

		void computeCentroids(cv::Mat const& image)
		{
			using namespace cv ;
			m_nearest_centroid.resize(image.rows*image.cols) ;

			bool converged = false ;
			int iter = 0 ;
			while(!converged)
			{
				std::cout<<"computeNearestCentroid"<<std::endl ;
				computeNearestCentroid(image) ;

				std::cout<<"updateCentroid"<<std::endl ;
				double eps = updateCentroid(image) ;

				std::cout<<"ITER : "<<iter<<" EPS="<<eps<<std::endl ;
				converged = eps< 1. ;
				++iter ;
			}

		}

		void computeSegmentation(cv::Mat& image)
		{
			std::cout<<"ComputeSegmentation"<<std::endl ;
              using namespace cv ;
		          int icount = 0 ;
		      switch(m_nb_channels)
		      {
		        case 1:
		          for(std::size_t i=0;i<image.rows;++i)
		          {
		            for(int j=0;j<image.cols;++j)
		            {
			          int ic = m_nearest_centroid[icount] ;
		              image.at<uchar>(i,j) = m_centroids[ic] ;
		              ++icount ;
		            }
		          }
		          break ;
		        case 3:
		          Mat_<Vec3b> _I = image;
		          for(std::size_t i=0;i<image.rows;++i)
		          {
		            for(int j=0;j<image.cols;++j)
		            {
		              int ic = m_nearest_centroid[icount] ;
		              for(int k=0;k<3;++k)
		              {
		                _I(i,j)[k] = m_centroids[3*ic+k] ;
		              }
		              ++icount ;
		            }
		          }
		          break ;
		      }
			std::cout<<"End ComputeSegmentation"<<std::endl ;

		}

		void process(cv::Mat& image)
		{
			std::cout<<"INIT CENTROID"<<std::endl ;
			initCentroids(image) ;

			std::cout<<"COMPUTE CENTROID"<<std::endl ;
			computeCentroids(image) ;

			std::cout<<"COMPUTE SEGMENTATION"<<std::endl ;
			computeSegmentation(image) ;
		}

	private :
		int getNearestCentroid(uchar c1)
		{
			int id = -1 ;
			int d = 255 ;
			for(std::size_t ic=0;ic<m_nb_centroids;++ic)
			{
				int d_ic = std::abs(c1-m_centroids[ic]) ;
				if( d_ic < d)
				{
					d = d_ic ;
					id = ic ;
				}
			}
			return id ;
		}

		int getNearestCentroid(uchar c1, uchar c2, uchar c3)
		{
			int id = -1 ;
			int d = 3*255*255 ;
			for(std::size_t ic=0;ic<m_nb_centroids;++ic)
			{
				int d_ic = (c1 - m_centroids[3*ic])*(c1 - m_centroids[3*ic]) +
						(c2 - m_centroids[3*ic+1])*(c2 - m_centroids[3*ic+1]) +
						(c3 - m_centroids[3*ic+2])*(c3 - m_centroids[3*ic+2]) ;
				if( d_ic < d)
				{
					d = d_ic ;
					id = ic ;
				}
			}
			return id ;
		}

		void initCentroids(cv::Mat const& image)
		{
		  int nb_pixels = image.rows*image.cols ;
		  std::random_device rd;
		  std::mt19937 gen(rd());
		  std::uniform_int_distribution<> dis_pixel(0,nb_pixels);
		  m_centroids.resize(m_nb_channels*m_nb_centroids) ;
		  switch(m_nb_channels)
	      {
		  case 1:
		  {
			  for(std::size_t ic=0;ic<m_nb_centroids;++ic)
			  {
				  int index = dis_pixel(gen) ;
				  int i = index/image.cols ;
				  int j = index%image.cols ;
				  m_centroids[i] = (uchar)  image.at<uchar>(i,j) ;
			  }
		  }
		  break ;
		  case 3:
		  {
		      cv::Mat_<cv::Vec3b> _I = image;
		      for(int k=0;k<3;++k)
			  {
					m_centroids[k] = (uchar)  _I(0,0)[k] ;
			  }
			  std::cout<<"COLOR["<<0<<"]"<<(int)_I(0,0)[0]<<" "<<(int)_I(0,0)[1]<<" "<<(int)_I(0,0)[2]<<std::endl ;

		      int ic = 1 ;
		      while(ic<m_nb_centroids)
		      {
		    	  int icount = 0 ;
				  for(std::size_t i=0;i<image.rows;++i)
				  {
					bool need_break = false ;
					for(int j=0;j<image.cols;++j)
					{
						uchar c0 = _I(i,j)[0] ;
						uchar c1 = _I(i,j)[1] ;
						uchar c2 = _I(i,j)[2] ;
						bool ok = true ;
						for(int i=0;i<ic;++i)
						{
							if( (c0== m_centroids[3*i]) && (c1== m_centroids[3*i+1]) && (c2== m_centroids[3*i+2]) )
							{
								ok =false ;
								break ;
							}
						}
						if(ok)
						{
							m_centroids[3*ic]   = c0 ;
							m_centroids[3*ic+1] = c1 ;
							m_centroids[3*ic+2] = c2 ;
							std::cout<<"COLOR["<<ic<<"]"<<(int)c0<<" "<<(int)c1<<" "<<(int)c2<<std::endl ;
							++ic ;
							need_break = true ;
							break ;
						}
						++icount ;
					}
					if(need_break) break ;
				  }
				  if(icount == image.rows*image.cols)
				  {
					  std::cout<<"TOO MANY CENTROIDS"<<std::endl ;
					  m_nb_centroids = ic;
				  }
		      }
		  }
		  break ;
	      }

		}

		void computeNearestCentroid(cv::Mat const& image)
		{
		   std::cout<<"Compute Nearest Centroid : "<<m_nb_channels<<" "<<image.rows<<" "<<image.cols<<std::endl ;
		   switch(m_nb_channels)
		   {
			case 1:
			{
			  int icount = 0 ;
			  for(std::size_t i=0;i<image.rows;++i)
			  {
				for(int j=0;j<image.cols;++j)
				{
					uchar pixel = image.at<uchar>(i,j) ;
					m_nearest_centroid[icount] = getNearestCentroid(pixel) ;
					++icount ;
				}
			  }
			}
			break ;
			case 3:
			{
			  cv::Mat_<cv::Vec3b> _I = image;
			  int icount = 0 ;
			  for(std::size_t i=0;i<image.rows;++i)
			  {
				for(int j=0;j<image.cols;++j)
				{
					uchar c1 = _I(i,j)[0] ;
					uchar c2 = _I(i,j)[1] ;
					uchar c3 = _I(i,j)[2] ;
					m_nearest_centroid[icount] = getNearestCentroid(c1,c2,c3) ;
  				    ++icount ;
				}
			  }
			}
			break ;
		  }
		  std::cout<<"End Compute Nearest Centroid"<<std::endl ;
		}

		double updateCentroid(cv::Mat const& image)
		{
		   std::cout<<"Update Centroid : "<<m_nb_channels<<" "<<image.rows<<" "<<image.cols<<std::endl ;
		   std::vector<int> new_centroids(m_nb_channels*m_nb_centroids) ;
		   new_centroids.assign(m_nb_channels*m_nb_centroids,0) ;

		   std::vector<int> centroid_size(m_nb_centroids) ;
		   centroid_size.assign(m_nb_centroids,0) ;

		   double eps = 0 ;
		   switch(m_nb_channels)
		   {
			case 1:
			{
			  int icount = 0 ;
			  for(std::size_t i=0;i<image.rows;++i)
			  {
				for(int j=0;j<image.cols;++j)
				{
					int centroid_id = m_nearest_centroid[icount] ;
		            uchar pixel = image.at<uchar>(i,j) ;
		            new_centroids[centroid_id] += pixel ;
		            centroid_size[centroid_id] += 1 ;
		            ++icount ;
				}
			  }
			  for(std::size_t ic=0;ic<m_nb_centroids;++ic)
			  {
				  new_centroids[ic] /= centroid_size[ic] ;
				  double d = new_centroids[ic] - m_centroids[ic] ;
				  eps += d*d ;
				  m_centroids[ic] = (uchar) new_centroids[ic] ;
			  }
			}
			break ;
			case 3:
			{
		      cv::Mat_<cv::Vec3b> _I = image;
		      int icount = 0 ;
			  for(std::size_t i=0;i<image.rows;++i)
			  {
				for(int j=0;j<image.cols;++j)
				{
   	              int centroid_id = m_nearest_centroid[icount] ;
				  //std::cout<<" ICOUNT "<<icount<<" "<<centroid_id<<" "<<centroid_size.size()<<std::endl ; ;

		          centroid_size[centroid_id] += 1 ;
				  for(int k=0;k<3;++k)
				  {
		                uchar pixel = _I(i,j)[k] ;
			            new_centroids[centroid_id*3+k] += pixel ;
				  }
				  ++icount ;
				}
			  }
			  for(std::size_t ic=0;ic<m_nb_centroids;++ic)
			  {
				  std::cout<<" IC "<<ic<<" "<<centroid_size[ic]<<std::endl ;
				  assert(centroid_size[ic] != 0 ) ;
				  for(int k=0;k<3;++k)
				  {
					  assert(centroid_size[ic] != 0 ) ;
					  new_centroids[ic*3+k] /= centroid_size[ic] ;
					  double d = new_centroids[ic*3+k]-m_centroids[ic*3+k] ;
					  eps = d*d ;
					  m_centroids[ic*3+k] = (uchar) new_centroids[ic*3+k] ;
				  }
				  std::cout<<"NEW CENTROID["<<ic<<"] : "<<new_centroids[ic*3]<<" "<<new_centroids[ic*3+1]<<" "<<new_centroids[ic*3+2]<<std::endl ;
			  }
			}
			break ;
		  }
		   std::cout<<" RETURN EPS "<<eps<<std::endl ;
		   return eps ;
		}

	    int m_nb_channels   = 3 ;
		int m_nb_centroids = 0 ;
		std::vector<uchar> m_centroids ;
		std::vector<int>   m_nearest_centroid ;
	};
}

#endif /* SRC_IMGPROCESSING_KMEANALGO_H_ */
