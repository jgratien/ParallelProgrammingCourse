/*
 * KMeanAlgoTbb.h
 *
 *  
 *      Author: Mariem Ben Charrada
 */

#ifndef SRC_IMGPROCESSING_KMEANALGOTBB_H_
#define SRC_IMGPROCESSING_KMEANALGOTBB_H_


namespace PPTP
{

	class KMeanAlgoTbb
	{
	public:
	    KMeanAlgoTbb(int nb_channels, int nb_centroids, int max_iter)
	    : m_nb_channels(nb_channels)
	    , m_nb_centroids(nb_centroids)
	    , m_max_iter(max_iter)
	    {}
		virtual ~KMeanAlgoTbb() {}

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
			{
		        	tbb::parallel_for(tbb::blocked_range<int>(0, image.rows),
				  [&](tbb::blocked_range<int> r)
				  {
		  			for(int i=r.begin(); i<r.end(); i++)
		  			{
		   
						{
		        				tbb::parallel_for(tbb::blocked_range<int>(0, image.cols),
				  			  [&](tbb::blocked_range<int> s)
				  			  {
		  						for(int j=s.begin(); j<s.end(); j++)
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
			  
				  			});
						}
					}
			
				  });
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
			{
		          {
		          tbb::parallel_for(tbb::blocked_range<int>(1, image.rows-1),
				  [&](tbb::blocked_range<int> r)
				  {
		  			for(int i=r.begin(); i<r.end(); i++)
		  			{
		   			  {
		        		    tbb::parallel_for(tbb::blocked_range<int>(1, image.cols-1),
				  		[&](tbb::blocked_range<int> s)
				  		  {
		  						for(int j=s.begin(); j<s.end(); j++)
		  						{
		              					  uchar pixel = image.at<uchar>(i,j);
			      					  int centroid_id = nearest_centroid(pixel);
			      					  image.at<uchar>(i,j) = m_centroids.at(centroid_id);
		            					}
		          			  });
					  }
					}
				  });
			  }
		          break ;
			}
		        case 3:
			{
		          Mat_<Vec3b> _I = image;
			  {
		          tbb::parallel_for(tbb::blocked_range<int>(1, image.rows-1),
				  [&](tbb::blocked_range<int> r)
				  {
		  			for(int i=r.begin(); i<r.end(); i++)
		  			{
		   			  {
		        		    tbb::parallel_for(tbb::blocked_range<int>(1, image.cols-1),
				  		[&](tbb::blocked_range<int> s)
				  		  {
		  						for(int j=s.begin(); j<s.end(); j++)
		  						{
		              					  uchar red = _I(i,j)[0];
								  uchar green = _I(i,j)[1];
								  uchar blue = _I(i,j)[2];
								  int centroid_id = nearest_centroid(red, green, blue);
		        					  _I(i,j)[0] = m_centroids.at(3*centroid_id);
								  _I(i,j)[1] = m_centroids.at(3*centroid_id+1);
								  _I(i,j)[2] = m_centroids.at(3*centroid_id+2);

		            					}
		          			  });
					  }
					}
				  });
			  }
		          break ;
		      }
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
		  uchar random_pixel;
		  
	            switch(m_nb_channels)
	            {		 
		      case(1):
		      {
			{
		        tbb::parallel_for(tbb::blocked_range<int>(0, m_nb_centroids),
				  [&](tbb::blocked_range<int> r)
				  {
		  			for(int i=r.begin(); i<r.end(); i++)
		  			{
		   
		        			random_pixel = image.at<uchar>(rand() % image.rows, rand() % image.cols);
						m_centroids.push_back(random_pixel);
	                			
		      			}
				  });
			 }
			 break;
		      }
		      case(3):
		      {
			{
			tbb::parallel_for(tbb::blocked_range<int>(0, m_nb_centroids),
				  [&](tbb::blocked_range<int> r)
				  {
		  			for(int i=r.begin(); i<r.end(); i++)
		  			{

						Mat_<Vec3b> _I = image;
						int random_line = rand() % image.rows;
						int random_column = rand() % image.cols;
						for(int k=0; k<3; k++)
						{
			  				random_pixel = _I(random_line, random_column)[k];
			  				m_centroids.push_back(random_pixel); 
						} 
					}
				  });
			}
			break;
		      }
		    }			  
		}

		// nearest centroid 
		int nearest_centroid(uchar red, uchar green=0, uchar blue=0)
		{
		  std::vector<double> distances(m_nb_centroids, 0);
		  double dist=0;
		    switch(m_nb_channels)
		    {
		      case(1):
		      {
		  			for(int i=0; i<m_nb_centroids; i++)
		  			{

						dist = abs(red - m_centroids[i]);
						distances.at(i) = dist;
					}
				
			
		        break;
		      }
		      case(3):
		      {
			
		  			for(int i=0; i<m_nb_centroids; i++)
		  			{

						double diff_red = pow(red - m_centroids[3*i], 2);
						double diff_green = pow(green - m_centroids[3*i+1], 2);
						double diff_blue = pow(blue - m_centroids[3*i+2], 2);
						dist = sqrt(diff_red + diff_green + diff_blue);
						distances.at(i) = dist;
					}
						
			 
			break;
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
		  {
		    tbb::parallel_for(tbb::blocked_range<int>(0, m_nb_centroids),
		      [&](tbb::blocked_range<int> r)
		        {
		  	  for(int j=r.begin(); j<r.end(); j++)
		  	  {
		    
			  switch(m_nb_channels)
			  {
			    case(1):
			    {

				double sum = tbb::parallel_reduce(tbb::blocked_range<int>(0, image.cols*image.rows), 0.0,
				  [&](tbb::blocked_range<int> r, double sum)
				  {
		    			for(int i=r.begin(); i<r.end(); i++)
	          			{
		    				if(pixel_segmentation[i]==j)
						{

			      			
			      			int index_i = i/image.cols;
			      			int index_j = i - image.cols*index_i;
			      			sum += (double) image.at<uchar>(index_i, index_j);
						}

		  			}
		  			return sum;
				  }, std::plus<double>() );

				int count = tbb::parallel_reduce(tbb::blocked_range<int>(0, image.cols*image.rows), 0,
				  [&](tbb::blocked_range<int> r, int count)
				  {
		    			for(int i=r.begin(); i<r.end(); i++)
	          			{
		    				if(pixel_segmentation[i]==j)
						{
							count++;
						}

		  			}
		  			return count;
				  }, std::plus<int>() );

				double barycenter = (double) sum / count;
				m_centroids[j] =  (uchar) barycenter;

			      break;
		            }
			    case(3):
			    {
				Mat_<Vec3b> _I = image;
				double sum_red = tbb::parallel_reduce(tbb::blocked_range<int>(0, image.cols*image.rows), 0.0,
				  [&](tbb::blocked_range<int> r, double sum_red)
				  {
		    			for(int i=r.begin(); i<r.end(); i++)
	          			{
		    				if(pixel_segmentation[i]==j)
						{

			      			
			      			int index_i = i/image.cols;
			      			int index_j = i - image.cols*index_i;
			      			sum_red += (double) _I(index_i,index_j)[0];
						}

		  			}
		  			return sum_red;
				  }, std::plus<double>() );

				double sum_green = tbb::parallel_reduce(tbb::blocked_range<int>(0, image.cols*image.rows), 0.0,
				  [&](tbb::blocked_range<int> r, double sum_green)
				  {
		    			for(int i=r.begin(); i<r.end(); i++)
	          			{
		    				if(pixel_segmentation[i]==j)
						{

			      			
			      			int index_i = i/image.cols;
			      			int index_j = i - image.cols*index_i;
			      			sum_green += (double) _I(index_i,index_j)[1];
						}

		  			}
		  			return sum_green;
				  }, std::plus<double>() );

				double sum_blue = tbb::parallel_reduce(tbb::blocked_range<int>(0, image.cols*image.rows), 0.0,
				  [&](tbb::blocked_range<int> r, double sum_blue)
				  {
		    			for(int i=r.begin(); i<r.end(); i++)
	          			{
		    				if(pixel_segmentation[i]==j)
						{

			      			
			      			int index_i = i/image.cols;
			      			int index_j = i - image.cols*index_i;
			      			sum_blue += (double) _I(index_i,index_j)[2];
						}

		  			}
		  			return sum_blue;
				  }, std::plus<double>() );

				int count = tbb::parallel_reduce(tbb::blocked_range<int>(0, image.cols*image.rows), 0,
				  [&](tbb::blocked_range<int> r, int count)
				  {
		    			for(int i=r.begin(); i<r.end(); i++)
	          			{
		    				if(pixel_segmentation[i]==j)
						{
							count++;
						}

		  			}
		  			return count;
				  }, std::plus<int>() );



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
		    });
		  }
		      
		}
		
		// calculate epsilon
		double compute_variance(std::vector<uchar> old_centroids)
		{
                  //double sum=0;
		  double sum = tbb::parallel_reduce(tbb::blocked_range<int>(0, m_nb_centroids), 0.0,
				  [&](tbb::blocked_range<int> r, double sum)
				  {
		    			for(int i=r.begin(); i<r.end(); i++)
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
		  			return sum;
				  }, std::plus<double>() );
		  return (double) sum / m_nb_centroids;

		}
	};
}

#endif /* SRC_IMGPROCESSING_KMEANALGOTBB_H_ */
