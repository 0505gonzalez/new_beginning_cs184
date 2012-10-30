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
#include "ModelObj.h"

// New helper transformation function to transform vector by modelview
// May be better done using newer glm functionality.
// Provided for your convenience.  Use is optional.
// Some of you may want to use the more modern routines in readfile.cpp
// that can also be used.

ModelObj list_of_models [max_obj_models]; // List containing all the models.
int size_of_list_models = 0;

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
            
            glLoadMatrixf(&(transf * obj->transform)[0][0]);
            
        }
	
        if(i == 0){
            //glBindTexture( GL_TEXTURE_2D, dirt );
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
        } else if (obj -> type == modelobj) {
            ModelObj * object = new ModelObj();
            bool found_flag = false;
            for (int i = 0; i < size_of_list_models; i++) {
                if (list_of_models[i].name == obj->name) {
                    found_flag = true;
                    *object = list_of_models[i];
                }
            }
            if (!found_flag) {
                if (!object->loadObj(obj->file_path, obj->shape_sides, obj->name)) {
                    exit(1);
                }
                list_of_models[size_of_list_models] = *object;
                size_of_list_models++;
            }

            glEnableClientState(GL_NORMAL_ARRAY);
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_INDEX_ARRAY);
        
            glIndexPointer(GL_UNSIGNED_INT, 0, object->vertex_indices);
            glNormalPointer(GL_FLOAT, 0, object->normals);
            glVertexPointer(3, GL_FLOAT, 0, object->vertices);
            if (object->shape == 4) {
                glDrawElements(GL_QUADS, object->num_of_indices, GL_UNSIGNED_INT, object->vertex_indices);
            } else if (object -> shape == 3) {
                glDrawElements(GL_TRIANGLES, object->num_of_indices, GL_UNSIGNED_INT, object->vertex_indices);
            }
            glDisableClientState(GL_INDEX_ARRAY);
            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_NORMAL_ARRAY);
        }
    }
    /**
    glBindTexture(GL_TEXTURE_2D, texNames[0]);
    glBegin(GL_QUADS);
     glTexCoord2f(0.0, 0.0);
     glVertex3f(0.0, 0.0, 0.0);
     glTexCoord2f(1.0, 0.0);
     glVertex3f(100.0, 0.0, 0.0);
     glTexCoord2f(1.0, 1.0);
     glVertex3f(100.0, 100.0, 0.0);
     glTexCoord2f(0.0, 1.0);
     glVertex3f(0.0, 100.0, 0.0);
    glEnd();
     **/

    if (use_char) {
        glUniform4fv(ambientcol, 1, character.ambient);
        glUniform4fv(diffusecol, 1, character.diffuse);
        glUniform4fv(specularcol, 1, character.specular);
        glUniform1f(shininesscol, character.shininess);
        glLoadMatrixf(&(transf * character.transform * glm::transpose(Transform::translate(0,0,0.4)) )[0][0]);
        glutSolidSphere(0.08, 20, 20);
        glLoadMatrixf(&(transf * character.transform * glm::transpose(Transform::translate(0,0,0.15)) )[0][0]);
        GLUquadricObj* quad_obj = gluNewQuadric();
        gluCylinder(quad_obj, 0.01, 0.01, 0.2, 10, 10);
        
        GLfloat black_ambient[4] = {0.1,0.1,0.1,1};
        GLfloat black_diffuse[4] = {0.1,0.1,0.1,1};
        GLfloat black_specular[4] = {0.1,0.1,0.1,1};
        GLfloat black_shininess = 2;
        
        glUniform4fv(ambientcol, 1, black_ambient);
        glUniform4fv(diffusecol, 1, black_diffuse);
        glUniform4fv(specularcol, 1, black_specular);
        glUniform1f(shininesscol, black_shininess);
        
        glUniform4fv(specularcol, 1, character.specular);
        glUniform1f(shininesscol, character.shininess);
        glLoadMatrixf(&(transf *character.transform * glm::transpose(Transform::translate(0,0,0.28)))[0][0]);
        quad_obj = gluNewQuadric();
        gluCylinder(quad_obj, 0.06, 0.01, 0.02, 10, 10);
        glLoadMatrixf(&(transf * character.transform * glm::transpose(Transform::translate(0,0,0.11)))[0][0]);
        quad_obj = gluNewQuadric();
        gluCylinder(quad_obj, 0.07, 0.07, 0.18, 10, 10);
        glLoadMatrixf(&(transf * character.transform * glm::transpose(Transform::translate(-0.04,0,0.03)))[0][0]);
        quad_obj = gluNewQuadric();
        gluCylinder(quad_obj, 0.02, 0.02, 0.08, 10, 10);
        
    }

	
    glutSwapBuffers();
}
