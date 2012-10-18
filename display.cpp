/*****************************************************************************/
/* This is the program skeleton for homework 2 in CS 184 by Ravi Ramamoorthi */
/* Extends HW 1 to deal with shading, more transforms and multiple objects   */
/*****************************************************************************/

// This file is display.cpp.  It includes the skeleton for the display routine

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

// New helper transformation function to transform vector by modelview
// May be better done using newer glm functionality.
// Provided for your convenience.  Use is optional.
// Some of you may want to use the more modern routines in readfile.cpp
// that can also be used.

void transformvec (const GLfloat input[4], GLfloat output[4]) {
    GLfloat modelview[16] ; // in column major order
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview) ;
    
    for (int i = 0 ; i < 4 ; i++) {
        output[i] = 0 ;
        for (int j = 0 ; j < 4 ; j++)
            output[i] += modelview[4*j+i] * input[j] ;
    }
}

void display() {
	glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    // I'm including the basic matrix setup for model view to
    // give some sense of how this works.
    
	glMatrixMode(GL_MODELVIEW);
	mat4 mv ;
    
    if (useGlu) mv = glm::lookAt(eye,center,up) ;
	else {
        mv = Transform::lookAt(eye,center,up) ;
        mv = glm::transpose(mv) ; // accounting for row major
    }
    glLoadMatrixf(&mv[0][0]) ;
    
    // Set Light and Material properties for the teapot
    // Lights are transformed by current modelview matrix.
    // The shader can't do this globally.
    // So we need to do so manually.
    if (numused) {
        glUniform1i(enablelighting,true) ;
        glUniform1i(numusedcol, numused);
        
        for(int j = 0; j < numused; j++){
            GLfloat inputPosn[4];
            GLfloat outputPosn[4];
            
            inputPosn[0] = lightposn[4*j + 0];
            inputPosn[1] = lightposn[4*j + 1];
            inputPosn[2] = lightposn[4*j + 2];
            inputPosn[3] = lightposn[4*j + 3];
            
            transformvec(inputPosn, outputPosn);

            
            lightransf[4*j + 0] = outputPosn[0];
            lightransf[4*j + 1] = outputPosn[1];
            lightransf[4*j + 2] = outputPosn[2];
            lightransf[4*j + 3] = outputPosn[3];
        }
        glUniform4fv(lightpos, numLights, lightransf);
        glUniform4fv(lightcol, numLights, lightcolor);
        
    }
    else glUniform1i(enablelighting,false) ;
    
    
    
    
    // Transformations for objects, involving translation and scaling
    mat4 sc(1.0) , tr(1.0), transf(1.0) ;
    sc = Transform::scale(sx,sy,1.0) ;
    tr = Transform::translate(tx,ty,0.0) ;
    transf = transf * mv;
    glLoadMatrixf(&transf[0][0]) ;
        
    for (int i = 0 ; i < numobjects ; i++) {
        object * obj = &(objects[i]) ;
        
        {
            glUniform4fv(ambientcol,1,obj -> ambient);
            glUniform4fv(diffusecol,1,obj -> diffuse);
            glUniform4fv(specularcol,1,obj -> specular);
            glUniform1f(shininesscol, obj -> shininess);
            
            glLoadMatrixf(&(transf * obj -> transform)[0][0]);
            
        }
        
        // Actually draw the object
        // We provide the actual glut drawing functions for you.
        if (obj -> type == cube) {
            glutSolidCube(obj->size) ;
        }
        else if (obj -> type == sphere) {
            const int tessel = 20 ;
            glutSolidSphere(obj->size, tessel, tessel) ;
        }
        else if (obj -> type == teapot) {
            glutSolidTeapot(obj->size) ;
        }
    }

    if (use_char) {
      glUniform4fv(ambientcol, 1, character.ambient);
      glUniform4fv(diffusecol, 1, character.diffuse);
      glUniform4fv(specularcol, 1, character.specular);
      glUniform1f(shininesscol, character.shininess);
      glLoadMatrixf(&(transf * character.transform)[0][0]);
      glutSolidCube(character.size);
    }
    glutSwapBuffers();
}
