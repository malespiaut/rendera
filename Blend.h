/*
Copyright (c) 2014 Joe Davisson.

This file is part of Rendera.

Rendera is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Rendera is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Rendera; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
*/

#ifndef BLEND_H
#define BLEND_H

#include "rendera.h"

class Blend
{
public:
  Blend();
  virtual ~Blend();

  int current(int, int, int);
  int invert(int, int, int);
  int trans(int, int, int);
  int add(int, int, int);
  int sub(int, int, int);
  int colorize(int, int, int);
  int force_lum(int, int);
  void hsv_to_rgb(int, int, int, int *, int *, int *);
  void rgb_to_hsv(int, int, int, int *, int *, int *);

  int current_blend;
};

#endif
