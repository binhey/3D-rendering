/*
 *  objload.h
 *  
 *
 *  Created by Donald House.
 *  Copyright 2011 Clemson University. All rights reserved.
 *
 */

#ifndef __OBJLOAD__
#define __OBJLOAD__

#include <cstdlib>
#include <iostream>

#include "Vector.h"
#include "OBJFile.h"
#include "PolySurf.h"

using namespace std;


//
// Load the scene and get the centroid and bounding box
//
void load_scene(PolySurf **scene, OBJFile &objfile);

//
// Validate command line, and initialize viewport width, and output filename
//
void get_commandline(const char* file_name, OBJFile &objfile);

#endif
