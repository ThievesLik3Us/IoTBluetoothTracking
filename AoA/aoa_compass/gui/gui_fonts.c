/***************************************************************************//**
 * @file
 * @brief Compass GUI helper for texts.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "gui_fonts.h"

static char deg_char = 7;

static void print_bitmap_string(void* font, char* s)
{
  if (s && strlen(s)) {
    while (*s) {
      glutBitmapCharacter(font, *s);
      s++;
    }
  }
}

static void print_stroke_string(void* font, char* s)
{
  if (s && strlen(s)) {
    while (*s) {
      glutStrokeCharacter(font, *s);
      s++;
    }
  }
}

static int get_text_offset(int value)
{
  unsigned int offset = 75;

  if ( (value >= 10) && (value < 100) ) {
    offset = 150; /* 300 needed ( 2 * 150 )*/
  }

  if ( value > 100 ) {
    offset = 225; /* 300 needed ( 3 * 75 ) */
  }

  if ( (value < 0) && (value >= -10) ) {
    offset = 200; /* 200 needed */
  }

  if ( (value < -10) && (value > -100) ) {
    offset = 300; /* 350 needed ( 2 * 150 + 200) */
  }

  if ( value <= -100 ) {
    offset = 330; /* 500 needed ( 3 * 150 + 200) */
  }

  return offset;
}

void display_text_bitmap(const GLfloat x, GLfloat y, int type, char *format, ...)
{
  GLfloat yild;
  va_list args;
  char buffer[200];

  va_start(args, format);
  vsprintf(buffer, format, args);
  va_end(args);

  yild   = 3.0;

  void* bitmap_fonts[7] = {
    GLUT_BITMAP_9_BY_15,
    GLUT_BITMAP_8_BY_13,
    GLUT_BITMAP_TIMES_ROMAN_10,
    GLUT_BITMAP_TIMES_ROMAN_24,
    GLUT_BITMAP_HELVETICA_10,
    GLUT_BITMAP_HELVETICA_12,
    GLUT_BITMAP_HELVETICA_18
  };

  /* Draw the strings, according to the current mode and font. */
  glRasterPos2f(x, y);
  print_bitmap_string(bitmap_fonts[type], buffer);
  y -= yild;
}

void display_text_stroke(const GLfloat x, GLfloat y, int type, GLfloat scale, char *format, ...)
{
  GLfloat ystep;
  GLfloat stroke_scale;
  GLfloat yild;
  va_list args;
  char buffer[200];

  va_start(args, format);
  vsprintf(buffer, format, args);
  va_end(args);

  yild = 20.0;
  ystep  = 100.0;

  void* stroke_fonts[2] = {
    GLUT_STROKE_ROMAN,
    GLUT_STROKE_MONO_ROMAN
  };

  /* Draw the strings, according to the current mode and font. */
  stroke_scale = scale;
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glTranslatef(x, y, 0.0);
  glPushMatrix();
  glScalef(stroke_scale, stroke_scale, stroke_scale);
  print_stroke_string(stroke_fonts[type], buffer);
  glPopMatrix();
  glTranslatef(0.0, -yild, 0.0);
  glTranslatef(0.0, -ystep, 0.0);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
}

void display_info(const int angle, const int rssi, const int elevation)
{
  display_text_bitmap(30, 1400, 6, "Angle of Arrival Demo");
  display_text_bitmap(30, 1300, 6, "Move the tag around for position tracking.");

  /* RSSI */
  display_text_stroke(30, 280, 0, 1.0, "%d", rssi);
  display_text_bitmap(30 + 10 + get_text_offset(rssi), 350, 6, "dBm");
  display_text_bitmap(30 + 10 + get_text_offset(rssi), 280, 6, "RSSI");

  /* elevation */
  display_text_stroke(30, 150, 0, 1.0, "%d", elevation);
  display_text_bitmap(30 + get_text_offset(elevation), 210, 0, &deg_char);
  display_text_bitmap(30 + 10 + get_text_offset(elevation), 150, 6, "Elevation");

  /* Angle */
  display_text_stroke(30, 20, 0, 1.0, "%d", angle);
  display_text_bitmap(30 + get_text_offset(angle), 90, 0, &deg_char);
  display_text_bitmap(30 + 10 + get_text_offset(angle), 30, 6, "Position angle");
}
