///////////////////////////////////////////////////////////////////////
// Defines and draws a scene suitable for testing global illumination.
////////////////////////////////////////////////////////////////////////

#include "shader.h"
#include "scene.h"
#include "vector.h"
#include <vector>
#include "texture.h"

#include "math.h"
#include "GLee.h"
#include "freeglut.h"
#include <fstream>
#include <time.h>

float materials[][4] = {
    0.7, 0.7, 0.7, 0.0,
    0.7, 0.7, 0.7, 0.0,
    0.7, 0.1, 0.1, 0.0,
    0.0, 0.7, 0.1, 0.0,
    0.7, 0.7, 0.1, 0.0,
    0.1, 0.1, 0.7, 0.0
};

float vertices[][3] = {
     -65.0/280.0, 227.0/280.0-1.0,  275.8/280.0,
     -65.0/280.0, 332.0/280.0-1.0,  275.8/280.0,
      65.0/280.0, 332.0/280.0-1.0,  275.8/280.0,
      65.0/280.0, 227.0/280.0-1.0,  275.8/280.0,
    -274.8/280.0,   0.0/280.0-1.0, -273.0/280.0,
     278.0/280.0,   0.0/280.0-1.0, -273.0/280.0,
     278.0/280.0, 559.2/280.0-1.0, -273.0/280.0,
    -271.6/280.0, 559.2/280.0-1.0, -273.0/280.0,
     -12.0/280.0, 114.0/280.0-1.0, -273.0/280.0,
      38.0/280.0, 272.0/280.0-1.0, -273.0/280.0,
     196.0/280.0, 225.0/280.0-1.0, -273.0/280.0,
     148.0/280.0,  65.0/280.0-1.0, -273.0/280.0,
    -194.0/280.0, 406.0/280.0-1.0, -273.0/280.0,
     -36.0/280.0, 456.0/280.0-1.0, -273.0/280.0,
      13.0/280.0, 296.0/280.0-1.0, -273.0/280.0,
    -145.0/280.0, 247.0/280.0-1.0, -273.0/280.0,
    -278.0/280.0,   0.0/280.0-1.0,  275.8/280.0,
    -278.0/280.0, 559.2/280.0-1.0,  275.8/280.0,
     278.0/280.0, 559.2/280.0-1.0,  275.8/280.0,
     278.0/280.0,   0.0/280.0-1.0,  275.8/280.0,
     148.0/280.0,  65.0/280.0-1.0, -108.0/280.0,
     196.0/280.0, 225.0/280.0-1.0, -108.0/280.0,
      38.0/280.0, 272.0/280.0-1.0, -108.0/280.0,
     -12.0/280.0, 114.0/280.0-1.0, -108.0/280.0,
    -145.0/280.0, 247.0/280.0-1.0,   57.0/280.0,
      13.0/280.0, 296.0/280.0-1.0,   57.0/280.0,
     -36.0/280.0, 456.0/280.0-1.0,   57.0/280.0,
    -194.0/280.0, 406.0/280.0-1.0,   57.0/280.0
};

// contains an index into materials, followed by four indices into vertices
int quads[][5] = {
    1,  4,  5,  6,  7,          // floor
    1, 16, 17, 18, 19,          // ceiling
    5,  5,  4, 16, 19,          // front 
    4,  7,  6, 18, 17,          // back
    3,  6,  5, 19, 18,          // right
    2,  4,  7, 17, 16,          // left
    1, 20, 21, 22, 23,
    1,  8, 23, 22,  9,
    1, 11, 20, 23,  8,
    1, 10, 21, 20, 11,
    1,  9, 22, 21, 10,
    1, 24, 25, 26, 27,
    1, 15, 24, 27, 12,
    1, 12, 27, 26, 13,
    1, 13, 26, 25, 14,
    1, 14, 25, 24, 15

};

std::vector<float> splat_vertices;
std::vector<float> splat_ism_positions;

float whiteColor[4] = {1.0, 1.0, 1.0, 1.0};
float ambientColor[4] = {0.2f, 0.2f, 0.2f, 1.0f};
float lightColor[4] = {0.8f, 0.8f, 0.8f, 1.0f};
float specularColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
int shininess = 120;

GLuint TAM1[6]; // texture for hatches
static unsigned *image; // pointer to texture data
static int components;  // components of texture data

const float PI = 3.14159f;
const float rad = PI/180.0f;

ShaderProgram shader1;
ShaderProgram shader2;
ShaderProgram shader3;
ShaderProgram ism_shader;
ShaderProgram filter_shader;
ShaderProgram hatch_shader;

////////////////////////////////////////////////////////////////////////
void PolyNormal(float* a, float* b, float*c)
{
    V A(a[0], a[1], a[2]);
    V B(b[0], b[1], b[2]);
    V C(c[0], c[1], c[2]);
    V N = (B-A).cross(C-A);
    glNormal3f(N.x, N.y, N.z);
}

////////////////////////////////////////////////////////////////////////
void CreatRoom(Scene &scene, Scene &splats)
{
    //make the scene with the sqaures and stuff
    scene.roomDL = glGenLists(1);
    glNewList(scene.roomDL, GL_COMPILE);

    int nquads = sizeof(quads)/sizeof(quads[0]);
    glEnable(GL_CULL_FACE);
    for (int q=0;  q<nquads;  q++)
    {
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, materials[quads[q][0]]);
        PolyNormal(vertices[quads[q][1]], vertices[quads[q][2]], vertices[quads[q][3]]);
    glBegin(GL_QUADS);
        glVertex3fv(vertices[quads[q][1]]);
        glVertex3fv(vertices[quads[q][2]]);
        glVertex3fv(vertices[quads[q][3]]);
        glVertex3fv(vertices[quads[q][4]]);
    glEnd();
    }
    glDisable(GL_CULL_FACE);
        
    glEndList();

  
  //for each quad
    for (int q=0;  q<nquads;  q++)
    {
    glPointSize(10.0);
    //set the color
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, materials[quads[q][0]]);
    //set the normal
        PolyNormal(vertices[quads[q][1]], vertices[quads[q][2]], vertices[quads[q][3]]);

    V corner(vertices[quads[q][1]][0], vertices[quads[q][1]][1], vertices[quads[q][1]][2]);

    V delta_i(vertices[quads[q][2]][0] - vertices[quads[q][1]][0],
          vertices[quads[q][2]][1] - vertices[quads[q][1]][1],
          vertices[quads[q][2]][2] - vertices[quads[q][1]][2]);

    float length = delta_i.length();
    float i_step = 1.0f/(delta_i.length()*100.0f);

    V delta_j(vertices[quads[q][4]][0] - vertices[quads[q][1]][0],
          vertices[quads[q][4]][1] - vertices[quads[q][1]][1],
          vertices[quads[q][4]][2] - vertices[quads[q][1]][2]);

    float j_step = 1.0f/(delta_j.length()*100.0f);

    for(float i = 0.0f; i < 1.0f; i += i_step)
      for(float j = 0.0f; j < 1.0f; j += j_step)
      {
        V splat = corner + delta_i*i + delta_j*j;
        splat_vertices.push_back(splat.x);
        splat_vertices.push_back(splat.y);
        splat_vertices.push_back(splat.z);

        splat_ism_positions.push_back(rand()%NUM_ISM);
        splat_ism_positions.push_back(rand()%NUM_ISM);
      }
    }

  //make the scene with the splats and things
  splats.roomDL = glGenLists(1);
  glNewList(splats.roomDL, GL_COMPILE);

  glBegin(GL_POINTS); //tell open gl that these vertices are going to be points

  for(int i = 0; i < splat_vertices.size(); i+=3)
    glVertex3f(splat_vertices[i], splat_vertices[i+1], splat_vertices[i+2]);

  glEnd();
        
    glEndList();
}

////////////////////////////////////////////////////////////////////////
// InitializeScene is called with the expectaton that OpenGL modes
// will be setup.  This should include lights, materials, shaders,
// texture maps, and display lists containing the objects to be drawn.
void InitializeScene(Scene &scene, Scene &splats)
{
    scene.mode = 0;
    scene.cull = false;
  splats.mode = 0;
  splats.cull = false;

    //@@ Good starting view for the room:
    scene.front = 1.0;
    scene.eyeSpin = -90.0;
    scene.eyeTilt = -80.0;
    scene.translatex = 0.0;
    scene.translatey = 0.0;
    scene.zoom = 4.0;
    
    // Initialize OpenGL parameters that will not change

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  lightColor);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);
    glEnable(GL_LIGHT0);

    glMateriali(GL_FRONT, GL_SHININESS, shininess);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specularColor);

    // Create the shader programs from vert/frag files.
    shader1.CreateProgram();
    shader1.CreateShader("shader1.vert", GL_VERTEX_SHADER);
    shader1.CreateShader("shader1.frag", GL_FRAGMENT_SHADER);
    shader1.LinkProgram();
    
    shader2.CreateProgram();
    shader2.CreateShader("shader2.vert", GL_VERTEX_SHADER);
    shader2.CreateShader("shader2.frag", GL_FRAGMENT_SHADER);
    shader2.LinkProgram();
    
    shader3.CreateProgram();
    shader3.CreateShader("shader3.vert", GL_VERTEX_SHADER);
    shader3.CreateShader("shader3.frag", GL_FRAGMENT_SHADER);
    shader3.LinkProgram();

    ism_shader.CreateProgram();
    ism_shader.CreateShader("ISM.vert", GL_VERTEX_SHADER);
    ism_shader.CreateShader("ISM.frag", GL_FRAGMENT_SHADER);
    ism_shader.LinkProgram();
    
  filter_shader.CreateProgram();
  filter_shader.CreateShader("filter.vert", GL_VERTEX_SHADER);
  filter_shader.CreateShader("filter.frag", GL_FRAGMENT_SHADER);
  filter_shader.LinkProgram();

    hatch_shader.CreateProgram();
    hatch_shader.CreateShader("hatch.vert", GL_VERTEX_SHADER);
    hatch_shader.CreateShader("hatch.frag", GL_FRAGMENT_SHADER);
    hatch_shader.LinkProgram();

  ////////////////////////////////hatch textures////////////////////////////////////////
  glGenTextures(6, TAM1);
  glBindTexture(GL_TEXTURE_2D, TAM1[0]);
    image = read_texture("./TAM1-1.rgb", &scene.width, &scene.height, &components);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, components, scene.width, scene.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    free(image);
  glBindTexture(GL_TEXTURE_2D, TAM1[1]);
    image = read_texture("./TAM2-1.rgb", &scene.width, &scene.height, &components);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, components, scene.width, scene.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    free(image);  
  glBindTexture(GL_TEXTURE_2D, TAM1[2]);
    image = read_texture("./TAM3-1.rgb", &scene.width, &scene.height, &components);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, components, scene.width, scene.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    free(image);
  glBindTexture(GL_TEXTURE_2D, TAM1[3]);
    image = read_texture("./TAM4-1.rgb", &scene.width, &scene.height, &components);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, components, scene.width, scene.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    free(image);
  glBindTexture(GL_TEXTURE_2D, TAM1[4]);
    image = read_texture("./TAM5-1.rgb", &scene.width, &scene.height, &components);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, components, scene.width, scene.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    free(image);
  glBindTexture(GL_TEXTURE_2D, TAM1[5]);
    image = read_texture("./TAM6-1.rgb", &scene.width, &scene.height, &components);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, components, scene.width, scene.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    free(image);


    //glActiveTexture(GL_TEXTURE2);
    //glBindTexture(GL_TEXTURE_2D, TAM1[0]);
  
    //glActiveTexture(GL_TEXTURE3);
    //glBindTexture(GL_TEXTURE_2D, TAM1[1]);
  
    //glActiveTexture(GL_TEXTURE4);
    //glBindTexture(GL_TEXTURE_2D, TAM1[2]);
    //
    //glActiveTexture(GL_TEXTURE5);
    //glBindTexture(GL_TEXTURE_2D, TAM1[3]);
    //
    //glActiveTexture(GL_TEXTURE6);
    //glBindTexture(GL_TEXTURE_2D, TAM1[4]);
    //
    //glActiveTexture(GL_TEXTURE7);
    //glBindTexture(GL_TEXTURE_2D, TAM1[5]);
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Create the room
    CreatRoom(scene, splats);
}

////////////////////////////////////////////////////////////////////////
// Procedure DrawScene is called whenever the scene needs to be drawn.
void DrawScene(Scene &scene, Scene &splats)
{   
    float viewMatrix[16];
  float lightViewMatrix[16];
  float lightProjectionMatrix[16];
    int loc;

  // Moving from unit cube [-1,1] to [0,1]  
  const float bias[16] = {  
    0.5, 0.0, 0.0, 0.0, 
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 0.5, 0.0,
    0.5, 0.5, 0.5, 1.0};

    ///////////////////////////////////////////////////////////////////
    // Pass 1: Draw with light's transformatiosn into the S-buffer.
    ///////////////////////////////////////////////////////////////////

    // Set the light's position. 
    float lx = cos(scene.lightSpin);
    float ly = sin(scene.lightSpin);
    float  r = scene.lightTilt;
    float lPos[4] = {r*lx, r*ly, 0.98, 1.0};

    // Calculate the light's view frustum parameters
    float front = 0.05;
    float h = 2.0f*front;
    float w = h;

    // Select drawing to the s-buffer, set its viewport, and clear
    scene.sBuffer.Bind();
    glViewport(0,0,scene.width, scene.height);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

    // Draw scene using pass1 shader program
    shader1.Use();

    // Projection transformation for light (in gl_ProjectionMatrix)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); 
    glFrustum(-w, w, -h, h, front, 5.0);

  glGetFloatv(GL_PROJECTION_MATRIX, lightProjectionMatrix);
    
    // View transformation for light (in viewMatrix)
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(lPos[0],lPos[1],lPos[2], 
              lPos[0],lPos[1],lPos[2]-1.0, 
              0.0, 1.0, 0.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, lightViewMatrix);
    loc = glGetUniformLocation(shader1.program, "viewMatrix");
    glUniformMatrix4fv(loc, 1, 0, lightViewMatrix);

    // Model transformation for hierarchical model (in gl_ModelViewMatrix)
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
     
    //@@ Draw the room
    glCallList(scene.roomDL);

    // Unuse shader program, and unbind the FBO
    shader1.Unuse();
    scene.sBuffer.Unbind();

    ///////////////////////////////////////////////////////////////////
    // Pass 2: Draw with viewing transformatiosn into the G-buffer.
    ///////////////////////////////////////////////////////////////////

    double sx, sy;
    if (scene.width < scene.height) {
        sx = 0.4f*scene.front;
        sy = sx * scene.height/scene.width; }
    else {
        sy = 0.4f*scene.front;
        sx = sy * scene.width/scene.height; }

    // Select drawing to the g-buffer, set its viewport, and clear
    scene.gBuffer.Bind();
    glViewport(0,0,scene.width, scene.height);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

    // Use pass 2 shader program
    shader2.Use();

    // Projection transformation for eye (in gl_ProjectionMatrix)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-sx, sx, -sy, sy, scene.front, 100.0);

  float CamPos[3];
    // View transformation for eye (in viewMatrix)
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(scene.translatex, scene.translatey, -scene.zoom);
  CamPos[0] = scene.translatex;
  CamPos[1] = scene.translatey;
  CamPos[2] = -scene.zoom;

    glRotatef(scene.eyeTilt, 1.0f, 0.0f, 0.0f);
  float t1 = CamPos[0];
  float t2 = CamPos[1]*cos(scene.eyeTilt *rad)+CamPos[2]*sin(scene.eyeTilt * rad);
  float t3 = -CamPos[1]*sin(scene.eyeTilt *rad)+CamPos[2]*cos(scene.eyeTilt * rad);
  CamPos[0] = t1;
  CamPos[1] = t2;
  CamPos[2] = t3;


    glRotatef(scene.eyeSpin, 0.0f, 0.0f, 1.0f);
  t1 = CamPos[0]*cos(scene.eyeSpin *rad)+CamPos[1]*sin(scene.eyeSpin * rad);
    t2 = -CamPos[0]*sin(scene.eyeSpin *rad)+CamPos[1]*cos(scene.eyeSpin * rad);
  t3 = CamPos[2];
  CamPos[0] = t1;
  CamPos[1] = t2;
  CamPos[2] = t3;

    glGetFloatv(GL_MODELVIEW_MATRIX, viewMatrix);
    loc = glGetUniformLocation(shader2.program, "viewMatrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, viewMatrix);

  
    loc = glGetUniformLocation(shader2.program, "CamPos");
    glUniform3fv(loc, 1, CamPos);

    // Model matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Tell OpenGL where the light sits
    glLightfv(GL_LIGHT0, GL_POSITION, lPos);

    glColor3f(1.0, 0.0, 0.0);
    //@@ Draw the room
    glCallList(scene.roomDL);
        
    // Draw a small white dot at the position of the light.
    glDisable(GL_LIGHTING);
    glColor4fv(whiteColor);
    glPointSize(15.0);
    glEnable(GL_POINT_SMOOTH);
    glBegin(GL_POINTS);
    glColor4fv(whiteColor);
    glVertex3f(lPos[0],lPos[1],lPos[2]);
    glEnd();

    // Unuse shader program, and unbind the gBuffer
    shader2.Unuse();
    scene.gBuffer.Unbind();
        
    glBindTexture(GL_TEXTURE_2D, 0); // Un-Bind texture

  ///////////////////////////////////////////////////////////////////
    // Pass 3: Draw the splats into the ISM's
    ///////////////////////////////////////////////////////////////////

     //Select drawing to the ISM, set its viewport, and clear
  scene.ISM.Bind();
    glViewport(0,0,NUM_ISM*ISM_RES, NUM_ISM*ISM_RES);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    
  // Draw scene using pass1 shader program
    ism_shader.Use();

  //makes the splats sqaures
  glEnable(GL_PROGRAM_POINT_SIZE_ARB);
  glDisable(GL_POINT_SMOOTH);



    // Projection transformation for light (in gl_ProjectionMatrix)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // View transformation for light (in viewMatrix)
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
     glTranslatef(scene.translatex, scene.translatey, -scene.zoom);
    glRotatef(scene.eyeTilt, 1.0f, 0.0f, 0.0f);
    glRotatef(scene.eyeSpin, 0.0f, 0.0f, 1.0f);
    glGetFloatv(GL_MODELVIEW_MATRIX, viewMatrix);
    glUniformMatrix4fv(glGetUniformLocation(ism_shader.program, "viewMatrix"), 1, 0, viewMatrix);

  ism_shader.ActivateTexture(0, "sBuffer0", scene.sBuffer.textures[0]);
  ism_shader.ActivateTexture(1, "sBuffer1", scene.sBuffer.textures[1]);

    glUniform1i(glGetUniformLocation(ism_shader.program, "NUM_ISM"), NUM_ISM);
  glUniform1i(glGetUniformLocation(ism_shader.program, "ISM_RES"), ISM_RES);
  glUniform1f(glGetUniformLocation(ism_shader.program, "far"), 100.0f);
  glUniform1f(glGetUniformLocation(ism_shader.program, "near"), scene.front);

    // Model transformation for hierarchical model (in gl_ModelViewMatrix)
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

  //draw the splats
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, &splat_vertices[0]);

  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, 0, &splat_ism_positions[0]);

  glDrawArrays(GL_POINTS, 0, splat_vertices.size()/3);

  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);

  glDisable(GL_PROGRAM_POINT_SIZE_ARB);
  glEnable(GL_POINT_SMOOTH);

    // Unuse shader program, and unbind the FBO
    ism_shader.Unuse();
    scene.ISM.Unbind();
  glBindTexture(GL_TEXTURE_2D, 0);

  ///////////////////////////////////////////////////////////////////
    // Pass 4: Draw the image filter to fbo
    ///////////////////////////////////////////////////////////////////

     //Select drawing to the filterbuffer, set its viewport, and clear
  scene.fBuffer.Bind();
    // Set the viewport, and clear the screen
    glViewport(0,0,scene.width, scene.height);
    glClearColor(0.0,0.0,0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

    // Draw scene using filter shader program
    filter_shader.Use();
    
  filter_shader.ActivateTexture(0, "Normal", scene.gBuffer.textures[0]);
  filter_shader.ActivateTexture(1, "WorldPos", scene.gBuffer.textures[1]);
  filter_shader.ActivateTexture(2, "colorMap", scene.gBuffer.textures[2]);

  glUniform1i(glGetUniformLocation(filter_shader.program, "width"), scene.width);
    glUniform1i(glGetUniformLocation(filter_shader.program, "height"), scene.height);

  glBegin(GL_QUADS);
    glVertex2f(-1.0, -1.0);
    glVertex2f( 1.0, -1.0);
    glVertex2f( 1.0,  1.0);
    glVertex2f(-1.0,  1.0);
    glEnd();

    // Unuse shader program, and unbind the FBO
    filter_shader.Unuse();
    scene.fBuffer.Unbind();
  glBindTexture(GL_TEXTURE_2D, 0);

  ///////////////////////////////////////////////////////////////////
    // Pass 5: Draw the hatch filter to fbo
    ///////////////////////////////////////////////////////////////////

     //Select drawing to the filterbuffer, set its viewport, and clear
  scene.hBuffer.Bind();
    // Set the viewport, and clear the screen
    glViewport(0,0,scene.width, scene.height);
    glClearColor(0.0,0.0,0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

    // Draw scene using filter shader program
    hatch_shader.Use();

  glUniform1i(glGetUniformLocation(hatch_shader.program, "width"), scene.width);
    glUniform1i(glGetUniformLocation(hatch_shader.program, "height"), scene.height);
  hatch_shader.ActivateTexture(0,"hatch1",TAM1[1]);
  hatch_shader.ActivateTexture(1, "colorMap", scene.gBuffer.textures[2]);

  glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-sx, sx, -sy, sy, scene.front, 100.0);

    // View transformation for eye (in viewMatrix)
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(scene.translatex, scene.translatey, -scene.zoom);
  CamPos[0] = scene.translatex;
  CamPos[1] = scene.translatey;
  CamPos[2] = -scene.zoom;

    glRotatef(scene.eyeTilt, 1.0f, 0.0f, 0.0f);
  t1 = CamPos[0];
  t2 = CamPos[1]*cos(scene.eyeTilt *rad)+CamPos[2]*sin(scene.eyeTilt * rad);
  t3 = -CamPos[1]*sin(scene.eyeTilt *rad)+CamPos[2]*cos(scene.eyeTilt * rad);
  CamPos[0] = t1;
  CamPos[1] = t2;
  CamPos[2] = t3;


    glRotatef(scene.eyeSpin, 0.0f, 0.0f, 1.0f);
  t1 = CamPos[0]*cos(scene.eyeSpin *rad)+CamPos[1]*sin(scene.eyeSpin * rad);
    t2 = -CamPos[0]*sin(scene.eyeSpin *rad)+CamPos[1]*cos(scene.eyeSpin * rad);
  t3 = CamPos[2];
  CamPos[0] = t1;
  CamPos[1] = t2;
  CamPos[2] = t3;

    glGetFloatv(GL_MODELVIEW_MATRIX, viewMatrix);
  loc = glGetUniformLocation(hatch_shader.program, "viewMatrix");
    glUniformMatrix4fv(loc, 1, GL_FALSE, viewMatrix);



    // Model matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

  
  glLightfv(GL_LIGHT0, GL_POSITION, lPos);
    glColor3f(1.0, 0.0, 0.0);


  //glCallList(scene.roomDL);
      glPushMatrix();
    glScalef(1.0, 1.0, 1.0);
    glTranslatef(-1.0, 1.0, 0.0);
    
    glDisable(GL_CULL_FACE);
    GLUquadricObj *cylinder;
    cylinder = gluNewQuadric();
    glRotatef(90.0, 1.0, 0.0, 0.0);
    gluQuadricDrawStyle(cylinder, GLU_FILL);
    gluQuadricTexture(cylinder, GL_TRUE);
    gluQuadricNormals(cylinder, GLU_SMOOTH);
    gluCylinder(cylinder, 1.0, 1.0, 2.0, 30.0, 30.0);
    glEnable(GL_CULL_FACE);

    glTranslatef(3.0, -1.0, 0.0);

    GLUquadricObj *sphere;
    sphere = gluNewQuadric();
    gluQuadricDrawStyle(sphere, GLU_FILL);
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    gluSphere(sphere, 1.0, 30.0, 30.0);
    glPopMatrix();

        // Draw a small white dot at the position of the light.
    //glDisable(GL_LIGHTING);
    //glColor4fv(whiteColor);
    //glPointSize(15.0);
    //glEnable(GL_POINT_SMOOTH);
    //glBegin(GL_POINTS);
    //glColor4fv(whiteColor);
    //glVertex3f(lPos[0],lPos[1],lPos[2]);
    //glEnd();



    // Unuse shader program, and unbind the FBO
    hatch_shader.Unuse();
    scene.hBuffer.Unbind();
  glBindTexture(GL_TEXTURE_2D, 0);

    ///////////////////////////////////////////////////////////////////
    // Pass 6: Draw a full screen quad and pass the gBuffer and sBuffer
    ///////////////////////////////////////////////////////////////////

     //Draw scene using pass1 shader program
    shader3.Use();

    // Set the viewport, and clear the screen
    glViewport(0,0,scene.width, scene.height);
    glClearColor(0.5,0.5, 0.5, 1.0);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

    // Load s- and g- buffer textures into successive texture units
    shader3.ActivateTexture(0, "sBuffer0", scene.sBuffer.textures[0]);
  shader3.ActivateTexture(1, "sBuffer1", scene.sBuffer.textures[1]);
  shader3.ActivateTexture(2, "sBuffer2", scene.sBuffer.textures[2]);

    shader3.ActivateTexture(4, "gBuffer0", scene.gBuffer.textures[0]);
  shader3.ActivateTexture(5, "gBuffer1", scene.gBuffer.textures[1]);
  shader3.ActivateTexture(6, "gBuffer2", scene.gBuffer.textures[2]);
  shader3.ActivateTexture(7, "gBuffer3", scene.gBuffer.textures[3]);

  shader3.ActivateTexture(8, "ISM", scene.ISM.textures[0]);
  shader3.ActivateTexture(9, "Filter", scene.fBuffer.textures[0]);
  shader3.ActivateTexture(10, "FilterDepth", scene.fBuffer.textures[1]);
  shader3.ActivateTexture(11, "FilterNormal", scene.fBuffer.textures[2]);
  shader3.ActivateTexture(12, "Hatch", scene.hBuffer.textures[0]);

    glUniform1i(glGetUniformLocation(shader3.program, "mode"), scene.mode);
    glUniform1i(glGetUniformLocation(shader3.program, "WIDTH"), scene.width);
    glUniform1i(glGetUniformLocation(shader3.program, "HEIGHT"), scene.height);

  glUniform4fv(glGetUniformLocation(shader3.program, "AmbienceColor"), 1, ambientColor);
  glUniform3fv(glGetUniformLocation(shader3.program, "CameraPosition"), 1, CamPos);
  glUniform3fv(glGetUniformLocation(shader3.program, "LightPosition"), 1, lPos);

    glUniformMatrix4fv(glGetUniformLocation(shader3.program, "lightViewMatrix"), 1, GL_FALSE, lightViewMatrix);
  glUniformMatrix4fv(glGetUniformLocation(shader3.program, "lightProjectionMatrix"), 1, GL_FALSE, lightProjectionMatrix);

  loc = glGetUniformLocation(shader3.program, "NumVPLs");
    glUniform1i(loc, NUM_ISM);


    glBegin(GL_QUADS);
    glVertex2f(-1.0, -1.0);
    glVertex2f( 1.0, -1.0);
    glVertex2f( 1.0,  1.0);
    glVertex2f(-1.0,  1.0);
    glEnd();

    // Unuse shader program
    shader3.Unuse();
        
}
