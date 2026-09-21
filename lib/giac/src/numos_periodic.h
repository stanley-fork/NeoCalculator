// SPDX-License-Identifier: GPL-3.0-or-later
// NumOS producer observation, scoped to the existing serialized Giac boundary.
#ifndef NUMOS_GIAC_PERIODIC_H
#define NUMOS_GIAC_PERIODIC_H
#include "gen.h"
namespace giac {
// Runs the public solve unchanged. Records parameters at the three real trig
// producers, quotes them before use, and restores quotes/global solve counters
// on every exit. Never assigns, purges or changes an assumption.
gen numos_periodic_solve(const gen& args, vecteur& integer_parameters,
                        bool& overflow, const context* contextptr);
}
#endif
