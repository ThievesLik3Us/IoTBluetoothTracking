/***************************************************************************//**
 * @file
 * @brief Compass GUI helper for shapes and textures.
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

#ifndef GUI_QUAD_H
#define GUI_QUAD_H

#include <GL/glew.h>

void init_disk(GLuint*vboId, GLuint*iboId);
void init_cylinder(GLuint*vboId, GLuint*iboId);
void init_cube(void);
void init_logo_BT(GLuint*vboId, GLuint*iboId);
void init_logo_SI(GLuint*vboId, GLuint*iboId);
void init_stripes(void);

void draw_disk(const GLuint vboId, const GLuint iboId);
void draw_cylinder(const GLuint vboId, const GLuint iboId);
void draw_cube(void);
void draw_logo(const GLuint vboId, const GLuint iboId);
void draw_stripes(void);

#endif // GUI_QUAD_H
