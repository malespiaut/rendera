/*
Copyright (c) 2024 Joe Davisson.

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

#ifndef SELECTION_H
#define SELECTION_H

#include "Tool.H"

class Stroke;
class View;

class Selection : public Tool
{
public:
  Selection();
  ~Selection();

  void push(View *);
  void drag(View *);
  void release(View *);
  void move(View *);
  void key(View *);
  void done(View *, int);
  void redraw(View *);
  bool isActive();
  void reset();
  void reload();

private:
  int beginx, beginy, lastx, lasty;
  int state;
  bool drag_started;
  bool resize_started;
  int side;
  int offsetx;
  int offsety;

  bool inbox(int, int, int, int, int, int);
  void absrect(View *, int *, int *, int *, int *);
  void drawHandles(View *, Stroke *, int, int, int, int);
  void copy(View *);
  void crop(View *);
  void paste(View *);
};

#endif

