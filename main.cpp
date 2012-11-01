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
#include <sys/time.h>

using namespace std;

// Main variables in the program.  
#define MAINPROGRAM 
#include "variables.h" 
#include "readfile.h" // prototypes for readfile.cpp  
#include "ModelObj.h"

bool * key_states = new bool[256];

bool texturedAlready = false;
bool animate_fish = true;

//Animation Variables:
struct timeval timeStart,timeEnd,time_charStart,time_charEnd, time_skeletonStart, time_skeletonEnd, texStart, texEnd;
int time_start, time_end;
int delta_char_frame = 1;
bool pauseAnimations;

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
    std::cout << "press 'i' to zoom the camera in.\n";
    std::cout << "press 'o' to zoom the camera out.\n";
    std::cout << "press 't' to toggle texture. \n";
    std::cout << "press 'p' to toggle animation.\n";
    std::cout << "move with mouse to change the camera view.\n";
}



void keyboard(unsigned char key, int x, int y) {
    key_states[key] = true;
    if (key == 'p') {
        if (animate_fish) {
            animate_fish = false;
        } else {
            animate_fish = true;
        }
    }
    
}


void handleAnimation() {
    gettimeofday(&time_charEnd, NULL);
    if (animate_fish) {
        for (int i = 0; i < numobjects; i++) {
            if (objects[i].name == "fish") {
                objects[i].transform = glm::transpose(Transform::translate(0,-0.02,0)) * objects[i].transform;
                if ((objects[i].transform * glm::vec4(0,0,0,1)).y <= -20) {
                    
                    objects[i].transform = glm::transpose(Transform::translate(0,40,0)) * objects[i].transform;
                }
            }
        }
    }
    

    gettimeofday(&time_skeletonEnd, NULL);
    if((time_skeletonEnd.tv_sec - time_skeletonStart.tv_sec)*1000000.0+(time_skeletonEnd.tv_usec - time_skeletonStart.tv_usec) > 20000 && loadedSceneIndex == 0 && !pauseAnimations){
      vec3 axisOrigin = glm::vec3(0, 0, 1);
      mat4 rotOrigin = glm::mat4(Transform::rotate(-0.125, axisOrigin));
      rotOrigin = glm::transpose(rotOrigin);

      vec3 axisObj = glm::vec3(0, 1, 0);
      mat4 rotObj = glm::mat4(Transform::rotate(-0.008, axisObj));
      rotObj = glm::transpose(rotObj);

      objects[skeletonIndex].transform = rotOrigin * (objects[skeletonIndex].transform) * rotObj; 

      gettimeofday(&time_skeletonStart, NULL);
    }

    gettimeofday(&texEnd, NULL);
    if(!pauseAnimations){
      texAnimInterval = 2*(texEnd.tv_sec - texStart.tv_sec)%5;
    }
}

void valid_character_movement(glm::vec3 old_position, glm::vec3 new_position, glm::vec3 & char_dir, char key) {
    
    if (loadedSceneIndex == 1) {
        
        // Off the board
        if ( new_position.x < -19.8 || new_position.x > 19.8 ) {
            if (key == 'w' || key == 's') {
                char_dir.x = 0;
            } 
            if (key == 'a' || key == 'd' ) {
                char_dir.y = 0;
            }
        }
        if (new_position.y < -19.8 || new_position.y > 19.8 ) {
            if (key == 'w' || key == 's') {
                char_dir.y = 0;
            } 
            if (key == 'a' || key == 'd' ) {
                char_dir.x = 0;
            }
        }
        
        // Handle river / water
        if (((new_position.x > 12 && new_position.x < 17 && new_position.y > 0.8) ||  (new_position.x > 12 && new_position.x < 17 && new_position.y < -0.8)) && (old_position.y > 0.8 || old_position.y < -0.8)) {
            if (key == 'w' || key == 's') {
                char_dir.x = 0;
            } 
            if (key == 'a' || key == 'd' ) {
                char_dir.y = 0;
            }
        }
        if (((new_position.x > 12 && new_position.x < 17 && old_position.y < 0.8) && (new_position.x > 12 && new_position.x < 17 && old_position.y > -0.8))) {
            if (new_position.y > 0.8 || new_position.y < -0.8) {
                if (key == 'w' || key == 's') {
                    char_dir.y = 0;
                } 
                if (key == 'a' || key == 'd' ) {
                    char_dir.x = 0;
                }
            }   
        }
    }
    
}

void idleFunc ( ) {
    
    gettimeofday(&timeEnd, NULL);
    if ((timeEnd.tv_sec - timeStart.tv_sec)*1000000.0+(timeEnd.tv_usec - timeStart.tv_usec) > 33) {
        
        
        vec3 c_move_dir = 0.03f*glm::normalize(vec3(char_direction));
        //cout << char_position.x << " : " << char_position.y << endl;
        if (key_states['h']) {
            printHelp();
        }
        if (key_states[27]) { // Escape to quit                                                                                                                                                                   
            exit(0);
        }
        if (key_states['o']) {
            eye = center + glm::vec3(1.02*(eye.x-center.x), 1.02*(eye.y-center.y), 1.02*(eye.z-center.z));
            eyeinit = glm::vec3(1.02*eyeinit.x, 1.02*eyeinit.y, 1.02*eyeinit.z);
            
        }
        if (key_states['i']) {
            eye = center + glm::vec3(1/1.02*(eye.x-center.x), 1/1.02*(eye.y-center.y), 1/1.02*(eye.z-center.z));
            eyeinit = glm::vec3(1/1.02*eyeinit.x, 1/1.02*eyeinit.y, 1/1.02*eyeinit.z);
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
                valid_character_movement(vec3(character.transform * glm::vec4(0,0,0,1)),vec3((glm::transpose(Transform::translate(c_move_dir.x, c_move_dir.y, c_move_dir.z))*character.transform * glm::vec4(0,0,0,1))), c_move_dir, 'w');
                character.transform = glm::transpose(Transform::translate   (c_move_dir.x, c_move_dir.y, c_move_dir.z))*character.transform;
                eye += c_move_dir;
                center += c_move_dir;
                char_position += c_move_dir;
            }
            if (key_states['s']) {
                valid_character_movement(vec3(character.transform * glm::vec4(0,0,0,1)),vec3((glm::transpose(Transform::translate (-c_move_dir.x, -c_move_dir.y, -c_move_dir.z))*character.transform * glm::vec4(0,0,0,1))), c_move_dir, 's');
                character.transform = glm::transpose(Transform::translate(-c_move_dir.x, -c_move_dir.y, -c_move_dir.z))*character.transform;
                eye -= c_move_dir;
                center -= c_move_dir;
                char_position -= c_move_dir;
            }
            if (key_states['a']) {
                valid_character_movement(vec3(character.transform * glm::vec4(0,0,0,1)),vec3((glm::transpose(Transform::translate (-c_move_dir.y, c_move_dir.x, c_move_dir.z))*character.transform * glm::vec4(0,0,0,1))), c_move_dir, 'a');
                vec3 left_vec = vec3(-c_move_dir.y, c_move_dir.x, c_move_dir.z);
                character.transform = glm::transpose(Transform::translate(-c_move_dir.y, c_move_dir.x, c_move_dir.z))*character.transform;
                eye += left_vec;
                center += left_vec;
                char_position += left_vec;
            }
            if (key_states['d']) {
                valid_character_movement(vec3(character.transform * glm::vec4(0,0,0,1)),vec3((glm::transpose(Transform::translate(c_move_dir.y, -c_move_dir.x, c_move_dir.z))*character.transform * glm::vec4(0,0,0,1))), c_move_dir, 'd');
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
        if (loadedSceneIndex == 0) {
            collisionProcess();
        }
        gettimeofday(&timeStart, NULL);
    }
    
    handleAnimation();

}


void keyUp (unsigned char key, int x, int y) {
    key_states[key] = false;

    /* Single key presses */
    if (key == 't') {
            textureOn = !textureOn;
    }

    if (key == 'p') {
            pauseAnimations = !pauseAnimations;
    }
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
  textureOn = true;
  isTex = glGetUniformLocation(shaderprogram,"isTex") ;
  isBump = glGetUniformLocation(shaderprogram,"isBump") ;
        
  loadTex("images/textures/deathbed/washington.pbm", washington);
  
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) ; 
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR) ; 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) ;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) ;

  glEnable(GL_TEXTURE_2D) ; 
  glGenTextures(9, texNames);
  glBindTexture (GL_TEXTURE_2D, texNames[0]) ; 
				
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, washington) ;
  glDisable(GL_TEXTURE_2D) ;
  
  loadTex("images/textures/deathbed/spiral.pbm", spiral);
  
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) ; 
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR) ; 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) ;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) ;

  glEnable(GL_TEXTURE_2D) ; 
  glBindTexture (GL_TEXTURE_2D, texNames[1]) ; 
				
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, spiral) ;
  glDisable(GL_TEXTURE_2D) ;
  
  loadTex("images/textures/deathbed/fireplace.pbm", fireplace);
  
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) ; 
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR) ; 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) ;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) ;

  glEnable(GL_TEXTURE_2D) ; 
  glBindTexture (GL_TEXTURE_2D, texNames[2]) ; 
				
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, fireplace) ;
  glDisable(GL_TEXTURE_2D) ;


  /* Animated texture loading */
  loadTex("images/textures/deathbed/animated/0.pbm", anim0);
  
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) ; 
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR) ; 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) ;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) ;

  glEnable(GL_TEXTURE_2D) ; 
  glBindTexture (GL_TEXTURE_2D, texNames[3]) ; 
        
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, anim0) ;
  glDisable(GL_TEXTURE_2D) ;

  loadTex("images/textures/deathbed/animated/1.pbm", anim1);
  
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) ; 
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR) ; 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) ;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) ;

  glEnable(GL_TEXTURE_2D) ; 
  glBindTexture (GL_TEXTURE_2D, texNames[4]) ; 
        
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, anim1) ;
  glDisable(GL_TEXTURE_2D) ;

  loadTex("images/textures/deathbed/animated/2.pbm", anim2);
  
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) ; 
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR) ; 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) ;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) ;

  glEnable(GL_TEXTURE_2D) ; 
  glBindTexture (GL_TEXTURE_2D, texNames[5]) ; 
        
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, anim2) ;
  glDisable(GL_TEXTURE_2D) ;

  loadTex("images/textures/deathbed/animated/3.pbm", anim3);
  
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) ; 
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR) ; 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) ;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) ;

  glEnable(GL_TEXTURE_2D) ; 
  glBindTexture (GL_TEXTURE_2D, texNames[6]) ; 
        
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, anim3) ;
  glDisable(GL_TEXTURE_2D) ;

  loadTex("images/textures/deathbed/animated/4.pbm", anim4);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) ; 
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR) ; 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) ;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) ;

  glEnable(GL_TEXTURE_2D) ; 
  glBindTexture (GL_TEXTURE_2D, texNames[7]) ; 
        
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, anim4) ;
  glDisable(GL_TEXTURE_2D) ;


  loadTex("images/textures/deathbed/animated/4.pbm", anim4);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) ; 
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR) ; 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) ;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) ;

  glEnable(GL_TEXTURE_2D) ; 
  glBindTexture (GL_TEXTURE_2D, texNames[8]) ; 
        
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, anim4) ;
  glDisable(GL_TEXTURE_2D) ;

  loadTex("images/textures/deathbed/normalTextures/brickBumps.pbm", bump0);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) ; 
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR) ; 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) ;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) ;

  glEnable(GL_TEXTURE_2D) ; 
  glBindTexture (GL_TEXTURE_2D, texNames[8]) ; 
        
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, bump0) ;
  glDisable(GL_TEXTURE_2D) ;

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
  loadedSceneIndex = 1; //Default to one, will change when scene is first loaded
  startSceneFile = string("input/deathbed.scene");
  
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  gettimeofday(&timeStart, NULL);
  gettimeofday(&time_charStart, NULL);
  gettimeofday(&time_skeletonStart, NULL);
  gettimeofday(&texStart, NULL);
    
  initTextures();
  
  //Animation transitioning
  isTransitioning = false;
  pauseAnimations = false;
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
