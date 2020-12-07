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
#include <IMGProcessing/KMeanAlgo.h>
#include <iostream>
#include <fstream>
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
		("epsilon",value<double>()->default_value(1.0), "Minimum_displacement")
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
	image = imread(img_file.c_str(), CV_LOAD_IMAGE_COLOR);   // Read the file*/

	if(! image.data )                              // Check for invalid input
	{
		cout <<  "Could not open or find the image" << std::endl ;
		return -1;
	}

	const int ch = image.channels() ;  

	//const int k = vm["kmean"].as<int>();i,
	auto s = std::to_string(nb_proc);
	std::ofstream benchmark;
	benchmark.open (s+"_nbproc_report_MPI_.csv");
	benchmark << "Kmeans, Time(ms),\n";
	int kmeans[11] = {1,2,4,8,12,16,24,28,32,36,40};
	for(int iter =0; iter<11; iter++)
	{ 
		image = imread(img_file.c_str(), CV_LOAD_IMAGE_COLOR);   // Read the file
               	const int k = kmeans[iter];

		auto start = high_resolution_clock::now();
		if(my_rank==0)
		{
 
			/*cout<<"NB CHANNELS : "<<image.channels()<<std::endl ;
			cout<<"NROWS       : "<<image.rows<<std::endl ;
			cout<<"NCOLS       : "<<image.cols<<std::endl  ;*/  
			//const int ch = image.channels() ;  
			PPTP::KMeanAlgo kmean_algo(ch,k);
			//cout<<"Starting the kmeans cycles"<<endl;

			int ncols = image.cols;
			int nrows = image.rows;
			std::vector<uchar> pixels (ncols*nrows*3);
			if(image.isContinuous())
				pixels.assign(image.datastart, image.dataend);

			int r = nrows%nb_proc;       // Since we will be sending the image data by horizontal chunks we need to see how the remainding r rows will be distributed among the procs (the first r procs will get 1 additional row each)
			MPI_Bcast(&ncols, 1, MPI_INT, 0, MPI_COMM_WORLD); // Broadcasting the number of columns since all processors will be dealing with the same ncols
			int begin =0;
			int nrows_local_proc_zero = nrows/nb_proc;
			if(r>0) nrows_local_proc_zero++;

			//cout<<"Processor 0 is working with "<<nrows_local_proc_zero<<" rows of the image"<<std::endl;
			begin = nrows_local_proc_zero*ncols*3;

			for(int i=1;i<nb_proc;i++)
			{
				int nrows_local = nrows/nb_proc;
				if(i<r) nrows_local++;
				MPI_Send(&nrows_local, 1, MPI_INT, i, 1000, MPI_COMM_WORLD); 
				//cout<<"Processor "<<i<<" is working with "<<nrows_local<<" rows of the image"<<std::endl;
				uchar* ptr = pixels.data() + begin;
				MPI_Send(ptr, nrows_local*ncols*ch, MPI_UNSIGNED_CHAR, i, 2000, MPI_COMM_WORLD);
				begin += nrows_local*ncols*ch ;

				//cout<<"Processor "<<i<<" is working with "<<nrows_local*ncols<<" number of pixels"<<std::endl;
			}

			// Initialisation of first centroids, these centroids are chosen as random pixels of our image 	
			std::vector <double> centroids (ch*k,0.0);	
			for(int j=0; j<k; j++)

			{
				int co  = rand()%ncols;
				int ro  = rand()%nrows;
				if(ch==3)	

				{    Vec3b& rand = image.at<Vec3b>(ro,co);
					centroids[ch*j] = rand[0];
					centroids[ch*j+1] = rand[1];
					centroids[ch*j+2] = rand[2];
					/*cout<<"\nRed value of randomly intialised centroid number "<<j<<" is = "<<centroids[3*j];
					cout<<"\nGreen value of randomly intialised centroid number "<<j<<" is = "<<centroids[3*j+1];
					cout<<"\nBlue value of randomly intialised centroid number "<<j<<" is = "<<centroids[3*j+2];*/}

				else{ uchar rand = image.at<uchar>(ro, co);
					centroids[j]= (double)rand;  }
			}

			int count=0;
			double disp = 200;
			double epsilon = vm["epsilon"].as<double>();
			//double eps = epsilon ;
			int max_it = vm["max_iterations"].as<int>();
			//int max = max_it;
			int flag =1;
			// while loop that directs our kmeans alg/orithm
			while (count<max_it && disp>epsilon)
			{
				//cout<<"\nStarted with cycle number "<<count<<std::endl; 
				// PROCESS DATA ON PROCESSOR NUMBER ZEROmm
				// flag is used here to let other prrocessors know whether the while loop is active or not  
				MPI_Bcast(&flag, 1, MPI_INT, 0, MPI_COMM_WORLD);
				MPI_Bcast(centroids.data(), ch*k, MPI_DOUBLE, 0, MPI_COMM_WORLD);
				std::vector <double> base (ch*k);
				std::vector<int> size(k);
				for (int y=0; y<nrows_local_proc_zero; y++)
				{    
					for(int x=0; x<ncols; x++)
					{
						int index = y*ncols*ch+ch*x;
						int z = kmean_algo.nearest_centroid(pixels, centroids, index);

						if (ch==3)
						{
							base[z*ch] += pixels[index];  
							base[z*ch+1] += pixels[index+1];  
							base[z*ch+2] += pixels[index+2];  
							size[z]++;}


						else {
							base[z] += pixels[index];
							size[z]++;
						}
					}
				}
				//COLLECT RESULT DATA FROM OTHER PROCS AND ADDING CENTROIDS SUMS AND SIZE TO GET TOTAL OF ALL PROCS
				// UPDATE CENTROIDS BASED ON Reduced VALUES AND CHECK DISPLACEMENT
				std::vector <double> base_all(ch*k,0.0);
				std::vector <int> size_all(k,0.0);

				MPI_Reduce(base.data(), base_all.data(), ch*k, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
				MPI_Reduce(size.data(), size_all.data(), k, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

				std::vector <double> new_centroids (ch*k,0.0);
				for(int j=0; j<ch*k; j++)
				{  
					new_centroids[j] = base_all[j]/(size_all.at(j/ch));

					//cout <<"\nNew Centroids cycle number "<<count<<"   "<<new_centroids[3*j]<<"   "<<new_centroids[3*j+1]<<"  "<<new_centroids[3*j+2];
				}

				disp = kmean_algo.compute_displacement(new_centroids,centroids);
				//cout<<"\nDisplacement of cylce"<<count<<" = "<<disp<<std::endl;
				for(int e=0; e<ch*k; e++)
				{
					centroids.at(e) = new_centroids.at(e);
				} //    THESE CENTROIDS BECOME THE CENTROIDS WE COMPARE TO IN THE NEXT ITERATION, ALSO THE CENTROIDS WE USE TO SEGMENT THE IMAGE AFTER THE FINAL ITERATION (BREAK-OUT OF ALGO) 
				count++;     
			}
			flag = 0;
			MPI_Bcast(&flag, 1, MPI_INT, 0, MPI_COMM_WORLD);
			// AFTER BREAKING OUT OF THE KMEAN ALGO THE VECTOR 'centroids' HOLDS THE VALUES OF THE K FINAL CLUSTER CENTROIDS(COLORS THAT WILL APPEAR ON THE SEGMENTED IMAGE)
			MPI_Bcast(centroids.data(), ch*k, MPI_DOUBLE, 0, MPI_COMM_WORLD);
			/*for(int e=0; e<k; e++)
			{if (ch==3){	cout<<"\n\nRed value of centroid "<<e+1<<" of final cycle is = "<<+centroids[ch*e] ;
					   cout<<"\n\nGreen value of centroid "<<e+1<<" of final cycle is = "<<+centroids[ch*e+1] ;
					   cout<<"\n\nBlue value of centroid "<<e+1<<" of final cycle is = "<<+centroids[ch*e+2] ; }
			else{
				cout<<"\n\nGrayscale value of centroid "<<e+1<<" of final cycle is = "<<+centroids[e];
			}
			}*/

			for(int y=0; y<nrows_local_proc_zero; y++)
			{
				for(int x=0; x<ncols; x++)
				{       
					int index = y*ncols*ch+ch*x;
					int z = kmean_algo.nearest_centroid(pixels, centroids, index);

					if(ch==3)
					{ Vec3b& img = image.at<Vec3b>(y,x);	
						img[0] = centroids[ch*z]; // changing every single pixel's value accordimg to which centroid it belongs to out of the set of final centroids computed
						img[1] = centroids[ch*z+1];
						img[2] = centroids[ch*z+2];}

					else{
						uchar& pix = image.at<uchar>(y,x);
						pix = centroids[z];
					}
				}
			}

			int begf = nrows_local_proc_zero;
			for (int i=1; i<nb_proc; i++)
			{
				int nrows_local = nrows/nb_proc;
				if(i<r) nrows_local++;
				cout<<"\n   "<<nrows_local;
				MPI_Status status;
				std::vector<double> final_receive(nrows_local*ncols*ch);
				MPI_Recv(final_receive.data(), nrows_local*ncols*ch, MPI_DOUBLE, i, 8000, MPI_COMM_WORLD, &status);
				for (int ro=begf; ro<begf+nrows_local; ro++)
				{
					for(int co=0;co<ncols;co++)
					{      
						int index = (ro-begf)*ncols*ch + co*ch;

						if(ch==3)	
						{Vec3b& img = image.at<Vec3b>(ro,co);	
							//cout<<"   "<<final_receive[index]<<"   "<<final_receive[index+1]<<"   "<<final_receive[index+2];
							img[0] = final_receive[index];
							img[1] = final_receive[index+1];
							img[2] = final_receive[index+2];}

						else {
							uchar& pix = image.at<uchar>(ro,co);
							pix = centroids[index];

						}
					}
				}  
				begf += nrows_local;  
			}

			auto stop = high_resolution_clock::now(); 

			auto duration = duration_cast<milliseconds>(stop - start); 
			cout <<"\n\nThe process took "<< duration.count()<<" milliseconds for nb_proc size "<<nb_proc<<" with K = "<<k<< std::endl;

			benchmark<<k<<","<<duration.count()<<"\n";	
			imwrite("./Kmeans_Segmented_Image_MPI.jpg",image) ;          }        



		else
		{
			// MY RANK different 0    
			// RECV IMAGE DATA FROM PROC 0
			int ncols, nrows_local; 

			MPI_Bcast(&ncols, 1, MPI_INT, 0, MPI_COMM_WORLD); 

			PPTP::KMeanAlgo kmean_algo1(ch,k);
			//cout<<"Ncols received by processor "<<my_rank<<" is = "<<ncols<<std::endl;


			MPI_Status status;
			MPI_Recv(&nrows_local, 1, MPI_INT, 0, 1000, MPI_COMM_WORLD, &status);

			//cout<<"Nrows received by processor "<<my_rank<<" is = "<<nrows_local<<std::endl; 
			std::vector<uchar> p(ncols*nrows_local*ch);
			//cout<<"\nPixel vector size for processor "<<my_rank<<" = "<<ncols*nrows_local*ch;	  
			MPI_Recv(p.data(), ncols*nrows_local*ch, MPI_UNSIGNED_CHAR, 0, 2000, MPI_COMM_WORLD, &status);
			//cout<<"\nfirst pixel of processor "<<my_rank<<" has these RGB values"<<p[0]<<"  "<<p[1]<<"  "<<p[2];




			int flag_to_receive;
			MPI_Bcast(&flag_to_receive, 1, MPI_INT, 0, MPI_COMM_WORLD);

			int size = ch*k;
			std::vector<double> centroids(ch*k,0.0);

			while(flag_to_receive==1)

			{
				MPI_Bcast(centroids.data(), size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
				std::vector <double> base (ch*k,0.0);
				std::vector <int> size(k, 0.0);
				for (int y=0; y<nrows_local; y++)
				{
					for (int x=0; x<ncols; x++)
					{
						int index = y*ncols*ch+ch*x;
						int z = kmean_algo1.nearest_centroid(p, centroids, index);

						if(ch==3)
						{  	base[3*z] += p.at(index);  
							base[3*z+1] += p.at(index+1);  
							base[3*z+2] += p.at(index+2);
							size[z]++;}

						else{  base[z] += p.at(index); size[z]++;  }	
					}
				}

				std::vector <double> base_all(ch*k,0.0);
				std::vector <int> size_all(k,0);

				MPI_Reduce(base.data(),base_all.data(), ch*k, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
				MPI_Reduce(size.data(), size_all.data(), k, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);	
				MPI_Bcast(&flag_to_receive, 1, MPI_INT, 0, MPI_COMM_WORLD);

			}



			MPI_Bcast(centroids.data(), ch*k, MPI_DOUBLE, 0, MPI_COMM_WORLD);
			std::vector<double> final_send(nrows_local*ncols*ch);
			for (int y=0; y<nrows_local; y++)
			{
				for (int x=0; x<ncols; x++)
				{ 
					int index = y*ncols*3+3*x;
					int z = kmean_algo1.nearest_centroid(p, centroids, index);
					if(ch==3){
						final_send[index] = centroids[3*z];
						final_send[index+1] = centroids[3*z+1];
						final_send[index+2] = centroids[3*z+2];}
					else{
						final_send[index] = centroids[z];
					}
				}
			}
			MPI_Send(final_send.data(), nrows_local*ncols*3, MPI_DOUBLE, 0, 8000, MPI_COMM_WORLD);
		}  
	}


	benchmark.close();
	// break; 
	MPI_Finalize();
	return 0 ;}  

