/*****************************************************************************/
/* This is the program skeleton for homework 2 in CS 184 by Ravi Ramamoorthi */
/* Extends HW 1 to deal with shading, more transforms and multiple objects   */
/*****************************************************************************/

/*****************************************************************************/
// This file is readfile.cpp.  It includes helper functions for matrix
// transformations for a stack (matransform) and to rightmultiply the
// top of a stack.  These functions are given to aid in setting up the
// transformations properly, and to use glm functions in the right way.
// Their use is optional in your program.


// The functions readvals and readfile do basic parsing.  You can of course
// rewrite the parser as you wish, but we think this basic form might be
// useful to you.  It is a very simple parser.

// Please fill in parts that say YOUR CODE FOR HW 2 HERE.
// Read the other parts to get a context of what is going on.

/*****************************************************************************/

// Basic includes to get this file to work.
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include <GL/glut.h>
#include "Transform.h"

using namespace std ;
#include "variables.h"
#include "readfile.h"

int lineCount = 0;

// The function below applies the appropriate transform to a 4-vector
void matransform(stack<mat4> &transfstack, GLfloat * values) {
    mat4 transform = transfstack.top() ;
    vec4 valvec = vec4(values[0],values[1],values[2],values[3]) ;
    vec4 newval = valvec * transform ;
    for (int i = 0 ; i < 4 ; i++) values[i] = newval[i] ;
}

void rightmultiply(const mat4 & M, stack<mat4> &transfstack) {
    mat4 &T = transfstack.top() ;
    // Right multiply M, but do this left to account for row/column major
    T = T*M  ;
}

// Function to read the input data values
// Use is optional, but should be very helpful in parsing.
bool readvals(stringstream &s, const int numvals, GLfloat * values) {
    for (int i = 0 ; i < numvals ; i++) {
        s >> values[i] ;
        if (s.fail()) {
            cout << "Line " << lineCount << ": Failed reading value " << i << " will skip\n" ;
            return false ;
        }
    }
    return true ;
}

void readfile(const char * filename) {
    string str, cmd ;
    ifstream in ;
    in.open(filename) ;
    if (in.is_open()) {
        
        // I need to implement a matrix stack to store transforms.
        // This is done using standard STL Templates
        stack <mat4> transfstack ;
        transfstack.push(mat4(1.0)) ;  // identity
        
        getline (in, str) ;
        while (in) {
	    lineCount++;
            if ((str.find_first_not_of(" \t\r\n") != string::npos) && (str[0] != '#')) {
                // Ruled out comment and blank lines
                
                stringstream s(str) ;
                s >> cmd ;
                int i ;
                GLfloat values[10] ; // position and color for light, colors for others
                // Up to 10 params for cameras.
                bool validinput ; // validity of input
                
                // Process the light, add it to database.
                // Lighting Command
                if (cmd == "light") {
                    if (numused == numLights) // No more Lights
                        cerr << "Reached Maximum Number of Lights " << numused << " Will ignore further lights\n" ;
                    else {
                        validinput = readvals(s, 8, values) ; // Position/color for lts.
                        if (validinput) {
                            // YOUR CODE FOR HW 2 HERE.
                            // Note that values[0...7] shows the read in values
                            // Make use of lightposn[] and lightcolor[] arrays in variables.h
                            // Those arrays can then be used in display too.
                            for(int i = 0; i < 4; i++){
                                lightposn[numused*4 + i] = values[i];
                                lightcolor[numused*4 + i] = values[i + 4];
                            }
                            ++numused ;
                        }
                    }
                }
                
                // Material Commands
                // Ambient, diffuse, specular, shininess
                // Filling this in is pretty straightforward, so I've left it in
                // the skeleton, also as a hint of how to do the more complex ones.
                // Note that no transforms/stacks are applied to the colors.
                
		else if (cmd == "backgroundColor") {
                    validinput = readvals(s, 4, values) ; // colors
                    if (validinput)
                        for (i = 0 ; i < 4 ; i++) backgroundColor[i] = values[i] ;
                }
		
                else if (cmd == "ambient") {
                    validinput = readvals(s, 4, values) ; // colors
                    if (validinput)
                        for (i = 0 ; i < 4 ; i++) ambient[i] = values[i] ;
                }
                else if (cmd == "diffuse") {
                    validinput = readvals(s, 4, values) ;
                    if (validinput)
                        for (i = 0 ; i < 4 ; i++) diffuse[i] = values[i] ;
                }
                else if (cmd == "specular") {
                    validinput = readvals(s, 4, values) ;
                    if (validinput)
                        for (i = 0 ; i < 4 ; i++) specular[i] = values[i] ;
                }
                else if (cmd == "emission") {
                    validinput = readvals(s, 4, values) ;
                    if (validinput)
                        for (i = 0 ; i < 4 ; i++) emission[i] = values[i] ;
                }
                else if (cmd == "shininess") {
                    validinput = readvals(s, 1, values) ;
                    if (validinput) shininess = values[0] ;
                }
                else if (cmd == "size") {
                    validinput = readvals(s,2,values) ;
                    if (validinput) { w = (int) values[0] ; h = (int) values[1] ; }
                }
                else if (cmd == "camera") {
                    validinput = readvals(s,10,values) ; // 10 values eye cen up fov
                    if (validinput) {
                        // YOUR CODE FOR HW 2 HERE
                        // Use all of values[0...9]
                        // You may need to use the upvector fn in Transform.cpp
                        // to set up correctly.
                        // Set eyeinit upinit center fovy in variables.h
                        eyeinit = glm::vec3(values[0], values[1], values[2]);
                        upinit = glm::vec3(values[6], values[7], values[8]);
                        center = glm::vec3(values[3], values[4], values[5]);
                        fovy = values[9];
                        elevation = glm::asin(values[2] / glm::sqrt(values[0]*values[0] + values[1]*values[1] + values[2]*values[2]))*180.0/pi;
                        if (elevation < min_elevation || elevation > max_elevation) {
                            cerr << "elevation is not between " << min_elevation << " and " << max_elevation << "degrees. Elevation: " << elevation << endl;
                            exit(1);
                        }
                    }
                }

		// elevation <min_elev> <max_elev> specifies the range of rotating up. Must call elevation in the beginning. The two arguments are specified in degrees relative to the xy plane.
		else if (cmd == "elevation") {
                  validinput = readvals(s,2,values);
                  if (validinput) {
                    min_elevation = values[0];
                    max_elevation = values[1];
                  }
                }
		

                // I've left the code for loading objects in the skeleton, so
                // you can get a sense of how this works.
                else if (cmd == "sphere" || cmd == "cube" || cmd == "teapot") {
                    if (numobjects == maxobjects) // No more objects
                        cerr << "Reached Maximum Number of Objects " << numobjects << " Will ignore further objects\n" ;
                    else {
                        validinput = readvals(s, 1, values) ;
                        if (validinput) {
                            object * obj = &(objects[numobjects]) ;
                            obj -> size = values[0] ;
                            for (i = 0 ; i < 4 ; i++) {
                                (obj -> ambient)[i] = ambient[i] ;
                                (obj -> diffuse)[i] = diffuse[i] ;
                                (obj -> specular)[i] = specular[i] ;
                                (obj -> emission)[i] = emission[i] ;
                            }
                            obj -> shininess = shininess ;
                            obj -> transform = transfstack.top() ;
                            if (cmd == "sphere") obj -> type = sphere ;
                            else if (cmd == "cube") obj -> type = cube ;
                            else if (cmd == "teapot") obj -> type = teapot ;
                        }
                        ++numobjects ;
                    }
                }

		else if (cmd == "tree1"  || cmd == "horse" || cmd == "cow" || 
            cmd == "sheep" || cmd == "couch" || cmd == "deadOak" || cmd == "coffeeTable" || 
            cmd == "fence_post" || cmd == "fence_rail" || cmd == "skeleton" || cmd == "desk" || cmd == "television" ||
             cmd == "bridge_plank" || cmd == "bridge_post" || cmd == "fish"  || cmd == "gravestone" || cmd == "gorge") {

		  if (numobjects == maxobjects) // No more objects
		    cerr << "Reached Maximum Number of Objects " << numobjects << " Will ignore further objects\n" ;
		  else {
		    validinput = readvals(s, 0, values);
		    if(validinput) {
		      object *obj = &(objects[numobjects]);
		      for (i = 0; i < 4; i++) {
                  (obj -> ambient)[i] = ambient[i];
                  (obj -> diffuse)[i] = diffuse[i];
                  (obj -> specular)[i] = specular[i];
                  (obj -> emission)[i] = specular[i];
		      }
		      obj -> type = modelobj;
		      obj -> shininess = shininess;
		      obj -> transform = transfstack.top();

		      if (cmd == "tree1") {
			obj -> name = ((std::string)("tree1"));
			obj -> file_path = ((std::string)("images/tree/tree4b_lod1.obj"));
			obj -> shape_sides = 4;
		      }
		      if (cmd == "deadOak") {
			obj -> name = ((std::string)("deadOak"));
			obj -> file_path = ((std::string)("images/dead_tree2/tree_oak.obj"));
			obj -> shape_sides = 3;
                  
              }  

		      if (cmd == "coffeeTable") {
			obj -> name = ((std::string)("coffeeTable"));
			obj -> file_path = ((std::string)("images/furniture/table.obj"));
			obj -> shape_sides = 3;
		      }

		      if (cmd == "horse") {
			obj -> name = ((std::string)("horse"));
			obj -> file_path = ((std::string)("images/horse-obj/horse-obj.obj"));
			obj -> shape_sides = 4;
		      }
		      if (cmd == "cow") {
			obj -> name = ((std::string)("cow"));
			obj -> file_path = ((std::string)("images/cow/cow.obj"));
			obj -> shape_sides = 3;
		      }
		      if (cmd == "sheep") {
			obj -> name = ((std::string)("sheep"));
			obj -> file_path = ((std::string)("images/sheep/sheep.obj"));
			obj -> shape_sides = 3;
		      }
		      if (cmd == "fence_post") {
			obj -> name = ((std::string)("fence_post"));
			obj -> file_path = ((std::string)("images/fence/fence_post.txt"));
			obj -> shape_sides = 3;
		      }
		      if (cmd == "fence_rail") {
			obj -> name = ((std::string)("fence_rail"));
			obj -> file_path = ((std::string)("images/fence/fence_rail.txt"));
			obj -> shape_sides = 3;
		      }
              if (cmd == "fish") {
            obj -> name = ((std::string)("fish"));
            obj -> file_path = ((std::string)("images/fish/fish.obj"));
            obj -> shape_sides = 3;
              }
		      if (cmd == "skeleton") {
			obj -> name = ((std::string)("skeleton"));
			obj -> file_path = ((std::string)("images/skeleton.obj"));
			obj -> shape_sides = 3;
            skeletonIndex = numobjects;
            std::cout << "skeleton Index: " << skeletonIndex <<std::endl;
		      }

               if (cmd == "desk") {
            obj -> name = ((std::string)("desk"));
            obj -> file_path = ((std::string)("images/desk.obj"));
            obj -> shape_sides = 3;
              }
              if (cmd == "television") {
            obj -> name = ((std::string)("television"));
            obj -> file_path = ((std::string)("images/television.obj"));
            obj -> shape_sides = 4;
              }
              if (cmd == "gorge") {
            obj -> name = ((std::string)("gorge"));
            obj -> file_path = ((std::string)("images/gorge/gorge.txt"));
            obj -> shape_sides = 4;
              }
              if (cmd == "gravestone") {
            obj -> name = ((std::string)("gravestone"));
            obj -> file_path = ((std::string)("images/gravestone/gravestone.txt"));
            obj -> shape_sides = 4;
              }
              if (cmd == "bridge_plank") {
            obj -> name = ((std::string)("bridge_plank"));
            obj -> file_path = ((std::string)("images/bridge/bridge_base.txt"));
            obj -> shape_sides = 4;
              }
              if (cmd == "bridge_post") {
            obj -> name = ((std::string)("bridge_post"));
            obj -> file_path = ((std::string)("images/bridge/bridge_post.txt"));
            obj -> shape_sides = 4;
              }
		      ++numobjects;
		      ++num_obj_models;
		    }
		  }
		}

		// TEMPORARY: I need an object to play around with.
		// Input: character <size> --- creates a cube of size "size".
		else if (cmd == "character") {
		    validinput = readvals(s,3,values);
		    if (validinput) {
		      object * obj = &(character);
		      for (i = 0; i < 4; i++) {
                  (obj -> ambient)[i] = ambient[i] ;
                  (obj -> diffuse)[i] = diffuse[i] ;
                  (obj -> specular)[i] = specular[i] ;
                  (obj -> emission)[i] = emission[i] ;
		      }
		      
		      obj -> shininess = shininess;
		      obj -> transform = transfstack.top();
		      obj -> type = modelobj;
		      use_char = true;
		      char_direction = vec3(values[0], values[1], values[2]);
		      char_position = vec3(0,0,0); // for simplicity, character starts at origin
              char_frame = 0;
		    }
		}

               else if (cmd == "translate") {
                    validinput = readvals(s,3,values) ;
                    if (validinput) {
                        // YOUR CODE FOR HW 2 HERE.
                        // Think about how the transformation stack is affected
                        // You might want to use helper functions on top of file.
		        mat4 curTransMat = glm::transpose(Transform::translate((float) values[0], (float) values[1], (float) values[2]));
                        rightmultiply(curTransMat, transfstack);
                    }
                }
                else if (cmd == "scale") {
                    validinput = readvals(s,3,values) ;
                    if (validinput) {
                        // YOUR CODE FOR HW 2 HERE.
                        // Think about how the transformation stack is affected
                        // You might want to use helper functions on top of file.

		        mat4 curScaleMat = glm::transpose(Transform::scale((float) values[0], (float) values[1], (float) values[2]));
                        rightmultiply(curScaleMat, transfstack);
                    }
                }
                else if (cmd == "rotate") {
                    validinput = readvals(s,4,values) ; 
                    if (validinput) {
                        // YOUR CODE FOR HW 2 HERE. 
                        // values[0..2] are the axis, values[3] is the angle.  
                        // You may want to normalize the axis (or in Transform::rotate)
                        // See how the stack is affected, as above.
                        vec3 _axis = glm::vec3(values[0], values[1], values[2]);
                        vec3 axis = glm::normalize(_axis);
                        mat4 curRotationMat = glm::mat4(Transform::rotate((float) values[3], axis));
                        curRotationMat = glm::transpose(curRotationMat);
                        
                        rightmultiply(curRotationMat, transfstack);
                    }
                }

                
                // I include the basic push/pop code for matrix stacks
                else if (cmd == "pushTransform") 
                    transfstack.push(transfstack.top()) ; 
                else if (cmd == "popTransform") {
                    if (transfstack.size() <= 1) 
                        cerr << "Stack has no elements.  Cannot Pop\n" ; 
                    else transfstack.pop() ; 
                }
                
                else {
                    cerr << "Unknown Command: " << cmd << " Skipping \n" ; 
                }
            }
            getline (in, str) ; 
        }
        
        // Set up initial position for eye, up and amount
        // As well as booleans 
        
        eye = eyeinit ; 
        up = upinit ; 
        amount = 5;
        sx = sy = 1.0 ; // scales in x and y 
        tx = ty = 0.0 ; // translation in x and y  
        useGlu = false; // don't use the glu perspective/lookat fns
        
        glEnable(GL_DEPTH_TEST);
        
    }
    else {
        cerr << "Unable to Open Input Data File " << filename << "\n" ; 
        throw 2 ; 
    }
    
}


/* Loads a scene file */
void loadScene(string sceneFile){
    if(loadedSceneIndex == 0){
	   loadedSceneIndex = 1;
    }else{
	   loadedSceneIndex = 0;
    }
    
    char * fileName;
    fileName = new char[sceneFile.size() + 1];
    fileName[sceneFile.size()] = 0;
    memcpy(fileName,sceneFile.c_str(),sceneFile.size());
    
    clearSceneVars();
    
    readfile(fileName);
}

/* Empties scene variables */
void clearSceneVars(){
    numused = 0;
    numobjects = 0;
}

/* Sets all usable textures */
void declareTextures(){
    glGenTextures( 1, &dirt );
}
