/*
 * main.cpp
 *
 *  Created on: Oct 8, 2019
 *      Author: gratienj
 */



#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <random>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/variables_map.hpp>
#include <tbb/parallel_for.h>
#include <tbb/parallel_reduce.h>
#include <chrono>
using namespace std::chrono; 
using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
    using namespace boost::program_options ;
    options_description desc;
    desc.add_options()
        ("help", "produce help")
        ("file",value<std::string>(), "image file")
        ("nx",value<int>()->default_value(0), "NX")
        ("ny",value<int>()->default_value(0), "NY")
        ("tbb",value<int>()->default_value(1), "TBB")
        ("max_it",value<int>()->default_value(50), "MAX_ITERATIONS")
        ("kmean",value<int>()->default_value(4), "KMEAN")
        ("epsilon",value<double>()->default_value(2.0), "EPSILON")
        ("compute-nb-cc",value<int>()->default_value(0), "compute cc")
        ("to-gray",value<int>()->default_value(0), "create gray image") ;
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);
   if (vm.count("help"))
    {
        std::cout << desc << "\n";
        return 1;
    }
    std::string img_file = vm["file"].as<std::string>() ;
    Mat image;
    image = imread(img_file.c_str(), CV_LOAD_IMAGE_COLOR);   // Read the file
    int ncols = image.cols;
    int nrows = image.rows;

    if(! image.data )                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }


    cout<<"NB CHANNELS : "<<image.channels()<<std::endl ;
    cout<<"NROWS       : "<<image.rows<<std::endl ;
    cout<<"NCOLS       : "<<image.cols<<std::endl ;
    const int channels = image.channels();

    
      cout<<"Start of the kmeans process with selected image"<<std::endl;
      switch(channels)
      {
        case 1:
          {
		  //PPTP::KMeanAlgo algo(1,nb_centroids) ;
        	  //algo.process(image) ;
          }
          break ;


                 

        case 3:
          {     	
       		  auto start = high_resolution_clock::now();
		  //PPTP::KMeanAlgo algo(3,nb_centroids) ;
        	  //algo.process(image) ;
		  int k = vm["kmean"].as<int>();
		  //int k = 4;
		  cout<<"Value of K is "<<k<<std::endl;

		  std::vector<std::vector <double>> centroids2(k, std::vector<double>(3,0.0)) ;
                  cout <<"\nInitialising random centroids by choosing pixels from the image "<<std::endl;	 
		  for(int j=0; j<k; j++)
		  {	  
		    int co = rand()%ncols;
		    int ro = rand()%nrows;

		    Vec3b& rand2 = image.at<Vec3b>(ro,co);

		    centroids2[j].at(0) = rand2[0];
		    centroids2[j].at(1) = rand2[1];
		    centroids2[j].at(2) = rand2[2];

                                  cout<<"\nRed value for randomly initialised centroid number "<< j+1<<" is ="<<centroids2[j].at(0)<<std::endl; 
                                   cout<<"Green value for randomly initialised centroid number "<<j+1<<" is ="<<centroids2[j].at(1)<<std::endl; 
                                   cout<<"Blue value for randomly initialised centroid number "<<j+1<<" is ="<<centroids2[j].at(2)<<std::endl; 
		  } 
		  
		   int count =0 ;
		   double disp = 200;
		   double epsilon = vm["epsilon"].as<double>();
		   int max_it = vm["max_it"].as<int>();
	           std::vector<double> size2(k, 0);


		   while(count<max_it && disp>epsilon)

                       {
		         std::vector <double> base(4*k, 0.0)  ;


			 cout<<"\n\nParsing through image pixels to check centroid similarity, cycle number: "<<count+1<<std::endl;
			 for(int i=0; i<k; i++)
			 {
			   size2[i]=0;
			 
			 }
                             
		    std::vector<double> base_all(4*k, 0.0) ;
		    base_all = tbb::parallel_reduce(
                    tbb::blocked_range<int>(0,nrows),
                    std::vector<double>() ,
                    [&](const tbb::blocked_range<int> r, std::vector<double> base)-> std::vector<double>
                  {		    
			 
		  for(int y=r.begin(); y<r.end(); y++)
			 { base.resize(4*k);
				 
			 
                            tbb::parallel_for( tbb::blocked_range<int>(0,ncols),
                       [&](tbb::blocked_range<int> p)

		       { for(int x=p.begin(); x<p.end(); x++)

			     {

				     int z=0;
				     double sum=0;
				     Vec3b& pix = image.at<Vec3b>(y,x);
				     unsigned char r = fabs(pix[0] - centroids2[0].at(0));
                                     unsigned char g = fabs(pix[1] - centroids2[0].at(1));
                                     unsigned char b = fabs(pix[2] - centroids2[0].at(2));

				     double min = r*r + g*g + b*b;

                                     for(int t=1; t<k; t++)
				    	         {
					    r = fabs(pix[0] - centroids2[t].at(0));
                                            g = fabs(pix[1] - centroids2[t].at(1));
                                            b = fabs(pix[2] - centroids2[t].at(2));

                                            sum = r*r + g*g + b*b ;

					    if(sum<min) { min=sum; z=t; }
					 
						 }
					    
                                    //cout<<"\nCentroids number "<<z; 
				     base.at(4*z) += pix[0];
                                     base.at(4*z+1) += pix[1];	                                    
                                     base.at(4*z+2) += pix[2];                                 
                                     base.at(4*z+3)++; 
			     } });
		       		 
			 }
                        return base;
		  },

		    [&](std::vector<double> base_all, std::vector<double> base)->std::vector<double> 
		    
		    {//std::transform (base.begin(), base.end(), base_all.begin(), base.begin(), std::plus<double>());
		        for (int i=0; i<4*k; i++)
			 {
			    base_all.at(i) += base.at(i);
		
			 }		        return base_all;
		    } );
			  //std::plus<std::vector<double>>() );

		             std::vector<std::vector <double>> new_centroids2(k, std::vector<double>(3, 0.0)) ;
                              cout<<"Done with centroid sums of cycle "<<count+1<<std::endl;

			       for(int i=0; i<k; i++)
			       {
			             new_centroids2[i].at(0) = base_all[(channels+1)*i] /base_all[(channels+1)*i+3] ;
                                     new_centroids2[i].at(1) = base_all[(channels+1)*i+1]/base_all[(channels+1)*i+3] ;
                                     new_centroids2[i].at(2) = base_all[(channels+1)*i+2]/base_all[(channels+1)*i+3] ; 
                                     size2[i] = base_all[(channels+1)*i+3];
				     /*cout<<"\nRed value for centroid number "<<i+1<<" of cycle "<<count+1<<" is = "<<new_centroids[i].at(0)<<std::endl; 
                                   cout<<"Green value for centroid number "<<i+1<<" of cycle "<<count+1<<" is = "<<new_centroids[i].at(1)<<std::endl; 
                                   cout<<"Blue value for centroid number "<<i+1<<" of cycle "<<count+1<<" is = "<<new_centroids[i].at(2)<<std::endl; */
			     

			       }
			unsigned char r = fabs(new_centroids2[0].at(0) - centroids2[0].at(0));
                        unsigned char g = fabs(new_centroids2[0].at(1) - centroids2[0].at(1));
                        unsigned char b = fabs(new_centroids2[0].at(2) - centroids2[0].at(2));

				     disp = sqrt(r*r + g*g + b*b);
                                     for(int t=1; t<k; t++)
				         {
					    r = fabs(new_centroids2[t].at(0) - centroids2[t].at(0));
                                            g = fabs(new_centroids2[t].at(1) - centroids2[t].at(1));
                                            b = fabs(new_centroids2[t].at(2) - centroids2[t].at(2));

                                            double dispc = sqrt (r*r + g*g + b*b) ;

					    if(dispc>disp) 
					    
					    { 
					      disp=dispc;         } 
					 
					 }

				  cout<<"\n\nLargest Displacement of centroids at cylce "<<count+1<< " = "<<disp<<std::endl;  
                                  for(int e=0; e<k; e++)

				     {
				       centroids2[e].at(0) = new_centroids2[e].at(0); 
                                       centroids2[e].at(1) = new_centroids2[e].at(1); 
                                       centroids2[e].at(2) = new_centroids2[e].at(2);
				     
				     }
                                
                                 count++;
		       }
	                 cout<<"\n\nKmeans has done its work, now it's  time to display the new image"<<std::endl;

                         //#pragma omp parallel for
                         for (int i=0; i<k; i++)
			      {
			           cout<<"\nRed value for FINAL centroid number "<<i+1<<" is = "<<centroids2[i].at(0)<<std::endl;
				    
                                   cout<<"Green value for FINAL centroid number "<<i+1<<" is = "<<centroids2[i].at(1)<<std::endl; 
                                   cout<<"Blue value for FINAL centroid number "<<i+1<<" is = "<<centroids2[i].at(2)<<std::endl; 

				   cout<<"Number of pixels belonging to centorid number "<< i+1 <<" is = "<<size2[i]<<std::endl;
		    
			      }

                              tbb::parallel_for( tbb::blocked_range<int>(0,nrows),
                       [&](tbb::blocked_range<int> r)
		       {
			 for(int y=r.begin(); y<r.end(); y++)
			 {
                                        


                              tbb::parallel_for( tbb::blocked_range<int>(0,ncols),
                       [&](tbb::blocked_range<int> p)
                             {
				 for(int x=p.begin(); x<p.end(); x++)
                             {
				     int z=0;
				     double sum=0;
				     Vec3b& pix = image.at<Vec3b>(y,x);
                                     unsigned char r = fabs(pix[0] - centroids2[0].at(0));
                                     unsigned char g = fabs(pix[1] - centroids2[0].at(1));
                                     unsigned char b = fabs(pix[2] - centroids2[0].at(2));

				     double min = r*r + g*g + b*b;

                                     for(int t=1; t<k; t++)
				         {
					    r = fabs(pix[0] - centroids2[t].at(0));
                                            g = fabs(pix[1] - centroids2[t].at(1));
                                            b = fabs(pix[2] - centroids2[t].at(2));

                                            sum = r*r + g*g + b*b ;

					    if(sum<min) { min=sum; z=t; }
					 
					 
					 }

                                     
				    
				  pix[0] = (unsigned char)centroids2[z].at(0);
				  pix[1] = (unsigned char)centroids2[z].at(1);
				  pix[2] = (unsigned char)centroids2[z].at(2);
				  
				  } });
  	                   }});
                            	auto stop = high_resolution_clock::now(); 
                                auto duration2 = duration_cast<milliseconds>(stop - start); 
			        
				 cout <<"\n\nThe process WITH TBB  took "<< duration2.count()<<" milliseconds" << std::endl; 


			  

                cout<<"\nNew Image Displayed with "<<k<<" amount of colors"<<std::endl;	 
	
	        imwrite("./Seg_ImageTBB.jpg",image) ;

	  
          
                        }

          break ;
      }

       
    

    return 0 ; }
