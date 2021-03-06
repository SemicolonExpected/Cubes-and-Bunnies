// This example is heavily based on the tutorial at https://open.gl

// OpenGL Helpers to reduce the clutter
#include "Helpers.h"
#include "functions.h"

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>
#else
// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>
#endif


// Linear Algebra Library
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Geometry>

// Timer
#include <chrono>

//Strings for Modes
#include <string>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <cstdlib>
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>


using namespace std;
using namespace Eigen;

// VertexBufferObject wrapper
VertexBufferObject VBO; //vertices
VertexBufferObject VBO_C; //colors
VertexBufferObject NormalBuffer; //normals of vertices
IndexBufferObject IBO;

//Element Buffer
MatrixXi E(1,3);
MatrixXi Objs = Eigen::MatrixXi::Zero(4,1); //objs stores start and stop index of each object in the E matrix and the item type and what kind of shading
// Contains the vertex positions
Eigen::MatrixXf V = Eigen::MatrixXf::Zero(3,3);
// Contains the per-vertex color
Eigen::MatrixXf C(3,3);
Eigen::MatrixXf C_Black = Eigen::MatrixXf::Zero(3,3);
// Contains the view transformation
Eigen::Matrix4f view(4,4);
Eigen::MatrixXf N = Eigen::MatrixXf::Zero(3,3);

Eigen::MatrixXi bumpyEl = Eigen::MatrixXi::Zero(1,1);
Eigen::MatrixXi bunnyEl = Eigen::MatrixXi::Zero(1,1);
Eigen::MatrixXi cubeEl = Eigen::MatrixXi::Zero(1,36);

GLint viewX = 0, viewY = 0;
double xOrigin, yOrigin;

//int rotation = 0; //can be 0, -10, or 10
string MODE;

//transformation values
MatrixXf centers = Eigen::MatrixXf::Zero(3,1);
MatrixXf rotations = Eigen::MatrixXf::Zero(3,1); //angle for each axis
MatrixXf scale = Eigen::MatrixXf::Zero(1,1);
MatrixXf translations = Eigen::MatrixXf::Zero(3,1);

Vector3f worldRotation(0,0,0); float worldScale = 1;

int selectedObj = -1;

int ifPerspective = 0;

int cubeOffset = 0, bumpyOffset = 0, bunnyOffset = 0;
MatrixXf bunnyFaceNormals, bumpyFaceNormals, cubeFaceNormals;

Vector3f lightPos(0,5,0);

//imports vertices to VBO and gets the indexes of the vertexes for each face for both meshes
//when we import we should center the vertexes already on origin and scaled to make things easier. 
/*int importBumpyCube(){
  std::ifstream bumpyCube ("../data/bumpy_cube.off");
  string line;
  bumpyCube >> line;
  int numCubeVertices;
  int numCubeFaces;

  bumpyCube >> line; 
  numCubeVertices = stoi(line); 
  Vector3f cubeVertices[numCubeVertices];
  MatrixXf cubeVerts(3,numCubeVertices);
  bumpyCube >> line; 
  numCubeFaces = stoi(line);
  bumpyCube >> line; //get rid of the edges

  int offset = V.cols();
  V.conservativeResize(3, V.cols()+numCubeVertices);
  N.conservativeResize(3, V.cols());
  C.conservativeResize(3, V.cols());
  
  bumpyEl.resize(1,numCubeFaces*3);
  //get vertices
  for(int i = 0; i < numCubeVertices; i++){
    for(int k = 0; k < 3; k++){
      bumpyCube >> line;
      cubeVertices[i](k) = stof(line);
      cubeVerts(k,i) = cubeVertices[i](k);
    }
  }
  
  //get faces
  MatrixXf faceNormals(3,bumpyEl.cols()/3);
  
  for(int i = 0; i < bumpyEl.cols(); i+=3){  
    bumpyCube >> line; //get the first col
    for(int k = 0; k < 3; k++){
      bumpyCube >> line;
      bumpyEl(0,i+k) = stof(line)+offset;
    }
    Vector3f normal = getTriangleNormal(cubeVertices[bumpyEl(0,i)-offset], cubeVertices[bumpyEl(0,i+1)-offset], cubeVertices[bumpyEl(0,i+2)-offset]);
    faceNormals.col(i/3) << normal(0), normal(1), normal(2);
  }
  //get centroid
  Vector3d center = getMeshBarycenter(cubeVertices, numCubeVertices);
  //translate vertices so that bary is at origin. ie subtract center with every vertex;
  centerOnOrigin(cubeVertices, numCubeVertices, center);
  //scale so that max is 0.5 and -0.5 and at least 1 vertex is at 0.5 or -0.5
  scaleToUnit(cubeVertices, numCubeVertices, center);

  //get normals
  MatrixXf normals = getVertexNormals(cubeVerts, bumpyEl, faceNormals, offset);
  //upload into N then update NormalBuffer
  
  //upload vertices and normals to V
  for(int i = 0; i < numCubeVertices; i++){
    V(0,offset+i) = cubeVertices[i](0);  //x
    V(1,offset+i) = cubeVertices[i](1);  //y
    V(2,offset+i) = cubeVertices[i](2);  //z
    N.col(i+offset) = normals.col(i);
    //if(i%3 == 0){C.col(offset+i) << 0.53,0.38,0.79;}
    //if(i%3 == 1){C.col(offset+i) << 0,1,1;}
    //if(i%3 == 2){C.col(offset+i) << 0.55,0.47,0.85;}
    C.col(offset+i) << 0.53,0.38,0.79;
  }

  return offset;
}*/
int importBumpyCubeVertex(){
  std::ifstream bumpyCube ("../data/bumpy_cube.off");
  string line;
  bumpyCube >> line;
  int numCubeVertices;
  int numCubeFaces;

  bumpyCube >> line; 
  numCubeVertices = stoi(line); 
  Vector3f cubeVertices[numCubeVertices];
  MatrixXf cubeVerts(3,numCubeVertices);
  bumpyCube >> line; 
  numCubeFaces = stoi(line);
  bumpyCube >> line; //get rid of the edges
  
  int offset = V.cols();
  V.conservativeResize(3, V.cols()+numCubeFaces*3);
  N.conservativeResize(3, V.cols());
  C.conservativeResize(3, V.cols());

  //get vertices
  for(int i = 0; i < numCubeVertices; i++){
    for(int k = 0; k < 3; k++){
      bumpyCube >> line;
      cubeVertices[i](k) = stof(line);
      cubeVerts(k,i) = cubeVertices[i](k);
    }
  }

  bumpyEl.resize(1,numCubeFaces*3);
  MatrixXf faceNormals(3,numCubeFaces); 
  for(int i = 0; i < bumpyEl.cols(); i+=3){  
    bumpyCube >> line; //get the first col
    for(int k = 0; k < 3; k++){
      bumpyCube >> line;
      bumpyEl(0,i+k) = stof(line);
    }
    //999
    Vector3f normal = getTriangleNormal(cubeVertices[bumpyEl(0,i)],
					cubeVertices[bumpyEl(0,i+1)], cubeVertices[bumpyEl(0,i+2)]);
    faceNormals.col(i/3) << normal(0), normal(1), normal(2);  
  }
  bumpyFaceNormals = faceNormals;
  //get centroid
  Vector3d center = getMeshBarycenter(cubeVertices, numCubeVertices);
  //translate vertices so that bary is at origin. ie subtract center with every vertex;
  cubeVerts = centerOnOrigin(cubeVerts, numCubeVertices, center);
  //scale so that max is 0.5 and -0.5 and at least 1 vertex is at 0.5 or -0.5
  cubeVerts = scaleToUnit(cubeVerts, numCubeVertices, center);

  MatrixXf normals = getVertexNormals(cubeVerts, bumpyEl, faceNormals, 0);

  for(int i = 0; i < numCubeFaces*3; i++){
    V(0, offset + i) = cubeVerts(0, bumpyEl(0,i)); 
    V(1, offset + i) = cubeVerts(1, bumpyEl(0,i));
    V(2, offset + i) = cubeVerts(2, bumpyEl(0,i));
    N.col(i+offset) = normals.col(bumpyEl(0,i));
    C.col(offset+i) << 0.53,0.38,0.79;
  }
  return offset;
}
int importBunnyVertex(){
  std::ifstream bunny ("../data/bunny.off");
  string line;
  //gets rid of first line
  bunny >> line;

  int numBunnyVertices;
  int numBunnyFaces;
  bunny >> line;
  numBunnyVertices = stoi(line);
  Vector3f bunnyVertices[numBunnyVertices];
  MatrixXf bunnyVerts(3,numBunnyVertices); //this is better but I realize it too late so we have this matrix and the array
  bunny >> line;
  numBunnyFaces = stoi(line);
  bunny >> line; //get rid of the edges

  int offset = V.cols();
  
  V.conservativeResize(3, V.cols()+numBunnyFaces*3);
  N.conservativeResize(3, V.cols());
  C.conservativeResize(3, V.cols());
  bunnyEl.resize(1, numBunnyFaces*3);
  
  //get vertices
  for(int i = 0; i < numBunnyVertices; i++){
    for(int k = 0; k < 3; k++){
      bunny >> line;
      bunnyVertices[i](k) = stof(line);
      bunnyVerts(k,i) = bunnyVertices[i](k);
    }
  }
  
  //get faces
  MatrixXf faceNormals(3,bunnyEl.cols()/3);
  for(int i = 0; i < bunnyEl.cols(); i+=3){  
    bunny >> line; //get the first col
    for(int k = 0; k < 3; k++){
      bunny >> line;
      bunnyEl(0,i+k) = stof(line);
    }
    Vector3f normal = getTriangleNormal(bunnyVertices[bunnyEl(0,i)], bunnyVertices[bunnyEl(0,i+1)], bunnyVertices[bunnyEl(0,i+2)]);
    faceNormals.col(i/3) << normal(0), normal(1), normal(2);
  }
  bunnyFaceNormals = faceNormals;
  //get centroid
  Vector3d center = getMeshBarycenter(bunnyVertices, numBunnyVertices);
  //translate bary to origin
  bunnyVerts = centerOnOrigin(bunnyVerts, numBunnyVertices, center);
  //scale so that max is 0.5 and -0.5 and at least 1 vertex is at 0.5 or -0.5
  bunnyVerts = scaleToUnit(bunnyVerts, numBunnyVertices, center);

  //get normals
  MatrixXf normals = getVertexNormals(bunnyVerts, bunnyEl, faceNormals, 0);
  //upload into N
  //upload vertices to V
  
  for(int i = 0; i < numBunnyFaces*3; i++){ 
    V(0,offset+i) = bunnyVerts(0, bunnyEl(0,i));  //x
    V(1,offset+i) = bunnyVerts(1, bunnyEl(0,i));  //y
    V(2,offset+i) = bunnyVerts(2, bunnyEl(0,i));  //z
    N.col(i+offset) = normals.col(bunnyEl(0,i)); 
    C.col(offset+i) << 0.9686,0.7916,0.7882;
  }
  
  return offset;
}
/*int importBunny(){
  std::ifstream bunny ("../data/bunny.off");
  string line;
  //gets rid of first line
  bunny >> line;

  int numBunnyVertices;
  int numBunnyFaces;
  bunny >> line;
  numBunnyVertices = stoi(line);
  Vector3f bunnyVertices[numBunnyVertices];
  MatrixXf bunnyVerts(3,numBunnyVertices); //this is better but I realize it too late so we have this matrix and the array
  bunny >> line;
  numBunnyFaces = stoi(line);
  bunny >> line; //get rid of the edges

  int offset = V.cols();
  
  V.conservativeResize(3, V.cols()+numBunnyVertices);
  N.conservativeResize(3, V.cols());
  C.conservativeResize(3, V.cols());
  bunnyEl.resize(1, numBunnyFaces*3);
  
  //get vertices
  for(int i = 0; i < numBunnyVertices; i++){
    for(int k = 0; k < 3; k++){
      bunny >> line;
      bunnyVertices[i](k) = stof(line);
      bunnyVerts(k,i) = bunnyVertices[i](k);
    }
  }
  
  //get faces
  MatrixXf faceNormals(3,bunnyEl.cols()/3);
  for(int i = 0; i < bunnyEl.cols(); i+=3){  
    bunny >> line; //get the first col
    for(int k = 0; k < 3; k++){
      bunny >> line;
      bunnyEl(0,i+k) = stof(line)+offset;
    }
    Vector3f normal = getTriangleNormal(bunnyVertices[bunnyEl(0,i)-offset], bunnyVertices[bunnyEl(0,i+1)-offset], bunnyVertices[bunnyEl(0,i+2)-offset]);
    faceNormals.col(i/3) << normal(0), normal(1), normal(2);
  }
  //get centroid
  Vector3d center = getMeshBarycenter(bunnyVertices, numBunnyVertices);
  //translate bary to origin
  centerOnOrigin(bunnyVertices, numBunnyVertices, center);
  //scale so that max is 0.5 and -0.5 and at least 1 vertex is at 0.5 or -0.5
  scaleToUnit(bunnyVertices, numBunnyVertices, center);

  //get normals
  MatrixXf normals = getVertexNormals(bunnyVerts, bunnyEl, faceNormals, offset);
  //upload into N
  //upload vertices to V
  for(int i = 0; i < numBunnyVertices; i++){
    V(0,offset+i) = bunnyVertices[i](0);  //x
    V(1,offset+i) = bunnyVertices[i](1);  //y
    V(2,offset+i) = bunnyVertices[i](2);  //z
    N.col(i+offset) = normals.col(i);
    //if(i%3 == 0){C.col(offset+i) << 0.53,0.38,0.79;}
    //if(i%3 == 1){C.col(offset+i) << 1,0,1;}
    //if(i%3 == 2){C.col(offset+i) << 0.55,0.47,0.85;}
    C.col(offset+i) << 0.9686,0.7916,0.7882;
  }
  
  return offset;
}*/
/*int importCube(){
  int offset = V.cols();

  V.conservativeResize(3, V.cols()+8);
  N.conservativeResize(3, V.cols());
  C.conservativeResize(3, V.cols());

  MatrixXf cubeV(3,8);
  
  //-0.5,-0.5,-0.5//-0.5,-0.5,0.5//-0.5,-0.5,-0.5//-0.5,0.5,0.5//
  //012, 023, 014, 045, 035, 357, 124, 246, 645, 567, 367, 326
  V.col(offset)   << -0.5,-0.5,-0.5; cubeV.col(0) << -0.5,-0.5,-0.5; C.col(offset) << 0,1,1;
  V.col(offset+1) << -0.5,-0.5,0.5;  cubeV.col(1) << -0.5,-0.5,0.5;  C.col(offset+1) << 1,0,1;
  V.col(offset+2) << -0.5,0.5,-0.5;  cubeV.col(2) << -0.5,0.5,-0.5;  C.col(offset+2) << 1,1,0;
  V.col(offset+3) << -0.5,0.5,0.5;   cubeV.col(3) << -0.5,0.5,0.5;   C.col(offset+3) << 1,0.6,1;
  V.col(offset+4) << 0.5,-0.5,-0.5;  cubeV.col(4) << 0.5,-0.5,-0.5;  C.col(offset+4) << 0.9686,0.7916,0.7882; 
  V.col(offset+5) << 0.5,-0.5,0.5;   cubeV.col(5) << 0.5,-0.5,0.5;   C.col(offset+5) << 0.568627,0.6588,0.8196;
  V.col(offset+6) << 0.5,0.5, -0.5;  cubeV.col(6) << 0.5,0.5, -0.5;  C.col(offset+6) << 0,0,0;
  V.col(offset+7) << 0.5, 0.5, 0.5;  cubeV.col(7) << 0.5, 0.5, 0.5;  C.col(offset+7) << 1,1,1;
  //315 not 305 instead of 045, 375 and 315 instead of 302 371 and 517  -++ 3, +-- 4 415
  cubeEl << //-264 024,  014 (145), 012 123, 135 357, 236 367, 456 567
    0,1,2, 0,1,4, 0,2,4, 1,2,3, 1,5,4, 1,3,5,
    2,3,6, 2,4,6, 3,7,5, 4,5,6, 5,6,7, 3,6,7;

  MatrixXf faceNormals(3,12);
  for(int i = 0; i<cubeEl.cols(); i++){cubeEl(0,i) += offset;}
  for(int i = 0; i<cubeEl.cols(); i+=3){
    Vector3f normal = getTriangleNormal(V.col(cubeEl(0,i*3)), V.col(cubeEl(0,i*3+1)), V.col(cubeEl(0,i*3+2)));
    faceNormals.col(i/3) << normal(0), normal(1), normal(2);
  }

  //get normals
  MatrixXf normals = getVertexNormals(cubeV, cubeEl, faceNormals, offset);
  //upload into N

  for(int i = 0; i < 8; i++){
    N.col(i+offset) = normals.col(i);
  }
  
  return offset;
}*///already scaled and centered at origin
int importCubeVertex(){
  int offset = V.cols();

  V.conservativeResize(3, V.cols()+36);
  N.conservativeResize(3, V.cols());
  C.conservativeResize(3, V.cols());

  MatrixXf colors(3,8);
  MatrixXf cubeV(3,8); 
  
  cubeV.col(0) << -0.5,-0.5,-0.5; colors.col(0) << 0,1,1;
  cubeV.col(1) << -0.5,-0.5,0.5;  colors.col(1) << 1,0,1;
  cubeV.col(2) << -0.5,0.5,-0.5;  colors.col(2) << 1,1,0;
  cubeV.col(3) << -0.5,0.5,0.5;   colors.col(3) << 1,0.6,1;
  cubeV.col(4) << 0.5,-0.5,-0.5;  colors.col(4) << 0.9686,0.7916,0.7882; 
  cubeV.col(5) << 0.5,-0.5,0.5;   colors.col(5) << 0.568627,0.6588,0.8196;
  cubeV.col(6) << 0.5,0.5, -0.5;  colors.col(6) << 0,0,0;
  cubeV.col(7) << 0.5, 0.5, 0.5;  colors.col(7) << 1,1,1;

  cubeEl << 
    0,1,2, 0,1,4, 0,2,4, 1,2,3, 1,5,4, 1,3,5,
    2,3,6, 2,4,6, 3,7,5, 4,5,6, 5,6,7, 3,6,7;

  MatrixXf faceNormals(3,12);
  for(int i = 0; i < cubeEl.cols(); i+=3){
    Vector3f normal = getTriangleNormal(cubeV.col(cubeEl(0,i)), cubeV.col(cubeEl(0,i+1)), cubeV.col(cubeEl(0,i+2)));   
    faceNormals.col(i/3) << normal(0), normal(1), normal(2); 
  }
  cubeFaceNormals = faceNormals;

  MatrixXf normals = getVertexNormals(cubeV, cubeEl, faceNormals, 0);
  cout << normals <<endl;

  Vector3f magenta(1, 0/*float(106)/255*/, float(213)/255);
  for(int i = 0; i < cubeEl.cols(); i++){ 
    V.col(offset + i) = cubeV.col(cubeEl(0,i));  
    N.col(offset + i) = -normals.col(cubeEl(0,i));
    //C.col(offset + i) = colors.col(cubeEl(0,i));
    C.col(offset + i) = magenta.transpose();
  }

  return offset;
}
//the add functions just add another of that object into the element buffer
void addBumpyCube(){
  float temp = E.cols(); 
  Objs.conservativeResize(4, Objs.cols()+1); //cout << bumpyOffset + bumpyEl.cols()<<endl;
  //Objs(0, Objs.cols()-1) = temp; //start index
  Objs(0, Objs.cols()-1) = bumpyOffset; 
  //Objs(1, Objs.cols()-1) = temp + bumpyEl.cols();
  Objs(1, Objs.cols()-1) = bumpyOffset + bumpyEl.cols();//end index
  Objs(2, Objs.cols()-1) = bumpyEl.cols(); //# of indices
  Objs(3, Objs.cols()-1) = 3; //type of shading

  scale.conservativeResize(1, Objs.cols());        scale.col(Objs.cols()-1) << 1;
  rotations.conservativeResize(3, Objs.cols());    rotations.col(Objs.cols()-1) << 0,0,0;
  translations.conservativeResize(3, Objs.cols());  translations.col(Objs.cols()-1) << 0,0,0;

  E.conservativeResize(1, E.cols() + bumpyEl.cols());
		       
  for(int i = 0; i < bumpyEl.cols(); i++){
    E.col(temp + i) = bumpyEl.col(i);
  }
  //cout << "Number of Indices:" << E.cols() << " number of bumpy faces: "<<bumpyEl.cols()<<endl;
}
void addBunny(){
  float temp = E.cols();
  Objs.conservativeResize(4, Objs.cols()+1); //begin, end
  //Objs(0, Objs.cols()-1) = temp;
  //Objs(1, Objs.cols()-1) = temp + bunnyEl.cols();
  Objs(0, Objs.cols()-1) = bunnyOffset;
  Objs(1, Objs.cols()-1) = bunnyOffset + bunnyEl.cols();
  Objs(2, Objs.cols()-1) = bunnyEl.cols();
  Objs(3, Objs.cols()-1) = 3;

  centers.conservativeResize(3, Objs.cols());      centers.col(Objs.cols()-1) << 0,0,0;
  scale.conservativeResize(1, Objs.cols());        scale.col(Objs.cols()-1) << 1;
  rotations.conservativeResize(3, Objs.cols());    rotations.col(Objs.cols()-1) << 0,0,0;
  translations.conservativeResize(3, Objs.cols());  translations.col(Objs.cols()-1) << 0,0,0;

  E.conservativeResize(1, E.cols()+bunnyEl.cols());

  for(int i = 0; i < bunnyEl.cols(); i++){
    E.col(temp + i) = bunnyEl.col(i);
  }
  //cout << "Number of Indices:" << E.cols() << " number of bunny faces: "<<bunnyEl.cols()<<endl;
}
void addCube(){
  
  int temp = E.cols();
  Objs.conservativeResize(4, Objs.cols()+1);
  //Objs(0, Objs.cols()-1) = temp; Objs(1, Objs.cols()-1) = temp + 36;
  Objs(0, Objs.cols()-1) = cubeOffset; Objs(1, Objs.cols()-1) = cubeOffset + 36;
  Objs(2, Objs.cols()-1) = 36; Objs(3, Objs.cols()-1) = 1;
  centers.conservativeResize(3, Objs.cols());      centers.col(Objs.cols()-1) << 0,0,0;
  scale.conservativeResize(1, Objs.cols());        scale.col(Objs.cols()-1) << 1;
  rotations.conservativeResize(3, Objs.cols());    rotations.col(Objs.cols()-1) << 0,0,0;
  translations.conservativeResize(3, Objs.cols());  translations.col(Objs.cols()-1) << 0,0,0;

  E.conservativeResize(1, E.cols()+36);
  for(int i = 0; i < 36; i++){
    E.col(temp + i) = cubeEl.col(i);
    int index = E(0,(temp+i));
    //cout << "Index:" << E.col(temp+i) << " Vertex 1: "<< V.col(index) <<endl;
  }
  //cout << endl;
  //cout << "Number of Indices:" << E.cols() <<endl;
  //cout << "This is after I add the cube" << E <<endl;
}

void deleteObj(int objectNum){
  //delete from E, and all the transformation matrices columns starting from O
  
  for(int i = Objs(0,objectNum); i+Objs(2,objectNum) < E.cols(); i++){
    //E, rotations, scale, translations
    E.col(i) = E.col(i+Objs(2,+objectNum));
  }
  E.conservativeResize(1,E.cols()-Objs(2,objectNum));

  int numObjects = Objs.cols();
  for(int i = objectNum; i < numObjects-1; i++){
    Objs.col(i) = Objs.col(i+1);
    rotations.col(i) = rotations.col(i+1);
    scale.col(i) = scale.col(i+1);
    translations.col(i) = translations.col(i+1);
  }
  Objs.conservativeResize(4, numObjects-1);
  rotations.conservativeResize(3, numObjects-1);
  scale.conservativeResize(1, numObjects-1);
  translations.conservativeResize(3, numObjects-1);
  //delete from Objs matrix.
}

void sortFacesColors(MatrixXf &faces, MatrixXf &centers, MatrixXf &colors, int start, int end){
  //quicksort
  if(start < end){
    
    float pivot = centers(2,end); //last element so can just swap backward
    
    int lowInd = start;
    for(int i = start; i < end+1; i++){
      
      if(centers(2,i) > pivot){
	//swap arr[low] and arr[i] for all arr
	//increment low
	if(i!= lowInd){
	  
	  int tempColors = colors(0,lowInd);
	  colors(0,lowInd) = colors(0,i);
	  colors(0,i) = tempColors;
	
	  Vector3f tempCenters(centers(0,lowInd),centers(1,lowInd),centers(2,lowInd));
	  centers.col(lowInd) = centers.col(i);
	  centers.col(i) = tempCenters;

	  VectorXf tempFace(9); tempFace << faces.col(lowInd);
	  faces.col(lowInd) = faces.col(i);
	  faces.col(i) = tempFace;
	}
	lowInd++;
      }
    }
    int tempColors = colors(0,lowInd);
    colors(0,lowInd) = colors(0,end);
    colors(0,end) = tempColors;
    Vector3f tempCenters(centers(0,lowInd),centers(1,lowInd),centers(2,lowInd));
    centers.col(lowInd) = centers.col(end);
    centers.col(end) = tempCenters;
    VectorXf tempFace(9); tempFace << faces.col(lowInd);
    faces.col(lowInd) = faces.col(end);
    faces.col(end) = tempFace;
    
    sortFacesColors(faces, centers, colors, start, lowInd-1);
    sortFacesColors(faces, centers, colors, lowInd+1, end); 
  }
}

void exportToSVG(std::string filename, int w_width, int w_height){
  //for every object get the translated vertex with a corresponding index number
  //for every 3 indices sort by barry and render from back to front
  ofstream svgfile(filename);
  if(svgfile.is_open()){cout <<"is open"<<endl;
    //begin the svg file
    svgfile << "<svg height=\"" << w_height <<"\" width=\"" << w_width << "\">" << endl;
    //MatrixXi ifRendered = Eigen::MatrixXi::Zero(Objs.cols(), V.cols()/3); //matrix to make sure face hasnt already been rendered

    //for every triangles vertices check if its the closest by seeing ifInsideTriangle2 returns the same triangle
    //if so draw that triangle, if not skip
    MatrixXf rot = rotations;
    //rot(0) -= toRadian(5); rot(1) -= toRadian(5);  rot(1) -= toRadian(5);
    rot *= -1;
    Vector3f w_rot = worldRotation;
    w_rot *= -1;
    MatrixXf w_rotX(3,3), w_rotY(3,3), w_rotZ(3,3);
    w_rotX << 
      1,0,           0,
      0,cos(w_rot(0)),-sin(w_rot(0)),
      0,sin(w_rot(0)), cos(w_rot(0));
    w_rotY <<
      cos(w_rot(1)),0, sin(w_rot(1)),
      0,1,0,
      -sin(w_rot(1)),0,cos(w_rot(0)) ;
    w_rotZ <<
      cos(w_rot(2)), -sin(w_rot(2)), 0,
      sin(w_rot(2)), cos(w_rot(2)), 0,
      0, 0, 1;

    MatrixXf w_rotM = w_rotX * w_rotY * w_rotZ;
    MatrixXf V_1(9,1); MatrixXf centers(3,1); MatrixXf ind(1,1);
    V_1(0,0) = -100;
    
    for(int i = 0; i < Objs.cols(); i++){
      MatrixXf rotX(3,3), rotY(3,3), rotZ(3,3);
      rotX << 
	1, 0,             0,
	0, cos(rot(0,i)),-sin(rot(0,i)),
	0, sin(rot(0,i)), cos(rot(0,i));
      rotY <<
	cos(rot(1,i)), 0, sin(rot(1,i)),
	0,             1, 0,
	-sin(rot(1,i)), 0, cos(rot(1,i));
      rotZ <<
	cos(rot(2,i)),  -sin(rot(2,i)),0,
	sin(rot(2,i)),   cos(rot(2,i)),0,
	0,               0            ,1;
      MatrixXf rotM = rotX*rotY*rotZ;
      for(int j = Objs(0,i); j < Objs(1,i); j+=3){cout << i <<","<<j<<" : "<<Objs(1,i)<<endl;
	//if(ifRendered(i,j/3) == 0){
	Vector3f v1 = V.col(j);
	Vector3f v2 = V.col(j+1);
	Vector3f v3 = V.col(j+2);
	  
	Vector3f translation = translations.col(i);
	v1 = (v1.transpose() * rotM * scale(0,i) + translation.transpose()) * w_rotM * worldScale;
	v2 = (v2.transpose() * rotM * scale(0,i) + translation.transpose()) * w_rotM * worldScale;
	v3 = (v3.transpose() * rotM * scale(0,i) + translation.transpose()) * w_rotM * worldScale;
	if(ifPerspective == 1){ //more complicated
	  v1 = v1*(v1(2) / 0.5 + 1.0);
	  v2 = v2*(v2(2) / 0.5 + 1.0);
	  v3 = v3*(v3(2) / 0.5 + 1.0);
	}
	  
	Vector3f normal = getTriangleNormal(v1,v2,v3).normalized(); //normal vector
	Vector3f center((v1(0)+v2(0)+v3(0))/3,
			(v1(1)+v2(1)+v3(1))/3,
			(v1(2)+v2(2)+v3(2))/3);
	  
	float normalAngle = normal.dot(v1);
	//cout << normalAngle <<endl;
	if(normalAngle >= 0){

	  if(V_1(0,0) == -100){
	    V_1.col(0) << v1(0),v1(1),v1(2), v2(0),v2(1),v2(2), v3(0),v3(1),v3(2);
	    centers.col(0) << center(0), center(1), center(2);
	    ind(0,0) = j;
	  }
	  else{
	    V_1.conservativeResize(9, V_1.cols()+1);
	    V_1.col(V_1.cols()-1) << v1(0),v1(1),v1(2), v2(0),v2(1),v2(2), v3(0),v3(1),v3(2);
	    centers.conservativeResize(3,V_1.cols());
	    centers.col(V_1.cols()-1) << center(0), center(1), center(2);
	    ind.conservativeResize(1, V_1.cols());
	    ind(0, V_1.cols()-1) = j;
	    
	  }
	}
      }
    }
    
    sortFacesColors(V_1, centers, ind, 0, V_1.cols()-1);
    
	    //before doing this step maybe put them in a list of faces so a matrix of size (9, num faces)
	    //and a list of bary centers, then sort all the faces by z value and render them back to front
    // sort V_1, centers, and ind by z value from back to front
    for(int i = 0; i < V_1.cols(); i++){
      Vector4f v1_1(V_1(0,i),V_1(1,i),V_1(2,i),1); v1_1 = v1_1.transpose()*view;
      Vector4f v2_1(V_1(3,i),V_1(4,i),V_1(5,i),1); v2_1 = v2_1.transpose()*view;
      Vector4f v3_1(V_1(6,i),V_1(7,i),V_1(8,i),1); v3_1 = v3_1.transpose()*view;
      Vector3f v1(v1_1(0),v1_1(1),v1_1(2)); Vector3f v2(v2_1(0),v2_1(1),v2_1(2)); Vector3f v3(v3_1(0),v3_1(1),v3_1(2));
      Vector3f normal = getTriangleNormal(v1,v2,v3).normalized(); //normal vector
      Vector3f center((v1(0)+v2(0)+v3(0))/3,
		      (v1(1)+v2(1)+v3(1))/3,
			(v1(2)+v2(2)+v3(2))/3);

      Vector2f point1(v1_1(0),v1_1(1)); Vector2f point2(v2_1(0),v2_1(1)); Vector2f point3(v3_1(0),v3_1(1));
      Vector2f center1(center(0),center(1));

      Vector3f lightP = lightPos.transpose() * w_rotM * worldScale;
	    
      Vector3f eye = (-center).normalized();
      float dcolor = ((lightP - center).normalized().transpose()*normal);
      if(dcolor < 0){dcolor = 0;}
      Vector3f color = C.col(ind(0,i))*(1+dcolor)*255;
      if(color(0) > 255){color(0) = 255;}if(color(1) > 255){color(1) = 255;}if(color(2) > 255){color(2) = 255;}
      //do the aspect ratio thing before drawing by multiplying by view
      point1(0) = (point1(0)+1)*(w_width/2); point3(0) = (point3(0)+1)*(w_width/2); point2(0) = (point2(0)+1)*(w_width/2);
      point1(1) = (point1(1)*-1+1)*(w_height/2); point3(1) = (point3(1)*-1+1)*(w_height/2); point2(1) = (point2(1)*-1+1)*(w_height/2);
      svgfile << "<polygon points=\""<<point1(0)<<","<<point1(1)<<" "<<point2(0)<<","<<point2(1)<<" "
	      <<point3(0)<<","<<point3(1)<<"\" fill = \"rgba("
	      <<color(0)<<","<<color(1)<<","<<color(2)<<",1)\" />" <<endl;
    }
	     
      
    
    //end the svg file
    svgfile << "</svg>";
    svgfile.close();

  }
  else{
    cout << "File Not Created" <<endl;
  }
}
void exportToSVG2(std::string filename, int w_width, int w_height){
  //for every object get the translated vertex with a corresponding index number
  //for every 3 indices sort by barry and render from back to front
  ofstream svgfile(filename);
  if(svgfile.is_open()){
    //begin the svg file
    svgfile << "<svg height=\"" << w_height <<"\" width=\"" << w_width << "\">" << endl;
    
    MatrixXf rot = rotations;
    //rot(0) -= toRadian(5); rot(1) -= toRadian(5);  rot(1) -= toRadian(5);
    rot *= -1;
    Vector3f w_rot = worldRotation;
    w_rot *= -1;
    w_rot(2) *= -1;
    MatrixXf w_rotX(3,3), w_rotY(3,3), w_rotZ(3,3);
    w_rotX << 
      1,0,           0,
      0,cos(w_rot(0)),-sin(w_rot(0)),
      0,sin(w_rot(0)), cos(w_rot(0));
    w_rotY <<
      cos(w_rot(1)),0, sin(w_rot(1)),
      0,1,0,
      -sin(w_rot(1)),0,cos(w_rot(0)) ;
    w_rotZ <<
      cos(w_rot(2)), -sin(w_rot(2)), 0,
      sin(w_rot(2)), cos(w_rot(2)), 0,
      0, 0, 1;

    MatrixXf w_rotM = w_rotX * w_rotY * w_rotZ;

    MatrixXf V_1(9,1); MatrixXf centers(3,1); MatrixXf ind(1,1);
    V_1(0,0) = -100;
    
    for(int i = 0; i < Objs.cols(); i++){
      rot(2,i) *= -1;
      MatrixXf rotX(3,3), rotY(3,3), rotZ(3,3);
      rotX << 
	1, 0,             0,
	0, cos(rot(0,i)),-sin(rot(0,i)),
	0, sin(rot(0,i)), cos(rot(0,i));
      rotY <<
	cos(rot(1,i)), 0, sin(rot(1,i)),
	0,             1, 0,
	-sin(rot(1,i)), 0, cos(rot(1,i));
      rotZ <<
	cos(rot(2,i)),  -sin(rot(2,i)),0,
	sin(rot(2,i)),   cos(rot(2,i)),0,
	0,               0            ,1;
      MatrixXf rotM = rotX*rotY*rotZ;
      for(int j = Objs(0,i); j < Objs(1,i); j+=3){
	//if(ifRendered(i,j/3) == 0){
	Vector3f v1 = V.col(j);
	Vector3f v2 = V.col(j+1);
	Vector3f v3 = V.col(j+2);
	  
	Vector3f translation = translations.col(i);
	v1 = (v1.transpose() * rotM * scale(0,i) + translation.transpose()) * w_rotM * worldScale;
	v2 = (v2.transpose() * rotM * scale(0,i) + translation.transpose()) * w_rotM * worldScale;
	v3 = (v3.transpose() * rotM * scale(0,i) + translation.transpose()) * w_rotM * worldScale;
	
	//if world rotation around y axis is between 270 and 90 so 1/2pi to 1.5pi
	//so > 1.5pi or less than .5pi
	if(w_rot(1) >= -4/*(-1.5*M_PI)*/ && w_rot(1) <= -2.1/*(-0.5*M_PI)*/){
	  v1(2) *= -1; v2(2)*=-1; v3(2)*=-1;
	}

	if(ifPerspective == 1){ //more complicated
	  v1 = v1*(v1(2) * 0.5 + 1.0);
	  v2 = v2*(v2(2) * 0.5 + 1.0);
	  v3 = v3*(v3(2) * 0.5 + 1.0);
	}

	Vector3f center((v1(0)+v2(0)+v3(0))/3,
		      (v1(1)+v2(1)+v3(1))/3,
			(v1(2)+v2(2)+v3(2))/3);

	if(V_1(0,0) == -100){
	  V_1.col(0) << v1(0),v1(1),v1(2), v2(0),v2(1),v2(2), v3(0),v3(1),v3(2);
	  centers.col(0) << center(0), center(1), center(2);
	  ind(0,0) = j;
	}
	else{
	  V_1.conservativeResize(9, V_1.cols()+1);
	  V_1.col(V_1.cols()-1) << v1(0),v1(1),v1(2), v2(0),v2(1),v2(2), v3(0),v3(1),v3(2);
	  centers.conservativeResize(3,V_1.cols());
	  centers.col(V_1.cols()-1) << center(0), center(1), center(2);
	  ind.conservativeResize(1, V_1.cols());
	  ind(0, V_1.cols()-1) = j;
	}
      }
    }
    //sort all indices from back to front
    //cout << ind <<endl;
    sortFacesColors(V_1, centers, ind,  0, V_1.cols()-1); 
    //for(int i = 0; i < ind.cols(); i++){cout << centers(2,i) <<endl;}
    
    for(int i = 0; i < V_1.cols(); i++){
      Vector4f v1_1(V_1(0,i),V_1(1,i),V_1(2,i),1); v1_1 = v1_1.transpose()*view;
      Vector4f v2_1(V_1(3,i),V_1(4,i),V_1(5,i),1); v2_1 = v2_1.transpose()*view;
      Vector4f v3_1(V_1(6,i),V_1(7,i),V_1(8,i),1); v3_1 = v3_1.transpose()*view;
      
      Vector3f v1(v1_1(0),v1_1(1),v1_1(2)); Vector3f v2(v2_1(0),v2_1(1),v2_1(2)); Vector3f v3(v3_1(0),v3_1(1),v3_1(2));
      Vector3f normal = getTriangleNormal(v1,v2,v3).normalized()/*.cwiseAbs()*/; //normal vector
      Vector3f center((v1(0)+v2(0)+v3(0))/3,
		      (v1(1)+v2(1)+v3(1))/3,
		      (v1(2)+v2(2)+v3(2))/3);

      float normalAngle = normal.dot(v1);
      //if(normalAngle < 0){normal = normal.cwiseAbs();}

      Vector2f point1(v1_1(0),v1_1(1)); Vector2f point2(v2_1(0),v2_1(1)); Vector2f point3(v3_1(0),v3_1(1));
      Vector2f center1(center(0),center(1));

      Vector3f lightP = lightPos.transpose() * w_rotM * worldScale;
      //cout << lightP <<endl;
	    
      Vector3f eye = (-center).normalized();
      float dcolor = ((lightP - v1).normalized().transpose().dot(normal));
      if(normalAngle < 0){dcolor *= -1;}
      if(dcolor < 0){dcolor = 0;}
      
      //cout <<ind(0,i)<<endl;
      Vector3f color = C.col(ind(0,i))*(1+dcolor)*255; 
      if(color(0) > 255){color(0) = 255;}if(color(1) > 255){color(1) = 255;}if(color(2) > 255){color(2) = 255;}
      
      //do the aspect ratio thing before drawing by multiplying by view
      point1(0) = (point1(0)+1)*(w_width/2); point3(0) = (point3(0)+1)*(w_width/2); point2(0) = (point2(0)+1)*(w_width/2); 
      point1(1) = (point1(1)*-1+1)*(w_height/2); point3(1) = (point3(1)*-1+1)*(w_height/2); point2(1) = (point2(1)*-1+1)*(w_height/2);
      svgfile << "<polygon points=\""<<point1(0)<<","<<point1(1)<<" "<<point2(0)<<","<<point2(1)<<" "
	      <<point3(0)<<","<<point3(1)<<"\" fill = \"rgba("
	      <<color(0)<<","<<color(1)<<","<<color(2)<<",1)\" />" <<endl;
      /*"{"
                        "vec3 norm = normalize(f_normal);"
                        "vec3 eye = normalize(-f_position);"
                        "vec3 lightdir = normalize(f_lightPos - f_position);"
                        "vec3 reflectdir = reflect(-lightdir,norm);"
                        "float diffuse = clamp(dot(norm,lightdir),0,1);" //cosTheta
                        "vec3 specularStrength = vec3(1,1,1);"
                        "vec3 specFactor = vec3(1,1,1);"
                        "if(f_ifSpecular == 1){"
                            "specFactor = pow(max(dot(eye,reflectdir),0.0),32)*specularStrength;"
                            "outColor = vec4(f_color*(1+diffuse+specFactor), 1.0);" //1 is the ambient light str"
                        "}"
                        "else{"
                        "   outColor = vec4(f_color*(1+diffuse), 1.0);" //1 is the ambient light str
                        "}"
                    "}*/
    }
    //end the svg file
    svgfile << "</svg>";
    svgfile.close();

  }
  else{
    cout << "File Not Created" <<endl;
  }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
  glfwGetWindowSize(window, &width, &height);
  view(0,0) = float(height)/float(width);
  
  glViewport(viewX, viewY, width, height);
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    // Get the position of the mouse in the window
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // Get the size of the window
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Convert screen position to world coordinates
    Eigen::Vector4f p_screen(xpos,height-1-ypos,0,1);
    Eigen::Vector4f p_canonical(( (p_screen[0]-viewX )/width)*2-1,( (p_screen[1]-viewY)/height )*2-1,0,1);
    //add the xview y view and maybe any other translation here
    Eigen::Vector4f p_world = view.inverse()*p_canonical;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
      Vector2f point(p_world[0],p_world[1]); 
      selectedObj = isInsideObject(Objs, V, E, rotations, scale, translations, worldRotation, worldScale, point, ifPerspective);
      cout << selectedObj << " num objs " << Objs.cols() <<endl;
      //insert mode
      /*if(MODE == "INSERT"){
	V.conservativeResize(2, V.cols()+1);
	//V.col(V.cols()-1) << xworld, yworld;
	V.col(V.cols()-1) << p_world[0], p_world[1];
	if(V.cols()%3 == 0){ //if we have a full triangle then add colors
	  C.conservativeResize(3, V.cols());
	  C.col(V.cols()-1) << 0,0,1;
	  C.col(V.cols()-2) << 0,1,0;
	  C.col(V.cols()-3) << 1,0,0;
	}
      }
      //other modes 
      else{
	if(MODE != ""){
	  //check if mouse click inside triangle
	  //then do the specific modes (translation, delete, color)
	  //selectedTriangle = isInsideTriangle(V, xworld, yworld);
	  selectedTriangle = isInsideTriangle(V, p_world[0], p_world[1]);
	  if(selectedTriangle != -1){
	    //then hit
	    int triangleStartIndex = (selectedTriangle*3);
	    if(MODE == "TRANSLATION"){
	      //while mouse remains pressed move with cursor. need to get difference between original xworld and new x world same with y world
	      xOrigin = p_world[0];
	      yOrigin = p_world[1];
	    }
	    if(MODE == "DELETE"){//delete the three indexes and then shift the rest over then truncate the matrix.
	      for(int i = triangleStartIndex; i< V.cols()-3; i++){
		//if is the last 3 vertices will just truncate it
		cout << V.col(i) << endl;
		V.col(i) = V.col(i+3);
	      }
	      V.conservativeResize(2, V.cols()-3);
	    }
	    if(MODE == "COLOR"){
	      //get vertex closest to mouse click
	      //check xworld and yworld against each of the 3 vertices.
	      //Vector2d cursorPos(xworld,yworld);
	      Vector2d cursorPos(p_world[0],p_world[1]);
	      Vector2d
		vertex1((double)V(0,triangleStartIndex) ,(double)V(1,triangleStartIndex)),
		vertex2((double)V(0,triangleStartIndex+1) ,(double)V(1,triangleStartIndex+1)), vertex3((double)V(0,triangleStartIndex+2) ,(double)V(1,triangleStartIndex+2));
	      if( (vertex1 - cursorPos).norm() < (vertex2 - cursorPos).norm()
		  && (vertex1 - cursorPos).norm() < (vertex3 - cursorPos).norm()){
		selectedVertex = 1;
	      }
	      if((vertex2 - cursorPos).norm() < (vertex1 - cursorPos).norm()
		  && (vertex2 - cursorPos).norm() < (vertex3 - cursorPos).norm()){
		selectedVertex = 2;
	      }
	      if((vertex3 - cursorPos).norm() < (vertex2 - cursorPos).norm()
		  && (vertex3 - cursorPos).norm() < (vertex1 - cursorPos).norm()){
		selectedVertex = 3;
	      }
	    }
	  }
	}
      }*/

      //check if inside object.
    }

    // Upload the change to the GPU
    VBO.update(V);
    VBO_C.update(C);
}

int ifTrackball = 0;
Vector3f w_translate(0,0,-1);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){

  if(key == GLFW_KEY_1 && action == GLFW_RELEASE){addCube();}// insert cube
  if(key == GLFW_KEY_2 && action == GLFW_RELEASE){addBumpyCube();}// insert bumpycube
  if(key == GLFW_KEY_3 && action == GLFW_RELEASE){addBunny();}// insert bunny
  
  if(key == GLFW_KEY_O){ifPerspective = 0;} if(key == GLFW_KEY_P){ifPerspective=1;}

  if(key == GLFW_KEY_TAB && action == GLFW_RELEASE ){
    if(ifTrackball == 0){ifTrackball = 1; cout<<"Trackball"<<endl;} else{ifTrackball = 0; cout <<"no trackball"<<endl;}
  }
  //panning //qe to rotate around z axis 5 degrees at a time?
  if(key == GLFW_KEY_W){
    if(ifTrackball == 1){worldRotation(0) += toRadian(5);}
    if(ifTrackball == 0){
      float angle = atan(0.05/w_translate.norm());
      worldRotation(0) +=angle;
      float adj = w_translate.norm();
      //w_translate(1) -= tan(toRadian(5))/w_translate.norm(); //opposite = tan/adj
      w_translate(1) -= 0.05;
      float hypotenuse = w_translate.norm(); //hyp is distance to point
      float hypRatio = hypotenuse/adj;
      // scale * x = hypotenuse

      if(hypRatio > 1){hypRatio = 1 - (hypRatio-1);}
      else{hypRatio = 1 + (1-hypRatio);}
      worldScale *= hypRatio;
    } //hypotenuse = length to origin/cos angle
    if(worldRotation(0) < 0){worldRotation(0) += (2*M_PI);}
    if(worldRotation(0) > (2*M_PI)){worldRotation(0) -= (2*M_PI);}
  }
  if(key == GLFW_KEY_A){
    if(ifTrackball == 1){worldRotation(1) -= toRadian(5);}
    if(ifTrackball == 0){
      float adj = w_translate.norm();
      float angle = atan(0.05/w_translate.norm());
      worldRotation(1) -=angle;
      w_translate(0) += 0.05;
      //w_translate(0) += tan(toRadian(5))/w_translate.norm(); //opposite = tan/adj
      
      float hypotenuse = w_translate.norm(); //hyp is distance to point
      float hypRatio = hypotenuse/adj;
      // scale * x = hypotenuse

      if(hypRatio > 1){hypRatio = 1 - (hypRatio-1);}
      else{hypRatio = 1 + (1-hypRatio);}
      worldScale *= hypRatio;
    }
    if(worldRotation(1) < 0){worldRotation(1) += (2*M_PI);}
  }
  if(key == GLFW_KEY_S){
    if(ifTrackball == 1){worldRotation(0) -= toRadian(5);}    
    if(ifTrackball == 0){
      float adj = w_translate.norm();
      float angle = atan(0.05/w_translate.norm());
      worldRotation(0) -=angle;
      //w_translate(1) += tan(toRadian(5))/w_translate.norm(); //opposite = tan/adj
      //tan = opp/adj
      w_translate(1) += 0.05;
       
      float hypotenuse = w_translate.norm(); //hyp is distance to point
      float hypRatio = hypotenuse/adj;
      // scale * x = hypotenuse

      if(hypRatio > 1){hypRatio = 1 - (hypRatio-1);}
      else{hypRatio = 1 + (1-hypRatio);}
      worldScale *= hypRatio;
    }
    if(worldRotation(0) < 0){worldRotation(0) += (2*M_PI);}
    if(worldRotation(0) > (2*M_PI)){worldRotation(0) -= (2*M_PI);}
  }
  if(key == GLFW_KEY_D){
    if(ifTrackball ==1){worldRotation(1) += toRadian(5);}
    if(ifTrackball == 0){
      float adj = w_translate.norm();
      float angle = atan(0.05/w_translate.norm());
      worldRotation(1) +=angle;
      w_translate(0) -= 0.05;
      
      //w_translate(0) -= tan(toRadian(5))/w_translate.norm(); //opposite = tan/adj
      
      float hypotenuse = w_translate.norm(); //hyp is distance to point
      float hypRatio = hypotenuse/adj;
      // scale * x = hypotenuse

      if(hypRatio > 1){hypRatio = 1 - (hypRatio-1);}
      else{hypRatio = 1 + (1-hypRatio);}
      worldScale *= hypRatio;
    }
     if(worldRotation(1) > (2*M_PI)){worldRotation(1) -= (2*M_PI);}
  }
  if(key == GLFW_KEY_Q){worldRotation(2) -= toRadian(5);
    if(worldRotation(2) < 0){worldRotation(2) += (2*M_PI);}
    if(ifTrackball == 0){
      /* worldRotation(2) += toRadian(5);
      worldRotation(1) -= toRadian(5);
      float adj = w_translate.norm();
      w_translate(2) += tan(toRadian(5))/w_translate.norm(); //opposite = tan/adj
      
      float hypotenuse = w_translate.norm(); //hyp is distance to point
      float hypRatio = hypotenuse/adj;
      // scale * x = hypotenuse

      if(hypRatio > 1){hypRatio = 1 - (hypRatio-1);}
      else{hypRatio = 1 + (1-hypRatio);}
      worldScale *= hypRatio;*/
    }
  }
  if(key == GLFW_KEY_E){worldRotation(2) += toRadian(5);
     if(worldRotation(2) > (2*M_PI)){worldRotation(2) -= (2*M_PI);}
     if(ifTrackball == 0){
       /* worldRotation(2) -= toRadian(5);
       worldRotation(1) += toRadian(5);
      float adj = w_translate.norm();
      w_translate(2) -= tan(toRadian(5))/w_translate.norm(); //opposite = tan/adj
      
      float hypotenuse = w_translate.norm(); //hyp is distance to point
      float hypRatio = hypotenuse/adj;
      // scale * x = hypotenuse

      if(hypRatio > 1){hypRatio = 1 - (hypRatio-1);}
      else{hypRatio = 1 + (1-hypRatio);}
      worldScale *= hypRatio;*/
    }
  }

  //zooming in and out
  if(key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT || GLFW_KEY_KP_ADD || GLFW_KEY_EQUAL){
    if(key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT){worldScale *= 0.95; w_translate *= 0.95;}//zoom out
    if(key == GLFW_KEY_KP_ADD || key == GLFW_KEY_EQUAL){worldScale *= 1.05; w_translate *= 1.05;}//zoom in
  }

  //rotate and scale object hjkl and the arrow keys (hj is for rotating sideways like qe) toRadian(5);
  if(selectedObj != -1){
    //transformations
    //rotations
    if(key == GLFW_KEY_UP){ rotations(0,selectedObj) -= toRadian(5);} //-= rotation
    if(key == GLFW_KEY_DOWN){rotations(0,selectedObj) += toRadian(5);} //+= rotation
    if(key == GLFW_KEY_LEFT){ rotations(1,selectedObj) += toRadian(5);} //-=
    if(key == GLFW_KEY_RIGHT){rotations(1,selectedObj) -= toRadian(5); } //+=
    if(key == GLFW_KEY_H){rotations(2,selectedObj) += toRadian(5);} //-=
    if(key == GLFW_KEY_J){rotations(2,selectedObj) -= toRadian(5);} //+=

    //scaling
    if(key == GLFW_KEY_K){scale(0,selectedObj) *= 1.05;} //scale up
    if(key == GLFW_KEY_L){scale(0,selectedObj) *= 0.95;} //scale down

    //translation
    if(key == GLFW_KEY_G){translations(1,selectedObj) += 0.05;} 
    if(key == GLFW_KEY_C){translations(0,selectedObj) += 0.05;} 
    if(key == GLFW_KEY_V){translations(1,selectedObj) -= 0.05;} 
    if(key == GLFW_KEY_B){translations(0,selectedObj) -= 0.05;}
    if(key == GLFW_KEY_T){translations(2,selectedObj) -= 0.05;}
    if(key == GLFW_KEY_Y){translations(2,selectedObj) += 0.05;}
    
    //shading modes
    if(key == GLFW_KEY_8){Objs(3, selectedObj) = 1;}
    if(key == GLFW_KEY_9){Objs(3, selectedObj) = 2;}
    if(key == GLFW_KEY_0){Objs(3, selectedObj) = 3;}

    //backspace for delete
    if(key == GLFW_KEY_BACKSPACE && action == GLFW_RELEASE){deleteObj(selectedObj); selectedObj = -1;}
  }

  //print screen to export.
  //print screen for export
    if(key == GLFW_KEY_F12 && action == GLFW_RELEASE){
      int width, height;
      glfwGetWindowSize(window, &width, &height);
      exportToSVG("a3.svg", width, height);
    }
    if(key == GLFW_KEY_Z && action == GLFW_RELEASE){
      int width, height;
      glfwGetWindowSize(window, &width, &height);
      exportToSVG2("a3.svg", width, height);
    }

  // Upload the change to the GPU
  IBO.update(E);
}

int main(void)
{
  scale << 1;
  worldRotation(1) += toRadian(180);
  
    GLFWwindow* window;

    //Init stuff
    
    // Initialize the library
    if (!glfwInit())
        return -1;

    // Activate supersampling
    glfwWindowHint(GLFW_SAMPLES, 8);

    // Ensure that we get at least a 3.2 context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    // On apple we have to load a core profile with forward compatibility
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(640, 480, ":Triangle Soup:", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    //something to do with apple
    #ifndef __APPLE__
      glewExperimental = true;
      GLenum err = glewInit();
      if(GLEW_OK != err)
      {
        /* Problem: glewInit failed, something is seriously wrong. */
       fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
      }
      glGetError(); // pull and savely ignonre unhandled errors like GL_INVALID_ENUM
      fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    #endif

    // print version info
    int major, minor, rev;
    major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
    minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
    rev = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);
    printf("OpenGL version recieved: %d.%d.%d\n", major, minor, rev);
    printf("Supported OpenGL is %s\n", (const char*)glGetString(GL_VERSION));
    printf("Supported GLSL is %s\n", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

    // Initialize the VAO
    // A Vertex Array Object (or VAO) is an object that describes how the vertex
    // attributes are stored in a Vertex Buffer Object (or VBO). This means that
    // the VAO is not the actual object storing the vertex data,
    // but the descriptor of the vertex data.
    VertexArrayObject VAO;
    VAO.init();
    VAO.bind();

    // Initialize the VBO with the vertices data
    // A VBO is a data container that lives in the GPU memory
    VBO.init();
    V.resize(3,3);
    V <<
      0,     0.05, -0.05, //x
      0.05, -0.05, -0.05, //y
      0 ,    0 ,    0;    //z
    VBO.update(V);

    IBO.init();
    IBO.bind();
    E << 0,1,2;
    IBO.update(E);
    
    // Second VBO for colors
    VBO_C.init();

    C.resize(3,3);
    C <<
      1,  0, 0,
      0,  1, 0,
      0,  0, 1;

    NormalBuffer.init();
    NormalBuffer.bind();
    
    //add first triangles normals
    Vector3f a = getTriangleNormal(V.col(0), V.col(1), V.col(2));
    for(int i = 0; i < 3; i++){
      N.col(i) = a;
    }

    //cubeOffset = importCube(); //cube offset 3 num vertex 8 (indexes 3 to 10)
    //bunnyOffset = importBunny();
    //bumpyOffset = importBumpyCube();

    cubeOffset = importCubeVertex(); //cube offset 3 num vertex 8 (indexes 3 to 10)
    bunnyOffset = importBunnyVertex();
    bumpyOffset = importBumpyCubeVertex(); 
    
    VBO.update(V);
    VBO_C.update(C);
    NormalBuffer.update(N);
    
    // Initialize the OpenGL Program
    // A program controls the OpenGL pipeline and it must contains
    // at least a vertex shader and a fragment shader to be valid
    Program program;
    const GLchar* vertex_shader =   
            "#version 150 core\n"
                    "in vec3 position;"        "in vec3 color;""in vec3 normal;"
                    "uniform vec3 lightPos;"
                    "uniform vec3 rotation;" //rotation matrix the degrees of rotation for x y and z axis
                                    //access vec 3 via vec3.x vec3.y vec3.z respectively for (0) (1) and (2)
                    "uniform vec3 translation;" //translation matrix
                    "uniform float scale;" //scaling matrix
                    "uniform vec3 w_rotation;" "uniform float w_scale;"
                    "uniform float ambient;" "flat out float f_ambient;"
                    "out vec3 f_color;"        "out vec3 f_position;" "out vec3 f_normal;"
                    "out vec3 f_lightPos;" 
                    "uniform mat4 view;"
                    "uniform int ifSpecular;"  "flat out int f_ifSpecular;"
                    "uniform int ifPerspective;"
                    "void main()"
                    "{"
                        "mat3 rotX = mat3("
                          "vec3( 1,   0.0,               0.0),"
                          "vec3(0.0,  cos(rotation.x),  -sin(rotation.x)),"
                          "vec3(0.0,  sin(rotation.x),   cos(rotation.x))"
                        ");"
                        "mat3 rotY = mat3("
                          "vec3( cos(rotation.y),  0.0,  sin(rotation.y)),"
                          "vec3( 0.0,              1.0,  0.0),"
                          "vec3(-sin(rotation.y),  0.0,  cos(rotation.y))"
                        ");"
                        "mat3 rotZ = mat3("
                          "vec3( cos(rotation.z),  sin(rotation.z),  0.0),"
                          "vec3(-sin(rotation.z),  cos(rotation.z),  0.0),"
                          "vec3(        0.0,         0.0,            1.0)"
                        ");" //obj rotation matrices
                        "mat3 w_rotX = mat3("
                          "vec3( 1,   0.0,               0.0),"
                          "vec3(0.0,  cos(w_rotation.x),  -sin(w_rotation.x)),"
                          "vec3(0.0,  sin(w_rotation.x),   cos(w_rotation.x))"
                        ");"
                        "mat3 w_rotY = mat3("
                          "vec3( cos(w_rotation.y),  0.0,  sin(w_rotation.y)),"
                          "vec3( 0.0,              1.0,  0.0),"
                          "vec3(-sin(w_rotation.y),  0.0,  cos(w_rotation.y))"
                        ");"
                        "mat3 w_rotZ = mat3("
                          "vec3( cos(w_rotation.z),  sin(w_rotation.z),  0.0),"
                          "vec3(-sin(w_rotation.z),  cos(w_rotation.z),  0.0),"
                          "vec3(        0.0,         0.0,            1.0)"
                        ");" //world rotation matrices
                        "mat3 rotMat = rotX * rotY *rotZ;"
                        "mat3 w_rotMat = w_rotX * w_rotY * w_rotZ;"
                        "vec3 projectedPos = (position * rotMat * scale + translation) * w_rotMat* w_scale;"
                        "float perspectiveFactor = projectedPos.z * 0.5 + 1.0;"
      
                        "if(ifPerspective == 0){gl_Position = view * vec4(projectedPos, 1.0);}"
                        "if(ifPerspective == 1){gl_Position = view * vec4(projectedPos/perspectiveFactor, 1.0);}"
      
      //rotation and scale first since they need to be done at bary, then translate
                        "f_color = color;"
                        "f_normal = normal;"
                        "f_position = projectedPos;"
                        "f_lightPos = lightPos;"
                        "f_ifSpecular = ifSpecular;"
"f_ambient = ambient;"
                    "}";
    const GLchar* fragment_shader = 
            "#version 150 core\n"
                    "in vec3 f_color;"
                    "in vec3 f_position;"
                    "in vec3 f_lightPos;"
                    "in vec3 f_normal;"
      "flat in float f_ambient;"
                    "flat in int f_ifSpecular;"
                    "out vec4 outColor;"
                    "uniform vec3 triangleColor;"
                    "void main()"
                    "{"
                        "vec3 norm = normalize(f_normal);"
                        "vec3 eye = normalize(-f_position);"
                        "vec3 lightdir = normalize(f_lightPos - f_position);"
                        "vec3 reflectdir = reflect(-lightdir,norm);"
                        "float diffuse = clamp(dot(norm,lightdir),0,1);" //cosTheta
                        "vec3 specularStrength = vec3(1,1,1);"
                        "vec3 specFactor = vec3(1,1,1);"
                        "if(f_ifSpecular == 1){"
                            "specFactor = pow(max(dot(eye,reflectdir),0.0),32)*specularStrength;"
                            "outColor = vec4(f_color*((1*f_ambient)+diffuse+specFactor), 1.0);" //1 is the ambient light str"
                        "}"
                        "else{"
                        "   outColor = vec4(f_color*(1+diffuse), 1.0);" //1 is the ambient light str
                        "}"
                    "}";

    // Compile the two shaders and upload the binary to the GPU
    // Note that we have to explicitly specify that the output "slot" called outColor
    // is the one that we want in the fragment buffer (and thus on screen)
    program.init(vertex_shader,fragment_shader,"outColor");
    program.bind();

    program.bindVertexAttribArray("position",VBO);
    program.bindVertexAttribArray("color",VBO_C);
    program.bindVertexAttribArray("normal",NormalBuffer);

    // Save the current time --- it will be used to dynamically change the triangle color
    auto t_start = std::chrono::high_resolution_clock::now();

    // keyboard mouse framebuffer callbacks
    // Register the keyboard callback
    glfwSetKeyCallback(window, key_callback);

    // Register the mouse callback
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // Update viewport
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //glClearColor(1.0f, 0.0f, 1.0f, 1.0f); 
    //glClearColor(0.9f, 0.8f, 0.8f, 1.0f); //red = 0.9686;  green = 0.79216; blue = 0.7882;
    //glClearColor(0.6f, 0.7f, 0.8f, 1.0f);//red = 0.568627; green = 0.6588; blue = 0.8196;
    glClearColor(0.5f, 0.0f, 0.3f, 1.0f); //red = 0.529; blue = 0.29;
        
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Get size of the window
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    float aspect_ratio = float(height)/float(width); // corresponds to the necessary width scaling
    view <<
      aspect_ratio,0, 0, 0,
      0,           1, 0, 0,
      0,           0, 1, 0,
      0,           0, 0, 1;
    glUniformMatrix4fv(program.uniform("view"), 1, GL_FALSE, view.data());
    glUniform3f(program.uniform("lightPos"), lightPos(0), lightPos(1), lightPos(2));

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)){
      // Bind your VAO (not necessary if you have only one)
      VAO.bind();

      // Bind your program
      program.bind();

      //update the matrix incase frame buffer has changed.
      //we do perspective/ortho switching here instead of to key because frame buffer changes
      glUniformMatrix4fv(program.uniform("view"), 1, GL_FALSE, view.data());
      glUniform1i(program.uniform("ifPerspective"),ifPerspective);
      glUniform3f(program.uniform("w_rotation"), worldRotation(0), worldRotation(1), worldRotation(2));
      glUniform1f(program.uniform("w_scale"), worldScale);
      
      //void glUniformMatrix4fv(GLint location,GLsizei count,GLboolean transpose,const GLfloat *value);

      //gluLookAt (camera.x, camera.y, camera.z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

      // Set the uniform value depending on the time difference
      auto t_now = std::chrono::high_resolution_clock::now();
      float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
      //glUniform3f(program.uniform("triangleColor"), (float)(sin(time * 4.0f) + 1.0f) / 2.0f, 0.0f, 0.0f);
	
	
      //glMatrixMode(GL_MODELVIEW);
      // Clear the framebuffer
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//get cursor coords
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	glfwGetWindowSize(window, &width, &height);

	// Convert screen position to world coordinates
        Eigen::Vector4f p_screen(xpos,height-1-ypos,0,1);
	Eigen::Vector4f p_canonical(( (p_screen[0])/width)*2-1,( (p_screen[1])/height )*2-1,0,1);
	Eigen::Vector4f p_world = view.inverse()*p_canonical;

	/*if(MODE == "TRANSLATION" && selectedTriangle != -1 && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_RELEASE){
	  double diffX = p_world[0] - xOrigin;
	  double diffY = p_world[1] - yOrigin;

	  for(int i = selectedTriangle*3; i < selectedTriangle*3+3; i++){
	    V(0, i) = V(0, i) + diffX;
	    V(1, i) = V(1, i) + diffY;
	  }
	  xOrigin = p_world[0];
	  yOrigin = p_world[1];
	  VBO.update(V);
	}*/ //drag and move

	// Draw triangles

	C_Black.conservativeResize(3, C.cols());
	C_Black.setZero();

	//cube bunny bumpy

	/*for(int vIndex = 0; vIndex+3 <= V.cols(); vIndex+=3){
	  glDrawArrays(GL_TRIANGLES, vIndex, 3);
	  //if(MODE == "TRANSLATION" && rotation != 0){
	  //  glPopMatrix();
	  //}
	  }*/

	int wasCube = 0;
	if(selectedObj != -1){
	  Vector3f c1(0,0.75,0.75); Vector3f c2(1,0,1);
	  if(Objs(2,selectedObj) == 36){for(int i = 0; i < 36; i++){C.col(Objs(0,selectedObj)+i) = c1; }}
	  //else{for(int i = 0; i < 36; i++){C.col(Objs(0,selectedObj)+i) = c2;}}
	  VBO_C.update(C);
	}
	  
	if(Objs.cols() > 1){
	  for(int i = 1; i < Objs.cols(); i++){ 
	    //for each object upload its rotation translation and scale matrices
	    //then draw objects
	    float ambient = 1.0;
	    if(i == selectedObj){//cout << i << ":" <<selectedObj<<endl;
	      ambient = 0.75;
	    }
	    glUniform1f(program.uniform("ambient"), ambient);
	  
	    glUniform3f(program.uniform("rotation"), rotations(0,i), rotations(1,i), rotations(2,i));
	    glUniform3f(program.uniform("translation"), translations(0,i),translations(1,i),translations(2,i));
	    glUniform1f(program.uniform("scale"), scale(i));
	    glUniform1i(program.uniform("ifSpecular"), 0);
	    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	    
	    if(Objs(3,i) == 1){glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );}
	    if(Objs(3,i) == 3){glUniform1i(program.uniform("ifSpecular"), 1);}//phong
	    //cout << Objs(0,i) <<"-" <<Objs(1,i)<<endl;
	    for(int j = Objs(0,i); j < Objs(1,i); j+=3){
	      glDrawArrays(GL_TRIANGLES, j, 3);
	    }
	    //glDrawElements(GL_TRIANGLES, Objs(2,i), GL_UNSIGNED_INT, (void*)Objs(0,i));
	    if(Objs(3,i) == 2){
	      VBO_C.update(C_Black);
	      glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	      for(int j = Objs(0,i); j < Objs(1,i); j+=3){glDrawArrays(GL_TRIANGLES, j, 3);}
	      //glDrawElements(GL_TRIANGLES, Objs(2,i), GL_UNSIGNED_INT, (void*)Objs(0,i));
	      VBO_C.update(C);
	    }//flat + wireframe
	  }
	}
	if(selectedObj != -1){
	  Vector3f c2(0.75,0,0.75);
	  if(Objs(2,selectedObj) == 36){for(int i = 0; i < 36; i++){C.col(Objs(0,selectedObj)+i) = c2;}}
	  VBO_C.update(C);
	}

        // Swap front and back buffers
        glfwSwapBuffers(window);
        // Poll for and process events
        glfwPollEvents();
    }

    // Deallocate opengl memory
    program.free();
    VAO.free();
    VBO.free();
    IBO.free();
    VBO_C.free();

    // Deallocate glfw internals
    glfwTerminate();
    return 0;
}
