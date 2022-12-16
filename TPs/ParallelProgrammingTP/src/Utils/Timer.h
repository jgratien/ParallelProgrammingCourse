/*
 * Timer.h
 *
 *  Created on: Sep 24, 2018
 *      Author: gratienj
 */
#pragma once
#include <string>
#include <map>
#include <chrono>

namespace PPTP
{

class Timer
{
  public:
    class Sentry
    {
    public :
      Sentry(Timer& parent,std::string phase)
      : m_parent(parent)
      , m_phase(phase)
      {
        m_start = std::chrono::steady_clock::now();
      }

      virtual ~Sentry()
      {
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end - m_start);

        m_parent.add(m_phase,time_span.count()) ;
      }
    private:
      Timer&      m_parent ;
      std::string m_phase ;
      std::chrono::steady_clock::time_point m_start ;
    };

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
        std::cout<<iter.first<<":"<<iter.second<<std::endl ;
      }
      std::cout<<"================================"<<std::endl ;
    }
	
	void print_time() const {
		for(auto const& iter : m_counters)
      {
		std::cout<<iter.second<<std::endl ;
	  }
	}
	
	double d_print_time() const {
		for(auto const& iter : m_counters)
      {
		return iter.second;
	  }
	}

  private :
    std::map<std::string,double> m_counters ;
};

} /* namespace PPTP */

