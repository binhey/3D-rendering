/*
   mipmaps.cpp
   
   CP SC 405/605 Computer Graphics    Donald H. House     12/2/08
   
   OpenGL/GLUT Program to View a 3D cube or teapot using a mipmapped textures
   
   Keyboard keypresses have the following effects:
   a		- toggle drawing coordinate axes
   i       	- reinitialize (reset program to initial default state)
   p		- toggle between orthographic and perspective view
   q or Esc	- quit
   s		- toggle between flat shading and smooth shading
   t		- cycle through texturing modes (nearest, interpolation, mipmapped)
   w		- toggle between wireframe and shaded viewing

   Camera and model controls following the mouse:
   model yaw	- left-button, horizontal motion, rotation of the model around the y axis
   model tilt	- left-button, vertical motion, rotation of the model about the x axis
   camera yaw	- middle-button, horizontal motion, rotation of the camera about the y axis
   camera tilt	- middle-button, vertical motion, rotation of the camera about the x axis
   approach	- right-button, vertical or horizontal motion, translation of camera along z axis
   
   usage: mipmaps
 */

#include <cstdlib>
#include <cstdio>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include "Model.h"
#include "objtrace.h"
using namespace std;

//*****************************************************************
//  Demo Code
//*****************************************************************

#define FALSE		0
#define TRUE		1

#define HORIZONTAL	0
#define VERTICAL	1

#define ESC		27	// numeric code for keyboard Esc key

#define WIDTH           1200	// initial window dimensions
#define HEIGHT          960

#define ORTHO		0	// projection system codes
#define PERSPECTIVE	1

#define NONE		-1	// used to indicate no mouse button pressed

#define DRAWWIDTH	100	// view volume sizes (note: width and
#define DRAWHEIGHT	80	//   height should be in same ratio as window)
#define NEAR		10	// distance of near clipping plane
#define FAR		10000	// distance of far clipping plane

#define CUBEWIDTH	50	// dimension of the cube
#define DEPTH		-100	// initial z coord. of center of cube

#define ROTFACTOR	0.2     // degrees rotation per pixel of mouse movement
#define XLATEFACTOR	0.5     // units of translation per pixel of mouse movement



#define AMBIENT_FRACTION 0.2
#define DIFFUSE_FRACTION 0.8
#define SPECULAR_FRACTION 0.3

// colors used for lights, and materials for coordinate axes
const float DIM_PALEBLUE[] = {0.1, 0.1, 0.3, 1};
const float BRIGHT_PALEBLUE[] = {0.5, 0.5, 1, 1};
const float GRAY[] = {0.3, 0.3, 0.3, 1};
const float WHITE[] = {1, 1, 1, 1};
const float DIM_WHITE[] = {0.8, 0.8, 0.8, 1};
const float DIM_RED[] = {0.3, 0, 0, 1};
const float RED[] = {1, 0, 0, 1};
const float DIM_GREEN[] = {0, 0.3, 0, 1};
const float GREEN[] = {0, 1, 0, 1};
const float DIM_BLUE[] = {0, 0, 0.3, 1};
const float BLUE[] = {0, 0, 1, 1};

enum TEXTUREMODE{NOTEXTURE, NEARTEXTURE, LINEARTEXTURE, MIPMAPTEXTURE};

// white color for wireframe drawing
float white[3] = {1, 1, 1};

//
// Global variables updated and shared by callback routines
//

// Window dimensions
static double Width = WIDTH;
static double Height = HEIGHT;

// Viewing parameters
static int Projection;

// Model and Shading parameters

static int Axes;

// Camera position and orientation
static double Pan;
static double Tilt;
static double Approach;

// model orientation
static double ThetaX;
static double ThetaY;

// global variables to track mouse and shift key
static int MouseX;
static int MouseY;
static int Button = NONE;

// global variables to track wireframe/shaded mode, material color, and textuing mode
static int Wireframe;
static int Color;
static int SmoothShading;
static int TextureMode;
static int ColorMode;

// global variables to hold geometric models
Model Cylinder;
Model Cone;
PolySurf *obj_scene = NULL;

Vector3d init_transform(Vector3d x)
{
	Matrix4x4 I;
	I.identity();
	Vector3d pp1;
	Vector4d v(x[0],x[1],x[2],1);
	 I = xy_rotateX(I,-90.0/180.0*3.141592653589793);
	 I = xy_rotateY(I,30.0/180.0*3.141592653589793);
	 I = xy_translate(I,Vector3d(10,-10,-8));
	v = I*v;
	pp1[0] = v[0];
	pp1[1] = v[1];
	pp1[2] = v[2]; 
	return pp1;
}

Vector3d init_normal_transform(Vector3d x)
{
	Matrix4x4 I;
	I.identity();
	Vector3d pp1;
	Vector4d v(x[0],x[1],x[2],1);
	 I = xy_rotateX(I,-90.0/180.0*3.141592653589793);
	 I = xy_rotateY(I,30.0/180.0*3.141592653589793);
	v = I*v;
	pp1[0] = v[0];
	pp1[1] = v[1];
	pp1[2] = v[2]; 
	return pp1;
}

Matrix4x4 camara_matrix;


unsigned char* teapot_texture = NULL;
int texture_img_w;
int texture_img_h;

unsigned char* desk_texture = NULL;
int texture1_img_w;
int texture1_img_h;

unsigned char* wall_texture = NULL;
int texture2_img_w;
int texture2_img_h;


void draw_desk()
{
	float ambient_color[4] = {0.1,0.1,0.1,1};
	float diffuse_color[4] = {1.9,1.9,1.9,1};
	float specular_color[4] = {1,1,1,1};
	int shininess = 600;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_color);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_color);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular_color);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    
	    if(TextureMode != NOTEXTURE){
	      glEnable(GL_TEXTURE_2D);
	      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, ColorMode);
	      glBindTexture(GL_TEXTURE_2D, 1);	    // set the active texture
	      switch(TextureMode){
		case NEARTEXTURE:
		  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		  break;
		case LINEARTEXTURE:
		  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		  break;
		case MIPMAPTEXTURE:
		  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		  break;
	      }
	    }	
	glBindTexture(GL_TEXTURE_2D, 2);
	int op = (Wireframe? GL_LINE_LOOP: GL_POLYGON);
	
	Vector3d pp1(-100,-10,100),pp2(100,-10,100),pp3(100,-10,-100), pp4(-100,-10,-100);
	double s_x = 20,s_y = 20,t_x = 0,t_y = 0;
	Vector2d UV1(0*s_x+t_x,0*s_y+t_y),UV2(1*s_x+t_x,0*s_y+t_y),UV3(1*s_x+t_x,1*s_y+t_y),UV4(0*s_x+t_x,1*s_y+t_y);
	Vector3d Vn1(0,1,0),Vn2(0,1,0),Vn3(0,1,0),Vn4(0,1,0);
        int index;
	
			glBegin(op);
				if(!Wireframe)glNormal3f(Vn1.x, Vn1.y, Vn1.z);			
				glTexCoord2f(UV1.x, UV1.y);
				glVertex3f(pp1.x,pp1.y,pp1.z);
				if(!Wireframe)glNormal3f(Vn2.x, Vn2.y, Vn2.z);			
				glTexCoord2f(UV2.x, UV2.y);
				glVertex3f(pp2.x,pp2.y,pp2.z);
				if(!Wireframe)glNormal3f(Vn3.x, Vn3.y, Vn3.z);			
				glTexCoord2f(UV3.x, UV3.y);
				glVertex3f(pp3.x,pp3.y,pp3.z);
				if(!Wireframe)glNormal3f(Vn4.x, Vn4.y, Vn4.z);			
				glTexCoord2f(UV4.x, UV4.y);
				glVertex3f(pp4.x,pp4.y,pp4.z);	
    			glEnd();

}

void draw_wall()
{
	float ambient_color[4] = {1,1,1,1};
	float diffuse_color[4] = {0,0,0,1};
	float specular_color[4] = {1,1,1,1};
	int shininess = 600;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_color);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_color);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular_color);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    
	    if(TextureMode != NOTEXTURE){
	      glEnable(GL_TEXTURE_2D);
	      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, ColorMode);
	      glBindTexture(GL_TEXTURE_2D, 1);	    // set the active texture
	      switch(TextureMode){
		case NEARTEXTURE:
		  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		  break;
		case LINEARTEXTURE:
		  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		  break;
		case MIPMAPTEXTURE:
		  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		  break;
	      }
	    }
	glBindTexture(GL_TEXTURE_2D, 4);
	int op = (Wireframe? GL_LINE_LOOP: GL_POLYGON);
	Vector3d pp1(-100,-100,-20),pp2(100,-100,-20),pp3(100,100,-20), pp4(-100,100,-20);
	double s_x = 2.2,s_y = 3.2,t_x = -0.6,t_y = -0.35;
	Vector2d UV1(0*s_x+t_x,0*s_y+t_y),UV2(1*s_x+t_x,0*s_y+t_y),UV3(1*s_x+t_x,1*s_y+t_y),UV4(0*s_x+t_x,1*s_y+t_y);
	Vector3d Vn1(0,1,0),Vn2(0,1,0),Vn3(0,1,0),Vn4(0,1,0);
        int index;
	glRotatef(45,0,1,0);
			glBegin(op);
				if(!Wireframe)glNormal3f(Vn1.x, Vn1.y, Vn1.z);			
				glTexCoord2f(UV1.x, UV1.y);
				glVertex3f(pp1.x,pp1.y,pp1.z);
				if(!Wireframe)glNormal3f(Vn2.x, Vn2.y, Vn2.z);			
				glTexCoord2f(UV2.x, UV2.y);
				glVertex3f(pp2.x,pp2.y,pp2.z);
				if(!Wireframe)glNormal3f(Vn3.x, Vn3.y, Vn3.z);			
				glTexCoord2f(UV3.x, UV3.y);
				glVertex3f(pp3.x,pp3.y,pp3.z);
				if(!Wireframe)glNormal3f(Vn4.x, Vn4.y, Vn4.z);			
				glTexCoord2f(UV4.x, UV4.y);
				glVertex3f(pp4.x,pp4.y,pp4.z);	
    			glEnd();
	glRotatef(-45,0,1,0);
}


void draw_obj()
{
	float ambient_color[4] = {0.5,0.5,0.5,1};
	float diffuse_color[4] = {1,1,1,1};
	float specular_color[4] = {1,1,1,1};
	int shininess = 600;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_color);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_color);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular_color);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    
	    if(TextureMode != NOTEXTURE){
	      glEnable(GL_TEXTURE_2D);
	      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, ColorMode);
	      glBindTexture(GL_TEXTURE_2D, 1);	    // set the active texture
	      switch(TextureMode){
		case NEARTEXTURE:
		  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		  break;
		case LINEARTEXTURE:
		  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		  break;
		case MIPMAPTEXTURE:
		  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		  break;
	      }
	    }	
	
	int op = (Wireframe? GL_LINE_LOOP: GL_POLYGON);
	glBindTexture(GL_TEXTURE_2D, 1);
	Vector3d pp1,pp2,pp3;
	Vector2d UV1,UV2,UV3;
	Vector3d Vn1,Vn2,Vn3;
        int index;
	for(int fi = 0; fi < obj_scene->nfaces; fi++)
	{
	
		for(int fj = 1; fj <= obj_scene->faces[fi].nverts-2; fj++)
		{
			//cout<<fi<<","<<fj<<endl;
			
			index = obj_scene->faces[fi].verts[0][0];
			pp1 = obj_scene->verts[index];
			index = obj_scene->faces[fi].verts[0][1];
			Vn1 = obj_scene->norms[index];
                        index = obj_scene->faces[fi].verts[0][2];
			UV1 = obj_scene->uvs[index];

			index = obj_scene->faces[fi].verts[fj][0];
			pp2 = obj_scene->verts[index];
			index = obj_scene->faces[fi].verts[fj][1];
			Vn2 = obj_scene->norms[index];
			index = obj_scene->faces[fi].verts[fj][2];
			UV2 = obj_scene->uvs[index];

			index = obj_scene->faces[fi].verts[fj+1][0];
			pp3 = obj_scene->verts[index];
			index = obj_scene->faces[fi].verts[fj+1][1];
			Vn3 = obj_scene->norms[index];
			index = obj_scene->faces[fi].verts[fj+1][2];
			UV3 = obj_scene->uvs[index];
			
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_DST_COLOR);
			glDisable(GL_DEPTH_TEST);
			glBegin(op);
				if(!Wireframe)glNormal3f(Vn1.x, Vn1.y, Vn1.z);			
				glTexCoord2f(UV1.x, UV1.y);
				glVertex3f(pp1.x,pp1.y,pp1.z);
				if(!Wireframe)glNormal3f(Vn2.x, Vn2.y, Vn2.z);			
				glTexCoord2f(UV2.x, UV2.y);
				glVertex3f(pp2.x,pp2.y,pp2.z);
				if(!Wireframe)glNormal3f(Vn3.x, Vn3.y, Vn3.z);			
				glTexCoord2f(UV3.x, UV3.y);
				glVertex3f(pp3.x,pp3.y,pp3.z);	
    			glEnd();
			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);			

			pp1 = init_transform(pp1);
			pp2 = init_transform(pp2);
			pp3 = init_transform(pp3);
			Vn1 = init_normal_transform(Vn1);
			Vn2 = init_normal_transform(Vn2);
			Vn3 = init_normal_transform(Vn3);
			
			  
 
			glBegin(op);
				if(!Wireframe)glNormal3f(Vn1.x, Vn1.y, Vn1.z);			
				glTexCoord2f(UV1.x, UV1.y);
				glVertex3f(pp1.x,pp1.y,pp1.z);
				if(!Wireframe)glNormal3f(Vn2.x, Vn2.y, Vn2.z);			
				glTexCoord2f(UV2.x, UV2.y);
				glVertex3f(pp2.x,pp2.y,pp2.z);
				if(!Wireframe)glNormal3f(Vn3.x, Vn3.y, Vn3.z);			
				glTexCoord2f(UV3.x, UV3.y);
				glVertex3f(pp3.x,pp3.y,pp3.z);	
    			glEnd();
				
		}
	}
}

// Texture map to be used by program
static GLuint TextureID;	    // texture ID from OpenGL
static unsigned char *TextureImage;   // pixmap for texture image



//
// Routine to initialize the state of the program to start-up defaults
//
void setInitialState(){
  
  // initial camera viewing and shading model controls
  Projection = PERSPECTIVE;
  
  // model initially wireframe with white color, and flat shading
  Wireframe = FALSE;
  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
  TextureMode = MIPMAPTEXTURE;
  ColorMode = GL_MODULATE;
  SmoothShading = TRUE;
  
  // no axes drawn
  Axes = FALSE;
  
  // initial camera orientation and position
  Pan = 0;
  Tilt = 0;
  Approach = DEPTH;
  
  // initial model orientation
  ThetaX = 0;
  ThetaY = 0;
}

//
// Set up the projection matrix to be either orthographic or perspective
//
void updateProjection(){
  
  // initialize the projection matrix
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
  // determine the projection system and drawing coordinates
  if(Projection == ORTHO)
    glOrtho(-DRAWWIDTH/2, DRAWWIDTH/2, -DRAWHEIGHT/2, DRAWHEIGHT/2, NEAR, FAR);
  else{
    // scale drawing coords so center of model is same size as in ortho
    // if it is at its nominal location
    double scale = fabs((double)NEAR / (double)DEPTH);
    double xmax = scale * DRAWWIDTH / 2;
    double ymax = scale * DRAWHEIGHT / 2;
    glFrustum(-xmax, xmax, -ymax, ymax, NEAR, FAR);
  }
  
  // restore modelview matrix as the one being updated
  glMatrixMode(GL_MODELVIEW);
}

//
// routine to draw a set of coordinate axes centered at the origin
//
void drawAxes(float size, int wireframe){
  if(wireframe){     // wireframe drawing, so draw X, Y, Z axes as colored lines
    glBegin(GL_LINES);
      glColor3f(1, 0, 0);		  // x axis drawn in red
      glVertex3f(0, 0, 0);
      glVertex3f(size, 0, 0);
      
      glColor3f(0, 1, 0);		  // y axis drawn in green
      glVertex3f(0, 0, 0);
      glVertex3f(0, size, 0);
      
      glColor3f(0, 0, 1);		  // z axis drawn in blue
      glVertex3f(0, 0, 0);
      glVertex3f(0, 0, size);
    glEnd();
  }
  else{      // shaded drawing, so draw X, Y, Z axes as solid arrows
    const float height = size;
    
    // draw arrow shafts. they are all white
    glMaterialfv(GL_FRONT, GL_AMBIENT, GRAY);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, WHITE);
    // Z
    Cylinder.Draw(FALSE);
    // X
    glPushMatrix();
      glRotatef(90, 0, 1, 0);
      Cylinder.Draw(FALSE);
    glPopMatrix();
    // Y
    glPushMatrix();
      glRotatef(-90, 1, 0, 0);
      Cylinder.Draw(FALSE);
    glPopMatrix();
    
    // draw arrow heads. X is red, Y is green, and Z is blue
    // Z
    glPushMatrix();
      glMaterialfv(GL_FRONT, GL_AMBIENT, DIM_BLUE);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, BLUE);
      glTranslatef(0, 0, 0.8 * height);
      Cone.Draw(FALSE);
    glPopMatrix();
    // X
    glPushMatrix();
      glMaterialfv(GL_FRONT, GL_AMBIENT, DIM_RED);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, RED);
      glRotatef(90, 0, 1, 0);
      glTranslatef(0, 0, 0.8 * height);
      Cone.Draw(FALSE);
    glPopMatrix();
    // Y
    glPushMatrix();
      glMaterialfv(GL_FRONT, GL_AMBIENT, DIM_GREEN);
      glMaterialfv(GL_FRONT, GL_DIFFUSE, GREEN);
      glRotatef(-90, 1, 0, 0);
      glTranslatef(0, 0, 0.8 * height);
      Cone.Draw(FALSE);
    glPopMatrix();
  }
}

//
// routine to draw the current model
//
void drawModel(int wireframe){
  float ambient_color[4];
  float diffuse_color[4];
  float specular_color[4];
  int shininess;
  
  if(wireframe){
    glDisable(GL_TEXTURE_2D);
    
    // set drawing color to current hue, and draw with thicker wireframe lines
    glColor3f(white[0], white[1], white[2]);
    glLineWidth(2);
  }
  else{
    // set up material color to be white
    for(int i = 0; i < 3; i++){
      ambient_color[i] = AMBIENT_FRACTION * white[i];
      diffuse_color[i] = DIFFUSE_FRACTION * white[i];
      specular_color[i] = SPECULAR_FRACTION * white[i];
      shininess = 60;
    }
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_color);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_color);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_color);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    
    if(TextureMode != NOTEXTURE){
      glEnable(GL_TEXTURE_2D);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, ColorMode);
      glBindTexture(GL_TEXTURE_2D, TextureID);	    // set the active texture
      switch(TextureMode){
	case NEARTEXTURE:
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	  break;
	case LINEARTEXTURE:
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	  break;
	case MIPMAPTEXTURE:
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	  break;
      }
    }
  }
  
   glPushMatrix();
	double s_t = 1.0/100*30*sqrt(3.0);
	glScalef(1.0/s_t,1.0/s_t,1.0/s_t);
	glRotatef(-45,0,1,0);
	glRotatef(-atan(sqrt(2.0)/2.0)*180/3.1415926,-1,0,1);
	draw_desk();
        draw_wall();
	draw_obj();			     			
   glPopMatrix();
  
  glDisable(GL_TEXTURE_2D);
}

//
// Display callback
//
void doDisplay(){
  // distant light source, parallel rays coming from front upper right
  const float light_position[] = {30, 20, 10, 0};
  
  // clear the window to the background color
  glClear(GL_COLOR_BUFFER_BIT);
  
  if(!Wireframe)
    glClear(GL_DEPTH_BUFFER_BIT);  // solid - clear depth buffer
  
  if(!Wireframe && SmoothShading)  // establish shading model, flat or smooth
    glShadeModel(GL_SMOOTH);
  else
    glShadeModel(GL_FLAT);
  
  // light is positioned in camera space so it does not move with object
  glLoadIdentity();
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_AMBIENT, WHITE);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, WHITE);
  glLightfv(GL_LIGHT0, GL_SPECULAR, WHITE);
  glEnable(GL_LIGHT0);
  // establish camera coordinates
  
  camara_matrix.identity();  
  camara_matrix = xy_rotateX(camara_matrix,Tilt/180*3.1415926);
//  camara_matrix = xy_rotateY(camara_matrix,Pan/180*3.1415926);
// camara_matrix = xy_translate(camara_matrix, Vector3d(0,0,Approach));
  camara_matrix = xy_rotateY(camara_matrix,ThetaY/180*3.1415926);
  camara_matrix = xy_rotateX(camara_matrix,ThetaX/180*3.1415926);

  glRotatef(Tilt, 1, 0, 0);	    // tilt - rotate camera about x axis
  glRotatef(Pan, 0, 1, 0);	    // pan - rotate camera about y axis
  glTranslatef(0, 0, Approach);     // approach - translate camera along z axis
  
  // rotate the model
  glRotatef(ThetaY, 0, 1, 0);       // rotate model about x axis
  glRotatef(ThetaX, 1, 0, 0);       // rotate model about y axis
  
  // draw the model in wireframe or solid
  drawModel( Wireframe);
  
  // if axes are required, draw them
  if(Axes)
    drawAxes(0.8 * CUBEWIDTH, Wireframe);
  
  glutSwapBuffers();
}

//
// Keyboard callback routine. 
// Set various modes or take actions based on key presses
//
void handleKey(unsigned char key, int x, int y){
  
  switch(key){
    case 'r':			// T -- cycle through texture modes
    case 'R':
      raytracer_main(camara_matrix);
      break;

    case 'a':			// A -- toggle between drawing axes or not
    case 'A':
      Axes = !Axes;
      glutPostRedisplay();
      break;
      
    case 'p':			// P -- toggle between ortho and perspective
    case 'P':
      Projection = !Projection;
      updateProjection();
      glutPostRedisplay();
      break;
      
      
    case 'i':			// I -- reinitialize 
    case 'I':
      setInitialState();
      updateProjection();
      glutPostRedisplay();
      break;
      
    case 'm':			// M -- cycle through color modes
    case 'M':
      ColorMode = (ColorMode == GL_MODULATE? GL_DECAL: GL_MODULATE);
      glutPostRedisplay();
      break;
      
    case 't':			// T -- cycle through texture modes
    case 'T':
      TextureMode = (TextureMode == MIPMAPTEXTURE? NOTEXTURE: TextureMode + 1);
      glutPostRedisplay();
      break;
    
    case 'q':			// Q or Esc -- exit program
    case 'Q':
    case ESC:
      exit(0);
      
    case 's':			// S -- toggle between flat and smooth shading
    case 'S':
      SmoothShading = !SmoothShading;
      glutPostRedisplay();
      break;
      
    case 'w':			// M -- toggle between wireframe and shaded viewing
    case 'W':
      Wireframe = !Wireframe;
      if(Wireframe){
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
      }
      else{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
      }
      glutPostRedisplay();
      break;
  }
}

//
// Mouse Button Callback
// on button press, record mouse position and which button is pressed
//
void handleButtons(int button, int state, int x, int y){
  
  if(state == GLUT_UP)
    Button = NONE;		// no button pressed
  else{
    MouseY = -y;		// invert y window coordinate to correspond with OpenGL
    MouseX = x;
    
    Button = button;		// store which button pressed
  }
}

//
// Mouse Motion Callback
// when mouse moves with a button down, update appropriate camera parameter
//
void handleMotion(int x, int y){
  int delta;
  
  y = -y;
  int dy = y - MouseY;
  int dx = x - MouseX;
  
  switch(Button){
    case GLUT_LEFT_BUTTON:
      ThetaX -= ROTFACTOR * dy;
      ThetaY += ROTFACTOR * dx;
      glutPostRedisplay();
      break;
    case GLUT_MIDDLE_BUTTON:
      Pan -= ROTFACTOR * dx;
      Tilt += ROTFACTOR * dy;
      glutPostRedisplay();
      break;
    case GLUT_RIGHT_BUTTON:
      delta = (fabs(dx) > fabs(dy)? dx: dy);
      Approach += XLATEFACTOR * delta;
      glutPostRedisplay();
      break;
  }
  
  MouseX = x;
  MouseY = y;
}

//
// Reshape Callback
// Keep viewport the entire screen
//
void doReshape(int width, int height){
  
  glViewport(0, 0, width, height);
  Width = width;
  Height = height;
  
  updateProjection();
}

//
// Initialize OpenGL to establish lighting and colors
// and initialize viewing and model parameters
//
void initialize(){
  
  // initialize modelview matrix to identity
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  // specify window clear (background) color to be black
  glClearColor(0, 0, 0, 0);
  
  // position light and turn it on
  glEnable(GL_LIGHT0);
  
  // initialize viewing and model parameters
  setInitialState();
  updateProjection();
  
  // construct the models that will be used for cube and for coordinate arrows
  Cylinder.BuildCylinder(0.8 * CUBEWIDTH / 20.0, 1.24 * CUBEWIDTH); // cylinder for axes
  Cone.BuildCone(0.8 * CUBEWIDTH / 10.0, 0.16 * CUBEWIDTH); // cone for axes
  
  //
  // create the texture map image and assign a texture ID
  //
  //makeImage(TEXTUREWIDTH, TEXTUREHEIGHT, &TextureImage);   // create texture pixmap
  glGenTextures(1, &TextureID);		  // get OpenGL ID for this texture
  // make this texture the active texture
  glBindTexture(GL_TEXTURE_2D, TextureID);
  // set texture drawing parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // build mipmap in texture memory
  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, texture_img_w, texture_img_h, GL_RGBA, GL_UNSIGNED_BYTE, teapot_texture);

  glGenTextures(2, &TextureID);		  // get OpenGL ID for this texture
  // make this texture the active texture
  glBindTexture(GL_TEXTURE_2D, TextureID);
  // set texture drawing parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // build mipmap in texture memory
  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, texture1_img_w, texture1_img_h, GL_RGBA, GL_UNSIGNED_BYTE, desk_texture);

  glGenTextures(3, &TextureID);		  // get OpenGL ID for this texture
  // make this texture the active texture
  glBindTexture(GL_TEXTURE_2D, TextureID);
  // set texture drawing parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // build mipmap in texture memory
  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, texture2_img_w, texture2_img_h, GL_RGBA, GL_UNSIGNED_BYTE, wall_texture);
  

  // This is texture map sent to texture memory without mipmapping:
  //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXTUREWIDTH, TEXTUREHEIGHT,
  //	       0, GL_RGBA, GL_UNSIGNED_BYTE, TextureImage);  
}

//
// Main program to create window, setup callbacks, and initiate GLUT
//
int main(int argc, char* argv[]){
  
  // start up the glut utilities
  glutInit(&argc, argv);
  OBJFile objfile;
  get_commandline("teapot.obj", objfile);
  load_scene(&obj_scene, objfile);
  if(teapot_texture == NULL)teapot_texture = readimage("aa.jpg", texture_img_w, texture_img_h);
  if(desk_texture == NULL)desk_texture = readimage("tile.jpeg", texture1_img_w, texture1_img_h);
  if(wall_texture == NULL)wall_texture = readimage("floor.jpg", texture2_img_w, texture2_img_h);

  // create the graphics window, giving width, height, and title text
  // and establish double buffering, RGBA color
  // Depth buffering must be available for drawing the shaded model
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(WIDTH, HEIGHT);
  glutCreateWindow("3D Texture Viewer with Mipmaps");
  camara_matrix.identity();
  // register callback to draw graphics when window needs updating
  glutDisplayFunc(doDisplay);
  glutReshapeFunc(doReshape);
  glutKeyboardFunc(handleKey);
  glutMouseFunc(handleButtons);
  glutMotionFunc(handleMotion);
  
  initialize();
  
  glutMainLoop();
	
  if(teapot_texture != NULL)delete[] teapot_texture;
  if(desk_texture != NULL)delete[] desk_texture;
  if(wall_texture != NULL)delete[] wall_texture;
}
