//
// $Id$
//
/*!

  \file
  \ingroup buildblock  
  \brief Declaration of class TimeFrameDefinitions
    
  \author Kris Thielemans
      
  $Date$
  $Revision$
*/
/*
    Copyright (C) 2003- $Date$, Hammersmith Imanet Ltd
    See STIR/LICENSE.txt for details
*/
#ifndef __stir_TimeFrameDefinitions_H__
#define __stir_TimeFrameDefinitions_H__

#include "stir/common.h"
#include <string>
#include <vector>
#include <utility>

#ifndef STIR_NO_NAMESPACES
using std::ifstream;
using std::string;
using std::pair;
using std::vector;
#endif

START_NAMESPACE_STIR
/*!
  \ingroup buildblock
  \brief Class used for storing time frame durations

  Times are supposed to be relative to the scan start time.

  Will probably be superseded by Study classes.
*/
class TimeFrameDefinitions
{
public:
  //! Default constructor: no time frames at all
  TimeFrameDefinitions();

  //! Read the frame definitions from a .fdef file
  /*! This uses the '.fdef' format used by Peter Bloomfield's software.
    The format is a number of lines, each existing of 2 numbers
  \verbatim
    num_frames_of_this_duration   duration_in_secs
  \endverbatim
  This duration is a double number.

  This class in fact allows an extension of the above. Setting 
  \a num_frames_of_this_duration to 0 allows skipping
  a time period of the corresponding \a duration_in_secs.
  */
  explicit TimeFrameDefinitions(const string& fdef_filename);
  
  //! Construct from a list of time frames
  /*! Times have to be in increasing order*/
  TimeFrameDefinitions(const vector<pair<double, double> >&);

  //! \name get info for 1 frame (frame_num is 1 based)
  //@{
  double get_start_time(unsigned int frame_num) const;
  double get_end_time(unsigned int frame_num) const;
  double get_duration(unsigned int frame_num) const;
  //@}

  //! Get start time of first frame
  double get_start_time() const;
  //! Get end time of last frame
  double get_end_time() const;
  
  unsigned int get_num_frames() const;
  
private:
  vector<pair<double, double> > frame_times;
};

END_NAMESPACE_STIR
#endif
