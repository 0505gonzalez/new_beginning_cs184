#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include <GL/glut.h>
#include "shaders.h"
#include "Transform.h"
#include <math.h>
#include "Character.cpp"
#include <sys/time.h>

using namespace std;

// Main variables in the program.  
#define MAINPROGRAM 
#include "variables.h" 
#include "readfile.h" // prototypes for readfile.cpp  
#include "ModelObj.h"

bool * key_states = new bool[256];


//Animation Variables:
struct timeval timeStart,timeEnd,time_charStart,time_charEnd;
int time_start, time_end;
int delta_char_frame = 1;

void display(void) ;  // prototype for display function.
void collisionProcess(void);
void loadTex(const char * filename, GLubyte textureLocation[256][256][3]);

// Very basic code to read a ppm file
// And then set up buffers for texture coordinates
void loadTex (const char * filename, GLubyte textureLocation[256][256][3]) {
	int i,j,k ;
	FILE * fp ; 
	GLint err ; 
	assert(fp = fopen(filename,"rb")) ;
	fscanf(fp,"%*s %*d %*d %*d%*c") ;
	for (i = 0 ; i < 256 ; i++)
		for (j = 0 ; j < 256 ; j++)
			for (k = 0 ; k < 3 ; k++)
				fscanf(fp,"%c",&(textureLocation[i][j][k])) ;
	fclose(fp) ;  
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
    std::cout << "press 'w' to advance the character forward.\n";
    std::cout << "press 's' to advance the character backward.\n";
    std::cout << "press 'a' to move the character to the left.\n";
    std::cout << "press 'd' to move the character to the right.\n";
    std::cout << "move with mouse to change the camera view.\n";
}



void keyboard(unsigned char key, int x, int y) {
    key_states[key] = true;
    
}


void handleAnimation() {
    
}


void idleFunc ( ) {

    gettimeofday(&timeEnd, NULL);
    if ((timeEnd.tv_sec - timeStart.tv_sec)*1000000.0+(timeEnd.tv_usec - timeStart.tv_usec) > 33) {


        vec3 c_move_dir = 0.03f*glm::normalize(vec3(char_direction));
        cout << char_position.x << " : " << char_position.y << endl;
        if (key_states['h']) {
            printHelp();
        }
        if (key_states[27]) { // Escape to quit                                                                                                                                                                   
            exit(0);
        }
        if (key_states['w'] || key_states['a'] || key_states['s'] || key_states['d']) {
            gettimeofday(&time_charEnd, NULL);
            if ((time_charEnd.tv_sec - time_charStart.tv_sec)*1000000.0+(time_charEnd.tv_usec - time_charStart.tv_usec) > 50000) {
                if (char_frame == 6) {
                    delta_char_frame = -1;
                } else if (char_frame == -6) {
                    delta_char_frame = 1;
                }
                char_frame += delta_char_frame;
                gettimeofday(&time_charStart, NULL);
            }
            if (key_states['w']) {
                character.transform = glm::transpose(Transform::translate   (c_move_dir.x, c_move_dir.y, c_move_dir.z))*character.transform;
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
        } else {
            char_frame = 0;
            delta_char_frame = 1;
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
        collisionProcess();
        
        gettimeofday(&timeStart, NULL);
    }
    
    handleAnimation();

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

void collisionProcess(){
    if(char_position.y < -15 && char_position.x < 15 && char_position.x > 13){
	string fileName("input/paradise.scene");
	loadScene(fileName);
    }
    
    glutPostRedisplay();
}

/* Initializes textures */
void initTextures(){
  //isTex = glGetUniformLocation(shaderprogram, isTex);
  glGenTextures(1, texNames) ;
  
  loadTex("images/textures/deathbed/dirt.pbm", dirttexture);
  
  glBindTexture(GL_TEXTURE_2D, texNames[0]);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, dirttexture);
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
  
  gettimeofday(&timeStart, NULL);
  gettimeofday(&time_charStart, NULL);
    
  //initTextures();
}

int main (int argc, char* argv[]) {
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
    return 0;
}
