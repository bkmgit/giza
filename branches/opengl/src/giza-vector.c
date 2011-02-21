/* giza - a scientific plotting layer built on cairo
 *
 * This file is (or was) part of GIZA, a scientific plotting
 * layer built on cairo.
 * GIZA comes with ABSOLUTELY NO WARRANTY.
 * This is free software; and you are welcome to redistribute
 * it under the terms of the GNU General Public License
 * (see LICENSE file for details) and the provision that
 * this notice remains intact. If you modify this file, please
 * note section 2a) of the GPLv2 states that:
 *
 *  a) You must cause the modified files to carry prominent notices
 *     stating that you changed the files and the date of any change.
 *
 * Copyright (C) 2010 James Wetter. All rights reserved.
 * Contact: wetter.j@gmail.com
 *
 */

#include "giza-io-private.h"
#include "giza-private.h"
#include "giza-transforms-private.h"
#include <giza.h>
#include <math.h>

void
giza_vector (int n, int m, double horizontal[m][n], double vertical[m][n],
	     int i1, int i2, int j1, int j2, double scale, int position,
	     double affine[6], double blank)
{
  if (!_giza_check_device_ready ("giza_vector"))
    return;

  // Check ranges
  if (i1 < 0 || i2 >= n || i1 > i2)
    {
      _giza_error ("giza_vector",
		   "invalid index range for horizontal values");
      return;
    }
  if (j1 < 0 || j2 >= m || j1 > j2)
    {
      _giza_error ("giza_vector", "invalid index range for vertical values");
      return;
    }

  int i, j, oldBuf;
  double x1, x2, y1, y2;
  cairo_matrix_t mat;

  cairo_matrix_init (&mat, affine[0], affine[1], affine[2], affine[3],
                     affine[4], affine[5]); 

  double dscale = scale;
  // Find the scaling factor
  if (fabs (dscale) < GIZA_ZERO_DOUBLE)
    {
      for (j = j1; j <= j2; j++)
	{
	  for (i = i1; i <= i2; i++)
	    {
	      if (horizontal[j][i] != blank && vertical[j][i] != blank)
		{
		  double tmp =
		    sqrt (horizontal[j][i] * horizontal[j][i] +
			  vertical[j][i] * vertical[j][i]);
		  if (tmp > dscale)
		    dscale = tmp;
		}
	    }
	}
      if (fabs (dscale) < GIZA_ZERO_DOUBLE) { return; } 
      double dx2 = affine[0]*affine[0] + affine[1]*affine[1];
      double dy2 = affine[2]*affine[2] + affine[3]*affine[3];
      if (dx2 < dy2) 
        {
          dscale = sqrt(dx2)/dscale;
        } else {
          dscale = sqrt(dy2)/dscale;
        }
    }

  oldBuf = Sets.buf;
  giza_begin_buffer ();

 // Draw the arrows!
  double x, y;
  for (j = j1; j <= j2; j++)
    {
      for (i = i1; i <= i2; i++)
	{
	  if (!(horizontal[j][i] == blank && vertical[j][i] == blank))
          {	  
             x = (double) i + 0.5;
             y = (double) j + 0.5;

	     if (position < 0)
	       {
	         x2 = x;
	         y2 = y;
	         x1 = x2 - horizontal[j][i] * dscale;
	         y1 = y2 - vertical[j][i] * dscale;
	       }
	     else if (position == 0)
	       {
	         x2 = x + 0.5 * horizontal[j][i] * dscale;
	         y2 = y + 0.5 * vertical[j][i] * dscale;
	         x1 = x2 - horizontal[j][i] * dscale;
	         y1 = y2 - vertical[j][i] * dscale;
	       }
	     else
	       {
	         x1 = x;
	         y1 = y;
	         x2 = x1 + horizontal[j][i] * dscale;
	         y2 = y1 + vertical[j][i] * dscale;
	       }
	     cairo_matrix_transform_point (&mat, &x1, &y1);
	     cairo_matrix_transform_point (&mat, &x2, &y2);
	     giza_arrow (x1, y1, x2, y2);
	 }
       }
    }

  if (!oldBuf)
    giza_end_buffer ();
  
  if (!Sets.buf) giza_flush_device ();
}

void
giza_vector_float (int n, int m, float horizontal[m][n], float vertical[m][n],
	     int i1, int i2, int j1, int j2, float scale, int position,
	     float affine[6], float blank)
{
  if (!_giza_check_device_ready ("giza_vector"))
    return;

  // Check ranges
  if (i1 < 0 || i2 >= n || i1 > i2)
    {
      _giza_error ("giza_vector",
		   "invalid index range for horizontal values");
      return;
    }
  if (j1 < 0 || j2 >= m || j1 > j2)
    {
      _giza_error ("giza_vector", "invalid index range for vertical values");
      return;
    }

  int i, j, oldBuf;
  double x1, x2, y1, y2;
  cairo_matrix_t mat;

  cairo_matrix_init (&mat, affine[0], affine[1], affine[2], affine[3],
                     affine[4], affine[5]); 

 // Find the scaling factor
  double dscale = (double) scale;
  if (fabs (scale) < GIZA_ZERO_FLOAT)
    {
      for (j = j1; j <= j2; j++)
	{
	  for (i = i1; i <= i2; i++)
	    {
	      if (horizontal[j][i] != blank && vertical[j][i] != blank)
		{
		  double tmp =
		    sqrt (horizontal[j][i] * horizontal[j][i] +
			  vertical[j][i] * vertical[j][i]);
		  if (tmp > dscale)
		    dscale = tmp;
		}
	    }
	}
      if (fabs (dscale) < GIZA_ZERO_DOUBLE) { return; }
      float dx2 = affine[0]*affine[0] + affine[1]*affine[1];
      float dy2 = affine[2]*affine[2] + affine[3]*affine[3];
      if (dx2 < dy2) 
        {
          dscale = sqrt(dx2)/dscale;
        } else {
          dscale = sqrt(dy2)/dscale;
        }
    }

  oldBuf = Sets.buf;
  giza_begin_buffer ();

    // Draw the arrows!
  double x, y;
  for (j = j1; j <= j2; j++)
    {
      for (i = i1; i <= i2; i++)
	{
	  if (!(horizontal[j][i] == blank && vertical[j][i] == blank))
          {	  
             x = (double) i + 0.5;
             y = (double) j + 0.5;
 	     cairo_matrix_transform_point (&mat, &x, &y);

	     if (position < 0)
	       {
	         x2 = x;
	         y2 = y;
	         x1 = x2 - horizontal[j][i] * dscale;
	         y1 = y2 - vertical[j][i] * dscale;
	       }
	     else if (position == 0)
	       {
	         x2 = x + 0.5 * horizontal[j][i] * dscale;
	         y2 = y + 0.5 * vertical[j][i] * dscale;
	         x1 = x2 - horizontal[j][i] * dscale;
	         y1 = y2 - vertical[j][i] * dscale;
	       }
	     else
	       {
	         x1 = x;
	         y1 = y;
	         x2 = x1 + horizontal[j][i] * dscale;
	         y2 = y1 + vertical[j][i] * dscale;
	       }
  	       giza_arrow (x1, y1, x2, y2);
          }
	}
    }

  if (!oldBuf)
    giza_end_buffer ();
  
  if (!Sets.buf) giza_flush_device ();
}