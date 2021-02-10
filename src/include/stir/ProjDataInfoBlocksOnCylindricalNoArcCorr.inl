
/*
	Copyright 2018 ETH Zurich, Institute for Particle Physics and Astrophysics

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*!
  \file
  \ingroup projdata

  \brief Implementation of inline functions of class
	stir::ProjDataInfoBlocksOnCylindricalNoArcCorr

  \author Parisa Khateri

*/
#include "stir/Bin.h"
#include "stir/Succeeded.h"
#include "stir/LORCoordinates.h"
#include <math.h>

START_NAMESPACE_STIR

void
ProjDataInfoBlocksOnCylindricalNoArcCorr::
initialise_uncompressed_view_tangpos_to_det1det2_if_not_done_yet() const
{
  // for efficiency reasons, use "Double-Checked-Locking(DCL) pattern" with OpenMP atomic operation
  // OpenMP v3.1 or later required
  // thanks to yohjp: http://stackoverflow.com/questions/27975737/how-to-handle-cached-data-structures-with-multi-threading-e-g-openmp
#if defined(STIR_OPENMP) &&  _OPENMP >=201012
  bool initialised;
#pragma omp atomic read
  initialised = uncompressed_view_tangpos_to_det1det2_initialised;

  if (!initialised)
#endif
    {
#if defined(STIR_OPENMP)
#pragma omp critical(PROJDATAINFOCYLINDRICALNOARCCORR_VIEWTANGPOS_TO_DETS)
#endif
          {
            if (!uncompressed_view_tangpos_to_det1det2_initialised)
              initialise_uncompressed_view_tangpos_to_det1det2();
          }
    }
}

void
ProjDataInfoBlocksOnCylindricalNoArcCorr::
initialise_det1det2_to_uncompressed_view_tangpos_if_not_done_yet() const
{
  // as above
#if defined(STIR_OPENMP) &&  _OPENMP >=201012
  bool initialised;
#pragma omp atomic read
  initialised = det1det2_to_uncompressed_view_tangpos_initialised;

  if (!initialised)
#endif
    {
#if defined(STIR_OPENMP)
#pragma omp critical(PROJDATAINFOCYLINDRICALNOARCCORR_DETS_TO_VIEWTANGPOS)
#endif
          {
            if (!det1det2_to_uncompressed_view_tangpos_initialised)
              initialise_det1det2_to_uncompressed_view_tangpos();
          }
    }
}

/*! warning In cylindrical s is found from bin: sin(beta) = sin(tang_pos*angular_increment)
	In block it is calculated directly from corresponding lor
*/
float
ProjDataInfoBlocksOnCylindricalNoArcCorr::
get_s(const Bin& bin) const
{
	LORInAxialAndNoArcCorrSinogramCoordinates<float> lor;
	get_LOR(lor, bin);
	if (bin.view_num()==0 && lor.phi()>0.1)
		return -1*ring_radius * sin(lor.beta());
	return ring_radius * sin(lor.beta());
}

float
ProjDataInfoBlocksOnCylindricalNoArcCorr::
get_angular_increment() const
{
  return angular_increment;
}

void
ProjDataInfoBlocksOnCylindricalNoArcCorr::
get_det_num_pair_for_view_tangential_pos_num(
					     int& det1_num,
					     int& det2_num,
					     const int view_num,
					     const int tang_pos_num) const
{
  assert(get_view_mashing_factor() == 1);
	this->initialise_uncompressed_view_tangpos_to_det1det2_if_not_done_yet();

  det1_num = uncompressed_view_tangpos_to_det1det2[view_num][tang_pos_num].det1_num;
  det2_num = uncompressed_view_tangpos_to_det1det2[view_num][tang_pos_num].det2_num;
}

bool
ProjDataInfoBlocksOnCylindricalNoArcCorr::
get_view_tangential_pos_num_for_det_num_pair(int& view_num,
					     int& tang_pos_num,
					     const int det1_num,
					     const int det2_num) const
{
	assert(det1_num!=det2_num);
	this->initialise_det1det2_to_uncompressed_view_tangpos_if_not_done_yet();

  view_num =
    det1det2_to_uncompressed_view_tangpos[det1_num][det2_num].view_num/get_view_mashing_factor();
  tang_pos_num =
    det1det2_to_uncompressed_view_tangpos[det1_num][det2_num].tang_pos_num;
  return
    det1det2_to_uncompressed_view_tangpos[det1_num][det2_num].swap_detectors;
}

Succeeded
ProjDataInfoBlocksOnCylindricalNoArcCorr::
get_bin_for_det_pair(Bin& bin,
		     const int det_num1, const int ring_num1,
		     const int det_num2, const int ring_num2) const
{
  if (get_view_tangential_pos_num_for_det_num_pair(bin.view_num(), bin.tangential_pos_num(), det_num1, det_num2))
    return get_segment_axial_pos_num_for_ring_pair(bin.segment_num(), bin.axial_pos_num(), ring_num1, ring_num2);
  else
    return get_segment_axial_pos_num_for_ring_pair(bin.segment_num(), bin.axial_pos_num(), ring_num2, ring_num1);
}

Succeeded
ProjDataInfoBlocksOnCylindricalNoArcCorr::
get_bin_for_det_pos_pair(Bin& bin,
                         const DetectionPositionPair<>& dp) const
{
  return
    get_bin_for_det_pair(bin,
                         dp.pos1().tangential_coord(),
                         dp.pos1().axial_coord(),
		         		 dp.pos2().tangential_coord(),
                         dp.pos2().axial_coord());
}

void
ProjDataInfoBlocksOnCylindricalNoArcCorr::
get_det_pair_for_bin(
		     int& det_num1, int& ring_num1,
		     int& det_num2, int& ring_num2,
		     const Bin& bin) const
{
  get_det_num_pair_for_view_tangential_pos_num(det_num1, det_num2, bin.view_num(), bin.tangential_pos_num());
  get_ring_pair_for_segment_axial_pos_num( ring_num1, ring_num2, bin.segment_num(), bin.axial_pos_num());
}

void
ProjDataInfoBlocksOnCylindricalNoArcCorr::
get_det_pos_pair_for_bin(
		     DetectionPositionPair<>& dp,
		     const Bin& bin) const
{
  //lousy work around because types don't match TODO remove!
#if 1
  int t1=dp.pos1().tangential_coord(),
    a1=dp.pos1().axial_coord(),
    t2=dp.pos2().tangential_coord(),
    a2=dp.pos2().axial_coord();
  get_det_pair_for_bin(t1, a1, t2, a2, bin);
  dp.pos1().tangential_coord()=t1;
  dp.pos1().axial_coord()=a1;
  dp.pos2().tangential_coord()=t2;
  dp.pos2().axial_coord()=a2;

#else

  get_det_pair_for_bin(dp.pos1().tangential_coord(),
                       dp.pos1().axial_coord(),
		       dp.pos2().tangential_coord(),
                       dp.pos2().axial_coord(),
                       bin);
#endif
}

END_NAMESPACE_STIR
