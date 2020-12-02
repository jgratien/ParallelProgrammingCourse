/*
 * KMeanAlgo_parallel_omp.h
 *
 *  Created on: Nov 4, 2020
 *      Author: gratienj
 */

#pragma once

#include <stdlib.h>
#include <math.h>
#include <map>
#include "omp.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

namespace PPTP
{

	class KMeanAlgoParOmp
	{
	public:
		KMeanAlgoParOmp(int nb_channels, int nb_centroids, int nb_iterations)
	    : nb_channels(nb_channels)
		, m_nb_centroid(nb_centroids)
		, nb_iterations(nb_iterations)
	    {}
		virtual ~KMeanAlgoParOmp() {}
		

		bool compute_centroids_omp(cv::Mat const& image)
		{
			using namespace cv ;
	
			bool centroids_convergence=true;
		      	/*****COMPUTE NEAREST CENTROID*****/
			
			int centroid_id=0;
			
			//TO PARALLELISE
			#pragma omp parallel for private(centroid_id)
			for(int i=0;i<image.rows;i++)
			{
				for(int j=0;j<image.cols;j++)
				{
					// CALCUL NEAREST CENTROID
					//blabla#testpragma omp critical
					{	
						centroid_id = calc_closest_centroid(image, i, j);
						//collection_centroid_id.emplace(std::make_tuple(i,j),centroid_id);
						auto pixel_coord = std::make_tuple(i,j);
						if(collection_centroid_id.count(pixel_coord)==0 || collection_centroid_id[pixel_coord]!= centroid_id)
						{
							//#pragma omp critical
							{
								collection_centroid_id[pixel_coord]=centroid_id;
							}
							centroids_convergence=false;
						}
					}
				}
			}
			return centroids_convergence;
		}

		void update_centroids_omp(cv::Mat& image)
		{
			using namespace cv;

			std::vector<int> updated_centroids;
			std::vector<int> centroids_occurences;
			centroids_occurences.resize(m_nb_centroid);
			updated_centroids.resize(m_nb_centroid*nb_channels);
			int i,j;
			switch(nb_channels)
			{
				case 1:
					//TO PARALLELISE
					#pragma omp parallel for private(i) private(j)
					for(unsigned int r = 0; r < collection_centroid_id.size(); r++)
					{
						auto it = collection_centroid_id.begin();
						advance(it, r);
						for(int c=0;c<m_nb_centroid;c++)
						{
							if( it->second == c){
								tie(i, j)=it->first;
								#pragma omp critical
								{
									updated_centroids[c]+=image.at<uchar>(i,j);
									centroids_occurences[c]++;
								}
							}
						}
				
					}

					for(int c=0; c<m_nb_centroid; c++)
					{
						if(centroids_occurences[c] !=0)
							updated_centroids[c]/=centroids_occurences[c];
					}
					break;
				case 3:
					Mat_<Vec3b> _I = image;

					//TO PARALLELSIE
					#pragma omp parallel for private(i) private(j)
					for(unsigned int r = 0; r < collection_centroid_id.size(); r++)
					{
						auto it = collection_centroid_id.begin();
						advance(it,r);


						for(int c=0; c<m_nb_centroid; c++)
						{
							if(it->second == c){
								tie(i,j)=it->first;
								#pragma omp critical
								{
									centroids_occurences[c]++;
									for(int k=3*c;k<3*c +3;k++)
									{
										updated_centroids[k]+=_I(i,j)[k];
									}
								}
							}
						}
					}

					for(int c=0; c<m_nb_centroid; c++)
					{
						if(centroids_occurences[c] !=0)
						{
							for(int k=0;k<nb_channels;k++){
							updated_centroids[c+k]/=centroids_occurences[c];
							}
						}	
					}
					break;

			}
			m_centroids.resize(updated_centroids.size());
			for(int i=0; i<updated_centroids.size(); i++)
			{
				m_centroids[i]=(uchar)updated_centroids[i];
			}
			//m_centroids.swap(updated_centroids);

 		}

		void compute_segmentation_omp(cv::Mat& image)
		{
                      using namespace cv ;


		      int cluster_id =0;
		      switch(nb_channels)
		      {
		        case 1:
			  	//TO PARALLELISE
			  	#pragma omp parallel for private (cluster_id)
		        	for(std::size_t i=0;i<(size_t)image.rows;++i)
		          	{
		            		for(int j=0;j<image.cols;++j)
		           	 	{
						cluster_id=collection_centroid_id[std::make_tuple(i,j)];
			      			image.at<uchar>(i,j)=m_centroids[cluster_id];
		            		}
		          	}
		          	break ;
		        
			case 3:
		       		Mat_<Vec3b> _I = image;
		          	//TO PARALLELISE
			  	#pragma omp parallel for private (cluster_id)
			  	for(std::size_t i=0;i<(size_t)image.rows;++i)
		          	{
		            		for(int j=0;j<image.cols;++j)
		            		{
		              	
						cluster_id=collection_centroid_id[std::make_tuple(i,j)];
						for(int k=0;k<3;++k)
		              			{
							_I(i,j)[k]=m_centroids[3*cluster_id+k];	
		              			}
		            		}
		          	}
		          	break ;
		      }
		}

		void process_omp(cv::Mat& image)
		{
			std::cout<<"Start process"<<std::endl;
			std::cout<<"Centroid_init"<<std::endl;
			init_centroid(image);
			
			//std::map<std::tuple<int,int>, int> collection_centroid_id;
			std::cout<<"compute centroids"<<std::endl;
			int iterations=0;
			bool convergence_centroids=false;
			
			while(iterations<nb_iterations && !convergence_centroids){
				std::cout<<"iteration number: "<<iterations<<std::endl;
				convergence_centroids=compute_centroids_omp(image) ;
				update_centroids_omp(image);
				iterations++;
			}
			//Mat img_seg;
			std::cout<<"compute segmentation"<<std::endl; 
			compute_segmentation_omp(image) ;
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
						}while(abs(m_centroids[i]-m_centroids[i-1])>5 && std::find(m_centroids.begin(), m_centroids.end(), m_centroids[i]) != m_centroids.end());
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
							//std::cout<<"looping over "<<c<<std::endl;	
							row_id=(int)(rand()%image.rows);
							col_id=(int)(rand()%image.cols);
							dist=0;
							for(int i=3*c;i<3*c+3;i++)
							{
							
								m_centroids[i]=_I(row_id,col_id)[i];

								dist+=pow(_I(row_id,col_id)[i]-m_centroids[i-3],2);
							}
						}while(sqrt(dist)>5 && std::find(m_centroids.begin(), m_centroids.end(), m_centroids[c]) != m_centroids.end());
						
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
		int nb_iterations = 50;
		std::vector<uchar> m_centroids ;
		std::map<std::tuple<int,int>,int> collection_centroid_id;
	};
}

