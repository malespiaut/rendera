#include "rendera.h"

Blend::Blend()
{
  current_blend = 0;
}

Blend::~Blend()
{
}

int Blend::current(int c1, int c2, int t)
{
  switch(current_blend)
  {
    case 0:
      return trans(c1, c2, t);
  }

  return 0;
}

int Blend::invert(int c1, int c2, int t)
{
  return makecol(255 - getr(c1), 255 - getg(c1), 255 - getb(c1));
}

int Blend::trans(int c1, int c2, int t)
{
  int r = ((getr(c1) * t) + (getr(c2) * (255 - t))) / 255;
  int g = ((getg(c1) * t) + (getg(c2) * (255 - t))) / 255;
  int b = ((getb(c1) * t) + (getb(c2) * (255 - t))) / 255;

  return makecol(r, g, b);
}

int Blend::add(int c1, int c2, int t)
{
  t = 255 - t;

  int r = getr(c2);
  int g = getg(c2);
  int b = getb(c2);

  c2 = makecol(r, g, b);

  r = getr(c1) + (getr(c2) * t) / 255;
  g = getg(c1) + (getg(c2) * t) / 255;
  b = getb(c1) + (getb(c2) * t) / 255;

  r = MIN(r, 255);
  g = MIN(g, 255);
  b = MIN(b, 255);

  return makecol(r, g, b);
}

int Blend::sub(int c1, int c2, int t)
{
  int r = getr(c2);
  int g = getg(c2);
  int b = getb(c2);
  int h = 0, s = 0, v = 0;

  rgb_to_hsv(r, g, b, &h, &s, &v);
  h += 768;
  if(h >= 1536)
        h -= 1536;
  hsv_to_rgb(h, s, v, &r, &g, &b);
  c2 = makecol(r, g, b);

  t = 255 - t;
  r = getr(c1) - getr(c2) * t / 255;
  g = getg(c1) - getg(c2) * t / 255;
  b = getb(c1) - getb(c2) * t / 255;

  r = MAX(r, 0);
  g = MAX(g, 0);
  b = MAX(b, 0);

  return makecol(r, g, b);
}

int Blend::colorize(int c1, int c2, int t)
{
  int c3 = blend->trans(c1, c2, t);

  return force_lum(c3, getl(c1));
}

int Blend::force_lum(int c, int vdest)
{
  int i;
  int n[3];
  int vsrc = getl(c);

  n[0] = getg(c);
  n[1] = getr(c);
  n[2] = getb(c);

  while(vsrc < vdest)
  {
    for(i = 0; i < 3; i++)
    {
      if(n[i] < 255)
      {
        n[i]++;
        vsrc = getl(makecol(n[1], n[0], n[2]));
        if(vsrc == vdest)
          break;
      }
    }
  }

  while(vsrc > vdest)
  {
    for(i = 0; i < 3; i++)
    {
      if(n[i] > 0)
      {
        n[i]--;
        vsrc = getl(makecol(n[1], n[0], n[2]));
        if(vsrc == vdest)
          break;
      }
    }
  }

  return makecol(n[1], n[0], n[2]);
}

void Blend::hsv_to_rgb(int h, int s, int v, int *r, int *g, int *b)
{
  if(!s)
  {
    *r = *g = *b = v;
    return;
  }

  int i = h / 256;
  int f = (h & 255);

  int x = (v * (255 - s)) / 255;
  int y = (v * ((65025 - s * f) / 255)) / 255;
  int z = (v * ((65025 - s * (255 - f)) / 255)) / 255;

  switch(i)
  {
    case 6:
    case 0:
      *r = v;
      *g = z;
      *b = x;
      break;
    case 1:
      *r = y;
      *g = v;
      *b = x;
      break;
    case 2:
      *r = x;
      *g = v;
      *b = z;
      break;
    case 3:
      *r = x;
      *g = y;
      *b = v;
      break;
    case 4:
      *r = z;
      *g = x;
      *b = v;
      break;
    case 5:
      *r = v;
      *g = x;
      *b = y;
      break;
  }
}

void Blend::rgb_to_hsv(int r, int g, int b, int *h, int *s, int *v)
{
  int max = MAX(r, MAX(g, b));
  int min = MIN(r, MIN(g, b));
  int delta = max - min;

  *v = max;

  if(max)
    *s = (delta * 255) / max;
  else
    *s = 0;

  if(!*s)
  {
    *h = 0;
  }
  else
  {
    if(r == max)
      *h = ((g - b) * 255) / delta;
    else if(g == max)
      *h = 512 + ((b - r) * 255) / delta;
    else if(b == max)
      *h = 1024 + ((r - g) * 255) / delta;

    if(*h < 0)
      *h += 1536;
  }
}

