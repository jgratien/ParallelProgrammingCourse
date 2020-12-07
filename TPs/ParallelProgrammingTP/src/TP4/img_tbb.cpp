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

	using namespace std;


/*	if(! image.data )                              // Check for invalid input
	{
		cout <<  "Could not open or find the image" << std::endl ;
		return -1;
	}*/



	std::ofstream benchmark;
	benchmark.open ("TBB_Benchmark_Report.csv");
	benchmark << "Kmeans, Time(ms),TBB\n";
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


		//cout<<"Start of the kmeans process with selected image"<<std::endl;
		//int k = vm["kmean"].as<int>();
		//cout<<"Value of K is "<<k<<std::endl;

		PPTP::KMeanAlgo algo(ch,k) ;

		std::vector <double> centroids(ch*k, 0.0) ;
		//cout <<"\nInitialising random centroids by choosing pixels from the image "<<std::endl;	 
		for(int j=0; j<k; j++)
		{	  
			int co = rand()%ncols;
			int ro = rand()%nrows;
			if(ch==3)
			{	Vec3b& rand = image.at<Vec3b>(ro,co);

				centroids[j*ch] = rand[0];
				centroids[j*ch+1] = rand[1];
				centroids[j*ch+2] = rand[2];

				/*cout<<"\nRed value for randomly initialised centroid number "<< j+1<<" is ="<<centroids[j*ch]<<std::endl; 
				  cout<<"Green value for randomly initialised centroid number "<<j+1<<" is ="<<centroids[j*ch+1]<<std::endl; 
				  cout<<"Blue value for randomly initialised centroid number "<<j+1<<" is ="<<centroids[j*ch+2]<<std::endl;*/}


			else {
				uchar rand = image.at<uchar>(ro, co);
				centroids[j]= (double)rand;  

			}	
		} 

		int count =0 ;
		double disp = 200;
		double epsilon = vm["epsilon"].as<double>();
		int max_it = vm["max_it"].as<int>();
		std::vector<double> size2(k, 0);

		int l = ch + 1;
		while(count<max_it && disp>epsilon)

		{
			std::vector <double> base(l*k, 0.0)  ;


			//cout<<"\n\nParsing through image pixels to check centroid similarity, cycle number: "<<count+1<<std::endl;
			for(int i=0; i<k; i++)
			{
				size2[i]=0;

			}

			std::vector<double> base_all(l*k, 0.0) ;
			base_all = tbb::parallel_reduce(
					tbb::blocked_range<int>(0,nrows),
					std::vector<double>() ,
					[&](const tbb::blocked_range<int> r, std::vector<double> base)-> std::vector<double>
					{		    

					for(int y=r.begin(); y<r.end(); y++)
					{ base.resize(l*k) ;


					tbb::parallel_for( tbb::blocked_range<int>(0,ncols),
							[&](tbb::blocked_range<int> p)

							{ for(int x=p.begin(); x<p.end(); x++)

							{
							int index = y*ncols*ch+ch*x;
							int z = algo.nearest_centroid(pixels, centroids, index);


							if(ch==3)
							{base.at(l*z) += pixels[index];
							base.at(l*z+1) += pixels[index+1];	                                    
							base.at(l*z+2) += pixels[index+2];                                 
							base.at(l*z+3)++;}

							else {base.at(l*z)+=pixels[index];    base.at(l*z+1)++;}}	
							});

					}
					return base;
					},

				[&](std::vector<double> base_all, std::vector<double> base)->std::vector<double> 

				{//std::transform (base.begin(), base.end(), base_all.begin(), base.begin(), std::plus<double>());
					for (int i=0; i<l*k; i++)
					{
						base_all.at(i) += base.at(i);

					}		        return base_all;
				} );
			//std::plus<std::vector<double>>() );

			std::vector <double> new_centroids(ch*k,0.0) ;
			//cout<<"Done with centroid sums of cycle "<<count+1<<std::endl;

				for(int i=0; i<k; i++)
				{
				if(ch==3){	new_centroids[ch*i] = base_all[l*i] /base_all[l*i+3] ;
				new_centroids[ch*i+1] = base_all[l*i+1]/base_all[l*i+3] ;
				new_centroids[ch*i+2] = base_all[l*i+2]/base_all[l*i+3] ; 
				size2[i] = base_all[l*i+3];
				/*cout<<"\nRed value for centroid number "<<i+1<<" of cycle "<<count+1<<" is = "<<new_centroids[ch*i]<<std::endl; 
				cout<<"Green value for centroid number "<<i+1<<" of cycle "<<count+1<<" is = "<<new_centroids[ch*i+1]<<std::endl; 
				cout<<"Blue value for centroid number "<<i+1<<" of cycle "<<count+1<<" is = "<<new_centroids[ch*i+2]<<std::endl;*/}

				else{
				cout<<"\n\nGrayscale value of centroid "<<i+1<<" of cycle "<<count+1<<" is = "<<new_centroids[i];
				}



				}

			disp = algo.compute_displacement(new_centroids,centroids);

			//cout<<"\n\nLargest Displacement of centroids at cylce "<<count+1<< " = "<<disp<<std::endl;  
			for(int e=0; e<ch*k; e++)

			{
				centroids.at(e) = new_centroids.at(e);
			}

			count++;
		}

		cout<<"Count for K ="<<k<<" is "<<count<<endl;
		//cout<<"\n\nKmeans has done its work, now it's  time to display the new image"<<std::endl;

		//#pragma omp parallel for
		for (int i=0; i<k; i++)
		{
			cout<<"\nRed value for FINAL centroid number "<<i+1<<" is = "<<centroids[ch*i]<<std::endl;

			cout<<"Green value for FINAL centroid number "<<i+1<<" is = "<<centroids[ch*i+1]<<std::endl; 
			cout<<"Blue value for FINAL centroid number "<<i+1<<" is = "<<centroids[ch*i+2]<<std::endl; 

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
						int index = y*ncols*ch+ch*x;
						int z = algo.nearest_centroid(pixels, centroids, index);

						if(ch==3)
						{   Vec3b& pix = image.at<Vec3b>(y,x);
						pix[0] = (unsigned char)centroids[ch*z];
						pix[1] = (unsigned char)centroids[ch*z+1];
						pix[2] = (unsigned char)centroids[ch*z+2];}

						else{
						uchar& pix = image.at<uchar>(y,x);
						pix = centroids[z];
						}


						} });
				}});
		auto stop = high_resolution_clock::now(); 
		auto duration = duration_cast<milliseconds>(stop - start);
		cout <<"\n\nThe process took "<< duration.count()<<" milliseconds for TBB with K = "<<k<< std::endl;

		benchmark<<k<<","<<duration.count()<<",TBB\n";		





		//cout<<"\nNew Image Displayed with "<<k<<" amount of colors"<<std::endl;	 

		imwrite("./Seg_ImageTBB.jpg",image) ;}
	return 0 ;        }





