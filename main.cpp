////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Contains main function to create a window and run engine
///
/// Basic support for directional light, depth buffer, setting up 3d projection,
/// setting up a 3d camera, drawing a cube model, and running a set fixed frame
/// rate.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Includes

// STL
#include <cmath>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <math.h>
#include "Object.h"
#include "glm/glm/vec3.hpp"
#include "glm/glm/vec2.hpp"
#include "glm/glm/trigonometric.hpp"
#include "glm/glm/geometric.hpp"
#include "glm/glm/gtx/normalize_dot.hpp"
using namespace std;

// GL
#if   defined(OSX)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include <GLUT/glut.h>
#elif defined(LINUX)
#include <GL/glut.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#endif
////////////////////////////////////////////////////////////////////////////////
// Global variables - avoid these


// Window
int g_width{1500};
int g_height{900};
int g_window{0};

// Camera
float g_theta{0.f};

// Frame rate
const unsigned int FPS = 60;
float g_frameRate{0.f};
std::chrono::high_resolution_clock::time_point g_frameTime{
  std::chrono::high_resolution_clock::now()};
  float g_delay{0.f};
  float g_framesPerSecond{0.f};

//Menu Ids
  int menuID;

//Background Color Coordinates
  GLfloat backgroundAplha=0.0;

//Point Size
  GLfloat pointSize=1.0;

//Line Size
  GLfloat lineSize =1.0;

//Line Style
  GLshort lineStyle=0xFFFF;

//Translate Controls
 float xf=0.0;
float yf=0.0;
float zf=0.0;

//Perspective coordinates
float horizontalAngle =0.0;
float verticalAngle = 0.0;
glm::vec3 CameraEye =  glm::vec3(0.0f,0.0f,0.0f);
glm::vec3 CameraAt =  glm::vec3(0.0f,0.0f,-1.0f);
glm::vec3 CameraUp =  glm::vec3(0.0f,1.0f,0.0f);

//Creating the variables for the vectors for face, Normals, Textures, and Vertexs
  vector <glm::vec3> v;
  int currentIndexVertex=0;
  vector <glm::vec3> normals;
  int currentIndexNormals=0;
  vector<glm::vec3> vertexArray;
  vector <glm::vec3> normalArray;
  vector <glm::vec2> textureArray; 
  vector <glm::vec3> colorArray;
  vector<int> colorCodeArray;
  int currentIndexFaces=0;
  vector<glm::vec2 >textures;
  int currentIndexTextures=0;
  int numVertex=0;
  int numIndicies=0;
  bool textureHere=true;
  vector<Object> objectVector;



////////////////////////////////////////////////////////////////////////////////
// Functions

////////////////////////////////////////////////////////////////////////////////
/// @brief Initialize GL settings
  void
  initialize() {
    glClearColor(0.f, 0.4f, 0.6f, 0.f);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);
  }

////////////////////////////////////////////////////////////////////////////////
/// @brief Callback for resize of window
///
/// Responsible for setting window size (viewport) and projection matrix.
  void
  resize(GLint _w, GLint _h) {
    g_width = _w;
    g_height = _h;

  // Viewport
    glViewport(0, 0, g_width, g_height);

  // Projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.f, GLfloat(g_width)/g_height, 0.01f, 1000.f);
  }

////////////////////////////////////////////////////////////////////////////////
/// @brief Timer function to fix framerate in a GLUT application
/// @param _v Value (not used here)
///
/// Note, this is rudametary and fragile.
  void
  timer(int _v) {
    if(g_window != 0) {
      glutPostRedisplay();

      g_delay = std::max(0.f, 1.f/FPS - g_frameRate);
      glutTimerFunc((unsigned int)(1000.f*g_delay), timer, 0);
    }
    else
      exit(0);
  }


  void drawObject()
{
   int normalCounter=-1;

    //Enables the basic drawing functions using user input functions
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1,lineStyle);
    glLineWidth(lineSize);
    glPointSize(pointSize);


   
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   if(!objectVector[0].getTriangle()){
           glBegin(GL_QUADS);
      for(int i=0; i <vertexArray.size(); i++){
        glColor3f(colorArray[colorCodeArray[i]].x,colorArray[colorCodeArray[i]].y ,colorArray[colorCodeArray[i]].z );

          if(i%4==0){
          normalCounter++;
          glNormal3f(normalArray[normalCounter].x,normalArray[normalCounter].y,normalArray[normalCounter].z);
          }

      glVertex3f(vertexArray[i].x,vertexArray[i].y,vertexArray[i].z);  
      glTexCoord2f(textureArray[i].x, textureArray[i].y);
     }
}
  
  else{
      glBegin(GL_TRIANGLES);
      for(int i=0; i <vertexArray.size(); i++){
        glColor3f(colorArray[colorCodeArray[i]].x,colorArray[colorCodeArray[i]].y ,colorArray[colorCodeArray[i]].z );


           if(i%3==0){
          normalCounter++;
          glNormal3f(normalArray[normalCounter].x,normalArray[normalCounter].y,normalArray[normalCounter].z);
          }

           glVertex3f(vertexArray[i].x,vertexArray[i].y,vertexArray[i].z); 
        if(textureArray.size()>0){ 
        glTexCoord2f(textureArray[i].x, textureArray[i].y);
        }
      }
  }
 glEnd();
 glDisable(GL_LINE_STIPPLE);
glDisable(GL_LINE_SMOOTH);

}


////////////////////////////////////////////////////////////////////////////////
/// @brief Draw function for single frame
  void
  draw() {
    using namespace std::chrono;
  //////////////////////////////////////////////////////////////////////////////
  // Clear
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.22,1,0.984,backgroundAplha);

  //////////////////////////////////////////////////////////////////////////////
  // Draw

  // Single directional light
    static GLfloat lightPosition[] = { 0.5f, 1.0f, 1.5f, 0.0f };
    static GLfloat whiteLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    static GLfloat darkLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT0, GL_AMBIENT, darkLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);


for(int o=0; o<objectVector.size(); o++){
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(CameraEye.x,CameraEye.y,CameraEye.z,CameraEye.x+CameraAt.x,CameraEye.y+CameraAt.y,CameraEye.z+CameraAt.z,CameraUp.x, CameraUp.y, CameraUp.z);
  
  glPushMatrix();
  glScalef(objectVector[o].getXScale(), objectVector[o].getYScale(), objectVector[o].getZScale());
  glTranslatef(objectVector[o].getTranslate().x,objectVector[o].getTranslate().y,objectVector[o].getTranslate().z);
  
  vertexArray = objectVector[o].getVertexArray();
  normalArray = objectVector[o].getNormalArray();
  textureArray = objectVector[o].getTextureArray();
  colorArray = objectVector[o].getColorArray();
  colorCodeArray = objectVector[o].getColorCodeArray();
  drawObject();
  glPopMatrix();
   
  }

//Making the ground
  glPushMatrix();
  glTranslatef(0,-100,100);
  glColor3f(0.376, 0.502, 0.22);
  glBegin(GL_QUADS);
  glVertex3f(-5000.0,150,10000.0);
  glVertex3f(5000.0,150, 10000.0);
  glVertex3f(5000.0,0,-10000.0);
  glVertex3f(-5000.0,0,-10000.0);
  glEnd();
  glPopMatrix();

 
  //////////////////////////////////////////////////////////////////////////////
  // Show
glutSwapBuffers();

  //////////////////////////////////////////////////////////////////////////////
  // Record frame time
high_resolution_clock::time_point time = high_resolution_clock::now();
g_frameRate = duration_cast<duration<float>>(time - g_frameTime).count();
g_frameTime = time;
g_framesPerSecond = 1.f/(g_delay + g_frameRate);
  //printf("FPS: %6.2f\n", g_framesPerSecond);

}
void moveCameraLeft(float degree){
       horizontalAngle-=degree;
     CameraAt.x= cos(glm::radians(verticalAngle))*cos(glm::radians(horizontalAngle));
      CameraAt.y= sin(glm::radians(verticalAngle));
      CameraAt.z = cos(glm::radians(verticalAngle))* sin(glm::radians(horizontalAngle));
}

////////////////////////////////////////////////////////////////////////////////
/// @brief Callback function for keyboard presses
/// @param _key Key
/// @param _x X position of mouse
/// @param _y Y position of mouse
void keyPressed(GLubyte _key, GLint _x, GLint _y) {
  switch(_key) {
    // Escape key : quit application
    case 27:
    std::cout << "Destroying window: " << g_window << std::endl;
    glutDestroyWindow(g_window);
    g_window = 0;
    break; 
 //Camera Looks Up
    case 119:
  verticalAngle+=3.0;
  if(verticalAngle>89.9) {
    verticalAngle=89.9;
  }
  CameraAt.x= cos(glm::radians(verticalAngle))*cos(glm::radians(horizontalAngle));
  CameraAt.y= sin(glm::radians(verticalAngle));
  CameraAt.z = cos(glm::radians(verticalAngle))* sin(glm::radians(horizontalAngle));
    break;
//Camera Looks Down
    case 115:
 verticalAngle-=3.0;
  if(verticalAngle<-89.9) {
    verticalAngle=-89.9;
  }
  CameraAt.x= cos(glm::radians(verticalAngle))*cos(glm::radians(horizontalAngle));
  CameraAt.y= sin(glm::radians(verticalAngle));
  CameraAt.z = cos(glm::radians(verticalAngle))* sin(glm::radians(horizontalAngle));
    break;

//Camera Left
    case 97:
      moveCameraLeft(-3.0);
    break;

  //Camera Right
    case 100:
     horizontalAngle+=3.0;
    CameraAt.x= cos(glm::radians(verticalAngle))*cos(glm::radians(horizontalAngle));
      CameraAt.y= sin(glm::radians(verticalAngle));
      CameraAt.z = cos(glm::radians(verticalAngle))* sin(glm::radians(horizontalAngle));
    break;

    case 114:
    CameraEye.y+=1.0;
    break;

    case 102:
    CameraEye.y-=1.0;
    break;

    case 112:
    g_theta+=2.0f;
        
      
    cout << "Theta: " << glm::radians(g_theta) << " Before x: " << CameraEye.x << " Before z: " << CameraEye.z << endl;
    CameraEye.x = (10)*sin(glm::radians(g_theta)) + cos(glm::radians(verticalAngle))*cos(glm::radians(horizontalAngle));
    CameraEye.z = (10)*cos(glm::radians(g_theta)) + cos(glm::radians(verticalAngle))* sin(glm::radians(horizontalAngle));
  
     
    cout << "After x: " << CameraEye.x << " after z: " << CameraEye.z << endl;
    break;

    // Unhandled
    default:
    std::cout << "Unhandled key: " << (int)(_key) << std::endl;
    break;
  }
}





////////////////////////////////////////////////////////////////////////////////
/// @brief Callback function for keyboard presses of special keys
/// @param _key Key
/// @param _x X position of mouse
/// @param _y Y position of mouse
void
specialKeyPressed(GLint _key, GLint _x, GLint _y) {
  float CameraSpeed =0.5;
  switch(_key) {
    // Arrow keys
    case GLUT_KEY_LEFT:
    CameraEye -= glm::normalize(glm::cross(CameraAt,CameraUp))*CameraSpeed;
    break;
    case GLUT_KEY_RIGHT:
    CameraEye += glm::normalize(glm::cross(CameraAt,CameraUp))*CameraSpeed;
    break;
    case GLUT_KEY_UP:
    CameraEye += CameraSpeed*CameraAt;
    break;

    case GLUT_KEY_DOWN:
    CameraEye -= CameraSpeed*CameraAt;
    break;
    // Unhandled
    default:
    std::cout << "Unhandled special key: " << _key << std::endl;
    break;
  }
}



//Creates the Main Menu
void mainMenuHandler(int choice){
  switch (choice){


    default:
    cout << "Le default " << endl;
    break;
  }
}


void renderScene(std::string fileName){
  ifstream inFile;
   inFile.open(fileName.c_str());
   string line;

   while (getline(inFile,line))
      { 
      
        char objectName[256];
        float xt,yt,zt,sx,sy,sz,r,g,b;
    
        sscanf(line.c_str(),"%s %f %f %f %f %f %f %f %f %f ", &objectName, &xt, &yt, &zt, &sx, &sy, &sz, &r, &g, &b);

       
        if(strcmp(objectName, "tree.obj")==0){
          Object o;
          o.readFile("tree.obj");
          o.setTranslate(xt,yt,zt);
          o.setScale(sx,sy,sz);
          objectVector.push_back(o);
        }

        if(strcmp(objectName, "theBench.obj")==0){
          Object o;
          o.readFile("theBench.obj");
          o.setTranslate(xt,yt,zt);
          o.setScale(sx,sy, sz);
          objectVector.push_back(o);
        }
        if(strcmp(objectName, "house.obj")==0){
          Object o;
          o.readFile("house.obj");
          o.setTranslate(xt,yt,zt);
          o.setScale(sx,sy, sz);
          objectVector.push_back(o);
        }
       }
  
      inFile.close();


    //Making the ground

}

////////////////////////////////////////////////////////////////////////////////
// Main

////////////////////////////////////////////////////////////////////////////////
/// @brief main
/// @param _argc Count of command line arguments
/// @param _argv Command line arguments
/// @return Application success status
int
main(int _argc, char** _argv) {
  //////////////////////////////////////////////////////////////////////////////
  // Initialize GLUT Window
  std::cout << "Initializing GLUTWindow" << std::endl;
  // GLUT
  glutInit(&_argc, _argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowPosition(50, 100);
  glutInitWindowSize(g_width, g_height); // HD size
  g_window = glutCreateWindow("Spiderling: A Rudamentary Game Engine");
  
  renderScene("scene.txt");

  // GL
  initialize();



  menuID =glutCreateMenu(mainMenuHandler); 
  

  glutAttachMenu(GLUT_RIGHT_BUTTON);



  //////////////////////////////////////////////////////////////////////////////
  // Assign callback functions
  std::cout << "Assigning Callback functions" << std::endl;
  glutReshapeFunc(resize);
  glutDisplayFunc(draw);
  glutKeyboardFunc(keyPressed);
  glutSpecialFunc(specialKeyPressed);
  glutTimerFunc(1000/FPS, timer, 0);

  // Start application
  std::cout << "Starting Application" << std::endl;
  glutMainLoop();


  return 0;
}

#if   defined(OSX)
#pragma clang diagnostic pop
#endif
