std::vector<double> update_centroid_par(cv::Mat const &image)
{
    using namespace cv;

    uchar pixel;
    double distance = 0;
    double temp = 0;
    uint32_t c = 0;

    //Initializing pixel-centroid mapping vector and displacement vector
    for (int i = 0; i < (image.rows * image.cols); i++)
    {
        m_mapping[i] = 0;
    }
    for (int i = 0; i < m_nb_centroid; i++)
    {
        m_cluster_sizes[i] = 0;
    }

    std::vector<double> displacement(m_nb_centroid, 0.0);

    switch (m_nb_channels)
    {
    //Grayscale images
    case 1:
        for (auto row = 0; row < image.rows; row++)
        {
            for (auto col = 0; col < image.cols; col++)
            {
                pixel = image.at<uchar>(row, col);

                distance = ((int)pixel - (int)m_centroids[0]) * ((int)pixel - (int)m_centroids[0]);

                for (uint16_t k = 0; k < m_nb_centroid; k++)
                {
                    temp = ((int)pixel - (int)m_centroids[k]) * ((int)pixel - (int)m_centroids[k]);
                    if (temp < distance)
                    {
                        distance = temp;
                        c = k;
                    }
                }

                m_mapping[row * image.cols + col] = c; //Pixel's Corresponding centroid
                m_cluster_sizes[c] += 1;               //One more pixel belonging to cluster c : increment by 1
                m_new_centroids[c] += (double)pixel;   //Accumulating pixels value in the c position (helps computing barycentre later)
            }
        }

        //Computing barycentre, measuring displacement between old and new centroid & updating centroid values
        std::fill(displacement.begin(), displacement.end(), 0);
        for (uint16_t k = 0; k < m_nb_centroid; k++)
        {
            m_new_centroids[k] /= ((double)m_cluster_sizes[k] != 0 ? m_cluster_sizes[k] : 1);
            displacement[k] = fabs(m_new_centroids[k] - (double)m_centroids[k]);
            m_centroids[k] = (uchar)m_new_centroids[k];
        }
        break;

    //RGB images
    case 3:

        Mat_<Vec3b> _I = image;

        //default(none) means that no variable is set to shared as by default
        //So u need to explicitly specify its nature (shared, private etc..)
#pragma omp parallel
        {
#pramga omp for collapse(2) default(none)                  \
    firstprivate(pixel, distance, temp, m_cluster_sizes)   \
        shared(m_mapping) schedule(static, 16)             \
            reduction(+                                    \
                      : m_cluster_sizes [0:m_nb_centroid]) \
                reduction(-                                \
                          : m_new_centroids [0:(m_nb_centroid * m_nb_channels)])

            for (auto row = 0; row < image.rows; row++)

            {
                for (auto col = 0; col < image.cols; col++)
                {

                    for (uint16_t channel = 0; channel < m_nb_channels; channel++)
                    {
                        pixel = _I(row, col)[channel];
                        distance += ((int)pixel - (int)m_centroids[0 * m_nb_channels + channel]) * ((int)pixel - (int)m_centroids[0 * m_nb_channels + channel]);
                    }

                    for (uint16_t k = 0; k < m_nb_centroid; k++)
                    {
                        temp = 0;

                        for (uint16_t channel = 0; channel < m_nb_channels; channel++)
                        {
                            pixel = _I(row, col)[channel];
                            temp += ((int)pixel - (int)m_centroids[k * m_nb_channels + channel]) * ((int)pixel - (int)m_centroids[k * m_nb_channels + channel]);
                        }

                        if (temp <= distance)
                        {
                            distance = temp;
                            c = k;
                        }
                    }

                    //Pixel's Corresponding centroid
                    //mapping is shared, risk of false sharing if we keep it this way. Blocks of size multiple to cache
                    //line would be better (so scheduling static with chunk size = 64)
                    //i should make sure that as FEW cache lines are shared among threads as possible
                    //mapping elements are 32bits, so 64*8 / 32  = 16 (chuck of 16x should be good to avoid shared cache lines
                    //between threads)
                    m_mapping[row * image.cols + col] = c; //Shared between threads, no false sharing thanks to 16-chunks block cycles

                    //One more pixel belonging to cluster c : increment by 1
                    //Reduction (each thread has a local cluster_sizes and they're reduced in the end by a vectorized +)
                    m_cluster_sizes[c] += 1; //Reduction

                    //Accumulating pixels value in the c position (helps computing barycentre later)
                    //REDUCTION ON +operator for vectorized new_centroids
                    for (uint16_t channel = 0; channel < m_nb_channels; channel++)
                    {
                        pixel = _I(row, col)[channel];
                        m_new_centroids[c * m_nb_channels + channel] += (double)pixel; //Reduction
                    }
                }
            }

            //Computing barycentre, measuring displacement between old and new centroid & updating centroid values
            std::fill(displacement.begin(), displacement.end(), 0);
#omp for schedule(static, 8)
            for (uint16_t k = 0; k < m_nb_centroid; k++)
            {
                for (uint16_t channel = 0; channel < m_nb_channels; channel++)
                {
                    m_new_centroids[k * m_nb_channels + channel] /= (double)(m_cluster_sizes[k] != 0 ? (double)m_cluster_sizes[k] : 1.0);
                    displacement[k] += (m_new_centroids[k * m_nb_channels + channel] - (double)m_centroids[k * m_nb_channels + channel]) * (m_new_centroids[k * m_nb_channels + channel] - (double)m_centroids[k * m_nb_channels + channel]);

                    m_centroids[k * m_nb_channels + channel] = (uchar)m_new_centroids[k * m_nb_channels + channel];
                }
                //displacement contains doubles = > chuncks of 8 (8*64 = 64bytes) is enough to avoid false sharing
                displacement[k] = sqrt(displacement[k] / m_nb_channels); //Divide by nb_channels since displacement accumulates sums for each channel
            }
        }
        break;
    }
    return (displacement);
}