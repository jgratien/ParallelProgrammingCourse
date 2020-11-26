/*
 * main.cpp
 *
 *  Created on: Oct 8, 2019
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

    MPI_Init(&argc,&argv) ;

    int my_rank = 0 ;
    int nb_proc = 1 ;
    MPI_Comm_size(MPI_COMM_WORLD,&nb_proc) ;
    MPI_Comm_rank(MPI_COMM_WORLD,&my_rank) ;

    if(my_rank==0)
    {
      std::string img_file = vm["file"].as<std::string>() ;
      Mat image;
      image = imread(img_file.c_str(), CV_LOAD_IMAGE_COLOR);   // Read the file

      if(! image.data )                              // Check for invalid input
      {
          cout <<  "Could not open or find the image" << std::endl ;
          return -1;
      }

      cout<<"NB CHANNELS : "<<image.channels()<<std::endl ;
      cout<<"NROWS       : "<<image.rows<<std::endl ;
      cout<<"NCOLS       : "<<image.cols<<std::endl ;
      const int channels = image.channels();


      if(vm["kmean"].as<int>()==1)
      {
        switch(channels)
        {
          case 1:
            for(int i=1;i<image.rows-1;++i)
            {
              for(int j=1;j<image.cols-1;++j)
              {
                
              }
            }
            break ;
          case 3:
            Mat_<Vec3b> _I = image;
           
           cout<<"Starting the kmeans cycles"<<endl;
      

      if(vm["kmean"].as<int>()>0)
      {
	          MPI_Init(&argc,&argv);	
		  // SEND IMAGE DATA TO OTHER PROCESSORS SIMILAR TO MATRIX CASE
		 
		  int ncols = image.cols;
		  int nrows = image.rows;
		  
		  int r = nrows%nb_proc;       // Since we will be sending the image data by horizontal chunks we need to see how the remainding r rows will be distributed among the procs (the first r procs will get 1 additional row each)
		  MPI_Bcast(&ncols, 1, MPI_INT, 0, MPI_COMM_WORLD); // Broadcasting the number of columns since all processors will be dealing with the same ncols
		  int begin =0;
		  int nrows_local_proc_zero = nrows/nb_proc;
		  if(r>0) nrows_local_proc_zero++;
		  begin += nrows_local_proc_zero;

		  MPI_Datatype pixel;
		  MPI_Type_contiguous(3, MPI_UNSIGNED_CHAR, &pixel);
		  MPI_Type_commit(&pixel); 

                  MPI_Datatype sums;
                  MPI_Type_contiguous(3, MPI_DOUBLE, &sums);
                  MPI_Type_commit(&sums); 

		  

		  for(int i=1;i<nb_proc;i++)
		  {
	              int nrows_local = nrows/nb_proc;
	              if(i<r) nrows_local++;
	              MPI_Send(&nrows_local, 1, MPI_INT, i, 1000, MPI_COMM_WORLD);
	              cv::Vec3b* ptr = image.ptr<cv::Vec3b>(begin*ncols);

                      MPI_Send(ptr, nrows_local*ncols, pixel, i, 2000, MPI_COMM_WORLD);
		      begin += nrows_local ;
	  }

                  // Initialisation of first centroids, these centroids are chosen as random pixels of our image 

		  //int k = vm["kmean"].as<int>();
	          int k = 4;
	          std::vector<std::vector <unsigned char>> centroids(k, std::vector<unsigned char>(3))  ; 
		          
                  
		  for(int j=0; j<k; j++)

		  {
		        int co  = rand()%ncols;
			int ro  = rand()%nrows;

			
                        Vec3b& rand = image.at<Vec3b>(ro,co);		   
		        
			centroids[j].at(0) = rand[0];

   
			centroids[j].at(1) = rand[1];
		    
 
			centroids[j].at(2) = rand[2];

			cout<<"\nRed value of randomly intialised centroid number "<<j<<" is = "<<centroids[j].at(0);

	                cout<<"\nGreen value of randomly intialised centroid number "<<j<<" is = "<<centroids[j].at(1);

	                cout<<"\nBlue value of randomly intialised centroid number "<<j<<" is = "<<centroids[j].at(0);

		  }
                 
	          
		  MPI_Bcast(centroids.data(), k, pixel, 0, MPI_COMM_WORLD);
	
		
	int count=0;
	double disp = 200;
        //double epsilon = vm["epsilon"].as<double>();
	double eps = 2.0;
	//int max_it = vm["max_iterations"].as<int>();
	int max = 100;

	// while loop that directs our kmeans algorithm
	while (count<max && disp>eps)
	{

        
          // PROCESS DATA ON PROCESSOR NUMBER ZERO 
       

	std::vector<std::vector <double>> new_centroid_sums(k, std::vector<double>(3, 0.0))  ;
        std::vector<int> size(k, 0);	
		          
                  
	
          for (int y=0; y<nrows_local_proc_zero; y++)

	  {
	     	  
		  for (int x=0; x<ncols; x++)
	           {
		      double z=0;
		      double sum=0;
		      Vec3b& pix = image.at<Vec3b>(y,x);
		      unsigned char r = fabs(pix[0] - centroids[0].at(0));
		      unsigned char g = fabs(pix[1] - centroids[0].at(1));
		      unsigned char b = fabs(pix[2] - centroids[0].at(2));
		      double min = r*r + g*g + b*b;

		      for(int t=1; t<k;t++)
		      {
		      r = fabs(pix[0] - centroids[t].at(0));
		      g = fabs(pix[1] - centroids[t].at(1));
		      b = fabs(pix[2] - centroids[t].at(2));
		      sum = r*r + g*g + b*b;

                               if (sum<min)
			       {
			         min = sum; z = t;}
		
		      }
		  
		    new_centroid_sums[z].at(0) += pix[0];  

		    new_centroid_sums[z].at(1) += pix[1];  

		    new_centroid_sums[z].at(2) += pix[2];  
		    size[z]++;

	           }
	  
	  }


           //COLLECT RESULT DATA FROM OTHER PROCS AND ADDING CENTROIDS SUMS AND SIZE TO GET TOTAL OF ALL PROCS
	 
           for (int q=1; q<nb_proc;q++)

	   {
	           std::vector<std::vector <double>> centroid_sums_receive(k, std::vector<double>(3, 0.0))  ;  
		   MPI_Status status;
                   std::vector<int> size_to_receive(k);

		   MPI_Recv(centroid_sums_receive.data(), k, sums, q, 3000, MPI_COMM_WORLD, &status);
		   MPI_Recv(size_to_receive.data(), k, MPI_INT, q, 4000, MPI_COMM_WORLD, &status);

		  for(int h=0; h<k; h++) 

		  {
		    		  
		    new_centroid_sums[h].at(0) += centroid_sums_receive[h].at(0);  

		    new_centroid_sums[h].at(1) += centroid_sums_receive[h].at(1);  

		    new_centroid_sums[h].at(2) += centroid_sums_receive[h].at(2);  

		    size.at(h) += size_to_receive.at(h);
		  
		  }
	   
	   
	   }

        // UPDATE CENTROIDS BASED ON COLLECTED VALUES AND CHECK DISPLACEMENT
	
	      std::vector<std::vector <unsigned char>> new_centroids(k, std::vector<unsigned char>(3))  ; 
		          
                  
		  for(int j=0; j<k; j++)

		  {  
			new_centroids[j].at(0) = new_centroid_sums[j].at(0)/size.at(j);

   

			new_centroids[j].at(1) = new_centroid_sums[j].at(1)/size.at(j);
 		    
 
			new_centroids[j].at(2) = new_centroid_sums[j].at(2)/size.at(j);

			cout<<"\nRed value of centroid "<<j+1<<" of cycle number"<<count+1<<" is = "<<new_centroids[j].at(0) ;
                	cout<<"\nGreen value of centroid "<<j+1<<" of cycle number"<<count+1<<" is = "<<new_centroids[j].at(1) ;
                 	cout<<"\nBlue value of centroid "<<j+1<<" of cycle number"<<count+1<<" is = "<<new_centroids[j].at(2) ;


		  }
                 
	          
		  
		  
		  
		  unsigned char rc = fabs(new_centroids[0].at(0) - centroids[0].at(0));
                  unsigned char gc = fabs(new_centroids[0].at(1) - centroids[0].at(1));
                  unsigned char bc = fabs(new_centroids[0].at(2) - centroids[0].at(2));
		  disp = rc*rc + gc*gc + bc*bc;
                  
		  for(int a=1; a<k; a++)
		  {
		    		  
		         rc = fabs(new_centroids[a].at(0) - centroids[a].at(0));
                         gc = fabs(new_centroids[a].at(1) - centroids[a].at(1));
                         bc = fabs(new_centroids[a].at(2) - centroids[a].at(2));
		         double dispc = rc*rc + gc*gc + bc*bc;
                         if (dispc>disp) disp=dispc; // getting the largest displacement between all centroid displacements to check if it's greater than epsilon, we need  all the displacements to be less than epsilon to break out of the kmeans
		  
		  
		  }
		  
		  
		 MPI_Bcast(new_centroids.data(), k, pixel, 0, MPI_COMM_WORLD);

                 for(int e=0; e<k; e++)
		 {
		   centroids[e].at(0) = new_centroids[e].at(0);
	           centroids[e].at(1) = new_centroids[e].at(1);
		   centroids[e].at(2) = new_centroids[e].at(2);
		 
		 } //    THESE CENTROIDS BECOME THE CENTROIDS WE COMPARE TO IN THE NEXT ITERATION, ALSO THE CENTROIDS WE USE TO SEGMENT THE IMAGE AFTER THE FINAL ITERATION (BREAK-OUT OF ALGO) 

                  

              	count++;     }
        // AFTER BREAKING OUT OF THE KMEAN ALGO THE VECTOR 'centroids' HOLDS THE VALUES OF THE K FINAL CLUSTER CENTROIDS(COLORS THAT WILL APPEAR ON THE SEGMENTED IMAGE)
                  

	         for(int e=0; e<k; e++)

		 {	cout<<"\nRed value of centroid "<<e+1<<" of final cycle is = "<<centroids[e].at(0) ;
                	cout<<"\nGreen value of centroid "<<e+1<<" of final cycle is = "<<centroids[e].at(1) ;
                 	cout<<"\nBlue value of centroid "<<e+1<<" of final cycle is = "<<centroids[e].at(2) ; }




               for(int ro=0; ro<nrows; ro++)


		  {
		       for(int co=0; co<ncols; co++)
		       {
			
                         Vec3b& img = image.at<Vec3b>(ro,co);		   
		         double sumi =0;
		         int z =0;
		         unsigned char ri = fabs(centroids[0].at(0) - img[0]);
                         unsigned char gi = fabs(centroids[0].at(1) - img[1]);
                         unsigned char bi = fabs(centroids[0].at(2) - img[2]);
		         double mini = ri*ri + gi*gi + bi*bi;

			 // checking which of the final centroids each pixel of our image belongs to

                          for(int t=1; t<k; t++)
		  	   {
			      ri = fabs(centroids[t].at(0) - img[0]);
                              gi = fabs(centroids[t].at(1) - img[1]);
                              bi = fabs(centroids[t].at(2) - img[2]);
		              sumi = ri*ri + gi*gi + bi*bi;

			      if (sumi<mini) { mini=sumi; z=t;}
		            }

			img[0] = centroids[z].at(0); // changing every single pixel's value accordimg to which centroid it belongs to out of the set of final centroids computed
	                img[1] = centroids[z].at(1);
                 	img[2] = centroids[z].at(2);
		       }

		   }

        imwrite("./Kmeans_Segmented_Image.jpg",image) ;
          break; }}}
      MPI_Finalize();

    }
    else
    {
      // MY RANK different 0

      //if(vm["parallel-filter"].as<int>()==1)
      {
       
	 MPI_Init(&argc,&argv);	

	     
          // RECV IMAGE DATA FROM PROC 0
	 int ncols, nrows_local;

         MPI_Datatype pixel;
         MPI_Type_contiguous(3, MPI_UNSIGNED_CHAR, &pixel);
         MPI_Type_commit(&pixel); 
         
         
         MPI_Datatype sums;
         MPI_Type_contiguous(3, MPI_DOUBLE, &sums);
         MPI_Type_commit(&sums); 


	 double ncols_to_receive =0;
	 MPI_Bcast(&ncols_to_receive, 1, MPI_INT, 0, MPI_COMM_WORLD);
	 ncols = ncols_to_receive;

	 double nrows_local_receive = 0;
         MPI_Status status;
	 MPI_Recv(&nrows_local_receive, 1, MPI_INT, 0, 1000, MPI_COMM_WORLD, &status);
	 nrows_local = nrows_local_receive;


	 std::vector<std::vector <unsigned char>> p(ncols*nrows_local, std::vector<unsigned char>(3))  ;  
	 MPI_Recv(p.data(), ncols*nrows_local, pixel, 0, 2000, MPI_COMM_WORLD, &status);
		 
	 
	 // Process data from proc zero 
	 int k = vm["kmean"].as<int>();

     	 std::vector<std::vector <unsigned char>> centroids(k, std::vector<unsigned char>(3))  ;  
     
         MPI_Bcast(centroids.data(), k, pixel, 0, MPI_COMM_WORLD);
       

	std::vector<std::vector <double>> new_centroid_sums(k, std::vector<double>(3, 0.0))  ;
        std::vector<int> size(k, 0);	
	int beg =0;	          
                  
	
          for (int y=0; y<nrows_local; y++)

	  {
	     	  
		  for (int j=0; j<ncols; j++)
	           {
		      int z=0;
		      double sum=0;
		      
		      unsigned char r = fabs(p.at(beg)[0] - centroids[0].at(0));
		      unsigned char g = fabs(p.at(beg)[1] - centroids[0].at(1));
		      unsigned char b = fabs(p.at(beg)[2] - centroids[0].at(2));
		      double min = r*r + g*g + b*b;

		      for(int t=1; t<k;t++)
		      {
		        r = fabs(p.at(beg)[0] - centroids[t].at(0));
		        g = fabs(p.at(beg)[1] - centroids[t].at(1));
		        b = fabs(p.at(beg)[2] - centroids[t].at(2));
		        sum = r*r + g*g + b*b;

                               if (sum<min)
			       {min = sum; z = t;}
		       }
 
		    new_centroid_sums[z].at(0) += p.at(beg)[0];  

		    new_centroid_sums[z].at(1) += p.at(beg)[1];  

		    new_centroid_sums[z].at(2) += p.at(beg)[2];

		    size[z]++; beg++;

	           }
	  
	  }

          //SEND RESULT DATA TO PROC 0
         
		 
		   MPI_Send(new_centroid_sums.data(), k, sums, 0, 3000, MPI_COMM_WORLD);
		   MPI_Send(size.data(), k, MPI_INT, 0, 4000, MPI_COMM_WORLD);
		 
        
      } 
        MPI_Finalize(); 
      
    }
    MPI_Finalize();
    return 0 ;
}

