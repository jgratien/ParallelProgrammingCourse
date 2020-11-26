/*
 * KMeanAlgo.h
 *
 *  Created on: Nov 4, 2020
 *      Author: gratienj
 */

#ifndef SRC_IMGPROCESSING_KMEANALGO_H_
#define SRC_IMGPROCESSING_KMEANALGO_H_


#include <stdlib.h>
#include <math.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

namespace PPTP
{

	class KMeanAlgo
	{
	public:
		KMeanAlgo(int nb_channels, int nb_centroids)
	    : nb_channels(nb_channels)
		, m_nb_centroid(nb_centroids)
	    {}
		virtual ~KMeanAlgo() {}
		

		std::map<std::tuple<int,int>,int> compute_centroids(cv::Mat const& image)
		{
			using namespace cv ;
	
		      	/*****COMPUTE NEAREST CENTROID*****/
			
			std::map<std::tuple<int,int>, int> collection_centroid_id;
			int centroid_id=0;
			for(int i=0;i<image.rows;i++)
			{
				for(int j=0;j<image.cols;j++)
				{
					// CALCUL NEAREST CENTROID
					centroid_id = calc_closest_centroid(image, i, j);
					collection_centroid_id.emplace(std::make_tuple(i,j),centroid_id);
					
				}
			}

			return collection_centroid_id;
		}

		void compute_segmentation(cv::Mat& image, std::map<std::tuple<int,int>, int>& collection_centroid_id)
		{
                      using namespace cv ;

		      //Mat img_result= image;

		      int cluster_id =0;
		      switch(nb_channels)
		      {
		        case 1:
		          for(std::size_t i=1;i<(size_t)image.rows-1;++i)
		          {
		            for(int j=1;j<image.cols-1;++j)
		            {
		             	//uchar pixel = image.at<uchar>(i,j) ;
				cluster_id=collection_centroid_id[std::make_tuple(i,j)];
			      	image.at<uchar>(i,j)=m_centroids[cluster_id];
		            }
		          }
			  //return img_result;
		          break ;
		        case 3:
		          Mat_<Vec3b> _I = image;
		          for(std::size_t i=1;i<(size_t)image.rows-1;++i)
		          {
		            for(int j=1;j<image.cols-1;++j)
		            {
		              	
				cluster_id=collection_centroid_id[std::make_tuple(i,j)];
				for(int k=0;k<3;++k)
		              	{
		              		//uchar pixel = _I(i,j)[k] ;
					_I(i,j)[k]=m_centroids[3*cluster_id+k];	
		              	}
		            }
		          }
			  //return _I;
		          break ;
		      }
		}

		void process(cv::Mat& image)
		{
			std::cout<<"Start process"<<std::endl;
			std::cout<<"Centroid_init"<<std::endl;
			init_centroid(image);
			
			std::map<std::tuple<int,int>, int> collection_centroid_id;
			std::cout<<"compute centroids"<<std::endl;
			collection_centroid_id=compute_centroids(image) ;
			//Mat img_seg;
			std::cout<<"compute segmentation"<<std::endl; 
			compute_segmentation(image, collection_centroid_id) ;
		}

	private :

		
		void init_centroid(Mat& image){
		      	m_centroids.resize(m_nb_centroid*nb_channels);
		      	//INIT INITTIAL CENTROID

			int row_id=0;
			int col_id=0;
			switch(nb_channels)
			{
				case 1:
				{
					
					row_id=(int)(rand()%image.rows);
					col_id=(int)(rand()%image.cols);
					m_centroids[0]=image.at<uchar>(row_id,col_id);
					for(int i=1;i<(int)m_centroids.size();i++)
					{
						do{
						row_id=(int)(rand()%image.rows);
						col_id=(int)(rand()%image.cols);
						m_centroids[i]=image.at<uchar>(row_id,col_id);
						}while(abs(m_centroids[i]-m_centroids[i-1])>0 && std::find(m_centroids.begin(), m_centroids.end(), m_centroids[i]) != m_centroids.end());
					}
					break;
				}
				case 3: 
				{
						
					row_id=(int)(rand()%image.rows);
					col_id=(int)(rand()%image.cols);
					
					Mat_<Vec3b> _I = image;
					double dist=0;
					
					for(int k=0;k<3;k++){
						m_centroids[k]=_I(row_id,col_id)[k];
					}
					for(int c=1; c<m_nb_centroid; c++)
					{	
						do{
							std::cout<<"looping over "<<c<<std::endl;	
							row_id=(int)(rand()%image.rows);
							col_id=(int)(rand()%image.cols);
							dist=0;
							for(int i=3*c;i<3*c+3;i++)
							{
							
								m_centroids[i]=_I(row_id,col_id)[i];

								dist+=pow(_I(row_id,col_id)[i]-m_centroids[i-3],2);
							}
						}while(sqrt(dist)>0 && std::find(m_centroids.begin(), m_centroids.end(), m_centroids[c]) != m_centroids.end());
						
					}
					break;
				}
			}

		}

		int calc_closest_centroid(cv::Mat const& image, int i, int j){
		
			
			double min_dist=0;
			int id_centroid=0;
			
			switch(nb_channels)
			{
				case 1:
					{	
						uchar pixel = image.at<uchar>(i,j);
						min_dist=abs(pixel-m_centroids[0]);
						for (int c=1; c<m_nb_centroid; c++){
							if(min_dist>abs(pixel-m_centroids[c]))
							{
								min_dist=abs(pixel-m_centroids[c]);
								id_centroid=c;	
							}
						}
						break;
					}
				case 3:
					{
						Mat_<Vec3b> _I = image;	
						for(int k=0;k<3;k++){
							min_dist+=pow(_I(i,j)[k]-m_centroids[k],2);
						}
					
						min_dist=sqrt(min_dist);
						
						double clust_dist=0;	

						for(int c=1; c<m_nb_centroid;c++)
						{
							for(int k=3*c;k<3*c+3;k++)
							{
								clust_dist+=pow(_I(i,j)[k]-m_centroids[k],2);
							}
							clust_dist=sqrt(clust_dist);
							if(min_dist>clust_dist)
							{
								min_dist=clust_dist;
								id_centroid=c;
							}
						}
					break;
					}
			}
			return id_centroid;
		}

	    	int nb_channels  =3  ;
		int m_nb_centroid = 0 ;
		std::vector<uchar> m_centroids ;
	};
}

#endif /* SRC_IMGPROCESSING_KMEANALGO_H_ */
