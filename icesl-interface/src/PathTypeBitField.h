/*
 *  Copyright (c) 2020, INRIA
 *  All rights reserved.
 *
 *  IceSL plugin interface
 *
 *  MFX Team
 */

// SL 2020-09-17
#pragma once

/// \brief The various types of paths that can be combined in a 32bits bit field
enum PathType 
{
  // bits
  e_Undefined      = 0,
  e_Travel         = (1<<1), 
  e_Perimeter      = (1<<2),
  e_Infill         = (1<<3),
  e_GapFill        = (1<<4),
  e_Shell          = (1<<5),
  e_Support        = (1<<6),
  e_Bridge         = (1<<7),
  e_Raft           = (1<<8), 
  e_Brim           = (1<<9), 
  e_Shield         = (1<<10), 
  e_Tower          = (1<<11),
  e_Visible        = (1<<12),
  e_FreeZipper     = (1<<13),
  e_NoRetract      = (1<<14),
  e_TravelAbove    = (1<<15),
  e_Cavity         = (1<<16),
  e_Cover          = (1<<17),
  e_PrimeAtStart   = (1<<18),
  e_Ironing        = (1<<19),
};
