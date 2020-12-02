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
	    KMeanAlgo(int nb_channels, int nb_centroids, int max_iter)
	    : m_nb_channels(nb_channels)
	    , m_nb_centroids(nb_centroids)
	    , m_max_iter(max_iter)
	    {}
		virtual ~KMeanAlgo() {}

		void compute_centroids(cv::Mat const& image)
		{
		      using namespace cv ;
		      int nearest_centroid_id;

		      // INIT  CENTROIDS
		      //std::cout << "initialize centroids" << std::endl;
		      init_centroids(image);

		      // we'll store centroids corresponding to each pixel in pixel_segmentation
		      std::vector<int> pixel_segmentation(image.cols*image.rows, 0);

		      // COMPUTE CENTROIDS
		      int iter = 0; double epsilon=1;
		      while ( (iter<m_max_iter) && (epsilon>0.05) ) 
		      {
			std::cout << "iteration number: " << iter << std::endl;

			for(int i=0; i<image.rows; i++)
			{
			  for(int j=0; j<image.cols; j++)
			  {
			    switch(m_nb_channels)
			    {
			      case(1):
			      {
				uchar current_pixel = image.at<uchar>(i,j);
				nearest_centroid_id = nearest_centroid(current_pixel);
				pixel_segmentation.at(i*image.cols+j) = nearest_centroid_id;
				break;
			      }
			      case(3):
			      {
				Mat_<Vec3b> _I = image;
                                uchar current_red = _I(i,j)[0];
				uchar current_green = _I(i,j)[1];
				uchar current_blue = _I(i,j)[2];
				nearest_centroid_id = nearest_centroid(current_red, current_green, current_blue);
				pixel_segmentation.at(i*image.cols+j) = nearest_centroid_id;
				break;
			      }
		            }
			  }
			}
			
			// save old centroids before updating them to calculate their variance
			std::vector<uchar> old_centroids = m_centroids;

			// UPDATE CENTROIDS 
			update_centroids(pixel_segmentation, image);
			

			epsilon = compute_variance(old_centroids);
			std::cout << "epsilon: " << epsilon << std::endl;

			iter++;
			std::cout << "-------------------------------" << std::endl;
		      }


		}

		void compute_segmentation(cv::Mat& image)
		{
                      using namespace cv ;
		      switch(m_nb_channels)
		      {
		        case 1:
		          for(int i=1;i<image.rows-1;++i)
		          {
		            for(int j=1;j<image.cols-1;++j)
		            {
		              uchar pixel = image.at<uchar>(i,j);
			      int centroid_id = nearest_centroid(pixel);
			      image.at<uchar>(i,j) = m_centroids.at(centroid_id);
		            }
		          }
		          break ;
		        case 3:
		          Mat_<Vec3b> _I = image;
		          for(int i=1;i<image.rows-1;++i)
		          {
		            for(int j=1;j<image.cols-1;++j)
		            {
		                uchar red = _I(i,j)[0];
				uchar green = _I(i,j)[1];
				uchar blue = _I(i,j)[2];
				int centroid_id = nearest_centroid(red, green, blue);
		        	_I(i,j)[0] = m_centroids.at(3*centroid_id);
				_I(i,j)[1] = m_centroids.at(3*centroid_id+1);
				_I(i,j)[2] = m_centroids.at(3*centroid_id+2);
		            }
		          }
		          break ;
		      }
		}

		void process(cv::Mat& image)
		{
			std::cout << "computation of centroids ... " << std::endl;
			compute_centroids(image) ;
			std::cout << "segmentation starting ... " << std::endl;
			compute_segmentation(image) ;
			std::cout << "end!" << std::endl;
		}

	private :
	    	int m_nb_channels ;
		int m_nb_centroids ;
		int m_max_iter;
		std::vector<uchar> m_centroids ;

		//init
		void init_centroids(cv::Mat const& image)
		{
		  using namespace cv;
		  m_centroids.reserve(m_nb_centroids*m_nb_channels);
		  for(int i=0; i<m_nb_centroids; i++)
		  {
		    int random_pixel;
	            switch(m_nb_channels)
	            {		 
		      case(1):
		      {
		        random_pixel = image.at<uchar>(rand() % image.rows, rand() % image.cols);
			m_centroids.push_back(random_pixel);
	                break;
		      }
		      case(3):
		      {
			Mat_<Vec3b> _I = image;
			int random_line = rand() % image.rows;
			int random_column = rand() % image.cols;
			for(int k=0; k<3; k++)
			{
			  random_pixel = _I(random_line, random_column)[k];
			  m_centroids.push_back(random_pixel); 
			} 
			break;
		      }
 
		    }
		  }
		}

		// nearest centroid 
		int nearest_centroid(uchar red, uchar green=0, uchar blue=0)
		{
		  std::vector<double> distances(m_nb_centroids, 0);
		  double dist=0;
		  for(int i=0; i<m_nb_centroids; i++)
		  {
		    switch(m_nb_channels)
		    {
		      case(1):
		      {
			dist = abs(red - m_centroids[i]);
			distances.at(i) = dist;
		        break;
		      }
		      case(3):
		      {
			double diff_red = pow(red - m_centroids[3*i], 2);
			double diff_green = pow(green - m_centroids[3*i+1], 2);
			double diff_blue = pow(blue - m_centroids[3*i+2], 2);
			dist = sqrt(diff_red + diff_green + diff_blue);
			distances.at(i) = dist;
			break;
		      }
		    }
		  }
		  // determine the minimum distance to pixel and return the index of the centroid ie its id
		  double min = *std::min_element(distances.begin(), distances.end());
		  std::vector<double>::iterator itr = std::find(distances.begin(), distances.end(), min);
		  return std::distance(distances.begin(), itr);
	 	}

		// update centroids
		void update_centroids(std::vector<int> pixel_segmentation, cv::Mat const& image)
		{
		  using namespace cv;
		  for(int j=0; j<m_nb_centroids; j++)
		  {
		    double sum=0, sum_red=0, sum_green=0, sum_blue=0;
		    int count=0;
		    for(int i=0; i<image.cols*image.rows; i++)
		    {
			if(pixel_segmentation[i]==j)
			{
			  switch(m_nb_channels)
			  {
			    case(1):
			    {
			      int index_i = i/image.cols;
			      int index_j = i - image.cols*index_i;
			      sum += (double) image.at<uchar>(index_i,index_j);
			      count++; 
			      break;
		            }
			    case(3):
			    {
			      Mat_<Vec3b> _I = image;
			      // determine indexes i and j of the pixel corresponding to pixel_segmentation[i]
			      int index_i = i/image.cols;
			      int index_j = i - image.cols*index_i;
			      sum_red += (double) _I(index_i,index_j)[0];
			      sum_green += (double) _I(index_i,index_j)[1];
			      sum_blue += (double) _I(index_i,index_j)[2];
			      count++;
			      break;
			    }
			  } 
			}
		    }
		    switch(m_nb_channels)
	            {
		      case(1):
		      {
			double barycenter = (double) sum / count;
			m_centroids[j] =  (uchar) barycenter;
			break;
		      }
		      case(3):
		      {
			double barycenter_red = (double) sum_red / count;
			m_centroids[3*j] = (uchar) barycenter_red;
			double barycenter_green = (double) sum_green / count;
			m_centroids[3*j+1] = (uchar) barycenter_green;
			double barycenter_blue = (double) sum_blue / count;
			m_centroids[3*j+2] = (uchar) barycenter_blue;
			break;
		      }
		    }
		  }
		}
		
		// calculate epsilon
		double compute_variance(std::vector<uchar> old_centroids)
		{
                  double sum=0;
		  for(int i=0; i<m_nb_centroids; i++)
	          {
		    switch(m_nb_channels)
	            {
                      case(1):
		      {
			sum += abs(old_centroids[i] - m_centroids[i]);
			break;
		      }
		      case(3):
		      {
			double d_red = pow(old_centroids[3*i] - m_centroids[3*i], 2);
			double d_green = pow(old_centroids[3*i+1] - m_centroids[3*i+1], 2);
			double d_blue = pow(old_centroids[3*i+2] - m_centroids[3*i+2], 2);
			sum += std::sqrt(d_red + d_green + d_blue);
			break;
		      }
		    }
		  }
		  return (double) sum / m_nb_centroids;

		}
	};
}

#endif /* SRC_IMGPROCESSING_KMEANALGO_H_ */
