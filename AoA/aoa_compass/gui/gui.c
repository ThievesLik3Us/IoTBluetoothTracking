/***************************************************************************//**
 * @file
 * @brief Compass GUI.
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
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "texture.h"
#include "gui_fonts.h"
#include "gui_quad.h"
#include "gui.h"

/*******************************************************************************
**                             MACROS AND ENUMS                               **
*******************************************************************************/
#define RESOLUTION_WIDTH 1920
#define RESOLUTION_HEIGHT 1080
#define TIMER_RESOLUTION_MS 10
#define COMPASS_REVOLUTION 360

/*******************************************************************************
**                             LOCAL VARIABLES                                **
*******************************************************************************/
static float spin = 0;
static int rssi = 0;
static int elevation = 0;
static int targetSpin = 0;
static int anglex = 0.0;
static int perspective = 40.0;

/* Textures */
static GLuint texCompass;
static GLuint texSide;
static GLuint texBottom;
static GLuint texStripes;
static GLuint texBTLogo;
static GLuint texSILogo;

/* VBO */
static GLuint vboId_logo_0 = 0; /* BT Logo */
static GLuint iboId_logo_0 = 0;
static GLuint vboId_logo_1 = 0; /* SI Logo */
static GLuint iboId_logo_1 = 0;
static GLuint vboId_disk_0 = 0; /* Bottom */
static GLuint iboId_disk_0 = 0;
static GLuint vboId_disk_1 = 0; /* Top */
static GLuint iboId_disk_1 = 0;
static GLuint vboId_cylinder = 0; /* Cylinder */
static GLuint iboId_cylinder = 0;

/*******************************************************************************
**                             LOCAL ROUTINES                                 **
*******************************************************************************/
static void display(void);
static void display_first_half(void);
static void display_second_half(void);
static void myReshape(int w, int h);
static void myinit(void);
static void load_textures(void);
static void keyboard(unsigned char key, int x, int y);
static void idle(void);

/*******************************************************************************
**                                  MAIN                                      **
*******************************************************************************/
void gui_init(int argc, char *argv[])
{
  /* Setup glut environment */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL | GLUT_MULTISAMPLE);
  glutInitWindowSize(1280, 720);
  glutCreateWindow("Angle of Arrival Demo v1.5");

  glutDisplayFunc(display);
  glutReshapeFunc(myReshape);
  glutKeyboardFunc(keyboard);
  glutIdleFunc(idle);

/************************************ INIT GLEW********************************/
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    /* Problem: glewInit failed, something is seriously wrong. */
    printf("Error: %s\n", glewGetErrorString(err));
  }

  /* First half  */
  init_stripes();
  init_logo_BT(&vboId_logo_0, &iboId_logo_0);
  init_logo_SI(&vboId_logo_1, &iboId_logo_1);

  /* Second half  */
  init_cube();
  init_disk(&vboId_disk_0, &iboId_disk_0);
  init_disk(&vboId_disk_1, &iboId_disk_1);
  init_cylinder(&vboId_cylinder, &iboId_cylinder);

  /* app specific init */
  myinit();

  /* Load textures */
  load_textures();
}

void gui_main(void)
{
  glutMainLoopEvent();
  idle();
}

void gui_update(const float iq_azimuth, const int iq_rssi, const float iq_elevation)
{
  targetSpin = (int)iq_azimuth;
  rssi = iq_rssi;
  elevation = (int)iq_elevation;
}

/*******************************************************************************
**                              LOCAL ROUTINES                                **
*******************************************************************************/
static void load_textures(void)
{
  void* pTextureImage;
  int nComponents;
  int picture_height;
  int picture_width;

  /* Loading textures */
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

  //texCompass is global
  glGenTextures(1, &texCompass);

  /* Name texture */
  glBindTexture(GL_TEXTURE_2D, texCompass);       //now we work on texCompass
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  /* Load texture */
  pTextureImage = read_texture("./data/compass.sgi", &picture_width, &picture_height, &nComponents);
  gluBuild2DMipmaps(GL_TEXTURE_2D,     /* texture to specify */
                    GL_RGBA, /* internal texture storage format */
                    picture_width, /* texture width */
                    picture_height, /* texture height */
                    GL_RGBA, /* pixel format */
                    GL_UNSIGNED_BYTE, /* color component format */
                    pTextureImage); /* pointer to texture image */

  glBindTexture(GL_TEXTURE_2D, 0);

  //texCompass is global
  glGenTextures(1, &texBottom);

  /* Name texture */
  glBindTexture(GL_TEXTURE_2D, texBottom);       //now we work on texCompass
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  /* Load texture */
  pTextureImage = read_texture("./data/bottom.sgi", &picture_width, &picture_height, &nComponents);
  gluBuild2DMipmaps(GL_TEXTURE_2D,     /* texture to specify */
                    GL_RGBA, /* internal texture storage format */
                    picture_width, /* texture width */
                    picture_height, /* texture height */
                    GL_RGBA, /* pixel format */
                    GL_UNSIGNED_BYTE, /* color component format */
                    pTextureImage); /* pointer to texture image */

  glBindTexture(GL_TEXTURE_2D, 0);

  //texCompass is global
  glGenTextures(1, &texSide);

  /* Name texture */
  glBindTexture(GL_TEXTURE_2D, texSide);       //now we work on texCompass
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  /* Load texture */
  pTextureImage = read_texture("./data/side_colors.sgi", &picture_width, &picture_height, &nComponents);
  gluBuild2DMipmaps(GL_TEXTURE_2D,     /* texture to specify */
                    GL_RGBA, /* internal texture storage format */
                    picture_width, /* texture width */
                    picture_height, /* texture height */
                    GL_RGBA, /* pixel format */
                    GL_UNSIGNED_BYTE, /* color component format */
                    pTextureImage); /* pointer to texture image */

  glBindTexture(GL_TEXTURE_2D, 0);

  //texCompass is global
  glGenTextures(1, &texStripes);

  /* Name texture */
  glBindTexture(GL_TEXTURE_2D, texStripes);       //now we work on texCompass
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  /* Load texture */
  pTextureImage = read_texture("./data/stripes.sgi", &picture_width, &picture_height, &nComponents);
  gluBuild2DMipmaps(GL_TEXTURE_2D,     /* texture to specify */
                    GL_RGBA, /* internal texture storage format */
                    picture_width, /* texture width */
                    picture_height, /* texture height */
                    GL_RGBA, /* pixel format */
                    GL_UNSIGNED_BYTE, /* color component format */
                    pTextureImage); /* pointer to texture image */

  glBindTexture(GL_TEXTURE_2D, 0);

  //texCompass is global
  glGenTextures(1, &texBTLogo);

  /* Name texture */
  glBindTexture(GL_TEXTURE_2D, texBTLogo);       //now we work on texCompass
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  /* Load texture */
  pTextureImage = read_texture("./data/bluetooth_logo.sgi", &picture_width, &picture_height, &nComponents);

  gluBuild2DMipmaps(GL_TEXTURE_2D, /* texture to specify */
                    GL_RGBA, /* internal texture storage format */
                    picture_width, /* texture width */
                    picture_height, /* texture height */
                    GL_RGBA, /* pixel format */
                    GL_UNSIGNED_BYTE, /* color component format */
                    pTextureImage); /* pointer to texture image */
  glBindTexture(GL_TEXTURE_2D, 0);

  //texCompass is global
  glGenTextures(1, &texSILogo);

  /* Name texture */
  glBindTexture(GL_TEXTURE_2D, texSILogo);       //now we work on texCompass
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  /* Load texture */
  pTextureImage = read_texture("./data/silabs_logo.sgi", &picture_width, &picture_height, &nComponents);
  gluBuild2DMipmaps(GL_TEXTURE_2D,     /* texture to specify */
                    GL_RGBA, /* internal texture storage format */
                    picture_width, /* texture width */
                    picture_height, /* texture height */
                    GL_RGBA, /* pixel format */
                    GL_UNSIGNED_BYTE, /* color component format */
                    pTextureImage); /* pointer to texture image */
  glBindTexture(GL_TEXTURE_2D, 0);
}

static void myinit(void)
{
  glShadeModel(GL_SMOOTH);                    // shading mathod: GL_SMOOTH or GL_FLAT
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);      // 4-byte pixel alignment

  // enable /disable features
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_MULTISAMPLE_ARB);

  /*glEnable(GL_LIGHTING);*/
  glDisable(GL_LIGHTING);

  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

  glClearColor(.945, 0.965, 0.965, 0);                   // background color
  glDepthFunc(GL_LESS);

  return;
}

/******************************** Displaying **********************************/
/*  Here is where the light position is reset after the modeling
 *  transformation (glRotated) is called.  This places the
 *  light at a new position in world coordinates.  The cube
 *  represents the position of the light.
 */
static void display(void)
{
  /* clear buffer */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  /* start to draw */
  display_first_half();
  display_second_half();

  /* fbuffer management */
  glutSwapBuffers();
  glutPostRedisplay();
}

static void display_first_half(void)
{
  /* First half */
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, 3000, 0, 3000);
  glMatrixMode(GL_PROJECTION);        // switch to projection matrix
  glPushMatrix();                     // save current projection matrix
  glLoadIdentity();                   // reset projection matrix
  glTranslatef(0.0, 0.0, -1.0);

  /* Logo and marketing */
  glBindTexture(GL_TEXTURE_2D, texBTLogo);
  draw_logo(vboId_logo_0, iboId_logo_0);
  glBindTexture(GL_TEXTURE_2D, 0);

  /* Logo and marketing */
  glBindTexture(GL_TEXTURE_2D, texSILogo);
  draw_logo(vboId_logo_1, iboId_logo_1);
  glBindTexture(GL_TEXTURE_2D, 0);
  glPopMatrix();                   // restore to previous projection matrix

  glPushMatrix();                     // save current projection matrix
  glLoadIdentity();                   // reset projection matrix
  glBindTexture(GL_TEXTURE_2D, texStripes);
  draw_stripes();
  glBindTexture(GL_TEXTURE_2D, 0);

  /* Actual info */
  glEnable(GL_LIGHTING);
  display_info((int)(spin), rssi, elevation);
  glDisable(GL_LIGHTING);
  glPopMatrix();                   // restore to previous projection matrix

  // restore modelview matrix
  glMatrixMode(GL_MODELVIEW);      // switch to modelview matrix
  glPopMatrix();                   // restore to previous modelview matrix

  return;
}

static void display_second_half(void)
{
  int diffSpin;

  glPushMatrix();
  glTranslatef(1.5, 0.0, -5.0);
  glRotatef(anglex, 1.0, 0.0, 0.0); /* rotate the cube along x-axis */

  /* Needle and Tag */
  glPushMatrix();
  glRotatef((GLfloat) (COMPASS_REVOLUTION - spin), 0.0, 0.0, 1.0);
  glTranslatef(0.0, 0.9, 0.2);
  glRotated((GLdouble)90, 1.0, 0.0, 0.0);
  glColor3f(1.0, 0.0, 0.0);
  draw_cube();
  glPopMatrix();

  /* Bottom of the compass */
  glBindTexture(GL_TEXTURE_2D, texBottom);
  draw_disk(vboId_disk_0, iboId_disk_0);

  /* Side of the compass */
  glBindTexture(GL_TEXTURE_2D, texSide);
  draw_cylinder(vboId_cylinder, iboId_cylinder);
  glTranslatef(0, 0, 0.2);

  /* Top of the compass */
  glBindTexture(GL_TEXTURE_2D, texCompass);
  draw_disk(vboId_disk_1, iboId_disk_1);
  glPopMatrix();

  diffSpin = targetSpin - (int)spin;
  while (diffSpin < -180) diffSpin += 360;
  while (diffSpin >  180) diffSpin -= 360;

  spin += (float)diffSpin / 10.0;
  while (spin < 0) spin += 360;
  while (spin > 360) spin -= 360;

  return;
}

/**************************** Mouse and keyboard events ***********************/
static void myReshape(int w, int h)
{
  GLdouble aspect;

  /* Get aspect ratio */
  if (w <= h) {
    aspect = (GLdouble)h / (GLdouble)w;
  } else {
    aspect = (GLdouble)w / (GLdouble)h;
  }

  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective(perspective, (GLfloat) w / (GLfloat) h, 1.0, 40.0);
  /* Now determine where to draw things. */
  glScaled(aspect, aspect, aspect);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

static void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
    case 'x':
      anglex = (anglex + 3) % 360;
      break;
    case 'X':
      anglex = (anglex - 3) % 360;
      break;
  }
}

static void idle(void)
{
  #ifdef _WIN32
  Sleep(TIMER_RESOLUTION_MS);
  #else
  usleep(TIMER_RESOLUTION_MS * 1000);
  #endif
  glutPostRedisplay();
}
