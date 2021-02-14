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

#ifndef GUI_FONTS_H
#define GUI_FONTS_H

#include <GL/glew.h>

void display_text_bitmap(const GLfloat x, GLfloat y, int type, char *format, ...);
void display_text_stroke(const GLfloat x, GLfloat y, int type, GLfloat scale, char *format, ...);
void display_info(const int angle, const int rssi, const int elevation);

#endif // GUI_FONTS_H
