/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2014                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#define __STDC_FORMAT_MACROS
#define __STDC_CONSTANT_MACROS

#include <config.h>
#include "Patterns_gen.h"

#include <cassert>
#include <inttypes.h>

#include <pearl/CallbackManager.h>
#include <pearl/Callpath.h>
#include <pearl/GlobalDefs.h>

#include "AnalyzeTask.h"
#include "CbData.h"
#include "MpiPattern.h"
#ifdef _OPENMP
#  include "OmpPattern.h"
#endif

using namespace std;
using namespace pearl;
using namespace scout;



#line 19 "Generic.pattern"

#include <pearl/Leave_rep.h>
#include <pearl/ThreadEnd_rep.h>

#include "Callstack.h"

#line 17 "Async.pattern"


  #include <iostream>
  #include <pearl/AmListenerFactory.h>
  #include <pearl/AmRequestFactory.h>
  #include <pearl/Communicator.h>

  #if defined(_MPI)
  #  include <pearl/MpiAmListener.h>
  #  include <pearl/MpiAmRequest.h>
  #  include <pearl/AmRuntime.h>
  #  include "AmLockContention.h"
  #  include "AmSeverityExchange.h"
  #  include "AmSyncpointExchange.h"
  #  include "AmWaitForProgressRequest.h"
  #  include "AmWaitForProgressResponse.h"
  #endif // _MPI

  extern bool enableAsynchronous;

#line 19 "MPI.pattern"

  #include <cfloat>
  #include <list>

  #include <elg_error.h>

  #include <pearl/CallbackManager.h>
  #include <pearl/ProcessGroup.h>
  #include <pearl/Region.h>
  #include <pearl/String.h>

  #if defined(_MPI)
    #include <pearl/EventSet.h>
    #include <pearl/LocalData.h>
    #include <pearl/MpiCollEnd_rep.h>
    #include <pearl/MpiComm.h>
    #include <pearl/MpiMessage.h>
    #include <pearl/RemoteData.h>
    #include <pearl/RemoteEventSet.h>

    #include "MpiDatatypes.h"
    #include "MpiOperators.h"
  #endif   // _MPI

  #include "Roles.h"
  #include "scout_types.h"
  #include "user_events.h"

#line 19 "OMP.pattern"

#if defined(_OPENMP)
namespace {
    int omp_nest_level = 0;
}   // unnamed namespace
#endif

#line 19 "Thread.pattern"

#include "LockTracking.h"

#line 20 "MPI_RMA.pattern"

  #include <algorithm>
  #include <cfloat>
  #include <cstring>
  #include <iostream>
  #include <memory>
  #include <sstream>
  #include <set>

  #if defined(_MPI)
    #include <pearl/AmRequestFactory.h>
    #include <pearl/MpiGroup.h>
    #include <pearl/MpiWindow.h>
    #include <pearl/RmaEpoch.h>

    #include "AmWaitForProgressRequest.h"
    #include "LockEpochQueue.h"
    #include "MpiDatatypes.h"
    #include "MpiOperators.h"
    #include "Predicates.h"
  #endif

#line 17 "Statistics.pattern"

  #include <cstdio>
  #include <cstring>
  #include <pearl/pearl_replay.h>
  #include <sys/stat.h>
  #include "LockTracking.h"

  #include "Quantile.h"
  #include "ReportData.h"

  #if defined(_MPI)
    #include "MpiDatatypes.h"
    #include "MpiOperators.h"
  #endif   // _MPI

  #if defined(_OPENMP)
    #include "omp.h"
  #endif   // _OPENMP
  
  /// Maximum number of coefficients used for quantile approximation
  #define NUMBER_COEFF 60

  extern string archiveDirectory;
  extern bool enableStatistics;

  /* For debugging: #define WRITE_CONTROL_VALUES */

#line 17 "CriticalPath.pattern"

// #define CPADEBUG
#include "DelayOps.h"
#include "SynchpointHandler.h"
#include "TmapCacheHandler.h"
#include <algorithm>
#include <numeric>
#ifdef _OPENMP
#include "OmpEventHandler.h"
#include <omp.h>
#endif
#ifdef CPADEBUG
#include <sstream>
#include <fstream>
#endif

extern bool enableCriticalPath;

#line 17 "Delay.MPI.pattern"

  #include <pearl/Buffer.h>
  #include <pearl/Location.h>    
  #include <pearl/GlobalDefs.h>

#ifdef _MPI
  #include "MpiDelayOps.h"

  #include <pearl/LocalData.h>
  #include <pearl/RemoteData.h>
#endif

  #include "DelayCostMap.h"      
  #include "DelayOps.h"
  #include "user_events.h"

// #define DELAYDEBUG

#ifdef DELAYDEBUG
  #include <sstream>
  #include <fstream>
#endif

  extern bool enableDelayAnalysis;

#line 17 "Delay.OMP.pattern"

  #include <pearl/Buffer.h>
  #include <pearl/Location.h>    
  #include <pearl/GlobalDefs.h>

#ifdef _OPENMP
  #include "OmpDelayOps.h"
#endif

  #include "DelayCostMap.h"      
  #include "DelayOps.h"
  #include "user_events.h"

// #define OMPDELAYDEBUG

#ifdef OMPDELAYDEBUG
  #include <sstream>
  #include <fstream>
#endif

  extern bool enableDelayAnalysis;


/*
 *---------------------------------------------------------------------------
 *
 * class PatternTime
 *
 *---------------------------------------------------------------------------
 */


class PatternTime : public Pattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(LEAVE, PEARL_create_callback(this, &PatternTime::leave__cb));
        (it->second)->register_callback(THREAD_END, PEARL_create_callback(this, &PatternTime::thread_end__cb));
        (it->second)->register_callback(THREAD_TASK_COMPLETE, PEARL_create_callback(this, &PatternTime::thread_task_complete__cb));
        (it->second)->register_callback(OMP_MGMT_FORK, PEARL_create_callback(this, &PatternTime::omp_mgmt_fork__cb));
        (it->second)->register_callback(OMP_MGMT_JOIN, PEARL_create_callback(this, &PatternTime::omp_mgmt_join__cb));
        (it->second)->register_callback(FINISHED, PEARL_create_callback(this, &PatternTime::finished__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_TIME;
    }

    virtual long get_parent() const
    {
      return PAT_NONE;
    }

    virtual string get_name() const
    {
      return "Time";
    }

    virtual string get_unique_name() const
    {
      return "time";
    }

    virtual string get_descr() const
    {
      return "Total CPU allocation time (includes time allocated for idle threads)";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_INCLUSIVE;
    }

    /* Callback methods */
    void finished__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void leave__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void omp_mgmt_fork__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void omp_mgmt_join__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void thread_end__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void thread_task_complete__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternTime::finished__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 111 "Generic.pattern"

      // Combined loop over all callpaths to:
      // (1) Inclusify profile of non-master threads outside of parallel regions
      // (2) Fix the task root value by attributing its children
      
      uint32_t thread_id = data->mTrace->get_location().getThreadId();

      uint32_t count = data->mDefs->numCallpaths();
      for (uint32_t id = 0; id < count; ++id) {
        Callpath* callpath = data->mDefs->get_cnode(id);

        // Fix task root value
        if ((callpath->getRegion() == data->mDefs->getTaskRootRegion())
           ||(callpath->getRegion() == data->mDefs->getThreadRootRegion()))  {
          for (uint32_t i = 0; i < callpath->numChildren(); ++i) {
            uint32_t childId = callpath->getChild(i).getId();
            m_severity[callpath] += m_severity[data->mDefs->get_cnode(childId)];
          }
        }
        
        // skip in case of thread 0
        if (0 != thread_id) {
          // Does the callpath refer to an OpenMP parallel region?
          if (!is_omp_parallel(callpath->getRegion()))
            continue;

          // Propagate severity value "upwards"
          double    value  = m_severity[callpath];
          Callpath* parent = callpath->getParent();
          while (parent) {
            m_severity[parent] += value;
            parent = parent->getParent();
          }
        }
      }
    
}

void PatternTime::leave__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 55 "Generic.pattern"

      Event     enter = data->mCallstack->top();
      Callpath* cnode = enter.get_cnode();

      m_severity[cnode] += event->getTimestamp() - enter->getTimestamp() - data->mCallstack->getCurrentTimeOffset();
    
}

void PatternTime::omp_mgmt_fork__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 103 "Generic.pattern"

      m_severity[event.get_cnode()] += data->mIdle;
    
}

void PatternTime::omp_mgmt_join__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 107 "Generic.pattern"

      m_severity[event.get_cnode()] += data->mIdle;
    
}

void PatternTime::thread_end__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 62 "Generic.pattern"

      // updating thread stubnodes in calling tree 
      ThreadEnd_rep& threadEnd = event_cast<ThreadEnd_rep>(*event);
      uint64_t id = threadEnd.getSequenceCount();

      // prev == thread function LEAVE
      // check to avoid artifical regions
      Event prev = event.prev();
      while (prev.is_valid())
      {
        if (prev->isOfType(LEAVE))
        {
          Leave_rep& test = event_cast<Leave_rep>(*prev);
          if (test.getRegion().getRole() != Region::ROLE_ARTIFICIAL)
            break;
        }
        prev=prev.prev();
      }
      // if the loop didn't find a matching leave, your trace is broken
      assert(prev.is_valid());

      double value = prev->getTimestamp() - data->mThreadStartTime[id];
      Callpath* callpath = data->mDefs->get_cnode(data->mThreadStubNodeIds[id]);
      m_severity[callpath] += value;

      // Propagate severity value "upwards"
      Callpath* parent = callpath->getParent();
      while (parent) {
        m_severity[parent] += value;
        parent = parent->getParent();
      }
    
}

void PatternTime::thread_task_complete__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 95 "Generic.pattern"

      // Updating stub nodes in calling tree (only for the implicit task)
      data->mIdle = event->getTimestamp() - data->mCallstack->getStartTime() - data->mCallstack->getTotalTimeOffset();
      m_severity[data->mDefs->get_cnode(data->mCallstack->getStubNodeId())] += data->mIdle;

      cbmanager.notify(TASK_CORRECTION, event, data);
    
}


/*
 *---------------------------------------------------------------------------
 *
 * class PatternVisits
 *
 *---------------------------------------------------------------------------
 */


class PatternVisits : public Pattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(GROUP_ENTER, PEARL_create_callback(this, &PatternVisits::group_enter__cb));
        (it->second)->register_callback(THREAD_TASK_COMPLETE, PEARL_create_callback(this, &PatternVisits::thread_task_complete__cb));
        (it->second)->register_callback(THREAD_END, PEARL_create_callback(this, &PatternVisits::thread_end__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_VISITS;
    }

    virtual long get_parent() const
    {
      return PAT_NONE;
    }

    virtual string get_name() const
    {
      return "Visits";
    }

    virtual string get_unique_name() const
    {
      return "visits";
    }

    virtual string get_descr() const
    {
      return "Number of visits";
    }

    virtual string get_unit() const
    {
      return "occ";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void group_enter__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void thread_end__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void thread_task_complete__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternVisits::group_enter__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 188 "Generic.pattern"

      ++m_severity[event.get_cnode()];
    
}

void PatternVisits::thread_end__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 198 "Generic.pattern"

      // updating visits for thread stub nodes
      ThreadEnd_rep& threadEnd = event_cast<ThreadEnd_rep>(*event);
      uint64_t id = threadEnd.getSequenceCount();

      Callpath* callpath = data->mDefs->get_cnode(data->mThreadStubNodeIds[id]);
      ++m_severity[callpath];

      // Propagate severity value "upwards"
      Callpath* parent = callpath->getParent();
      while (parent) {
        ++m_severity[parent];
        parent = parent->getParent();
      }
    
}

void PatternVisits::thread_task_complete__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 192 "Generic.pattern"

      // updating visits for the task stub nodes on the implicit task
      // current mode: one stub node per instance and one visit per instance
      ++m_severity[data->mDefs->get_cnode(data->mCallstack->getStubNodeId())];
    
}


#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternAsync
 *
 *---------------------------------------------------------------------------
 */


class PatternAsync : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(PREPARE, PEARL_create_callback(this, &PatternAsync::prepare__cb));
        (it->second)->register_callback(FINISHED, PEARL_create_callback(this, &PatternAsync::finished__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_ASYNC;
    }

    virtual long get_parent() const
    {
      return PAT_NONE;
    }

    virtual string get_name() const
    {
      return "ASYNC";
    }

    virtual string get_unique_name() const
    {
      return "async";
    }

    virtual string get_descr() const
    {
      return "Driver for asynchronous pattern detection";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    virtual bool is_hidden() const
    {
      return true;
    }

    /* Callback methods */
    void finished__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void prepare__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternAsync::finished__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 85 "Async.pattern"

            AmRuntime::getInstance().shutdown();
        
}

void PatternAsync::prepare__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 57 "Async.pattern"

            pearl::AmRuntime::getInstance().setCbdata(data);
            pearl::AmRuntime::getInstance().setCbmanager(cbmanager);

            pearl::AmRuntime::getInstance().attach(AmLockContention::getInstance());
            pearl::AmRuntime::getInstance().attach(AmSeverityExchange::getInstance());
            pearl::AmRuntime::getInstance().attach(AmSyncpointExchange::getInstance());
            pearl::AmRuntime::getInstance().attach(AmWaitForProgressRequest::getInstance());
            pearl::AmRuntime::getInstance().attach(AmWaitForProgressResponse::getInstance());

            #if defined(_MPI)
            // Register MPI specific factory callbacks
            pearl::AmRequestFactory::registerCallback(Paradigm::MPI,
                                                      &MpiAmRequest::create);
            pearl::AmListenerFactory::registerCallback(Paradigm::MPI,
                                                      &MpiAmListener::create);
            #endif

            #if defined(_ARMCI)
            // Register ARMCI specific factory callbacks
            pearl::AmRequestFactory::registerCallback(Paradigm::ARMCI,
                                                      &MpiAmRequest::create);
            pearl::AmListenerFactory::registerCallback(Paradigm::ARMCI,
                                                      &MpiAmListener::create);
            #endif


        
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_Init
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_Init : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(GROUP_ENTER, PEARL_create_callback(this, &PatternMPI_Init::group_enter__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_INIT_EXIT;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_MANAGEMENT;
    }

    virtual string get_name() const
    {
      return "MPI Init/Finalize";
    }

    virtual string get_unique_name() const
    {
      return "mpi_init_exit";
    }

    virtual string get_descr() const
    {
      return "Time spent in MPI initialization/finalization calls";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    virtual bool is_hidden() const
    {
      return true;
    }

    /* Callback methods */
    void group_enter__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_Init::group_enter__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 125 "MPI.pattern"

      const Region& region = event->getRegion();

      if (is_mpi_init(region))
        cbmanager.notify(INIT, event, data);
      else if (is_mpi_finalize(region))
        cbmanager.notify(FINALIZE, event, data);
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_InitCompletion
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_InitCompletion : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(INIT_END, PEARL_create_callback(this, &PatternMPI_InitCompletion::init_end__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_INIT_COMPLETION;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_INIT_EXIT;
    }

    virtual string get_name() const
    {
      return "MPI Initialization Completion";
    }

    virtual string get_unique_name() const
    {
      return "mpi_init_completion";
    }

    virtual string get_descr() const
    {
      return "Time needed to finish MPI initialization";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void init_end__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_InitCompletion::init_end__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 159 "MPI.pattern"

      const CollectiveInfo& coll(data->mCollinfo);

      double min_time = coll.earliest_end.time;

      // Validate clock condition
      if (min_time < coll.latest.time) {
        // Do not report violation again -- already done by "Wait at Barrier"
        min_time = coll.latest.time;
      }

      // Calculate waiting time
      data->mIdle = event->getTimestamp() - min_time;
      if (data->mIdle > 0)
        m_severity[event.get_cnode()] += data->mIdle;

      // There will always be completion time at barriers; all processes need
      // to take part in most-severe instance tracking
      cbmanager.notify(INIT_COMPL, event, data);
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_FinalizeWait
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_FinalizeWait : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(FINALIZE_END, PEARL_create_callback(this, &PatternMPI_FinalizeWait::finalize_end__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_FINALIZE_WAIT;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_INIT_EXIT;
    }

    virtual string get_name() const
    {
      return "MPI Wait at Finalize";
    }

    virtual string get_unique_name() const
    {
      return "mpi_finalize_wait";
    }

    virtual string get_descr() const
    {
      return "Waiting time in front of MPI_Finalize";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void finalize_end__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_FinalizeWait::finalize_end__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 206 "MPI.pattern"

      const CollectiveInfo& coll(data->mCollinfo);

      // Validate clock condition
      if (coll.latest.time > event->getTimestamp()) {
        elg_cntl_msg("Unsynchronized clocks (loc: %" PRIu64 ", reg: %s, diff: %es)!\n",
                     event.get_location().getId(),
                     event.get_cnode()->getRegion().getDisplayName().getCString(),
                     coll.latest.time - event->getTimestamp());
        cbmanager.notify(CCV_COLL, event, data);
      }

      // Calculate waiting time (restricted to time spent in operation w/ccv)
      data->mIdle = std::min(coll.latest.time, event->getTimestamp()) - coll.my.time;
      if (data->mIdle > 0)
        m_severity[event.get_cnode()] += data->mIdle;

      // There will always be waiting time at barriers; all processes need
      // to take part in most-severe instance tracking
      cbmanager.notify(WAIT_FINALIZE, event, data);
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_BarrierWait
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_BarrierWait : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(SYNC_COLL, PEARL_create_callback(this, &PatternMPI_BarrierWait::sync_coll__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_BARRIER_WAIT;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_SYNC_COLLECTIVE;
    }

    virtual string get_name() const
    {
      return "MPI Wait at Barrier";
    }

    virtual string get_unique_name() const
    {
      return "mpi_barrier_wait";
    }

    virtual string get_descr() const
    {
      return "Waiting time in front of MPI barriers";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void sync_coll__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_BarrierWait::sync_coll__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 381 "MPI.pattern"

      const CollectiveInfo& coll(data->mCollinfo);

      // Validate clock condition
      if (coll.latest.time > event->getTimestamp()) {
        elg_cntl_msg("Unsynchronized clocks (loc: %" PRIu64 ", reg: %s, diff: %es)!\n",
                     event.get_location().getId(),
                     event.get_cnode()->getRegion().getDisplayName().getCString(),
                     coll.latest.time - event->getTimestamp());
        cbmanager.notify(CCV_COLL, event, data);
      }

      // Calculate waiting time (restricted to time spent in operation w/ccv)
      data->mIdle = std::min(coll.latest.time, event->getTimestamp()) - coll.my.time;
      if (data->mIdle > 0)
        m_severity[event.get_cnode()] += data->mIdle;

      // There will always be waiting time at barriers; all processes need
      // to take part in most-severe instance tracking
      cbmanager.notify(WAIT_BARRIER, event, data);
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_BarrierCompletion
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_BarrierCompletion : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(SYNC_COLL, PEARL_create_callback(this, &PatternMPI_BarrierCompletion::sync_coll__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_BARRIER_COMPLETION;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_SYNC_COLLECTIVE;
    }

    virtual string get_name() const
    {
      return "MPI Barrier Completion";
    }

    virtual string get_unique_name() const
    {
      return "mpi_barrier_completion";
    }

    virtual string get_descr() const
    {
      return "Time needed to finish MPI barriers";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void sync_coll__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_BarrierCompletion::sync_coll__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 430 "MPI.pattern"

      const CollectiveInfo& coll(data->mCollinfo);

      double min_time = coll.earliest_end.time;

      // Validate clock condition
      if (min_time < coll.latest.time) {
        // Do not report violation again -- already done by "Wait at Barrier"
        min_time = coll.latest.time;
      }

      // Calculate waiting time
      data->mIdle = event->getTimestamp() - min_time;
      if (data->mIdle > 0)
        m_severity[event.get_cnode()] += data->mIdle;

      // There will always be completion time at barriers; all processes need
      // to take part in most-severe instance tracking
      cbmanager.notify(BARRIER_COMPL, event, data);
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_P2P
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_P2P : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(PRE_SEND, PEARL_create_callback(this, &PatternMPI_P2P::pre_send__cb));
        (it->second)->register_callback(PRE_RECV, PEARL_create_callback(this, &PatternMPI_P2P::pre_recv__cb));
        (it->second)->register_callback(POST_RECV, PEARL_create_callback(this, &PatternMPI_P2P::post_recv__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_POINT2POINT;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_COMMUNICATION;
    }

    virtual string get_name() const
    {
      return "MPI point-to-point communication";
    }

    virtual string get_unique_name() const
    {
      return "mpi_point2point";
    }

    virtual string get_descr() const
    {
      return "Time spent in MPI point-to-point communication";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    virtual bool is_hidden() const
    {
      return true;
    }

    /* Callback methods */
    void post_recv__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_recv__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_send__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_P2P::post_recv__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 517 "MPI.pattern"

      // Validate clock condition
      RemoteEvent send = data->mRemote->get_event(ROLE_SEND);
      if (send->getTimestamp() > event->getTimestamp()) {
        elg_cntl_msg("Unsynchronized clocks (loc: %" PRIu64 ", reg: %s, diff: %es)!\n",
                     event.get_location().getId(),
                     event.get_cnode()->getRegion().getDisplayName().getCString(),
                     send->getTimestamp() - event->getTimestamp());
        cbmanager.notify(CCV_P2P, event, data);
      }
    
}

void PatternMPI_P2P::pre_recv__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 511 "MPI.pattern"

      data->mLocal->add_event(event, ROLE_RECV);
      data->mLocal->add_event(event.enterptr(), ROLE_ENTER_RECV);
      data->mLocal->add_event(event.leaveptr(), ROLE_LEAVE_RECV);
    
}

void PatternMPI_P2P::pre_send__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 506 "MPI.pattern"

      data->mLocal->add_event(event, ROLE_SEND);
      data->mLocal->add_event(event.enterptr(), ROLE_ENTER_SEND);
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_LateSender
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_LateSender : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(PRE_SEND, PEARL_create_callback(this, &PatternMPI_LateSender::pre_send__cb));
        (it->second)->register_callback(PRE_RECV, PEARL_create_callback(this, &PatternMPI_LateSender::pre_recv__cb));
        (it->second)->register_callback(POST_RECV, PEARL_create_callback(this, &PatternMPI_LateSender::post_recv__cb));
        (it->second)->register_callback(LEAVE, PEARL_create_callback(this, &PatternMPI_LateSender::leave__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_LATESENDER;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_POINT2POINT;
    }

    virtual string get_name() const
    {
      return "MPI Late Sender";
    }

    virtual string get_unique_name() const
    {
      return "mpi_latesender";
    }

    virtual string get_descr() const
    {
      return "Time in MPI point-to-point receive operation waiting for a message";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void leave__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void post_recv__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_recv__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_send__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
  protected:
    /* Protected methods */

    virtual void init()
    {

#line 567 "MPI.pattern"

    m_max_idle = 0.0;
  
    }


  private:

#line 562 "MPI.pattern"

    double         m_max_idle;
    EventSet       m_receive;
    RemoteEventSet m_send;
  
};


/*----- Callback methods -----*/

void PatternMPI_LateSender::leave__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 630 "MPI.pattern"

      if (m_max_idle > 0.0 &&
          is_mpi_wait_multi(event.enterptr()->getRegion())) {
        m_severity[event.get_cnode()] += m_max_idle;

        Event recv             = m_receive.get_event(ROLE_RECV);
        RemoteEvent send       = m_send.get_event(ROLE_SEND);
        RemoteEvent leave_send = m_send.get_event(ROLE_LEAVE_SEND_LS);

        data->mIdle = m_max_idle;

        data->mCompletion = 0;
        if (event->getTimestamp() > leave_send->getTimestamp())
          data->mCompletion = event->getTimestamp() - leave_send->getTimestamp();

        data->mLocal->add_event(recv, ROLE_RECV_LS);
        data->mRemote->add_event(send, ROLE_SEND_LS);

        cbmanager.notify(LATE_SENDER, recv, data);
      }

      m_max_idle = 0.0;
    
}

void PatternMPI_LateSender::post_recv__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 583 "MPI.pattern"

      RemoteEvent enter_send = data->mRemote->get_event(ROLE_ENTER_SEND_LS);
      RemoteEvent leave_send = data->mRemote->get_event(ROLE_LEAVE_SEND_LS);
      Event       enter_recv = data->mLocal->get_event(ROLE_ENTER_RECV_LS);
      Event       leave_recv = data->mLocal->get_event(ROLE_LEAVE_RECV_LS);

      const Region& region = enter_recv->getRegion();

      if (is_mpi_testx(region))
        return;

      // Validate clock condition
      pearl::timestamp_t max_time = enter_send->getTimestamp();
      if (max_time > leave_recv->getTimestamp()) {
        // Do not report violation again -- already done in generic P2P code
        max_time = leave_recv->getTimestamp();
      }

      // Calculate waiting time
      data->mIdle = max_time - enter_recv->getTimestamp();
      if (data->mIdle > 0) {
        if (is_mpi_wait_multi(region)) {
          if (data->mIdle > m_max_idle) {
            RemoteEvent send = data->mRemote->get_event(ROLE_SEND_LS);

            m_receive.clear();
            m_send.clear();

            m_max_idle = data->mIdle;
            m_receive.add_event(event, ROLE_RECV);
            m_send.add_event(send, ROLE_SEND);
            m_send.add_event(leave_send, ROLE_LEAVE_SEND_LS);

            cbmanager.notify(LATE_SENDER_CANDIDATE, event, data);
          }
        } else {
          m_severity[event.get_cnode()] += data->mIdle;

          data->mCompletion = 0;
          if (leave_recv->getTimestamp() > leave_send->getTimestamp())
            data->mCompletion = leave_recv->getTimestamp() - leave_send->getTimestamp();

          cbmanager.notify(LATE_SENDER, event, data);
        }
      }
    
}

void PatternMPI_LateSender::pre_recv__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 577 "MPI.pattern"

      data->mLocal->add_event(event, ROLE_RECV_LS);
      data->mLocal->add_event(event.enterptr(), ROLE_ENTER_RECV_LS);
      data->mLocal->add_event(event.leaveptr(), ROLE_LEAVE_RECV_LS);
    
}

void PatternMPI_LateSender::pre_send__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 571 "MPI.pattern"

      data->mLocal->add_event(event, ROLE_SEND_LS);
      data->mLocal->add_event(event.enterptr(), ROLE_ENTER_SEND_LS);
      data->mLocal->add_event(event.leaveptr(), ROLE_LEAVE_SEND_LS);
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_LateSenderWO
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_LateSenderWO : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(LATE_SENDER, PEARL_create_callback(this, &PatternMPI_LateSenderWO::late_sender__cb));
        (it->second)->register_callback(POST_RECV, PEARL_create_callback(this, &PatternMPI_LateSenderWO::post_recv__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_LATESENDER_WO;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_LATESENDER;
    }

    virtual string get_name() const
    {
      return "MPI Late Sender, wrong order";
    }

    virtual string get_unique_name() const
    {
      return "mpi_latesender_wo";
    }

    virtual string get_descr() const
    {
      return "Late Sender situation due to MPI messages received in the wrong order";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void late_sender__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void post_recv__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);


  private:

#line 685 "MPI.pattern"

    static const uint32_t BUFFERSIZE = 100;

    struct LateSender {
      LateSender(RemoteEvent send, Event recv, timestamp_t idle)
        : m_send(send), m_recv(recv), mIdle(idle) {}

      RemoteEvent m_send;
      Event       m_recv;
      timestamp_t mIdle;
    };
    typedef std::list<LateSender> LsBuffer;

    LsBuffer m_buffer;
  
};


/*----- Callback methods -----*/

void PatternMPI_LateSenderWO::late_sender__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 701 "MPI.pattern"

      // Construct entry
      LateSender item(data->mRemote->get_event(ROLE_SEND_LS),
                      event, data->mIdle);

      // Store entry in buffer
      if (m_buffer.size() == BUFFERSIZE)
        m_buffer.pop_front();
      m_buffer.push_back(item);
    
}

void PatternMPI_LateSenderWO::post_recv__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 712 "MPI.pattern"

      RemoteEvent send = data->mRemote->get_event(ROLE_SEND);

      // Search for "wrong order" situations
      LsBuffer::iterator it = m_buffer.begin();
      while (it != m_buffer.end()) {
        if (it->m_send->getTimestamp() > send->getTimestamp()) {
          double tmp = data->mIdle;

          data->mIdle = it->mIdle;
          m_severity[it->m_recv.get_cnode()] += data->mIdle;

          // Store data and notify specializations
          data->mRemote->add_event(it->m_send, ROLE_SEND_LSWO);
          data->mLocal->add_event(it->m_recv, ROLE_RECV_LSWO);
          cbmanager.notify(LATE_SENDER_WO, event, data);

          it = m_buffer.erase(it);

          data->mIdle = tmp;
        } else  {
          ++it;
        }
      }
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_LswoDifferent
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_LswoDifferent : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(LATE_SENDER_WO, PEARL_create_callback(this, &PatternMPI_LswoDifferent::late_sender_wo__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_LSWO_DIFFERENT;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_LATESENDER_WO;
    }

    virtual string get_name() const
    {
      return "MPI Late Sender, wrong order (different source)";
    }

    virtual string get_unique_name() const
    {
      return "mpi_lswo_different";
    }

    virtual string get_descr() const
    {
      return "Late Sender, Wrong Order situation due to MPI messages received from different sources";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void late_sender_wo__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_LswoDifferent::late_sender_wo__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 765 "MPI.pattern"

      Event recv = data->mLocal->get_event(ROLE_RECV_LSWO);

      if (recv->getSource() != event->getSource())
        m_severity[recv.get_cnode()] += data->mIdle;
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_LswoSame
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_LswoSame : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(LATE_SENDER_WO, PEARL_create_callback(this, &PatternMPI_LswoSame::late_sender_wo__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_LSWO_SAME;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_LATESENDER_WO;
    }

    virtual string get_name() const
    {
      return "MPI Late Sender, wrong order (same source)";
    }

    virtual string get_unique_name() const
    {
      return "mpi_lswo_same";
    }

    virtual string get_descr() const
    {
      return "Late Sender, Wrong Order situation due to MPI messages received from the same source";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void late_sender_wo__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_LswoSame::late_sender_wo__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 798 "MPI.pattern"

      Event recv = data->mLocal->get_event(ROLE_RECV_LSWO);

      if (recv->getSource() == event->getSource())
        m_severity[recv.get_cnode()] += data->mIdle;
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_LateReceiver
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_LateReceiver : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(LATE_SENDER_CANDIDATE, PEARL_create_callback(this, &PatternMPI_LateReceiver::late_sender_candidate__cb));
      }
      {
        CallbackManagerMap::iterator it = cbmgrs.find("bws");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(PRE_SEND, PEARL_create_callback(this, &PatternMPI_LateReceiver::pre_send_bws_cb));
        (it->second)->register_callback(POST_SEND, PEARL_create_callback(this, &PatternMPI_LateReceiver::post_send_bws_cb));
        (it->second)->register_callback(MPI_SEND_COMPLETE, PEARL_create_callback(this, &PatternMPI_LateReceiver::mpi_send_complete_bws_cb));
        (it->second)->register_callback(PRE_RECV, PEARL_create_callback(this, &PatternMPI_LateReceiver::pre_recv_bws_cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_LATERECEIVER;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_POINT2POINT;
    }

    virtual string get_name() const
    {
      return "MPI Late Receiver";
    }

    virtual string get_unique_name() const
    {
      return "mpi_latereceiver";
    }

    virtual string get_descr() const
    {
      return "Time in MPI point-to-point send operation waiting for the receiver to become ready";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void late_sender_candidate__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void mpi_send_complete_bws_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void post_send_bws_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_recv_bws_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_send_bws_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
  protected:
    /* Protected methods */

    virtual void init()
    {

#line 858 "MPI.pattern"

#ifdef DEBUG_MULTI_LR
    std::ostringstream fnamestr;
    fnamestr << epk_archive_get_name() << "/dbg_lr." << rank << ".dat";

    m_dbg_out.open(fnamestr.str().c_str(), std::fstream::app);
#endif
  
    }


  private:

#line 838 "MPI.pattern"

    struct multiwait_info_t {
      int    sendcount;
      Event  maxevent;
      double maxidle;
      double maxcmpl;
    };

    typedef std::set<pearl::Event, const pearl::EventKeyCompare>
      event_set_t;
    typedef std::map<Event, multiwait_info_t, pearl::EventKeyCompare>
      multiwait_map_t;

    event_set_t     m_ls_set;
    multiwait_map_t m_multiwait_map;

#ifdef DEBUG_MULTI_LR
    std::ofstream m_dbg_out;
#endif
  
};


/*----- Callback methods -----*/

void PatternMPI_LateReceiver::late_sender_candidate__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 867 "MPI.pattern"

      m_ls_set.insert(event.enterptr());
    
}

void PatternMPI_LateReceiver::mpi_send_complete_bws_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 936 "MPI.pattern"

      Event enter = event.enterptr();

      if (is_mpi_wait_multi(enter->getRegion()) && !m_ls_set.count(enter)) {
        multiwait_map_t::iterator it = m_multiwait_map.find(enter);

        if (it == m_multiwait_map.end()) {
          multiwait_info_t info;

          info.sendcount = 1;
          info.maxidle   = 0.0;

          m_multiwait_map.insert(std::make_pair(enter, info));
        } else {
          it->second.sendcount++;
        }
      }        
    
}

void PatternMPI_LateReceiver::post_send_bws_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 882 "MPI.pattern"

      // backward replay: msg received from destination here

      Event       enter_sendcmp = data->mLocal->get_event(ROLE_ENTER_SEND_LR);
      Event       leave_sendcmp = data->mLocal->get_event(ROLE_LEAVE_SEND_LR);
      RemoteEvent enter_recvreq = data->mRemote->get_event(ROLE_ENTER_RECV_LR);
      RemoteEvent leave_recvreq = data->mRemote->get_event(ROLE_LEAVE_RECV_LR);

      // Calculate waiting time
      data->mIdle = 
        leave_sendcmp->getTimestamp() <= enter_recvreq->getTimestamp() ? 0 : 
          enter_recvreq->getTimestamp() - enter_sendcmp->getTimestamp();

      if (data->mIdle > 0)
        data->mCompletion = 
          leave_sendcmp->getTimestamp() > leave_recvreq->getTimestamp() ? 0 :
            leave_sendcmp->getTimestamp() - leave_recvreq->getTimestamp();
      
      Region region = enter_sendcmp->getRegion();

      if (is_mpi_block_send(region) || is_mpi_wait_single(region)) {
        if (data->mIdle > 0) {
          m_severity[enter_sendcmp.get_cnode()] += data->mIdle;

          cbmanager.notify(LATE_RECEIVER, event, data);
        }
      } else if (is_mpi_wait_multi(region)) {
        multiwait_map_t::iterator it = m_multiwait_map.find(enter_sendcmp);

        if (it == m_multiwait_map.end())
          return;

        if (data->mIdle > it->second.maxidle) {
          it->second.maxidle  = data->mIdle;
          it->second.maxcmpl  = data->mCompletion;
          it->second.maxevent = event;
        }

        --it->second.sendcount;

        if (it->second.sendcount == 0) {
          if (it->second.maxidle > 0.0) {
            m_severity[it->first.get_cnode()] += it->second.maxidle;
            data->mIdle       = it->second.maxidle;
            data->mCompletion = it->second.maxcmpl;

            cbmanager.notify(LATE_RECEIVER, it->second.maxevent, data);
          }

          m_multiwait_map.erase(it);
        }  
      }
    
}

void PatternMPI_LateReceiver::pre_recv_bws_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 955 "MPI.pattern"

      data->mLocal->add_event(event, ROLE_RECV_LR);

      Event request = event.request();

      data->mLocal->add_event(request.enterptr(), ROLE_ENTER_RECV_LR);
      data->mLocal->add_event(request.leaveptr(), ROLE_LEAVE_RECV_LR);
    
}

void PatternMPI_LateReceiver::pre_send_bws_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 873 "MPI.pattern"

      data->mLocal->add_event(event, ROLE_SEND_LR);

      Event completion = event.completion();

      data->mLocal->add_event(completion.enterptr(), ROLE_ENTER_SEND_LR);
      data->mLocal->add_event(completion.leaveptr(),  ROLE_LEAVE_SEND_LR);
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_EarlyReduce
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_EarlyReduce : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(COLL_N21, PEARL_create_callback(this, &PatternMPI_EarlyReduce::coll_n21__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_EARLYREDUCE;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_COLLECTIVE;
    }

    virtual string get_name() const
    {
      return "MPI Early Reduce";
    }

    virtual string get_unique_name() const
    {
      return "mpi_earlyreduce";
    }

    virtual string get_descr() const
    {
      return "Waiting time due to an early receiver in MPI n-to-1 operations";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void coll_n21__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_EarlyReduce::coll_n21__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1037 "MPI.pattern"

      const CollectiveInfo& coll(data->mCollinfo);

      // Wait-state detection
      if (coll.my.rank == coll.root.rank && event->getBytesReceived() != 0) {
        // Validate clock condition
        if (coll.latest.time > event->getTimestamp()) {
          elg_cntl_msg("Unsynchronized clocks (loc: %" PRIu64 ", reg: %s, diff: %es)!\n",
                       event.get_location().getId(),
                       event.get_cnode()->getRegion().getDisplayName().getCString(),
                       coll.latest.time - event->getTimestamp());
          cbmanager.notify(CCV_COLL, event, data);
        }

        // Calculate waiting time (restricted to time spent in operation)
        data->mIdle = std::min(coll.earliest.time, event->getTimestamp()) - coll.my.time;

        if (data->mIdle > 0) {
          m_severity[event.get_cnode()] += data->mIdle;

          // Early Reduce time only occurs on the root process, i.e.,
          // most-severe instance tracking can be performed locally
          cbmanager.notify(EARLY_REDUCE, event, data);
        }
      }
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_EarlyScan
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_EarlyScan : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(COLL_SCAN, PEARL_create_callback(this, &PatternMPI_EarlyScan::coll_scan__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_EARLYSCAN;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_COLLECTIVE;
    }

    virtual string get_name() const
    {
      return "MPI Early Scan";
    }

    virtual string get_unique_name() const
    {
      return "mpi_earlyscan";
    }

    virtual string get_descr() const
    {
      return "Waiting time due to an early receiver in an MPI scan operation";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void coll_scan__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_EarlyScan::coll_scan__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1088 "MPI.pattern"

      // Ignore zero-sized transfers
      if (event->getBytesSent() == 0 && event->getBytesReceived() == 0)
        return;

      const CollectiveInfo& coll(data->mCollinfo);
      double max_time = coll.latest.time;

      // Validate clock condition
      if (max_time > event->getTimestamp()) {
        elg_cntl_msg("Unsynchronized clocks (loc: %" PRIu64 ", reg: %s, diff: %es)!\n",
                     event.get_location().getId(),
                     event.get_cnode()->getRegion().getDisplayName().getCString(),
                     max_time - event->getTimestamp());
        cbmanager.notify(CCV_COLL, event, data);

        // Restrict waiting time to time spent in operation
        max_time = event->getTimestamp();
      }

      // Calculate waiting time
      data->mIdle = max_time - coll.my.time;
      if (data->mIdle > 0)
        m_severity[event.get_cnode()] += data->mIdle;

      // All processes need to take part in the most-severe instance tracking
      cbmanager.notify(EARLY_SCAN, event, data);
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_LateBroadcast
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_LateBroadcast : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(COLL_12N, PEARL_create_callback(this, &PatternMPI_LateBroadcast::coll_12n__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_LATEBROADCAST;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_COLLECTIVE;
    }

    virtual string get_name() const
    {
      return "MPI Late Broadcast";
    }

    virtual string get_unique_name() const
    {
      return "mpi_latebroadcast";
    }

    virtual string get_descr() const
    {
      return "Waiting time due to a late sender in MPI 1-to-n operations";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void coll_12n__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_LateBroadcast::coll_12n__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1144 "MPI.pattern"

      const CollectiveInfo& coll(data->mCollinfo);

      // Wait-state detection (w/o root and zero-sized transfers)
      data->mIdle = 0.0;
      if (coll.my.rank != coll.root.rank && event->getBytesReceived() != 0) {
        // Validate clock condition
        if (coll.root.time > event->getTimestamp()) {
          elg_cntl_msg("Unsynchronized clocks (loc: %" PRIu64 ", reg: %s, diff: %es)!\n",
                       event.get_location().getId(),
                       event.get_cnode()->getRegion().getDisplayName().getCString(),
                       coll.root.time - event->getTimestamp());
          cbmanager.notify(CCV_COLL, event, data);
        }

        // Calculate waiting time
        data->mIdle = std::min(coll.root.time, event->getTimestamp()) - coll.my.time;
        if (data->mIdle > 0)
          m_severity[event.get_cnode()] += data->mIdle;
      }

      // All processes need to take part in the most-severe instance tracking
      cbmanager.notify(LATE_BCAST, event, data);
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_WaitNxN
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_WaitNxN : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(COLL_N2N, PEARL_create_callback(this, &PatternMPI_WaitNxN::coll_n2n__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_WAIT_NXN;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_COLLECTIVE;
    }

    virtual string get_name() const
    {
      return "MPI Wait at N x N";
    }

    virtual string get_unique_name() const
    {
      return "mpi_wait_nxn";
    }

    virtual string get_descr() const
    {
      return "Time due to inherent synchronization in MPI n-to-n operations";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void coll_n2n__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_WaitNxN::coll_n2n__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1202 "MPI.pattern"

      pearl::timestamp_t max_time = data->mCollinfo.latest.time;

      // Non-receiver processes do not have to wait
      if (event->getBytesReceived() == 0)
        max_time = -DBL_MAX;

      // Validate clock condition
      if (max_time > event->getTimestamp()) {
        elg_cntl_msg("Unsynchronized clocks (loc: %" PRIu64 ", reg: %s, diff: %es)!\n",
                     event.get_location().getId(),
                     event.get_cnode()->getRegion().getDisplayName().getCString(),
                     max_time - event->getTimestamp());
        cbmanager.notify(CCV_COLL, event, data);

        // Restrict waiting time to time spent in operation
        max_time = event->getTimestamp();
      }

      // Calculate waiting time
      Event begin = data->mLocal->get_event(ROLE_BEGIN_COLL);
      data->mIdle = max_time - begin->getTimestamp();
      if (data->mIdle > 0)
        m_severity[event.get_cnode()] += data->mIdle;

      // There will always be waiting time at NxN collectives; all processes
      // need to take part in most-severe instance tracking
      cbmanager.notify(WAIT_NXN, event, data);
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_NxNCompletion
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_NxNCompletion : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(COLL_N2N, PEARL_create_callback(this, &PatternMPI_NxNCompletion::coll_n2n__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_NXN_COMPLETION;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_COLLECTIVE;
    }

    virtual string get_name() const
    {
      return "MPI N x N Completion";
    }

    virtual string get_unique_name() const
    {
      return "mpi_nxn_completion";
    }

    virtual string get_descr() const
    {
      return "Time needed to finish an MPI n-to-n collective operation";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void coll_n2n__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_NxNCompletion::coll_n2n__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1259 "MPI.pattern"

      const CollectiveInfo& coll(data->mCollinfo);

      // Only an N'xN' synchronization pattern can have a completion time
      data->mIdle = 0.0;
      if (event->getBytesSent() != 0 && event->getBytesReceived() != 0) {
        // Calculate waiting time (restricted to time spent in operation w/ccv)
        data->mIdle = event->getTimestamp() - std::max(coll.earliest_end.time, coll.latest.time);
        if (data->mIdle > 0)
          m_severity[event.get_cnode()] += data->mIdle;
      }

      // All processes need to take part in most-severe instance tracking
      cbmanager.notify(NXN_COMPL, event, data);
    
}

#endif   /* _MPI */

#if defined(_OPENMP)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternOMP_Mgmt
 *
 *---------------------------------------------------------------------------
 */


class PatternOMP_Mgmt : public OmpPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(THREAD_FORK, PEARL_create_callback(this, &PatternOMP_Mgmt::thread_fork__cb));
        (it->second)->register_callback(THREAD_JOIN, PEARL_create_callback(this, &PatternOMP_Mgmt::thread_join__cb));
        (it->second)->register_callback(GROUP_ENTER, PEARL_create_callback(this, &PatternOMP_Mgmt::group_enter__cb));
        (it->second)->register_callback(LEAVE, PEARL_create_callback(this, &PatternOMP_Mgmt::leave__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_OMP_MANAGEMENT;
    }

    virtual long get_parent() const
    {
      return PAT_OMP_TIME;
    }

    virtual string get_name() const
    {
      return "OMP Management";
    }

    virtual string get_unique_name() const
    {
      return "omp_management";
    }

    virtual string get_descr() const
    {
      return "Time spent in OpenMP thread management";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void group_enter__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void leave__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void thread_fork__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void thread_join__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternOMP_Mgmt::group_enter__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 162 "OMP.pattern"

      // Are we entering an OpenMP parallel region?
      const Region& region = event->getRegion();
      if (!is_omp_parallel(region))
        return;

      // Determine timestamp of FORK event
      static timestamp_t forkTimestamp;
      #pragma omp master
      {
        Event fork = event.prev();
        while (fork->getType() != THREAD_FORK)
          --fork;
        forkTimestamp = fork->getTimestamp();
      }

      // Calculate thread fork time
      #pragma omp barrier
      data->mIdle = std::max(0.0, event->getTimestamp() - forkTimestamp);
      m_severity[event.get_cnode()] += data->mIdle;
      cbmanager.notify(OMP_MGMT_FORK, event, data);
      #pragma omp barrier
    
}

void PatternOMP_Mgmt::leave__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 186 "OMP.pattern"

      // Are we leaving an OpenMP parallel region?
      Event   enter  = data->mCallstack->top();
      const Region& region = enter->getRegion();
      if (!is_omp_parallel(region))
        return;

      // Determine timestamp of JOIN event
      static timestamp_t joinTimestamp;
      #pragma omp master
      {
        Event join = event.next();
        while (join->getType() != THREAD_JOIN)
          ++join;

        joinTimestamp = join->getTimestamp();
      }

      // Calculate thread join time
      #pragma omp barrier
      data->mIdle = std::max(0.0, joinTimestamp - event->getTimestamp());
      m_severity[enter.get_cnode()] += data->mIdle;
      cbmanager.notify(OMP_MGMT_JOIN, event, data);
      #pragma omp barrier
    
}

void PatternOMP_Mgmt::thread_fork__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 152 "OMP.pattern"

      // Increment nesting level
      omp_nest_level++;
    
}

void PatternOMP_Mgmt::thread_join__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 157 "OMP.pattern"

      // Decrement nesting level
      omp_nest_level--;
    
}

#endif   /* _OPENMP */

#if defined(_OPENMP)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternOMP_Mgmt_Fork
 *
 *---------------------------------------------------------------------------
 */


class PatternOMP_Mgmt_Fork : public OmpPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(OMP_MGMT_FORK, PEARL_create_callback(this, &PatternOMP_Mgmt_Fork::omp_mgmt_fork__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_OMP_FORK;
    }

    virtual long get_parent() const
    {
      return PAT_OMP_MANAGEMENT;
    }

    virtual string get_name() const
    {
      return "OMP Fork";
    }

    virtual string get_unique_name() const
    {
      return "omp_fork";
    }

    virtual string get_descr() const
    {
      return "Time spent in forking OpenMP thread teams";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void omp_mgmt_fork__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternOMP_Mgmt_Fork::omp_mgmt_fork__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 230 "OMP.pattern"

      m_severity[event.get_cnode()] += data->mIdle;
    
}

#endif   /* _OPENMP */

#if defined(_OPENMP)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternOMP_Sync_Barrier
 *
 *---------------------------------------------------------------------------
 */


class PatternOMP_Sync_Barrier : public OmpPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(TASK_CORRECTION, PEARL_create_callback(this, &PatternOMP_Sync_Barrier::task_correction__cb));
        (it->second)->register_callback(ENTER, PEARL_create_callback(this, &PatternOMP_Sync_Barrier::enter__cb));
        (it->second)->register_callback(LEAVE, PEARL_create_callback(this, &PatternOMP_Sync_Barrier::leave__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_OMP_BARRIER;
    }

    virtual long get_parent() const
    {
      return PAT_OMP_SYNCHRONIZATION;
    }

    virtual string get_name() const
    {
      return "OMP Barrier";
    }

    virtual string get_unique_name() const
    {
      return "omp_barrier";
    }

    virtual string get_descr() const
    {
      return "Time spent in OpenMP barrier synchronization";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    virtual bool is_hidden() const
    {
      return true;
    }

    /* Callback methods */
    void enter__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void leave__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void task_correction__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);


  private:

#line 276 "OMP.pattern"

    double mTaskExecutionTime;
  
};


/*----- Callback methods -----*/

void PatternOMP_Sync_Barrier::enter__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 284 "OMP.pattern"

      const Region& region = event.get_cnode()->getRegion();
      if (!is_omp_barrier(region))
        return;

      // Reset task execution time when entering the barrier
      mTaskExecutionTime = 0;
    
}

void PatternOMP_Sync_Barrier::leave__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 293 "OMP.pattern"

      // Are we leaving an (implicit or explicit) OpenMP barrier?
      Event         enter  = data->mCallstack->top();
      const Region& region = enter->getRegion();
      if (!is_omp_barrier(region))
        return;

      // Skip OpenMP barrier waiting time calculation when running serially
      // (the barriers called in the algorithm cause ill side effects...)
      if (0 == omp_nest_level)
        return;

      // Determine latest enter time
      timestamp_t enterTime = enter->getTimestamp();
      // Implicitly shared timestamp for reduction
      static timestamp_t latestEnter;
      #pragma omp barrier
      #pragma omp master
      latestEnter = 0;
      #pragma omp barrier
      #pragma omp critical
      {
        if(latestEnter < enterTime)
          latestEnter = enterTime;
      }
      #pragma omp barrier

      // Calculate waiting time
      data->mIdle = latestEnter - enterTime - mTaskExecutionTime;

      // Notify sub-patterns
      if (is_omp_ebarrier(region)) {
        cbmanager.notify(OMP_EBARRIER, event, data);
      } else if (is_omp_ibarrier(region)) {
        cbmanager.notify(OMP_IBARRIER, event, data);
      }
    
}

void PatternOMP_Sync_Barrier::task_correction__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 280 "OMP.pattern"

       mTaskExecutionTime += data->mIdle; 
     
}

#endif   /* _OPENMP */

#if defined(_OPENMP)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternOMP_Sync_Ebarrier_Wait
 *
 *---------------------------------------------------------------------------
 */


class PatternOMP_Sync_Ebarrier_Wait : public OmpPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(OMP_EBARRIER, PEARL_create_callback(this, &PatternOMP_Sync_Ebarrier_Wait::omp_ebarrier__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_OMP_EBARRIER_WAIT;
    }

    virtual long get_parent() const
    {
      return PAT_OMP_EBARRIER;
    }

    virtual string get_name() const
    {
      return "OMP Wait at Explicit Barrier";
    }

    virtual string get_unique_name() const
    {
      return "omp_ebarrier_wait";
    }

    virtual string get_descr() const
    {
      return "Waiting time in front of explicit OpenMP barriers";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void omp_ebarrier__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternOMP_Sync_Ebarrier_Wait::omp_ebarrier__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 381 "OMP.pattern"

      Event enter = data->mCallstack->top();

      if(data->mIdle > 0)
        m_severity[enter.get_cnode()] += data->mIdle;

      // There will always be waiting time at barriers; all threads need
      // to take part in most-severe instance tracking
      cbmanager.notify(OMP_EBARRIER_WAIT, event, data);
    
}

#endif   /* _OPENMP */

#if defined(_OPENMP)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternOMP_Sync_Ibarrier_Wait
 *
 *---------------------------------------------------------------------------
 */


class PatternOMP_Sync_Ibarrier_Wait : public OmpPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(OMP_IBARRIER, PEARL_create_callback(this, &PatternOMP_Sync_Ibarrier_Wait::omp_ibarrier__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_OMP_IBARRIER_WAIT;
    }

    virtual long get_parent() const
    {
      return PAT_OMP_IBARRIER;
    }

    virtual string get_name() const
    {
      return "OMP Wait at Implicit Barrier";
    }

    virtual string get_unique_name() const
    {
      return "omp_ibarrier_wait";
    }

    virtual string get_descr() const
    {
      return "Waiting time in front of implicit OpenMP barriers";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void omp_ibarrier__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternOMP_Sync_Ibarrier_Wait::omp_ibarrier__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 442 "OMP.pattern"

      Event enter = data->mCallstack->top();

      if(data->mIdle > 0)
        m_severity[enter.get_cnode()] += data->mIdle;

      // There will always be waiting time at barriers; all threads need
      // to take part in most-severe instance tracking
      cbmanager.notify(OMP_IBARRIER_WAIT, event, data);
    
}

#endif   /* _OPENMP */

#if defined(_OPENMP)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternOMP_Lock_Contention_Critical
 *
 *---------------------------------------------------------------------------
 */


class PatternOMP_Lock_Contention_Critical : public OmpPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(OMP_CONTENTION_CRITICAL, PEARL_create_callback(this, &PatternOMP_Lock_Contention_Critical::omp_contention_critical__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_OMP_LOCK_CONTENTION_CRITICAL;
    }

    virtual long get_parent() const
    {
      return PAT_OMP_CRITICAL;
    }

    virtual string get_name() const
    {
      return "OMP Critical Contention";
    }

    virtual string get_unique_name() const
    {
      return "omp_lock_contention_critical";
    }

    virtual string get_descr() const
    {
      return "Time lost waiting for an OpenMP critical section to become available";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void omp_contention_critical__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternOMP_Lock_Contention_Critical::omp_contention_critical__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 501 "OMP.pattern"

      if (data->mIdle > 0) { 
        m_severity[event.get_cnode()] += data->mIdle;
        //instance tracking
        cbmanager.notify(THREAD_LOCK_CONTENTION_OMPCRITICAL_WAIT, event, data);
     }
    
}

#endif   /* _OPENMP */

#if defined(_OPENMP)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternOMP_Lock_Contention_API
 *
 *---------------------------------------------------------------------------
 */


class PatternOMP_Lock_Contention_API : public OmpPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(OMP_CONTENTION_API, PEARL_create_callback(this, &PatternOMP_Lock_Contention_API::omp_contention_api__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_OMP_LOCK_CONTENTION_API;
    }

    virtual long get_parent() const
    {
      return PAT_OMP_LOCK_API;
    }

    virtual string get_name() const
    {
      return "OMP Lock API Contention";
    }

    virtual string get_unique_name() const
    {
      return "omp_lock_contention_api";
    }

    virtual string get_descr() const
    {
      return "Time lost waiting for an OpenMP lock to be acquired";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void omp_contention_api__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternOMP_Lock_Contention_API::omp_contention_api__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 556 "OMP.pattern"

      if (data->mIdle > 0) { 
        m_severity[event.get_cnode()] += data->mIdle;
        //instance tracking
        cbmanager.notify(THREAD_LOCK_CONTENTION_OMPLOCK_WAIT, event, data);
      }
    
}

#endif   /* _OPENMP */

/*
 *---------------------------------------------------------------------------
 *
 * class PatternPthread_Lock_Contention_Mutex_Lock
 *
 *---------------------------------------------------------------------------
 */


class PatternPthread_Lock_Contention_Mutex_Lock : public Pattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(PTHREAD_CONTENTION_MUTEX_LOCK, PEARL_create_callback(this, &PatternPthread_Lock_Contention_Mutex_Lock::pthread_contention_mutex_lock__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_PTHREAD_LOCK_CONTENTION_MUTEX_LOCK;
    }

    virtual long get_parent() const
    {
      return PAT_PTHREAD_LOCK_API;
    }

    virtual string get_name() const
    {
      return "Pthread Mutex Lock Contention";
    }

    virtual string get_unique_name() const
    {
      return "pthread_lock_contention_mutex_lock";
    }

    virtual string get_descr() const
    {
      return "Time lost waiting for a POSIX threads mutex to be acquired";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void pthread_contention_mutex_lock__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternPthread_Lock_Contention_Mutex_Lock::pthread_contention_mutex_lock__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 133 "Pthread.pattern"

      if (data->mIdle > 0) { 
        m_severity[event.get_cnode()] += data->mIdle;
        //instance tracking
        cbmanager.notify(THREAD_LOCK_CONTENTION_PTHREADMUTEX_WAIT, event, data);
      }
    
}


/*
 *---------------------------------------------------------------------------
 *
 * class PatternPthread_Lock_Contention_Conditional
 *
 *---------------------------------------------------------------------------
 */


class PatternPthread_Lock_Contention_Conditional : public Pattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(PTHREAD_CONTENTION_CONDITIONAL, PEARL_create_callback(this, &PatternPthread_Lock_Contention_Conditional::pthread_contention_conditional__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_PTHREAD_LOCK_CONTENTION_CONDITIONAL;
    }

    virtual long get_parent() const
    {
      return PAT_PTHREAD_CONDITIONAL;
    }

    virtual string get_name() const
    {
      return "Pthread Condition Lock Contention";
    }

    virtual string get_unique_name() const
    {
      return "pthread_lock_contention_conditional";
    }

    virtual string get_descr() const
    {
      return "Time lost waiting for a POSIX threads mutex to be acquired in a condition operation";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void pthread_contention_conditional__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternPthread_Lock_Contention_Conditional::pthread_contention_conditional__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 186 "Pthread.pattern"

      if (data->mIdle > 0) { 
        m_severity[event.get_cnode()] += data->mIdle;
        //instance tracking
        cbmanager.notify(THREAD_LOCK_CONTENTION_PTHREADCONDITION_WAIT, event, data);
      }
    
}


#if defined(_OPENMP)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternThread_LockContention
 *
 *---------------------------------------------------------------------------
 */


class PatternThread_LockContention : public OmpPattern
{
  public:

    /* Constructors & destructor */
    virtual ~PatternThread_LockContention()
    {

#line 66 "Thread.pattern"

   PEARL_Barrier(); 
   if (PEARL_GetThreadNumber() == 0) {
     delete mOpenmpLockTracking; 
     delete mPthreadLockTracking; 
   }
  
    }
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(THREAD_ACQUIRE_LOCK, PEARL_create_callback(this, &PatternThread_LockContention::thread_acquire_lock__cb));
        (it->second)->register_callback(THREAD_RELEASE_LOCK, PEARL_create_callback(this, &PatternThread_LockContention::thread_release_lock__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_THREAD_LOCK_CONTENTION;
    }

    virtual long get_parent() const
    {
      return PAT_NONE;
    }

    virtual string get_name() const
    {
      return "Thread Lock Contention";
    }

    virtual string get_unique_name() const
    {
      return "thread_lock_contention";
    }

    virtual string get_descr() const
    {
      return "Time spent in thread lock contention";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    virtual bool is_hidden() const
    {
      return true;
    }

    /* Callback methods */
    void thread_acquire_lock__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void thread_release_lock__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
  protected:
    /* Protected methods */

    virtual void init()
    {

#line 59 "Thread.pattern"

   if (PEARL_GetThreadNumber() == 0) {
     mOpenmpLockTracking = new scout::LockTracking(); 
     mPthreadLockTracking = new scout::LockTracking();
   }
   PEARL_Barrier(); 
  
    }


  private:

#line 35 "Thread.pattern"

    LockTracking *getLockTrackingVariant(Paradigm paradigm)
    {
      switch (native_value(paradigm)) {
        case Paradigm::OPENMP:
          return mOpenmpLockTracking;
          break;

        case Paradigm::PTHREAD:
          return mPthreadLockTracking;
          break;

        default:
          return NULL;
      }
    }
    
    static LockTracking *mOpenmpLockTracking;
    static LockTracking *mPthreadLockTracking;
  
};


/*----- Static class data -----*/


#line 55 "Thread.pattern"

    LockTracking *PatternThread_LockContention::mOpenmpLockTracking;
    LockTracking *PatternThread_LockContention::mPthreadLockTracking;
  


/*----- Callback methods -----*/

void PatternThread_LockContention::thread_acquire_lock__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 74 "Thread.pattern"

      Paradigm paradigm = getLockParadigm(event);

      LockTracking* lockMap = getLockTrackingVariant(paradigm);
      if (lockMap == NULL ){
        // unsupported paradigm 
        return;
      }

      // wait time=last release time  - respective Enter timestamp
      data->mIdle = lockMap->acquireLock(event)
                    - event.enterptr()->getTimestamp();

      LockType lockType = getLockType(event);
      // distribute time dependent on lock type
      if (lockType == LockType::OMP_LOCK_API)
      {
        cbmanager.notify(OMP_CONTENTION_API, event, data);
      }         
      else if (lockType == LockType::OMP_CRITICAL)
      {
        cbmanager.notify(OMP_CONTENTION_CRITICAL, event, data);
      }
      else if (lockType == LockType::PTHREAD_MUTEX_LOCK)
      {
        cbmanager.notify(PTHREAD_CONTENTION_MUTEX_LOCK, event, data);
      } 
      else if (lockType == LockType::PTHREAD_MUTEX_TRYLOCK)
      {
        cbmanager.notify(PTHREAD_CONTENTION_MUTEX_LOCK, event, data);
      }
      else if (lockType == LockType::PTHREAD_CONDITION_WAIT)
      {
        cbmanager.notify(PTHREAD_CONTENTION_CONDITIONAL, event, data);
      }
    
}

void PatternThread_LockContention::thread_release_lock__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 111 "Thread.pattern"
  
      Paradigm paradigm  = getLockParadigm(event);

      LockTracking* lockMap = getLockTrackingVariant(paradigm);
      if (lockMap == NULL ){
        // unsupported paradigm 
        return;
      }
      lockMap->releaseLock(event);
    
}

#endif   /* _OPENMP */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_SyncsSend
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_SyncsSend : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(GROUP_SEND, PEARL_create_callback(this, &PatternMPI_SyncsSend::group_send__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_SYNCS_SEND;
    }

    virtual long get_parent() const
    {
      return PAT_SYNCS_P2P;
    }

    virtual string get_name() const
    {
      return "MPI P2P send synchronizations";
    }

    virtual string get_unique_name() const
    {
      return "syncs_send";
    }

    virtual string get_descr() const
    {
      return "Number of MPI point-to-point send synchronization operations";
    }

    virtual string get_unit() const
    {
      return "occ";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void group_send__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_SyncsSend::group_send__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 73 "Message.pattern"

      if (event->getBytesSent() == 0)
        ++m_severity[event.get_cnode()];
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_SyncsRecv
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_SyncsRecv : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(POST_RECV, PEARL_create_callback(this, &PatternMPI_SyncsRecv::post_recv__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_SYNCS_RECV;
    }

    virtual long get_parent() const
    {
      return PAT_SYNCS_P2P;
    }

    virtual string get_name() const
    {
      return "MPI P2P recv synchronizations";
    }

    virtual string get_unique_name() const
    {
      return "syncs_recv";
    }

    virtual string get_descr() const
    {
      return "Number of MPI point-to-point receive synchronization operations";
    }

    virtual string get_unit() const
    {
      return "occ";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void post_recv__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_SyncsRecv::post_recv__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 95 "Message.pattern"

      RemoteEvent send = data->mRemote->get_event(ROLE_SEND);

      if (send->getBytesSent() == 0)
        ++m_severity[event.get_cnode()];
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_SyncsColl
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_SyncsColl : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(MPI_COLLECTIVE_END, PEARL_create_callback(this, &PatternMPI_SyncsColl::mpi_collective_end__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_SYNCS_COLL;
    }

    virtual long get_parent() const
    {
      return PAT_SYNCS;
    }

    virtual string get_name() const
    {
      return "MPI collective synchronizations";
    }

    virtual string get_unique_name() const
    {
      return "syncs_coll";
    }

    virtual string get_descr() const
    {
      return "Number of MPI collective synchronization operations";
    }

    virtual string get_unit() const
    {
      return "occ";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void mpi_collective_end__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_SyncsColl::mpi_collective_end__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 130 "Message.pattern"

      if (event->getBytesSent() == 0 && event->getBytesReceived() == 0)
        ++m_severity[event.get_cnode()];
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_CommsSend
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_CommsSend : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(GROUP_SEND, PEARL_create_callback(this, &PatternMPI_CommsSend::group_send__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_COMMS_SEND;
    }

    virtual long get_parent() const
    {
      return PAT_COMMS_P2P;
    }

    virtual string get_name() const
    {
      return "MPI P2P send communications";
    }

    virtual string get_unique_name() const
    {
      return "comms_send";
    }

    virtual string get_descr() const
    {
      return "Number of MPI point-to-point send communication operations";
    }

    virtual string get_unit() const
    {
      return "occ";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void group_send__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_CommsSend::group_send__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 233 "Message.pattern"

      if (event->getBytesSent() != 0)
        ++m_severity[event.get_cnode()];
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_CommsRecv
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_CommsRecv : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(POST_RECV, PEARL_create_callback(this, &PatternMPI_CommsRecv::post_recv__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_COMMS_RECV;
    }

    virtual long get_parent() const
    {
      return PAT_COMMS_P2P;
    }

    virtual string get_name() const
    {
      return "MPI P2P recv communications";
    }

    virtual string get_unique_name() const
    {
      return "comms_recv";
    }

    virtual string get_descr() const
    {
      return "Number of MPI point-to-point receive communication operations";
    }

    virtual string get_unit() const
    {
      return "occ";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void post_recv__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_CommsRecv::post_recv__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 255 "Message.pattern"

      RemoteEvent send = data->mRemote->get_event(ROLE_SEND);

      if (send->getBytesSent() != 0)
        ++m_severity[event.get_cnode()];
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_CommsExch
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_CommsExch : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(MPI_COLLECTIVE_END, PEARL_create_callback(this, &PatternMPI_CommsExch::mpi_collective_end__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_COMMS_CXCH;
    }

    virtual long get_parent() const
    {
      return PAT_COMMS_COLL;
    }

    virtual string get_name() const
    {
      return "MPI collective exchange communications";
    }

    virtual string get_unique_name() const
    {
      return "comms_cxch";
    }

    virtual string get_descr() const
    {
      return "Number of MPI collective communications as source and destination";
    }

    virtual string get_unit() const
    {
      return "occ";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void mpi_collective_end__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_CommsExch::mpi_collective_end__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 305 "Message.pattern"

      if (event->getBytesSent() != 0 && event->getBytesReceived() != 0)
        ++m_severity[event.get_cnode()];
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_CommsSrc
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_CommsSrc : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(MPI_COLLECTIVE_END, PEARL_create_callback(this, &PatternMPI_CommsSrc::mpi_collective_end__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_COMMS_CSRC;
    }

    virtual long get_parent() const
    {
      return PAT_COMMS_COLL;
    }

    virtual string get_name() const
    {
      return "MPI collective communications as source";
    }

    virtual string get_unique_name() const
    {
      return "comms_csrc";
    }

    virtual string get_descr() const
    {
      return "Number of MPI collective communications as source";
    }

    virtual string get_unit() const
    {
      return "occ";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void mpi_collective_end__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_CommsSrc::mpi_collective_end__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 328 "Message.pattern"

      if (event->getBytesSent() != 0 && event->getBytesReceived() == 0)
        ++m_severity[event.get_cnode()];
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_CommsDst
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_CommsDst : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(MPI_COLLECTIVE_END, PEARL_create_callback(this, &PatternMPI_CommsDst::mpi_collective_end__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_COMMS_CDST;
    }

    virtual long get_parent() const
    {
      return PAT_COMMS_COLL;
    }

    virtual string get_name() const
    {
      return "MPI collective communications as destination";
    }

    virtual string get_unique_name() const
    {
      return "comms_cdst";
    }

    virtual string get_descr() const
    {
      return "Number of MPI collective communications as destination";
    }

    virtual string get_unit() const
    {
      return "occ";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void mpi_collective_end__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_CommsDst::mpi_collective_end__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 351 "Message.pattern"

      if (event->getBytesSent() == 0 && event->getBytesReceived() != 0)
        ++m_severity[event.get_cnode()];
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_BytesSent
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_BytesSent : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(GROUP_SEND, PEARL_create_callback(this, &PatternMPI_BytesSent::group_send__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_BYTES_SENT;
    }

    virtual long get_parent() const
    {
      return PAT_BYTES_P2P;
    }

    virtual string get_name() const
    {
      return "MPI P2P bytes sent";
    }

    virtual string get_unique_name() const
    {
      return "bytes_sent";
    }

    virtual string get_descr() const
    {
      return "Number of bytes sent in MPI point-to-point communication operations";
    }

    virtual string get_unit() const
    {
      return "bytes";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void group_send__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_BytesSent::group_send__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 517 "Message.pattern"

      if (event->getBytesSent() > 0)
        m_severity[event.get_cnode()] += event->getBytesSent();
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_BytesRcvd
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_BytesRcvd : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(POST_RECV, PEARL_create_callback(this, &PatternMPI_BytesRcvd::post_recv__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_BYTES_RCVD;
    }

    virtual long get_parent() const
    {
      return PAT_BYTES_P2P;
    }

    virtual string get_name() const
    {
      return "MPI P2P bytes received";
    }

    virtual string get_unique_name() const
    {
      return "bytes_rcvd";
    }

    virtual string get_descr() const
    {
      return "Number of bytes received in MPI point-to-point communication operations";
    }

    virtual string get_unit() const
    {
      return "bytes";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void post_recv__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_BytesRcvd::post_recv__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 557 "Message.pattern"

      RemoteEvent send = data->mRemote->get_event(ROLE_SEND);

      if (send->getBytesSent() > 0)
        m_severity[event.get_cnode()] += send->getBytesSent();
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_BytesCout
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_BytesCout : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(MPI_COLLECTIVE_END, PEARL_create_callback(this, &PatternMPI_BytesCout::mpi_collective_end__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_BYTES_COUT;
    }

    virtual long get_parent() const
    {
      return PAT_BYTES_COLL;
    }

    virtual string get_name() const
    {
      return "MPI collective bytes outgoing";
    }

    virtual string get_unique_name() const
    {
      return "bytes_cout";
    }

    virtual string get_descr() const
    {
      return "Number of bytes outgoing in MPI collective communication operations";
    }

    virtual string get_unit() const
    {
      return "bytes";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void mpi_collective_end__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_BytesCout::mpi_collective_end__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 615 "Message.pattern"

      uint64_t bytesSent = event->getBytesSent();
      if (bytesSent > 0)
        m_severity[event.get_cnode()] += bytesSent;
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_BytesCin
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_BytesCin : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(MPI_COLLECTIVE_END, PEARL_create_callback(this, &PatternMPI_BytesCin::mpi_collective_end__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_BYTES_CIN;
    }

    virtual long get_parent() const
    {
      return PAT_BYTES_COLL;
    }

    virtual string get_name() const
    {
      return "MPI collective bytes incoming";
    }

    virtual string get_unique_name() const
    {
      return "bytes_cin";
    }

    virtual string get_descr() const
    {
      return "Number of bytes incoming in MPI collective communication operations";
    }

    virtual string get_unit() const
    {
      return "bytes";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void mpi_collective_end__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_BytesCin::mpi_collective_end__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 643 "Message.pattern"

      uint64_t bytesReceived = event->getBytesReceived();
      if (bytesReceived > 0)
        m_severity[event.get_cnode()] += bytesReceived;
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_BytesPut
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_BytesPut : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(MPI_RMA_PUT_START, PEARL_create_callback(this, &PatternMPI_BytesPut::mpi_rma_put_start__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_BYTES_PUT;
    }

    virtual long get_parent() const
    {
      return PAT_BYTES_RMA;
    }

    virtual string get_name() const
    {
      return "MPI one-sided bytes sent";
    }

    virtual string get_unique_name() const
    {
      return "bytes_put";
    }

    virtual string get_descr() const
    {
      return "Number of bytes sent in MPI one-sided communication operations";
    }

    virtual string get_unit() const
    {
      return "bytes";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void mpi_rma_put_start__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_BytesPut::mpi_rma_put_start__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 694 "Message.pattern"

      m_severity[event.get_cnode()] += event->getBytesSent();
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_BytesGet
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_BytesGet : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(MPI_RMA_GET_START, PEARL_create_callback(this, &PatternMPI_BytesGet::mpi_rma_get_start__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_BYTES_GET;
    }

    virtual long get_parent() const
    {
      return PAT_BYTES_RMA;
    }

    virtual string get_name() const
    {
      return "MPI one-sided bytes received";
    }

    virtual string get_unique_name() const
    {
      return "bytes_get";
    }

    virtual string get_descr() const
    {
      return "Number of bytes received in MPI one-sided communication operations";
    }

    virtual string get_unit() const
    {
      return "bytes";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void mpi_rma_get_start__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_BytesGet::mpi_rma_get_start__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 720 "Message.pattern"

      m_severity[event.get_cnode()] += event->getBytesReceived();
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_SLR_Count
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_SLR_Count : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("bws");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(LATE_RECEIVER, PEARL_create_callback(this, &PatternMPI_SLR_Count::late_receiver_bws_cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_SLR_COUNT;
    }

    virtual long get_parent() const
    {
      return PAT_SYNCS_SEND;
    }

    virtual string get_name() const
    {
      return "MPI Late Receiver Instances (Synchronizations)";
    }

    virtual string get_unique_name() const
    {
      return "mpi_slr_count";
    }

    virtual string get_descr() const
    {
      return "Number of Late Receiver instances in MPI synchronizations";
    }

    virtual string get_unit() const
    {
      return "occ";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void late_receiver_bws_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_SLR_Count::late_receiver_bws_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 31 "Counts.pattern"

      Event completion = data->mLocal->get_event(ROLE_ENTER_SEND_LR);

      if (event->getBytesSent() == 0)
        ++m_severity[completion.get_cnode()];
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_SLS_Count
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_SLS_Count : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(LATE_SENDER, PEARL_create_callback(this, &PatternMPI_SLS_Count::late_sender__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_SLS_COUNT;
    }

    virtual long get_parent() const
    {
      return PAT_SYNCS_RECV;
    }

    virtual string get_name() const
    {
      return "MPI Late Sender Instances (Synchronizations)";
    }

    virtual string get_unique_name() const
    {
      return "mpi_sls_count";
    }

    virtual string get_descr() const
    {
      return "Number of Late Sender instances in MPI synchronizations";
    }

    virtual string get_unit() const
    {
      return "occ";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void late_sender__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_SLS_Count::late_sender__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 55 "Counts.pattern"

      RemoteEvent send = data->mRemote->get_event(ROLE_SEND_LS);

      if (send->getBytesSent() == 0)
        ++m_severity[event.get_cnode()];
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_SLSWO_Count
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_SLSWO_Count : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(LATE_SENDER_WO, PEARL_create_callback(this, &PatternMPI_SLSWO_Count::late_sender_wo__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_SLSWO_COUNT;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_SLS_COUNT;
    }

    virtual string get_name() const
    {
      return "MPI Late Sender, Wrong Order Instances (Synchronizations)";
    }

    virtual string get_unique_name() const
    {
      return "mpi_slswo_count";
    }

    virtual string get_descr() const
    {
      return "Number of Late Sender instances in MPI synchronizations where messages are received in wrong order";
    }

    virtual string get_unit() const
    {
      return "occ";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void late_sender_wo__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_SLSWO_Count::late_sender_wo__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 80 "Counts.pattern"

      RemoteEvent send = data->mRemote->get_event(ROLE_SEND_LSWO);
      Event       recv = data->mLocal->get_event(ROLE_RECV_LSWO);

      if (send->getBytesSent() == 0)
        ++m_severity[recv.get_cnode()];
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_CLR_Count
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_CLR_Count : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("bws");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(LATE_RECEIVER, PEARL_create_callback(this, &PatternMPI_CLR_Count::late_receiver_bws_cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_CLR_COUNT;
    }

    virtual long get_parent() const
    {
      return PAT_COMMS_SEND;
    }

    virtual string get_name() const
    {
      return "MPI Late Receiver Instances (Communications)";
    }

    virtual string get_unique_name() const
    {
      return "mpi_clr_count";
    }

    virtual string get_descr() const
    {
      return "Number of Late Receiver instances in MPI communications";
    }

    virtual string get_unit() const
    {
      return "occ";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void late_receiver_bws_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_CLR_Count::late_receiver_bws_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 105 "Counts.pattern"

      Event completion = data->mLocal->get_event(ROLE_ENTER_SEND_LR);

      if (event->getBytesSent() > 0)
        ++m_severity[completion.get_cnode()];
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_CLS_Count
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_CLS_Count : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(LATE_SENDER, PEARL_create_callback(this, &PatternMPI_CLS_Count::late_sender__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_CLS_COUNT;
    }

    virtual long get_parent() const
    {
      return PAT_COMMS_RECV;
    }

    virtual string get_name() const
    {
      return "MPI Late Sender Instances (Communications)";
    }

    virtual string get_unique_name() const
    {
      return "mpi_cls_count";
    }

    virtual string get_descr() const
    {
      return "Number of Late Sender instances in MPI communications";
    }

    virtual string get_unit() const
    {
      return "occ";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void late_sender__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_CLS_Count::late_sender__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 129 "Counts.pattern"

      RemoteEvent send = data->mRemote->get_event(ROLE_SEND_LS);

      if (send->getBytesSent() != 0)
        ++m_severity[event.get_cnode()];
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_CLSWO_Count
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_CLSWO_Count : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(LATE_SENDER_WO, PEARL_create_callback(this, &PatternMPI_CLSWO_Count::late_sender_wo__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_CLSWO_COUNT;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_CLS_COUNT;
    }

    virtual string get_name() const
    {
      return "MPI Late Sender, Wrong Order Instances (Communications)";
    }

    virtual string get_unique_name() const
    {
      return "mpi_clswo_count";
    }

    virtual string get_descr() const
    {
      return "Number of Late Sender instances in MPI communications where messages are received in wrong order";
    }

    virtual string get_unit() const
    {
      return "occ";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void late_sender_wo__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_CLSWO_Count::late_sender_wo__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 153 "Counts.pattern"

      RemoteEvent send = data->mRemote->get_event(ROLE_SEND_LSWO);
      Event       recv = data->mLocal->get_event(ROLE_RECV_LSWO);

      if (send->getBytesSent() != 0)
        ++m_severity[recv.get_cnode()];
    
}

#endif   /* _MPI */

#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_RmaWaitAtCreate
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_RmaWaitAtCreate : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(MPI_RMA_WAIT_AT_CREATE, PEARL_create_callback(this, &PatternMPI_RmaWaitAtCreate::mpi_rma_wait_at_create__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_RMA_WAIT_AT_CREATE;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_MGMT_WIN;
    }

    virtual string get_name() const
    {
      return "MPI Wait at Window Create";
    }

    virtual string get_unique_name() const
    {
      return "mpi_rma_wait_at_create";
    }

    virtual string get_descr() const
    {
      return "Time spent in MPI_Win_create, waiting for the last process to enter the call";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void mpi_rma_wait_at_create__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_RmaWaitAtCreate::mpi_rma_wait_at_create__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 58 "MPI_RMA.pattern"

        m_severity[event.get_cnode()] += data->mIdle;
    
}

#endif   /* _MPI && HAS_MPI2_1SIDED */

#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_RmaWaitAtFree
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_RmaWaitAtFree : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(MPI_RMA_WAIT_AT_FREE, PEARL_create_callback(this, &PatternMPI_RmaWaitAtFree::mpi_rma_wait_at_free__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_RMA_WAIT_AT_FREE;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_MGMT_WIN;
    }

    virtual string get_name() const
    {
      return "MPI Wait at Window Free";
    }

    virtual string get_unique_name() const
    {
      return "mpi_rma_wait_at_free";
    }

    virtual string get_descr() const
    {
      return "Time spent in MPI_Win_free, waiting for the last process to enter the call";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void mpi_rma_wait_at_free__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_RmaWaitAtFree::mpi_rma_wait_at_free__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 79 "MPI_RMA.pattern"

        m_severity[event.get_cnode()] += data->mIdle;
    
}

#endif   /* _MPI && HAS_MPI2_1SIDED */

#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_RmaSync
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_RmaSync : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(MPI_RMA_LOCK, PEARL_create_callback(this, &PatternMPI_RmaSync::mpi_rma_lock__cb));
        (it->second)->register_callback(MPI_RMA_UNLOCK, PEARL_create_callback(this, &PatternMPI_RmaSync::mpi_rma_unlock__cb));
        (it->second)->register_callback(MPI_RMA_GATS, PEARL_create_callback(this, &PatternMPI_RmaSync::mpi_rma_gats__cb));
        (it->second)->register_callback(MPI_RMA_COLLECTIVE_END, PEARL_create_callback(this, &PatternMPI_RmaSync::mpi_rma_collective_end__cb));
        (it->second)->register_callback(MPI_RMA_LC_COMPUTE, PEARL_create_callback(this, &PatternMPI_RmaSync::mpi_rma_lc_compute__cb));
        (it->second)->register_callback(MPI_RMA_POST_COMPLETE, PEARL_create_callback(this, &PatternMPI_RmaSync::mpi_rma_post_complete__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_RMA_SYNCHRONIZATION;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_SYNCHRONIZATION;
    }

    virtual string get_name() const
    {
      return "MPI One-sided Synchronization";
    }

    virtual string get_unique_name() const
    {
      return "mpi_rma_synchronization";
    }

    virtual string get_descr() const
    {
      return "Time spent in MPI one-sided synchronization calls";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    virtual bool is_hidden() const
    {
      return true;
    }

    /* Callback methods */
    void mpi_rma_collective_end__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void mpi_rma_gats__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void mpi_rma_lc_compute__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void mpi_rma_lock__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void mpi_rma_post_complete__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void mpi_rma_unlock__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);


  private:

#line 99 "MPI_RMA.pattern"

    typedef struct {
      LocalData               events;                ///< Local event set for caching GATS start events
      map<uint32_t, RmaEpoch> lockEpoch;            ///< Map to store different
      LockEpochQueue          epochQueue;           ///< Queue to save epoch events of remote processes
      ContentionInfoStackMap   syncPointInfoStacks; ///< Map to store syncpoint exchange data
    } win_t;

    map<MpiWindow*, win_t> m_windows;  ///< Map of window-related cache data
  
};


/*----- Callback methods -----*/

void PatternMPI_RmaSync::mpi_rma_collective_end__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 191 "MPI_RMA.pattern"

      // NOTE: The heuristic for collective calls in RMA is to check
      //       whether all the recorded timespans of all participating
      //       processes overlap. If not all timespans overlap, no
      //       collective synchronization is assumed.
      Event   enter  = event.enterptr();
      const Region* region = &enter->getRegion();

      MpiWindow* win = dynamic_cast<MpiWindow*>(event->get_window());
      assert(win);

      MPI_Comm comm = dynamic_cast<MpiComm*>(win->get_comm())->getComm();
      assert(comm);

      // Check valid overlap & possible idle time
      const CollectiveInfo& coll(data->mCollinfo);
      if (coll.earliest_end.time > coll.latest.time
          && coll.my.time < coll.latest.time)
      {
          data->mIdle = coll.latest.time - coll.my.time;
      }
      else
      {
          data->mIdle = 0;
      }

      if (is_mpi_rma_fence(region))
      {
          if (data->mIdle > 0)
          {
              cbmanager.notify(MPI_RMA_WAIT_AT_FENCE, event, data);

              if (data->mLastRmaOp.time > enter->getTimestamp()
                  && data->mLastRmaOp.time <= event->getTimestamp())
              {
                  data->mIdle = data->mLastRmaOp.time - enter->getTimestamp();
                  cbmanager.notify(MPI_RMA_EARLY_FENCE, event, data);
              }
          }

          // analyze pairwise sync pattern
          int comm_size = 0;
          MPI_Comm_size(comm, &comm_size);

          // count of remote partners is stored in data->mCount
          data->mCount = comm_size - data->mCount;
          cbmanager.notify(MPI_RMA_PWS_UNNEEDED_COUNT, event, data);

          data->mCount = comm_size;
          cbmanager.notify(MPI_RMA_PWS_COUNT, event, data);

          // compute lock competition
          data->mWindow = win;
          //cbmanager.notify(MPI_RMA_LC_COMPUTE, event, data);
      }
      else if (is_mpi_rma_create(region))
      {
          MpiWindow* win   = event->get_window();
          win_t&     entry = m_windows[win];

          if (data->mIdle > 0)
          {
              cbmanager.notify(MPI_RMA_WAIT_AT_CREATE, event, data);
          }

          // register lock epoch queue for new window
          AmLockContention::getInstance().registerQueue(win->get_id(), &entry.epochQueue);
      }
      else if (is_mpi_rma_free(region))
      {
          if (data->mIdle > 0)
          {
              cbmanager.notify(MPI_RMA_WAIT_AT_FREE, event, data);

              if (data->mLastRmaOp.time > enter->getTimestamp())
              {
                  data->mIdle = data->mLastRmaOp.time - enter->getTimestamp();
                  cbmanager.notify(MPI_RMA_EARLY_FREE, event, data);
              }
          }

          // compute lock competition
          data->mWindow = win;
          //cbmanager.notify(MPI_RMA_LC_COMPUTE, event, data);
      }
      data->mLastRmaOp.time = -DBL_MAX;
    
}

void PatternMPI_RmaSync::mpi_rma_gats__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 149 "MPI_RMA.pattern"

      Event      enter  = event.enterptr();
      MpiWindow* win    = event->get_window();
      const Region*    region = &(enter->getRegion());
      MpiGroup*  group  = event->get_group();
      win_t&     entry  = m_windows[win];

      // check which GATS region we are in
      if (is_mpi_rma_wait(region))
      {
        // analyze pairwise RMA sync pattern on target
        data->mCount = group->numRanks();
        cbmanager.notify(MPI_RMA_PWS_COUNT, event, data);

        data->mCount -= data->mCountOrigin;
        cbmanager.notify(MPI_RMA_PWS_UNNEEDED_COUNT, event, data);

        // notify callback for full GATS pattern analysis on target
        cbmanager.notify(MPI_RMA_POST_WAIT, event, data);
      }
      else if (is_mpi_rma_start(region))
      {
        // cache start events for later analysis
        entry.events.add_event(enter, ROLE_ENTER_RMA_START);
        entry.events.add_event(event, ROLE_LEAVE_RMA_START);
      }
      else if (is_mpi_rma_complete(region))
      {
        // analyze paiwise RMA sync pattern on origin
        data->mCount = group->numRanks();
        cbmanager.notify(MPI_RMA_PWS_COUNT, event, data);

        data->mCount -= data->mCountTarget;
        cbmanager.notify(MPI_RMA_PWS_UNNEEDED_COUNT, event, data);

        // set cached data as
        *data->mLocal = entry.events;

        cbmanager.notify(MPI_RMA_POST_COMPLETE, event, data);
      }
    
}

void PatternMPI_RmaSync::mpi_rma_lc_compute__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 279 "MPI_RMA.pattern"

        MpiWindow* win   = data->mWindow;
        win_t&     entry = m_windows[win];
        bool       current_exclusive_epoch;

#ifdef DEBUG_LOCK_CONTENTION
        bool evaluate_locks = (entry.epochQueue.size() > 0);
        if (evaluate_locks)
            cerr << "Starting MPI_RMA_LC_COMPUTE" << endl;
#endif

        while (entry.epochQueue.size() >= 2)
        {
            // Priority queue of epochs to be processed
            LockEpochQueue scheduled_epochs;

            // Remove next epoch from queue and schedule it
            RemoteRmaEpochPtr current_epoch(entry.epochQueue.top());
            entry.epochQueue.pop();

            // check if current_epoch is exclusive
            RemoteRmaEpoch::const_iterator lock_it = current_epoch->begin();
            while (lock_it != current_epoch->end()
                    && (*lock_it)->isOfType(MPI_RMA_LOCK))
                ++lock_it;

            if ((*lock_it)->is_exclusive())
                current_exclusive_epoch = true;
            else
                current_exclusive_epoch = false;

            scheduled_epochs.push(current_epoch);

#ifdef DEBUG_LOCK_CONTENTION
            RemoteRmaEpoch::const_iterator ex_it = current_epoch->begin();
            while (ex_it != current_epoch->end()
                    && (*ex_it)->isOfType(MPI_RMA_UNLOCK))
                ++ex_it;

            cerr << "Unlock Time: " << (*ex_it)->getTimestamp() << endl;
#endif

            RemoteRmaEpochPtr reference_epoch(entry.epochQueue.top());
            bool               exclusive_epoch_found = false;

            if (!current_exclusive_epoch)
            {
                do
                {
                    reference_epoch = entry.epochQueue.top();
                    // get lock event
                    RemoteRmaEpoch::const_iterator lock_it = reference_epoch->begin();
                    while (lock_it != reference_epoch->end()
                        && (*lock_it)->isOfType(MPI_RMA_LOCK))
                        ++lock_it;

                    // if lock is shared, schedule epoch and get next
                    if (!(*lock_it)->is_exclusive())
                    {
                        scheduled_epochs.push(reference_epoch);
                        entry.epochQueue.pop();
                    }
                    else
                    {
                        exclusive_epoch_found = true;
                    }

                } while (!exclusive_epoch_found  && !entry.epochQueue.empty());
            }

            if (current_exclusive_epoch || exclusive_epoch_found)
            {
                while (scheduled_epochs.size() > 0)
                {
                    current_epoch = scheduled_epochs.top();
                    scheduled_epochs.pop();
                    RemoteRmaEpoch::reverse_iterator ce_it = current_epoch->rbegin();

                    // get previous exclusive lock owner's unlock event
                    RemoteRmaEpoch::reverse_iterator ref_it = reference_epoch->rbegin();
                    while ((*ref_it)->isOfType(MPI_RMA_UNLOCK))
                        ++ref_it;

#ifdef DEBUG_LOCK_CONTENTION
                    cerr << "ref_it: " << (*ref_it)->getTimestamp() << "  " << ref_it->get_location()->get_id()
                         << " ce_it: " << (*ce_it)->getTimestamp()<< "  " << ce_it->get_location()->get_id()
                         << endl;
#endif
                    // Search event of current lock owner preceeding previous owners unlock
                    while (ce_it != current_epoch->rend()
                          && (*ce_it)->getTimestamp() > (*ref_it)->getTimestamp())
                    {
                        ++ce_it;
                    }

                    if ((ce_it != current_epoch->rend())
                        && ((*ce_it)->isOfType(ENTER)
                            || (*ce_it)->isOfType(MPI_RMA_LOCK)
                            || (*ce_it)->isOfType(MPI_RMA_PUT_START) // FIXME: this may have to be PUT_END!
                            || (*ce_it)->isOfType(MPI_RMA_GET_START) // FIXME: this may have to be GET_END!
                           ))
                    {
#ifdef DEBUG_LOCK_CONTENTION
                        switch((*ce_it)->getType())
                        {
                            case ENTER: cerr << "Lock found on ENTER" << endl; break;
                            case MPI_RMA_LOCK: cerr << "Lock found on LOCK" << endl; break;
                            case MPI_RMA_PUT_START: cerr << "Lock found on PUT" << endl; break;
                            case MPI_RMA_GET_START: cerr << "Lock found on GET" << endl; break;
                            default: cerr << "Lock found on unexpected event!" << endl;
                        };
#endif
                        timestamp_t idle_time = (*ref_it)->getTimestamp() - (*ce_it)->getTimestamp();

                        if (idle_time > 0)
                        {
                            RemoteEvent& wait_event = *ce_it;

                            //determine next Event type of MPI_RMA_LOCK, MPI_RMA_PUT_START or MPI_RMA_GET_START
                            if((*ce_it)->isOfType(ENTER))
                            {
                                //set iterator to ce_it
                                RemoteRmaEpoch::iterator it = current_epoch->begin();
                                while (it != current_epoch->end()
                                        && (*it)->getTimestamp() <= (*ce_it)->getTimestamp())
                                    ++it;

                                //set it to MPI_RMA_LOCK, MPI_RMA_PUT_START or MPI_RMA_GET_START or MPI_RMA_UNLOCK
                                while (it != current_epoch->end()
                                        && !((*it)->isOfType(MPI_RMA_LOCK)
                                            || (*it)->isOfType(MPI_RMA_PUT_START)
                                            || (*it)->isOfType(MPI_RMA_GET_START)
                                            || (*it)->isOfType(MPI_RMA_UNLOCK)))
                                    ++it;

                                wait_event  = *it;
                            }

                            //prepare info to push on stacks for both events
                            RemoteEvent first_event     = *ref_it;
                            ident_t     wait_id         = wait_event.get_remote_index();
                            ident_t     first_id        = first_event.get_remote_index();
                            uint32_t    first_rank      = ref_it->get_location().getId();
                            uint32_t    wait_rank       = ce_it->get_location().getId();
                            uint32_t    first_sync_rank = wait_rank;
                            uint32_t    wait_sync_rank  = first_rank;

#ifdef DEBUG_LOCK_CONTENTION
                            cerr << "Stack info first: " << first_id << " " << idle_time << " " << first_sync_rank
                                 << "\nStack info wait: " << wait_id << " " << idle_time << " " << wait_sync_rank << endl;
#endif
                            //build info-structs - negative idletime means other processes have waited
                            ContentionInfo wait_info  = {wait_id, idle_time, wait_sync_rank};
                            ContentionInfo first_info = {first_id, -idle_time, first_sync_rank};

                            //push wait_info on stack of correspondent map
                            if (entry.syncPointInfoStacks.find(wait_rank) == entry.syncPointInfoStacks.end())
                            {
                                ContentionInfoStack wait_stack;
                                wait_stack.push(wait_info);
                                entry.syncPointInfoStacks[wait_rank] = wait_stack;
                            }
                            else
                            {
                                entry.syncPointInfoStacks[wait_rank].push(wait_info);
                            }

                            //push first_info on stack of correspondent map
                            if (entry.syncPointInfoStacks.find(first_rank) == entry.syncPointInfoStacks.end())
                            {
                                ContentionInfoStack first_stack;
                                first_stack.push(first_info);
                                entry.syncPointInfoStacks[first_rank] = first_stack;
                            }
                            else
                            {
                                entry.syncPointInfoStacks[first_rank].push(first_info);
                            }

                        } //if (idle_time > 0)
                        //delete current_epoch;
                    } //if ce_it type_of...

                } //while (scheduled_epochs.size() > 0)
            } //if (current_exclusive_epoch || exclusive_epoch_found)
        } //while (entry.epochQueue.size() >= 2)

        // pop last entry in queue
        if (entry.epochQueue.size() > 0)
            entry.epochQueue.pop();

        //send synchpoint information for lock-epochs to origins
        ContentionInfoStackMap::iterator mit = entry.syncPointInfoStacks.begin();
        while (mit != entry.syncPointInfoStacks.end())
        {
            ContentionInfoStack infoStack = mit->second;

            MpiComm* comm = dynamic_cast<MpiComm*>(win->get_comm());
            assert(comm);

            while (!infoStack.empty())
            {
                ContentionInfo& info = infoStack.top();
                AmRequest::Ptr request = AmRequestFactory::create(comm->getGroup().getLocalRank(mit->first),
                                                                  *comm);
                AmSyncpointExchange::pack(MPI_RMA_LOCK_COMPETITION, 0, info, request.get());

                AmRuntime::getInstance().enqueue(request);
                infoStack.pop();
            }

            ++mit;
        }
        entry.syncPointInfoStacks.clear();

#ifdef DEBUG_LOCK_CONTENTION
        if (evaluate_locks)
            cerr << "MPI_RMA_LC_COMPUTE done." << endl;
#endif
    
}

void PatternMPI_RmaSync::mpi_rma_lock__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 110 "MPI_RMA.pattern"

        // get reference to window data cache
        MpiWindow* win   = event->get_window();
        win_t&     entry = m_windows[win];

        // save complete lock region to epoch store
        entry.lockEpoch[event->get_remote()].push_back(event.enterptr());
        entry.lockEpoch[event->get_remote()].push_back(event);
        entry.lockEpoch[event->get_remote()].push_back(event.leaveptr());
        // notify direct RMA synchronization
        cbmanager.notify(RMA_SYNC_DIRECT, event, data);
    
}

void PatternMPI_RmaSync::mpi_rma_post_complete__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 500 "MPI_RMA.pattern"

      Event       enter       = event.enterptr();
      timestamp_t enter_time  = enter->getTimestamp();
      Event       start_enter =
                      data->mLocal->get_event(ROLE_ENTER_RMA_START);
      Event       start_exit =
                      data->mLocal->get_event(ROLE_LEAVE_RMA_START);

#ifdef GATSDEBUG
      MpiWindow* win = event->get_window();
      assert(win);

      MpiComm* comm = dynamic_cast<MpiComm*>(win->get_comm());
      assert(comm);

      int myrank;
      MPI_Comm_rank(comm->get_comm(), &myrank);
#endif

      if ((data->mLastPost.time > enter_time) &&
          (data->mLastPost.time < event->getTimestamp()))
      {
        /* non-blocking start and post concurrent to complete */
        data->mIdle     = data->mLastPost.time - enter_time;
        data->mCallpathId = enter.get_cnode()->getId();

#ifdef GATSDEBUG
        ostringstream out;
        out << "LATE POST : Event ID = " << event.get_id()
            << " global rank = " << myrank
            << " idle time = " << data->mIdle ;
        cerr << out.str() << endl;
#endif

        cbmanager.notify(MPI_RMA_LATE_POST, event, data);
      }
      else if ((data->mLastPost.time < start_exit->getTimestamp()) &&
               (data->mLastPost.time > start_enter->getTimestamp()))
      {
        /* blocking start */
        data->mIdle     = data->mLastPost.time - start_enter->getTimestamp();
        data->mCallpathId = start_enter.get_cnode()->getId();

#ifdef GATSDEBUG
        ostringstream out;
        out << "LATE POST : Event ID = " << start_exit.get_id()
            << " global rank = " << myrank
            << " idle time = " << data->mIdle ;
        cerr << out.str() << endl;
#endif

        cbmanager.notify(MPI_RMA_LATE_POST, start_exit, data);
      }
      else
      {
        // check all RMA operations between start and complete
        MpiWindow* win         = event->get_window();
        Event      evt_it      = event.prev().prev();
        bool       start_found = false;

        while (!start_found)
        {
          if (((evt_it->isOfType(MPI_RMA_PUT_START))
               || (evt_it->isOfType(MPI_RMA_GET_START)))
             && (evt_it->get_window() == win))
          {
            if (data->mLastPost.time > evt_it->getTimestamp() &&
                data->mLastPost.time < evt_it.leaveptr()->getTimestamp())
            {
              data->mIdle     = data->mLastPost.time - evt_it->getTimestamp();
              data->mCallpathId = evt_it.enterptr().get_cnode()->getId();

#ifdef GATSDEBUG
              ostringstream out;
              out << "LATE POST : Event ID = " << evt_it.get_id()
                  << " global rank = " << myrank
                  << " idle time = " << data->mIdle ;
              cerr << out.str() << endl;
#endif

              cbmanager.notify(MPI_RMA_LATE_POST, evt_it, data);
            }
          }

          // FIXME: The 'leaveptr().prev()' construct is only guaranteed to
          //        work for EPIK traces and needs to be revised once OTF2
          //        RMA records are supported!
          if (evt_it->isOfType(ENTER) &&
              is_mpi_rma_start(evt_it->getRegion()) &&
              evt_it.leaveptr().prev()->get_window() == win)
          {
            start_found = true;
          }

          /* check previous event */
          --evt_it;
        }
      }
    
}

void PatternMPI_RmaSync::mpi_rma_unlock__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 123 "MPI_RMA.pattern"

        // get reference to window data cache
        MpiWindow* win   = event->get_window();
        win_t&     entry = m_windows[win];

        // save complete unlock region to epoch store
        entry.lockEpoch[event->get_remote()].push_back(event.enterptr());
        entry.lockEpoch[event->get_remote()].push_back(event);
        entry.lockEpoch[event->get_remote()].push_back(event.leaveptr());

        // create active message request
        AmRequest::Ptr request = AmRequestFactory::create(event->get_remote(),
                                                        *(win->get_comm()));

        // pack lock epoch data to check for lock competition
        AmLockContention::pack(win->get_id(),
                                entry.lockEpoch[event->get_remote()],
                                request.get());

        // enqueue request
        AmRuntime::getInstance().enqueue(request);

        // clear lock epoch cache
        entry.lockEpoch[event->get_remote()].clear();
    
}

#endif   /* _MPI && HAS_MPI2_1SIDED */

#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_RmaSyncLatePost
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_RmaSyncLatePost : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(MPI_RMA_LATE_POST, PEARL_create_callback(this, &PatternMPI_RmaSyncLatePost::mpi_rma_late_post__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_RMA_SYNC_LATE_POST;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_RMA_SYNC_ACTIVE;
    }

    virtual string get_name() const
    {
      return "MPI Late Post (Synchronization)";
    }

    virtual string get_unique_name() const
    {
      return "mpi_rma_sync_late_post";
    }

    virtual string get_descr() const
    {
      return "Time spent in MPI one-sided active target synchronization waiting for exposure epoch to start";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void mpi_rma_late_post__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_RmaSyncLatePost::mpi_rma_late_post__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 635 "MPI_RMA.pattern"

        Callpath* callpath = data->mDefs->get_cnode(data->mCallpathId);

        if (is_mpi_rma_sync(&callpath->getRegion()))
        {
            m_severity[callpath] += data->mIdle;
        }
    
}

#endif   /* _MPI && HAS_MPI2_1SIDED */

#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_RmaEarlyWait
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_RmaEarlyWait : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(MPI_RMA_POST_WAIT, PEARL_create_callback(this, &PatternMPI_RmaEarlyWait::mpi_rma_post_wait__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_RMA_EARLY_WAIT;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_RMA_SYNC_ACTIVE;
    }

    virtual string get_name() const
    {
      return "MPI Early Wait";
    }

    virtual string get_unique_name() const
    {
      return "mpi_rma_early_wait";
    }

    virtual string get_descr() const
    {
      return "Time spent in MPI_Win_wait waiting for last exposure epoch to finish";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void mpi_rma_post_wait__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_RmaEarlyWait::mpi_rma_post_wait__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 663 "MPI_RMA.pattern"

      Event       enter      = event.enterptr();
      timestamp_t enter_time = enter->getTimestamp();

      if (data->mLastComplete.time > enter_time)
      {
        // compute early wait
        data->mIdle = data->mLastComplete.time - enter_time;
        m_severity[enter.get_cnode()] += data->mIdle;

#ifdef GATSDEBUG
        MpiWindow* win = event->get_window();
        assert(win);

        MpiComm* comm = dynamic_cast<MpiComm*>(win->get_comm());
        assert(comm);

        int myrank;
        MPI_Comm_rank(comm->get_comm(), &myrank);

        ostringstream out;
        out << "\nPattern Early_Wait : Event ID: " << event.get_id()
            << " mein Rang: " << myrank
            <<" idle time = " << data->mIdle ;
        cerr << out.str() << endl;
#endif

        cbmanager.notify(MPI_RMA_EARLY_WAIT, event, data);
      }
    
}

#endif   /* _MPI && HAS_MPI2_1SIDED */

#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_RmaLateComplete
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_RmaLateComplete : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(MPI_RMA_EARLY_WAIT, PEARL_create_callback(this, &PatternMPI_RmaLateComplete::mpi_rma_early_wait__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_RMA_LATE_COMPLETE;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_RMA_EARLY_WAIT;
    }

    virtual string get_name() const
    {
      return "MPI Late Complete";
    }

    virtual string get_unique_name() const
    {
      return "mpi_rma_late_complete";
    }

    virtual string get_descr() const
    {
      return "Time spent waiting in MPI_Win_wait between last RMA access and last MPI_Win_complete";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void mpi_rma_early_wait__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_RmaLateComplete::mpi_rma_early_wait__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 715 "MPI_RMA.pattern"

      Event       enter      = event.enterptr();
      timestamp_t enter_time = enter->getTimestamp();
      timestamp_t max_ts     = max(data->mLastRmaOp.time, enter_time);

      // save severity (it does not have to be checked as this is only
      // triggered if an Early Wait is found.)
      m_severity[enter.get_cnode()] +=
          (data->mLastComplete.time - max_ts);
    
}

#endif   /* _MPI && HAS_MPI2_1SIDED */

#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_RmaWaitAtFence
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_RmaWaitAtFence : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(MPI_RMA_WAIT_AT_FENCE, PEARL_create_callback(this, &PatternMPI_RmaWaitAtFence::mpi_rma_wait_at_fence__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_RMA_WAIT_AT_FENCE;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_RMA_SYNC_ACTIVE;
    }

    virtual string get_name() const
    {
      return "MPI Wait at Fence";
    }

    virtual string get_unique_name() const
    {
      return "mpi_rma_wait_at_fence";
    }

    virtual string get_descr() const
    {
      return "Time spent in front of MPI_Win_fence, waiting for other processes";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void mpi_rma_wait_at_fence__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_RmaWaitAtFence::mpi_rma_wait_at_fence__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 745 "MPI_RMA.pattern"

      m_severity[event.get_cnode()] += data->mIdle;
    
}

#endif   /* _MPI && HAS_MPI2_1SIDED */

#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_RmaEarlyFence
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_RmaEarlyFence : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(MPI_RMA_EARLY_FENCE, PEARL_create_callback(this, &PatternMPI_RmaEarlyFence::mpi_rma_early_fence__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_RMA_EARLY_FENCE;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_RMA_WAIT_AT_FENCE;
    }

    virtual string get_name() const
    {
      return "MPI Early Fence";
    }

    virtual string get_unique_name() const
    {
      return "mpi_rma_early_fence";
    }

    virtual string get_descr() const
    {
      return "Waiting time in MPI_Win_fence due to pending RMA operations";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void mpi_rma_early_fence__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_RmaEarlyFence::mpi_rma_early_fence__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 768 "MPI_RMA.pattern"

      m_severity[event.get_cnode()] += data->mIdle;
    
}

#endif   /* _MPI && HAS_MPI2_1SIDED */

#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMpi_RmaSyncLockContention
 *
 *---------------------------------------------------------------------------
 */


class PatternMpi_RmaSyncLockContention : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(MPI_RMA_LOCK_COMPETITION, PEARL_create_callback(this, &PatternMpi_RmaSyncLockContention::mpi_rma_lock_competition__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_RMA_SYNC_LOCK_COMPETITION;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_RMA_SYNC_PASSIVE;
    }

    virtual string get_name() const
    {
      return "MPI Lock Contention (Synchronization)";
    }

    virtual string get_unique_name() const
    {
      return "mpi_rma_sync_lock_competition";
    }

    virtual string get_descr() const
    {
      return "Time spent waiting in MPI_Win_lock/MPI_Win_unlock acquiring a lock for a window";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void mpi_rma_lock_competition__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMpi_RmaSyncLockContention::mpi_rma_lock_competition__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 805 "MPI_RMA.pattern"

        Callpath* callpath = data->mDefs->get_cnode(data->mCallpathId);

        if (data->mIdle > 0
            && is_mpi_rma_passive(&callpath->getRegion()))
        {
            m_severity[callpath] += data->mIdle;
        }
    
}

#endif   /* _MPI && HAS_MPI2_1SIDED */

#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMpi_RmaSyncWaitForProgress
 *
 *---------------------------------------------------------------------------
 */


class PatternMpi_RmaSyncWaitForProgress : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(MPI_RMA_WAIT_FOR_PROGRESS, PEARL_create_callback(this, &PatternMpi_RmaSyncWaitForProgress::mpi_rma_wait_for_progress__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_RMA_SYNC_WAIT_FOR_PROGRESS;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_RMA_SYNC_PASSIVE;
    }

    virtual string get_name() const
    {
      return "MPI Wait for Progress (Synchronization)";
    }

    virtual string get_unique_name() const
    {
      return "mpi_rma_sync_wait_for_progress";
    }

    virtual string get_descr() const
    {
      return "Time spent waiting in MPI_Win_lock/MPI_Win_unlock waiting for progress on the target";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void mpi_rma_wait_for_progress__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMpi_RmaSyncWaitForProgress::mpi_rma_wait_for_progress__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 833 "MPI_RMA.pattern"

        Callpath* callpath = data->mDefs->get_cnode(data->mCallpathId);

        if (is_mpi_rma_passive(&callpath->getRegion()))
        {
            m_severity[callpath] += data->mIdle;
        }
    
}

#endif   /* _MPI && HAS_MPI2_1SIDED */

#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_RmaCommLatePost
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_RmaCommLatePost : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(MPI_RMA_LATE_POST, PEARL_create_callback(this, &PatternMPI_RmaCommLatePost::mpi_rma_late_post__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_RMA_COMM_LATE_POST;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_RMA_COMMUNICATION;
    }

    virtual string get_name() const
    {
      return "MPI Late Post (Communication)";
    }

    virtual string get_unique_name() const
    {
      return "mpi_rma_comm_late_post";
    }

    virtual string get_descr() const
    {
      return "Time spent in MPI one-sided communication operations waiting for exposure epoch to start";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void mpi_rma_late_post__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_RmaCommLatePost::mpi_rma_late_post__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 878 "MPI_RMA.pattern"

        Callpath* callpath = data->mDefs->get_cnode(data->mCallpathId);

        if (is_mpi_rma_comm(&callpath->getRegion()))
        {
            m_severity[callpath] += data->mIdle;
        }
    
}

#endif   /* _MPI && HAS_MPI2_1SIDED */

#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMpi_RmaCommLockContention
 *
 *---------------------------------------------------------------------------
 */


class PatternMpi_RmaCommLockContention : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(MPI_RMA_LOCK_COMPETITION, PEARL_create_callback(this, &PatternMpi_RmaCommLockContention::mpi_rma_lock_competition__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_RMA_COMM_LOCK_COMPETITION;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_RMA_COMMUNICATION;
    }

    virtual string get_name() const
    {
      return "MPI Lock Contention (Communication)";
    }

    virtual string get_unique_name() const
    {
      return "mpi_rma_comm_lock_competition";
    }

    virtual string get_descr() const
    {
      return "Time spent waiting in MPI_Win_lock/MPI_Win_unlock acquiring a lock for a window";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void mpi_rma_lock_competition__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMpi_RmaCommLockContention::mpi_rma_lock_competition__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 904 "MPI_RMA.pattern"

        Callpath* callpath = data->mDefs->get_cnode(data->mCallpathId);

        if (data->mIdle > 0
            && is_mpi_rma_comm(&callpath->getRegion()))
        {
            m_severity[callpath] += data->mIdle;
        }
    
}

#endif   /* _MPI && HAS_MPI2_1SIDED */

#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMpi_RmaCommWaitForProgress
 *
 *---------------------------------------------------------------------------
 */


class PatternMpi_RmaCommWaitForProgress : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(MPI_RMA_WAIT_FOR_PROGRESS, PEARL_create_callback(this, &PatternMpi_RmaCommWaitForProgress::mpi_rma_wait_for_progress__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_RMA_COMM_WAIT_FOR_PROGRESS;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_RMA_COMMUNICATION;
    }

    virtual string get_name() const
    {
      return "MPI Wait for Progress (Communication)";
    }

    virtual string get_unique_name() const
    {
      return "mpi_rma_comm_wait_for_progress";
    }

    virtual string get_descr() const
    {
      return "Time spent waiting in MPI_Win_lock/MPI_Win_unlock waiting for progress on the target";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void mpi_rma_wait_for_progress__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMpi_RmaCommWaitForProgress::mpi_rma_wait_for_progress__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 932 "MPI_RMA.pattern"

        Callpath* callpath = data->mDefs->get_cnode(data->mCallpathId);

        if (is_mpi_rma_comm(&callpath->getRegion()))
        {
            m_severity[callpath] += data->mIdle;
        }
    
}

#endif   /* _MPI && HAS_MPI2_1SIDED */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_RmaPairsyncCount
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_RmaPairsyncCount : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(MPI_RMA_PWS_COUNT, PEARL_create_callback(this, &PatternMPI_RmaPairsyncCount::mpi_rma_pws_count__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_RMA_PAIRSYNC_COUNT;
    }

    virtual long get_parent() const
    {
      return PAT_NONE;
    }

    virtual string get_name() const
    {
      return "MPI pair-wise one-sided synchronizations";
    }

    virtual string get_unique_name() const
    {
      return "mpi_rma_pairsync_count";
    }

    virtual string get_descr() const
    {
      return "Number of pair-wise MPI one-sided synchronizations";
    }

    virtual string get_unit() const
    {
      return "occ";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void mpi_rma_pws_count__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_RmaPairsyncCount::mpi_rma_pws_count__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 969 "MPI_RMA.pattern"

        m_severity[event.get_cnode()] += data->mCount;
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_RmaPairsyncUnneededCount
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_RmaPairsyncUnneededCount : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(MPI_RMA_PWS_UNNEEDED_COUNT, PEARL_create_callback(this, &PatternMPI_RmaPairsyncUnneededCount::mpi_rma_pws_unneeded_count__cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_RMA_PAIRSYNC_UNNEEDED_COUNT;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_RMA_PAIRSYNC_COUNT;
    }

    virtual string get_name() const
    {
      return "MPI unneeded pair-wise one-sided synchronizations";
    }

    virtual string get_unique_name() const
    {
      return "mpi_rma_pairsync_unneeded_count";
    }

    virtual string get_descr() const
    {
      return "Number of unneeded pair-wise MPI one-sided synchronizations";
    }

    virtual string get_unit() const
    {
      return "occ";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void mpi_rma_pws_unneeded_count__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_RmaPairsyncUnneededCount::mpi_rma_pws_unneeded_count__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1000 "MPI_RMA.pattern"

        m_severity[event.get_cnode()] += data->mCount;
    
}

#endif   /* _MPI */

/*
 *---------------------------------------------------------------------------
 *
 * class PatternStatistics
 *
 *---------------------------------------------------------------------------
 */


class PatternStatistics : public Pattern
{
  public:

    /* Constructors & destructor */
    virtual ~PatternStatistics()
    {

#line 1189 "Statistics.pattern"

    // Release statistics objects
#if defined(_MPI)
    delete ls_quant;
    delete lsw_quant;
    delete lr_quant;
    delete wnxn_quant;
    delete wb_quant;
    delete er_quant;
    delete es_quant;
    delete lb_quant;
    delete bc_quant;
    delete nxnc_quant;
#endif

#if defined(_OPENMP)
    delete omp_eb_quant;
    delete omp_ib_quant;
    delete thread_lc_ol_quant;
    delete thread_lc_oc_quant;
    delete thread_lc_pm_quant;
    delete thread_lc_pc_quant;
#endif // _OPENMP
  
    }
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(PREPARE, PEARL_create_callback(this, &PatternStatistics::prepare__cb));
        (it->second)->register_callback(LATE_SENDER, PEARL_create_callback(this, &PatternStatistics::late_sender__cb));
        (it->second)->register_callback(LATE_SENDER_WO, PEARL_create_callback(this, &PatternStatistics::late_sender_wo__cb));
        (it->second)->register_callback(WAIT_NXN, PEARL_create_callback(this, &PatternStatistics::wait_nxn__cb));
        (it->second)->register_callback(WAIT_BARRIER, PEARL_create_callback(this, &PatternStatistics::wait_barrier__cb));
        (it->second)->register_callback(EARLY_REDUCE, PEARL_create_callback(this, &PatternStatistics::early_reduce__cb));
        (it->second)->register_callback(EARLY_SCAN, PEARL_create_callback(this, &PatternStatistics::early_scan__cb));
        (it->second)->register_callback(LATE_BCAST, PEARL_create_callback(this, &PatternStatistics::late_bcast__cb));
        (it->second)->register_callback(NXN_COMPL, PEARL_create_callback(this, &PatternStatistics::nxn_compl__cb));
        (it->second)->register_callback(BARRIER_COMPL, PEARL_create_callback(this, &PatternStatistics::barrier_compl__cb));
        (it->second)->register_callback(OMP_EBARRIER_WAIT, PEARL_create_callback(this, &PatternStatistics::omp_ebarrier_wait__cb));
        (it->second)->register_callback(OMP_IBARRIER_WAIT, PEARL_create_callback(this, &PatternStatistics::omp_ibarrier_wait__cb));
        (it->second)->register_callback(THREAD_LOCK_CONTENTION_OMPLOCK_WAIT, PEARL_create_callback(this, &PatternStatistics::thread_lock_contention_omplock_wait__cb));
        (it->second)->register_callback(THREAD_LOCK_CONTENTION_OMPCRITICAL_WAIT, PEARL_create_callback(this, &PatternStatistics::thread_lock_contention_ompcritical_wait__cb));
        (it->second)->register_callback(THREAD_LOCK_CONTENTION_PTHREADMUTEX_WAIT, PEARL_create_callback(this, &PatternStatistics::thread_lock_contention_pthreadmutex_wait__cb));
        (it->second)->register_callback(THREAD_LOCK_CONTENTION_PTHREADCONDITION_WAIT, PEARL_create_callback(this, &PatternStatistics::thread_lock_contention_pthreadcondition_wait__cb));
      }
      {
        CallbackManagerMap::iterator it = cbmgrs.find("bws");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(LATE_RECEIVER, PEARL_create_callback(this, &PatternStatistics::late_receiver_bws_cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_STATISTICS;
    }

    virtual long get_parent() const
    {
      return PAT_NONE;
    }

    virtual string get_name() const
    {
      return "STATISTICS";
    }

    virtual string get_unique_name() const
    {
      return "statistics";
    }

    virtual string get_descr() const
    {
      return "Statistics for waiting time distribution";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    virtual bool is_hidden() const
    {
      return true;
    }

    /* Callback methods */
    void barrier_compl__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void early_reduce__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void early_scan__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void late_bcast__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void late_receiver_bws_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void late_sender__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void late_sender_wo__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void nxn_compl__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void omp_ebarrier_wait__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void omp_ibarrier_wait__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void prepare__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void thread_lock_contention_ompcritical_wait__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void thread_lock_contention_omplock_wait__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void thread_lock_contention_pthreadcondition_wait__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void thread_lock_contention_pthreadmutex_wait__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void wait_barrier__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void wait_nxn__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
  protected:
    /* Protected methods */

    virtual void init()
    {

#line 1164 "Statistics.pattern"

    // Create statistics objects for MPI metrics
#if defined(_MPI)
    ls_quant   = new Quantile(NUMBER_COEFF);
    lsw_quant  = new Quantile(NUMBER_COEFF);
    lr_quant   = new Quantile(NUMBER_COEFF);
    wnxn_quant = new Quantile(NUMBER_COEFF);
    wb_quant   = new Quantile(NUMBER_COEFF);
    er_quant   = new Quantile(NUMBER_COEFF);
    es_quant   = new Quantile(NUMBER_COEFF);
    lb_quant   = new Quantile(NUMBER_COEFF);
    bc_quant   = new Quantile(NUMBER_COEFF);
    nxnc_quant = new Quantile(NUMBER_COEFF);
#endif

#if defined(_OPENMP)
    omp_eb_quant       = new Quantile(NUMBER_COEFF);
    omp_ib_quant       = new Quantile(NUMBER_COEFF);
    thread_lc_ol_quant = new Quantile(NUMBER_COEFF);
    thread_lc_oc_quant = new Quantile(NUMBER_COEFF);
    thread_lc_pm_quant = new Quantile(NUMBER_COEFF);
    thread_lc_pc_quant = new Quantile(NUMBER_COEFF);
#endif // _OPENMP
  
    }


  private:

#line 68 "Statistics.pattern"

    /// Symbolic names for entries in arrays storing upper bounds of metric
    /// durations
    enum duration_t {
      LS_MAX_DURATION = 0,
      LR_MAX_DURATION,
      WNXN_MAX_DURATION,
      WB_MAX_DURATION,
      ER_MAX_DURATION,
      ES_MAX_DURATION,
      LB_MAX_DURATION,
      BC_MAX_DURATION,
      NXNC_MAX_DURATION,
      OMP_EB_MAX_DURATION,
      OMP_IB_MAX_DURATION,
      THREAD_LC_OL_MAX_DURATION,
      THREAD_LC_OC_MAX_DURATION,
      THREAD_LC_PM_MAX_DURATION,
      THREAD_LC_PC_MAX_DURATION,
      MAX_DURATION_ENTRIES
    };

    /// CallbackData struct used for preparation replay
    struct MaxDurations : public pearl::CallbackData
    {
      double m_max_duration[MAX_DURATION_ENTRIES];

      MaxDurations()
      {
        memset(m_max_duration, 0, MAX_DURATION_ENTRIES * sizeof(double));
      }

      ~MaxDurations()
      {
      }

      void update_duration(duration_t idx, double value)
      {
        if (value > m_max_duration[idx]) {
          m_max_duration[idx] = value;
        }
      }
    };

    // returns the position with first identical cnode
    class find_cnode_position
    {
      public:
        find_cnode_position (pearl::ident_t n)
        : cnode(n)
        {}

        bool operator() (TopMostSevere element) const
        {
          return element.cnode == cnode;
        }

      private:
        pearl::ident_t cnode;
    };

    // returns the position of first element with cnode not in this list and 
    // bigger idletime
    class find_idletime_position
    {
      public:
        find_idletime_position (pearl::ident_t n, pearl::timestamp_t m)
        : cnode(n), idle (m)
        {}

        bool operator() (TopMostSevere element) const
        {
          return ((element.cnode != cnode) && (element.idletime < idle));
        }

      private:
        pearl::ident_t cnode;
        pearl::timestamp_t idle;
    };

    void mpicollective_cb(const pearl::CallbackManager& cbmanager,
                          int                           user_event,
                          const pearl::Event&           event,
                          pearl::CallbackData*          cdata)
    {
      Event         begin    = event.beginptr();
      double        duration = event->getTimestamp() - begin->getTimestamp();
      MaxDurations* data     = static_cast<MaxDurations*>(cdata);

      const Region& region = event.get_cnode()->getRegion();
      if (is_mpi_barrier(region)) {
        // Wait at Barrier
        data->update_duration(WB_MAX_DURATION, duration);
        // Barrier Completion
        data->update_duration(BC_MAX_DURATION, duration);
      } else if (is_mpi_12n(region)) {
        // Late Broadcast
        data->update_duration(LB_MAX_DURATION, duration);
      } else if (is_mpi_n21(region)) {
        // Early Reduce
        data->update_duration(ER_MAX_DURATION, duration);
      } else if (is_mpi_scan(region)) {
        // Early Scan
        data->update_duration(ES_MAX_DURATION, duration);
      } else if (is_mpi_n2n(region)) {
        // Wait at NxN
        data->update_duration(WNXN_MAX_DURATION, duration);
        // NxN Completion
        data->update_duration(NXNC_MAX_DURATION, duration);
      }
    }

    void send_cb(const pearl::CallbackManager& cbmanager,
                 int                           user_event,
                 const pearl::Event&           event,
                 pearl::CallbackData*          cdata)
    {
      Event         enter    = event.enterptr();
      Event         leave    = event.leaveptr();
      double        duration = leave->getTimestamp() - enter->getTimestamp();
      MaxDurations* data     = static_cast<MaxDurations*>(cdata);

      // Late Receiver
      data->update_duration(LR_MAX_DURATION, duration);
    }

    void recv_cb(const pearl::CallbackManager& cbmanager,
                 int                           user_event,
                 const pearl::Event&           event,
                 pearl::CallbackData*          cdata)
    {
      Event         enter    = event.enterptr();
      Event         leave    = event.leaveptr();
      double        duration = leave->getTimestamp() - enter->getTimestamp();
      MaxDurations* data     = static_cast<MaxDurations*>(cdata);

      // Late Sender
      data->update_duration(LS_MAX_DURATION, duration);
    }

    void leave_cb(const pearl::CallbackManager& cbmanager,
                  int                           user_event,
                  const pearl::Event&           event,
                  pearl::CallbackData*          cdata)
    {
      const Region& region = event.get_cnode()->getRegion();
      if (!is_omp_barrier(region))
        return;

      Event         enter    = event.enterptr();
      double        duration = event->getTimestamp() - enter->getTimestamp();
      MaxDurations* data     = static_cast<MaxDurations*>(cdata);

      if (is_omp_ebarrier(region)) {
        // OpenMP explicit barrier
        data->update_duration(OMP_EB_MAX_DURATION, duration);
      } else if (is_omp_ibarrier(region)) {
        // OpenMP implicit barrier
        data->update_duration(OMP_IB_MAX_DURATION, duration);
      }
    }

    void acquire_cb(const pearl::CallbackManager& cbmanager,
                    int                           user_event,
                    const pearl::Event&           event,
                    pearl::CallbackData*          cdata)
    {
      Event         enter    = event.enterptr();
      double        duration = event->getTimestamp() - enter->getTimestamp();
      MaxDurations* data     = static_cast<MaxDurations*>(cdata);

      LockType lockType = getLockType(event);
      if (lockType == LockType::OMP_LOCK_API)
      {
        data->update_duration(THREAD_LC_OL_MAX_DURATION, duration);
      }
      else if (lockType == LockType::OMP_CRITICAL)
      {
        data->update_duration(THREAD_LC_OC_MAX_DURATION, duration);
      }
      else if (lockType == LockType::PTHREAD_MUTEX_LOCK
               || lockType == LockType::PTHREAD_MUTEX_TRYLOCK)
      {
        data->update_duration(THREAD_LC_PM_MAX_DURATION, duration);
      }
      else if (lockType == LockType::PTHREAD_CONDITION_WAIT)
      {
        data->update_duration(THREAD_LC_PC_MAX_DURATION, duration);
      }
    }
    
    // Statistics collector objects for individual patterns
#if defined(_MPI)
    Quantile* ls_quant;
    Quantile* lsw_quant;
    Quantile* lr_quant;
    Quantile* lrw_quant;
    Quantile* wnxn_quant;
    Quantile* wb_quant;
    Quantile* er_quant;
    Quantile* es_quant;
    Quantile* lb_quant;
    Quantile* bc_quant;
    Quantile* nxnc_quant;
#endif // _MPI

#if defined(_OPENMP)
    Quantile* omp_eb_quant;
    Quantile* omp_ib_quant;
    Quantile* thread_lc_ol_quant;
    Quantile* thread_lc_oc_quant;
    Quantile* thread_lc_pm_quant;
    Quantile* thread_lc_pc_quant;
#endif // (_OPENMP)

    // define deques for most severe instances
#if defined(_MPI)
    vector<TopMostSevere> LateSender, LateSenderWO, LateReceiver, EarlyReduce;
    vector<TopMostSevere> BarrierNxN, Barrier, NxnCompl, BarrierCompl, LateBcast, EarlyScan;
#endif // _MPI

    static MaxDurations sharedDurations;

#if defined(_OPENMP)
    vector<TopMostSevere> OmpEBarrier, OmpIBarrier;
    vector<TopMostSevere> ThreadLockContentionOMPLockAPI;
    vector<TopMostSevere> ThreadLockContentionOMPCritical;
    vector<TopMostSevere> ThreadLockContentionPthreadMutex;
    vector<TopMostSevere> ThreadLockContentionPthreadCondition;

    static vector<TopMostSevere> sharedLC_OL;
    static vector<TopMostSevere> sharedLC_OC;
    static vector<TopMostSevere> sharedLC_PM;
    static vector<TopMostSevere> sharedLC_PC;

    static TopMostSevereMaxSum sharedMaxSum;


    // merges thread local TopMostSevere vectors into a shared one using the stat_collector method
    void result_merge_thread_topmost(vector<TopMostSevere> &sharedInst,  vector<TopMostSevere> inst)
    {
        #pragma omp critical
        {
          for (size_t i=0; i< inst.size();i++)
          {
            stat_collector(sharedInst,inst[i].idletime,
                                      inst[i].entertime,
                                      inst[i].exittime,
                                      inst[i].cnode,
                                      inst[i].rank);
          }
        }
    }
#endif // _OPENMP
    
    // merge results of found patterns
    // from all processes and threads 
    // static variables used here as an implicit shared variables
    // to perform reduction for OpenMP threads
    void result_merge(Quantile* quant)
    {
      // Determine global number of instances
      // (The 'static' variable 'shared_n' is implicitly shared!)
      double n, global_n;
      static double shared_n;
      #pragma omp master
      {
        shared_n = 0.0;
      }
      
      // Determine global coefficients for collecting statistics
      // (The 'static' array 'shared_coeff' is implicitly shared!)
      double coeff[NUMBER_COEFF], global_coeff[NUMBER_COEFF];
      static double shared_coeff[NUMBER_COEFF];

      #pragma omp master
      {
        for (int i = 0; i < NUMBER_COEFF; i++) {
          shared_coeff[i] = 0.0;
        }
      }

#ifdef ADAPTIVE_APPROACH
      // Determine global coefficients for control values
      // (The 'static' array 'shared_control_val' is implicitly shared!)
      double control_val[NUMBER_COEFF], global_control_val[NUMBER_COEFF];
      static double shared_control_val[NUMBER_COEFF];
    
      #pragma omp master
      {
        for (int i = 0; i < NUMBER_COEFF; i++) {
          shared_control_val[i] = 0.0;
        }
      }
#endif

      // Determine global max
      // (The 'static' variable 'shared_max' is implicitly shared!)
      double max_val, global_max;
      static double shared_max;
      #pragma omp master
      {
        shared_max = -DBL_MAX;
      }
      
      // Determine global min
      // (The 'static' variable 'shared_min' is implicitly shared!)
      double min_val, global_min;
      static double shared_min;
      #pragma omp master
      {
        shared_min = DBL_MAX;
      }

      // Determine global sum
      // (The 'static' variable 'shared_sum' is implicitly shared!)
      double sum, global_sum;
      static double shared_sum;
      #pragma omp master
      {
        shared_sum = 0.0;
      }

      // Determine global squared sum
      // (The 'static' variable 'shared_squared_sum' is implicitly shared!)
      double squared_sum, global_squared_sum;
      static double shared_squared_sum;
      #pragma omp master
      {
        shared_squared_sum = 0.0;
      }
      
      n = quant->get_n();
      
      #pragma omp barrier
      #pragma omp critical
      {
        shared_n += n;
      }
      #pragma omp barrier
      
#if defined(_MPI)
      #pragma omp master
      {
        MPI_Allreduce(&shared_n, &global_n, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        shared_n = global_n;
      }
#endif // _MPI
      
      #pragma omp barrier
      global_n = shared_n;
      #pragma omp barrier

      for(int i=0; i<NUMBER_COEFF; i++) {
        coeff[i] = (n/global_n) * quant->get_coeff(i);
      }
      
      #pragma omp barrier
      #pragma omp critical
      {
        for (int i = 0; i < NUMBER_COEFF; i++) {
          shared_coeff[i] += coeff[i];
        }
      }
      #pragma omp barrier
      
#if defined(_MPI)
      #pragma omp master
      {
        MPI_Allreduce(shared_coeff, global_coeff, NUMBER_COEFF, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        for (int i = 0; i < NUMBER_COEFF; i++) {
          shared_coeff[i] = global_coeff[i];
        }
      }
#endif // _MPI

      #pragma omp barrier
      for (int i = 0; i < NUMBER_COEFF; i++) {
        global_coeff[i] = shared_coeff[i];
      }

#ifdef ADAPTIVE_APPROACH    
      for(int i=0; i<NUMBER_COEFF; i++) {
        control_val[i] = (n/global_n) * quant->get_control_val(i);
      }
      
      #pragma omp barrier
      #pragma omp critical
      {
        for (int i = 0; i < NUMBER_COEFF; i++) {
          shared_control_val[i] += control_val[i];
        }
      }
      #pragma omp barrier
#if defined(_MPI)
      #pragma omp master
      {
        MPI_Allreduce(shared_control_val, global_control_val, NUMBER_COEFF, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        for (int i = 0; i < NUMBER_COEFF; i++) {
          shared_control_val[i] = global_control_val[i];
        }
      }
#endif // _MPI

      #pragma omp barrier
      for (int i = 0; i < NUMBER_COEFF; i++) {
        global_control_val[i] = shared_control_val[i];
      }

#endif    

      max_val     = quant->get_max_val();
      min_val     = quant->get_min_val();
      sum         = quant->get_sum();
      squared_sum = quant->get_squared_sum();

      #pragma omp barrier
      
      #pragma omp critical
      {
        if(shared_max < max_val)
        {
          shared_max = max_val;
        }
      
        if(shared_min > min_val)
        {
          shared_min = min_val;
        }
      
        shared_sum += sum;
        shared_squared_sum += squared_sum;
      }
      #pragma omp barrier
      
#if defined(_MPI)
      #pragma omp master
      {
        MPI_Allreduce(&shared_max, &global_max, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
        MPI_Allreduce(&shared_min, &global_min, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
        MPI_Allreduce(&shared_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        MPI_Allreduce(&shared_squared_sum, &global_squared_sum, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        shared_max = global_max;
        shared_min = global_min;
        shared_sum = global_sum;
        shared_squared_sum = global_squared_sum;
      }
#endif // _MPI

      #pragma omp barrier
      global_max = shared_max;
      global_min = shared_min;
      global_sum = shared_sum;
      global_squared_sum = shared_squared_sum;
      #pragma omp barrier
      
#ifdef ADAPTIVE_APPROACH
      quant->set_global_values(global_n, global_sum, global_squared_sum, global_min, global_max, global_coeff, global_control_val);
#else      
      quant->set_global_values(global_n, global_sum, global_squared_sum, global_min, global_max, global_coeff);
#endif
    }

    void write_cube_file(Quantile* quant, vector<TopMostSevere>& instance, ReportData& data, FILE* cube_fp)
    {
      string patternName     = quant->get_metric();
      double number_obs      = quant->get_n();
      double sum_obs         = quant->get_sum();
      double squared_sum_obs = quant->get_squared_sum();
      double min_val         = quant->get_min_val();
      double max_val         = quant->get_max_val();
      double variance        = (squared_sum_obs/number_obs)-(sum_obs * sum_obs)/(number_obs*number_obs);
      double mean            = sum_obs/number_obs;
      double lower_quant     = quant->get_lower_quant();
      double median          = quant->get_median();
      double upper_quant     = quant->get_upper_quant();

      if (number_obs>0) {
        fprintf(cube_fp, "%-26s %9.0f %1.7f %1.7f %1.10f %1.10f %1.10f", patternName.c_str(), number_obs, mean, median, min_val, max_val, sum_obs);
        if (number_obs >= 2)
          fprintf(cube_fp, " %1.10f", variance);
        if (number_obs >= 5)
          fprintf(cube_fp, " %1.10f %1.10f", lower_quant, upper_quant);

        unsigned int i = 0;
        if (instance[i].idletime > 0.0)
          fprintf(cube_fp, "\n");
        while (i < instance.size() && instance[i].idletime > 0.0) {
            fprintf(cube_fp, "- cnode %d enter: %1.10f exit: %1.10f duration: %1.10f rank: %d\n",data.cnodes[instance[i].cnode]->id, instance[i].entertime, instance[i].exittime, instance[i].idletime, instance[i].rank);
          i++;
        }

        fprintf(cube_fp, "\n");
      }
    }  

#ifdef WRITE_CONTROL_VALUES

#if defined(_MPI)
    vector<double> md_ls, md_lsw, md_lr, md_lrw, md_wnxn, md_wb, md_er, md_es, md_lb, md_bc, md_nxnc;
#endif // _MPI

#if defined(_OPENMP)
    vector<double> md_omp_eb, md_omp_ib;
    vector<double> md_thread_lc_ol, md_thread_lc_oc, md_thread_lc_pm, md_thread_lc_pc;
#endif // _OPENMP

    void write_values(Quantile* quant)
    {
      string filename = "stats/evaluation/" + quant->get_metric() + "_values.txt";
      FILE* quant_fp = fopen(filename.c_str(), "w");

#ifdef ADAPTIVE_APPROACH
      int opt_number_coeff = quant->get_number_coeff();
#endif
      double number_obs = quant->get_n();
      double sum_obs = quant->get_sum();
      double squared_sum_obs = quant->get_squared_sum();
      double min_val = quant->get_min_val();
      double max_val = quant->get_max_val();
      double factor = quant->get_factor();
      double coeff[NUMBER_COEFF];

      for(int i=0; i<NUMBER_COEFF; i++) {
        coeff[i] = quant->get_coeff(i);
      }

      double mean = sum_obs/number_obs;
      double variance = (squared_sum_obs/number_obs)-(sum_obs * sum_obs)/(number_obs*number_obs);

      fprintf(quant_fp, "Number of all coefficients and all observations n : %d and %10.0f\n", NUMBER_COEFF, number_obs);
#ifdef ADAPTIVE_APPROACH
      fprintf(quant_fp, "Optimal number of coefficients: %d\n", opt_number_coeff);
#endif
      fprintf(quant_fp, "sum : %2.10f, squared sum: %2.10f\n", sum_obs, squared_sum_obs);
      fprintf(quant_fp, "mean : %2.10f, variance: %2.10f\n", mean, variance);

      double lower_quant = quant->get_lower_quant();
      double median = quant->get_median();
      double upper_quant = quant->get_upper_quant();

      fprintf(quant_fp, "upper bound   : %2.15f\n\n", (1/factor));
      fprintf(quant_fp, "minimum       : %2.15f\n", min_val);
      fprintf(quant_fp, "lower quartile: %2.15f\n", (lower_quant));
      fprintf(quant_fp, "median        : %2.15f\n", (median));
      fprintf(quant_fp, "upper quartile: %2.15f\n", (upper_quant));
      fprintf(quant_fp, "maximum       : %2.15f\n\n", max_val);
    
      for(int i=0; i<NUMBER_COEFF; i++) {
        fprintf(quant_fp, "coeff nr %2d : %2.15f\n", (i+1), coeff[i]);
      }

      fclose(quant_fp);
    }

#endif

    // This function performs insertion
    // sort of array of structs, where 
    // arr - input array to be sorted
    // len - length of the array to be sorted
    void insertionSort(TopMostSevere* arr, int len)
    {
      int j;
      TopMostSevere tmp;
      for (int i = 1; i < len; i++) {
        j = i;
        while (j > 0 && arr[j - 1].idletime < arr[j].idletime) {
          tmp        = arr[j];
          arr[j]     = arr[j - 1];
          arr[j - 1] = tmp;
          j--;
        }
      }
    }

    // This function inserts an element 
    // into the sorted vector of structures
    // and shifts the rest elements, where
    // in    - First input array
    // inout - Second input array, also storing the result
    // i     - the id of the element which should be inserted
    // j     - pointer to the position where the structure should be inserted
    // len   - length of the vestor os structures
    void insertAndShift(TopMostSevere* invec, TopMostSevere* inoutvec, int i, int& j, int& len)
    {
      //shift
      std::memmove(&inoutvec[j + 1], &inoutvec[j], (len - j - 1) * sizeof(TopMostSevere));
      //insert
      inoutvec[j] = invec[i];
    }

#if defined(_MPI)
    // performs reduction to compute
    // the top most severe instsnces
    void result_top_most(vector<TopMostSevere>& instance, TopMostSevere  * tmpRecv)
    {
      int len_sum;
      int len = instance.size();
      // check if one needs to make reduction
      MPI_Allreduce(&len, &len_sum, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
      if (len_sum > 0) {
        if (instance.size() < TOP_SEVERE_INSTANCES) {
          if (len > 1) {
            insertionSort(&instance[0], len);
          }
          for (int i = TOP_SEVERE_INSTANCES - instance.size(); i > 0; i--) {
            instance.push_back(TopMostSevere());
          }
        }

        // define global rank
        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        MPI_Reduce(&instance[0], &tmpRecv[0], 1, TOPSEVEREARRAY,
                   MAX_TOPSEVEREARRAY, 0, MPI_COMM_WORLD);
        
        if (rank == 0) {
          for (int i = 0; i < TOP_SEVERE_INSTANCES; ++i) {
            instance[i] = tmpRecv[i];
          }
        }
      }
    }
#endif // _MPI

    // collects and keep sorted
    // most severe instances
    // with different cnodes
    void stat_collector(vector<TopMostSevere> &instance, pearl::timestamp_t idle, pearl::timestamp_t enter, pearl::timestamp_t exit, pearl::ident_t cnode, pearl::ident_t rank)
    {
      TopMostSevere tmp;
      vector<TopMostSevere>::iterator iter_cnode, iter_idle;
      int pos_cnode, pos_idle, len;
      tmp = TopMostSevere(idle, enter, exit, cnode, rank);
      
      // if our vector buffer is not full
      if (instance.size() < TOP_SEVERE_INSTANCES) {
        // first severity data is inserted without any conditions
        if (instance.empty()) {
          instance.push_back(TopMostSevere(idle,enter,exit,cnode, rank));
        }
        // if it is not the first one, we have to check further conditions
        else {
          // find the position of the severity item with an identical "cnode" value
          iter_cnode = find_if(instance.begin(), instance.end(), find_cnode_position(cnode));
          pos_cnode = (int) (iter_cnode - instance.begin());
          
          // if we have severity with such "cnode" value and greater "idletime"
          // then we update data with new values of time
          if (((unsigned int)pos_cnode != instance.size()) && (idle > instance[pos_cnode].idletime)) {
            instance[pos_cnode] = tmp;
          }
          // if we've not found any sevirity item with identical "cnode" value
          // it will be inserted into the vector
          else if ((unsigned int)pos_cnode == instance.size()) {
            instance.push_back(TopMostSevere(idle,enter,exit,cnode,rank));
            // if the maximum value of severities was reached, sort the vector due to "idletime"
            if (instance.size() == TOP_SEVERE_INSTANCES) {
              insertionSort(&instance[0], instance.size());
            }
          }
        }
      }
      // now our vector buffer is full
      else {
        len = instance.size();
        
        // find the position of the severity item with an identical "cnode" value
        iter_cnode = find_if(instance.begin(), instance.end(), find_cnode_position(cnode));
        pos_cnode = (int) (iter_cnode - instance.begin());
        
        // find the position of the severity item with not identical "cnode" and greater "idletime"
        iter_idle = find_if(instance.begin(), instance.end(), find_idletime_position(cnode, idle));
        pos_idle = (int) (iter_idle - instance.begin());
        
        // update the values if our next item with the same "cnode" has greater "idletime"
        // and sort it afterwards due to "idletime"
        if (((unsigned int)pos_cnode != instance.size()) && (idle > instance[pos_cnode].idletime)) {
          instance[pos_cnode] = tmp;
          insertionSort(&instance[0], len);
        }
        // if we found the severity with a new "cnode" vlue and greater "idletime"
        // then the vector will be shifted
        else if (((unsigned int)pos_idle != instance.size()) && ((unsigned int)pos_cnode == instance.size())) {  
          insertAndShift(&tmp, &instance[0], 0, pos_idle, len);
        }
      }
    }
    
#if defined(_MPI)
    // performs MPI reduction of idletime, enter and exit times
    // of most severe instances for collective operations
    void stat_collective(vector<TopMostSevere> &inst, pearl::timestamp_t idle, pearl::timestamp_t enter, pearl::timestamp_t exit, pearl::ident_t cnode, pearl::ident_t global_rank, MpiComm* comm)
    {
      // define local rank
      int local_rank;
      MPI_Comm_rank(comm->getComm(), &local_rank);
      
      TopMostSevereMaxSum tmpSend, tmpRecv;
      tmpSend = TopMostSevereMaxSum(idle, idle, enter, exit, cnode, global_rank);
      
      MPI_Reduce(&tmpSend, &tmpRecv, 1, TOPSEVERECOLL, MAXSUM_TOPSEVERECOLL, 0, comm->getComm());
      
      if (local_rank == 0) {
#if !defined(MOST_SEVERE_MAX)
        stat_collector(inst, tmpRecv.idletime, tmpRecv.entertime, tmpRecv.exittime, tmpRecv.cnode, tmpRecv.rank);
#else
        stat_collector(inst, tmpRecv.idletime_max, tmpRecv.entertime, tmpRecv.exittime, tmpRecv.cnode, tmpRecv.rank);
#endif
      }
    }
#endif // _MPI

#if defined(_OPENMP)
    // performs OpenMP reduction of idletime, enter and exit times
    // of most severe instances for collective operations
    void stat_collective(vector<TopMostSevere> &inst, pearl::timestamp_t idle, pearl::timestamp_t enter, pearl::timestamp_t exit, pearl::ident_t cnode, pearl::ident_t rank)
    {
      #pragma omp barrier
      #pragma omp master
      {
        sharedMaxSum = TopMostSevereMaxSum(-DBL_MAX,0.0,DBL_MAX,-DBL_MAX,cnode,rank);
      }
      #pragma omp barrier
      #pragma omp critical
      {
        sharedMaxSum.idletime += idle;
        
        if(sharedMaxSum.entertime > enter)
          sharedMaxSum.entertime = enter;
        
        if(sharedMaxSum.exittime < exit)
          sharedMaxSum.exittime = exit;
          
        if(sharedMaxSum.idletime_max < idle) {
          sharedMaxSum.idletime_max = idle;
          sharedMaxSum.rank = rank;
        }
      }      
      #pragma omp barrier
      
      #pragma omp master
      {
#if !defined(MOST_SEVERE_MAX)
        stat_collector(inst, sharedMaxSum.idletime, sharedMaxSum.entertime, sharedMaxSum.exittime, sharedMaxSum.cnode, sharedMaxSum.rank);
#else
        stat_collector(inst, sharedMaxSum.idletime_max, sharedMaxSum.entertime, sharedMaxSum.exittime, sharedMaxSum.cnode, sharedMaxSum.rank);
#endif
      }
      
    }
#endif // _OPENMP


    public:
      void gen_severities(ReportData&              data,
                          int                      rank,
                          const pearl::LocalTrace& trace)
      {
#if defined(_MPI)
        result_merge(ls_quant);
        result_merge(lsw_quant);
        result_merge(lr_quant);
        result_merge(wnxn_quant);
        result_merge(wb_quant);
        result_merge(er_quant);
        result_merge(es_quant);
        result_merge(lb_quant);
        result_merge(bc_quant);
        result_merge(nxnc_quant);
#endif // _MPI
#if defined(_OPENMP)
        result_merge(omp_eb_quant);
        result_merge(omp_ib_quant);
        result_merge(thread_lc_ol_quant);
        result_merge(thread_lc_oc_quant);
        result_merge(thread_lc_pm_quant);
        result_merge(thread_lc_pc_quant);
#endif

#if defined(_OPENMP)
        #pragma omp barrier
        result_merge_thread_topmost(sharedLC_OL, ThreadLockContentionOMPLockAPI);
        result_merge_thread_topmost(sharedLC_OC, ThreadLockContentionOMPCritical);
        result_merge_thread_topmost(sharedLC_PM, ThreadLockContentionPthreadMutex);
        result_merge_thread_topmost(sharedLC_PC, ThreadLockContentionPthreadCondition);

        #pragma omp barrier
#endif // _OPENMP

        #pragma omp master
        {
#ifdef WRITE_CONTROL_VALUES
          if (rank == 0) {
            mkdir("stats", 0700);
            mkdir("stats/evaluation", 0700);
#if defined(_MPI)
            mkdir("stats/ls_stats",   0700);
            mkdir("stats/lsw_stats",  0700);
            mkdir("stats/lr_stats",   0700);
            mkdir("stats/wnxn_stats", 0700);
            mkdir("stats/wb_stats",   0700);
            mkdir("stats/er_stats",   0700);
            mkdir("stats/es_stats",   0700);
            mkdir("stats/lb_stats",   0700);
            mkdir("stats/bc_stats",   0700);
            mkdir("stats/nxnc_stats", 0700);
#endif // _MPI

#if defined(_OPENMP)
            mkdir("stats/omp_eb_stats", 0700);
            mkdir("stats/omp_ib_stats", 0700);
            mkdir("stats/thread_lc_ol_stats", 0700);
            mkdir("stats/thread_lc_oc_stats", 0700);
            mkdir("stats/thread_lc_pm_stats", 0700);
            mkdir("stats/thread_lc_pc_stats", 0700);
#endif // _OPENMP
          }
#if defined(_MPI)
          MPI_Barrier(MPI_COMM_WORLD);
#endif // _MPI
        }

          // write values for OpenMp patterns
#if defined(_OPENMP)
          char name_omp[50];
          FILE* stats_omp_fp;
          unsigned int thread_id;
          thread_id = omp_get_thread_num();
          #pragma omp barrier
          
          // OpenMP explicit barrier
          snprintf(name_omp, 50, "stats/omp_eb_stats/omp_eb_stats.%06dx%06u", rank, thread_id);
          stats_omp_fp = fopen(name_omp, "w");
          for (size_t i = 0; i < md_omp_eb.size(); i++)
            fprintf(stats_omp_fp, "%2.14f\n", md_omp_eb[i]);
          fclose(stats_omp_fp);

          // OpenMP implicit barrier
          snprintf(name_omp, 50, "stats/omp_ib_stats/omp_ib_stats.%06dx%06u", rank, thread_id);
          stats_omp_fp = fopen(name_omp, "w");
          for (size_t i = 0; i < md_omp_ib.size(); i++)
            fprintf(stats_omp_fp, "%2.14f\n", md_omp_ib[i]);
          fclose(stats_omp_fp);

          // Thread LockContention OMP Lock API
          snprintf(name_omp, 50, "stats/thread_lc_ol_stats/thread_lc_ol_stats.%06dx%06u", rank, thread_id);
          stats_omp_fp = fopen(name_omp, "w");
          for (size_t i = 0; i < md_thread_lc_ol.size(); i++)
            fprintf(stats_omp_fp, "%2.14f\n", md_thread_lc_ol[i]);
          fclose(stats_omp_fp);

          // Thread LockContention OMP Critical
          snprintf(name_omp, 50, "stats/thread_lc_oc_stats/thread_lc_oc_stats.%06dx%06u", rank, thread_id);
          stats_omp_fp = fopen(name_omp, "w");
          for (size_t i = 0; i < md_thread_lc_oc.size(); i++)
            fprintf(stats_omp_fp, "%2.14f\n", md_thread_lc_oc[i]);
          fclose(stats_omp_fp);

          // Thread LockContention Pthread Mutex
          snprintf(name_omp, 50, "stats/thread_lc_pm_stats/thread_lc_pm_stats.%06dx%06u", rank, thread_id);
          stats_omp_fp = fopen(name_omp, "w");
          for (size_t i = 0; i < md_thread_lc_pm.size(); i++)
            fprintf(stats_omp_fp, "%2.14f\n", md_thread_lc_pm[i]);
          fclose(stats_omp_fp);

          // Thread LockContention Pthread Condition
          snprintf(name_omp, 50, "stats/thread_lc_pc_stats/thread_lc_pc_stats.%06dx%06u", rank, thread_id);
          stats_omp_fp = fopen(name_omp, "w");
          for (size_t i = 0; i < md_thread_lc_pc.size(); i++)
            fprintf(stats_omp_fp, "%2.14f\n", md_thread_lc_pc[i]);
          fclose(stats_omp_fp);
#endif

        #pragma omp master
        {
#if defined(_MPI)
          char name_mpi[40];
          FILE* stats_mpi_fp;

          // Late Sender values
          snprintf(name_mpi, 40, "stats/ls_stats/ls_stats.%06d", rank);
          stats_mpi_fp = fopen(name_mpi, "w");
          for (size_t i = 0; i < md_ls.size(); i++)
            fprintf(stats_mpi_fp, "%2.14f\n", md_ls[i]);
          fclose(stats_mpi_fp);

          // Late Sender, Wrong Order values
          snprintf(name_mpi, 40, "stats/lsw_stats/lsw_stats.%06d", rank);
          stats_mpi_fp = fopen(name_mpi, "w");
          for (size_t i = 0; i < md_lsw.size(); i++)
            fprintf(stats_mpi_fp, "%2.14f\n", md_lsw[i]);
          fclose(stats_mpi_fp);

          // Late Receiver values
          snprintf(name_mpi, 40, "stats/lr_stats/lr_stats.%06d", rank);
          stats_mpi_fp = fopen(name_mpi, "w");
          for (size_t i = 0; i < md_lr.size(); i++)
            fprintf(stats_mpi_fp, "%2.14f\n", md_lr[i]);
          fclose(stats_mpi_fp);

          // Wait at NxN values
          snprintf(name_mpi, 40, "stats/wnxn_stats/wnxn_stats.%06d", rank);
          stats_mpi_fp = fopen(name_mpi, "w");
          for (size_t i = 0; i < md_wnxn.size(); i++)
            fprintf(stats_mpi_fp, "%2.14f\n", md_wnxn[i]);
          fclose(stats_mpi_fp);

          // Wait at Barrier values
          snprintf(name_mpi, 40, "stats/wb_stats/wb_stats.%06d", rank);
          stats_mpi_fp = fopen(name_mpi, "w");
          for (size_t i = 0; i < md_wb.size(); i++)
            fprintf(stats_mpi_fp, "%2.14f\n", md_wb[i]);
          fclose(stats_mpi_fp);

          // Early Reduce values
          snprintf(name_mpi, 40, "stats/er_stats/er_stats.%06d", rank);
          stats_mpi_fp = fopen(name_mpi, "w");
          for (size_t i = 0; i < md_er.size(); i++)
            fprintf(stats_mpi_fp, "%2.14f\n", md_er[i]);
          fclose(stats_mpi_fp);

          // Early Scan values
          snprintf(name_mpi, 40, "stats/es_stats/es_stats.%06d", rank);
          stats_mpi_fp = fopen(name_mpi, "w");
          for (size_t i = 0; i < md_es.size(); i++)
            fprintf(stats_mpi_fp, "%2.14f\n", md_es[i]);
          fclose(stats_mpi_fp);

          // Late Broadcast values
          snprintf(name_mpi, 40, "stats/lb_stats/lb_stats.%06d", rank);
          stats_mpi_fp = fopen(name_mpi, "w");
          for (size_t i = 0; i < md_lb.size(); i++)
            fprintf(stats_mpi_fp, "%2.14f\n", md_lb[i]);
          fclose(stats_mpi_fp);

          // Barrier Completion values
          snprintf(name_mpi, 40, "stats/bc_stats/bc_stats.%06d", rank);
          stats_mpi_fp = fopen(name_mpi, "w");
          for (size_t i = 0; i < md_bc.size(); i++)
            fprintf(stats_mpi_fp, "%2.14f\n", md_bc[i]);
          fclose(stats_mpi_fp);

          // NxN Completion values
          snprintf(name_mpi, 40, "stats/nxnc_stats/nxnc_stats.%06d", rank);
          stats_mpi_fp = fopen(name_mpi, "w");
          for (size_t i = 0; i < md_nxnc.size(); i++)
            fprintf(stats_mpi_fp, "%2.14f\n", md_nxnc[i]);
          fclose(stats_mpi_fp);
#endif // _MPI
#endif

          // Merge results for each pattern
#if defined(_MPI)
          // define temporary receive buffer 
          TopMostSevere * tmpRecv;
          tmpRecv = new TopMostSevere[TOP_SEVERE_INSTANCES];

          result_top_most(LateSender, tmpRecv);
          result_top_most(LateSenderWO, tmpRecv);
          result_top_most(LateReceiver, tmpRecv);
          result_top_most(EarlyReduce, tmpRecv);
          result_top_most(BarrierNxN, tmpRecv);
          result_top_most(Barrier, tmpRecv);
          result_top_most(NxnCompl, tmpRecv);
          result_top_most(BarrierCompl, tmpRecv);
          result_top_most(LateBcast, tmpRecv);
          result_top_most(EarlyScan, tmpRecv);
#if defined(_OPENMP)
          result_top_most(OmpEBarrier, tmpRecv);
          result_top_most(OmpIBarrier, tmpRecv);
          result_top_most(sharedLC_OL, tmpRecv);
          result_top_most(sharedLC_OC, tmpRecv);
          result_top_most(sharedLC_PM, tmpRecv);
          result_top_most(sharedLC_PC, tmpRecv);
#endif // _OPENMP
          // free temporary receive buffer 
          delete [] tmpRecv;
#endif // _MPI

          if (rank == 0) {
            // Open statistics file
            string filename = archiveDirectory + "/trace.stat";
            FILE*  cube_fp  = fopen(filename.c_str(), "w");
            if (NULL == cube_fp) {
              // Only warn (instead of abort) since we still might get a
              // valid CUBE file
              elg_warning("Could not create statistics file!");
            } else {
              // Write statistics
              fprintf(cube_fp, "PatternName               Count      Mean    Median      Minimum      Maximum      Sum     Variance    Quartil25    Quartil75\n");

#if defined(_MPI)
              ls_quant->set_metric("mpi_latesender");
              ls_quant->calc_quantiles();
              write_cube_file(ls_quant, LateSender, data, cube_fp);

              lsw_quant->set_metric("mpi_latesender_wo");
              lsw_quant->calc_quantiles();
              write_cube_file(lsw_quant, LateSenderWO, data, cube_fp);

              lr_quant->set_metric("mpi_latereceiver");
              lr_quant->calc_quantiles();
              write_cube_file(lr_quant, LateReceiver, data, cube_fp);

              wnxn_quant->set_metric("mpi_wait_nxn");
              wnxn_quant->calc_quantiles();
              write_cube_file(wnxn_quant, BarrierNxN, data, cube_fp);

              wb_quant->set_metric("mpi_barrier_wait");
              wb_quant->calc_quantiles();
              write_cube_file(wb_quant, Barrier, data, cube_fp);

              er_quant->set_metric("mpi_earlyreduce");
              er_quant->calc_quantiles();
              write_cube_file(er_quant, EarlyReduce, data, cube_fp);

              es_quant->set_metric("mpi_earlyscan");
              es_quant->calc_quantiles();
              write_cube_file(es_quant, EarlyScan, data, cube_fp);

              lb_quant->set_metric("mpi_latebroadcast");
              lb_quant->calc_quantiles();
              write_cube_file(lb_quant, LateBcast, data, cube_fp);

              bc_quant->set_metric("mpi_barrier_completion");
              bc_quant->calc_quantiles();
              write_cube_file(bc_quant, BarrierCompl, data, cube_fp);

              nxnc_quant->set_metric("mpi_nxn_completion");
              nxnc_quant->calc_quantiles();
              write_cube_file(nxnc_quant, NxnCompl, data, cube_fp);
#endif // _MPI

#if defined(_OPENMP)
              omp_eb_quant->set_metric("omp_ebarrier_wait");
              omp_eb_quant->calc_quantiles();
              write_cube_file(omp_eb_quant, OmpEBarrier, data, cube_fp);

              omp_ib_quant->set_metric("omp_ibarrier_wait");
              omp_ib_quant->calc_quantiles();
              write_cube_file(omp_ib_quant, OmpIBarrier, data, cube_fp);

              thread_lc_ol_quant->set_metric("omp_lock_contention_api");
              thread_lc_ol_quant->calc_quantiles();
              write_cube_file(thread_lc_ol_quant, sharedLC_OL, data, cube_fp);

              thread_lc_oc_quant->set_metric("omp_lock_contention_critical");
              thread_lc_oc_quant->calc_quantiles();
              write_cube_file(thread_lc_oc_quant, sharedLC_OC, data, cube_fp);

              thread_lc_pm_quant->set_metric("pthread_lock_contention_mutex_lock");
              thread_lc_pm_quant->calc_quantiles();
              write_cube_file(thread_lc_pm_quant, sharedLC_PM, data, cube_fp);

              thread_lc_pc_quant->set_metric("pthread_lock_contention_conditional");
              thread_lc_pc_quant->calc_quantiles();
              write_cube_file(thread_lc_pc_quant, sharedLC_PC, data, cube_fp);
#endif // _OPENMP

              // Close statistics file
              fclose(cube_fp);
              
#ifdef WRITE_CONTROL_VALUES
              // Write coefficients per pattern for debugging
#if defined(_MPI)
              write_values(ls_quant);
              write_values(lsw_quant);
              write_values(lr_quant);
              write_values(wnxn_quant);
              write_values(wb_quant);
              write_values(er_quant);
              write_values(es_quant);
              write_values(lb_quant);
              write_values(bc_quant);
              write_values(nxnc_quant);
#endif // _MPI

#if defined(_OPENMP)
              write_values(omp_eb_quant);
              write_values(omp_ib_quant);
              write_values(thread_lc_ol_quant);
              write_values(thread_lc_oc_quant);
              write_values(thread_lc_pm_quant);
              write_values(thread_lc_pc_quant);
#endif // _OPENMP
#endif

            }
          }
        }
      }
  
};


/*----- Static class data -----*/


#line 56 "Statistics.pattern"

#if defined(_OPENMP)
    vector<TopMostSevere> PatternStatistics::sharedLC_OL;
    vector<TopMostSevere> PatternStatistics::sharedLC_OC;
    vector<TopMostSevere> PatternStatistics::sharedLC_PM;
    vector<TopMostSevere> PatternStatistics::sharedLC_PC;

    TopMostSevereMaxSum PatternStatistics::sharedMaxSum;
#endif // (_OPENMP)

    PatternStatistics::MaxDurations PatternStatistics::sharedDurations;
  


/*----- Callback methods -----*/

void PatternStatistics::barrier_compl__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1520 "Statistics.pattern"

#if defined(_MPI)
      if (data->mIdle > 0)
        bc_quant->add_value(data->mIdle);
      else
        data->mIdle = 0.0;
        
      MpiComm* comm = event->getComm();

      Event begin = data->mLocal->get_event(ROLE_BEGIN_COLL);

      stat_collective(BarrierCompl, data->mIdle,
                      begin->getTimestamp(),
                      event->getTimestamp(),
                      event.get_cnode()->getId(),
                      event.get_location().getRank(),
                      comm);

#ifdef WRITE_CONTROL_VALUES
      if (data->mIdle > 0)
        md_bc.push_back(data->mIdle);
#endif
#else   // !_MPI
      // Ugly hack to avoid 'unused variable' warning
      (void)data;
#endif   // _MPI
    
}

void PatternStatistics::early_reduce__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1410 "Statistics.pattern"

#if defined(_MPI)
      er_quant->add_value(data->mIdle);

      Event begin = data->mLocal->get_event(ROLE_BEGIN_COLL);
      stat_collector(EarlyReduce,
                     data->mIdle,
                     begin->getTimestamp(),
                     event->getTimestamp(),
                     event.get_cnode()->getId(),
                     event.get_location().getRank());

#ifdef WRITE_CONTROL_VALUES
      if (data->mIdle > 0)
        md_er.push_back(data->mIdle);
#endif
#else   // !_MPI
      // Ugly hack to avoid 'unused variable' warning
      (void)data;
#endif   // _MPI
    
}

void PatternStatistics::early_scan__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1433 "Statistics.pattern"

#if defined(_MPI)
      if (data->mIdle > 0)
        es_quant->add_value(data->mIdle);
      else
        data->mIdle = 0.0;
        
      MpiComm* comm = event->getComm();
      
      Event begin = data->mLocal->get_event(ROLE_BEGIN_COLL);

      stat_collective(EarlyScan, data->mIdle,
                      begin->getTimestamp(),
                      event->getTimestamp(),
                      event.get_cnode()->getId(),
                      event.get_location().getRank(),
                      comm);
      
#ifdef WRITE_CONTROL_VALUES
      if (data->mIdle > 0)
        md_es.push_back(data->mIdle);
#endif
#else   // !_MPI
      // Ugly hack to avoid 'unused variable' warning
      (void)data;
#endif   // _MPI
    
}

void PatternStatistics::late_bcast__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1462 "Statistics.pattern"

#if defined(_MPI)
      if (data->mIdle > 0)
        lb_quant->add_value(data->mIdle);
      else
        data->mIdle = 0.0;
        
      MpiComm* comm = event->getComm();

      Event begin = data->mLocal->get_event(ROLE_BEGIN_COLL);
      
      stat_collective(LateBcast, data->mIdle,
                      begin->getTimestamp(),
                      event->getTimestamp(),
                      event.get_cnode()->getId(),
                      event.get_location().getRank(),
                      comm);

#ifdef WRITE_CONTROL_VALUES
      if (data->mIdle > 0)
        md_lb.push_back(data->mIdle);
#endif
#else   // !_MPI
      // Ugly hack to avoid 'unused variable' warning
      (void)data;
#endif   // _MPI
    
}

void PatternStatistics::late_receiver_bws_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1726 "Statistics.pattern"

#if defined(_MPI)
      lr_quant->add_value(data->mIdle);
      
      Event enter = data->mLocal->get_event(ROLE_ENTER_SEND_LR);
      
      stat_collector(LateReceiver,
                     data->mIdle,
                     enter->getTimestamp(),
                     event->getTimestamp(),
                     enter.get_cnode()->getId(),
                     event.get_location().getRank());
                     
#ifdef WRITE_CONTROL_VALUES
      md_lr.push_back(data->mIdle);
#endif
#else   // !_MPI
      // Ugly hack to avoid 'unused variable' warning
      (void)data;
#endif   // _MPI
    
}

void PatternStatistics::late_sender__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1309 "Statistics.pattern"

#if defined(_MPI)
      ls_quant->add_value(data->mIdle);

      Event enter = data->mLocal->get_event(ROLE_RECV_LS);

      stat_collector(LateSender,
                     data->mIdle,
                     enter.enterptr()->getTimestamp(),
                     event->getTimestamp(),
                     enter.get_cnode()->getId(),
                     event.get_location().getRank());
      
#ifdef WRITE_CONTROL_VALUES
      md_ls.push_back(data->mIdle);
#endif
#else   // !_MPI
      // Ugly hack to avoid 'unused variable' warning
      (void)data;
#endif   // _MPI
    
}

void PatternStatistics::late_sender_wo__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1332 "Statistics.pattern"

#if defined(_MPI)
      lsw_quant->add_value(data->mIdle);
      
      RemoteEvent enter = data->mRemote->get_event(ROLE_SEND);
      Event exit = data->mLocal->get_event(ROLE_RECV_LSWO);
      
      stat_collector(LateSenderWO,
                     data->mIdle,
                     enter->getTimestamp(),
                     event->getTimestamp(),
                     exit.get_cnode()->getId(),
                     event.get_location().getRank());
                     
#ifdef WRITE_CONTROL_VALUES
      md_lsw.push_back(data->mIdle);
#endif
#else   // !_MPI
      // Ugly hack to avoid 'unused variable' warning
      (void)data;
#endif   // _MPI
    
}

void PatternStatistics::nxn_compl__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1491 "Statistics.pattern"

#if defined(_MPI)
      if (data->mIdle > 0)
        nxnc_quant->add_value(data->mIdle);
      else
        data->mIdle = 0.0;
        
      MpiComm* comm = event->getComm();

      Event begin = data->mLocal->get_event(ROLE_BEGIN_COLL);

      stat_collective(NxnCompl, data->mIdle,
                      begin->getTimestamp(),
                      event->getTimestamp(),
                      event.get_cnode()->getId(),
                      event.get_location().getRank(),
                      comm);

#ifdef WRITE_CONTROL_VALUES
      if (data->mIdle > 0)
        md_nxnc.push_back(data->mIdle);
#endif
#else   // !_MPI
      // Ugly hack to avoid 'unused variable' warning
      (void)data;
#endif   // _MPI
    
}

void PatternStatistics::omp_ebarrier_wait__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1549 "Statistics.pattern"

#if defined(_OPENMP)
      if (data->mIdle > 0)
        omp_eb_quant->add_value(data->mIdle);
      else
        data->mIdle = 0.0;

#if defined(_MPI)
      stat_collective(OmpEBarrier, data->mIdle,
                      data->mCallstack->top()->getTimestamp(),
                      event->getTimestamp(),
                      event.get_cnode()->getId(),
                      event.get_location().getRank());
#else   // set rank 0 for pure OpenMP case
      stat_collective(OmpEBarrier, data->mIdle,
                      data->mCallstack->top()->getTimestamp(),
                      event->getTimestamp(),
                      event.get_cnode()->getId(),
                      0);
#endif // !_MPI

#ifdef WRITE_CONTROL_VALUES
      if (data->mIdle > 0)
        md_omp_eb.push_back(data->mIdle);
#endif
#else   // !_OPENMP
      // Ugly hack to avoid 'unused variable' warning
      (void)data;
#endif   // _OPENMP
    
}

void PatternStatistics::omp_ibarrier_wait__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1581 "Statistics.pattern"

#if defined(_OPENMP)
      if (data->mIdle > 0)
        omp_ib_quant->add_value(data->mIdle);
      else
        data->mIdle = 0.0;

#if defined(_MPI)
      stat_collective(OmpIBarrier, data->mIdle,
                      data->mCallstack->top()->getTimestamp(),
                      event->getTimestamp(),
                      event.get_cnode()->getId(),
                      event.get_location().getRank());
#else   // set rank 0 for pure OpenMP case
      stat_collective(OmpIBarrier, data->mIdle,
                      data->mCallstack->top()->getTimestamp(),
                      event->getTimestamp(),
                      event.get_cnode()->getId(),
                      0);
#endif // !_MPI

#ifdef WRITE_CONTROL_VALUES
      if (data->mIdle > 0)
        md_omp_ib.push_back(data->mIdle);
#endif
#else   // !_OPENMP
      // Ugly hack to avoid 'unused variable' warning
      (void)data;
#endif   // _OPENMP
    
}

void PatternStatistics::prepare__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1215 "Statistics.pattern"

      // Determine local upper bounds for pattern durations
      CallbackManager cb;
      MaxDurations    durations;

#if defined(_MPI)
      cb.register_callback(MPI_COLLECTIVE_END, PEARL_create_callback(this, &PatternStatistics::mpicollective_cb));
      cb.register_callback(GROUP_SEND, PEARL_create_callback(this, &PatternStatistics::send_cb));
      cb.register_callback(GROUP_RECV, PEARL_create_callback(this, &PatternStatistics::recv_cb));
#endif // _MPI

#if defined(_OPENMP)
      cb.register_callback(LEAVE, PEARL_create_callback(this, &PatternStatistics::leave_cb));
      cb.register_callback(THREAD_ACQUIRE_LOCK, PEARL_create_callback(this, &PatternStatistics::acquire_cb));
#endif // _OPENMP
      
      PEARL_forward_replay(*(data->mTrace), cb, &durations);
      
      // Determine global upper bounds for pattern durations
      #pragma omp master
      {
        sharedDurations.m_max_duration[OMP_EB_MAX_DURATION]       = - DBL_MAX;
        sharedDurations.m_max_duration[OMP_IB_MAX_DURATION]       = - DBL_MAX;
        sharedDurations.m_max_duration[THREAD_LC_OL_MAX_DURATION] = - DBL_MAX;
        sharedDurations.m_max_duration[THREAD_LC_OC_MAX_DURATION] = - DBL_MAX;
        sharedDurations.m_max_duration[THREAD_LC_PM_MAX_DURATION] = - DBL_MAX;
        sharedDurations.m_max_duration[THREAD_LC_PC_MAX_DURATION] = - DBL_MAX;
      }
      
      #pragma omp barrier 
      #pragma omp critical
      {
        if (sharedDurations.m_max_duration[OMP_EB_MAX_DURATION] < durations.m_max_duration[OMP_EB_MAX_DURATION]) {
          sharedDurations.m_max_duration[OMP_EB_MAX_DURATION] = durations.m_max_duration[OMP_EB_MAX_DURATION];
        }
        if (sharedDurations.m_max_duration[OMP_IB_MAX_DURATION] < durations.m_max_duration[OMP_IB_MAX_DURATION]) {
          sharedDurations.m_max_duration[OMP_IB_MAX_DURATION] = durations.m_max_duration[OMP_IB_MAX_DURATION];
        }
        if (sharedDurations.m_max_duration[THREAD_LC_OL_MAX_DURATION] < durations.m_max_duration[THREAD_LC_OL_MAX_DURATION]) {
          sharedDurations.m_max_duration[THREAD_LC_OL_MAX_DURATION] = durations.m_max_duration[THREAD_LC_OL_MAX_DURATION];
        }
        if (sharedDurations.m_max_duration[THREAD_LC_OC_MAX_DURATION] < durations.m_max_duration[THREAD_LC_OC_MAX_DURATION]) {
          sharedDurations.m_max_duration[THREAD_LC_OC_MAX_DURATION] = durations.m_max_duration[THREAD_LC_OC_MAX_DURATION];
        }
        if (sharedDurations.m_max_duration[THREAD_LC_PM_MAX_DURATION] < durations.m_max_duration[THREAD_LC_PM_MAX_DURATION]) {
          sharedDurations.m_max_duration[THREAD_LC_PM_MAX_DURATION] = durations.m_max_duration[THREAD_LC_PM_MAX_DURATION];
        }
        if (sharedDurations.m_max_duration[THREAD_LC_PC_MAX_DURATION] < durations.m_max_duration[THREAD_LC_PC_MAX_DURATION]) {
          sharedDurations.m_max_duration[THREAD_LC_PC_MAX_DURATION] = durations.m_max_duration[THREAD_LC_PC_MAX_DURATION];
        }
      }
      #pragma omp barrier
      
#if defined(_MPI)
      #pragma omp master
      {
        durations.m_max_duration[OMP_EB_MAX_DURATION]       = sharedDurations.m_max_duration[OMP_EB_MAX_DURATION];
        durations.m_max_duration[OMP_IB_MAX_DURATION]       = sharedDurations.m_max_duration[OMP_IB_MAX_DURATION];
        durations.m_max_duration[THREAD_LC_OL_MAX_DURATION] = sharedDurations.m_max_duration[THREAD_LC_OL_MAX_DURATION];
        durations.m_max_duration[THREAD_LC_OC_MAX_DURATION] = sharedDurations.m_max_duration[THREAD_LC_OC_MAX_DURATION];
        durations.m_max_duration[THREAD_LC_PM_MAX_DURATION] = sharedDurations.m_max_duration[THREAD_LC_PM_MAX_DURATION];
        durations.m_max_duration[THREAD_LC_PC_MAX_DURATION] = sharedDurations.m_max_duration[THREAD_LC_PC_MAX_DURATION];
        MPI_Allreduce(durations.m_max_duration,
                      sharedDurations.m_max_duration,
                      MAX_DURATION_ENTRIES, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
      }
#endif // _MPI

      #pragma omp barrier
      
#if defined(_MPI)
      ls_quant->set_factor_by_upperbound(sharedDurations.m_max_duration[LS_MAX_DURATION]);
      lsw_quant->set_factor_by_upperbound(sharedDurations.m_max_duration[LS_MAX_DURATION]);
      lr_quant->set_factor_by_upperbound(sharedDurations.m_max_duration[LR_MAX_DURATION]);
      wnxn_quant->set_factor_by_upperbound(sharedDurations.m_max_duration[WNXN_MAX_DURATION]);
      wb_quant->set_factor_by_upperbound(sharedDurations.m_max_duration[WB_MAX_DURATION]);
      er_quant->set_factor_by_upperbound(sharedDurations.m_max_duration[ER_MAX_DURATION]);
      es_quant->set_factor_by_upperbound(sharedDurations.m_max_duration[ES_MAX_DURATION]);
      lb_quant->set_factor_by_upperbound(sharedDurations.m_max_duration[LB_MAX_DURATION]);
      bc_quant->set_factor_by_upperbound(sharedDurations.m_max_duration[BC_MAX_DURATION]);
      nxnc_quant->set_factor_by_upperbound(sharedDurations.m_max_duration[NXNC_MAX_DURATION]);
#endif // _MPI

#if defined(_OPENMP)
      omp_eb_quant->set_factor_by_upperbound(sharedDurations.m_max_duration[OMP_EB_MAX_DURATION]);
      omp_ib_quant->set_factor_by_upperbound(sharedDurations.m_max_duration[OMP_IB_MAX_DURATION]);
      thread_lc_ol_quant->set_factor_by_upperbound(sharedDurations.m_max_duration[THREAD_LC_OL_MAX_DURATION]);
      thread_lc_oc_quant->set_factor_by_upperbound(sharedDurations.m_max_duration[THREAD_LC_OC_MAX_DURATION]);
      thread_lc_pm_quant->set_factor_by_upperbound(sharedDurations.m_max_duration[THREAD_LC_PM_MAX_DURATION]);
      thread_lc_pc_quant->set_factor_by_upperbound(sharedDurations.m_max_duration[THREAD_LC_PC_MAX_DURATION]);
#endif // _OPENMP  
    
}

void PatternStatistics::thread_lock_contention_ompcritical_wait__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1641 "Statistics.pattern"

#if defined(_OPENMP)
      thread_lc_oc_quant->add_value(data->mIdle);

#if defined(_MPI)
      stat_collector(ThreadLockContentionOMPCritical, data->mIdle,
                      event.enterptr()->getTimestamp(),
                      event->getTimestamp(), 
                      event.get_cnode()->getId(),
                      event.get_location().getRank());
#else   // set rank 0 for pure OpenMP case
      stat_collector(ThreadLockContentionOMPCritical, data->mIdle,
                      event.enterptr()->getTimestamp(), 
                      event->getTimestamp(),
                      event.get_cnode()->getId(),
                      0);
#endif // !_MPI

#ifdef WRITE_CONTROL_VALUES
      md_thread_lc_oc.push_back(data->mIdle);
#endif
#else   // !_OPENMP
      // Ugly hack to avoid 'unused variable' warning
      (void)data;
#endif   // _OPENMP
    
}

void PatternStatistics::thread_lock_contention_omplock_wait__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1613 "Statistics.pattern"

#if defined(_OPENMP)
      thread_lc_ol_quant->add_value(data->mIdle);

#if defined(_MPI)
      stat_collector(ThreadLockContentionOMPLockAPI, data->mIdle,
                      event.enterptr()->getTimestamp(),
                      event->getTimestamp(), 
                      event.get_cnode()->getId(),
                      event.get_location().getRank());
#else   // set rank 0 for pure OpenMP case
      stat_collector(ThreadLockContentionOMPLockAPI, data->mIdle,
                      event.enterptr()->getTimestamp(), 
                      event->getTimestamp(),
                      event.get_cnode()->getId(),
                      0);
#endif // !_MPI

#ifdef WRITE_CONTROL_VALUES
      md_thread_lc_ol.push_back(data->mIdle);
#endif
#else   // !_OPENMP
      // Ugly hack to avoid 'unused variable' warning
      (void)data;
#endif   // _OPENMP
    
}

void PatternStatistics::thread_lock_contention_pthreadcondition_wait__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1697 "Statistics.pattern"

#if defined(_OPENMP)
      thread_lc_pc_quant->add_value(data->mIdle);

#if defined(_MPI)
      stat_collector(ThreadLockContentionPthreadCondition, data->mIdle,
                      event.enterptr()->getTimestamp(),
                      event->getTimestamp(), 
                      event.get_cnode()->getId(),
                      event.get_location().getRank());
#else   // set rank 0 for pure OpenMP case
      stat_collector(ThreadLockContentionPthreadCondition, data->mIdle,
                      event.enterptr()->getTimestamp(), 
                      event->getTimestamp(),
                      event.get_cnode()->getId(),
                      0);
#endif // !_MPI

#ifdef WRITE_CONTROL_VALUES
      md_thread_lc_pc.push_back(data->mIdle);
#endif
#else   // !_OPENMP
      // Ugly hack to avoid 'unused variable' warning
      (void)data;
#endif   // _OPENMP
    
}

void PatternStatistics::thread_lock_contention_pthreadmutex_wait__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1669 "Statistics.pattern"

#if defined(_OPENMP)
      thread_lc_pm_quant->add_value(data->mIdle);

#if defined(_MPI)
      stat_collector(ThreadLockContentionPthreadMutex, data->mIdle,
                      event.enterptr()->getTimestamp(),
                      event->getTimestamp(), 
                      event.get_cnode()->getId(),
                      event.get_location().getRank());
#else   // set rank 0 for pure OpenMP case
      stat_collector(ThreadLockContentionPthreadMutex, data->mIdle,
                      event.enterptr()->getTimestamp(), 
                      event->getTimestamp(),
                      event.get_cnode()->getId(),
                      0);
#endif // !_MPI

#ifdef WRITE_CONTROL_VALUES
      md_thread_lc_pm.push_back(data->mIdle);
#endif
#else   // !_OPENMP
      // Ugly hack to avoid 'unused variable' warning
      (void)data;
#endif   // _OPENMP
    
}

void PatternStatistics::wait_barrier__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1382 "Statistics.pattern"

#if defined(_MPI)
      if (data->mIdle > 0)
        wb_quant->add_value(data->mIdle);
      else
        data->mIdle = 0.0;
      
      MpiComm* comm = event->getComm();
      Event begin   = data->mLocal->get_event(ROLE_BEGIN_COLL);

      stat_collective(Barrier, data->mIdle,
                      begin->getTimestamp(),
                      event->getTimestamp(),
                      event.get_cnode()->getId(),
                      event.get_location().getRank(),
                      comm);

#ifdef WRITE_CONTROL_VALUES
      if (data->mIdle > 0)
        md_wb.push_back(data->mIdle);
#endif
#else   // !_MPI
      // Ugly hack to avoid 'unused variable' warning
      (void)data;
#endif   // _MPI
    
}

void PatternStatistics::wait_nxn__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1355 "Statistics.pattern"

#if defined(_MPI)
      if (data->mIdle > 0)
        wnxn_quant->add_value(data->mIdle);
      else
        data->mIdle = 0.0;
      
      MpiComm* comm = event->getComm();

      stat_collective(BarrierNxN, data->mIdle,
                      data->mCallstack->top()->getTimestamp(),
                      event->getTimestamp(),
                      event.get_cnode()->getId(),
                      event.get_location().getRank(),
                      comm);

#ifdef WRITE_CONTROL_VALUES
      if (data->mIdle > 0)
        md_wnxn.push_back(data->mIdle);
#endif
#else   // !_MPI
      // Ugly hack to avoid 'unused variable' warning
      (void)data;
#endif   // _MPI
    
}


/*
 *---------------------------------------------------------------------------
 *
 * class PatternCriticalPath
 *
 *---------------------------------------------------------------------------
 */


class PatternCriticalPath : public Pattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("bwc");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(PREPARE, PEARL_create_callback(this, &PatternCriticalPath::prepare_bwc_cb));
        (it->second)->register_callback(FINISHED, PEARL_create_callback(this, &PatternCriticalPath::finished_bwc_cb));
        (it->second)->register_callback(OMP_MGMT_FORK, PEARL_create_callback(this, &PatternCriticalPath::omp_mgmt_fork_bwc_cb));
        (it->second)->register_callback(OMP_BARRIER, PEARL_create_callback(this, &PatternCriticalPath::omp_barrier_bwc_cb));
        (it->second)->register_callback(FINALIZE_END, PEARL_create_callback(this, &PatternCriticalPath::finalize_end_bwc_cb));
        (it->second)->register_callback(INIT_END, PEARL_create_callback(this, &PatternCriticalPath::init_end_bwc_cb));
        (it->second)->register_callback(COLL_12N, PEARL_create_callback(this, &PatternCriticalPath::coll_12n_bwc_cb));
        (it->second)->register_callback(COLL_N2N, PEARL_create_callback(this, &PatternCriticalPath::coll_n2n_bwc_cb));
        (it->second)->register_callback(SYNC_COLL, PEARL_create_callback(this, &PatternCriticalPath::sync_coll_bwc_cb));
        (it->second)->register_callback(COLL_N21, PEARL_create_callback(this, &PatternCriticalPath::coll_n21_bwc_cb));
        (it->second)->register_callback(PRE_RECV, PEARL_create_callback(this, &PatternCriticalPath::pre_recv_bwc_cb));
        (it->second)->register_callback(POST_SEND, PEARL_create_callback(this, &PatternCriticalPath::post_send_bwc_cb));
        (it->second)->register_callback(POST_INV_RECVREQ, PEARL_create_callback(this, &PatternCriticalPath::post_inv_recvreq_bwc_cb));
        (it->second)->register_callback(PRE_INV_SENDCMP, PEARL_create_callback(this, &PatternCriticalPath::pre_inv_sendcmp_bwc_cb));
      }
      {
        CallbackManagerMap::iterator it = cbmgrs.find("bws");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(PREPARE, PEARL_create_callback(this, &PatternCriticalPath::prepare_bws_cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_CRITICAL_PATH;
    }

    virtual long get_parent() const
    {
      return PAT_NONE;
    }

    virtual string get_name() const
    {
      return "Critical path profile";
    }

    virtual string get_unique_name() const
    {
      return "critical_path";
    }

    virtual string get_descr() const
    {
      return "Profile of the application's critical path";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void coll_12n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void coll_n21_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void coll_n2n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void finalize_end_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void finished_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void init_end_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void omp_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void omp_mgmt_fork_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void post_inv_recvreq_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void post_send_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void prepare_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void prepare_bws_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_inv_sendcmp_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_recv_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void sync_coll_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);


  private:

#line 86 "CriticalPath.pattern"

    bool  m_on_critical_path;
    Event m_end_event;
    Event m_finalize_event;

    static std::vector<double> m_tmpvec;

#ifdef CPADEBUG
    std::ofstream m_dbg;
#endif

    void update_path_severities(Event from, CbData* data) {
      timemap_t tmap = data->mTmapCacheHandler->getRuntimes(from, m_end_event);
      
      for (timemap_t::const_iterator it = tmap.begin(); it != tmap.end(); ++it)
        m_severity[data->mDefs->get_cnode(it->first)] += it->second;

      SynchpointInfo spi = data->mSynchpointHandler->getSynchpointInfo(from);

      m_severity[from.get_cnode()] -= std::min(spi.waitTime, tmap[from.get_cnode()->getId()]);
    }

    void update_first_path_severities(CbData* data) {
      timemap_t tmap = data->mTmapCacheHandler->getRuntimes(Event(), m_end_event);
      
      for (timemap_t::const_iterator it = tmap.begin(); it != tmap.end(); ++it)
        m_severity[data->mDefs->get_cnode(it->first)] += it->second;
    }

    void collate_global_critical_path(CbData* data) {
      uint32_t ncpaths = data->mDefs->numCallpaths();

      // Collate global critical-path profile
      data->mGlobalCriticalPath.resize(ncpaths, 0.0);
      data->mLocalCriticalPath.resize(ncpaths, 0.0);

      for (map<Callpath*, double>::const_iterator it = m_severity.begin(); it != m_severity.end(); ++it)
        data->mLocalCriticalPath[(it->first)->getId()] = it->second;

#pragma omp master
      {
        m_tmpvec.assign(ncpaths, 0.0);
      }
#pragma omp barrier
      { }
#pragma omp critical
      {
        std::transform(m_tmpvec.begin(), m_tmpvec.end(), data->mLocalCriticalPath.begin(),
                       m_tmpvec.begin(), std::plus<double>());
      }
#pragma omp barrier
      { }

#ifdef _MPI
#pragma omp master
      {
        MPI_Allreduce(MPI_IN_PLACE, &m_tmpvec[0], ncpaths, MPI_DOUBLE, 
                      MPI_SUM, MPI_COMM_WORLD);
      }
#endif

#pragma omp barrier
      { }
      data->mGlobalCriticalPath.assign(m_tmpvec.begin(), m_tmpvec.end());

      // Collate local non-waiting time profile
      data->mLocalTimeProfile.resize(ncpaths, 0.0);
      
      timemap_t ltmap = data->mTmapCacheHandler->getRuntimes(Event(), Event());
      TimeMapSum wtsm = CalculateWaitMap(data->mSynchpointHandler->getSynchpoints());

      for (timemap_t::const_iterator it = ltmap.begin(); it != ltmap.end(); ++it) {
        data->mLocalTimeProfile[it->first] += it->second;

        timemap_t::const_iterator wit = wtsm.mMap.find(it->first);

        if (wit != wtsm.mMap.end())
          data->mLocalTimeProfile[it->first] -= std::min(it->second, wit->second);
      }        
    }
  
};


/*----- Static class data -----*/


#line 83 "CriticalPath.pattern"

    std::vector<double> PatternCriticalPath::m_tmpvec;
  


/*----- Callback methods -----*/

void PatternCriticalPath::coll_12n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 310 "CriticalPath.pattern"

#ifdef _MPI
        CollectiveInfo& ci(data->mCollinfo);

        int iflag = 0;

        if (m_on_critical_path && data->mSynchpointHandler->isWaitstate(event)) {
          // --- set severities
          update_path_severities(event, data);

          iflag              = 1;
          m_on_critical_path = false;          
        }

        int oflag = 0;

        MPI_Reduce(&iflag, &oflag, 1, MPI_INT, MPI_MAX, ci.root.rank, event->getComm()->getComm());

        if (oflag) {
          m_on_critical_path = true;
          m_end_event        = event;
#ifdef CPADEBUG
          if (!data->mSynchpointHandler->isSynchpoint(event))
            m_dbg << "COLL_12N on critical path is not a registered synchpoint!" << endl;
#endif
        }
#endif
    
}

void PatternCriticalPath::coll_n21_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 397 "CriticalPath.pattern"

#ifdef _MPI
        CollectiveInfo& ci(data->mCollinfo);

        /*
         * Treat reductions differently than regular wait-state analysis:
         *   Continue on last _rank_ entering the reduction if:
         *     - root is on the critical path
         *     *or*
         *     - someone is on the critical path, 
         *       and this is a globally synchronizing reduction (max enter < min exit)
         */        

        // move critical path flag (if necessary)        

        int iflag = 0;

        if (m_on_critical_path && 
            ((ci.my.rank == ci.root.rank && ci.root.time < ci.latest.time) ||
             (ci.latest.time < ci.earliest_end.time))) {
          // --- set severities
          update_path_severities(event, data);

          iflag              = 1;
          m_on_critical_path = false;          
        }

        int oflag = 0;

        MPI_Reduce(&iflag, &oflag, 1, MPI_INT, MPI_MAX, ci.latest.rank, event->getComm()->getComm());

        if (oflag) {
          m_on_critical_path = true;
          m_end_event        = event;
#ifdef CPADEBUG
          if (!data->mSynchpointHandler->isSynchpoint(event))
            m_dbg << "COLL_N21 on critical path is not a registered synchpoint!" << endl;
#endif
        }
#endif
    
}

void PatternCriticalPath::coll_n2n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 339 "CriticalPath.pattern"

#ifdef _MPI
        CollectiveInfo& ci(data->mCollinfo);

        int iflag = 0;

        if (m_on_critical_path && data->mSynchpointHandler->isWaitstate(event)) {
          // --- set severities
          update_path_severities(event, data);
          
          iflag              = 1;
          m_on_critical_path = false;
        }

        int oflag = 0;

        MPI_Reduce(&iflag, &oflag, 1, MPI_INT, MPI_MAX, ci.latest.rank, event->getComm()->getComm());
        
        if (oflag) {
          m_on_critical_path = true;
          m_end_event        = event;
#ifdef CPADEBUG
          if (!data->mSynchpointHandler->isSynchpoint(event))
            m_dbg << "COLL_N2N on critical path is not a registered synchpoint!" << endl;
#endif
        }
#endif
    
}

void PatternCriticalPath::finalize_end_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 290 "CriticalPath.pattern"

      CollectiveInfo& ci(data->mCollinfo);

      if (ci.my.rank == ci.latest.rank) {
        m_on_critical_path = true;
        m_end_event        = event;
#ifdef CPADEBUG
        if (!data->mSynchpointHandler->isSynchpoint(event))
          m_dbg << "FINALIZE_END on critical path is not a registered synchpoint!" << endl;
#endif
      }

      m_finalize_event = event;
    
}

void PatternCriticalPath::finished_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 198 "CriticalPath.pattern"

#ifndef _MPI
#pragma omp master
      {
        update_first_path_severities(data);
      }      
#endif
      collate_global_critical_path(data);
      cbmanager.notify(CRITICALPATH_FINISHED, event, data);
    
}

void PatternCriticalPath::init_end_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 305 "CriticalPath.pattern"

      if (m_on_critical_path)
        update_path_severities(event, data);
    
}

void PatternCriticalPath::omp_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 252 "CriticalPath.pattern"

#ifdef _OPENMP
      static bool flag;

#pragma omp single
      {
        flag = false;
      }
#pragma omp critical
      {
        if (m_on_critical_path) {
          // --- set severities
          update_path_severities(event, data);

          m_on_critical_path = false;
          flag               = true;
#ifdef CPADEBUG
          m_dbg << "Was on critical path in omp barrier" << std::endl;
#endif
        }
      }

#pragma omp barrier
      { }

      if (flag && !data->mSynchpointHandler->isWaitstate(event)) {
        // FIXME: bit of a problem here ... might have two critical paths 
        m_on_critical_path = true;
        m_end_event        = event;
#ifdef CPADEBUG
        m_dbg << "Now on critical path in omp barrier" << std::endl;
#endif
      }
#pragma omp barrier
      { }
#endif 
    
}

void PatternCriticalPath::omp_mgmt_fork_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 209 "CriticalPath.pattern"

#ifdef _OPENMP
      static bool flag;

#pragma omp single
      {
        flag = false;
      }
#pragma omp critical
      {
        if (m_on_critical_path) {
          // --- set severities
          update_path_severities(event, data);
        
          m_on_critical_path = false;
          flag               = true;

#ifdef CPADEBUG
          m_dbg << "Was on critical path in fork" << std::endl;
#endif
        }
      }

#pragma omp barrier
      { }

#pragma omp master
      {          
        if (flag) {
          m_on_critical_path = true;
          m_end_event        = event;

#ifdef CPADEBUG
          m_dbg << "Now on critical path in fork" << std::endl;
#endif
        }        
      }
#pragma omp barrier
      { }

#endif
    
}

void PatternCriticalPath::post_inv_recvreq_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 479 "CriticalPath.pattern"

#ifdef _MPI
      // --- get critical path info flag

      uint32_t flag = data->mInvRemote->get_buffer(BUFFER_CRITICALPATH)->get_uint32();

      if (flag) {
        m_on_critical_path = true;
        m_end_event        = event;
#ifdef CPADEBUG
          if (!data->mSynchpointHandler->isSynchpoint(event))
            m_dbg << "RECVREQ on critical path is not a registered synchpoint!" << endl;
#endif
      }
#endif
    
}

void PatternCriticalPath::post_send_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 459 "CriticalPath.pattern"

#ifdef _MPI
      if (!data->mSynchpointHandler->isSynchpoint(event))
        return;

      // --- get critical path info flag

      uint32_t flag = data->mRemote->get_buffer(BUFFER_CRITICALPATH)->get_uint32();

      if (flag) {
        m_on_critical_path = true;
        m_end_event        = event;
#ifdef CPADEBUG
          if (!data->mSynchpointHandler->isSynchpoint(event))
            m_dbg << "SEND on critical path is not a registered synchpoint!" << endl;
#endif
      }
#endif
    
}

void PatternCriticalPath::prepare_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 188 "CriticalPath.pattern"

      m_on_critical_path = false;
#ifndef _MPI 
#pragma omp master
      {
        m_on_critical_path = true;
      }
#endif
    
}

void PatternCriticalPath::prepare_bws_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 169 "CriticalPath.pattern"

#ifdef CPADEBUG
      int rank = 0, thread = 0;
#ifdef _MPI
      MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif
#ifdef _OPENMP
      thread = omp_get_thread_num();
#endif
      std::ostringstream filename;
      filename << "cpadbg_r" << rank << "t" << thread << ".out";
      m_dbg.open(filename.str().c_str());

      m_dbg << "bws prepare: mUpdateCallstack is " << data->mCallstack.getUpdate() << std::endl;
#endif // CPADEBUG
    
}

void PatternCriticalPath::pre_inv_sendcmp_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 496 "CriticalPath.pattern"

#ifdef _MPI
      Buffer*  buffer = new Buffer(16);
      uint32_t flag   = 0;

      if (m_on_critical_path && data->mSynchpointHandler->isWaitstate(event)) {
          // --- set severities
          update_path_severities(event, data);
  
          // --- critical path continues on wait-state causing process

          flag               = 1;
          m_on_critical_path = false;
      }

      buffer->put_uint32(flag);
      data->mInvLocal->add_buffer(buffer, BUFFER_CRITICALPATH);
#endif
    
}

void PatternCriticalPath::pre_recv_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 439 "CriticalPath.pattern"

#ifdef _MPI
      Buffer*  buffer = new Buffer(16);
      uint32_t flag   = 0;

      if (m_on_critical_path && data->mSynchpointHandler->isWaitstate(event)) {
          // --- set severities
          update_path_severities(event, data);
  
          // --- critical path continues on wait-state causing process

          flag               = 1;
          m_on_critical_path = false;
      }

      buffer->put_uint32(flag);
      data->mLocal->add_buffer(buffer, BUFFER_CRITICALPATH);
#endif
    
}

void PatternCriticalPath::sync_coll_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 368 "CriticalPath.pattern"

#ifdef _MPI
        CollectiveInfo& ci(data->mCollinfo);

        int iflag = 0;

        if (m_on_critical_path && data->mSynchpointHandler->isWaitstate(event)) {
          // --- set severities
          update_path_severities(event, data);
          
          iflag              = 1;
          m_on_critical_path = false;
        }

        int oflag = 0;

        MPI_Reduce(&iflag, &oflag, 1, MPI_INT, MPI_MAX, ci.latest.rank, event->getComm()->getComm());
        
        if (oflag) {
          m_on_critical_path = true;
          m_end_event        = event;
#ifdef CPADEBUG
          if (!data->mSynchpointHandler->isSynchpoint(event))
            m_dbg << "SYNC_COLL on critical path is not a registered synchpoint!" << endl;
#endif
        }
#endif
    
}


/*
 *---------------------------------------------------------------------------
 *
 * class PatternCriticalPathActivities
 *
 *---------------------------------------------------------------------------
 */


class PatternCriticalPathActivities : public Pattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("bwc");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(CRITICALPATH_FINISHED, PEARL_create_callback(this, &PatternCriticalPathActivities::criticalpath_finished_bwc_cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_CRITICAL_PATH_ACTIVITIES;
    }

    virtual long get_parent() const
    {
      return PAT_PERFORMANCE_IMPACT_CRITICALPATH;
    }

    virtual string get_name() const
    {
      return "Activity impact";
    }

    virtual string get_unique_name() const
    {
      return "critical_path_activities";
    }

    virtual string get_descr() const
    {
      return "Resource consumption of activities on the critical path";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void criticalpath_finished_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternCriticalPathActivities::criticalpath_finished_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 624 "CriticalPath.pattern"

      for (int i = 0; i < data->mDefs->numCallpaths(); ++i)
        if (data->mGlobalCriticalPath[i] > 0.0)
          m_severity[data->mDefs->get_cnode(i)] += 
            std::min(data->mGlobalCriticalPath[i], data->mLocalTimeProfile[i]);
    
}


/*
 *---------------------------------------------------------------------------
 *
 * class PatternCriticalImbalanceImpact
 *
 *---------------------------------------------------------------------------
 */


class PatternCriticalImbalanceImpact : public Pattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("bwc");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(CRITICALPATH_FINISHED, PEARL_create_callback(this, &PatternCriticalImbalanceImpact::criticalpath_finished_bwc_cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_CRITICAL_IMBALANCE_IMPACT;
    }

    virtual long get_parent() const
    {
      return PAT_PERFORMANCE_IMPACT_CRITICALPATH;
    }

    virtual string get_name() const
    {
      return "Critical-path imbalance impact";
    }

    virtual string get_unique_name() const
    {
      return "critical_imbalance_impact";
    }

    virtual string get_descr() const
    {
      return "Performance impact of load imbalance on the critical path";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void criticalpath_finished_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);


  private:

#line 660 "CriticalPath.pattern"

    static vector<double> phr;
  
};


/*----- Static class data -----*/


#line 663 "CriticalPath.pattern"

    vector<double> PatternCriticalImbalanceImpact::phr;
  


/*----- Callback methods -----*/

void PatternCriticalImbalanceImpact::criticalpath_finished_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 667 "CriticalPath.pattern"

      //
      // --- calculate local headroom

      // shortcuts
      uint32_t ncnodes = data->mDefs->numCallpaths();
      const vector<double>& gcpath(data->mGlobalCriticalPath);
      const vector<double>& lcpath(data->mLocalCriticalPath);
      const vector<double>& ltimes(data->mLocalTimeProfile);      

      double  gcplen   = std::accumulate(gcpath.begin(), gcpath.end(), 0.0);
      double  lplen    = 0.0;

      vector<double> hr(gcpath);

      for (uint32_t i = 0; i < ncnodes; ++i) {
        lplen += ltimes[i];
        hr[i] -= std::min(ltimes[i], hr[i]);
      }

      double  lhrlen = std::accumulate(hr.begin(), hr.end(), 0.0);
      double  hrf    = (gcplen - lplen) / lhrlen;
      
      for (int i = 0; i < ncnodes; ++i)
        hr[i] *= hrf;

      //
      // --- exchange headrooms and calculate severity

#pragma omp master
      {
        phr.assign(ncnodes, 0.0);
      }
#pragma omp barrier
      { }
#pragma omp critical
      {
        std::transform(phr.begin(), phr.end(), hr.begin(), 
                       phr.begin(), std::plus<double>());
      }
#pragma omp barrier
      { }

#ifdef _MPI
#pragma omp master
      {
        MPI_Allreduce(MPI_IN_PLACE, &phr[0], ncnodes, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
      }
#endif

#pragma omp barrier
      { }
      hr.assign(phr.begin(), phr.end());

      for (uint32_t i = 0; i < ncnodes; ++i) {
        if (!(lcpath[i] > 0.0))
          continue;

        double sev = (lcpath[i] / gcpath[i]) * hr[i];

        if (sev > 0.0)
          m_severity[data->mDefs->get_cnode(i)] += sev;
      }
    
}


/*
 *---------------------------------------------------------------------------
 *
 * class PatternInterPartitionImbalance
 *
 *---------------------------------------------------------------------------
 */


class PatternInterPartitionImbalance : public Pattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("bwc");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(CRITICALPATH_FINISHED, PEARL_create_callback(this, &PatternInterPartitionImbalance::criticalpath_finished_bwc_cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_INTER_PARTITION_IMBALANCE;
    }

    virtual long get_parent() const
    {
      return PAT_CRITICAL_IMBALANCE_IMPACT;
    }

    virtual string get_name() const
    {
      return "Inter-partition imbalance";
    }

    virtual string get_unique_name() const
    {
      return "inter_partition_imbalance";
    }

    virtual string get_descr() const
    {
      return "Performance impact of load imbalance between process partitions that perform different activities";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void criticalpath_finished_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);


  private:

#line 778 "CriticalPath.pattern"

    static vector<double> phr;
  
};


/*----- Static class data -----*/


#line 781 "CriticalPath.pattern"

    vector<double> PatternInterPartitionImbalance::phr;
  


/*----- Callback methods -----*/

void PatternInterPartitionImbalance::criticalpath_finished_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 785 "CriticalPath.pattern"

      //
      // --- calculate local headroom

      // shortcuts
      uint32_t ncnodes = data->mDefs->numCallpaths();
      const vector<double>& gcpath(data->mGlobalCriticalPath);
      const vector<double>& lcpath(data->mLocalCriticalPath);
      const vector<double>& ltimes(data->mLocalTimeProfile);      

      double  gcplen   = std::accumulate(gcpath.begin(), gcpath.end(), 0.0);
      double  lplen    = 0.0;

      vector<double> hr(gcpath);

      for (uint32_t i = 0; i < ncnodes; ++i) {
        if (ltimes[i] > 0.0) {
          lplen += ltimes[i] + (hr[i] - std::min(ltimes[i], hr[i]));
          hr[i]  = 0.0;
        }
      }

      double lhrlen = std::accumulate(hr.begin(), hr.end(), 0.0);
      double hrf    = (gcplen - lplen) / lhrlen;
      
      for (int i = 0; i < ncnodes; ++i)
        hr[i] *= hrf;

      //
      // --- exchange headrooms and calculate severity

#pragma omp master
      {
        phr.assign(ncnodes, 0.0);
      }
#pragma omp barrier
      { }
#pragma omp critical
      {
        std::transform(phr.begin(), phr.end(), hr.begin(), 
                       phr.begin(), std::plus<double>());
      }
#pragma omp barrier
      { }

#ifdef _MPI
#pragma omp master
      {
        MPI_Allreduce(MPI_IN_PLACE, &phr[0], ncnodes, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
      }
#endif

#pragma omp barrier
      { }
      hr.assign(phr.begin(), phr.end());

      for (uint32_t i = 0; i < ncnodes; ++i) {
        if (!(lcpath[i] > 0.0))
          continue;

        double sev = (lcpath[i] / gcpath[i]) * hr[i];

        if (sev > 0.0)
          m_severity[data->mDefs->get_cnode(i)] += sev;
      }
    
}


/*
 *---------------------------------------------------------------------------
 *
 * class PatternNonCriticalPathActivities
 *
 *---------------------------------------------------------------------------
 */


class PatternNonCriticalPathActivities : public Pattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("bwc");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(CRITICALPATH_FINISHED, PEARL_create_callback(this, &PatternNonCriticalPathActivities::criticalpath_finished_bwc_cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_NON_CRITICAL_PATH_ACTIVITIES;
    }

    virtual long get_parent() const
    {
      return PAT_PERFORMANCE_IMPACT;
    }

    virtual string get_name() const
    {
      return "Non-critical-path activities";
    }

    virtual string get_unique_name() const
    {
      return "non_critical_path_activities";
    }

    virtual string get_descr() const
    {
      return "Overall resource consumption of activities that are not on the critical path";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void criticalpath_finished_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternNonCriticalPathActivities::criticalpath_finished_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 869 "CriticalPath.pattern"

      for (uint32_t i = 0; i < data->mDefs->numCallpaths(); ++i)
        m_severity[data->mDefs->get_cnode(i)] += 
          std::max(data->mLocalTimeProfile[i] - data->mGlobalCriticalPath[i], 0.0);
    
}


#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternDelay_LateSender
 *
 *---------------------------------------------------------------------------
 */


class PatternDelay_LateSender : public Pattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("bwc");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(PREPARE, PEARL_create_callback(this, &PatternDelay_LateSender::prepare_bwc_cb));
        (it->second)->register_callback(PRE_RECV, PEARL_create_callback(this, &PatternDelay_LateSender::pre_recv_bwc_cb));
        (it->second)->register_callback(POST_SEND, PEARL_create_callback(this, &PatternDelay_LateSender::post_send_bwc_cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_DELAY_LATESENDER;
    }

    virtual long get_parent() const
    {
      return PAT_DELAY_LATESENDER_AGGREGATE;
    }

    virtual string get_name() const
    {
      return "MPI short-term Late Sender delay costs";
    }

    virtual string get_unique_name() const
    {
      return "delay_latesender";
    }

    virtual string get_descr() const
    {
      return "Delays that directly cause MPI Late Sender wait states";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void post_send_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void prepare_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_recv_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);


  private:

#line 183 "Delay.MPI.pattern"

#ifdef DELAYDEBUG
    std::ofstream m_dbg;
#endif
  
};


/*----- Callback methods -----*/

void PatternDelay_LateSender::post_send_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 224 "Delay.MPI.pattern"

       // Shortcuts
       const TmapCacheHandler*  TmH = data->mTmapCacheHandler;
       const SynchpointHandler* SpH = data->mSynchpointHandler;

       // is this a synchpoint?
       if (!SpH->isSynchpoint(event))
         return;

       // --- get delay info buffer

       Buffer* buffer = data->mRemote->get_buffer(BUFFER_DELAY);
       data->mRwait   = buffer->get_double();

       // was there a late-sender wait state?
       if (!(data->mRwait > 0))
         return;

       // --- find previous synchronization point

       int rank      = event->getComm()->getGroup().getGlobalRank(event->getDestination());
       data->mPrevSp = SpH->findPreviousMpiSynchpoint(event, rank);

       // --- get local and remote time maps and calculate delay

       data->mPropSpMap = SpH->getSynchpointsBetween(data->mPrevSp, event);
       data->mWaitInfo  = CalculateWaitMap(data->mPropSpMap);
       data->mDelayInfo = 
         CalculateDiff(PruneDelayTimemap(TmH->getRuntimes(data->mPrevSp, event), 
                                         data->mWaitInfo.mMap,
                                         data->mPrevSp,
                                         SpH->getSynchpointInfo(data->mPrevSp)),
                       UnpackTimemap(buffer));
                                                          
       // shortcuts
       const TimeMapSum& d(data->mDelayInfo);
       const TimeMapSum& w(data->mWaitInfo);

       // --- calculate delay costs

       double scale = 0.0;

       if (d.mSum + w.mSum > 0)
         scale = std::min(data->mRwait / (d.mSum + w.mSum), 1.0);

#ifdef DELAYDEBUG
       m_dbg << "\nLate-sender delay rwait = " << data->mRwait 
             << " delay sum = " << d.mSum << " wait sum = " << w.mSum 
             << "\nEvent" << event << "\nPrevSp" << data->mPrevSp
             << "Delay map\n";
       for (timemap_t::const_iterator it = d.mMap.begin(); it != d.mMap.end(); ++it)
         m_dbg << " (" << data->mDefs->getCallpath(it->first).getRegion().getDisplayName().getString()
               << ", " << it->second << ")\n";
#endif
       // --- set severities

       if (d.mSum > 0)
         for (timemap_t::const_iterator it = d.mMap.begin(); it != d.mMap.end(); ++it)
           m_severity[data->mDefs->get_cnode(it->first)] += scale * it->second;

       // --- invoke subsequent handlers

       cbmanager.notify(DELAY_LATE_SENDER, event, data);
     
}

void PatternDelay_LateSender::prepare_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 189 "Delay.MPI.pattern"

#ifdef DELAYDEBUG
       int rank = 0, thread = 0;
#ifdef _MPI
       MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif
       std::ostringstream filename;
       filename << "delaydbg_r" << rank << "t" << thread << ".out";
       m_dbg.open(filename.str().c_str());
#endif // DELAYDEBUG
     
}

void PatternDelay_LateSender::pre_recv_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 201 "Delay.MPI.pattern"

       // Shortcuts
       const TmapCacheHandler*  TmH = data->mTmapCacheHandler;
       const SynchpointHandler* SpH = data->mSynchpointHandler;

       // cost-accounting backward replay

       Buffer* buffer = new Buffer(256);
       SynchpointInfo spi = SpH->getSynchpointInfo(event);

       buffer->put_double(spi.waitTime);

       timemap_t tmap;

       if (spi.waitTime > 0) {
         int rank = event->getComm()->getGroup().getGlobalRank(event->getSource());
         tmap = GetMpiDelayTimemap(event, rank, SpH, TmH);
       }

       PackTimemap(buffer, tmap);
       data->mLocal->add_buffer(buffer, BUFFER_DELAY);
     
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternDelay_LateSender_LT
 *
 *---------------------------------------------------------------------------
 */


class PatternDelay_LateSender_LT : public Pattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("bwc");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(PRE_RECV, PEARL_create_callback(this, &PatternDelay_LateSender_LT::pre_recv_bwc_cb));
        (it->second)->register_callback(DELAY_LATE_SENDER, PEARL_create_callback(this, &PatternDelay_LateSender_LT::delay_late_sender_bwc_cb));
        (it->second)->register_callback(PRE_INV_SENDCMP, PEARL_create_callback(this, &PatternDelay_LateSender_LT::pre_inv_sendcmp_bwc_cb));
        (it->second)->register_callback(DELAY_LATE_RECEIVER, PEARL_create_callback(this, &PatternDelay_LateSender_LT::delay_late_receiver_bwc_cb));
        (it->second)->register_callback(PRE_DELAY_BARRIER, PEARL_create_callback(this, &PatternDelay_LateSender_LT::pre_delay_barrier_bwc_cb));
        (it->second)->register_callback(DELAY_BARRIER, PEARL_create_callback(this, &PatternDelay_LateSender_LT::delay_barrier_bwc_cb));
        (it->second)->register_callback(PRE_DELAY_N2N, PEARL_create_callback(this, &PatternDelay_LateSender_LT::pre_delay_n2n_bwc_cb));
        (it->second)->register_callback(DELAY_N2N, PEARL_create_callback(this, &PatternDelay_LateSender_LT::delay_n2n_bwc_cb));
        (it->second)->register_callback(PRE_DELAY_12N, PEARL_create_callback(this, &PatternDelay_LateSender_LT::pre_delay_12n_bwc_cb));
        (it->second)->register_callback(DELAY_12N, PEARL_create_callback(this, &PatternDelay_LateSender_LT::delay_12n_bwc_cb));
        (it->second)->register_callback(PRE_DELAY_OMP_BARRIER, PEARL_create_callback(this, &PatternDelay_LateSender_LT::pre_delay_omp_barrier_bwc_cb));
        (it->second)->register_callback(DELAY_OMP_BARRIER, PEARL_create_callback(this, &PatternDelay_LateSender_LT::delay_omp_barrier_bwc_cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_DELAY_LATESENDER_LONGTERM;
    }

    virtual long get_parent() const
    {
      return PAT_DELAY_LATESENDER_AGGREGATE;
    }

    virtual string get_name() const
    {
      return "MPI long-term Late Sender delay costs";
    }

    virtual string get_unique_name() const
    {
      return "delay_latesender_longterm";
    }

    virtual string get_descr() const
    {
      return "Delays that indirectly cause MPI Late Sender wait states through wait-state propagation";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void delay_12n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_late_receiver_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_late_sender_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_n2n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_omp_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_delay_12n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_delay_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_delay_n2n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_delay_omp_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_inv_sendcmp_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_recv_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);


  private:

#line 322 "Delay.MPI.pattern"

    DelayCostMap m_cost;
  
};


/*----- Callback methods -----*/

void PatternDelay_LateSender_LT::delay_12n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 432 "Delay.MPI.pattern"

       // Set severities 
       map<uint32_t, TimeMapSum>::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_LS);

       if (dCit != data->mCollDelayInfo.end() && dCit->second.mSum > 0.0)
         for (timemap_t::const_iterator it = dCit->second.mMap.begin(); it != dCit->second.mMap.end(); ++it)
           m_severity[data->mDefs->get_cnode(it->first)] += it->second;

       // Update long-term costs for propagating wait states in synchronization interval
       map<uint32_t, ScaleMap>::const_iterator sCit = data->mSumScales.find(KEY_RCOST_LS);

       if (sCit != data->mSumScales.end())
         m_cost.addCosts(data->mPropSpMap, sCit->second);
    
}

void PatternDelay_LateSender_LT::delay_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 394 "Delay.MPI.pattern"

       // Set severities 
       map<uint32_t, TimeMapSum>::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_LS);

       if (dCit != data->mCollDelayInfo.end() && dCit->second.mSum > 0.0)
         for (timemap_t::const_iterator it = dCit->second.mMap.begin(); it != dCit->second.mMap.end(); ++it)
           m_severity[data->mDefs->get_cnode(it->first)] += it->second;

       // Update long-term costs for propagating wait states in synchronization interval
       map<uint32_t, ScaleMap>::const_iterator sCit = data->mSumScales.find(KEY_RCOST_LS);

       if (sCit != data->mSumScales.end())
         m_cost.addCosts(data->mPropSpMap, sCit->second);
    
}

void PatternDelay_LateSender_LT::delay_late_receiver_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 366 "Delay.MPI.pattern"

       // Shortcuts
       const TimeMapSum& d(data->mDelayInfo);
       const TimeMapSum& w(data->mWaitInfo);

       if (!(d.mSum + w.mSum > 0))
         return;

       // --- get remote cost & set severities

       double rcost = data->mInvRemote->get_buffer(BUFFER_RCOST_LS)->get_double();
       double scale = rcost / (d.mSum + w.mSum);

       if (!(scale > 0))
         return;

       for (timemap_t::const_iterator it = d.mMap.begin(); it != d.mMap.end(); ++it)
         m_severity[data->mDefs->get_cnode(it->first)] += scale * it->second;

       // --- set propagating long-term costs

       m_cost.addCosts(data->mPropSpMap, scale);
    
}

void PatternDelay_LateSender_LT::delay_late_sender_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 333 "Delay.MPI.pattern"

       // Shortcuts
       const TimeMapSum& d(data->mDelayInfo);
       const TimeMapSum& w(data->mWaitInfo);
       
       if (!(d.mSum + w.mSum > 0))
         return;

       // --- get remote cost

       double rcost = data->mRemote->get_buffer(BUFFER_RCOST_LS)->get_double();

       // --- set severities

       double scale_w = std::min(data->mRwait / (d.mSum + w.mSum), 1.0);
       double scale_c = rcost / (d.mSum + w.mSum);

       if (scale_c > 0)
         for (timemap_t::const_iterator it = d.mMap.begin(); it != d.mMap.end(); ++it)
           m_severity[data->mDefs->get_cnode(it->first)] += scale_c * it->second;

       // --- set propagating long-term costs

       m_cost.addCosts(data->mPropSpMap, scale_w + scale_c);
    
}

void PatternDelay_LateSender_LT::delay_n2n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 413 "Delay.MPI.pattern"

       // Set severities 
       map<uint32_t, TimeMapSum>::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_LS);

       if (dCit != data->mCollDelayInfo.end() && dCit->second.mSum > 0.0)
         for (timemap_t::const_iterator it = dCit->second.mMap.begin(); it != dCit->second.mMap.end(); ++it)
           m_severity[data->mDefs->get_cnode(it->first)] += it->second;

       // Update long-term costs for propagating wait states in synchronization interval
       map<uint32_t, ScaleMap>::const_iterator sCit = data->mSumScales.find(KEY_RCOST_LS);

       if (sCit != data->mSumScales.end())
         m_cost.addCosts(data->mPropSpMap, sCit->second);
    
}

void PatternDelay_LateSender_LT::delay_omp_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 451 "Delay.MPI.pattern"

       // Set severities 
       map<uint32_t, TimeMapSum>::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_LS);

       if (dCit != data->mCollDelayInfo.end() && dCit->second.mSum > 0.0)
         for (timemap_t::const_iterator it = dCit->second.mMap.begin(); it != dCit->second.mMap.end(); ++it)
           m_severity[data->mDefs->get_cnode(it->first)] += it->second;

       // Update long-term costs for propagating wait states in synchronization interval
       map<uint32_t, ScaleMap>::const_iterator sCit = data->mSumScales.find(KEY_RCOST_LS);

       if (sCit != data->mSumScales.end())
         m_cost.addCosts(data->mPropSpMap, sCit->second);
    
}

void PatternDelay_LateSender_LT::pre_delay_12n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 428 "Delay.MPI.pattern"

       data->mInputScales.insert(make_pair(KEY_RCOST_LS, m_cost.getCosts(event)));
    
}

void PatternDelay_LateSender_LT::pre_delay_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 390 "Delay.MPI.pattern"

       data->mInputScales.insert(make_pair(KEY_RCOST_LS, m_cost.getCosts(event)));
    
}

void PatternDelay_LateSender_LT::pre_delay_n2n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 409 "Delay.MPI.pattern"

       data->mInputScales.insert(make_pair(KEY_RCOST_LS, m_cost.getCosts(event)));
    
}

void PatternDelay_LateSender_LT::pre_delay_omp_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 447 "Delay.MPI.pattern"

       data->mInputScales.insert(make_pair(KEY_RCOST_LS, m_cost.getCosts(event)));
    
}

void PatternDelay_LateSender_LT::pre_inv_sendcmp_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 359 "Delay.MPI.pattern"

       Buffer* buffer = new Buffer(16);

       buffer->put_double(m_cost.getCosts(event));
       data->mInvLocal->add_buffer(buffer, BUFFER_RCOST_LS);
    
}

void PatternDelay_LateSender_LT::pre_recv_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 326 "Delay.MPI.pattern"

       Buffer* buffer = new Buffer(16);

       buffer->put_double(m_cost.getCosts(event));
       data->mLocal->add_buffer(buffer, BUFFER_RCOST_LS);
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternDelay_LateReceiver
 *
 *---------------------------------------------------------------------------
 */


class PatternDelay_LateReceiver : public Pattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("bwc");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(PRE_INV_SENDCMP, PEARL_create_callback(this, &PatternDelay_LateReceiver::pre_inv_sendcmp_bwc_cb));
        (it->second)->register_callback(POST_INV_RECVREQ, PEARL_create_callback(this, &PatternDelay_LateReceiver::post_inv_recvreq_bwc_cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_DELAY_LATERECEIVER;
    }

    virtual long get_parent() const
    {
      return PAT_DELAY_LATERECEIVER_AGGREGATE;
    }

    virtual string get_name() const
    {
      return "MPI short-term Late Receiver delay costs";
    }

    virtual string get_unique_name() const
    {
      return "delay_latereceiver";
    }

    virtual string get_descr() const
    {
      return "Delays that directly cause MPI Late Receiver wait states";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void post_inv_recvreq_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_inv_sendcmp_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternDelay_LateReceiver::post_inv_recvreq_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 614 "Delay.MPI.pattern"

       // Shortcuts
       const TmapCacheHandler*  TmH = data->mTmapCacheHandler;
       const SynchpointHandler* SpH = data->mSynchpointHandler;

       // is this a synchpoint?
       if (!SpH->isSynchpoint(event))
         return;

       // --- get delay info buffer

       Buffer* buffer = data->mInvRemote->get_buffer(BUFFER_DELAY);
       data->mRwait   = buffer->get_double();

       // was there a late-receiver wait state?
       if (!(data->mRwait > 0))
         return;

       // --- find previous synchronization point

       Event recvcmp = event.completion();
       int rank      = recvcmp->getComm()->getGroup().getGlobalRank(recvcmp->getSource());
       data->mPrevSp = SpH->findPreviousMpiSynchpoint(event, rank);

       // --- get local and remote time maps and calculate delay

       data->mPropSpMap = SpH->getSynchpointsBetween(data->mPrevSp, event);
       data->mWaitInfo  = CalculateWaitMap(data->mPropSpMap);
       data->mDelayInfo = 
         CalculateDiff(PruneDelayTimemap(TmH->getRuntimes(data->mPrevSp, event), 
                                         data->mWaitInfo.mMap,
                                         data->mPrevSp,
                                         SpH->getSynchpointInfo(data->mPrevSp)),
                       UnpackTimemap(buffer));

       // shortcuts
       const TimeMapSum& d(data->mDelayInfo);
       const TimeMapSum& w(data->mWaitInfo);

       // --- calculate delay costs

       double scale = 0.0;

       if (d.mSum + w.mSum > 0)
         scale = std::min(data->mRwait / (d.mSum + w.mSum), 1.0);

       // --- set severities

       if (d.mSum > 0)
         for (timemap_t::const_iterator it = d.mMap.begin(); it != d.mMap.end(); ++it)
           m_severity[data->mDefs->get_cnode(it->first)] += scale * it->second;

       // --- invoke subsequent handlers

       cbmanager.notify(DELAY_LATE_RECEIVER, event, data);
     
}

void PatternDelay_LateReceiver::pre_inv_sendcmp_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 590 "Delay.MPI.pattern"

       // Shortcuts
       const TmapCacheHandler*  TmH = data->mTmapCacheHandler;
       const SynchpointHandler* SpH = data->mSynchpointHandler;

       // cost-accounting backward replay

       Buffer* buffer = new Buffer(256);
       SynchpointInfo spi = SpH->getSynchpointInfo(event);

       buffer->put_double(spi.waitTime);

       timemap_t tmap;

       if (spi.waitTime > 0) {
         Event sendreq = event.request();
         int rank = sendreq->getComm()->getGroup().getGlobalRank(sendreq->getDestination());
         tmap = GetMpiDelayTimemap(event, rank, SpH, TmH);
       }

       PackTimemap(buffer, tmap);
       data->mInvLocal->add_buffer(buffer, BUFFER_DELAY);
     
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternDelay_LateReceiver_LT
 *
 *---------------------------------------------------------------------------
 */


class PatternDelay_LateReceiver_LT : public Pattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("bwc");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(PRE_RECV, PEARL_create_callback(this, &PatternDelay_LateReceiver_LT::pre_recv_bwc_cb));
        (it->second)->register_callback(DELAY_LATE_SENDER, PEARL_create_callback(this, &PatternDelay_LateReceiver_LT::delay_late_sender_bwc_cb));
        (it->second)->register_callback(PRE_INV_SENDCMP, PEARL_create_callback(this, &PatternDelay_LateReceiver_LT::pre_inv_sendcmp_bwc_cb));
        (it->second)->register_callback(DELAY_LATE_RECEIVER, PEARL_create_callback(this, &PatternDelay_LateReceiver_LT::delay_late_receiver_bwc_cb));
        (it->second)->register_callback(PRE_DELAY_BARRIER, PEARL_create_callback(this, &PatternDelay_LateReceiver_LT::pre_delay_barrier_bwc_cb));
        (it->second)->register_callback(DELAY_BARRIER, PEARL_create_callback(this, &PatternDelay_LateReceiver_LT::delay_barrier_bwc_cb));
        (it->second)->register_callback(PRE_DELAY_N2N, PEARL_create_callback(this, &PatternDelay_LateReceiver_LT::pre_delay_n2n_bwc_cb));
        (it->second)->register_callback(DELAY_N2N, PEARL_create_callback(this, &PatternDelay_LateReceiver_LT::delay_n2n_bwc_cb));
        (it->second)->register_callback(PRE_DELAY_12N, PEARL_create_callback(this, &PatternDelay_LateReceiver_LT::pre_delay_12n_bwc_cb));
        (it->second)->register_callback(DELAY_12N, PEARL_create_callback(this, &PatternDelay_LateReceiver_LT::delay_12n_bwc_cb));
        (it->second)->register_callback(PRE_DELAY_OMP_BARRIER, PEARL_create_callback(this, &PatternDelay_LateReceiver_LT::pre_delay_omp_barrier_bwc_cb));
        (it->second)->register_callback(DELAY_OMP_BARRIER, PEARL_create_callback(this, &PatternDelay_LateReceiver_LT::delay_omp_barrier_bwc_cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_DELAY_LATERECEIVER_LONGTERM;
    }

    virtual long get_parent() const
    {
      return PAT_DELAY_LATERECEIVER_AGGREGATE;
    }

    virtual string get_name() const
    {
      return "MPI long-term Late Receiver delay costs";
    }

    virtual string get_unique_name() const
    {
      return "delay_latereceiver_longterm";
    }

    virtual string get_descr() const
    {
      return "Delays that indirectly cause MPI Late Receiver wait states through wait-state propagation";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void delay_12n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_late_receiver_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_late_sender_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_n2n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_omp_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_delay_12n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_delay_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_delay_n2n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_delay_omp_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_inv_sendcmp_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_recv_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);


  private:

#line 704 "Delay.MPI.pattern"

    DelayCostMap m_cost;
  
};


/*----- Callback methods -----*/

void PatternDelay_LateReceiver_LT::delay_12n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 814 "Delay.MPI.pattern"

       // Set severities 
       map<uint32_t, TimeMapSum>::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_LR);

       if (dCit != data->mCollDelayInfo.end() && dCit->second.mSum > 0.0)
         for (timemap_t::const_iterator it = dCit->second.mMap.begin(); it != dCit->second.mMap.end(); ++it)
           m_severity[data->mDefs->get_cnode(it->first)] += it->second;

       // Update long-term costs for propagating wait states in synchronization interval
       map<uint32_t, ScaleMap>::const_iterator sCit = data->mSumScales.find(KEY_RCOST_LR);

       if (sCit != data->mSumScales.end())
         m_cost.addCosts(data->mPropSpMap, sCit->second);
    
}

void PatternDelay_LateReceiver_LT::delay_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 776 "Delay.MPI.pattern"

       // Set severities 
       map<uint32_t, TimeMapSum>::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_LR);

       if (dCit != data->mCollDelayInfo.end() && dCit->second.mSum > 0.0)
         for (timemap_t::const_iterator it = dCit->second.mMap.begin(); it != dCit->second.mMap.end(); ++it)
           m_severity[data->mDefs->get_cnode(it->first)] += it->second;

       // Update long-term costs for propagating wait states in synchronization interval
       map<uint32_t, ScaleMap>::const_iterator sCit = data->mSumScales.find(KEY_RCOST_LR);

       if (sCit != data->mSumScales.end())
         m_cost.addCosts(data->mPropSpMap, sCit->second);
    
}

void PatternDelay_LateReceiver_LT::delay_late_receiver_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 746 "Delay.MPI.pattern"

       // Shortcuts
       const TimeMapSum& d(data->mDelayInfo);
       const TimeMapSum& w(data->mWaitInfo);
     
       if (!(d.mSum + w.mSum > 0))
         return;

       // --- get remote cost

       double rcost = data->mInvRemote->get_buffer(BUFFER_RCOST_LR)->get_double();

       // --- set severities

       double scale_w = std::min(data->mRwait / (d.mSum + w.mSum), 1.0);
       double scale_c = rcost / (d.mSum + w.mSum);

       if (scale_c > 0)
         for (timemap_t::const_iterator it = d.mMap.begin(); it != d.mMap.end(); ++it)
           m_severity[data->mDefs->get_cnode(it->first)] += scale_c * it->second;

       // --- set propagating long-term costs

       m_cost.addCosts(data->mPropSpMap, scale_w + scale_c);
    
}

void PatternDelay_LateReceiver_LT::delay_late_sender_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 715 "Delay.MPI.pattern"

       // Shortcuts
       const TimeMapSum& d(data->mDelayInfo);
       const TimeMapSum& w(data->mWaitInfo);
     
       if (!(d.mSum + w.mSum > 0))
         return;

       // --- get remote cost & set severities

       double rcost = data->mRemote->get_buffer(BUFFER_RCOST_LR)->get_double();
       double scale = rcost / (d.mSum + w.mSum);

       if (!(scale > 0))
         return;

       for (timemap_t::const_iterator it = d.mMap.begin(); it != d.mMap.end(); ++it)
         m_severity[data->mDefs->get_cnode(it->first)] += scale * it->second;

       // --- set propagating long-term costs

       m_cost.addCosts(data->mPropSpMap, scale);
    
}

void PatternDelay_LateReceiver_LT::delay_n2n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 795 "Delay.MPI.pattern"

       // Set severities 
       map<uint32_t, TimeMapSum>::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_LR);

       if (dCit != data->mCollDelayInfo.end() && dCit->second.mSum > 0.0)
         for (timemap_t::const_iterator it = dCit->second.mMap.begin(); it != dCit->second.mMap.end(); ++it)
           m_severity[data->mDefs->get_cnode(it->first)] += it->second;

       // Update long-term costs for propagating wait states in synchronization interval
       map<uint32_t, ScaleMap>::const_iterator sCit = data->mSumScales.find(KEY_RCOST_LR);

       if (sCit != data->mSumScales.end())
         m_cost.addCosts(data->mPropSpMap, sCit->second);
    
}

void PatternDelay_LateReceiver_LT::delay_omp_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 833 "Delay.MPI.pattern"

       // Set severities 
       map<uint32_t, TimeMapSum>::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_LR);

       if (dCit != data->mCollDelayInfo.end() && dCit->second.mSum > 0.0)
         for (timemap_t::const_iterator it = dCit->second.mMap.begin(); it != dCit->second.mMap.end(); ++it)
           m_severity[data->mDefs->get_cnode(it->first)] += it->second;

       // Update long-term costs for propagating wait states in synchronization interval
       map<uint32_t, ScaleMap>::const_iterator sCit = data->mSumScales.find(KEY_RCOST_LR);

       if (sCit != data->mSumScales.end())
         m_cost.addCosts(data->mPropSpMap, sCit->second);
    
}

void PatternDelay_LateReceiver_LT::pre_delay_12n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 810 "Delay.MPI.pattern"

       data->mInputScales.insert(make_pair(KEY_RCOST_LR, m_cost.getCosts(event)));
    
}

void PatternDelay_LateReceiver_LT::pre_delay_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 772 "Delay.MPI.pattern"

       data->mInputScales.insert(make_pair(KEY_RCOST_LR, m_cost.getCosts(event)));
    
}

void PatternDelay_LateReceiver_LT::pre_delay_n2n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 791 "Delay.MPI.pattern"

       data->mInputScales.insert(make_pair(KEY_RCOST_LR, m_cost.getCosts(event)));
    
}

void PatternDelay_LateReceiver_LT::pre_delay_omp_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 829 "Delay.MPI.pattern"

       data->mInputScales.insert(make_pair(KEY_RCOST_LR, m_cost.getCosts(event)));
    
}

void PatternDelay_LateReceiver_LT::pre_inv_sendcmp_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 739 "Delay.MPI.pattern"

       Buffer* buffer = new Buffer(16);

       buffer->put_double(m_cost.getCosts(event));
       data->mInvLocal->add_buffer(buffer, BUFFER_RCOST_LR);
    
}

void PatternDelay_LateReceiver_LT::pre_recv_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 708 "Delay.MPI.pattern"

       Buffer* buffer = new Buffer(16);

       buffer->put_double(m_cost.getCosts(event));
       data->mLocal->add_buffer(buffer, BUFFER_RCOST_LR);
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternDelay_Barrier
 *
 *---------------------------------------------------------------------------
 */


class PatternDelay_Barrier : public Pattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("bwc");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(SYNC_COLL, PEARL_create_callback(this, &PatternDelay_Barrier::sync_coll_bwc_cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_DELAY_BARRIER;
    }

    virtual long get_parent() const
    {
      return PAT_DELAY_BARRIER_AGGREGATE;
    }

    virtual string get_name() const
    {
      return "MPI short-term barrier delay costs";
    }

    virtual string get_unique_name() const
    {
      return "delay_barrier";
    }

    virtual string get_descr() const
    {
      return "Delays that directly cause wait states in MPI barriers";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void sync_coll_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternDelay_Barrier::sync_coll_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 986 "Delay.MPI.pattern"

      const CollectiveInfo& ci(data->mCollinfo);

      data->mInputScales.clear();

      cbmanager.notify(PRE_DELAY_BARRIER, event, data);

      CalculateCollectiveDelay(ci.my, ci.latest, event, data);

      if (ci.my.rank == ci.latest.rank) {
        // Set severities 
        map<uint32_t, TimeMapSum>::const_iterator dWit = data->mCollDelayInfo.find(KEY_WAITTIME);

        if (dWit != data->mCollDelayInfo.end() && dWit->second.mSum > 0.0)
          for (timemap_t::const_iterator it = dWit->second.mMap.begin(); it != dWit->second.mMap.end(); ++it)
            m_severity[data->mDefs->get_cnode(it->first)] += it->second;

        cbmanager.notify(DELAY_BARRIER, event, data);
      }
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternDelay_Barrier_LT
 *
 *---------------------------------------------------------------------------
 */


class PatternDelay_Barrier_LT : public Pattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("bwc");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(PRE_RECV, PEARL_create_callback(this, &PatternDelay_Barrier_LT::pre_recv_bwc_cb));
        (it->second)->register_callback(DELAY_LATE_SENDER, PEARL_create_callback(this, &PatternDelay_Barrier_LT::delay_late_sender_bwc_cb));
        (it->second)->register_callback(PRE_INV_SENDCMP, PEARL_create_callback(this, &PatternDelay_Barrier_LT::pre_inv_sendcmp_bwc_cb));
        (it->second)->register_callback(DELAY_LATE_RECEIVER, PEARL_create_callback(this, &PatternDelay_Barrier_LT::delay_late_receiver_bwc_cb));
        (it->second)->register_callback(PRE_DELAY_BARRIER, PEARL_create_callback(this, &PatternDelay_Barrier_LT::pre_delay_barrier_bwc_cb));
        (it->second)->register_callback(DELAY_BARRIER, PEARL_create_callback(this, &PatternDelay_Barrier_LT::delay_barrier_bwc_cb));
        (it->second)->register_callback(PRE_DELAY_N2N, PEARL_create_callback(this, &PatternDelay_Barrier_LT::pre_delay_n2n_bwc_cb));
        (it->second)->register_callback(DELAY_N2N, PEARL_create_callback(this, &PatternDelay_Barrier_LT::delay_n2n_bwc_cb));
        (it->second)->register_callback(PRE_DELAY_12N, PEARL_create_callback(this, &PatternDelay_Barrier_LT::pre_delay_12n_bwc_cb));
        (it->second)->register_callback(DELAY_12N, PEARL_create_callback(this, &PatternDelay_Barrier_LT::delay_12n_bwc_cb));
        (it->second)->register_callback(PRE_DELAY_OMP_BARRIER, PEARL_create_callback(this, &PatternDelay_Barrier_LT::pre_delay_omp_barrier_bwc_cb));
        (it->second)->register_callback(DELAY_OMP_BARRIER, PEARL_create_callback(this, &PatternDelay_Barrier_LT::delay_omp_barrier_bwc_cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_DELAY_BARRIER_LONGTERM;
    }

    virtual long get_parent() const
    {
      return PAT_DELAY_BARRIER_AGGREGATE;
    }

    virtual string get_name() const
    {
      return "MPI long-term barrier delay costs";
    }

    virtual string get_unique_name() const
    {
      return "delay_barrier_longterm";
    }

    virtual string get_descr() const
    {
      return "Delays that indirectly cause wait states in MPI barriers through wait-state propagation";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void delay_12n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_late_receiver_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_late_sender_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_n2n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_omp_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_delay_12n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_delay_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_delay_n2n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_delay_omp_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_inv_sendcmp_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_recv_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);


  private:

#line 1033 "Delay.MPI.pattern"

    DelayCostMap m_cost;
  
};


/*----- Callback methods -----*/

void PatternDelay_Barrier_LT::delay_12n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1146 "Delay.MPI.pattern"

       // Set severities 
       map<uint32_t, TimeMapSum>::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_BARRIER);

       if (dCit != data->mCollDelayInfo.end() && dCit->second.mSum > 0.0)
         for (timemap_t::const_iterator it = dCit->second.mMap.begin(); it != dCit->second.mMap.end(); ++it)
           m_severity[data->mDefs->get_cnode(it->first)] += it->second;

       // Update long-term costs for propagating wait states in synchronization interval
       map<uint32_t, ScaleMap>::const_iterator sCit = data->mSumScales.find(KEY_RCOST_BARRIER);

       if (sCit != data->mSumScales.end())
         m_cost.addCosts(data->mPropSpMap, sCit->second);
    
}

void PatternDelay_Barrier_LT::delay_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1103 "Delay.MPI.pattern"

       // Set severities 
       map<uint32_t, TimeMapSum>::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_BARRIER);

       if (dCit != data->mCollDelayInfo.end() && dCit->second.mSum > 0.0)
         for (timemap_t::const_iterator it = dCit->second.mMap.begin(); it != dCit->second.mMap.end(); ++it)
           m_severity[data->mDefs->get_cnode(it->first)] += it->second;

       // Update long-term costs for propagating wait states in synchronization interval
       map<uint32_t, ScaleMap>::const_iterator sCit = data->mSumScales.find(KEY_RCOST_BARRIER);

       if (sCit != data->mSumScales.end())
         m_cost.addCosts(data->mPropSpMap, sCit->second);

       map<uint32_t, ScaleMap>::const_iterator sWit = data->mSumScales.find(KEY_WAITTIME);

       if (sWit != data->mSumScales.end())
         m_cost.addCosts(data->mPropSpMap, sWit->second);
    
}

void PatternDelay_Barrier_LT::delay_late_receiver_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1075 "Delay.MPI.pattern"

       // Shortcuts
       const TimeMapSum& d(data->mDelayInfo);
       const TimeMapSum& w(data->mWaitInfo);

       if (!(d.mSum + w.mSum > 0))
         return;

       // --- get remote cost & set severities

       double rcost = data->mInvRemote->get_buffer(BUFFER_RCOST_BARRIER)->get_double();
       double scale = rcost / (d.mSum + w.mSum);

       if (!(scale > 0))
         return;

       for (timemap_t::const_iterator it = d.mMap.begin(); it != d.mMap.end(); ++it)
         m_severity[data->mDefs->get_cnode(it->first)] += scale * it->second;

       // --- set propagating long-term costs

       m_cost.addCosts(data->mPropSpMap, scale);
    
}

void PatternDelay_Barrier_LT::delay_late_sender_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1044 "Delay.MPI.pattern"

       // Shortcuts
       const TimeMapSum& d(data->mDelayInfo);
       const TimeMapSum& w(data->mWaitInfo);
     
       if (!(d.mSum + w.mSum > 0))
         return;

       // --- get remote cost & set severities

       double rcost = data->mRemote->get_buffer(BUFFER_RCOST_BARRIER)->get_double();
       double scale = rcost / (d.mSum + w.mSum);

       if (!(scale > 0))
         return;

       for (timemap_t::const_iterator it = d.mMap.begin(); it != d.mMap.end(); ++it)
         m_severity[data->mDefs->get_cnode(it->first)] += scale * it->second;

       // --- set propagating long-term costs

       m_cost.addCosts(data->mPropSpMap, scale);
    
}

void PatternDelay_Barrier_LT::delay_n2n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1127 "Delay.MPI.pattern"

       // Set severities 
       map<uint32_t, TimeMapSum>::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_BARRIER);

       if (dCit != data->mCollDelayInfo.end() && dCit->second.mSum > 0.0)
         for (timemap_t::const_iterator it = dCit->second.mMap.begin(); it != dCit->second.mMap.end(); ++it)
           m_severity[data->mDefs->get_cnode(it->first)] += it->second;

       // Update long-term costs for propagating wait states in synchronization interval
       map<uint32_t, ScaleMap>::const_iterator sCit = data->mSumScales.find(KEY_RCOST_BARRIER);

       if (sCit != data->mSumScales.end())
         m_cost.addCosts(data->mPropSpMap, sCit->second);
    
}

void PatternDelay_Barrier_LT::delay_omp_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1165 "Delay.MPI.pattern"

       // Set severities 
       map<uint32_t, TimeMapSum>::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_BARRIER);

       if (dCit != data->mCollDelayInfo.end() && dCit->second.mSum > 0.0)
         for (timemap_t::const_iterator it = dCit->second.mMap.begin(); it != dCit->second.mMap.end(); ++it)
           m_severity[data->mDefs->get_cnode(it->first)] += it->second;

       // Update long-term costs for propagating wait states in synchronization interval
       map<uint32_t, ScaleMap>::const_iterator sCit = data->mSumScales.find(KEY_RCOST_BARRIER);

       if (sCit != data->mSumScales.end())
         m_cost.addCosts(data->mPropSpMap, sCit->second);
    
}

void PatternDelay_Barrier_LT::pre_delay_12n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1142 "Delay.MPI.pattern"

       data->mInputScales.insert(make_pair(KEY_RCOST_BARRIER, m_cost.getCosts(event)));
    
}

void PatternDelay_Barrier_LT::pre_delay_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1099 "Delay.MPI.pattern"

       data->mInputScales.insert(make_pair(KEY_RCOST_BARRIER, m_cost.getCosts(event)));
    
}

void PatternDelay_Barrier_LT::pre_delay_n2n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1123 "Delay.MPI.pattern"

       data->mInputScales.insert(make_pair(KEY_RCOST_BARRIER, m_cost.getCosts(event)));
    
}

void PatternDelay_Barrier_LT::pre_delay_omp_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1161 "Delay.MPI.pattern"

       data->mInputScales.insert(make_pair(KEY_RCOST_BARRIER, m_cost.getCosts(event)));
    
}

void PatternDelay_Barrier_LT::pre_inv_sendcmp_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1068 "Delay.MPI.pattern"

       Buffer* buffer = new Buffer(16);

       buffer->put_double(m_cost.getCosts(event));
       data->mInvLocal->add_buffer(buffer, BUFFER_RCOST_BARRIER);
    
}

void PatternDelay_Barrier_LT::pre_recv_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1037 "Delay.MPI.pattern"

       Buffer* buffer = new Buffer(16);

       buffer->put_double(m_cost.getCosts(event));
       data->mLocal->add_buffer(buffer, BUFFER_RCOST_BARRIER);
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternDelay_NxN
 *
 *---------------------------------------------------------------------------
 */


class PatternDelay_NxN : public Pattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("bwc");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(COLL_N2N, PEARL_create_callback(this, &PatternDelay_NxN::coll_n2n_bwc_cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_DELAY_N2N;
    }

    virtual long get_parent() const
    {
      return PAT_DELAY_N2N_AGGREGATE;
    }

    virtual string get_name() const
    {
      return "MPI short-term N x N delay costs";
    }

    virtual string get_unique_name() const
    {
      return "delay_n2n";
    }

    virtual string get_descr() const
    {
      return "Delays that directly cause wait states in MPI n-to-n collective communications";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void coll_n2n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternDelay_NxN::coll_n2n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1224 "Delay.MPI.pattern"

      const CollectiveInfo& ci(data->mCollinfo);

      data->mInputScales.clear();

      cbmanager.notify(PRE_DELAY_N2N, event, data);

      CalculateCollectiveDelay(ci.my, ci.latest, event, data);

      if (ci.my.rank == ci.latest.rank) {
        // Set severities 
        map<uint32_t, TimeMapSum>::const_iterator dWit = data->mCollDelayInfo.find(KEY_WAITTIME);

        if (dWit != data->mCollDelayInfo.end() && dWit->second.mSum > 0.0)
          for (timemap_t::const_iterator it = dWit->second.mMap.begin(); it != dWit->second.mMap.end(); ++it)
            m_severity[data->mDefs->get_cnode(it->first)] += it->second;

        cbmanager.notify(DELAY_N2N, event, data);
      }
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternDelay_NxN_LT
 *
 *---------------------------------------------------------------------------
 */


class PatternDelay_NxN_LT : public Pattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("bwc");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(PRE_RECV, PEARL_create_callback(this, &PatternDelay_NxN_LT::pre_recv_bwc_cb));
        (it->second)->register_callback(DELAY_LATE_SENDER, PEARL_create_callback(this, &PatternDelay_NxN_LT::delay_late_sender_bwc_cb));
        (it->second)->register_callback(PRE_INV_SENDCMP, PEARL_create_callback(this, &PatternDelay_NxN_LT::pre_inv_sendcmp_bwc_cb));
        (it->second)->register_callback(DELAY_LATE_RECEIVER, PEARL_create_callback(this, &PatternDelay_NxN_LT::delay_late_receiver_bwc_cb));
        (it->second)->register_callback(PRE_DELAY_BARRIER, PEARL_create_callback(this, &PatternDelay_NxN_LT::pre_delay_barrier_bwc_cb));
        (it->second)->register_callback(DELAY_BARRIER, PEARL_create_callback(this, &PatternDelay_NxN_LT::delay_barrier_bwc_cb));
        (it->second)->register_callback(PRE_DELAY_N2N, PEARL_create_callback(this, &PatternDelay_NxN_LT::pre_delay_n2n_bwc_cb));
        (it->second)->register_callback(DELAY_N2N, PEARL_create_callback(this, &PatternDelay_NxN_LT::delay_n2n_bwc_cb));
        (it->second)->register_callback(PRE_DELAY_12N, PEARL_create_callback(this, &PatternDelay_NxN_LT::pre_delay_12n_bwc_cb));
        (it->second)->register_callback(DELAY_12N, PEARL_create_callback(this, &PatternDelay_NxN_LT::delay_12n_bwc_cb));
        (it->second)->register_callback(PRE_DELAY_OMP_BARRIER, PEARL_create_callback(this, &PatternDelay_NxN_LT::pre_delay_omp_barrier_bwc_cb));
        (it->second)->register_callback(DELAY_OMP_BARRIER, PEARL_create_callback(this, &PatternDelay_NxN_LT::delay_omp_barrier_bwc_cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_DELAY_N2N_LONGTERM;
    }

    virtual long get_parent() const
    {
      return PAT_DELAY_N2N_AGGREGATE;
    }

    virtual string get_name() const
    {
      return "MPI long-term N x N delay costs";
    }

    virtual string get_unique_name() const
    {
      return "delay_n2n_longterm";
    }

    virtual string get_descr() const
    {
      return "Delays that indirectly cause wait states in MPI n-to-n collective communications through wait-state propagation";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void delay_12n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_late_receiver_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_late_sender_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_n2n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_omp_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_delay_12n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_delay_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_delay_n2n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_delay_omp_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_inv_sendcmp_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_recv_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);


  private:

#line 1271 "Delay.MPI.pattern"

    DelayCostMap m_cost;
  
};


/*----- Callback methods -----*/

void PatternDelay_NxN_LT::delay_12n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1384 "Delay.MPI.pattern"

       // Set severities 
       map<uint32_t, TimeMapSum>::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_N2N);

       if (dCit != data->mCollDelayInfo.end() && dCit->second.mSum > 0.0)
         for (timemap_t::const_iterator it = dCit->second.mMap.begin(); it != dCit->second.mMap.end(); ++it)
           m_severity[data->mDefs->get_cnode(it->first)] += it->second;

       // Update long-term costs for propagating wait states in synchronization interval
       map<uint32_t, ScaleMap>::const_iterator sCit = data->mSumScales.find(KEY_RCOST_N2N);

       if (sCit != data->mSumScales.end())
         m_cost.addCosts(data->mPropSpMap, sCit->second);
    
}

void PatternDelay_NxN_LT::delay_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1341 "Delay.MPI.pattern"

       // Set severities 
       map<uint32_t, TimeMapSum>::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_N2N);

       if (dCit != data->mCollDelayInfo.end() && dCit->second.mSum > 0.0)
         for (timemap_t::const_iterator it = dCit->second.mMap.begin(); it != dCit->second.mMap.end(); ++it)
           m_severity[data->mDefs->get_cnode(it->first)] += it->second;

       // Update long-term costs for propagating wait states in synchronization interval
       map<uint32_t, ScaleMap>::const_iterator sCit = data->mSumScales.find(KEY_RCOST_N2N);

       if (sCit != data->mSumScales.end())
         m_cost.addCosts(data->mPropSpMap, sCit->second);
    
}

void PatternDelay_NxN_LT::delay_late_receiver_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1313 "Delay.MPI.pattern"

       // Shortcuts
       const TimeMapSum& d(data->mDelayInfo);
       const TimeMapSum& w(data->mWaitInfo);

       if (!(d.mSum + w.mSum > 0))
         return;

       // --- get remote cost & set severities

       double rcost = data->mInvRemote->get_buffer(BUFFER_RCOST_N2N)->get_double();
       double scale = rcost / (d.mSum + w.mSum);

       if (!(scale > 0))
         return;

       for (timemap_t::const_iterator it = d.mMap.begin(); it != d.mMap.end(); ++it)
         m_severity[data->mDefs->get_cnode(it->first)] += scale * it->second;

       // --- set propagating long-term costs

       m_cost.addCosts(data->mPropSpMap, scale);
    
}

void PatternDelay_NxN_LT::delay_late_sender_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1282 "Delay.MPI.pattern"

       // Shortcuts
       const TimeMapSum& d(data->mDelayInfo);
       const TimeMapSum& w(data->mWaitInfo);
     
       if (!(d.mSum + w.mSum > 0))
         return;

       // --- get remote cost & set severities

       double rcost = data->mRemote->get_buffer(BUFFER_RCOST_N2N)->get_double();
       double scale = rcost / (d.mSum + w.mSum);

       if (!(scale > 0))
         return;

       for (timemap_t::const_iterator it = d.mMap.begin(); it != d.mMap.end(); ++it)
         m_severity[data->mDefs->get_cnode(it->first)] += scale * it->second;

       // --- set propagating long-term costs

       m_cost.addCosts(data->mPropSpMap, scale);
    
}

void PatternDelay_NxN_LT::delay_n2n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1360 "Delay.MPI.pattern"

       // Set severities 
       map<uint32_t, TimeMapSum>::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_N2N);

       if (dCit != data->mCollDelayInfo.end() && dCit->second.mSum > 0.0)
         for (timemap_t::const_iterator it = dCit->second.mMap.begin(); it != dCit->second.mMap.end(); ++it)
           m_severity[data->mDefs->get_cnode(it->first)] += it->second;

       // Update long-term costs for propagating wait states in synchronization interval
       map<uint32_t, ScaleMap>::const_iterator sCit = data->mSumScales.find(KEY_RCOST_N2N);

       if (sCit != data->mSumScales.end())
         m_cost.addCosts(data->mPropSpMap, sCit->second);

       map<uint32_t, ScaleMap>::const_iterator sWit = data->mSumScales.find(KEY_WAITTIME);

       if (sWit != data->mSumScales.end())
         m_cost.addCosts(data->mPropSpMap, sWit->second);
    
}

void PatternDelay_NxN_LT::delay_omp_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1403 "Delay.MPI.pattern"

       // Set severities 
       map<uint32_t, TimeMapSum>::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_N2N);

       if (dCit != data->mCollDelayInfo.end() && dCit->second.mSum > 0.0)
         for (timemap_t::const_iterator it = dCit->second.mMap.begin(); it != dCit->second.mMap.end(); ++it)
           m_severity[data->mDefs->get_cnode(it->first)] += it->second;

       // Update long-term costs for propagating wait states in synchronization interval
       map<uint32_t, ScaleMap>::const_iterator sCit = data->mSumScales.find(KEY_RCOST_N2N);

       if (sCit != data->mSumScales.end())
         m_cost.addCosts(data->mPropSpMap, sCit->second);
    
}

void PatternDelay_NxN_LT::pre_delay_12n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1380 "Delay.MPI.pattern"

       data->mInputScales.insert(make_pair(KEY_RCOST_N2N, m_cost.getCosts(event)));
    
}

void PatternDelay_NxN_LT::pre_delay_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1337 "Delay.MPI.pattern"

       data->mInputScales.insert(make_pair(KEY_RCOST_N2N, m_cost.getCosts(event)));
    
}

void PatternDelay_NxN_LT::pre_delay_n2n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1356 "Delay.MPI.pattern"

       data->mInputScales.insert(make_pair(KEY_RCOST_N2N, m_cost.getCosts(event)));
    
}

void PatternDelay_NxN_LT::pre_delay_omp_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1399 "Delay.MPI.pattern"

       data->mInputScales.insert(make_pair(KEY_RCOST_N2N, m_cost.getCosts(event)));
    
}

void PatternDelay_NxN_LT::pre_inv_sendcmp_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1306 "Delay.MPI.pattern"

       Buffer* buffer = new Buffer(16);

       buffer->put_double(m_cost.getCosts(event));
       data->mInvLocal->add_buffer(buffer, BUFFER_RCOST_N2N);
    
}

void PatternDelay_NxN_LT::pre_recv_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1275 "Delay.MPI.pattern"

       Buffer* buffer = new Buffer(16);

       buffer->put_double(m_cost.getCosts(event));
       data->mLocal->add_buffer(buffer, BUFFER_RCOST_N2N);
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternDelay_12N
 *
 *---------------------------------------------------------------------------
 */


class PatternDelay_12N : public Pattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("bwc");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(COLL_12N, PEARL_create_callback(this, &PatternDelay_12N::coll_12n_bwc_cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_DELAY_12N;
    }

    virtual long get_parent() const
    {
      return PAT_DELAY_12N_AGGREGATE;
    }

    virtual string get_name() const
    {
      return "MPI short-term 1-to-N delay costs";
    }

    virtual string get_unique_name() const
    {
      return "delay_12n";
    }

    virtual string get_descr() const
    {
      return "Delays that directly cause wait states in MPI 1-to-n collective communications";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void coll_12n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternDelay_12N::coll_12n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1462 "Delay.MPI.pattern"

      const CollectiveInfo& ci(data->mCollinfo);

      data->mInputScales.clear();

      cbmanager.notify(PRE_DELAY_12N, event, data);

      CalculateCollectiveDelay(ci.my, ci.root, event, data);

      if (ci.my.rank == ci.root.rank) {
        // Set severities 
        map<uint32_t, TimeMapSum>::const_iterator dWit = data->mCollDelayInfo.find(KEY_WAITTIME);

        if (dWit != data->mCollDelayInfo.end() && dWit->second.mSum > 0.0)
          for (timemap_t::const_iterator it = dWit->second.mMap.begin(); it != dWit->second.mMap.end(); ++it)
            m_severity[data->mDefs->get_cnode(it->first)] += it->second;

        cbmanager.notify(DELAY_12N, event, data);
      }
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternDelay_12N_LT
 *
 *---------------------------------------------------------------------------
 */


class PatternDelay_12N_LT : public Pattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("bwc");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(PRE_RECV, PEARL_create_callback(this, &PatternDelay_12N_LT::pre_recv_bwc_cb));
        (it->second)->register_callback(DELAY_LATE_SENDER, PEARL_create_callback(this, &PatternDelay_12N_LT::delay_late_sender_bwc_cb));
        (it->second)->register_callback(PRE_INV_SENDCMP, PEARL_create_callback(this, &PatternDelay_12N_LT::pre_inv_sendcmp_bwc_cb));
        (it->second)->register_callback(DELAY_LATE_RECEIVER, PEARL_create_callback(this, &PatternDelay_12N_LT::delay_late_receiver_bwc_cb));
        (it->second)->register_callback(PRE_DELAY_BARRIER, PEARL_create_callback(this, &PatternDelay_12N_LT::pre_delay_barrier_bwc_cb));
        (it->second)->register_callback(DELAY_BARRIER, PEARL_create_callback(this, &PatternDelay_12N_LT::delay_barrier_bwc_cb));
        (it->second)->register_callback(PRE_DELAY_N2N, PEARL_create_callback(this, &PatternDelay_12N_LT::pre_delay_n2n_bwc_cb));
        (it->second)->register_callback(DELAY_N2N, PEARL_create_callback(this, &PatternDelay_12N_LT::delay_n2n_bwc_cb));
        (it->second)->register_callback(PRE_DELAY_12N, PEARL_create_callback(this, &PatternDelay_12N_LT::pre_delay_12n_bwc_cb));
        (it->second)->register_callback(DELAY_12N, PEARL_create_callback(this, &PatternDelay_12N_LT::delay_12n_bwc_cb));
        (it->second)->register_callback(PRE_DELAY_OMP_BARRIER, PEARL_create_callback(this, &PatternDelay_12N_LT::pre_delay_omp_barrier_bwc_cb));
        (it->second)->register_callback(DELAY_OMP_BARRIER, PEARL_create_callback(this, &PatternDelay_12N_LT::delay_omp_barrier_bwc_cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_DELAY_12N_LONGTERM;
    }

    virtual long get_parent() const
    {
      return PAT_DELAY_12N_AGGREGATE;
    }

    virtual string get_name() const
    {
      return "MPI long-term 1-to-N delay costs";
    }

    virtual string get_unique_name() const
    {
      return "delay_12n_longterm";
    }

    virtual string get_descr() const
    {
      return "Delays that indirectly cause wait states in MPI 1-to-n collective communications through wait-state propagation";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void delay_12n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_late_receiver_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_late_sender_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_n2n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_omp_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_delay_12n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_delay_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_delay_n2n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_delay_omp_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_inv_sendcmp_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_recv_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);


  private:

#line 1509 "Delay.MPI.pattern"

    DelayCostMap m_cost;
  
};


/*----- Callback methods -----*/

void PatternDelay_12N_LT::delay_12n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1617 "Delay.MPI.pattern"

       // Set severities 
       map<uint32_t, TimeMapSum>::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_12N);

       if (dCit != data->mCollDelayInfo.end() && dCit->second.mSum > 0.0)
         for (timemap_t::const_iterator it = dCit->second.mMap.begin(); it != dCit->second.mMap.end(); ++it)
           m_severity[data->mDefs->get_cnode(it->first)] += it->second;

       // Update long-term costs for propagating wait states in synchronization interval
       map<uint32_t, ScaleMap>::const_iterator sCit = data->mSumScales.find(KEY_RCOST_12N);

       if (sCit != data->mSumScales.end())
         m_cost.addCosts(data->mPropSpMap, sCit->second);

       map<uint32_t, ScaleMap>::const_iterator sWit = data->mSumScales.find(KEY_WAITTIME);

       if (sWit != data->mSumScales.end())
         m_cost.addCosts(data->mPropSpMap, sWit->second);
    
}

void PatternDelay_12N_LT::delay_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1579 "Delay.MPI.pattern"

       // Set severities 
       map<uint32_t, TimeMapSum>::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_12N);

       if (dCit != data->mCollDelayInfo.end() && dCit->second.mSum > 0.0)
         for (timemap_t::const_iterator it = dCit->second.mMap.begin(); it != dCit->second.mMap.end(); ++it)
           m_severity[data->mDefs->get_cnode(it->first)] += it->second;

       // Update long-term costs for propagating wait states in synchronization interval
       map<uint32_t, ScaleMap>::const_iterator sCit = data->mSumScales.find(KEY_RCOST_12N);

       if (sCit != data->mSumScales.end())
         m_cost.addCosts(data->mPropSpMap, sCit->second);
    
}

void PatternDelay_12N_LT::delay_late_receiver_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1551 "Delay.MPI.pattern"

       // Shortcuts
       const TimeMapSum& d(data->mDelayInfo);
       const TimeMapSum& w(data->mWaitInfo);

       if (!(d.mSum + w.mSum > 0))
         return;

       // --- get remote cost & set severities

       double rcost = data->mInvRemote->get_buffer(BUFFER_RCOST_12N)->get_double();
       double scale = rcost / (d.mSum + w.mSum);

       if (!(scale > 0))
         return;

       for (timemap_t::const_iterator it = d.mMap.begin(); it != d.mMap.end(); ++it)
         m_severity[data->mDefs->get_cnode(it->first)] += scale * it->second;

       // --- set propagating long-term costs

       m_cost.addCosts(data->mPropSpMap, scale);
    
}

void PatternDelay_12N_LT::delay_late_sender_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1520 "Delay.MPI.pattern"

       // Shortcuts
       const TimeMapSum& d(data->mDelayInfo);
       const TimeMapSum& w(data->mWaitInfo);
     
       if (!(d.mSum + w.mSum > 0))
         return;

       // --- get remote cost & set severities

       double rcost = data->mRemote->get_buffer(BUFFER_RCOST_12N)->get_double();
       double scale = rcost / (d.mSum + w.mSum);

       if (!(scale > 0))
         return;

       for (timemap_t::const_iterator it = d.mMap.begin(); it != d.mMap.end(); ++it)
         m_severity[data->mDefs->get_cnode(it->first)] += scale * it->second;

       // --- set propagating long-term costs

       m_cost.addCosts(data->mPropSpMap, scale);
    
}

void PatternDelay_12N_LT::delay_n2n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1598 "Delay.MPI.pattern"

       // Set severities 
       map<uint32_t, TimeMapSum>::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_12N);

       if (dCit != data->mCollDelayInfo.end() && dCit->second.mSum > 0.0)
         for (timemap_t::const_iterator it = dCit->second.mMap.begin(); it != dCit->second.mMap.end(); ++it)
           m_severity[data->mDefs->get_cnode(it->first)] += it->second;

       // Update long-term costs for propagating wait states in synchronization interval
       map<uint32_t, ScaleMap>::const_iterator sCit = data->mSumScales.find(KEY_RCOST_12N);

       if (sCit != data->mSumScales.end())
         m_cost.addCosts(data->mPropSpMap, sCit->second);
    
}

void PatternDelay_12N_LT::delay_omp_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1641 "Delay.MPI.pattern"

       // Set severities 
       map<uint32_t, TimeMapSum>::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_12N);

       if (dCit != data->mCollDelayInfo.end() && dCit->second.mSum > 0.0)
         for (timemap_t::const_iterator it = dCit->second.mMap.begin(); it != dCit->second.mMap.end(); ++it)
           m_severity[data->mDefs->get_cnode(it->first)] += it->second;

       // Update long-term costs for propagating wait states in synchronization interval
       map<uint32_t, ScaleMap>::const_iterator sCit = data->mSumScales.find(KEY_RCOST_12N);

       if (sCit != data->mSumScales.end())
         m_cost.addCosts(data->mPropSpMap, sCit->second);
    
}

void PatternDelay_12N_LT::pre_delay_12n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1613 "Delay.MPI.pattern"

       data->mInputScales.insert(make_pair(KEY_RCOST_12N, m_cost.getCosts(event)));
    
}

void PatternDelay_12N_LT::pre_delay_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1575 "Delay.MPI.pattern"

       data->mInputScales.insert(make_pair(KEY_RCOST_12N, m_cost.getCosts(event)));
    
}

void PatternDelay_12N_LT::pre_delay_n2n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1594 "Delay.MPI.pattern"

       data->mInputScales.insert(make_pair(KEY_RCOST_12N, m_cost.getCosts(event)));
    
}

void PatternDelay_12N_LT::pre_delay_omp_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1637 "Delay.MPI.pattern"

       data->mInputScales.insert(make_pair(KEY_RCOST_12N, m_cost.getCosts(event)));
    
}

void PatternDelay_12N_LT::pre_inv_sendcmp_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1544 "Delay.MPI.pattern"

       Buffer* buffer = new Buffer(16);

       buffer->put_double(m_cost.getCosts(event));
       data->mInvLocal->add_buffer(buffer, BUFFER_RCOST_12N);
    
}

void PatternDelay_12N_LT::pre_recv_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1513 "Delay.MPI.pattern"

       Buffer* buffer = new Buffer(16);

       buffer->put_double(m_cost.getCosts(event));
       data->mLocal->add_buffer(buffer, BUFFER_RCOST_12N);
    
}

#endif   /* _MPI */

#if defined(_OPENMP)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternDelay_OmpBarrier
 *
 *---------------------------------------------------------------------------
 */


class PatternDelay_OmpBarrier : public Pattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("bwc");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(OMP_BARRIER, PEARL_create_callback(this, &PatternDelay_OmpBarrier::omp_barrier_bwc_cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_DELAY_OMPBARRIER;
    }

    virtual long get_parent() const
    {
      return PAT_DELAY_OMPBARRIER_AGGREGATE;
    }

    virtual string get_name() const
    {
      return "OMP short-term barrier delay costs";
    }

    virtual string get_unique_name() const
    {
      return "delay_ompbarrier";
    }

    virtual string get_descr() const
    {
      return "Delays that directly cause wait states in OpenMP barriers";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void omp_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);


  private:

#line 100 "Delay.OMP.pattern"

#ifdef OMPDELAYDEBUG
    std::ofstream m_dbg;
#endif
  
};


/*----- Callback methods -----*/

void PatternDelay_OmpBarrier::omp_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 106 "Delay.OMP.pattern"

      const CollectiveInfo& ci(data->mCollinfo);

      data->mInputScales.clear();

      cbmanager.notify(PRE_DELAY_OMP_BARRIER, event, data);

      CalculateOmpBarrierDelay(ci.my, ci.latest, event, data);

      if (ci.my.rank == ci.latest.rank) {
        // Set severities 
        map<uint32_t, TimeMapSum>::const_iterator dWit = data->mCollDelayInfo.find(KEY_WAITTIME);

        if (dWit != data->mCollDelayInfo.end() && dWit->second.mSum > 0.0)
          for (timemap_t::const_iterator it = dWit->second.mMap.begin(); it != dWit->second.mMap.end(); ++it)
            m_severity[data->mDefs->get_cnode(it->first)] += it->second;

        cbmanager.notify(DELAY_OMP_BARRIER, event, data);
      }
    
}

#endif   /* _OPENMP */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternDelay_OmpBarrier_LT
 *
 *---------------------------------------------------------------------------
 */


class PatternDelay_OmpBarrier_LT : public Pattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("bwc");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(PRE_RECV, PEARL_create_callback(this, &PatternDelay_OmpBarrier_LT::pre_recv_bwc_cb));
        (it->second)->register_callback(DELAY_LATE_SENDER, PEARL_create_callback(this, &PatternDelay_OmpBarrier_LT::delay_late_sender_bwc_cb));
        (it->second)->register_callback(PRE_INV_SENDCMP, PEARL_create_callback(this, &PatternDelay_OmpBarrier_LT::pre_inv_sendcmp_bwc_cb));
        (it->second)->register_callback(DELAY_LATE_RECEIVER, PEARL_create_callback(this, &PatternDelay_OmpBarrier_LT::delay_late_receiver_bwc_cb));
        (it->second)->register_callback(PRE_DELAY_BARRIER, PEARL_create_callback(this, &PatternDelay_OmpBarrier_LT::pre_delay_barrier_bwc_cb));
        (it->second)->register_callback(DELAY_BARRIER, PEARL_create_callback(this, &PatternDelay_OmpBarrier_LT::delay_barrier_bwc_cb));
        (it->second)->register_callback(PRE_DELAY_N2N, PEARL_create_callback(this, &PatternDelay_OmpBarrier_LT::pre_delay_n2n_bwc_cb));
        (it->second)->register_callback(DELAY_N2N, PEARL_create_callback(this, &PatternDelay_OmpBarrier_LT::delay_n2n_bwc_cb));
        (it->second)->register_callback(PRE_DELAY_12N, PEARL_create_callback(this, &PatternDelay_OmpBarrier_LT::pre_delay_12n_bwc_cb));
        (it->second)->register_callback(DELAY_12N, PEARL_create_callback(this, &PatternDelay_OmpBarrier_LT::delay_12n_bwc_cb));
        (it->second)->register_callback(DELAY_OMP_BARRIER, PEARL_create_callback(this, &PatternDelay_OmpBarrier_LT::delay_omp_barrier_bwc_cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_DELAY_OMPBARRIER_LONGTERM;
    }

    virtual long get_parent() const
    {
      return PAT_DELAY_OMPBARRIER_AGGREGATE;
    }

    virtual string get_name() const
    {
      return "OMP long-term barrier delay costs";
    }

    virtual string get_unique_name() const
    {
      return "delay_ompbarrier_longterm";
    }

    virtual string get_descr() const
    {
      return "Delays that indirectly cause wait states in OpenMP barriers through wait-state propagation";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void delay_12n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_late_receiver_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_late_sender_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_n2n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_omp_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_delay_12n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_delay_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_delay_n2n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_inv_sendcmp_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_recv_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);


  private:

#line 153 "Delay.OMP.pattern"

    DelayCostMap m_cost;
  
};


/*----- Callback methods -----*/

void PatternDelay_OmpBarrier_LT::delay_12n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 261 "Delay.OMP.pattern"

       // Set severities 
       map<uint32_t, TimeMapSum>::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_OMPBARRIER);

       if (dCit != data->mCollDelayInfo.end() && dCit->second.mSum > 0.0)
         for (timemap_t::const_iterator it = dCit->second.mMap.begin(); it != dCit->second.mMap.end(); ++it)
           m_severity[data->mDefs->get_cnode(it->first)] += it->second;

       // Update long-term costs for propagating wait states in synchronization interval
       map<uint32_t, ScaleMap>::const_iterator sCit = data->mSumScales.find(KEY_RCOST_OMPBARRIER);

       if (sCit != data->mSumScales.end())
         m_cost.addCosts(data->mPropSpMap, sCit->second);
    
}

void PatternDelay_OmpBarrier_LT::delay_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 223 "Delay.OMP.pattern"

       // Set severities 
       map<uint32_t, TimeMapSum>::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_OMPBARRIER);

       if (dCit != data->mCollDelayInfo.end() && dCit->second.mSum > 0.0)
         for (timemap_t::const_iterator it = dCit->second.mMap.begin(); it != dCit->second.mMap.end(); ++it)
           m_severity[data->mDefs->get_cnode(it->first)] += it->second;

       // Update long-term costs for propagating wait states in synchronization interval
       map<uint32_t, ScaleMap>::const_iterator sCit = data->mSumScales.find(KEY_RCOST_OMPBARRIER);

       if (sCit != data->mSumScales.end())
         m_cost.addCosts(data->mPropSpMap, sCit->second);
    
}

void PatternDelay_OmpBarrier_LT::delay_late_receiver_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 195 "Delay.OMP.pattern"

       // Shortcuts
       const TimeMapSum& d(data->mDelayInfo);
       const TimeMapSum& w(data->mWaitInfo);

       if (!(d.mSum + w.mSum > 0))
         return;

       // --- get remote cost & set severities

       double rcost = data->mInvRemote->get_buffer(BUFFER_RCOST_OMPBARRIER)->get_double();
       double scale = rcost / (d.mSum + w.mSum);

       if (!(scale > 0))
         return;

       for (timemap_t::const_iterator it = d.mMap.begin(); it != d.mMap.end(); ++it)
         m_severity[data->mDefs->get_cnode(it->first)] += scale * it->second;

       // --- set propagating long-term costs

       m_cost.addCosts(data->mPropSpMap, scale);
    
}

void PatternDelay_OmpBarrier_LT::delay_late_sender_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 164 "Delay.OMP.pattern"

       // Shortcuts
       const TimeMapSum& d(data->mDelayInfo);
       const TimeMapSum& w(data->mWaitInfo);
     
       if (!(d.mSum + w.mSum > 0))
         return;

       // --- get remote cost & set severities

       double rcost = data->mRemote->get_buffer(BUFFER_RCOST_OMPBARRIER)->get_double();
       double scale = rcost / (d.mSum + w.mSum);

       if (!(scale > 0))
         return;

       for (timemap_t::const_iterator it = d.mMap.begin(); it != d.mMap.end(); ++it)
         m_severity[data->mDefs->get_cnode(it->first)] += scale * it->second;

       // --- set propagating long-term costs

       m_cost.addCosts(data->mPropSpMap, scale);
    
}

void PatternDelay_OmpBarrier_LT::delay_n2n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 242 "Delay.OMP.pattern"

       // Set severities 
       map<uint32_t, TimeMapSum>::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_OMPBARRIER);

       if (dCit != data->mCollDelayInfo.end() && dCit->second.mSum > 0.0)
         for (timemap_t::const_iterator it = dCit->second.mMap.begin(); it != dCit->second.mMap.end(); ++it)
           m_severity[data->mDefs->get_cnode(it->first)] += it->second;

       // Update long-term costs for propagating wait states in synchronization interval
       map<uint32_t, ScaleMap>::const_iterator sCit = data->mSumScales.find(KEY_RCOST_OMPBARRIER);

       if (sCit != data->mSumScales.end())
         m_cost.addCosts(data->mPropSpMap, sCit->second);
    
}

void PatternDelay_OmpBarrier_LT::delay_omp_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 276 "Delay.OMP.pattern"

       map<uint32_t, ScaleMap>::const_iterator it = data->mSumScales.find(KEY_WAITTIME);

       if (it != data->mSumScales.end())
         m_cost.addCosts(data->mPropSpMap, it->second);
    
}

void PatternDelay_OmpBarrier_LT::pre_delay_12n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 257 "Delay.OMP.pattern"

       data->mInputScales.insert(make_pair(KEY_RCOST_OMPBARRIER, m_cost.getCosts(event)));
    
}

void PatternDelay_OmpBarrier_LT::pre_delay_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 219 "Delay.OMP.pattern"

       data->mInputScales.insert(make_pair(KEY_RCOST_OMPBARRIER, m_cost.getCosts(event)));
    
}

void PatternDelay_OmpBarrier_LT::pre_delay_n2n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 238 "Delay.OMP.pattern"

       data->mInputScales.insert(make_pair(KEY_RCOST_OMPBARRIER, m_cost.getCosts(event)));
    
}

void PatternDelay_OmpBarrier_LT::pre_inv_sendcmp_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 188 "Delay.OMP.pattern"

       Buffer* buffer = new Buffer(16);

       buffer->put_double(m_cost.getCosts(event));
       data->mInvLocal->add_buffer(buffer, BUFFER_RCOST_OMPBARRIER);
    
}

void PatternDelay_OmpBarrier_LT::pre_recv_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 157 "Delay.OMP.pattern"

       Buffer* buffer = new Buffer(16);

       buffer->put_double(m_cost.getCosts(event));
       data->mLocal->add_buffer(buffer, BUFFER_RCOST_OMPBARRIER);
    
}

#endif   /* _MPI */

#if defined(_OPENMP)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternDelay_OmpIdle
 *
 *---------------------------------------------------------------------------
 */


class PatternDelay_OmpIdle : public Pattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("bwc");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(OMP_MGMT_FORK, PEARL_create_callback(this, &PatternDelay_OmpIdle::omp_mgmt_fork_bwc_cb));
        (it->second)->register_callback(FINISHED, PEARL_create_callback(this, &PatternDelay_OmpIdle::finished_bwc_cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_DELAY_OMPIDLE;
    }

    virtual long get_parent() const
    {
      return PAT_DELAY_OMPIDLE_AGGREGATE;
    }

    virtual string get_name() const
    {
      return "OMP short-term idleness delay costs";
    }

    virtual string get_unique_name() const
    {
      return "delay_ompidle";
    }

    virtual string get_descr() const
    {
      return "Delays on the OpenMP master thread that leave OpenMP worker threads idle";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void finished_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void omp_mgmt_fork_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);


  private:

#line 326 "Delay.OMP.pattern"

#ifdef OMPDELAYDEBUG
    std::ofstream m_dbg;
#endif
  
};


/*----- Callback methods -----*/

void PatternDelay_OmpIdle::finished_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 371 "Delay.OMP.pattern"

#pragma omp master
      {
        // Shortcuts
        const TmapCacheHandler*  TmH = data->mTmapCacheHandler;
        const SynchpointHandler* SpH = data->mSynchpointHandler;

        data->mPrevSp    = SpH->findPreviousOmpSynchpoint(event);
        data->mPropSpMap = SpH->getSynchpointsBetween(data->mPrevSp, Event());
        data->mWaitInfo  = CalculateWaitMap(data->mPropSpMap);
        data->mDelayInfo = 
          PruneDelayTimemap(TmH->getRuntimes(data->mPrevSp, event),
                            data->mWaitInfo.mMap,
                            data->mPrevSp,
                            SpH->getSynchpointInfo(data->mPrevSp));

        // shortcuts
        const TimeMapSum& d(data->mDelayInfo);
        const TimeMapSum& w(data->mWaitInfo);

        // --- calculate delay costs

        double scale = 0.0;

        if (d.mSum + w.mSum > 0)
          scale = (omp_get_num_threads() - 1);

        // --- set severities

        if (d.mSum > 0)
          for (timemap_t::const_iterator it = d.mMap.begin(); it != d.mMap.end(); ++it)
            m_severity[data->mDefs->get_cnode(it->first)] += scale * it->second;

        data->mOmpIdleScale = scale;

        cbmanager.notify(DELAY_OMP_IDLE, event, data);
      }
    
}

void PatternDelay_OmpIdle::omp_mgmt_fork_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 332 "Delay.OMP.pattern"

#pragma omp master
      {
        // Shortcuts
        const TmapCacheHandler*  TmH = data->mTmapCacheHandler;
        const SynchpointHandler* SpH = data->mSynchpointHandler;

        data->mPrevSp    = SpH->findPreviousOmpSynchpoint(event);
        data->mPropSpMap = SpH->getSynchpointsBetween(data->mPrevSp, event);
        data->mWaitInfo  = CalculateWaitMap(data->mPropSpMap);
        data->mDelayInfo = 
          PruneDelayTimemap(TmH->getRuntimes(data->mPrevSp, event),
                            data->mWaitInfo.mMap,
                            data->mPrevSp,
                            SpH->getSynchpointInfo(data->mPrevSp));

        // shortcuts
        const TimeMapSum& d(data->mDelayInfo);
        const TimeMapSum& w(data->mWaitInfo);

        // --- calculate delay costs

        double scale = 0.0;

        if (d.mSum + w.mSum > 0)
          scale = (omp_get_num_threads() - 1);

        // --- set severities

        if (d.mSum > 0)
          for (timemap_t::const_iterator it = d.mMap.begin(); it != d.mMap.end(); ++it)
            m_severity[data->mDefs->get_cnode(it->first)] += scale * it->second;

        data->mOmpIdleScale = scale;

        cbmanager.notify(DELAY_OMP_IDLE, event, data);
      }
    
}

#endif   /* _OPENMP */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternDelay_OmpIdle_LT
 *
 *---------------------------------------------------------------------------
 */


class PatternDelay_OmpIdle_LT : public Pattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("bwc");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(PRE_RECV, PEARL_create_callback(this, &PatternDelay_OmpIdle_LT::pre_recv_bwc_cb));
        (it->second)->register_callback(DELAY_LATE_SENDER, PEARL_create_callback(this, &PatternDelay_OmpIdle_LT::delay_late_sender_bwc_cb));
        (it->second)->register_callback(PRE_INV_SENDCMP, PEARL_create_callback(this, &PatternDelay_OmpIdle_LT::pre_inv_sendcmp_bwc_cb));
        (it->second)->register_callback(DELAY_LATE_RECEIVER, PEARL_create_callback(this, &PatternDelay_OmpIdle_LT::delay_late_receiver_bwc_cb));
        (it->second)->register_callback(PRE_DELAY_BARRIER, PEARL_create_callback(this, &PatternDelay_OmpIdle_LT::pre_delay_barrier_bwc_cb));
        (it->second)->register_callback(DELAY_BARRIER, PEARL_create_callback(this, &PatternDelay_OmpIdle_LT::delay_barrier_bwc_cb));
        (it->second)->register_callback(PRE_DELAY_N2N, PEARL_create_callback(this, &PatternDelay_OmpIdle_LT::pre_delay_n2n_bwc_cb));
        (it->second)->register_callback(DELAY_N2N, PEARL_create_callback(this, &PatternDelay_OmpIdle_LT::delay_n2n_bwc_cb));
        (it->second)->register_callback(PRE_DELAY_12N, PEARL_create_callback(this, &PatternDelay_OmpIdle_LT::pre_delay_12n_bwc_cb));
        (it->second)->register_callback(DELAY_12N, PEARL_create_callback(this, &PatternDelay_OmpIdle_LT::delay_12n_bwc_cb));
        (it->second)->register_callback(DELAY_OMP_IDLE, PEARL_create_callback(this, &PatternDelay_OmpIdle_LT::delay_omp_idle_bwc_cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_DELAY_OMPIDLE_LONGTERM;
    }

    virtual long get_parent() const
    {
      return PAT_DELAY_OMPIDLE_AGGREGATE;
    }

    virtual string get_name() const
    {
      return "OMP long-term idleness delay costs";
    }

    virtual string get_unique_name() const
    {
      return "delay_ompidle_longterm";
    }

    virtual string get_descr() const
    {
      return "Delays that indirectly leave OpenMP worker threads idle";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void delay_12n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_late_receiver_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_late_sender_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_n2n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_omp_idle_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_delay_12n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_delay_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_delay_n2n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_inv_sendcmp_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_recv_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);


  private:

#line 448 "Delay.OMP.pattern"

    DelayCostMap m_cost;
  
};


/*----- Callback methods -----*/

void PatternDelay_OmpIdle_LT::delay_12n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 556 "Delay.OMP.pattern"

       // Set severities 
       map<uint32_t, TimeMapSum>::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_OMPIDLE);

       if (dCit != data->mCollDelayInfo.end() && dCit->second.mSum > 0.0)
         for (timemap_t::const_iterator it = dCit->second.mMap.begin(); it != dCit->second.mMap.end(); ++it)
           m_severity[data->mDefs->get_cnode(it->first)] += it->second;

       // Update long-term costs for propagating wait states in synchronization interval
       map<uint32_t, ScaleMap>::const_iterator sCit = data->mSumScales.find(KEY_RCOST_OMPIDLE);

       if (sCit != data->mSumScales.end())
         m_cost.addCosts(data->mPropSpMap, sCit->second);
    
}

void PatternDelay_OmpIdle_LT::delay_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 518 "Delay.OMP.pattern"

       // Set severities 
       map<uint32_t, TimeMapSum>::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_OMPIDLE);

       if (dCit != data->mCollDelayInfo.end() && dCit->second.mSum > 0.0)
         for (timemap_t::const_iterator it = dCit->second.mMap.begin(); it != dCit->second.mMap.end(); ++it)
           m_severity[data->mDefs->get_cnode(it->first)] += it->second;

       // Update long-term costs for propagating wait states in synchronization interval
       map<uint32_t, ScaleMap>::const_iterator sCit = data->mSumScales.find(KEY_RCOST_OMPIDLE);

       if (sCit != data->mSumScales.end())
         m_cost.addCosts(data->mPropSpMap, sCit->second);
    
}

void PatternDelay_OmpIdle_LT::delay_late_receiver_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 490 "Delay.OMP.pattern"

       // Shortcuts
       const TimeMapSum& d(data->mDelayInfo);
       const TimeMapSum& w(data->mWaitInfo);

       if (!(d.mSum + w.mSum > 0))
         return;

       // --- get remote cost & set severities

       double rcost = data->mInvRemote->get_buffer(BUFFER_RCOST_OMPIDLE)->get_double();
       double scale = rcost / (d.mSum + w.mSum);

       if (!(scale > 0))
         return;

       for (timemap_t::const_iterator it = d.mMap.begin(); it != d.mMap.end(); ++it)
         m_severity[data->mDefs->get_cnode(it->first)] += scale * it->second;

       // --- set propagating long-term costs

       m_cost.addCosts(data->mPropSpMap, scale);
    
}

void PatternDelay_OmpIdle_LT::delay_late_sender_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 459 "Delay.OMP.pattern"

       // Shortcuts
       const TimeMapSum& d(data->mDelayInfo);
       const TimeMapSum& w(data->mWaitInfo);
     
       if (!(d.mSum + w.mSum > 0))
         return;

       // --- get remote cost & set severities

       double rcost = data->mRemote->get_buffer(BUFFER_RCOST_OMPIDLE)->get_double();
       double scale = rcost / (d.mSum + w.mSum);

       if (!(scale > 0))
         return;

       for (timemap_t::const_iterator it = d.mMap.begin(); it != d.mMap.end(); ++it)
         m_severity[data->mDefs->get_cnode(it->first)] += scale * it->second;

       // --- set propagating long-term costs

       m_cost.addCosts(data->mPropSpMap, scale);
    
}

void PatternDelay_OmpIdle_LT::delay_n2n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 537 "Delay.OMP.pattern"

       // Set severities 
       map<uint32_t, TimeMapSum>::const_iterator dCit = data->mCollDelayInfo.find(KEY_RCOST_OMPIDLE);

       if (dCit != data->mCollDelayInfo.end() && dCit->second.mSum > 0.0)
         for (timemap_t::const_iterator it = dCit->second.mMap.begin(); it != dCit->second.mMap.end(); ++it)
           m_severity[data->mDefs->get_cnode(it->first)] += it->second;

       // Update long-term costs for propagating wait states in synchronization interval
       map<uint32_t, ScaleMap>::const_iterator sCit = data->mSumScales.find(KEY_RCOST_OMPIDLE);

       if (sCit != data->mSumScales.end())
         m_cost.addCosts(data->mPropSpMap, sCit->second);
    
}

void PatternDelay_OmpIdle_LT::delay_omp_idle_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 571 "Delay.OMP.pattern"

       if (data->mWaitInfo.mSum > 0.0)
         m_cost.addCosts(data->mPropSpMap, data->mOmpIdleScale);
    
}

void PatternDelay_OmpIdle_LT::pre_delay_12n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 552 "Delay.OMP.pattern"

       data->mInputScales.insert(make_pair(KEY_RCOST_OMPIDLE, m_cost.getCosts(event)));
    
}

void PatternDelay_OmpIdle_LT::pre_delay_barrier_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 514 "Delay.OMP.pattern"

       data->mInputScales.insert(make_pair(KEY_RCOST_OMPIDLE, m_cost.getCosts(event)));
    
}

void PatternDelay_OmpIdle_LT::pre_delay_n2n_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 533 "Delay.OMP.pattern"

       data->mInputScales.insert(make_pair(KEY_RCOST_OMPIDLE, m_cost.getCosts(event)));
    
}

void PatternDelay_OmpIdle_LT::pre_inv_sendcmp_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 483 "Delay.OMP.pattern"

       Buffer* buffer = new Buffer(16);

       buffer->put_double(m_cost.getCosts(event));
       data->mInvLocal->add_buffer(buffer, BUFFER_RCOST_OMPIDLE);
    
}

void PatternDelay_OmpIdle_LT::pre_recv_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 452 "Delay.OMP.pattern"

       Buffer* buffer = new Buffer(16);

       buffer->put_double(m_cost.getCosts(event));
       data->mLocal->add_buffer(buffer, BUFFER_RCOST_OMPIDLE);
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_Wait_Indirect_LS
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_Wait_Indirect_LS : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(RPL_RESUME_FWD, PEARL_create_callback(this, &PatternMPI_Wait_Indirect_LS::rpl_resume_fwd__cb));
      }
      {
        CallbackManagerMap::iterator it = cbmgrs.find("bwc");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(DELAY_LATE_SENDER, PEARL_create_callback(this, &PatternMPI_Wait_Indirect_LS::delay_late_sender_bwc_cb));
      }
      {
        CallbackManagerMap::iterator it = cbmgrs.find("fwc");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(PRE_SEND, PEARL_create_callback(this, &PatternMPI_Wait_Indirect_LS::pre_send_fwc_cb));
        (it->second)->register_callback(POST_RECV, PEARL_create_callback(this, &PatternMPI_Wait_Indirect_LS::post_recv_fwc_cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_WAIT_INDIRECT_LATESENDER;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_WAIT_INDIRECT;
    }

    virtual string get_name() const
    {
      return "Indirect MPI Late Sender wait states";
    }

    virtual string get_unique_name() const
    {
      return "mpi_wait_indirect_latesender";
    }

    virtual string get_descr() const
    {
      return "Indirect waiting time in MPI Late Sender wait states that results from propagation";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void delay_late_sender_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void post_recv_fwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_send_fwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void rpl_resume_fwd__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);


  private:

#line 114 "Waitstates.MPI.pattern"

    typedef std::map<Event, double, EventKeyCompare> eventmap_t;

    eventmap_t m_pw;
  
};


/*----- Callback methods -----*/

void PatternMPI_Wait_Indirect_LS::delay_late_sender_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 125 "Waitstates.MPI.pattern"

      // Shortcuts
      const TimeMapSum& d(data->mDelayInfo);
      const TimeMapSum& w(data->mWaitInfo);

      if (w.mSum > 0) {
        double scale = std::min(data->mRwait / (d.mSum + w.mSum), 1.0);
        m_pw[event] += scale * w.mSum;
      }
    
}

void PatternMPI_Wait_Indirect_LS::post_recv_fwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 145 "Waitstates.MPI.pattern"

      double pw = data->mRemote->get_buffer(BUFFER_PROPWAIT)->get_double();

      if (pw > 0.0)
        m_severity[event.get_cnode()] += pw;
    
}

void PatternMPI_Wait_Indirect_LS::pre_send_fwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 137 "Waitstates.MPI.pattern"

      eventmap_t::iterator it = m_pw.find(event);
      
      Buffer* buf = new Buffer(16);
      buf->put_double(it == m_pw.end() ? 0.0 : it->second);

      data->mLocal->add_buffer(buf, BUFFER_PROPWAIT);
    
}

void PatternMPI_Wait_Indirect_LS::rpl_resume_fwd__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 120 "Waitstates.MPI.pattern"

      m_pw.clear();
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_Wait_Indirect_LR
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_Wait_Indirect_LR : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(RPL_RESUME_FWD, PEARL_create_callback(this, &PatternMPI_Wait_Indirect_LR::rpl_resume_fwd__cb));
      }
      {
        CallbackManagerMap::iterator it = cbmgrs.find("bwc");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(DELAY_LATE_RECEIVER, PEARL_create_callback(this, &PatternMPI_Wait_Indirect_LR::delay_late_receiver_bwc_cb));
      }
      {
        CallbackManagerMap::iterator it = cbmgrs.find("fwc");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(PRE_INV_RECVREQ, PEARL_create_callback(this, &PatternMPI_Wait_Indirect_LR::pre_inv_recvreq_fwc_cb));
        (it->second)->register_callback(POST_INV_SENDCMP, PEARL_create_callback(this, &PatternMPI_Wait_Indirect_LR::post_inv_sendcmp_fwc_cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_WAIT_INDIRECT_LATERECEIVER;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_WAIT_INDIRECT;
    }

    virtual string get_name() const
    {
      return "Indirect MPI Late Receiver wait states";
    }

    virtual string get_unique_name() const
    {
      return "mpi_wait_indirect_latereceiver";
    }

    virtual string get_descr() const
    {
      return "Indirect waiting time in MPI Late Receiver wait states that results from propagation";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void delay_late_receiver_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void post_inv_sendcmp_fwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_inv_recvreq_fwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void rpl_resume_fwd__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);


  private:

#line 168 "Waitstates.MPI.pattern"

    typedef std::map<Event, double, EventKeyCompare> eventmap_t;

    eventmap_t m_pw;
  
};


/*----- Callback methods -----*/

void PatternMPI_Wait_Indirect_LR::delay_late_receiver_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 179 "Waitstates.MPI.pattern"

      // Shortcuts
      const TimeMapSum& d(data->mDelayInfo);
      const TimeMapSum& w(data->mWaitInfo);

      if (w.mSum > 0) {
        double scale = std::min(data->mRwait / (d.mSum + w.mSum), 1.0);
        m_pw[event] += scale * w.mSum;
      }
    
}

void PatternMPI_Wait_Indirect_LR::post_inv_sendcmp_fwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 199 "Waitstates.MPI.pattern"

      double pw = data->mInvRemote->get_buffer(BUFFER_PROPWAIT)->get_double();

      if (pw > 0.0)
        m_severity[event.get_cnode()] += pw;
    
}

void PatternMPI_Wait_Indirect_LR::pre_inv_recvreq_fwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 191 "Waitstates.MPI.pattern"

      eventmap_t::iterator it = m_pw.find(event);
      
      Buffer* buf = new Buffer(16);
      buf->put_double(it == m_pw.end() ? 0.0 : it->second);

      data->mInvLocal->add_buffer(buf, BUFFER_PROPWAIT);
    
}

void PatternMPI_Wait_Indirect_LR::rpl_resume_fwd__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 174 "Waitstates.MPI.pattern"

      m_pw.clear();
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_Wait_PT
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_Wait_PT : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(RPL_RESUME_FWD, PEARL_create_callback(this, &PatternMPI_Wait_PT::rpl_resume_fwd__cb));
      }
      {
        CallbackManagerMap::iterator it = cbmgrs.find("bwc");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(DELAY_LATE_SENDER, PEARL_create_callback(this, &PatternMPI_Wait_PT::delay_late_sender_bwc_cb));
        (it->second)->register_callback(DELAY_LATE_RECEIVER, PEARL_create_callback(this, &PatternMPI_Wait_PT::delay_late_receiver_bwc_cb));
      }
      {
        CallbackManagerMap::iterator it = cbmgrs.find("fwc");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(GROUP_RECV, PEARL_create_callback(this, &PatternMPI_Wait_PT::group_recv_fwc_cb));
        (it->second)->register_callback(MPI_SEND, PEARL_create_callback(this, &PatternMPI_Wait_PT::mpi_send_fwc_cb));
        (it->second)->register_callback(MPI_SEND_COMPLETE, PEARL_create_callback(this, &PatternMPI_Wait_PT::mpi_send_complete_fwc_cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_WAIT_PROPAGATING;
    }

    virtual long get_parent() const
    {
      return PAT_WAITSTATES_PROPAGATING_VS_TERMINAL;
    }

    virtual string get_name() const
    {
      return "Propagating MPI point-to-point wait states";
    }

    virtual string get_unique_name() const
    {
      return "mpi_wait_propagating";
    }

    virtual string get_descr() const
    {
      return "Aggregate amount of MPI point-to-point waiting time that propagates further";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    virtual bool is_hidden() const
    {
      return true;
    }

    /* Callback methods */
    void delay_late_receiver_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void delay_late_sender_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void group_recv_fwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void mpi_send_complete_fwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void mpi_send_fwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void rpl_resume_fwd__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);


  private:

#line 361 "Waitstates.MPI.pattern"

    typedef std::map<Event, double, EventKeyCompare> eventmap_t;

    eventmap_t m_pw;
  
};


/*----- Callback methods -----*/

void PatternMPI_Wait_PT::delay_late_receiver_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 398 "Waitstates.MPI.pattern"

      // Shortcuts
      const TimeMapSum& d(data->mDelayInfo);
      const TimeMapSum& w(data->mWaitInfo);

      if (!(w.mSum > 0))
        return;

      double scale = std::min(data->mRwait / (d.mSum + w.mSum), 1.0);

      if (!(scale > 0))
        return;

      // --- update maximum propagating wait time for wait states in current sync interval

      for (SynchpointInfoMap::const_iterator it = data->mPropSpMap.begin(); it != data->mPropSpMap.end(); ++it) {
        if (it->second.waitTime > 0) {
          eventmap_t::iterator pwit = m_pw.find(it->first);

          if (pwit == m_pw.end())
            m_pw.insert(std::make_pair(it->first, scale * it->second.waitTime));
          else
            pwit->second = std::max(scale * it->second.waitTime, pwit->second);
        }
      }
    
}

void PatternMPI_Wait_PT::delay_late_sender_bwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 372 "Waitstates.MPI.pattern"

      // Shortcuts
      const TimeMapSum& d(data->mDelayInfo);
      const TimeMapSum& w(data->mWaitInfo);

      if (!(w.mSum > 0))
        return;

      double scale = std::min(data->mRwait / (d.mSum + w.mSum), 1.0);

      if (!(scale > 0))
        return;

      // --- update maximum propagating wait time for wait states in current sync interval

      for (SynchpointInfoMap::const_iterator it = data->mPropSpMap.begin(); it != data->mPropSpMap.end(); ++it) {
        if (it->second.waitTime > 0) {
          eventmap_t::iterator pwit = m_pw.find(it->first);

          if (pwit == m_pw.end())
            m_pw.insert(std::make_pair(it->first, scale * it->second.waitTime));
          else
            pwit->second = std::max(scale * it->second.waitTime, pwit->second);
        }
      }
    
}

void PatternMPI_Wait_PT::group_recv_fwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 713 "Waitstates.MPI.pattern"

      eventmap_t::iterator pwmit = m_pw.find(event);

      if (pwmit != m_pw.end()) {
        data->mIdle = pwmit->second;
        cbmanager.notify(WAITSTATE_PROP_LS, event, data);
      }
    
}

void PatternMPI_Wait_PT::mpi_send_complete_fwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 729 "Waitstates.MPI.pattern"

      eventmap_t::iterator pwmit = m_pw.find(event);

      if (pwmit != m_pw.end()) {
        data->mIdle = pwmit->second;
        cbmanager.notify(WAITSTATE_PROP_LR, event, data);
      }
    
}

void PatternMPI_Wait_PT::mpi_send_fwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 721 "Waitstates.MPI.pattern"

      eventmap_t::iterator pwmit = m_pw.find(event);

      if (pwmit != m_pw.end()) {
        data->mIdle = pwmit->second;
        cbmanager.notify(WAITSTATE_PROP_LR, event, data);
      }
    
}

void PatternMPI_Wait_PT::rpl_resume_fwd__cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 367 "Waitstates.MPI.pattern"

      m_pw.clear();
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class Pattern_Wait_PropagatingLS
 *
 *---------------------------------------------------------------------------
 */


class Pattern_Wait_PropagatingLS : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("fwc");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(WAITSTATE_PROP_LS, PEARL_create_callback(this, &Pattern_Wait_PropagatingLS::waitstate_prop_ls_fwc_cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_WAIT_PROPAGATING_LS;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_WAIT_PROPAGATING;
    }

    virtual string get_name() const
    {
      return "Propagating MPI Late Sender wait states";
    }

    virtual string get_unique_name() const
    {
      return "mpi_wait_propagating_ls";
    }

    virtual string get_descr() const
    {
      return "MPI Late Sender wait states which cause further waiting time on other processes";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void waitstate_prop_ls_fwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void Pattern_Wait_PropagatingLS::waitstate_prop_ls_fwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 776 "Waitstates.MPI.pattern"

      m_severity[event.get_cnode()] += data->mIdle;
    
}

#endif   /* _MPI */

#if defined(_MPI)
/*
 *---------------------------------------------------------------------------
 *
 * class Pattern_Wait_PropagatingLR
 *
 *---------------------------------------------------------------------------
 */


class Pattern_Wait_PropagatingLR : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManagerMap& cbmgrs)
    {
      init();

      {
        CallbackManagerMap::iterator it = cbmgrs.find("fwc");

        assert(it != cbmgrs.end());

        (it->second)->register_callback(WAITSTATE_PROP_LR, PEARL_create_callback(this, &Pattern_Wait_PropagatingLR::waitstate_prop_lr_fwc_cb));
      }
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_WAIT_PROPAGATING_LR;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_WAIT_PROPAGATING;
    }

    virtual string get_name() const
    {
      return "Propagating MPI Late Receiver wait states";
    }

    virtual string get_unique_name() const
    {
      return "mpi_wait_propagating_lr";
    }

    virtual string get_descr() const
    {
      return "MPI Late Receiver wait states which cause further waiting time on other processes";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual CubeMetricType get_mode() const
    {
      return CUBE_METRIC_EXCLUSIVE;
    }

    /* Callback methods */
    void waitstate_prop_lr_fwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void Pattern_Wait_PropagatingLR::waitstate_prop_lr_fwc_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 797 "Waitstates.MPI.pattern"

      m_severity[event.get_cnode()] += data->mIdle;
    
}

#endif   /* _MPI */

//--- Implementation --------------------------------------------------------

void scout::create_patterns(AnalyzeTask* analyzer)
{
  analyzer->addPattern(new PatternTime());
  analyzer->addPattern(new PatternVisits());
#if defined(_MPI)
  if (enableAsynchronous)
    analyzer->addPattern(new PatternAsync());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_Init());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_InitCompletion());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_FinalizeWait());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_BarrierWait());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_BarrierCompletion());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_P2P());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_LateSender());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_LateSenderWO());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_LswoDifferent());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_LswoSame());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_LateReceiver());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_EarlyReduce());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_EarlyScan());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_LateBroadcast());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_WaitNxN());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_NxNCompletion());
#endif   /* _MPI */
#if defined(_OPENMP)
  analyzer->addPattern(new PatternOMP_Mgmt());
#endif   /* _OPENMP */
#if defined(_OPENMP)
  analyzer->addPattern(new PatternOMP_Mgmt_Fork());
#endif   /* _OPENMP */
#if defined(_OPENMP)
  analyzer->addPattern(new PatternOMP_Sync_Barrier());
#endif   /* _OPENMP */
#if defined(_OPENMP)
  analyzer->addPattern(new PatternOMP_Sync_Ebarrier_Wait());
#endif   /* _OPENMP */
#if defined(_OPENMP)
  analyzer->addPattern(new PatternOMP_Sync_Ibarrier_Wait());
#endif   /* _OPENMP */
#if defined(_OPENMP)
  analyzer->addPattern(new PatternOMP_Lock_Contention_Critical());
#endif   /* _OPENMP */
#if defined(_OPENMP)
  analyzer->addPattern(new PatternOMP_Lock_Contention_API());
#endif   /* _OPENMP */
  analyzer->addPattern(new PatternPthread_Lock_Contention_Mutex_Lock());
  analyzer->addPattern(new PatternPthread_Lock_Contention_Conditional());
#if defined(_OPENMP)
  analyzer->addPattern(new PatternThread_LockContention());
#endif   /* _OPENMP */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_SyncsSend());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_SyncsRecv());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_SyncsColl());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_CommsSend());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_CommsRecv());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_CommsExch());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_CommsSrc());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_CommsDst());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_BytesSent());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_BytesRcvd());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_BytesCout());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_BytesCin());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_BytesPut());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_BytesGet());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_SLR_Count());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_SLS_Count());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_SLSWO_Count());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_CLR_Count());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_CLS_Count());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_CLSWO_Count());
#endif   /* _MPI */
#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
  analyzer->addPattern(new PatternMPI_RmaWaitAtCreate());
#endif   /* _MPI && HAS_MPI2_1SIDED */
#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
  analyzer->addPattern(new PatternMPI_RmaWaitAtFree());
#endif   /* _MPI && HAS_MPI2_1SIDED */
#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
  analyzer->addPattern(new PatternMPI_RmaSync());
#endif   /* _MPI && HAS_MPI2_1SIDED */
#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
  analyzer->addPattern(new PatternMPI_RmaSyncLatePost());
#endif   /* _MPI && HAS_MPI2_1SIDED */
#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
  analyzer->addPattern(new PatternMPI_RmaEarlyWait());
#endif   /* _MPI && HAS_MPI2_1SIDED */
#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
  analyzer->addPattern(new PatternMPI_RmaLateComplete());
#endif   /* _MPI && HAS_MPI2_1SIDED */
#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
  analyzer->addPattern(new PatternMPI_RmaWaitAtFence());
#endif   /* _MPI && HAS_MPI2_1SIDED */
#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
  analyzer->addPattern(new PatternMPI_RmaEarlyFence());
#endif   /* _MPI && HAS_MPI2_1SIDED */
#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
  analyzer->addPattern(new PatternMpi_RmaSyncLockContention());
#endif   /* _MPI && HAS_MPI2_1SIDED */
#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
  analyzer->addPattern(new PatternMpi_RmaSyncWaitForProgress());
#endif   /* _MPI && HAS_MPI2_1SIDED */
#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
  analyzer->addPattern(new PatternMPI_RmaCommLatePost());
#endif   /* _MPI && HAS_MPI2_1SIDED */
#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
  analyzer->addPattern(new PatternMpi_RmaCommLockContention());
#endif   /* _MPI && HAS_MPI2_1SIDED */
#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
  analyzer->addPattern(new PatternMpi_RmaCommWaitForProgress());
#endif   /* _MPI && HAS_MPI2_1SIDED */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_RmaPairsyncCount());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_RmaPairsyncUnneededCount());
#endif   /* _MPI */
  if (enableStatistics)
    analyzer->addPattern(new PatternStatistics());
  if (enableCriticalPath)
    analyzer->addPattern(new PatternCriticalPath());
  analyzer->addPattern(new PatternCriticalPathActivities());
  analyzer->addPattern(new PatternCriticalImbalanceImpact());
  analyzer->addPattern(new PatternInterPartitionImbalance());
  analyzer->addPattern(new PatternNonCriticalPathActivities());
#if defined(_MPI)
  if (enableDelayAnalysis)
    analyzer->addPattern(new PatternDelay_LateSender());
#endif   /* _MPI */
#if defined(_MPI)
  if (enableDelayAnalysis)
    analyzer->addPattern(new PatternDelay_LateSender_LT());
#endif   /* _MPI */
#if defined(_MPI)
  if (enableDelayAnalysis)
    analyzer->addPattern(new PatternDelay_LateReceiver());
#endif   /* _MPI */
#if defined(_MPI)
  if (enableDelayAnalysis)
    analyzer->addPattern(new PatternDelay_LateReceiver_LT());
#endif   /* _MPI */
#if defined(_MPI)
  if (enableDelayAnalysis)
    analyzer->addPattern(new PatternDelay_Barrier());
#endif   /* _MPI */
#if defined(_MPI)
  if (enableDelayAnalysis)
    analyzer->addPattern(new PatternDelay_Barrier_LT());
#endif   /* _MPI */
#if defined(_MPI)
  if (enableDelayAnalysis)
    analyzer->addPattern(new PatternDelay_NxN());
#endif   /* _MPI */
#if defined(_MPI)
  if (enableDelayAnalysis)
    analyzer->addPattern(new PatternDelay_NxN_LT());
#endif   /* _MPI */
#if defined(_MPI)
  if (enableDelayAnalysis)
    analyzer->addPattern(new PatternDelay_12N());
#endif   /* _MPI */
#if defined(_MPI)
  if (enableDelayAnalysis)
    analyzer->addPattern(new PatternDelay_12N_LT());
#endif   /* _MPI */
#if defined(_OPENMP)
  if (enableDelayAnalysis)
    analyzer->addPattern(new PatternDelay_OmpBarrier());
#endif   /* _OPENMP */
#if defined(_MPI)
  if (enableDelayAnalysis)
    analyzer->addPattern(new PatternDelay_OmpBarrier_LT());
#endif   /* _MPI */
#if defined(_OPENMP)
  if (enableDelayAnalysis)
    analyzer->addPattern(new PatternDelay_OmpIdle());
#endif   /* _OPENMP */
#if defined(_MPI)
  if (enableDelayAnalysis)
    analyzer->addPattern(new PatternDelay_OmpIdle_LT());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_Wait_Indirect_LS());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_Wait_Indirect_LR());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new PatternMPI_Wait_PT());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new Pattern_Wait_PropagatingLS());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->addPattern(new Pattern_Wait_PropagatingLR());
#endif   /* _MPI */
}


