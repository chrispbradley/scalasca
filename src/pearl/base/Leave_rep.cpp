/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2013                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>
#include <pearl/Leave_rep.h>

#include <iostream>

#include <pearl/Buffer.h>
#include <pearl/Callpath.h>
#include <pearl/GlobalDefs.h>
#include <pearl/Region.h>

using namespace std;
using namespace pearl;


//---------------------------------------------------------------------------
//
//  class Leave_rep
//
//---------------------------------------------------------------------------

//--- Constructors & destructor ---------------------------------------------

Leave_rep::Leave_rep(timestamp_t   timestamp,
                     const Region& region,
                     uint64_t*     metrics)
  : Event_rep(timestamp),
    mRegion(const_cast<Region*>(&region))
{
#ifdef PEARL_ENABLE_METRICS
  mMetrics = metrics;
#endif
}


Leave_rep::Leave_rep(const GlobalDefs& defs, Buffer& buffer)
  : Event_rep(defs, buffer)
{
  mCallpath = defs.get_cnode(buffer.get_id());
#ifdef PEARL_ENABLE_METRICS
  mMetrics = NULL;
#endif
}


Leave_rep::~Leave_rep()
{
#ifdef PEARL_ENABLE_METRICS
  delete[] mMetrics;
#endif
}


//--- Event type information ------------------------------------------------

event_t Leave_rep::getType() const
{
  return LEAVE;
}


bool Leave_rep::isOfType(event_t type) const
{
  return ((LEAVE == type)
          || (GROUP_ALL == type));
}


//--- Access event data -----------------------------------------------------

const Region& Leave_rep::getRegion() const
{
  return mCallpath->getRegion();
}


#ifdef PEARL_ENABLE_METRICS
uint64_t Leave_rep::getMetric(int index) const
{
  // FIXME: Validation of arguments missing.
  return mMetrics[index];
}
#endif


//--- Serialize event data (protected) --------------------------------------

void Leave_rep::pack(Buffer& buffer) const
{
  Event_rep::pack(buffer);

  buffer.put_uint32(mCallpath->getId());
}


//--- Generate human-readable output of event data (protected) --------------

ostream& Leave_rep::output(ostream& stream) const
{
  Event_rep::output(stream);

  // WARNING: This assumes that the trace has already been preprocessed!
  return stream << "  region = " << mCallpath->getRegion() << endl;
}


//--- Private methods -------------------------------------------------------

const Region& Leave_rep::getRegionLeft() const
{
  return *mRegion;
}


Callpath* Leave_rep::getCallpath() const
{
  return mCallpath;
}


void Leave_rep::setCallpath(Callpath* callpath)
{
  mCallpath = callpath;
}
