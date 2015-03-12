///////////////////////////////////////////////////////////////////////
// Provides the framework for graphics projects.
//
// Gary Herron
//
// Copyright © 2011 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////

#include "shader.h"
#include "scene.h"
#include "fbo.h"

#ifdef _WIN32
// Includes for Windows
    #include <windows.h>
    #include <cstdlib>
    #include <fstream>
    #include <limits>
    #include <crtdbg.h>

#else

// Includes for Linux
    #include <algorithm>
    #define sprintf_s sprintf
#include <fstream>
#endif

#include "GLee.h"
#include <freeglut.h>

#include "math.h"

Scene scene;
Scene splats;

// Some globals used for mouse handling.
int mouseX, mouseY;
bool leftDown = false;
bool middleDown = false;
bool rightDown = false;
bool shifted;

////////////////////////////////////////////////////////////////////////
// Called by GLUT when the scene needs to be redrawn.
void ReDraw()
{
    DrawScene(scene,splats);
    glutSwapBuffers();
    glutIdleFunc(NULL);
}

////////////////////////////////////////////////////////////////////////
// Called by GLUT when the window size is changed.
void ReshapeWindow(int w, int h)
{
    if (w && h) {
        glViewport(0, 0, w, h); }
    scene.width = w;
    scene.height = h;
    glutIdleFunc(&ReDraw);
    scene.sBuffer.CreateFBO(scene.width, scene.height, 3);
    scene.gBuffer.CreateFBO(scene.width, scene.height, 4);
  scene.ISM.CreateFBO(NUM_ISM*ISM_RES, NUM_ISM*ISM_RES, 1);
  scene.fBuffer.CreateFBO(scene.width, scene.height, 3);
  scene.hBuffer.CreateFBO(scene.width, scene.height, 1);

}

////////////////////////////////////////////////////////////////////////
// Called by GLut when there are keyboard actions.
void KeyboardInput(unsigned char key, int, int)
{
    switch(key) {

    case 'c':
        scene.cull = !scene.cull;
        glutIdleFunc(&ReDraw);
        break;

    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        scene.mode = key-'0';
        glutIdleFunc(&ReDraw);
        break;

    case 27:                    // Escape key
    case 'q':
        exit(0);
    }
}

////////////////////////////////////////////////////////////////////////
// Called by GLut when a mouse button changes state.
void MouseButton(int button, int state, int x, int y)
{
    shifted = glutGetModifiers() && GLUT_ACTIVE_SHIFT;

    if (button == GLUT_LEFT_BUTTON)
        leftDown = (state == GLUT_DOWN);

    else if (button == GLUT_MIDDLE_BUTTON)
        middleDown = (state == GLUT_DOWN);

    else if (button == GLUT_RIGHT_BUTTON)
        rightDown = (state == GLUT_DOWN);

    else if (button%8 == 3)
        scene.zoom = pow(scene.zoom, 1.0f/1.1f);

    else if (button%8 == 4)
        scene.zoom = pow(scene.zoom, 1.1f);

    mouseX = x;
    mouseY = y;

    glutIdleFunc(&ReDraw);
}

////////////////////////////////////////////////////////////////////////
// Called by GLut when a mouse moves (while a button is down)
void MouseMotion(int x, int y)
{
    int dx = x-mouseX;
    int dy = y-mouseY;

    if (leftDown && shifted) {
        scene.lightSpin += dx/100.0;
        scene.lightTilt -= dy/100.0;
        if (scene.lightTilt<0.0) scene.lightTilt = 0.0;
        if (scene.lightTilt>1.0) scene.lightTilt = 1.0; }

    else if (leftDown) {
        scene.eyeSpin += dx;
        scene.eyeTilt += dy; }

    if (middleDown) {
        scene.zoom += dy/10.0f; }


    if (rightDown && shifted) {
        /* nothing */ }
    else if (rightDown) {
        scene.translatex += dx/100.0f;
        scene.translatey -= dy/100.0f; }

    // Record this position 
    mouseX = x;
    mouseY = y;

    // Draw the scene, transformed by the new values.
    glutIdleFunc(&ReDraw);
}

////////////////////////////////////////////////////////////////////////
// Do the OpenGL/GLut setup and then enter the interactive loop.
int main(int argc, char** argv)
{
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(750,750);
    glutInit(&argc, argv);
    glutCreateWindow("Graphics Sample");

    glutDisplayFunc(&ReDraw);
    glutReshapeFunc(&ReshapeWindow);
    glutKeyboardFunc(&KeyboardInput);
    glutMouseFunc(&MouseButton);
    glutMotionFunc(&MouseMotion);

    InitializeScene(scene, splats);

    // This function enters an event loop, and never returns.
    glutMainLoop();
}
