-/*
 * helloworld.cpp
 *
 *  Created on: Aug 16, 2018
 *      Author: gratienj
 */

#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/variables_map.hpp>
#include "omp.h"
#include "tbb/tbb.h"

#include <string>
#include <vector>
#include <fstream>

#include "Utils/Timer.h"

class Tile
{
  public :
    int m_id = -1 ;
    int m_nb_children = 0 ;
    int m_parent_counter = 0 ;
    tbb::atomic<int> m_tbb_parent_counter ;

    int m_nb_parents = 0 ;
    std::vector<int> m_children_id;

    int m_launcher_counter = 0 ;
    tbb::atomic<int> m_tbb_launcher_counter ;

    void init(int id, int nx)
    {
      m_id = id ;
      int i = id/nx ;
      int j = id%nx ;
      if(i>0)
        ++m_nb_parents ;
      if(j>0)
        ++m_nb_parents ;
      if(i<nx-1)
        m_children_id.push_back(id+nx) ;
      if(j<nx-1)
        m_children_id.push_back(id+1) ;
    }

    void initCounter()
    {
      m_parent_counter = 0 ;
      m_launcher_counter = 0 ;
      m_tbb_parent_counter = 0 ;
      m_tbb_launcher_counter = 0 ;

    }

    void execute()
    {
      //std::cout<<"TILE("<<m_id<<")"<<std::endl ;
      sleep(1) ;
    }

    void execute(std::vector<Tile>& tile_list,int& nb_task_executed)
    {
#pragma omp task shared(tile_list,nb_task_executed)
      {
        execute() ;
#pragma omp atomic
        ++nb_task_executed ;
        //std::cout<<"NB EXECUTED TASKS : "<<nb_task_executed<<std::endl ;

        notifyChilfren(tile_list,nb_task_executed) ;
      }
    }

    void notifyChilfren(std::vector<Tile>& tile_list,int& nb_task_executed)
    {
      for(auto id : m_children_id)
        tile_list[id].receiveParentNotification(tile_list,nb_task_executed) ;
    }


    void receiveParentNotification(std::vector<Tile>& tile_list,int& nb_task_executed)
    {
#pragma omp atomic
      ++m_parent_counter ;

      if(m_parent_counter==m_nb_parents)
      {
        if(++m_launcher_counter==1) // I AM the firts launcher
        {
          execute(tile_list,nb_task_executed) ;
        }
      }
    }
};

void executeTile(int id,std::vector<Tile>& tile_list,int& nb_task_executed)
{
#pragma omp task shared(tile_list,nb_task_executed)
  {
      tile_list[id].execute() ;
#pragma omp atomic
      ++nb_task_executed ;
      std::cout<<"NB EXECUTED TASKS : "<<nb_task_executed<<std::endl ;

      for(auto child_id : tile_list[id].m_children_id)
      {
#pragma omp atomic
        ++tile_list[child_id].m_parent_counter ;

        if(tile_list[child_id].m_parent_counter==tile_list[child_id].m_nb_parents)
        {
          if(++tile_list[id].m_launcher_counter==1) // I AM the firts launcher
          {
            executeTile(child_id,tile_list,nb_task_executed) ;
          }
        }
      }
  }
}

int main(int argc, char** argv)
{
  using namespace boost::program_options ;
  options_description desc;
  desc.add_options()
      ("help", "produce help")
      ("nb-threads",value<int>()->default_value(0), "nb threads")
      ("nrows",value<int>()->default_value(0), "matrix size")
      ("nx",value<int>()->default_value(0), "nx grid size")
      ("file",value<std::string>(), "file input")
      ("eigen",value<int>()->default_value(0), "use eigen package") ;
  variables_map vm;
  store(parse_command_line(argc, argv, desc), vm);
  notify(vm);

  if (vm.count("help"))
  {
      std::cout << desc << "\n";
      return 1;
  }
  PPTP::Timer timer ;

  int nb_threads = vm["nb-threads"].as<int>() ;
  if(nb_threads>0)
    omp_set_num_threads(nb_threads) ;

  int nb_procs     = omp_get_num_procs() ;
  std::cout<<"NB PROCS     :"<<nb_procs<<std::endl ;
  int nb_available_threads = omp_get_max_threads() ;
  std::cout<<"NB AVAILABLE_THREADS :"<<nb_available_threads<<std::endl ;

  using namespace PPTP ;

  int nx = vm["nx"].as<int>() ;

  int nb_tiles = nx*nx ;

  std::vector<Tile> tile_list(nb_tiles) ;
  for(int i=0;i<nb_tiles;++i)
    tile_list[i].init(i,nx) ;

  {
    std::cout<<"Serial Wave computation"<<std::endl ;
    PPTP::Timer::Sentry sentry(timer,"Wave") ;
    for( auto& tile : tile_list)
      tile.execute() ;
  }
  {
    std::cout<<"Serial Graph Wave computation"<<std::endl ;
    PPTP::Timer::Sentry sentry(timer,"WaveGraph") ;
    int nb_task_executed = 0 ;
    tile_list[0].execute(tile_list,nb_task_executed) ;
    std::cout<<"NB EXECUTED TASKS : "<<nb_task_executed<<std::endl ;
  }
  {
    std::cout<<"Parallel Graph Wave computation"<<std::endl ;
    for(int i=0;i<nb_tiles;++i)
      tile_list[i].initCounter() ;

    PPTP::Timer::Sentry sentry(timer,"OMPWaveGraph") ;
    int nb_task_executed = 0 ;
    #pragma omp parallel shared(nb_task_executed,tile_list)
    {
      #pragma omp single
      {
        //executeTile(0,tile_list,nb_task_executed) ;
        tile_list[0].execute(tile_list,nb_task_executed) ;
      }
      #pragma omp taskwait
    }
    std::cout<<"NB EXECUTED TASKS : "<<nb_task_executed<<std::endl ;
  }
  {
    for(int i=0;i<nb_tiles;++i)
      tile_list[i].initCounter() ;

    std::cout<<"Parallel TBB Graph Wave computation"<<std::endl ;
    tbb::atomic<int> nb_tbb_task_executed ;
    nb_tbb_task_executed = 0 ;
    PPTP::Timer::Sentry sentry(timer,"TBBWaveGraph") ;
    int id = 0 ;
    tbb::parallel_do( &id,&id+1,
                      [&]( const int& id, tbb::parallel_do_feeder<int>&feeder )
                      {
                        tile_list[id].execute() ;
                        ++nb_tbb_task_executed ;
                        //std::cout<<"NB EXECUTED TASKS : "<<nb_tbb_task_executed<<std::endl ;

                        for(auto child_id : tile_list[id].m_children_id)
                        {
                          if(++tile_list[child_id].m_parent_counter==tile_list[child_id].m_nb_parents)
                          {
                            //std::cout<<"FEED add "<<child_id<<std::endl ;
                             feeder.add(child_id) ;
                          }
                          //else
                          //  std::cout<<"notify("<<child_id<<") "<<tile_list[child_id].m_parent_counter<<std::endl ;
                        }
                      }
                     );
    std::cout<<"NB EXECUTED TASKS : "<<nb_tbb_task_executed<<std::endl ;
  }


  timer.printInfo() ;
  return 0 ;
}
