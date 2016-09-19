/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2014                                                     **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef PEARL_THREADEND_REP_H
#define PEARL_THREADEND_REP_H


#include <pearl/Event_rep.h>


/*-------------------------------------------------------------------------*/
/**
 *  @file    ThreadEnd_rep.h
 *  @ingroup PEARL_base
 *  @brief   Declaration of the class ThreadEnd_rep.
 *
 *  This header file provides the declaration of the class ThreadEnd_rep.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 *  @class   ThreadEnd_rep
 *  @ingroup PEARL_base
 *  @brief   %Event representation for THREAD_END events.
 **/
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR ThreadEnd_rep : public Event_rep
{
  public:
    /// @name Constructors & destructor
    /// @{

    ThreadEnd_rep(timestamp_t timestamp,
                  uint32_t    threadContingent,
                  uint64_t    sequenceCount);
    ThreadEnd_rep(const GlobalDefs& defs, Buffer& buffer);

    /// @}
    /// @name Event type information
    /// @{

    virtual event_t getType() const;
    virtual bool    isOfType(event_t type) const;

    /// @}
    /// @name Access event data
    /// @{

    uint64_t getSequenceCount() const;

    uint32_t getThreadContingent() const;

    /// @}


  protected:
    /// @name Serialize event data
    /// @{

    virtual void pack(Buffer& buffer) const;

    /// @}
    /// @name Generate human-readable output of event data
    /// @{

    virtual std::ostream& output(std::ostream& stream) const;

    /// @}


  private:
    /// Thread creation sequence count
    uint64_t mSequenceCount;
    
    /// Thread contingent/team id
    uint32_t mThreadContingent;
};


}   // namespace pearl


#endif   // !PEARL_THREADEND_REP_H
