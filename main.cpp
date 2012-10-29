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
#include "ModelObj.h"

bool * key_states = new bool[256];

void display(void) ;  // prototype for display function.

void saveScreenshot(string fname) {
	int pix = w * h;
	BYTE pixels[3*pix];
	glReadBuffer(GL_FRONT);
	glReadPixels(0,0,w,h,GL_BGR,GL_UNSIGNED_BYTE, pixels);
    
	FIBITMAP *img = FreeImage_ConvertFromRawBits(pixels, w, h, w * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);
    
	std::cout << "Saving screenshot: " << fname << "\n";
    
	FreeImage_Save(FIF_PNG, img, fname.c_str(), 0);
}


void reshape(int width, int height) {
    w = width;
    h = height;
    mat4 mv; // for lookat
    float aspect = w / (float) h, zNear = 0.001, zFar = 99.99 ;
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
    
    if(key == 'p'){
        std::string fname("screenshot.png");
        saveScreenshot(fname);
    }
}

void idleFunc ( ) {
    vec3 c_move_dir = 0.03f*glm::normalize(vec3(char_direction));
    cout << char_position.x << " : " << char_position.y << endl;
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
        vec3 left_vec = vec3(-c_move_dir.y, c_move_dir.x, c_move_dir.z);
        character.transform = glm::transpose(Transform::translate(-c_move_dir.y, c_move_dir.x, c_move_dir.z))*character.transform;
        eye += left_vec;
        center += left_vec;
        char_position += left_vec;
    }
    if (key_states['d']) {
        vec3 right_vec = vec3(c_move_dir.y, -c_move_dir.x, c_move_dir.z);
        character.transform = glm::transpose(Transform::translate(c_move_dir.y, -c_move_dir.x, c_move_dir.z))*character.transform;
        eye += right_vec;
        center += right_vec;
        char_position += right_vec;
    }
    
    distance_eye_to_eyeinit = eye - eyeinit;
    float amount_rot_x = (w/2.0-mousex)*0.003/w*360.0;
    float amount_rot_y = -(h/2.0-mousey)*0.001/h*360.0;
    
    /* Don't allow movement in the center of the screen */
    if(mousex < (w/2.0 + w/5.0) && mousex > (w/2.0 - w/5.0)){
        amount_rot_x = 0.0;
    }
    if(mousey < (h/2.0 + h/3.0) && mousey > (h/2.0 - h/3.0)){
        amount_rot_y = 0.0;
    }
    
    if (elevation + amount_rot_y < min_elevation) {
        amount_rot_y = min_elevation - elevation;
    } else if (elevation + amount_rot_y > max_elevation) {
        amount_rot_y = max_elevation - elevation;
    }
    elevation += amount_rot_y;
    Transform::left(amount_rot_x, eyeinit, up);
    Transform::left(amount_rot_x, char_direction, up);
    character.transform = glm::transpose(Transform::translate(char_position.x, char_position.y, char_position.z))*glm::transpose(mat4(Transform::rotate(amount_rot_x, up)))*glm::transpose(Transform::translate(-char_position.x, -char_position.y, -char_position.z))*character.transform;
    Transform::up(amount_rot_y, eyeinit, up);
    eye = eyeinit + distance_eye_to_eyeinit;
    glutPostRedisplay();
}


void keyUp (unsigned char key, int x, int y) {
    key_states[key] = false;
}

void mouse(int x, int y) {
    mousex = x;
    mousey = y;
    if (mousex < 0) {
        mousex = 0;
    } else if (mousex > w) {
        mousex = w;
    }
    if (mousey < 0) {
        mousey = 0;
    } else if (mousey > h) {
        mousey = h;
    }
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
    startSceneFile = string("input/paradise.scene");
    
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
    glutKeyboardUpFunc(keyUp);
    glutReshapeWindow(w,h);
    glutIdleFunc(idleFunc);
    glutPassiveMotionFunc(mouse);
    for (int i = 0; i < 256; i++) {
        key_states[i] = false;
    }
    printHelp();
    glutMainLoop();
    FreeImage_DeInitialise();
    return 0;
}
