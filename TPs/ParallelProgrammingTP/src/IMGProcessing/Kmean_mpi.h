
// INIT CENTROID
std::vector<uchar> init_centroid(std::vector<uchar> flat_image, int nb_channels)
{
	std::vector<uchar> cent(nb_channels);
	int random_index;
	switch(nb_channels)
	{
	  case(1):
	  {
	     random_index = rand() % flat_image.size();
	     cent[0] = flat_image[random_index]; 
	     break;
	  }
	  case(3):
	  {
	    // we pick a random index multiple of 3, to have all the colors of the pixel
	    random_index = (rand() % (flat_image.size() / 3 )) * 3;
	    cent[0] = flat_image[random_index];
	    cent[1] = flat_image[random_index+1]; 
	    cent[2] = flat_image[random_index+2]; 
	  }
	}
	return cent;
}


// NEAREST CENTROID
int nearest_centroid(int nb_channels, int nb_centroids, std::vector<uchar> centroids, uchar red, uchar green = 0, uchar blue = 0)
{
	std::vector<double> distances(nb_centroids);
	double dist = 0;
	for(int i=0; i<nb_centroids; i++)
	{
	  switch(nb_channels)
	  {
	    case(1):
	    {
		dist = abs(red - centroids[i]);
		distances.at(i) = dist;
		break;
            }
	    case(3):
	    {
		double diff_red = pow(red - centroids[3*i], 2);
		double diff_green = pow(green - centroids[3*i+1], 2);
		double diff_blue = pow(blue - centroids[3*i+2], 2);
		dist = sqrt(diff_red + diff_green + diff_blue);
		distances.at(i) = dist;
		break;
	    }
	  }
	}

	// index of the minimum distance
	double min = *std::min_element(distances.begin(), distances.end());
	std::vector<double>::iterator itr = std::find(distances.begin(), distances.end(), min);
	return std::distance(distances.begin(), itr);
}

// UPDATE PIXEL COLLECTION
void update_pixel_collection(std::vector<int>& pixel_collection, int nb_channels, int nb_rows, int nb_cols, int nb_centroids, std::vector<uchar> centroids, std::vector<uchar> flat_image)
{
	int nearest_centroid_id;
	for(int i=0; i<nb_rows*nb_cols; i++)
	{
		switch(nb_channels)
		{
		  case(1):
		  {
			nearest_centroid_id = nearest_centroid(nb_channels, nb_centroids, centroids, flat_image[i]);
			pixel_collection.at(i) = nearest_centroid_id;
			break;
		  }
		  case(3):
		  {
			nearest_centroid_id = nearest_centroid(nb_channels, nb_centroids, centroids, flat_image[3*i], flat_image[3*i+1], flat_image[3*i+2]);
			pixel_collection.at(i) = nearest_centroid_id;
			break;
		  }
		}
	}
}

// COUNT PIXELS FOR EACH CENTROIDS
std::vector<double> count_pixels(int nb_centroids, std::vector<int> pixel_collection, std::vector<uchar> flat_image, int nb_rows, int nb_cols, int nb_channels)
{
	std::vector<double> count_res((nb_channels+1)*nb_centroids);
	for(int j=0; j<nb_centroids; j++)
	{
	  double sum=0, sum_red=0, sum_green=0, sum_blue=0;
	  int count=0;
	  for(int i=0; i<nb_cols*nb_rows; i++)
	  {
	    if(pixel_collection[i]==j)
	    {
	      switch(nb_channels)
	      {
		case(1):
		{
			sum += (double) flat_image[i];
			count ++;
			break;
		}
		case(3):
		{
			sum_red += (double) flat_image[3*i];
			sum_green += (double) flat_image[3*i+1];
			sum_blue += (double) flat_image[3*i+2];
			count ++;
			break;
		}
	      }
	    }
	  }
	  switch(nb_channels)
	  {
	    case(1):
	    {
		count_res[2*j] = sum;
		count_res[2*j+1] = count;
		break;
	    }
	    case(3):
	    {
		count_res[4*j] = sum_red;
		count_res[4*j+1] = sum_green;
		count_res[4*j+2] = sum_blue;
		count_res[4*j+3] = count;
		break;
	    }
	  }
	}

	return count_res;
}

// CALCULALTE BARYCENTER
std::vector<uchar> compute_barycenter_and_update_centroids(std::vector<double> count_pxl, int nb_centroids, int nb_channels)
{
	std::vector<uchar> centroids(nb_centroids*nb_channels);
	for(int i=0; i<nb_centroids; i++)
	{
	  switch(nb_channels)
	  {
	    case(1):
	    {
		double barycenter = (double) count_pxl[2*i] / (double) count_pxl[2*i+1];
		centroids[i] = (uchar) barycenter;
		break;
	    }
	    case(3):
	    {
		double barycenter_red = (double) count_pxl[4*i] / (double) count_pxl[4*i+3];
		centroids[3*i] = (uchar) barycenter_red;
		double barycenter_green = (double) count_pxl[4*i+1] / (double) count_pxl[4*i+3];
		centroids[3*i+1] = (uchar) barycenter_green;

		double barycenter_blue = (double) count_pxl[4*i+2] / (double) count_pxl[4*i+3];
		centroids[3*i+2] = (uchar) barycenter_blue;


	    }
	  }
	}

	return centroids;
}

// COMPUTE VARIANCE
double compute_variance(std::vector<uchar> old_centroids, std::vector<uchar> centroids, int nb_channels, int nb_centroids)
{
	double sum=0;
	for(int i=0; i<nb_centroids; i++)
	{
	  switch(nb_channels)
	  {
	    case(1):
	    {
		sum += abs(old_centroids[i] - centroids[i]);
		break;
	    }
	    case(3):
	    {
		double d_red = pow(old_centroids[3*i] - centroids[3*i], 2);
		double d_green = pow(old_centroids[3*i+1] - centroids[3*i+1], 2);
		double d_blue = pow(old_centroids[3*i+2] - centroids[3*i+2], 2);
		sum += sqrt(d_red + d_green + d_blue);
		break;
	    }
	  }
	}
	return (double) sum / (double) nb_centroids;
}

// COMPUTE SEGMENTATION

void compute_segmentation(std::vector<uchar>& flat_image, int nb_rows, int nb_cols, int nb_channels, int nb_centroids, std::vector<uchar> centroids)
{
	for(int i=0; i<nb_cols*nb_rows; i++)
	{
	  switch(nb_channels)
	  {
	    case(1):
	    {
	      uchar pixel = flat_image[i];
	      int id_centroid = nearest_centroid(nb_channels, nb_centroids, centroids, pixel);
	      flat_image[i] = centroids[id_centroid];
	      break;
	    }
	    case(3):
	    {
	      uchar red = flat_image[3*i];
	      uchar green = flat_image[3*i+1];
	      uchar blue = flat_image[3*i+2];
	      int id_centroid = nearest_centroid(nb_channels, nb_centroids, centroids, red, green, blue);
	      flat_image[3*i] = centroids[3*id_centroid];
	      flat_image[3*i+1] = centroids[3*id_centroid+1];
	      flat_image[3*i+2] = centroids[3*id_centroid+2];
	      break;
	    }
	  }
	}
}

// UNFLAT IMAGE
void unflat_image(std::vector<uchar> flat_image, cv::Mat& image, int nb_channels)
{
	using namespace cv;
	for(int i=0; i<image.rows; i++)
	{
	  for(int j=0; j<image.cols; j++)
	  {
	    switch(nb_channels)
	    {
	      case(1):
	      {
		image.at<uchar>(i,j) = flat_image[i*image.cols+j];
		break;
	      }
	      case(3):
	      {
		//Mat_<Vec3b> _I = image;
		image.at<Vec3b>(i,j)[0] = flat_image[3*(i*image.cols+j)];
	        image.at<Vec3b>(i,j)[1] = flat_image[3*(i*image.cols+j)+1];
		image.at<Vec3b>(i,j)[2] = flat_image[3*(i*image.cols+j)+2];
		break;
	      }
	    }
	  }
	}
}
