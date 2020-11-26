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
#include <IMGProcessing/KMeanAlgo.h>
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
        ("show",value<int>()->default_value(0), "show image")
        ("seg",value<int>()->default_value(1), "kmean segmentation")
	("max_it",value<int>()->default_value(50), "Max_iterations")
	("epsilon",value<double>()->default_value(0.9), "Epsilon")
        ("kmean-value",value<int>()->default_value(4), "KMean k value") ;
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

    if(vm["show"].as<int>()==1)
    {
      namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
      imshow( "Display window", image );                   // Show our image inside it.
      waitKey(0);                                          // Wait for a keystroke in the window
      return 0 ;
    }

    cout<<"NB CHANNELS : "<<image.channels()<<std::endl ;
    cout<<"NROWS       : "<<image.rows<<std::endl ;
    cout<<"NCOLS       : "<<image.cols<<std::endl ;
    const int channels = image.channels();

    //int nb_centroids = vm["nb-centroids"].as<int>() ;
    if(vm["seg"].as<int>()==1)
    {
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
		 /* int kmeans  = vm["kmean"].as<int>();
		  int k = kmeans;
		  cout<<"Value of K is"<<k<<std::endl;*/
		  int k = 2 ;

		  std::vector<std::vector <double>> centroids(k, std::vector<double>(3,0.0)) ;
                  cout <<"\nInitialising random centroids by choosing pixels from the image "<<std::endl;	 
                  //#pragma omp parallel for
		  for(int j=0; j<k; j++)
		  {
		    
		   	  
		    int co = rand()%ncols;
		    int ro = rand()%nrows;

		    Vec3b& rand = image.at<Vec3b>(ro,co);

		    centroids[j].at(0) = rand[0];
		    centroids[j].at(1) = rand[1];
		    centroids[j].at(2) = rand[2];

                                  /* cout<<"\nRed value for randomly initialised centroid number "<< j+1<<" is ="<<centroids[j].at(0)<<std::endl; 
                                   cout<<"Green value for randomly initialised centroid number "<<j+1<<" is ="<<centroids[j].at(1)<<std::endl; 
                                   cout<<"Blue value for randomly initialised centroid number "<<j+1<<" is ="<<centroids[j].at(2)<<std::endl; */

		  
		  
		  } 
		  
		   int count =0 ;
		   double disp = 200;
		   //double epsilon = vm["epsilon"].as<double>();
		   double const eps = 1.00;
		   //int max_it = vm["max_iterations"].as<int>();
		   int max = 100;
	           std::vector<int> size(k, 0);


		   while(count<max && disp>eps)

                       {
		         std::vector<std::vector <double>> new_centroid_sums(k, std::vector<double>(3, 0.0))  ;
			                  	 cout<<"\n\nParsing through image pixels to check centroid similarity, cycle number: "<<count+1<<std::endl;

			 for(int i=0; i<k; i++)
			 {
			   size[i]=0;
			 
			 }


		         //#pragma omp parallel for
			 for(int y=0; y<nrows; y++)
			 {
				 for(int x=0; x<ncols; x++)

		                  {
				     int z=0;
				     double sum=0;
				     Vec3b& pix = image.at<Vec3b>(y,x);
				     unsigned char r = fabs(pix[0] - centroids[0].at(0));
                                     unsigned char g = fabs(pix[1] - centroids[0].at(1));
                                     unsigned char b = fabs(pix[2] - centroids[0].at(2));

				     double min = r*r + g*g + b*b;

                                     for(int t=1; t<k; t++)
				         {
					    r = fabs(pix[0] - centroids[t].at(0));
                                            g = fabs(pix[1] - centroids[t].at(1));
                                            b = fabs(pix[2] - centroids[t].at(2));

                                            sum = r*r + g*g + b*b ;

					    if(sum<min) { min=sum; z=t; }
					 
					 
					 }

                                     //#pragma omp atomic
				     new_centroid_sums[z].at(0) += pix[0];

				     //#pragma omp atomic
                                     new_centroid_sums[z].at(1) += pix[1];	 

                                     //#pragma omp atomic
                                     new_centroid_sums[z].at(2) += pix[2];

                                     //#pragma omp atomic
                                     size[z]++;  
				  }
		       
		 

			 }
		             std::vector<std::vector <double>> new_centroids(k, std::vector<double>(3, 0.0)) ;
                              cout<<"Done with centroid sums of cycle "<<count+1<<std::endl;

                               //#pragma omp parallel for 
			       for(int i=0; i<k; i++)
			       {
			             new_centroids[i].at(0) = new_centroid_sums[i].at(0)/size[i] ;
                                     new_centroids[i].at(1) = new_centroid_sums[i].at(1)/size[i] ;
                                     new_centroids[i].at(2) = new_centroid_sums[i].at(2)/size[i] ; 
                                  /*cout<<"\nRed value for centroid number "<<i+1<<" of cycle "<<count+1<<" is = "<<new_centroids[i].at(0)<<std::endl; 
                                   cout<<"Green value for centroid number "<<i+1<<" of cycle "<<count+1<<" is = "<<new_centroids[i].at(1)<<std::endl; 
                                   cout<<"Blue value for centroid number "<<i+1<<" of cycle "<<count+1<<" is = "<<new_centroids[i].at(2)<<std::endl; */
			     

			       }
 
			      




                	             unsigned char r = fabs(new_centroids[0].at(0) - centroids[0].at(0));
                                     unsigned char g = fabs(new_centroids[0].at(1) - centroids[0].at(1));
                                     unsigned char b = fabs(new_centroids[0].at(2) - centroids[0].at(2));

				     disp = r*r + g*g + b*b;
                                     //#pragma omp parallel for
                                     for(int t=1; t<k; t++)
				         {
					    r = fabs(new_centroids[t].at(0) - centroids[t].at(0));
                                            g = fabs(new_centroids[t].at(1) - centroids[t].at(1));
                                            b = fabs(new_centroids[t].at(2) - centroids[t].at(2));

                                            double dispc = r*r + g*g + b*b ;

					    if(dispc>disp) 
					    
					    { 
					      //#pragma omp atomic read
						    disp=dispc; } 
					 
					 }

				  cout<<"\n\nLargest Displacement of centroids at cylce "<<count+1<< " = "<<disp<<std::endl;  
                                  //#pragma omp parallel for
                                  for(int e=0; e<k; e++)

				     {
				       centroids[e].at(0) = new_centroids[e].at(0); 
                                       centroids[e].at(1) = new_centroids[e].at(1); 
                                       centroids[e].at(2) = new_centroids[e].at(2);
				     
				     }
                                
                                 count++;



		       }

		      

	                 cout<<"\n\nKmeans has done its work, now it's  time to display the new image"<<std::endl;

                         //#pragma omp parallel for
                         for (int i=0; i<k; i++)
			      {
			           cout<<"\nRed value for FINAL centroid number "<<i+1<<" is = "<<centroids[i].at(0)<<std::endl;
				    
                                   cout<<"Green value for FINAL centroid number "<<i+1<<" is = "<<centroids[i].at(1)<<std::endl; 
                                   cout<<"Blue value for FINAL centroid number "<<i+1<<" is = "<<centroids[i].at(2)<<std::endl; 

				   cout<<"Number of pixels belonging to centorid number "<< i+1 <<" is = "<<size[i]<<std::endl;
		    
			      }



		         
                         //#pragma omp parallel for collapse(2)
			 for(int y=0; y<nrows; y++)
			 {
				 for(int x=0; x<ncols; x++)

		                  {
				     int z=0;
				     double sum=0;
				     Vec3b& pix = image.at<Vec3b>(y,x);
				     unsigned char r = fabs(pix[0] - centroids[0].at(0));
                                     unsigned char g = fabs(pix[1] - centroids[0].at(1));
                                     unsigned char b = fabs(pix[2] - centroids[0].at(2));

				     double min = r*r + g*g + b*b;

                                     for(int t=1; t<k; t++)
				         {
					    r = fabs(pix[0] - centroids[t].at(0));
                                            g = fabs(pix[1] - centroids[t].at(1));
                                            b = fabs(pix[2] - centroids[t].at(2));

                                            sum = r*r + g*g + b*b ;

					    if(sum<min) { min=sum; z=t; }
					 
					 
					 }

                                     
				    
				  pix[0] = centroids[z].at(0);
				  pix[1] = centroids[z].at(1);
				  pix[2] = centroids[z].at(2);
				
			
				  
				  }
		       
		 
  	                   }
                             
				 auto stop = high_resolution_clock::now(); 

                                 auto duration = duration_cast<microseconds>(stop - start); 
  
// To get the value of duration use the count() 
// member function on the duration object 
			  cout <<"\n\nThe process took "<< duration.count()<<" microseconds" << std::endl; 



			  

	cout<<"\nNew Image Displayed with "<<k<<" amount of colors"<<std::endl;	 
	
	        imwrite("./Seg_Image.jpg",image) ;

	  }
          break ;
      }

       
    }

    return 0 ;
}

