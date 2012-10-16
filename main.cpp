#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include <GL/glut.h>
#include "shaders.h"
#include "Transform.h"
#include <FreeImage.h>
#include <math.h>
#include "Character.cpp"

using namespace std;

// Main variables in the program.  
#define MAINPROGRAM 
#include "variables.h" 
#include "readfile.h" // prototypes for readfile.cpp  

bool * key_states = new bool[256];

void display(void) ;  // prototype for display function.  

void reshape(int width, int height) {
  w = width;
  h = height;
  mat4 mv; // for lookat
  float aspect = w / (float) h, zNear = 0.1, zFar = 99.0 ;
  glMatrixMode(GL_PROJECTION);
  mv = Transform::perspective(fovy,aspect,zNear,zFar);
  mv = glm::transpose(mv);
  glLoadMatrixf(&mv[0][0]);
  glViewport(0,0,w,h);
}

void printHelp() {
  std::cout << "\npress 'h' to print this message again.\n";
}



void keyboard(unsigned char key, int x, int y) {
  key_states[key] = true;
}

void idleFunc ( ) {
  vec3 c_move_dir = 0.03f*glm::normalize(vec3(char_direction));
  float amount_rot = 3.0;

  if (key_states['+']) {

  }
  if (key_states['h']) {
    printHelp();
  }
  if (key_states[27]) { // Escape to quit                                                                                                                                                                   
    exit(0);
  }
  if (key_states['w']) {
    character.transform = glm::transpose(Transform::translate(c_move_dir.x, c_move_dir.y, c_move_dir.z))*character.transform;
    eye += c_move_dir;
    center += c_move_dir;
    char_position += c_move_dir;
  }
  if (key_states['s']) {
    character.transform = glm::transpose(Transform::translate(-c_move_dir.x, -c_move_dir.y, -c_move_dir.z))*character.transform;
    eye -= c_move_dir;
    center -= c_move_dir;
    char_position -= c_move_dir;
  }
  if (key_states['a']) {
    distance_eye_to_eyeinit = eye - eyeinit;
    character.transform = glm::transpose(Transform::translate(char_position.x, char_position.y, char_position.z))*glm::transpose(mat4(Transform::rotate(amount_rot, up)))*glm::transpose(Transform::translate(-char_position.x, -char_position.y, -char_position.z))*character.transform;
    Transform::left(amount_rot, eyeinit, up);
    eye = eyeinit + distance_eye_to_eyeinit;
    Transform::left(amount_rot, char_direction, up);
  }
  if (key_states['d']) {
    distance_eye_to_eyeinit = eye - eyeinit;
      character.transform = glm::transpose(Transform::translate(char_position.x, char_position.y, char_position.z))*glm::transpose(mat4(Transform::rotate(-amount_rot, up)))*glm::transpose(Transform::translate(-char_position.x, -char_position.y, -char_position.z))*character.transform;
      Transform::left(-amount_rot, eyeinit, up);
      eye = eyeinit + distance_eye_to_eyeinit;
      Transform::left(-amount_rot, char_direction, up);
  }
  glutPostRedisplay();
}


void keyUp (unsigned char key, int x, int y) {
  key_states[key] = false;
}

void mouse(int button, int state, int x, int y) {
  if (state == GLUT_DOWN) {
    if (button == GLUT_LEFT_BUTTON) {
      mousex = x;
      mousey = y;
    }
  }
  glutPostRedisplay();
}

void mousedrag(int x, int y) {
  distance_eye_to_eyeinit = eye - eyeinit;
  Transform::up((mousey-y)*1.0/h*360.0, eyeinit, up);
  eye = eyeinit + distance_eye_to_eyeinit;
  mousex = x;
  mousey = y;
  glutPostRedisplay();
}

void init() {
  // Initialize shaders
  vertexshader = initshaders(GL_VERTEX_SHADER, "shaders/light.vert.glsl") ;
  fragmentshader = initshaders(GL_FRAGMENT_SHADER, "shaders/light.frag.glsl") ;
  shaderprogram = initprogram(vertexshader, fragmentshader) ; 
  enablelighting = glGetUniformLocation(shaderprogram,"enablelighting") ;
  lightpos = glGetUniformLocation(shaderprogram,"lightposn") ;       
  lightcol = glGetUniformLocation(shaderprogram,"lightcolor") ;       
  numusedcol = glGetUniformLocation(shaderprogram,"numused") ;       
  ambientcol = glGetUniformLocation(shaderprogram,"ambient") ;       
  diffusecol = glGetUniformLocation(shaderprogram,"diffuse") ;       
  specularcol = glGetUniformLocation(shaderprogram,"specular") ;       
  emissioncol = glGetUniformLocation(shaderprogram,"emission") ;       
  shininesscol = glGetUniformLocation(shaderprogram,"shininess") ;
  
  //Init scene names
  startSceneFile = string("input/whole_scene.txt");
}

int main (int argc, char* argv[]) {
  FreeImage_Initialise();
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutCreateWindow("HW4: A New Beginning");
  init();
  loadScene(startSceneFile);
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(mousedrag);
  glutKeyboardUpFunc(keyUp);
  glutReshapeWindow(w,h);
  glutIdleFunc(idleFunc);
  for (int i = 0; i < 256; i++) {
    key_states[i] = false;
  }
  printHelp();
  glutMainLoop();
  FreeImage_DeInitialise();
  return 0;
}
