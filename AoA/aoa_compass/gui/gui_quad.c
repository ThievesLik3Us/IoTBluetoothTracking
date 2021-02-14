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

#include <math.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "gui_quad.h"

/*******************************************************************************
**                             MACROS AND ENUMS                               **
*******************************************************************************/

/*******************************************************************************
**                                TYPEDEFS                                    **
*******************************************************************************/
#define SECTOR 360
#define FACTOR (0.7071068f / 1.42f) /* (sqrt(2)/2)/1.3 */

const float PI = 3.1415926f;

/*******************************************************************************
**                             LOCAL VARIABLES                                **
*******************************************************************************/
static const unsigned int sectorCount = SECTOR;
static const float radius = 1.0f;

static GLuint vboId = 0; /* ID of VBO for vertex arrays */
static GLuint iboId = 0; /* ID of VBO for index array */

static GLuint vboId_stripe_0 = 0;
static GLuint iboId_stripe_0 = 0;

// vertex coords array for glDrawElements() ===================================
// A cube has 6 sides and each side has 4 vertices, therefore, the total number
// of vertices is 24 (6 sides * 4 verts), and 72 floats in the vertex array
// since each vertex has 3 components (x,y,z) (= 24 * 3)
static const GLfloat vertices[]  = {
  .02f, .01f, .1f, -.02f, .01f, .1f, -.02f, -.01f, .1f, .02f, -.01f, .1f,       // v0,v1,v2,v3 (front)
  .02f, .01f, .1f, .02f, -.01f, .1f, .02f, -.01f, -.1f, .02f, .01f, -.1f,      // v0,v3,v4,v5 (right)
  .02f, .01f, .1f, .02f, .01f, -.1f, -.02f, .01f, -.1f, -.02f, .01f, .1f,       // v0,v5,v6,v1 (top)
  -.02f, .01f, .1f, -.02f, .01f, -.1f, -.02f, -.01f, -.1f, -.02f, -.01f, .1f,   // v1,v6,v7,v2 (left)
  -.02f, -.01f, -.1f, .02f, -.01f, -.1f, .02f, -.01f, .1f, -.02f, -.01f, .1f,   // v7,v4,v3,v2 (bottom)
  .02f, -.01f, -.1f, -.02f, -.01f, -.1f, -.02f, .01f, -.1f, .02f, .01f, -.1f    // v4,v7,v6,v5 (back)
};

// normal array
static const GLfloat normals[] = {
  0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1,           // v0,v1,v2,v3 (front)
  1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0,           // v0,v3,v4,v5 (right)
  0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,           // v0,v5,v6,v1 (top)
  -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0,       // v1,v6,v7,v2 (left)
  0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0,       // v7,v4,v3,v2 (bottom)
  0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1        // v4,v7,v6,v5 (back)
};

// texCoord array
static const GLfloat texCoords[] = {
  1, 0, 0, 0, 0, 1, 1, 1,                       // v0,v1,v2,v3 (front)
  0, 0, 0, 1, 1, 1, 1, 0,                       // v0,v3,v4,v5 (right)
  1, 1, 1, 0, 0, 0, 0, 1,                       // v0,v5,v6,v1 (top)
  1, 0, 0, 0, 0, 1, 1, 1,                       // v1,v6,v7,v2 (left)
  0, 1, 1, 1, 1, 0, 0, 0,                       // v7,v4,v3,v2 (bottom)
  0, 1, 1, 1, 1, 0, 0, 0                        // v4,v7,v6,v5 (back)
};

// index array for glDrawElements() ===========================================
// A cube has 36 indices = 6 sides * 2 tris * 3 verts
static const GLuint indices[] = {
  0, 1, 2, 2, 3, 0,         // v0-v1-v2, v2-v3-v0 (front)
  4, 5, 6, 6, 7, 4,         // v0-v3-v4, v4-v5-v0 (right)
  8, 9, 10, 10, 11, 8,      // v0-v5-v6, v6-v1-v0 (top)
  12, 13, 14, 14, 15, 12,   // v1-v6-v7, v7-v2-v1 (left)
  16, 17, 18, 18, 19, 16,   // v7-v4-v3, v3-v2-v7 (bottom)
  20, 21, 22, 22, 23, 20    // v4-v7-v6, v6-v5-v4 (back)
};

static GLfloat disk_vertices[SECTOR * 9];
static GLfloat disk_normals[SECTOR * 9];
static GLuint disk_indices[SECTOR * 3];
static GLfloat disk_texCoords[SECTOR * 6];

static GLfloat cylinder_vertices[SECTOR * 12];
static GLfloat cylinder_normals[SECTOR * 12];
static GLfloat cylinder_texCoords[SECTOR * 8];
static GLuint cylinder_indices[SECTOR * 6];

// vertex coords array for glDrawElements() ===================================
// A cube has 6 sides and each side has 4 vertices, therefore, the total number
// of vertices is 24 (6 sides * 4 verts), and 72 floats in the vertex array
// since each vertex has 3 components (x,y,z) (= 24 * 3)
static const GLfloat logo_vertices_BT[]  = {
  30.0f, 1600.0f, .0f,   /* silabs */
  30.0f, 2076.0f, .0f,
  506.0f, 2076.0f, .0f,
  506.0f, 1600.0f, .0f
};

static const GLfloat logo_vertices_SI[]  = {
  30.0f, 2600.0f, .0f,   /* silabs */
  30.0f, 3000.0f, .0f,
  530.0f, 3000.0f, .0f,
  530.0f, 2600.0f, .0f
};

// texCoord array
static const GLfloat logo_texCoords[] = {
  0, 0, 0, 1, 1, 1, 1, 0,
};

// index array for glDrawElements() ===========================================
// A cube has 36 indices = 6 sides * 2 tris * 3 verts
static const GLuint logo_indices[] = {
  0, 1, 2, 2, 3, 0         // v0-v1-v2, v2-v3-v0 (front)
};

static const GLfloat stripes_vertices[]  = {
  0.0f, 0.0f, .0f,
  10.0f, 0.0f, .0f,
  10.0f, 3000.0f, .0f,
  0.0f, 3000.0f, .0f,
  1490.0f, 0.0f, .0f,
  1500.0f, 0.0f, .0f,
  1500.0f, 3000.0f, .0f,
  1490.0f, 3000.0f, .0f
};

// texCoord array
static const GLfloat stripes_texCoords[] = {
  0, 0, 0, 1, 1, 1, 1, 0,
  0, 0, 0, 1, 1, 1, 1, 0
};

// index array for glDrawElements() ===========================================
// A cube has 36 indices = 6 sides * 2 tris * 3 verts
static const GLuint stripes_indices[] = {
  0, 1, 2, 2, 3, 0,
  4, 5, 6, 6, 7, 4
};

/*******************************************************************************
**                                 API                                        **
*******************************************************************************/
void init_cube(void)
{
  /*********************************** INIT BUFFER ******************************/
  // create vertex buffer objects, you need to delete them when program exits
  // Try to put both vertex coords array, vertex normal array and vertex color in the same buffer object.
  // glBufferData with NULL pointer reserves only memory space.
  // Copy actual data with multiple calls of glBufferSubData for vertex positions, normals etc.
  // target flag is GL_ARRAY_BUFFER, and usage flag is GL_STATIC_DRAW
  glGenBuffers(1, &vboId);
  glBindBuffer(GL_ARRAY_BUFFER, vboId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(normals) + sizeof(texCoords), 0, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); /* copy vertices starting from 0 offest */
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(normals), normals); /* copy normals after vertices */
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(normals), sizeof(texCoords), texCoords); /* copy colours after normals */
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenBuffers(1, &iboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void init_logo_BT(GLuint*vboId, GLuint*iboId)
{
  glGenBuffers(1, vboId);
  glBindBuffer(GL_ARRAY_BUFFER, *vboId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(logo_vertices_BT) + sizeof(logo_texCoords), 0, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(logo_vertices_BT), logo_vertices_BT);                                                    // copy vertices starting from 0 offest
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(logo_vertices_BT), sizeof(logo_texCoords), logo_texCoords); // copy colours after normals
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenBuffers(1, iboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *iboId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(logo_indices), logo_indices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void init_logo_SI(GLuint*vboId, GLuint*iboId)
{
  glGenBuffers(1, vboId);
  glBindBuffer(GL_ARRAY_BUFFER, *vboId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(logo_vertices_SI) + sizeof(logo_texCoords), 0, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(logo_vertices_SI), logo_vertices_SI);                                                    // copy vertices starting from 0 offest
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(logo_vertices_SI), sizeof(logo_texCoords), logo_texCoords); // copy colours after normals
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenBuffers(1, iboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *iboId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(logo_indices), logo_indices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void init_stripes(void)
{
  glGenBuffers(1, &vboId_stripe_0);
  glBindBuffer(GL_ARRAY_BUFFER, vboId_stripe_0);
  glBufferData(GL_ARRAY_BUFFER, sizeof(stripes_vertices) + sizeof(logo_texCoords), 0, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(stripes_vertices), stripes_vertices);
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(stripes_vertices), sizeof(stripes_texCoords), stripes_texCoords); // copy colours after normals
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenBuffers(1, &iboId_stripe_0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId_stripe_0);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(stripes_indices), stripes_indices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void init_disk(GLuint*vboId, GLuint*iboId)
{
  const float PI = 3.1415926f;
  unsigned int i;
  float sectorStep = ((2 * PI) / SECTOR);
  float sectorAngle;

  for (i = 0; i < sectorCount; i++) {
    sectorAngle = i * sectorStep;

    /* vertices */
    disk_vertices[(i * 9)] = 0.0f;
    disk_vertices[(i * 9) + 1] = 0.0f;
    disk_vertices[(i * 9) + 2] = 0.0f;

    disk_vertices[(i * 9) + 3] = radius * cosf(sectorAngle);
    disk_vertices[(i * 9) + 4] = radius * sinf(sectorAngle);
    disk_vertices[(i * 9) + 5] = 0.0f;

    disk_vertices[(i * 9) + 6] = radius * cosf(sectorAngle + sectorStep);
    disk_vertices[(i * 9) + 7] = radius * sinf(sectorAngle + sectorStep);
    disk_vertices[(i * 9) + 8] = 0.0f;
  }

  for (i = 0; i < (sectorCount * 3); i += 3) {
    /* normals */
    disk_normals[i] = 0;
    disk_normals[i + 1] = 0;
    disk_normals[i + 2] = 1;

    /* indices */
    disk_indices[i] = i;
    disk_indices[i + 1] = i + 1;
    disk_indices[i + 2] = i + 2;
  }

  for (i = 0; i < (sectorCount * 2 * 3); i += 6) {
    sectorAngle = (i / 6) * sectorStep;

    /* texCoords */
    disk_texCoords[i] = 0.5f;
    disk_texCoords[i + 1] = 0.5f;

    disk_texCoords[i + 2] = (0.5f + FACTOR * cosf(sectorAngle));
    disk_texCoords[i + 3] = (0.5f + FACTOR * sinf(sectorAngle));

    disk_texCoords[i + 4] = (0.5f + FACTOR * cosf(sectorAngle + sectorStep));
    disk_texCoords[i + 5] = (0.5f + FACTOR * sinf(sectorAngle + sectorStep));
  }

  /* create vertex buffer objects, you need to delete them when program exits
  ** Try to put both vertex coords array, vertex normal array and vertex color in the same buffer object.
  ** glBufferData with NULL pointer reserves only memory space.
  ** Copy actual data with multiple calls of glBufferSubData for vertex positions, normals, colors, etc.
  ** target flag is GL_ARRAY_BUFFER, and usage flag is GL_STATIC_DRAW */
  glGenBuffers(1, vboId);
  glBindBuffer(GL_ARRAY_BUFFER, *vboId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(disk_vertices) + sizeof(disk_normals) + sizeof(disk_texCoords), 0, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(disk_vertices), disk_vertices);
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(disk_vertices), sizeof(disk_normals), disk_normals);
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(disk_vertices) + sizeof(disk_normals), sizeof(disk_texCoords), disk_texCoords); // copy colours after normals
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenBuffers(1, iboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *iboId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(disk_indices), disk_indices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  return;
}

void init_cylinder(GLuint*vboId, GLuint*iboId)
{
  const float PI = 3.1415926f;
  unsigned int i;
  float sectorStep = ((2 * PI) / SECTOR);
  float sectorAngle;

  for (i = 0; i < sectorCount; i++) {
    sectorAngle = i * sectorStep;

    /* vertices */
    cylinder_vertices[(i * 12)] = radius * cosf(sectorAngle);
    cylinder_vertices[(i * 12) + 1] = radius * sinf(sectorAngle);
    cylinder_vertices[(i * 12) + 2] = 0.0f;

    cylinder_vertices[(i * 12) + 3] = radius * cosf(sectorAngle);
    cylinder_vertices[(i * 12) + 4] = radius * sinf(sectorAngle);
    cylinder_vertices[(i * 12) + 5] = 0.2f; /* h */

    cylinder_vertices[(i * 12) + 6] = radius * cosf(sectorAngle + sectorStep);
    cylinder_vertices[(i * 12) + 7] = radius * sinf(sectorAngle + sectorStep);
    cylinder_vertices[(i * 12) + 8] = 0.2f; /* h */

    cylinder_vertices[(i * 12) + 9] = radius * cosf(sectorAngle + sectorStep);
    cylinder_vertices[(i * 12) + 10] = radius * sinf(sectorAngle + sectorStep);
    cylinder_vertices[(i * 12) + 11] = 0.0f;

    /* normals */
    cylinder_normals[(i * 12)] = 0;
    cylinder_normals[(i * 12) + 1] = 0;
    cylinder_normals[(i * 12) + 2] = 1;

    cylinder_normals[(i * 12) + 3] = 0;
    cylinder_normals[(i * 12) + 4] = 0;
    cylinder_normals[(i * 12) + 5] = 1;

    cylinder_normals[(i * 12) + 6] = 0;
    cylinder_normals[(i * 12) + 7] = 0;
    cylinder_normals[(i * 12) + 8] = 1;

    cylinder_normals[(i * 12) + 9] = 0;
    cylinder_normals[(i * 12) + 10] = 0;
    cylinder_normals[(i * 12) + 11] = 1;

    /* Indices */
    cylinder_indices[(i * 6)] = (i * 4);
    cylinder_indices[(i * 6) + 1] = (i * 4) + 1;
    cylinder_indices[(i * 6) + 2] = (i * 4) + 2;

    cylinder_indices[(i * 6) + 3] = (i * 4) + 3;
    cylinder_indices[(i * 6) + 4] = (i * 4) + 2;
    cylinder_indices[(i * 6) + 5] = (i * 4);

    /* tex coords */
    cylinder_texCoords[(i * 8)] = ((float)i / sectorCount);
    cylinder_texCoords[(i * 8) + 1] = 0;

    cylinder_texCoords[(i * 8) + 2] = ((float)i / sectorCount);
    cylinder_texCoords[(i * 8) + 3] = 1;

    cylinder_texCoords[(i * 8) + 4] = ((float)(i + 1) / sectorCount);
    cylinder_texCoords[(i * 8) + 5] = 1;

    cylinder_texCoords[(i * 8) + 6] = ((float)(i + 1) / sectorCount);
    cylinder_texCoords[(i * 8) + 7] = 0;
  }

  /* create vertex buffer objects, you need to delete them when program exits
  ** Try to put both vertex coords array, vertex normal array and vertex color in the same buffer object.
  ** glBufferData with NULL pointer reserves only memory space.
  ** Copy actual data with multiple calls of glBufferSubData for vertex positions, normals, colors, etc.
  ** target flag is GL_ARRAY_BUFFER, and usage flag is GL_STATIC_DRAW */
  glGenBuffers(1, vboId);
  glBindBuffer(GL_ARRAY_BUFFER, *vboId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cylinder_vertices) + sizeof(cylinder_normals) + sizeof(cylinder_texCoords), 0, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cylinder_vertices), cylinder_vertices);
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(cylinder_vertices), sizeof(cylinder_normals), cylinder_normals);
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(cylinder_vertices) + sizeof(cylinder_normals), sizeof(cylinder_texCoords), cylinder_texCoords); // copy colours after normals
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenBuffers(1, iboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *iboId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cylinder_indices), cylinder_indices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  return;
}

void draw_cube(void)
{
  // bind VBOs with IDs and set the buffer offsets of the bound VBOs
  // When buffer object is bound with its ID, all pointers in gl*Pointer()
  // are treated as offset instead of real pointer.
  glBindBuffer(GL_ARRAY_BUFFER, vboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);

  // enable vertex arrays
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);

  // before draw, specify vertex and index arrays with their offsets
  glNormalPointer(GL_FLOAT, 0, (void*)sizeof(vertices));
  glTexCoordPointer(2, GL_FLOAT, 0, (void*)(sizeof(vertices) + sizeof(normals)));
  glVertexPointer(3, GL_FLOAT, 0, 0);

  glDrawElements(GL_TRIANGLES,            // primitive type
                 36,                      // # of indices
                 GL_UNSIGNED_INT,         // data type
                 (void*)0);               // ptr to indices

  glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);

  // it is good idea to release VBOs with ID 0 after use.
  // Once bound with 0, all pointers in gl*Pointer() behave as real
  // pointer, so, normal vertex array operations are re-activated
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void draw_logo(const GLuint vboId, const GLuint iboId)
{
  // bind VBOs with IDs and set the buffer offsets of the bound VBOs
  // When buffer object is bound with its ID, all pointers in gl*Pointer()
  // are treated as offset instead of real pointer.
  glBindBuffer(GL_ARRAY_BUFFER, vboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);

  // enable vertex arrays
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);

  // before draw, specify vertex and index arrays with their offsets
  glTexCoordPointer(2, GL_FLOAT, 0, (void*)(sizeof(logo_vertices_BT)));
  glVertexPointer(3, GL_FLOAT, 0, 0);

  glDrawElements(GL_TRIANGLES,            // primitive type
                 6,                      // # of indices
                 GL_UNSIGNED_INT,         // data type
                 (void*)0);               // ptr to indices

  glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);

  // it is good idea to release VBOs with ID 0 after use.
  // Once bound with 0, all pointers in gl*Pointer() behave as real
  // pointer, so, normal vertex array operations are re-activated
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void draw_stripes(void)
{
  glBindBuffer(GL_ARRAY_BUFFER, vboId_stripe_0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId_stripe_0);

  // enable vertex arrays
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);

  // before draw, specify vertex and index arrays with their offsets
  glTexCoordPointer(2, GL_FLOAT, 0, (void*)(sizeof(stripes_vertices)));
  glVertexPointer(3, GL_FLOAT, 0, 0);

  glDrawElements(GL_TRIANGLES,            // primitive type
                 12,                      // # of indices
                 GL_UNSIGNED_INT,         // data type
                 (void*)0);               // ptr to indices

  glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);

  // it is good idea to release VBOs with ID 0 after use.
  // Once bound with 0, all pointers in gl*Pointer() behave as real
  // pointer, so, normal vertex array operations are re-activated
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void draw_disk(const GLuint vboId, const GLuint iboId)
{
  // bind VBOs with IDs and set the buffer offsets of the bound VBOs
  // When buffer object is bound with its ID, all pointers in gl*Pointer()
  // are treated as offset instead of real pointer.
  glBindBuffer(GL_ARRAY_BUFFER, vboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);

  // enable vertex arrays
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);

  // before draw, specify vertex and index arrays with their offsets
  glNormalPointer(GL_FLOAT, 0, (void*)sizeof(disk_vertices));
  glTexCoordPointer(2, GL_FLOAT, 0, (void*)(sizeof(disk_vertices) + sizeof(disk_normals)));
  glVertexPointer(3, GL_FLOAT, 0, 0);

  glDrawElements(GL_TRIANGLES,            // primitive type
                 (SECTOR * 3),                      // # of indices
                 GL_UNSIGNED_INT,         // data type
                 (void*)0);               // ptr to indices

  glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);

  // it is good idea to release VBOs with ID 0 after use.
  // Once bound with 0, all pointers in gl*Pointer() behave as real
  // pointer, so, normal vertex array operations are re-activated
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void draw_cylinder(const GLuint vboId, const GLuint iboId)
{
  // bind VBOs with IDs and set the buffer offsets of the bound VBOs
  // When buffer object is bound with its ID, all pointers in gl*Pointer()
  // are treated as offset instead of real pointer.
  glBindBuffer(GL_ARRAY_BUFFER, vboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);

  // enable vertex arrays
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);

  // before draw, specify vertex and index arrays with their offsets
  glNormalPointer(GL_FLOAT, 0, (void*)sizeof(cylinder_vertices));
  glTexCoordPointer(2, GL_FLOAT, 0, (void*)(sizeof(cylinder_vertices) + sizeof(cylinder_normals)));
  glVertexPointer(3, GL_FLOAT, 0, 0);

  glDrawElements(GL_TRIANGLES,            // primitive type
                 (SECTOR * 6),                      // # of indices
                 GL_UNSIGNED_INT,         // data type
                 (void*)0);               // ptr to indices

  glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);

  // it is good idea to release VBOs with ID 0 after use.
  // Once bound with 0, all pointers in gl*Pointer() behave as real
  // pointer, so, normal vertex array operations are re-activated
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
