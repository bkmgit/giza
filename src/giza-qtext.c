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
 * Copyright (C) 2010 James Wetter & Daniel Price. All rights reserved.
 * Contact: wetter.j@gmail.com
 *          daniel.price@monash.edu
 *
 */

#include "giza-private.h"
#include "giza-io-private.h"
#include "giza-text-private.h"
#include "giza-transforms-private.h"
#include <math.h>
#include <stdio.h>
#include <giza.h>

/**
 * Text: giza_qtext
 *
 * Synopsis: Returns the co-ordinates of a box bounding the given string if printed by giza_ptext.
 *
 * Input:
 *  -x     :- The x-coord of the text in world-coords
 *  -y     :- The y-coord of the text in world-coords
 *  -angle :- The angle to draw the text at, in degrees
 *  -just  :- The horizontal justification of the string. 0. for left-justified, 1. for right-justified
 *  -text  :- The text to be drawn
 *  -xbox  :- 
 *  -ybox  :- Set to the world co-ords of the bounding box
 */
void
giza_qtext (double x, double y, double angle, double just, char *text, double xbox[4], double ybox[4])
{
  if (!_giza_check_device_ready ("giza_qtext"))
    {
      return;
    }

  /* save the character height (can be changed during superscript/subscripting) */
  double ch;
  giza_get_character_height (&ch);

  cairo_save (context);

  _giza_set_trans (GIZA_TRANS_WORLD);
  cairo_user_to_device (context, &x, &y);

  /* Set the rotation matrix */
  /*double theta = -angle * GIZA_DEG_TO_RAD; */
  /*Sets.fontAngle = theta; */
  /*cairo_matrix_t mat; */
  /*cairo_get_font_matrix (context, &mat); */
  /*cairo_matrix_rotate (&mat, theta); */
  /*cairo_set_font_matrix (context, &mat); */

  double width = 0., height = 0.;
  _giza_set_trans (GIZA_TRANS_IDEN);
  cairo_move_to (context, 0., 0.);
  _giza_parse_string (text, &width, &height, _giza_action_get_size);

  double cosangle = cos(angle * GIZA_DEG_TO_RAD);
  double sinangle = sin(angle * GIZA_DEG_TO_RAD);
  xbox[0] = x - width * just * cosangle;
  xbox[3] = x + width * (1 - just) * cosangle;
  xbox[1] = xbox[0] - height * sinangle;
  xbox[2] = xbox[3] - height * sinangle;

  ybox[0] = y + width * just * sinangle;
  ybox[3] = y - width * (1 - just) * sinangle;
  ybox[1] = ybox[0] - height * cosangle;
  ybox[2] = ybox[3] - height * cosangle;

  _giza_set_trans (GIZA_TRANS_WORLD);
  cairo_device_to_user (context, &xbox[0], &ybox[0]);
  cairo_device_to_user (context, &xbox[1], &ybox[1]);
  cairo_device_to_user (context, &xbox[2], &ybox[2]);
  cairo_device_to_user (context, &xbox[3], &ybox[3]);

  cairo_restore (context);

  /* restore the original character height (and font matrix) */
  giza_set_character_height (ch);
}

/**
 * Text: giza_qtext_float
 *
 * Synopsis: Same as giza_qtext but takes floats
 *
 * See Also: giza_qtext
 */
void
giza_qtext_float (float x, float y, float angle, float just, char *text, float xbox[4], float ybox[4])
{
  double xboxd[4], yboxd[4];

  giza_qtext ((double) x, (double) y, (double) angle, (double) just, text, xboxd, yboxd);

  xbox[0] = (float) xboxd[0];
  xbox[1] = (float) xboxd[1];
  xbox[2] = (float) xboxd[2];
  xbox[3] = (float) xboxd[3];

  ybox[0] = (float) yboxd[0];
  ybox[1] = (float) yboxd[1];
  ybox[2] = (float) yboxd[2];
  ybox[3] = (float) yboxd[3];
}

/**
 * Text: giza_qtextlen
 *
 * Synopsis: Returns the length of a string as would be printed by giza_ptext in a variety of units
 * (added by DJP)
 *
 * Input:
 *   -units  :- The units in which to return the values
 *   -text   :- The text to be drawn
 *   -xlen   :- The length of the text in the x-direction
 *   -ylen   :- The length of the text in the y-direction
 */
void
giza_qtextlen (int units, char *text, double *xlen, double *ylen)
{ 
  if(!_giza_check_device_ready("giza_qtextlen"))
    {
      *xlen = 0.;
      *ylen = 0.;
      return;
    }

  /* save the character height (can be changed during superscript/subscripting) */
  double ch;
  giza_get_character_height (&ch);

  cairo_save (context);

  _giza_set_trans (GIZA_TRANS_IDEN); 
  cairo_move_to (context, 0., 0.);
  _giza_parse_string (text, xlen, ylen, _giza_action_get_size);

  /* got text length in device units: convert as necessary
     to desired units */
  
  switch (units)
    {
    case GIZA_UNITS_NORMALIZED:
      *xlen = *xlen / Dev.width;
      *ylen = *ylen / Dev.height;
      break;
    case GIZA_UNITS_PIXELS:
      *xlen = *xlen * Dev.deviceUnitsPerPixel;
      *ylen = *ylen * Dev.deviceUnitsPerPixel;
      break;
    case GIZA_UNITS_DEVICE:
      break;
    case GIZA_UNITS_MM:
      *xlen = *xlen * Dev.deviceUnitsPermm;
      *ylen = *ylen * Dev.deviceUnitsPermm;
      break;
    case GIZA_UNITS_INCHES:
      *xlen = *xlen * Dev.deviceUnitsPermm/25.4;
      *ylen = *ylen * Dev.deviceUnitsPermm/25.4;
      break;    
    case GIZA_UNITS_WORLD:
      _giza_set_trans (GIZA_TRANS_NORM);
      cairo_user_to_device_distance (context, xlen, ylen);
      break;
    default:
      _giza_warning ("giza_get_viewport", "Invalid units, using normalised device units.");
      *xlen = *xlen / Dev.width;
      *ylen = *ylen / Dev.height;
      break;
    }
   
  cairo_restore (context);

  /* restore the original character height (and font matrix) */
  giza_set_character_height (ch);
}

/**
 * Text: giza_qtextlen_float
 *
 * Synopsis: Same functionality as giza_qtextlen but uses floats
 *
 * See Also: giza_qtextlen
 */
void
giza_qtextlen_float (int units, char *text, float *xlen, float *ylen)
{ 
  double xlend = 0., ylend = 0.;
  giza_qtextlen(units, text, &xlend, &ylend);
  *xlen = (float) xlend;
  *ylen = (float) ylend;
}
