/*
 * This file is part of the OpenKinect Project. http://www.openkinect.org
 *
 * Copyright (c) 2026 individual OpenKinect contributors. See the CONTRIB file
 * for details.
 *
 * This code is licensed to you under the terms of the Apache License, version
 * 2.0, or, at your option, the terms of the GNU General Public License,
 * version 2.0. See the APACHE20 and GPL2 files for the text of the licenses,
 * or the following URLs:
 * http://www.apache.org/licenses/LICENSE-2.0
 * http://www.gnu.org/licenses/gpl-2.0.txt
 *
 * If you redistribute this file in source form, modified or unmodified, you
 * may:
 *   1) Leave this header intact and distribute it under the same terms,
 *      accompanying it with the APACHE20 and GPL20 files, or
 *   2) Delete the Apache 2.0 clause and accompany it with the GPL2 file, or
 *   3) Delete the GPL v2 clause and accompany it with the APACHE20 file
 * In all cases you must keep the copyright notice intact and include a copy
 * of the CONTRIB file.
 *
 * Binary distributions must follow the binary distribution requirements of
 * either License.
 */

#pragma once

/*
 * Depth/colour frame phase alignment ("NudgeStreamTiming", §10C.4, §20.1.6,
 * §20.7.3). Kept free of device code so that it can be tested on the host.
 */

#include <stdint.h>

#define FN_TIMING_PERIOD_US 33333

/*
 * The value of GM command 0x22 [v, 1, 1] that shifts the depth stream by
 * nudge_us (Microsoft's driver, FUN_00011ee8). k2 = (int)(f2 * 1000) and
 * k3 = (int)f3 from the timing floats of command 0x16 parameter 6.
 * Returns -1 without timing floats (k2 == 0).
 */
static inline int fn_timing_nudge_value(int32_t nudge_us, int32_t k2, int32_t k3)
{
	uint32_t v;
	if (k2 <= 0)
		return -1;
	v = (uint32_t)(nudge_us - k3 + FN_TIMING_PERIOD_US) % FN_TIMING_PERIOD_US;
	v = (uint32_t)((uint64_t)v * 1000 / (uint32_t)k2);
	if (v < 25)
		v = 25;
	if (v > 2047)
		v = 2047;
	return (int)v;
}

/*
 * Kinect10's phase estimate (FUN_180049160). Feed it the midpoint of every
 * colour and depth frame (host time in µs, depth already 3600 µs earlier, as
 * the driver stamps it). After more than 5 overlapping samples it gives the
 * nudge that brings depth into phase with colour, and starts over.
 */
typedef struct {
	int64_t D, Dp, C, Cp; // latest and previous depth and colour midpoints
	int have_d, have_c;   // midpoints seen of each (counts up to 2)
	int count;
	int64_t min_diff;
} fn_phase_state;

static inline void fn_phase_reset(fn_phase_state *s)
{
	s->D = s->Dp = s->C = s->Cp = 0;
	s->have_d = s->have_c = 0;
	s->count = 0;
	s->min_diff = INT64_MAX;
}

// Returns 1 and sets *nudge_us when a nudge is due, 0 otherwise. rate is the
// colour frame rate (30, 15 or 12; anything else disables the estimate).
static inline int fn_phase_update(fn_phase_state *s, int is_depth, int64_t mid_us, int rate, int32_t *nudge_us)
{
	int64_t diff, t, period;
	int sample = 0;
	if (rate != 30 && rate != 15 && rate != 12)
		return 0;
	period = 1000000 / rate;
	if (is_depth) {
		s->Dp = s->D;
		s->D = mid_us;
		if (s->have_d < 2) s->have_d++;
	} else {
		s->Cp = s->C;
		s->C = mid_us;
		if (s->have_c < 2) s->have_c++;
	}
	if (s->have_d < 2 || s->have_c < 2)
		return 0;

	if (s->Cp <= s->D && s->D <= s->C) {
		diff = s->C - s->D;
		t = s->Cp + period;
		if (s->D < t && t - s->D < diff)
			diff = t - s->D;
		sample = 1;
	} else if (s->Dp <= s->C && s->C <= s->D) {
		diff = s->C - s->Dp;
		t = s->C + period;
		if (s->D < t && t - s->D < diff)
			diff = t - s->D;
		sample = 1;
	}
	if (!sample)
		return 0;
	s->count++;
	if (diff < s->min_diff)
		s->min_diff = diff;
	if (s->count <= 5)
		return 0;

	{
		int64_t m = s->min_diff % FN_TIMING_PERIOD_US;
		int valid = 1;
		if (m == 0)
			valid = 0;
		else if (m < 20000)
			*nudge_us = (int32_t)(m < 1400 ? m : 1400);
		else
			*nudge_us = (int32_t)(m > 32000 ? m : 32000);
		fn_phase_reset(s);
		return valid;
	}
}
