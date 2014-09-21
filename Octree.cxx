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

#include "Octree.H"

Octree::Octree()
{
  root = new node_t;
  root->value = 0;

  int i;

  for(i = 0; i < 8; i++)
    root->child[i] = 0;
}

Octree::~Octree()
{
  clear(root);
}

// recursively delete octree node
void Octree::clear(struct node_t *node)
{
  int i;

  for(i = 0; i < 8; i++)
    if(node->child[i])
      clear(node->child[i]);

  delete node;
}

// writes value to leaf node
void Octree::write(const int &r, const int &g, const int &b,
                   const float &value)
{
  struct node_t *node = root;
  int i, j;

  for(i = 7; i >= 0; i--)
  {
    const int index = ((r >> i) & 1) << 0 |
                      ((g >> i) & 1) << 1 |
                      ((b >> i) & 1) << 2;

    if(!node->child[index])
    {
      node->child[index] = new node_t;
      node = node->child[index];
      node->value = 0;

      for(j = 0; j < 8; j++)
        node->child[j] = 0;
    }
    else
    {
      node = node->child[index];
    }
  }

  node->value = value;
}

// sets entire path to value (used by palette lookup)
void Octree::writePath(const int &r, const int &g, const int &b,
                        const float &value)
{
  struct node_t *node = root;
  int i, j;

  for(i = 7; i >= 0; i--)
  {
    const int index = ((r >> i) & 1) << 0 |
                      ((g >> i) & 1) << 1 |
                      ((b >> i) & 1) << 2;

    if(!node->child[index])
    {
      node->child[index] = new node_t;
      node = node->child[index];
      node->value = value;

      for(j = 0; j < 8; j++)
        node->child[j] = 0;
    }
    else
    {
      node = node->child[index];
    }
  }
}

// read value at leaf node
float Octree::read(const int &r, const int &g, const int &b)
{
  struct node_t *node = root;
  int i;

  for(i = 7; i >= 0; i--)
  {
    const int index = ((r >> i) & 1) << 0 |
                      ((g >> i) & 1) << 1 |
                      ((b >> i) & 1) << 2;

    if(node->child[index])
      node = node->child[index];
    else
      break;
  }

  return node->value;
}

