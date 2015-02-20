/*
   Sample OpenGL/GLUT Program to simulate the ray tracer
  

   Display window saved to a file with 'w' keypress.
   Redo the render using 'r' keypress.
   Change the scene.txt to change the parameters.
   Program quits when either 'q' or ESC are pressed.

   CP SC 405/605            Yuan Xiong         3/2/2014

   usage: raytrace [l|v] [pixwidth] scene.txt [image-filename.ext]
*/
#include"objtrace.h"






//////////////////////////////
//my variables
Matrix4x4 camara_out;
static int icolor = 0;
unsigned char *img = 0;
unsigned char *depth = 0; // depth field
int img_w=DEFAULT_WIDTH,img_h=DEFAULT_HEIGHT;
int wnd_w,wnd_h;
double xfactor = 1.0;
double yfactor = 1.0;
int projection_mode = 0;
string img_filename = "";
string scene_filename = "";
//the viewport and canvas
Vector3d camara(0,0,0.5);
Vector3d screen_center(0,0,0);
Vector3d view_direction(0,0,-1);
double focal = 0.5;
Vector3d up_direction(0,1,0);
double canvas_width = 0.5;
double canvas_height = 0.4;
double screen_ratio = 1.25;
double object_refraction_index = 0;
int object_smooth = 1;
// functions
Vector3d colorInRange(Vector3d color);
void handleReshape(int w, int h);
int initial();
void render();
int read_scene();
int nrays = 1;
//the objects
xy_environment models;
 PolySurf *scene = NULL;	      // polygonal surface data structure
bool use_raytracer = true; // use raytracer or OpenGL render
string texture_file="brazil-flag.png";
unsigned char *texture = NULL;
int t_img_w = 0;
int t_img_h = 0;

unsigned char *texture1 = NULL;
int t1_img_w = 0;
int t1_img_h = 0;

unsigned char *texture2 = NULL;
int t2_img_w = 0;
int t2_img_h = 0;

unsigned char *texture3 = NULL;
int t3_img_w = 0;
int t3_img_h = 0;

unsigned char *texture4 = NULL;
int t4_img_w = 0;
int t4_img_h = 0;

void DepthFieldBlur(unsigned char *img, unsigned char *depth, int img_w, int img_h);

Vector3d transform(Vector3d x)
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

Vector3d normal_transform(Vector3d x)
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

Vector3d colorInRange(Vector3d color)
{
	if(color.x>1)color.x = 1;
	if(color.x<0)color.x = 0;
	if(color.y>1)color.y = 1;
	if(color.y<0)color.y = 0;
	if(color.z>1)color.z = 1;
	if(color.z<0)color.z = 0;
	return color;
}

/*
   Display Callback Routine: clear the screen and draw the image
   This routine is called every time the window on the screen needs
   to be redrawn, like if the window is iconized and then reopened
   by the user, and when the window is first created. It is also
   called whenever the program calls glutPostRedisplay()
*/

void my_drawImage(){
 if(use_raytracer)
 {
 	// update the window size
	  wnd_w = glutGet(GLUT_WINDOW_WIDTH);
	  wnd_h = glutGet(GLUT_WINDOW_HEIGHT);
	  glClearColor(0,0,0,1);
	   glPixelZoom(xfactor, yfactor);
	 
	  glRasterPos2i(wnd_w/2 - xfactor * img_w/2, wnd_h/2 - yfactor * img_h/2); // post the image in the center of the screen
	  
	  glDrawPixels(img_w, img_h, GL_RGBA, GL_UNSIGNED_BYTE, img);
	  glutSwapBuffers();

	  // flush the OpenGL pipeline to the viewport
	  glFlush();
 }
 else
 {
   	
 }
}

/*
  Keyboard Callback Routine: 'c' cycle through colors, 'q' or ESC quit
  This routine is called every time a key is pressed on the keyboard
*/
void handleKey2(unsigned char key, int x, int y){
  
  switch(key){
      
    case 'r':
    case 'R':
      use_raytracer = !use_raytracer;
      render();
      glutPostRedisplay();
      break;

    case 'w':
    case 'W':
      glutPostRedisplay();
      if(img_filename != "")writeimage(img_filename);
      else writeimage();
      break;
   
  
    case 'p':
    case 'P':
      projection_mode = 1 - projection_mode;
      render();
      glutPostRedisplay();
      break;
    case 'i':
    case 'I':
      initial();
      use_raytracer = false;
      //render();
      glutPostRedisplay();
      break;
    case 'q':		// q - quit
    case 'Q':
    case 27:		// esc - quit
      exit(0);
      
    default:		// not a valid key -- just ignore it
      return;
  }
}
//special keys
void SpecialInput(int key, int x, int y)
{
switch(key)
{
    case GLUT_KEY_LEFT:
     
      break;
    case GLUT_KEY_RIGHT:
     
      break;
    case GLUT_KEY_UP:
     
      break;
    case GLUT_KEY_DOWN:
     
      //glutPostRedisplay();
      break;
}

glutPostRedisplay();
} 

/*
 Reshape Callback Routine: sets up the viewport and drawing coordinates
 This routine is called when the window is created and every time the window 
 is resized, by the program or by the user
*/
void handleReshape(int w, int h){
  // make the viewport the entire window
  glViewport(0, 0, w, h);
  wnd_w = w;
  wnd_h = h;
  // define the drawing coordinate system on the viewport
  // to be measured in pixels
  if(w < img_w){xfactor = 1.0 * w / img_w;}
  else xfactor = 1;
  if(h < img_h){yfactor = 1.0 * h / img_h;}
  else yfactor = 1;
  if(xfactor < yfactor)yfactor = xfactor;
  else xfactor = yfactor;
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, w, 0, h);//left,right,bottom,top
  glMatrixMode(GL_MODELVIEW);
}


int initial()
{
  xfactor = 1;
  yfactor = 1;
  // the model list
  int re = 0;
  if(models.objects.size() == 0)re = read_scene();
  return re;
}


/*
   Main program to draw the square, change colors, and wait for quit
*/
int raytracer_main(Matrix4x4 camara_m){

  
  // start up the glut utilities
 // glutInit(&argc, argv);
 // delete argv[0];
 // delete argv;
 
        camara_out = camara_m;
	projection_mode = 0;
	img_w = DEFAULT_WIDTH;
        img_h = DEFAULT_HEIGHT;
        scene_filename = "";
        string obj_filename = "teapot.obj";
  
 
  // read the objfile using Dr. House's objload
  OBJFile objfile;
  Vector3d centroid, bboxmin, bboxmax;  // center and bounding box

  // check the command line first, we may abort
  get_commandline(obj_filename.c_str(), objfile);

  // load scene and print the scene
  load_scene(&scene, objfile);
 // cout << *scene << endl;
  
  // get and print the centroid and bounding box
  centroid = scene->Centroid();
  bboxmin = scene->MinBBox();
  bboxmax = scene->MaxBBox();
  cout << "Centroid = " << centroid << ", Bounding Box: (" << bboxmin << ", " << bboxmax << ")" << endl;  


  if(initial() < 0 )return -1;
  render();
  if(img_filename!="")writeimage(img_filename, img, img_w, img_h);
  // create the graphics window, giving width, height, and title text
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize(img_w, img_h);
  glutCreateWindow("Yuan Xiong ray tracer");
  

  // set up the callback routines to be called when glutMainLoop() detects
  // an event
  glutDisplayFunc(my_drawImage);	  // display callback
  glutKeyboardFunc(handleKey2);	  // keyboard callback
  glutReshapeFunc(handleReshape); // window resize callback
  glutSpecialFunc(SpecialInput); // special keyboard input
  // specify window clear (background) color to be opaque white
  glClearColor(0, 0, 0, 1);

  // Routine that loops forever looking for events. It calls the registered 
  // callback routine to handle each event that is detected
  glutMainLoop();
  if(img!=NULL){delete[] img; img = NULL;}
  if(depth!=NULL){delete[] depth; depth = NULL;}
  if(texture!=NULL){delete []texture; texture = NULL;}
  if(texture1!=NULL){delete []texture1; texture1 = NULL;}
  if(texture2!=NULL){delete []texture2; texture2 = NULL;}
  if(texture3!=NULL){delete []texture3; texture3 = NULL;}
  if(texture4!=NULL){delete []texture4; texture4 = NULL;}
  return 0;
}

void render()
{	
    struct timeval ts, te;
    gettimeofday(&ts, NULL);
    if(!use_raytracer)return;
  
	double x,y,z;
	double du, dv; // the resolution
	Vector3d pixel_position;
	Vector3d ray_start, ray_end;
	Vector3d screen_x, screen_y, screen_z;
	//the screen 
        screen_center = camara + view_direction.normalize() * focal;
	screen_z = -1* view_direction.normalize(); // the direction vector where the camara is looking at
	screen_x = up_direction % screen_z;
	screen_x = screen_x.normalize();
        screen_y = screen_z % screen_x; // the y coordinate of the screen
        cout<<"Camara at "<<camara<<",  Looking at "<<screen_center<<endl;
        canvas_height = canvas_width / screen_ratio;
        cout<<"Canvas width "<<canvas_width<<",  Canvas height "<<canvas_height<<endl;
	du = 1.0 * canvas_width / (img_w);
	dv = 1.0 * canvas_height / (img_h);
	Vector3d screen_corner;//the up left corner of the screen
	screen_corner = screen_center - 0.5 * canvas_width * screen_x - 0.5 * canvas_height * screen_y;
	string msg="";
	char temp[256]={0};
	//the color
	Vector3d final_color(0,0,0), temp_color;
	int i,j;
	if(img == NULL) img = new unsigned char[img_w * img_h * 4];
	if(depth == NULL) depth = new unsigned char[img_w * img_h];
	if(projection_mode == 0)ray_start = camara;
	else ray_start = camara + 1000*screen_z;
	double weight = 1.0 / nrays;
	for(j = 0; j < img_h; j++)
	{   for(i = 0; i < img_w; i++)
	{
		msg = string(temp);
		for(int k = 0; k< strlen(temp); k++)cout<<'\b';
		sprintf(temp,"%2.2f %% finished. ", 1.0*(i+j*img_w)/img_w/img_h*100);
		cout<<temp;
		final_color = Vector3d(0,0,0);
		
                //middel
		pixel_position = screen_corner + (0.5+i)*du*screen_x + (0.5+j)*dv*screen_y;
		ray_end = pixel_position;
		//cout<<ray_start;
		//cout<<ray_end;
		 	double pixel_depth = -1;
			temp_color = RecursiveShade(NULL, ray_start, ray_end, models, MAX_SHADE_LEVEL, 1,pixel_depth);
			if(pixel_depth < 0)pixel_depth = 0;
			if(pixel_depth > 100)pixel_depth = 100;
			depth[i+j*img_w] = (int)pixel_depth;
			final_color = final_color + weight * colorInRange(temp_color); 
		        for(int k = 1; k < nrays; k++)
			{
				pixel_position = screen_corner + (i+1.0*rand()/RAND_MAX)*du*screen_x + (j+1.0*rand()/RAND_MAX)*dv*screen_y;
				ray_end = pixel_position;
			
				temp_color = RecursiveShade(NULL, ray_start, ray_end, models, MAX_SHADE_LEVEL, 1,pixel_depth);
				
				final_color = final_color + weight * colorInRange(temp_color); 
			}
		

		
	
		int co;
		co = 255*final_color.x;
		if(co>255)co = 255;
		if(co<0)co = 0;
		img[(i+j*img_w)*4] = co;
		co = 255*final_color.y;
		if(co<0)co = 0;
		if(co>255)co = 255;
		img[(i+j*img_w)*4+1] = co;
		co = 255*final_color.z;
		if(co<0)co = 0;
		if(co>255)co = 255;
		img[(i+j*img_w)*4+2] = co;
		img[(i+j*img_w)*4+3] = 255;	
	}
        }
	double min_depth = 100000, max_depth = -1;
	for(i = 0; i < img_w*img_h; i++)
	{
		if(depth[i]>max_depth)max_depth = depth[i];
		if(depth[i]<min_depth)min_depth = depth[i];
	}
	for(i = 0; i < img_w*img_h; i++)
	{
		double t = (depth[i] - min_depth)/(max_depth-min_depth)*3;
		depth[i] = (pow(4,t)-1)/21;
	}	
		
        DepthFieldBlur(img, depth, img_w, img_h);
	gettimeofday(&te, NULL);
	cout<<"Rander time:"<<(te.tv_sec - ts.tv_sec)<<" s";
	cout<<endl;
	
}

int read_scene()
{
	  xy_object *temp;
  	  xy_light *light;
          int id = -1;
          if(models.objects.size()>0)return 1;

	 char buf[256];
           string tag_name;  
      		 ifstream in("scene.txt");  
       		   if (! in.is_open()) { cout << "Error opening scene file"; return -1;} 
	   double t;
	   int i;
	   //loading camara and canvas setting
		in.getline(buf,256);
           	sscanf(buf,"viewport: %lf %lf %lf", &(camara.x), &(camara.y),&(camara.z));
		Vector4d camara4(camara[0],camara[1],camara[2],1);
		
		camara4 = camara_out * camara4;
		camara[0] = camara4[0];
		camara[1] = camara4[1];
		camara[2] = camara4[2];
	 	in.getline(buf,256);
           	sscanf(buf,"viewdirection: %lf %lf %lf", &(view_direction.x),&(view_direction.y),&(view_direction.z));
		in.getline(buf,256);
           	sscanf(buf,"updirection: %lf %lf %lf", &(up_direction.x),&(up_direction.y),&(up_direction.z));
		in.getline(buf,256);
           	sscanf(buf,"focal length: %lf", &focal);
		in.getline(buf,256);
           	sscanf(buf,"screen ratio: %lf", &screen_ratio);
		in.getline(buf,256);
           	sscanf(buf,"screen width: %lf", &canvas_width);
                in.getline(buf,256);
           	sscanf(buf,"window width: %d", &img_w);
                img_h = img_w / screen_ratio;
		in.getline(buf,256);
           	sscanf(buf,"object refraction index: %lf", &object_refraction_index);
		in.getline(buf,256);
           	sscanf(buf,"object smooth: %d", &object_smooth);
		in.getline(buf,256);
		char temp_str[256];
		sscanf(buf,"texture file: %s", temp_str);
		texture_file = string(temp_str);
	  	cout<<"texture file:"<<texture_file<<endl;
		in.getline(buf,256);
           	sscanf(buf,"nrays: %d", &nrays);
        //create obj from objfile
        //read the texture image
	if(texture == NULL)texture = readimage(texture_file, t_img_w, t_img_h);
	if(scene !=NULL)
	{
		models.root.box = Box( Vector3d(-100,-100,-100), Vector3d(100,100,100));
		Vector3d pp1,pp2,pp3;
		Vector2d UV1,UV2,UV3;
		Vector3d Vn1,Vn2,Vn3;
		Color a_color;
		Color d_color;
		Color s_color;
		double alpha;
		int illu_mode;
		double exp;
		int txt_true = -1;
		int smooth = -1;
		int index;
		int fj;
		for(int fi = 0; fi < scene->nfaces; fi++)
		{
		
			for(fj = 1; fj <= scene->faces[fi].nverts-2; fj++)
			{
				//cout<<fi<<","<<fj<<endl;
				
				index = scene->faces[fi].verts[0][0];
				pp1 = scene->verts[index];
				smooth = index = scene->faces[fi].verts[0][1];
				if(smooth>=0)Vn1 = scene->norms[index];
                                txt_true = index = scene->faces[fi].verts[0][2];
				if(index>=0)UV1 = scene->uvs[index];

				index = scene->faces[fi].verts[fj][0];
				pp2 = scene->verts[index];
				smooth = index = scene->faces[fi].verts[fj][1];
				if(smooth>=0)Vn2 = scene->norms[index];
				txt_true = index = scene->faces[fi].verts[fj][2];
				if(index>=0)UV2 = scene->uvs[index];

				index = scene->faces[fi].verts[fj+1][0];
				pp3 = scene->verts[index];
				smooth = index = scene->faces[fi].verts[fj+1][1];
				if(smooth>=0)Vn3 = scene->norms[index];
				txt_true = index = scene->faces[fi].verts[fj+1][2];
				if(index>=0)UV3 = scene->uvs[index];
	
				
				int nmaterial = scene->faces[fi].material;
				if(nmaterial>=0)
				{
					a_color = scene->materials[nmaterial].a;
					d_color = scene->materials[nmaterial].d;
					s_color = scene->materials[nmaterial].s;
					alpha = scene->materials[nmaterial].alpha;
					illu_mode = scene->materials[nmaterial].illum_model;
					exp = scene->materials[nmaterial].exp;
				}
				else
				{
					a_color = Color(0.0,0.0,0.0,0);
					d_color = Color(0.05,0.05,0.05,1);
					s_color = Color(0.7,0.6,0.5,1);
					alpha = 1;
					illu_mode = 1;
					exp = 100;
				}
				//for the caustic teapot
	
				temp = new xy_triangle(
					pp1,pp2,pp3, 
					++id, Vector3d(1,1,1), Vector4d(0.1,1,2,20));
				if(txt_true>=0)
				{
					temp->texture = texture;
					temp->img_w = t_img_w;
					temp->img_h = t_img_h;
					temp->uv1 = UV1;
					temp->uv2 = UV2;
					temp->uv3 = UV3;
				}
				if(smooth>=0 && object_smooth > 0)
				{
					temp->smooth_mode = 1;
					temp->vn1 = Vn1;
					temp->vn2 = Vn2;
					temp->vn3 = Vn3;
				}
				temp->refraction_index = object_refraction_index;
				temp->set_ambient_color(a_color);
				temp->set_specular_color(s_color);
				temp->set_diffuse_color(d_color);
				temp->set_alpha(alpha);
				temp->set_specular_power(exp);
				temp->illum_mode = illu_mode;
				models.objects.push_back(temp);
				models.root.objs.push_back(id);
				

				//for the textured teapot
				pp1 = transform(pp1);
				pp2 = transform(pp2);
				pp3 = transform(pp3);
				Vn1 = normal_transform(Vn1);
				Vn2 = normal_transform(Vn2);
				Vn3 = normal_transform(Vn3);
	
				temp = new xy_triangle(pp1, pp2, pp3, ++id, Vector3d(1,1,1), Vector4d(0.1,1,0.8,20));
				if(txt_true>=0)
				{
					temp->texture = texture;
					temp->img_w = t_img_w;
					temp->img_h = t_img_h;
					temp->uv1 = UV1;
					temp->uv2 = UV2;
					temp->uv3 = UV3;
				}
				if(smooth>=0 && object_smooth > 0)
				{
					temp->smooth_mode = 1;
					temp->vn1 = Vn1;
					temp->vn2 = Vn2;
					temp->vn3 = Vn3;
				}
				temp->refraction_index = 0;
				temp->set_ambient_color(a_color);
				temp->set_specular_color(s_color);
				temp->set_diffuse_color(d_color);
				temp->set_alpha(alpha);
				temp->set_specular_power(exp);
				temp->illum_mode = illu_mode;
				models.objects.push_back(temp);
				models.root.objs.push_back(id);
			}
		}
	}
         
   	models.cutTree(&(models.root));
          
        if(1)
	{
	  
	   in.getline(buf,256);
	   tag_name = string(buf);
	   while(buf[0]!='@') {in.getline(buf,256);} // the first part, objects, begin with @
	   if(in.eof()){in.close();return 0;}
	   in.getline(buf,256);
	   while(buf[0]!='#' && buf[0] != '@' && !in.eof()) {in.getline(buf,256);}

	   if(buf[0]=='@'||in.eof()){in.close();return 0;} // failed to load in objects
	   
	   while(!in.eof() && buf[0] == '#') // #, an object
	   {
		int visible;
		int nid;
		sscanf(buf,"#%d", &nid);
		in.getline(buf,256);
           	sscanf(buf,"visible: %d", &visible);
		if(visible == 0) //ignore this object
		{
			while(!in.eof() && buf[0] != '#' && buf[0] != '@')in.getline(buf,256);
			continue;
		}	
		int type;	
		in.getline(buf,256);
           	sscanf(buf,"type: %d", &type);
                double index;
		switch(type)
		{
			case 1://sphere
			{	
			   cout<<"New object: sphere."<<endl;
			   Vector3d position, color;
			   double radius;
			   Vector4d factors;
			   in.getline(buf,256);
           		   sscanf(buf,"position: %lf %lf %lf", &(position.x), &(position.y),&(position.z));
			   in.getline(buf,256);
           		   sscanf(buf,"radius: %lf", &radius);
			   in.getline(buf,256);
           		   sscanf(buf,"color: %lf %lf %lf", &(color.x), &(color.y),&(color.z));
			   in.getline(buf,256);
           		   sscanf(buf,"ambient: %lf", &(factors.x));
			   in.getline(buf,256);
           		   sscanf(buf,"diffuse: %lf", &(factors.y));
			   in.getline(buf,256);
           		   sscanf(buf,"specular: %lf", &(factors.z));
			   in.getline(buf,256);
           		   sscanf(buf,"specular exponent: %lf", &(factors.w));
			   in.getline(buf,256);
           		   sscanf(buf,"refraction index: %lf", &index);
			   temp = new xy_sphere(position, radius, color, ++id, factors);//red shpere
			   temp->refraction_index = index;
	  		   models.objects.push_back(temp);
			   models.root.objs.push_back(id);
			   while(!in.eof() && buf[0] != '#'&& buf[0] != '@')in.getline(buf,256);
			   
			}break;
			case 2: // plane with texture and bump mapping
			{
			   cout<<"New object: plane."<<endl;
			   Vector3d normal, color;
			   double distance;
			   Vector4d factors;
			   in.getline(buf,256);
           		   sscanf(buf,"normal: %lf %lf %lf", &(normal.x), &(normal.y),&(normal.z));
			   in.getline(buf,256);
           		   sscanf(buf,"distance to the origin: %lf", &distance);
			   in.getline(buf,256);
           		   sscanf(buf,"color: %lf %lf %lf", &(color.x), &(color.y),&(color.z));
			   in.getline(buf,256);
           		   sscanf(buf,"ambient: %lf", &(factors.x));
			   in.getline(buf,256);
           		   sscanf(buf,"diffuse: %lf", &(factors.y));
			   in.getline(buf,256);
           		   sscanf(buf,"specular: %lf", &(factors.z));
			   in.getline(buf,256);
           		   sscanf(buf,"specular exponent: %lf", &(factors.w));
			   in.getline(buf,256);
           		   sscanf(buf,"refraction index: %lf", &index);
			   temp = new xy_plane(normal, distance, ++id, color, factors);//red shpere
			   temp->refraction_index = index;
			   //set the texture for the plane
				char temp_str[256];
				in.getline(buf,256);
				sscanf(buf,"texture file: %s", temp_str);
				string file_name = string(temp_str);
			  	cout<<"texture file:"<<file_name<<endl;
			   switch(nid)
			   {
				case 1://desk
				{
					if(texture1 == NULL)texture1 = readimage(file_name, t1_img_w, t1_img_h);
					temp->texture = texture1;
					temp->img_w = t1_img_w;
					temp->img_h = t1_img_h;
					temp->tile_mode = 2;
					temp->bump_mode = 1;
					temp->bump_depth = 2.0;
					Matrix4x4 trans;
					trans.identity();
					
					trans = xy_scale(trans,Vector3d(1.0/10,1.0/10,0)); // inverse
					trans = xy_translate(trans,Vector3d(0.5,0.5,0)); // inverse
					temp->transform_matrix = trans;
					
					
				}break;

				case 2://background
				{
					if(texture2 == NULL)texture2 = readimage(file_name, t2_img_w, t2_img_h);
					temp->texture = texture2;
					temp->img_w = t2_img_w;
					temp->img_h = t2_img_h;
					temp->tile_mode = 2;
					
					Matrix4x4 trans;
					trans.identity();
					trans = xy_rotateZ(trans,90/180.0*3.1415926); // inverse
					trans = xy_scale(trans,Vector3d(1.0/80,1.0/50,0)); // inverse
					trans = xy_translate(trans,Vector3d(0.5,0.2,0)); // inverse
					temp->transform_matrix = trans;
					
					
				}break;

				case 3://background
				{
					if(texture3 == NULL)texture3 = readimage(file_name, t3_img_w, t3_img_h);
					temp->texture = texture3;
					temp->img_w = t3_img_w;
					temp->img_h = t3_img_h;
					temp->tile_mode = 2;
					
					Matrix4x4 trans;
					trans.identity();
					trans = xy_rotateZ(trans,90/180.0*3.1415926); // inverse
					trans = xy_scale(trans,Vector3d(1.0/120,1.0/120,0)); // inverse
					trans = xy_translate(trans,Vector3d(0.5,0.5,0)); // inverse
					temp->transform_matrix = trans;
					
					
				}break;
				
				
				default:break;
			   }

	  		   models.objects.push_back(temp);
			   models.root.objs.push_back(id);
			   while(!in.eof() && buf[0] != '#'&& buf[0] != '@')in.getline(buf,256);
			}break;
			case 3:
			{
			   cout<<"New object: box."<<endl;
			   Vector3d corner1,corner2, color;
			   Vector4d factors;
			   in.getline(buf,256);
           		   sscanf(buf,"corner: %lf %lf %lf", &(corner1.x), &(corner1.y),&(corner1.z));
			   in.getline(buf,256);
           		   sscanf(buf,"corner: %lf %lf %lf", &(corner2.x), &(corner2.y),&(corner2.z));
			   in.getline(buf,256);
           		   sscanf(buf,"color: %lf %lf %lf", &(color.x), &(color.y),&(color.z));
			   in.getline(buf,256);
           		   sscanf(buf,"ambient: %lf", &(factors.x));
			   in.getline(buf,256);
           		   sscanf(buf,"diffuse: %lf", &(factors.y));
			   in.getline(buf,256);
           		   sscanf(buf,"specular: %lf", &(factors.z));
			   in.getline(buf,256);
           		   sscanf(buf,"specular exponent: %lf", &(factors.w));
			   in.getline(buf,256);
           		   sscanf(buf,"refraction index: %lf", &index);
			   temp = new xy_box(corner1,corner2, ++id, color, factors);//red shpere
	  		   temp->refraction_index = index;
			   models.objects.push_back(temp);
			   models.root.objs.push_back(id);
			   while(!in.eof() && buf[0] != '#'&& buf[0] != '@')in.getline(buf,256);
			}break;
			default: {cout<<"Error, unknown object type in scene file."<<endl;in.close();return -1;}
		}//switch ends		
	   }//while ends

	   // the second part, lights, begin with @
	   if(in.eof()){in.close();return 0;}
	   in.getline(buf,256);
	   while(buf[0]!='#' && !in.eof()) {in.getline(buf,256);}

	   if(in.eof()){in.close();return 0;} // failed to load in lights
	   
	   while(!in.eof() && buf[0] == '#') // #, a light
	   {
		int visible;
		in.getline(buf,256);
           	sscanf(buf,"visible: %d", &visible);
		if(visible == 0) //ignore this object
		{
			while(!in.eof() && buf[0] != '#')in.getline(buf,256);
			continue;
		}	
		int type;	
		in.getline(buf,256);
           	sscanf(buf,"type: %d", &type);
		switch(type)
		{
			case 1://point light
			{	
			   cout<<"New light: point light."<<endl;
			   Vector3d position, color;
			   double illumination;
			   int decay;
			   Vector4d factors;
			   in.getline(buf,256);
           		   sscanf(buf,"position: %lf %lf %lf", &(position.x), &(position.y),&(position.z));
			   in.getline(buf,256);
           		   sscanf(buf,"color: %lf %lf %lf", &(color.x), &(color.y),&(color.z));
			   in.getline(buf,256);
           		   sscanf(buf,"illumination: %lf", &illumination);
			   in.getline(buf,256);
           		   sscanf(buf,"decay mode: %d", &decay);
			  
			   light = new xy_light(position, illumination*color, decay); // the yellow light 
	  		   models.lights.push_back(light);
			   while(!in.eof() && buf[0] != '#'&& buf[0] != '@')in.getline(buf,256);
			   
			}break;
			case 2://spot light
			{	
			   cout<<"New light: spot light."<<endl;
			   Vector3d position, color;
			   Vector3d looking;
  			   double angle;
			   int exponent;
			   double illumination;
			   int decay;
			   Vector4d factors;
			   in.getline(buf,256);
           		   sscanf(buf,"position: %lf %lf %lf", &(position.x), &(position.y),&(position.z));
			   in.getline(buf,256);
           		   sscanf(buf,"color: %lf %lf %lf", &(color.x), &(color.y),&(color.z));
			   in.getline(buf,256);
           		   sscanf(buf,"illumination: %lf", &illumination);
			   in.getline(buf,256);
           		   sscanf(buf,"decay mode: %d", &decay);
			   in.getline(buf,256);
           		   sscanf(buf,"looking at: %lf %lf %lf", &(looking.x), &(looking.y),&(looking.z));
			   in.getline(buf,256);
           		   sscanf(buf,"angle: %lf", &angle);
			   in.getline(buf,256);
           		   sscanf(buf,"exponent: %d", &exponent);
			   light = new xy_spotLight(position, illumination*color, decay, looking, angle, exponent); // the yellow light 
	  		   models.lights.push_back(light);
			   while(!in.eof() && buf[0] != '#')in.getline(buf,256);
			   
			}break;
		
			case 3://area light
			{	
			   cout<<"New light: area light."<<endl;
			   Vector3d position, color;
			   Vector3d looking;
  			   double scale;
			   
			   double illumination;
			   int decay;
			   Vector4d factors;
			   in.getline(buf,256);
           		   sscanf(buf,"position: %lf %lf %lf", &(position.x), &(position.y),&(position.z));
			   in.getline(buf,256);
           		   sscanf(buf,"color: %lf %lf %lf", &(color.x), &(color.y),&(color.z));
			   in.getline(buf,256);
           		   sscanf(buf,"illumination: %lf", &illumination);
			   in.getline(buf,256);
           		   sscanf(buf,"decay mode: %d", &decay);
			   in.getline(buf,256);
           		   sscanf(buf,"looking at: %lf %lf %lf", &(looking.x), &(looking.y),&(looking.z));
			   in.getline(buf,256);
           		   sscanf(buf,"scale: %lf", &scale);
			   
			   //set the texture for the light
				char temp_str[256];
				in.getline(buf,256);
				sscanf(buf,"texture file: %s", temp_str);
				string file_name = string(temp_str);
			  	cout<<"texture file:"<<file_name<<endl;
			   if(texture4 == NULL)texture4 = readimage(file_name, t4_img_w, t4_img_h);
			   
			   light = new xy_areaLight(position, illumination*color, decay, looking, Vector3d(0,1,0), t4_img_w, t4_img_h, scale, texture4); 
	  		   models.lights.push_back(light);
			   while(!in.eof() && buf[0] != '#')in.getline(buf,256);
			   
			}break;
			
			default: {cout<<"Error, unknown light type in scene file."<<endl;in.close();return -1;}
		}//switch ends		
	   }//while ends
	   in.close();
	 
	   return 1;		
	}
	
	return 0;

}

void DepthFieldBlur(unsigned char *img, unsigned char *depth, int img_w, int img_h)
{
	int pix_count = 0;
	int i,j,m,n,u,v, r,g,b,a;
	unsigned char *copy = new unsigned char[img_w*img_h*4];
	for(j = 0; j < img_h; j++)
	   for(i = 0; i < img_w; i++)
	{
		pix_count = 0;
		r = 0;g = 0;b = 0;a = 0;
		int ra = depth[(i+j*img_w)];
		if(ra<=0)
		{
			copy[(i+j*img_w)*4] = img[(i+j*img_w)*4];
			copy[(i+j*img_w)*4+1] = img[(i+j*img_w)*4];
			copy[(i+j*img_w)*4+2] = img[(i+j*img_w)*4];
			copy[(i+j*img_w)*4+3] = img[(i+j*img_w)*4];
		}
		for(n = -ra; n <= ra; n++)
	   		for(m = -ra; m <= ra; m++)
		{
			u = i + m;
			v = j + n;
			if(u<0||u>=img_w||v<0||v>=img_h)continue;
			pix_count++;
			r += img[(u+v*img_w)*4];
			g += img[(u+v*img_w)*4+1];
			b += img[(u+v*img_w)*4+2];
			a += img[(u+v*img_w)*4+3];			
		}
		r/=pix_count;
		if(r>255)r = 255;
		if(r<0)r = 0;
		g/=pix_count;
		if(g>255)g = 255;
		if(g<0)g = 0;
		b/=pix_count;
		if(b>255)b = 255;
		if(b<0)b = 0;
		a/=pix_count;
		if(a>255)a = 255;
		if(a<0)a = 0;
		copy[(i+j*img_w)*4] = (unsigned char)r;
		copy[(i+j*img_w)*4+1] = (unsigned char)g;
		copy[(i+j*img_w)*4+2] = (unsigned char)b;
		copy[(i+j*img_w)*4+3] = (unsigned char)a;
	}	
	for(i = 0; i<img_w*img_h*4;i++)img[i]=copy[i];
	delete copy;



}

