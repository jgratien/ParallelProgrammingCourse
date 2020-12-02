/*
 * main.cpp
 *
 *  Created on: Oct 
 *      Author: gratienj
 */


#include <mpi.h>
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
#include <chrono>

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
    using namespace std::chrono;	
    using namespace boost::program_options ;
    options_description desc;
    desc.add_options()
        ("help", "produce help")
        ("file",value<std::string>(), "image file")
        ("show",value<int>()->default_value(0), "show image")
        ("add-noise",value<int>()->default_value(0), "add noise")
        ("noise-density",value<int>()->default_value(0), "noise density")
        ("kmean",value<int>()->default_value(4), "Kmeans clustering")
	("max_iterations",value<int>()->default_value(50), "Max_number_of_iterations")
	("epsilon",value<double>()->default_value(0.05), "Minimum_displacement")
        ("to-gray",value<int>()->default_value(0), "create gray image") ;
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << "\n";
        return 1;
    }
    
    //using namespace PPTP;
    //Timer timer;

    MPI_Init(&argc,&argv) ;

    int my_rank = 0 ;
    int nb_proc = 1 ;
    MPI_Comm_size(MPI_COMM_WORLD,&nb_proc) ;
    MPI_Comm_rank(MPI_COMM_WORLD,&my_rank) ;

   
    
      std::string img_file = vm["file"].as<std::string>() ;
      Mat image;
      image = imread(img_file.c_str(), CV_LOAD_IMAGE_COLOR);   // Read the file

      if(! image.data )                              // Check for invalid input
      {
          cout <<  "Could not open or find the image" << std::endl ;
          return -1;
      }


         int kmean = vm["kmean"].as<int>();
	 int  k = kmean;
  
     auto start = high_resolution_clock::now();
  if(my_rank==0)
    {

      cout<<"NB CHANNELS : "<<image.channels()<<std::endl ;
      cout<<"NROWS       : "<<image.rows<<std::endl ;
      cout<<"NCOLS       : "<<image.cols<<std::endl  ;  
      const int channels = image.channels() ;  

      //if(vm["kmean"].as<int>       
      /*switch(channels)
        
          case 1:
            for(int i=1;i<image.rows-1;++i)
            {
              for(int j=1;j<image.cols-1;++j)
              {
                
              }
            }
            break ;
          case 3:

	    if(my_rank==0)*/

	       
           
      
      cout<<"Starting the kmeans cycles"<<endl;
		 
		  int ncols = image.cols;
		  int nrows = image.rows;
		  //image.convertTo(image, CV_32F);
		  std::vector<uchar> pixels (ncols*nrows*3);
		  if(image.isContinuous())
                  pixels.assign(image.datastart, image.dataend);
		  
		  int r = nrows%nb_proc;       // Since we will be sending the image data by horizontal chunks we need to see how the remainding r rows will be distributed among the procs (the first r procs will get 1 additional row each)
		  MPI_Bcast(&ncols, 1, MPI_INT, 0, MPI_COMM_WORLD); // Broadcasting the number of columns since all processors will be dealing with the same ncols
		  int begin =0;
		  int nrows_local_proc_zero = nrows/nb_proc;
		  if(r>0) nrows_local_proc_zero++;
		  
		  cout<<"Processor 0 is working with "<<nrows_local_proc_zero<<" rows of the image"<<std::endl;
                  begin = nrows_local_proc_zero*ncols*3;
	

		  for(int i=1;i<nb_proc;i++)
		  {
	              int nrows_local = nrows/nb_proc;
	              if(i<r) nrows_local++;
	              MPI_Send(&nrows_local, 1, MPI_INT, i, 1000, MPI_COMM_WORLD); 
                	  cout<<"Processor "<<i<<" is working with "<<nrows_local<<" rows of the image"<<std::endl;


			  //cv::Vec3b* ptr = image.ptr<cv::Vec3b>(begin,ncols);
		      
                      uchar* ptr = pixels.data() + begin;
                      MPI_Send(ptr, nrows_local*ncols*3, MPI_UNSIGNED_CHAR, i, 2000, MPI_COMM_WORLD);
		      begin += nrows_local*ncols*3 ;

  	  cout<<"Processor "<<i<<" is working with "<<nrows_local*ncols<<" number of pixels"<<std::endl;



	  }

                  // Initialisation of first centroids, these centroids are chosen as random pixels of our image 
	          //std::vector<std::vector <double>> centroids(k, std::vector<double>(3, 0.0))  ; 
		  //	
	std::vector <double> red_centroids (k,0.0);

	std::vector <double> green_centroids (k,0.0);
       	std::vector <double> blue_centroids (k,0.0);
        std::vector<int> size(k, 0);	
	  for(int j=0; j<k; j++)

		  {
		        int co  = rand()%ncols;
			int ro  = rand()%nrows;

			
                        Vec3b& rand = image.at<Vec3b>(ro,co);		   
		        
			red_centroids[j] = rand[0];

   
			green_centroids[j] = rand[1];
		    
 
			blue_centroids[j] = rand[2];

			cout<<"\nRed value of randomly intialised centroid number "<<j<<" is = "<<red_centroids[j];

	                cout<<"\nGreen value of randomly intialised centroid number "<<j<<" is = "<<blue_centroids[j];

	                cout<<"\nBlue value of randomly intialised centroid number "<<j<<" is = "<<green_centroids[j];

		  }
	
	int count=0;
	double disp = 200;
        double epsilon = vm["epsilon"].as<double>();
	double eps = epsilon ;
	int max_it = vm["max_iterations"].as<int>();
	int max = max_it;
        int flag =1;

        

	// while loop that directs our kmeans algorithm
	while (count<max && disp>eps)
	{
            cout<<"\nStarted with cycle number "<<count<<std::endl; 
          // PROCESS DATA ON PROCESSOR NUMBER ZEROmm
	// flag is used here to let other prrocessors know whether the while loop is active or not  
	 
	 
	    
	          MPI_Bcast(&flag, 1, MPI_INT, 0, MPI_COMM_WORLD);
       
		  MPI_Bcast(red_centroids.data(), k, MPI_DOUBLE, 0, MPI_COMM_WORLD);

		  MPI_Bcast(green_centroids.data(), k, MPI_DOUBLE, 0, MPI_COMM_WORLD);

		  MPI_Bcast(blue_centroids.data(), k, MPI_DOUBLE, 0, MPI_COMM_WORLD);


         	std::vector <double> red (k,0.0);

	        std::vector <double> green (k,0.0);
         	std::vector <double> blue (k,0.0);
        
                std::vector<int> size(k,0);
                  
	
          for (int y=0; y<nrows_local_proc_zero; y++)

	  {
	     	  
		  for (int x=0; x<ncols; x++)
	           {
		      double z=0;
		      double sum=0;;
		      Vec3b& pix = image.at<Vec3b>(y,x);
		      double r = pix[0] - red_centroids[0];
		      double g = pix[1] - green_centroids[0];
		      double b = pix[2] - blue_centroids[0];
		      double min = r*r + g*g + b*b;

		      for(int t=1; t<k;t++)
		      {
		      r = pix[0] - red_centroids[t];
		      g = pix[1] - blue_centroids[t];
		      b = pix[2] - green_centroids[t];
		      sum = r*r + g*g + b*b;

                               if (sum<min)
			       {
			         min = sum; z = t;}
		
		      }
		  
		    red[z] += pix[0];  

		    green[z] += pix[1];  

		    blue[z] += pix[2];  
		    size[z]++;

	           }
	  
	  }

            cout<<"successfully processed on processor zero for cycle "<<count<<std::endl;
           //COLLECT RESULT DATA FROM OTHER PROCS AND ADDING CENTROIDS SUMS AND SIZE TO GET TOTAL OF ALL PROCS
	 
          
                 
        // UPDATE CENTROIDS BASED ON Reduced VALUES AND CHECK DISPLACEMENT
	std::vector <double> red_all(k,0.0);

	std::vector <double> green_all (k,0.0);
       	std::vector <double> blue_all (k,0.0);

 	std::vector <int> size_total (k,0.0);

        


	MPI_Reduce(&red[0], &red_all[0], k, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Reduce(&green[0], &green_all[0], k, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);	
	MPI_Reduce(&blue[0], &blue_all[0], k, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
       	MPI_Reduce(&size[0], &size_total[0], k, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	
	std::vector <double> red_new (k,0.0);

	std::vector <double> green_new (k,0.0);
       	std::vector <double> blue_new (k,0.0);
        
		  for(int j=0; j<k; j++)

		  {  
			red_new[j] = red_all[j]/size_total.at(j);

                	green_new[j] = green_all[j]/size_total.at(j);

                	blue_new[j] = blue_all[j]/size_total.at(j);


			cout<<"\nRed value of centroid "<<j+1<<" of cycle number"<<count+1<<" is = "<<red_new[j] ;
                	cout<<"\nGreen value of centroid "<<j+1<<" of cycle number"<<count+1<<" is = "<<green_new[j] ;
                 	cout<<"\nBlue value of centroid "<<j+1<<" of cycle number"<<count+1<<" is = "<<blue_new[j] ;
			cout<<"\nThe number of pixels that belong to this centroid"<<size_total[j];


		  }
		  
		  double rc = fabs(red_new.at(0) - red_centroids.at(0));
		  double gc = fabs(green_new.at(0) - green_centroids.at(0));
		  double bc = fabs(blue_new.at(0) - blue_centroids.at(0));

		  disp = rc*rc + gc*gc + bc*bc;
                  double dispc;
		  for(int a=1; a<k; a++)
		  {
		    	    
		       rc = fabs(red_new.at(a) - red_centroids.at(a));
		       gc = fabs(green_new.at(a) - green_centroids.at(a));
		       bc = fabs(blue_new.at(a) - blue_centroids.at(a));
		          dispc = rc*rc + gc*gc + bc*bc;
                         if (dispc>disp) disp=dispc; // getting the largest displacement between all centroid displacements to check if it's greater than epsilon, we need  all the displacements to be less than epsilon to break out of the kmeans
		  }
                 
		  disp = sqrt(disp);
                 for(int e=0; e<k; e++)
		 {
		   red_centroids.at(e) = red_new.at(e);
          	   green_centroids.at(e) = green_new.at(e);
	           blue_centroids.at(e) = blue_new.at(e);
		 
		 } //    THESE CENTROIDS BECOME THE CENTROIDS WE COMPARE TO IN THE NEXT ITERATION, ALSO THE CENTROIDS WE USE TO SEGMENT THE IMAGE AFTER THE FINAL ITERATION (BREAK-OUT OF ALGO) 



              	count++;     }



	      flag = 0;
	      MPI_Bcast(&flag, 1, MPI_INT, 0, MPI_COMM_WORLD);
        // AFTER BREAKING OUT OF THE KMEAN ALGO THE VECTOR 'centroids' HOLDS THE VALUES OF THE K FINAL CLUSTER CENTROIDS(COLORS THAT WILL APPEAR ON THE SEGMENTED IMAGE)
                  
                
               	  MPI_Bcast(red_centroids.data(), k, MPI_DOUBLE, 0, MPI_COMM_WORLD);

		  MPI_Bcast(green_centroids.data(), k, MPI_DOUBLE, 0, MPI_COMM_WORLD);

		  MPI_Bcast(blue_centroids.data(), k, MPI_DOUBLE, 0, MPI_COMM_WORLD);


	         for(int e=0; e<k; e++)

		 {	cout<<"\n\nRed value of centroid "<<e+1<<" of final cycle is = "<<red_centroids[e] ;
                	cout<<"\n\nGreen value of centroid "<<e+1<<" of final cycle is = "<<green_centroids[e] ;
                 	cout<<"\n\nBlue value of centroid "<<e+1<<" of final cycle is = "<<blue_centroids[e] ; }




               for(int ro=0; ro<nrows_local_proc_zero; ro++)


		  {
		       for(int co=0; co<ncols; co++)
		       {
			
                         Vec3b& img = image.at<Vec3b>(ro,co);		   
		         double sumi =0;
		         int z =0;
		         unsigned char ri = fabs(red_centroids[0] - img[0]);
                         unsigned char gi = fabs(green_centroids[0] - img[1]);
                         unsigned char bi = fabs(blue_centroids[0] - img[2]);
		         double mini = ri*ri + gi*gi + bi*bi;

			 // checking which of the final centroids each pixel of our image belongs to

                          for(int t=1; t<k; t++)
		  	   {
			      ri = fabs(red_centroids[t] - img[0]);
                              gi = fabs(green_centroids[t] - img[1]);
                              bi = fabs(blue_centroids[t] - img[2]);
		              sumi = ri*ri + gi*gi + bi*bi;

			      if (sumi<mini) { mini=sumi; z=t;}
		   
			 
	                   }

			img[0] = (unsigned char) red_centroids[z]; // changing every single pixel's value accordimg to which centroid it belongs to out of the set of final centroids computed
	                img[1] = (unsigned char) green_centroids[z];
                        img[2] = (unsigned char) blue_centroids[z];
		       }

		   
		 }

             int begf = nrows_local_proc_zero;
             for (int i=1; i<nb_proc; i++)
	     {
	           	
	           int nrows_local = nrows/nb_proc;
	              if(i<r) nrows_local++;
                      MPI_Status status;
		      std::vector<double> final_receive(nrows_local*ncols*3);
	              MPI_Recv(final_receive.data(), nrows_local*ncols*3, MPI_DOUBLE, i, 8000, MPI_COMM_WORLD, &status);
		      int starts=0;
		      

		    for (int ro=begf; ro<begf+nrows_local; ro++)
		    {
		        for(int co=0;co<ncols;co++)
			{    
			    Vec3b& img = image.at<Vec3b>(ro,co);	
                            img[0] = final_receive[starts+3*co];
                            img[1] = final_receive[starts+3*co+1];
                            img[2] = final_receive[starts+3*co+2];

			}
                         
                         starts += 3*ncols; 
		    }  
		    begf += nrows_local;  
	     
	     }





          auto stop = high_resolution_clock::now(); 

                                 auto duration = duration_cast<milliseconds>(stop - start); 
  
 
			  cout <<"\n\nThe process took "<< duration.count()<<" milliseconds" << std::endl; 


        imwrite("./Kmeans_Segmented_Image_MPI.jpg",image) ;}        
                 

	
    else
    {
      // MY RANK different 0    
          // RECV IMAGE DATA FROM PROC 0
	 int ncols, nrows_local; 
	 
	 MPI_Bcast(&ncols, 1, MPI_INT, 0, MPI_COMM_WORLD);
	 
	  cout<<"Ncols received by processor "<<my_rank<<" is = "<<ncols<<std::endl;

	 
         MPI_Status status;
	 MPI_Recv(&nrows_local, 1, MPI_INT, 0, 1000, MPI_COMM_WORLD, &status);
	 
	 cout<<"Nrows received by processor "<<my_rank<<" is = "<<nrows_local<<std::endl; 
	 std::vector<uchar> p(ncols*nrows_local*3);
         cout<<"\nPixel vector size for processor "<<my_rank<<" = "<<ncols*nrows_local*3;	  
	 MPI_Recv(p.data(), ncols*nrows_local*3, MPI_UNSIGNED_CHAR, 0, 2000, MPI_COMM_WORLD, &status);
	 cout<<"\nfirst pixel of processor "<<my_rank<<" has these RGB values"<<p[0]<<"  "<<p[1]<<"  "<<p[2];
		 
	 
	 

	      int flag_to_receive;
	      MPI_Bcast(&flag_to_receive, 1, MPI_INT, 0, MPI_COMM_WORLD);


	          std::vector <double> red_receive;
		  red_receive.resize(k);
                  
	          std::vector <double> green_receive;
		  green_receive.resize(k);

                  std::vector <double> blue_receive;
		  blue_receive.resize(k);



         while(flag_to_receive==1)

      {
     	      
		 MPI_Bcast(red_receive.data(), k, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	         
		 MPI_Bcast(green_receive.data(), k, MPI_DOUBLE, 0, MPI_COMM_WORLD);


	         MPI_Bcast(blue_receive.data(), k, MPI_DOUBLE, 0, MPI_COMM_WORLD);

         	std::vector <double> red (k,0.0);

	        std::vector <double> green (k,0.0);
       	        std::vector <double> blue (k,0.0);
                std::vector<int> size(k, 0);
	        int beg =0;	          
                  
	
          for (int y=0; y<nrows_local; y++)

	  {
	     	  
		  for (int j=0; j<ncols; j++)
	           {
		      int z=0;
		      double sum=0;
		      
		      double r = (p.at(beg) - red_receive[0]);
		      double g = (p.at(beg+1) - green_receive[0]);
		      double b = (p.at(beg+2) - blue_receive[0]);
		      double min = r*r + g*g + b*b;

		      for(int t=1; t<k;t++)
		      {
		        r = (p.at(beg) - red_receive[t]);
		        g = (p.at(beg+1) - green_receive[t]);
		        b = (p.at(beg+2) - blue_receive[t]);
		        sum = r*r + g*g + b*b;

                               if (sum<min)
			       {min = sum; z = t;}
		       }
 
		    red[z] += p.at(beg);  

		    green[z] += p.at(beg+1);  

		    blue[z] += p.at(beg+2);

		    size[z]++; beg+=3;

	           }
	  
	  }

	std::vector <double> red_all(k,0.0);
	std::vector <double> green_all (k,0.0);
       	std::vector <double> blue_all (k,0.0);
 	std::vector <int> size_total (k,0.0);

	MPI_Reduce(&red[0], &red_all[0], k, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Reduce(&green[0], &green_all[0], k, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);	
	MPI_Reduce(&blue[0], &blue_all[0], k, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
       	MPI_Reduce(&size[0], &size_total[0], k, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);	


        MPI_Bcast(&flag_to_receive, 1, MPI_INT, 0, MPI_COMM_WORLD);
      
    }


	  
               	  MPI_Bcast(red_receive.data(), k, MPI_DOUBLE, 0, MPI_COMM_WORLD);

		  MPI_Bcast(green_receive.data(), k, MPI_DOUBLE, 0, MPI_COMM_WORLD);

		  MPI_Bcast(blue_receive.data(), k, MPI_DOUBLE, 0, MPI_COMM_WORLD);

                  std::vector<double> final_send(nrows_local*ncols*3);

        int start = 0;
        for (int y=0; y<nrows_local; y++)

	  {
	     	  
		  for (int j=0; j<ncols; j++)
	           {
		      int z=0;
		      double sum=0;
		      
		      double r = (p.at(start) - red_receive[0]);
		      double g = (p.at(start+1) - green_receive[0]);
		      double b = (p.at(start+2) - blue_receive[0]);
		      double min = r*r + g*g + b*b;

		      for(int t=1; t<k;t++)
		      {
		        r = (p.at(start) - red_receive[t]);
		        g = (p.at(start+1) - green_receive[t]);
		        b = (p.at(start+2) - blue_receive[t]);
		        sum = r*r + g*g + b*b;

                               if (sum<min)
			       {min = sum; z = t;}
		       }
 
		    final_send[start] = red_receive[z];

		    final_send[start+1] = green_receive[z];

		    final_send[start+2] = blue_receive[z];
		    start+=3; 
	           }
	  
	  }
             MPI_Send(final_send.data(), nrows_local*ncols*3, MPI_DOUBLE, 0, 8000, MPI_COMM_WORLD);


	      
      
    }  
    
   // break; 
    MPI_Finalize();
    return 0 ;}  

