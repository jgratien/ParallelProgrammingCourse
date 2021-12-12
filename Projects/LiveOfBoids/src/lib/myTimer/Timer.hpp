#pragma once
#include "omp.h"
#include <string>
#include <map>
#include <iostream>

class Timer
{
  public:
    Timer(){}
    virtual ~Timer(){}

    void add(std::string const& phase,double value)
    {
      auto iter = m_counters.find(phase) ;
      if(iter==m_counters.end())
        m_counters[phase] = value ;
      else
        iter->second += value ;
    }

    void printInfo() const {
      std::cout<<"================================"<<std::endl ;
      std::cout<<"PERF INFO : "<<std::endl ;
      for(auto const& iter : m_counters)
      {
        std::cout<<iter.first<<"_"<<iter.second<<std::endl ;
      }
      std::cout<<"================================"<<std::endl ;
    }

  private :
    std::map<std::string,double> m_counters ;
};

class Sentry
{
public :
    Sentry(Timer& parent,std::string phase)
    : m_parent(parent)
    , m_phase(phase)
    {
    m_start = omp_get_wtime();
    }

    virtual ~Sentry()
    {
    double end = omp_get_wtime() ;
    m_parent.add(m_phase,end-m_start) ;
    }
private:
    Timer&      m_parent ;
    std::string m_phase ;
    double      m_start = 0. ;
};




