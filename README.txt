Computer Science 184 - Assignment 4

By: Juan Gonzalez (cs184-am) and Alvin Wong (cs184-ax)

A New Beginning

How to run the program: 
Using the Makefile, type

make

This will create an executable program called new_beginning. To run this, type

./new_beginning

The scene starts in the dark scene, next to the gravestones. To go to the nature scene, locate the bridge that extends to a portal to another dimension. In the nature scene, to go back to the dark scene, locate the black tombstone of death.

______________________________________________________________________________

Requirements:

1) A complete scene. 
   - The dark scene has a small house, and inside the house is a fireplace, a bed, a pillow, a picture, a computer, etc. Outside the house are wilted, dark trees and a skeleton (for added spookiness). There is a path from the house to a certain destination.
   - The nature scene has shiny trees, some animals, including cows, a horse, sheep, and fish. It has a river that can be crossed over by a bridge. A fence surrounds the whole scene, and the sun shines overhead.
   
2) One object created by hand.
   - Using old-style OpenGL, the portrait of George Washington, the fireplace, and the portal leading to the nature scene were all constructed as rectangles.
   - Using the OBJ parser, we also manually constructed text files that follow the obj format. From this, we created polygons based on our own specified coordinates and our own calculated normals, and these include the fence and the bridge and the river base.

3) One object loaded from a .obj file.
   - We have plenty - trees, all the animals, and the skeleton, for example.
   
4) All placement done by hand.
   - We didn't even use a modeling program to help load objects into the scene. We stuck with the framework of HW2 by using a readfile to parse different commands from a scene file.

5) Two objects textured with an image.
   - This can be seen in the dark scene, where we have a portrait of George Washington, a texture to support the fireplace, and a nebula texture to symbolize a portal to another dimension.
   
6) Shiny objects, dull objects.
   - A good example is in the nature scene. The bridge over the river is very shiny (high specular term), whereas the fish in the river are very dull.
   
7) At least one directional light.
   - The sun in the nature scene.

8) At least one point light source.
   - The fireplace in the dark scene.

9) At least one object instantiated more than once.
   - We implemented this by only loading each .obj file's vertices, texture coordinates, and normals ONE time. We stored this data in an array, so that whenever we make a call to create a new object, the "new" object uses the preexisting generated coordinates, and is transformed to another place in the world via different Model View Matrix.

10) At least half of the objects in the scene must have correct normals to interact with the lights correctly.
   - All the OBJ files we pulled in had normals specified, so we used those. For the objects we created, most of them are specified with correct normals. Take the nature scene, for example. Everything except the fish there has correct normals. This definitely satisfies > 1/2 of all objects.
   
11) Use double buffering, hidden surface elimination, and a perspective projection.

    - We took the framework from HW2, which includes double buffering, depth testing (Z Buffer), and our own perspective projection matrix defined in Transform.cpp.

12) Mechanism to turn on/off texture
    - Press 't'.
    
13) Programmable fragment shaders + 1 add-on.
    - We took the shader code from HW2. We also implemented bump mapping, which is generating bumps on objects via a normal map. This can be seen in the road leaving the house in the dark scene.
    
14) Both the mouse and keyboard.
    - Mouse rotates view. Keyboard translate character according to the WASD keys. The p,t,i,o,h keys are defined. p = pause/start animation. t = turn on/off textures, i = zoom in, o = zoom out, h = print help

15) User must be able to look around their current position and move forward and backwards.
    - Again, mouse movement and WASD keys.
    - 'i' to zoom in, 'o' to zoom out.
    
16) Animation
    - The character is animated when he moves. The fish in the river and the skeleton move. Press p to toggle on/off animation.
    

Extra Credit
1) Parametric or curved Surfaces - the river was modeled after a cosine curve, sampled at 20 points.
2) Animated Textures - the computer in the deathbed scene shows this.
3) Anything Else / Collision Detection (Very simple) - The nature scene has a minimal function collision detection scheme so that the character does not go off the plane or into the river. And yes, the character does have eyes.