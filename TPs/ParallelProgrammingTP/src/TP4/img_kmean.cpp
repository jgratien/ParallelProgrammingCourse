/*
 * main.cpp
 *
 *  Created on: Oct 8, 2019
 *      Author: gratienj
 */

//#include <IMGProcessing/KMeanAlgo.h>

#include <algorithm>
#include <vector>
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
		("openmp",value<int>()->default_value(1), "OPEN_MP")
		("epsilon",value<double>()->default_value(0.9), "Epsilon")
		("kmean",value<int>()->default_value(4), "KMean k value") ;
	variables_map vm;
	store(parse_command_line(argc, argv, desc), vm);
	notify(vm);

	if (vm.count("help"))
	{
		std::cout << desc << "\n";
		return 1;
	}



	/*if(vm["show"].as<int>()==1)
	{
		namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
		imshow( "Display window", image );                   // Show our image inside it.
		waitKey(0);                                          // Wait for a keystroke in the window
		return 0 ;
	}*/

	std::ofstream benchmark;

	
	benchmark.open ("OPEN_MP_Benchmark_Report.csv");
	benchmark << "Kmeans, Time(ms),OPEN_MP\n";
	int kmeans[17] = {1,2,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60};
	for(int iter =0; iter<17; iter++)
	{
		auto start = high_resolution_clock::now();

		std::string img_file = vm["file"].as<std::string>() ;
		Mat image;
		image = imread(img_file.c_str(), CV_LOAD_IMAGE_COLOR);   // Read the file
		int ncols = image.cols;
		int nrows = image.rows;


		const int k = kmeans[iter];

		/*cout<<"NB CHANNELS : "<<image.channels()<<std::endl ;
		  cout<<"NROWS       : "<<image.rows<<std::endl ;
		  cout<<"NCOLS       : "<<image.cols<<std::endl ;*/
		const int ch = image.channels();
		std::vector<uchar> pixels (ncols*nrows*ch);
		if(image.isContinuous())
			pixels.assign(image.datastart, image.dataend);	




		int openmp = vm["openmp"].as<int>();
		PPTP::KMeanAlgo algo(ch,k) ;

		std::vector <double> centroids(ch*k, 0.0) ;
		//cout <<"\nInitialising random centroids by choosing pixels from the image "<<std::endl;	 
#pragma omp parallel for
		for(int j=0; j<k; j++)
		{

			int co = rand()%ncols;
			int ro = rand()%nrows;
			if(ch==3)
			{	Vec3b& rand = image.at<Vec3b>(ro,co);

				centroids[j*ch] = rand[0];
				centroids[j*ch+1] = rand[1];
				centroids[j*ch+2] = rand[2];

			/*	cout<<"\nRed value for randomly initialised centroid number "<< j+1<<" is ="<<centroids[j]<<std::endl; 
				cout<<"Green value for randomly initialised centroid number "<<j+1<<" is ="<<centroids[j+1]<<std::endl; 
				cout<<"Blue value for randomly initialised centroid number "<<j+1<<" is ="<<centroids[j+2]<<std::endl;*/}


			else {
				uchar rand = image.at<uchar>(ro, co);
				centroids[j]= (double)rand;  
			}			
		} 

		int count =0 ;
		double disp = 200;
		const double eps = vm["epsilon"].as<double>();
		const int max = vm["max_it"].as<int>();
		std::vector<double> size(k, 0);


		while(count<max && disp>eps)

		{
			std::vector <double> base(ch*k, 0.0)  ;
		//	cout<<"\n\nParsing through image pixels to check centroid similarity, cycle number: "<<count+1<<std::endl;


#pragma omp declare reduction(vec_double_plus : std::vector<double>: std::transform(omp_out.begin(), omp_out.end(), omp_in.begin(), omp_out.begin(), std::plus<double>()))initializer(omp_priv = omp_orig) 




			for(int i=0; i<k; i++)
			{
				size[i]=0;

			}                        
#pragma omp parallel for collapse(2) reduction(vec_double_plus : base, size) if(openmp==1)
			for(int y=0; y<nrows; y++)
			{
				for(int x=0; x<ncols; x++)
				{
					int index = y*ncols*ch+ch*x;
					int z = algo.nearest_centroid(pixels, centroids, index);


					if(ch==3)
					{base.at(ch*z) += (double)pixels[index];
						base.at(ch*z+1) += (double)pixels[index+1];	                                    
						base.at(ch*z+2) += (double)pixels[index+2];                                 
						size[z]++;}

					else {base.at(ch*z)+=pixels[index];    size[z]++;}	

				}

			}
			cout<<"Done with centroid sums of cycle "<<count+1<<std::endl;
			std::vector <double> new_centroids(ch*k,0.0) ;
			//#pragma omp parallel for if(openmp==1)
			for(int i=0; i<k; i++)

			{
				if(ch==3){	new_centroids[ch*i] = base[ch*i]/size[i] ;
					new_centroids[ch*i+1] = base[ch*i+1]/size[i] ;
					new_centroids[ch*i+2] = base[ch*i+2]/size[i] ; 
					/*cout<<"\nRed value for centroid number "<<i+1<<" of cycle "<<count+1<<" is = "<<new_centroids[ch*i]<<std::endl; 
					cout<<"Green value for centroid number "<<i+1<<" of cycle "<<count+1<<" is = "<<new_centroids[ch*i+1]<<std::endl; 
					cout<<"Blue value for centroid number "<<i+1<<" of cycle "<<count+1<<" is = "<<new_centroids[ch*i+2]<<std::endl;*/}

				else{

					new_centroids[i] = base[i]/size[i] ;

					//cout<<"\n\nGrayscale value of centroid "<<i+1<<" of cycle "<<count+1<<" is = "<<new_centroids[i];
				}

			}

			if(ch==3)
			{	
				unsigned char r = new_centroids[0] - centroids[0];
				unsigned char g = new_centroids[1] - centroids[1];
				unsigned char b = new_centroids[2] - centroids[2];

				disp = (r*r + g*g + b*b);
#pragma omp parallel for if(openmp==1)
				for(int t=1; t<k; t++)
				{
					r = new_centroids[t*ch] - centroids[t*ch];
					g = new_centroids[t*ch+1] - centroids[t*ch+1];
					b = new_centroids[t*ch+2] - centroids[t*ch+2];

					double dispc = (r*r + g*g + b*b) ;
#pragma omp critical 

					if(dispc>disp) 

					{	

						disp=dispc;         } 

				}}


			else {

				unsigned char r = new_centroids[0] - centroids[0];

				disp = r*r;
#pragma omp parallel for if(openmp==1)
				for(int t=1; t<k; t++)
				{
					r = new_centroids[t] - centroids[t];

					double dispc = r*r ;

#pragma omp critical 
					if(dispc>disp) 

					{ 
						disp=dispc;         } 

				}}

			disp = sqrt(disp);

			//cout<<"\n\nLargest Displacement of centroids at cylce "<<count+1<< " = "<<disp<<std::endl;  
#pragma omp parallel for if(openmp==1)
			for(int e=0; e<ch*k; e++)

			{
				centroids.at(e) = new_centroids.at(e);
			}

			count++;
		}
		//cout<<"\n\nKmeans has done its work, now it's  time to display the new image"<<std::endl;

		//#pragma omp parallel for
	/*	for (int i=0; i<k; i++)
		{
			cout<<"\nRed value for FINAL centroid number "<<i+1<<" is = "<<centroids[i*ch]<<std::endl;

			cout<<"Green value for FINAL centroid number "<<i+1<<" is = "<<centroids[i*ch+1]<<std::endl; 
			cout<<"Blue value for FINAL centroid number "<<i+1<<" is = "<<centroids[i*ch+2]<<std::endl; 

			cout<<"Number of pixels belonging to centorid number "<< i+1 <<" is = "<<size[i]<<std::endl;

		}*/         
#pragma omp parallel for collapse(2) if(openmp==1)
		for(int y=0; y<nrows; y++)
		{
			for(int x=0; x<ncols; x++)
			{
				int index = y*ncols*ch+ch*x;
				int z = algo.nearest_centroid(pixels, centroids, index);

				if(ch==3)
				{   Vec3b& pix = image.at<Vec3b>(y,x);
					pix[0] = centroids[ch*z];
					pix[1] = centroids[ch*z+1];
					pix[2] = centroids[ch*z+2];}

				else{
					uchar& pix = image.at<uchar>(y,x);
					pix = centroids[z];}
			}
		}
		auto stop = high_resolution_clock::now(); 
		auto duration = duration_cast<milliseconds>(stop - start); 

		if (openmp == 1) 
			cout <<"\n\nThe process WITH OPEN_MP took "<< duration.count()<<" milliseconds for K = "<<k<< std::endl; 
		//cout << "The OPEN_MP version is "<<acc<<" milliseconds faster than sequential"<< std::endl;

		else cout <<"\n\nThe process WITHOUT OPEN_MP took "<< duration.count()<<" milliseconds with K = "<<k<< std::endl; 

		benchmark<<k<<","<<duration.count()<<",OPEN_MP\n";
		




		
		imwrite("./Seg_Image_OPENMP.jpg",image) ;}

		return 0 ;
	}

