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

#ifndef MAP_H
#define MAP_H

#include "rendera.h"

class Map
{
public:
  Map(int, int);
  virtual ~Map();

  int w, h;
  unsigned char *data;
  unsigned char *row;

  void clear(int);
  void setpixel(int, int, int);
  int getpixel(int, int);
  void line(int, int, int, int, int);
  void oval(int, int, int, int, int);
  void ovalfill(int, int, int, int, int);
  void rect(int, int, int, int, int);
  void rectfill(int, int, int, int, int);
  void hline(int, int, int, int);
  void vline(int, int, int, int);
};

#endif