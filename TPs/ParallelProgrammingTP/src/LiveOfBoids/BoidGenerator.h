/*
 * BoidGenerator.h
 *
 *  Created on: Dec 7, 2024
 *      Author: ossaradj
 */

#ifndef SRC_LIVEOFBOIDS_BOIDGENERATOR_H_
#define SRC_LIVEOFBOIDS_BOIDGENERATOR_H_

#include <vector>
#include <random>
#include "tbb/tbb.h"
#include "omp.h"
#include "Boid.h"

class BoidGenerator
{
  public:
    BoidGenerator(){}
    virtual ~BoidGenerator(){}

    void setChunkSize(int chunk_size)
    {
        m_chunk_size = chunk_size ;
    };

    void generate(int nb, std::vector<Boid>& boids)
    {
        std::random_device rd;
        std::mt19937 eng(rd());        
        std::uniform_real_distribution<float> random_x(0.0f, static_cast<float>(600));
        std::uniform_real_distribution<float> random_y(0.0f, static_cast<float>(400));
        std::uniform_real_distribution<float> angle_space(0.0f, 2.0f * M_PI);
        std::uniform_real_distribution<float> velocity_space(0.1f, 0.5f);

        for (int i=0; i<nb; ++i)
        {
            auto angle = angle_space(eng);                    // Random direction
            auto speed = velocity_space(eng);                // Random speed
            boids.push_back(Boid(Vector2D{random_x(eng), random_y(eng)}, Vector2D{speed * std::cos(angle), speed * std::sin(angle)}));
        }
    };

    void findNeighbors(std::vector<Boid>& boids, std::vector<int>& y, float radius) const
    {
        for(int i=0; i<boids.size(); ++i)
        {
            y[i] = boids[i].get_neighbors(boids, radius).size() ;
        }
    };

    void omptaskfindNeighbors(std::vector<Boid>& boids, std::vector<int>& y, float radius) const
    {
        std::size_t n_rows = boids.size();
        std::size_t nb_task = (n_rows+m_chunk_size-1)/m_chunk_size ;

        #pragma omp parallel
        {
            //TODO TASK OPENMP
            #pragma omp single
            {
              for(std::size_t task_id = 0; task_id < nb_task; ++task_id)
              {
                std::size_t start_row = task_id * m_chunk_size;
                std::size_t end_row = std::min(start_row + m_chunk_size, n_rows);

                #pragma omp task firstprivate(start_row, end_row)
                {
                  for(std::size_t irow =start_row; irow<end_row; ++irow)
                  {
                    std::vector<Boid> local_neighbors ;
                    for (size_t i = 0; i < n_rows; ++i)
                    {
                        if ((&boids[i] != &boids[irow]) && 
                                (boids[irow].getPosition().euclidean_dist(boids[i].getPosition()) < radius))
                        {
                            local_neighbors.push_back(boids[i]);
                        }
                    }
                    y[irow] = local_neighbors.size() ;
                  }
                }
              }
            }
        }
        
        // #pragma omp parallel for
        // for(int i = 0; i < boids.size(); ++i)
        // {
        //     y[i] = boids[i].omptaskget_neighbors(boids, radius).size();
        // }
        
    };

    void omptilefindNeighbors(std::vector<Boid>& boids, std::vector<int>& y, float radius) const
    {
        std::size_t n_rows = boids.size();
        std::size_t n_cols = boids.size();
        y.resize(n_rows, 0);
        std::size_t nb_row_tasks = (n_rows + m_chunk_size - 1) / m_chunk_size;
        std::size_t nb_col_tasks = (n_cols + m_chunk_size - 1) / m_chunk_size;

        #pragma omp parallel
        {
            #pragma omp single
            {
                for (std::size_t row_task_id = 0; row_task_id < nb_row_tasks; ++row_task_id)
                {
                    std::size_t start_row = row_task_id * m_chunk_size;
                    std::size_t end_row = std::min(start_row + m_chunk_size, n_rows);

                    for (std::size_t col_task_id = 0; col_task_id < nb_col_tasks; ++col_task_id)
                    {
                        std::size_t start_col = col_task_id * m_chunk_size;
                        std::size_t end_col = std::min(start_col + m_chunk_size, n_cols);

                        #pragma omp task firstprivate(start_row, end_row, start_col, end_col)
                        {
                            for (std::size_t irow = start_row; irow < end_row; ++irow)
                            {
                                std::vector<Boid> local_neighbors;
                                for (std::size_t icol = start_col; icol < end_col; ++icol)
                                {
                                    if ((&boids[icol] != &boids[irow]) &&
                                        (boids[irow].getPosition().euclidean_dist(boids[icol].getPosition()) < radius))
                                    {
                                        local_neighbors.push_back(boids[icol]);
                                    }
                                }
                                #pragma omp atomic
                                y[irow] += local_neighbors.size();
                            }
                        }
                    }
                }
            }
        }
    };

    void tbbrangefindNeighbors(std::vector<Boid>& boids, std::vector<int>& y, float radius) const
    {
        std::size_t n_rows = boids.size() ;
        {
            // TODO TBB WITH RANGE
          tbb::parallel_for(tbb::blocked_range<size_t>(0, n_rows, m_chunk_size),
                          [&](tbb::blocked_range<size_t> const& r)
                          {
                            for(auto irow=r.begin(); irow<r.end(); ++irow)
                            {
                              std::vector<Boid> local_neighbors ;
                              for(std::size_t i =0; i<n_rows;++i)
                              {
                                if ((&boids[i] != &boids[irow]) && 
                                (boids[irow].getPosition().euclidean_dist(boids[i].getPosition()) < radius))
                                {
                                    local_neighbors.push_back(boids[i]);
                                }
                              }
                              y[irow] = local_neighbors.size() ;
                            }
                          });
        }
      };

      void tbbrange2dfindNeighbors(std::vector<Boid>& boids, std::vector<int>& y, float radius) const
      {
        std::size_t n_rows = boids.size() ;
        y.resize(n_rows, 0);
        tbb::spin_mutex mutex ;
        {
                // TODO TBB RANGE 2D
                tbb::parallel_for(tbb::blocked_range2d<size_t>(0, n_rows, m_chunk_size, 0, n_rows, m_chunk_size),
                          [&](tbb::blocked_range2d<size_t> const& r)
                          {
                            for(auto irow=r.rows().begin(); irow<r.rows().end(); ++irow)
                            {
                              std::vector<Boid> local_neighbors ;
                              for(auto jcol =r.cols().begin(); jcol<r.cols().end();++jcol)
                              {
                                if ((&boids[jcol] != &boids[irow]) && 
                                (boids[irow].getPosition().euclidean_dist(boids[jcol].getPosition()) < radius))
                                {
                                    local_neighbors.push_back(boids[jcol]);
                                }
                              }
                              {
                                tbb::spin_mutex::scoped_lock lock(mutex) ;
                                y[irow] += local_neighbors.size() ;
                              }
                            }
                          });
        }
      };

    private:
        int m_chunk_size = 1 ;

};

#endif /* SRC_LIVEOFBOIDS_BOIDGENERATOR_H_ */