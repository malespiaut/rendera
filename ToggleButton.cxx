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

#include "rendera.h"

ToggleButton::ToggleButton(Fl_Group *g, int x, int y, int w, int h, const char *label, const char *filename)
: Fl_Button(x, y, w, h, label)
{
  var = 0;
  group = g;
  image = new Fl_PNG_Image(filename);
  resize(group->x() + x, group->y() + y, w, h);
  tooltip(label);
}

ToggleButton::~ToggleButton()
{
}

int ToggleButton::handle(int event)
{
  switch(event)
  {
    case FL_PUSH:
      switch(Fl::event_button())
      {
        case 1:
          var = 1 - var;
          do_callback();
          redraw();
          return 1;
      }
  }

  return 0;
}

void ToggleButton::draw()
{
  if(var)
    image->draw(x() + 1, y() + 1);
  else
    image->draw(x(), y());
  fl_draw_box(var ? FL_DOWN_FRAME : FL_UP_FRAME, x(), y(), w(), h(), FL_BLACK);
}
