/*
Copyright (c) 2023 Joe Davisson.

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

#include <vector>
#include <algorithm>

#include "Blend.H"
#include "Bitmap.H"
#include "Dialog.H"
#include "Gamma.H"
#include "Gui.H"
#include "Inline.H"
#include "Octree.H"
#include "Palette.H"
#include "Project.H"
#include "Quantize.H"
#include "View.H"
#include "Widget.H"

void Quantize::makeColor(color_type *c,
                 const float r, const float g, const float b, const float freq)
{
  c->r = r;
  c->g = g;
  c->b = b;
  c->freq = freq;
  c->active = true;
}

// compute quantization error
float Quantize::error(color_type *c1, color_type *c2)
{
  const float r = c1->r - c2->r;
  const float g = c1->g - c2->g;
  const float b = c1->b - c2->b;

  return ((c1->freq * c2->freq) / (c1->freq + c2->freq)) *
          (r * r + g * g + b * b);
}

// merge two colors
void Quantize::merge(color_type *c1, color_type *c2)
{
  const float mul = 1.0f / (c1->freq + c2->freq);

  c1->r = (c1->freq * c1->r + c2->freq * c2->r) * mul;
  c1->g = (c1->freq * c1->g + c2->freq * c2->g) * mul;
  c1->b = (c1->freq * c1->b + c2->freq * c2->b) * mul;
  c1->freq += c2->freq;  
}

// reduces color count by averaging sections of the color cube
int Quantize::limitColors(Octree *histogram, color_type *colors, int step)
{
  int count = 0;

  for(int b = 0; b <= 256 - step; b += step)
  {
    for(int g = 0; g <= 256 - step; g += step)
    {
      for(int r = 0; r <= 256 - step; r += step)
      {
        float rr = 0;
        float gg = 0;
        float bb = 0;
        float div = 0;

        for(int k = 0; k < step; k++)
        {
          const int bk = b + k;

          for(int j = 0; j < step; j++)
          {
            const int gj = g + j;

            for(int i = 0; i < step; i++)
            {
              const int ri = r + i;
              const float d = histogram->read(ri, gj, bk);

              if(d > 0)
                histogram->write(ri, gj, bk, 0);

              rr += d * ri;
              gg += d * gj;
              bb += d * bk;
              div += d;
            }
          }
        }

        if(div > 0)
        {
          rr /= div;
          gg /= div;
          bb /= div;
          makeColor(&colors[count], rr, gg, bb, div);
          count++;
        }
      }
    }
  }

  return count;
}

// stretch a palette to obtain the exact number of colors desired
/*
void Quantize::stretchPalette(int *data, int current, int target)
{
  std::vector<int> temp(target);

  const float ax = (float)(current - 1) / (float)(target - 1);
  int *c[2];

  c[0] = c[1] = &data[0];

  for(int x = 0; x < target; x++)
  {
    float uu = (x * ax);
    int u1 = uu;

    if(u1 > current - 1)
      u1 = current - 1;

    int u2 = (u1 < (current - 1) ? u1 + 1 : u1);
    float u = uu - u1;

    c[0] += u1;
    c[1] += u2;

    float f[2];

    f[0] = (1.0 - u);
    f[1] = u;

    float r = 0, g = 0, b = 0;

    for(int i = 0; i < 2; i++)
    {
      r += (float)getr(*c[i]) * f[i];
      g += (float)getg(*c[i]) * f[i];
      b += (float)getb(*c[i]) * f[i];
    }

    temp[x] = makeRgb((int)r, (int)g, (int)b);

    c[0] -= u1;
    c[1] -= u2;
  }

  for(int x = 0; x < target; x++)
    data[x] = temp[x];
}
*/

// Pairwise clustering quantization, adapted from the algorithm described here:
//
// http://www.visgraf.impa.br/Projects/quantization/quant.html
// http://www.visgraf.impa.br/sibgrapi97/anais/pdf/art61.pdf
//
void Quantize::pca(Bitmap *src, Palette *pal, int size)
{
  // popularity histogram
  Octree histogram;

  int max;
  int rep = size;

  // build histogram, inc is the weight of 1 pixel in the image
  float inc = 1.0 / (src->cw * src->ch);
  int count = 0;

  for(int j = src->ct; j <= src->cb; j++)
  {
    int *p = src->row[j] + src->cl;

    for(int i = src->cl; i <= src->cr; i++)
    {
      rgba_type rgba = getRgba(*p++);
      float freq = histogram.read(rgba.r, rgba.g, rgba.b);

      if(freq < inc)
        count++;

      histogram.write(rgba.r, rgba.g, rgba.b, freq + inc);
    }
  }

  // color list
  std::vector<color_type> colors(4096);

  for(int i = 0; i < 4096; i++)
    colors[i].active = false;

  // quantization error matrix
  std::vector<float> err_data(((4096 + 1) * 4096) / 2);

  // skip if already enough colors
  if(count <= rep)
  {
    count = 0;

    for(int i = 0; i < 16777216; i++)
    {
      rgba_type rgba = getRgba(i);
      const float freq = histogram.read(rgba.r, rgba.g, rgba.b);

      if(freq > 0)
      {
        makeColor(&colors[count], rgba.r, rgba.g, rgba.b, freq);
        count++;
      }
    }
  }
  else
  {
    count = limitColors(&histogram, &colors[0], 16);
  }

  // set max
  max = count;
  if(max < rep)
    rep = max;

  // init error matrix
  for(int j = 0; j < max; j++)
  {
    for(int i = 0; i < j; i++)
      err_data[i + (j + 1) * j / 2] = error(&colors[i], &colors[j]);
  }

  Gui::progressShow(count - rep);

  // measure offset between array elements
  const int step = &(colors[1].active) - &(colors[0].active);

  while(count > rep)
  {
    int ii = 0, jj = 0;
    float least_err = 999999;
    bool *a = &(colors[0].active);

    // find lowest value in error matrix
    for(int j = 0; j < max; j++, a += step)
    {
      if(*a)
      {
        float *e = &err_data[(j + 1) * j / 2];
        bool *b = &(colors[0].active);

        for(int i = 0; i < j; i++, e++, b += step)
        {
          if(*b && (*e < least_err))
          {
            least_err = *e;
            ii = i;
            jj = j;
          }
        }
      }
    }

    // compute quantization level and place in i, delete j
    merge(&colors[ii], &colors[jj]);
    colors[jj].active = false;
    count--;

    // recompute error matrix for new row
    for(int j = ii; j < max; j++)
    {
      if(colors[j].active)
        err_data[ii + (j + 1) * j / 2] = error(&colors[ii], &colors[j]);
    }

    // user cancelled operation
    if(Fl::get_key(FL_Escape))
      return;

    Gui::progressUpdate(count);
  }

  Gui::progressHide();

  // build palette
  int index = 0;

  for(int i = 0; i < max; i++)
  {
    if(colors[i].active)
    {
      pal->data[index] =
        makeRgb((int)colors[i].r, (int)colors[i].g, (int)colors[i].b);

      index++;
    }
  }

  pal->max = index;

/*
  // stretch palette
  if(pal->max != size)
  {
    stretchPalette(pal->data, pal->max, size);
    pal->max = size;
  }
*/

  // redraw palette widget
  Gui::paletteDraw();
  Project::palette->fillTable();
}

