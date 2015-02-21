#include "fbo.h"

////////////////////////////////////////////////////////////////////////
// The scene class contains all the parameters needed to define and
// draw the (really) simple scene, including:
//   * Geometry (in a display list)
//   * Light parameters
//   * Material properties
//   * Viewport size parameters
//   * Viewing transformation values
//   * others ...
//
// Some of these parameters are set when the scene is built, and
// others are set by the framework in response to user mouse/keyboard
// interactions.  All of them should be used to draw the scene.

class Scene
{
public:
    // Some user controllable parameters
    int mode; // Communicated to the shaders as "mode".  Keys '0'-'9'
    bool cull; // Boolean to perform pass 1 front-face culling or not; Key 'c'

    // Viewing transformation parameters;  Mouse buttons 1-3
    float front;
    float eyeSpin;
    float eyeTilt;
    float translatex;
    float translatey;
    float zoom;

    // Light position parameters;  Mouse buttons SHIFT 1-3
    float lightSpin;
    float lightTilt;
    
    // The displaylists containing the objects to be drawn
    int roomDL;

    // Viewport
    int width, height;

    // The Frame-buffer-objects
    FBO gBuffer, sBuffer, ISM, fBuffer, hBuffer;
};

void InitializeScene(Scene &scene,Scene &splats);
void BuildScene(Scene &scene,Scene &splats);
void DrawScene(Scene &scene,Scene &splats);

#define NUM_ISM 16
#define ISM_RES 64
