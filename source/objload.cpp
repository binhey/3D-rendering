/*
 *  objtest.cpp
 *
 *  Program to test OBJ File reader Code
 *  Used for Programming Project 6, CPSC 405/605
 *
 *  Donald H. House,   3/30/2011
 *
 *  usage: objtest filename.obj
 *
  Loads the OBJ file and then prints the polygonal surface data structure
  corresponding to the OBJ file and any MTL file loaded with it.
*/
#include "objload.h"


//
// Load the scene and get the centroid and bounding box
//
void load_scene(PolySurf **scene, OBJFile &objfile){

  int err = objfile.read();
  *scene = objfile.getscene();

  if(err || scene == NULL){
    char *filename = objfile.getfilename();
    cerr << "OBJ file " << filename << " has errors" << endl;
    exit(2);
  }
}

//
// Validate command line, and initialize viewport width, and output filename
//
void get_commandline(const char* file_name, OBJFile &objfile){
  int suffix;
  
  suffix = strlen(file_name) - 4;
  if(strcmp(&(file_name[suffix]), ".obj") != 0)
    goto abort;
  objfile.setfilename(file_name);
  return;

abort:
  cerr << "usage: objtest filename.obj" << endl;
  exit(1);
}
/*
//
// Main program to load the OBJ file and print the polygonal surface
// data structure
//
int main(int argc, char* argv[]){
  OBJFile objfile;
  PolySurf *scene;	      // polygonal surface data structure
  Vector3d centroid, bboxmin, bboxmax;  // center and bounding box

  // check the command line first, we may abort
  get_commandline(argc, argv, objfile);

  // load scene and print the scene
  load_scene(&scene, objfile);
  cout << *scene << endl;
  
  // get and print the centroid and bounding box
  centroid = scene->Centroid();
  bboxmin = scene->MinBBox();
  bboxmax = scene->MaxBBox();
  cout << "Centroid = " << centroid << ", Bounding Box: (" << bboxmin << ", " << bboxmax << ")" << endl;
}*/
