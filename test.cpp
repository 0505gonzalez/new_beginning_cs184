#include <iostream>
#include "ModelObj.h"

using namespace std;

int main () {
  ModelObj * object = new ModelObj();
  object->loadObj("test_file.txt", 3);

  cout << "Vertices: " << endl;
  for (int i = 0; i < 9; i++) {
    cout << " " << object->vertices[i]; 
  }
  cout << endl;
  cout << "Textures: " << endl;
  for (int i = 0; i < 4; i++) {
    cout << " " << object->textures[i];
  }
  cout << endl;
  cout << "Normals: " << endl;
  for (int i = 0; i < 9; i++) {
    cout << " " << object->normals[i];
  }
  cout << endl;
  cout << "Vertex Indices: " << endl;
  for (int i = 0; i < 9; i++) {
    cout << " " << object->vertex_indices[i];
  }
  cout << endl;
  cout << "Texture Indices: " << endl;
  for (int i = 0; i < 9; i++) {
    cout << " " << object->texture_indices[i];
  }
  cout << endl;
  cout << "Normal Indices: " << endl;
  for (int i = 0; i < 9; i++) {
    cout << " " << object->normal_indices[i];
  }
  cout << endl;

  return 0;
}
