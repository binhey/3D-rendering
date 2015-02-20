/********************************************************************

	xy_object.h	Header File

	Objects, Image processing Methods, and Procedures.
        'xy' stands for the name 'Yuan Xiong'
		Yuan Xiong  Jan 29, 2014
		School of Computing.

	Copyright (C) - Yuan Xiong. 2014

*********************************************************************/

#ifndef _H_xy_object
#define _H_xy_object
#include <Magick++.h>
#include "Utility.h"
#include "Vector.h"
#include "PolySurf.h"
#include <Magick++.h>
#include "BIH.h"
#include <cstdlib>
#include <iostream>
#include <vector>
#include "Matrix.h"
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif
using namespace std;
//using namespace Magick;
#define SMALL_VALUE  0.000000001
#define HUGE_VALUE 100000000
#define QUITE_FAR    Vector3d(10000000, 10000000, 10000000)
#define FAR_FAR_AWAY Vector3d(100000000,100000000,100000000)
#define MAX_SHADE_LEVEL 8
#define MIN_ATTENUATION 0.01
#define TOO_CLOSE 0.0001
#define GLOBAL_ILLUMINATION_RAYS 10
#define GLOBAL_ILLUMINATION_DISTANCE 10
#define GLOBAL_ILLUMINATION_FACTOR 0.5



class xy_object;


//
class Collision
{
public:
      Collision():hit_position(FAR_FAR_AWAY),
	hit_normal(FAR_FAR_AWAY),
	hit_distance(HUGE_VALUE),
	pobj(NULL)
      {};
      ~Collision(){}
      Vector3d hit_position;
      Vector3d hit_normal;
      double hit_distance;
      xy_object *pobj;
      int unused_int[4];
      double unused_double[4];
      Vector3d unused_Vector3d[4];
};

/*
  Function to flip an array in y coordinate to make it upside down
*/
void image_flip(unsigned char *pixmap, int w, int h);
/*
  Routine to write the current framebuffer to an image file
*/
void writeimage();//using file name got from std::cin
void writeimage(string outfilename);
void writeimage(string outfilename, unsigned char *img, int img_w, int img_h);

unsigned char* readimage(string img_filename, int &img_w, int &img_h);
Vector3d jitter(double r);
//intersection functions
Vector3d sphereIntersection(Vector3d ray_start, Vector3d ray_end, Vector3d &normal, Vector3d center, double radius, Vector3d &close_i, Vector3d &far_i);
Vector3d planeIntersection(Vector3d ray_start, Vector3d ray_end, Vector3d &normal, Vector3d plane_normal, double plane_dis);
Vector3d triangleIntersection(Vector3d ori, Vector3d P, Vector3d p1, Vector3d p2, Vector3d p3);
Vector3d getUVW(Vector3d x, Vector3d pp1, Vector3d pp2, Vector3d pp3);
class xy_environment;
class xy_light;
double testShadow(Vector3d P, xy_environment &env, xy_light *p_light);
/*
  Class xy_object, the basic class describing an object.
*/
class xy_object
{
public:
  xy_object():obj_name("default object"),obj_id(0),obj_position(Vector3d(0,0,0)), obj_color(Vector3d(1,1,1))
  {
	ambient_factor = 0.1;
	diffuse_factor = 1.0;
	specular_factor = 1.0;
	specular_power = 100;	
	diffuse_color = Vector3d(1,1,1);
        specular_color = Vector3d(1,1,1);
	ambient_color = Vector3d(1,1,1);
	illum_mode = 0;
	alpha = 1;
	refraction_index = 0;
	texture = NULL;
	smooth_mode = 0;
	transform_matrix.identity();//no transformation
        tile_mode = 0;//no tile
	bump_mode = 0;//no bump
        bump_depth = 0;
	uv_up = Vector3d(0,0,1);
	
  }
  xy_object(Vector3d position, Vector3d color):obj_name("default object"),obj_id(0),obj_position(position), obj_color(color)
  {
	ambient_factor = 0.1;
	diffuse_factor = 1.0;
	specular_factor = 1.0;
	specular_power = 100;	
        diffuse_color = Vector3d(1,1,1);
        specular_color = Vector3d(1,1,1);
	ambient_color = Vector3d(1,1,1);
	refraction_index = 0;
	illum_mode = 0;
	alpha = 1;
	texture = NULL;
	smooth_mode = 0;
	transform_matrix.identity();//no transformation
        tile_mode = 0;//no tile
	bump_mode = 0;//no bump
        bump_depth = 0;
	uv_up = Vector3d(0,0,1);
  }  //convert constructor
  //using default copy constructor
  //using default assignment operator

  ~xy_object() {}//using default destructor
  
  virtual Vector3d getColor(){return obj_color;}//return object color without texture and shading
  virtual Vector3d getColor(Vector3d position){return obj_color;}//return object color with texture, no shading
  virtual Vector3d getShadeColor(Vector3d p, Vector3d b, xy_environment &env);//return object color within a shading environment
  virtual Vector3d getNormal(Vector3d b) = 0; // pure virtual function
  virtual Vector3d getCheckBoardColor(Vector3d point, Vector3d color1, Vector3d color2, double index)//return check board color
	{
		
		Vector3d result, temp_result;
		double t1, t2;
		int scale = 1 , div = 1;
		int r;

		t1 = floor(point.x/index)+floor(point.y/index)+floor(point.z/index);
		if(index < 1)r = (int)(1 / index);
		else r = (int)index;
		if((int)(t1)%r == 0)
		result =  color1;
		else result =  color2;
		/*
		//jitter
  		result = result * 0.2;
		for(int ji = 0; ji < 4; ji++)//jitter
		{
			if(((int)(floor(point.x+ jitter(0.1).x) + floor(point.y+jitter(0.1).y) + floor(point.z+jitter(0.1).z)))%index == 0)
				temp_result =  color1;
			else temp_result =  color2;
			result = result + 0.2 * temp_result;
		}*/
		return result;
	}
  virtual Collision findIntersection(Vector3d ray_start, Vector3d ray_end){return Collision();}
  virtual double get_ambient_factor(Vector3d pos){return ambient_factor;}
  virtual double get_diffuse_factor(Vector3d pos){return diffuse_factor;}
  virtual double get_specular_factor(Vector3d pos){return specular_factor;}
  virtual double get_specular_power(Vector3d pos){return specular_power;}
  virtual double get_alpha(Vector3d pos){return alpha;}
  virtual Vector3d get_diffuse_color(Vector3d pos){return diffuse_color;}
  virtual Vector3d get_specular_color(Vector3d pos){return specular_color;}
  virtual Vector3d get_ambient_color(Vector3d pos){return ambient_color;}
  
  virtual void set_ambient_factor(double x){ambient_factor = x;}
  virtual void set_diffuse_factor(double x){diffuse_factor = x;}
  virtual void set_specular_factor(double x){specular_factor = x;}
  virtual void set_specular_power(double x){specular_power = x;}
  virtual void set_alpha(double x){alpha = x;}
  virtual void set_diffuse_color(Vector3d x){diffuse_color = x;}
  virtual void set_specular_color(Vector3d x){specular_color = x;}
  virtual void set_ambient_color(Vector3d x){ambient_color = x;}
	
  virtual void set_diffuse_color(Color x){for(int i = 0; i < 3; i++)diffuse_color[i] = x[i];}
  virtual void set_specular_color(Color x){for(int i = 0; i < 3; i++)specular_color[i] = x[i];}
  virtual void set_ambient_color(Color x){for(int i = 0; i < 3; i++)ambient_color[i] = x[i];}
  virtual Vector3d getCenter(){return obj_position;}
  virtual Vector3d getMin(){return obj_position;}
  virtual Vector3d getMax(){return obj_position;}

  double refraction_index;
  string obj_name;
  int obj_id;
  Vector3d obj_position;
  Vector3d obj_color;
  int illum_mode;
  unsigned char *texture;
  Matrix4x4 transform_matrix;
  int tile_mode;
  int bump_mode;
  double bump_depth;
  Vector3d uv_up;
  int img_w;
  int img_h;
  Vector2d uv1,uv2,uv3;
  Vector3d vn1,vn2,vn3;
  int smooth_mode;
 protected:
  double ambient_factor;
  double diffuse_factor;
  double specular_factor;
  double specular_power;
  Vector3d ambient_color;
  Vector3d diffuse_color;
  Vector3d specular_color;
  
  double alpha;

  
};


//function object
class sortByX
{
  public:
  bool operator()(xy_object* lhs, xy_object* rhs) const {
    return lhs->getCenter().x < rhs->getCenter().x;
  }

};
class sortByY
{
  public:
  bool operator()(xy_object* lhs, xy_object* rhs) const {
    return lhs->getCenter().y < rhs->getCenter().y;
  }
};
class sortByZ
{
  public:
  bool operator()(xy_object* lhs, xy_object* rhs) const {
    return lhs->getCenter().z < rhs->getCenter().z;
  }

};



//default light is a point light
class xy_light 
{
public:
  xy_light():light_name("default light"),light_id(0),light_position(Vector3d(0,0,0)), light_color(Vector3d(1,1,1)),decayMode(0){}
  xy_light(Vector3d position, Vector3d color, int decay):light_name("point light"),light_id(0), light_position(position), light_color(color), decayMode(decay){}  //convert constructor
  //using default copy constructor
  //using default assignment operator

  ~xy_light() {}//using default destructor
 

  string light_name;
  int light_id;
  Vector3d light_position;
  Vector3d light_color;
  int decayMode;//light attenuation
  //0 no decay
  //1 linear
  //2 quadratic
  //3 cubic
  virtual double getDecayFactor(double distance)
  {
        double d = fabs(distance);
	switch(decayMode)
        {
            case 1: return 1.0/(0.005*d + 1.0);
            case 2: return 1.0/(0.02*d*d + 0.2*d + 1);
            case 3: return 1.0/(0.002*d*d*d + 0.02*d*d + 0.2*d + 1);
	    default: return 1;
        }
 	return 1.0;
  }
  virtual double getDecayShadow(double distance, int md)
  {
        double d = fabs(distance);
	switch(md)
        {
            case 1: return 1.0/(0.075*d + 1.0);
            case 2: 
		{
			double t = 1.0/(0.002*d*d+0.02*d + 1);
			if(t >0.999)return 0.999;
			else return t;
		}
            case 3: 
		{
			double t = 1.0/(0.002*d*d*d + 0.02*d*d+ 0.02*d + 1);
			return t;
		}
	    default: return 1;
        }
 	return 1.0;
  }

  virtual Vector3d getLightColor(Vector3d P, xy_environment &env)
	{
		double shadow = testShadow(P,env,this);
		return shadow * light_color;
		
	}

};

class xy_spotLight: public xy_light
{
public:
  xy_spotLight():light_name("spot light"),light_id(0),light_position(Vector3d(0,0,0)), light_color(Vector3d(1,1,1)),decayMode(0),looking_at(Vector3d(-1,-1,-1)), angle(90), exponent(1){}
  xy_spotLight(Vector3d position, Vector3d color, int decay, Vector3d looking, double a, int exp):light_name("default light"),light_id(0), light_position(position), light_color(color), decayMode(decay),
	looking_at(looking), angle(a), exponent(exp){}  //convert constructor
  //using default copy constructor
  //using default assignment operator

  ~xy_spotLight() {}//using default destructor
 

  string light_name;
  int light_id;
  Vector3d light_position;
  Vector3d light_color;
  int decayMode;//light attenuation
  Vector3d looking_at;
  double angle;
  int exponent;

  virtual Vector3d getLightColor(Vector3d P, xy_environment &env)
	{
		double shadow = testShadow(P,env,this);
	
		Vector3d u, ul;
		ul = looking_at - light_position;
		if(ul.norm()<SMALL_VALUE)return Vector3d(0,0,0);
                ul = ul.normalize();
		u = light_position - P;
		if(u.norm() <SMALL_VALUE)return light_color;
                u = u.normalize();
		double cos_theta = -1*u*ul;
		if(cos_theta <= cos(angle/2.0 /180.0 * 3.1415926))return Vector3d(0,0,0);
		else
		{ return shadow*light_color * pow(cos_theta, exponent);}
		
	}

};

class xy_areaLight: public xy_light
{
public:
  xy_areaLight(Vector3d position, Vector3d color, int decay, Vector3d looking, Vector3d up, int area_w, int area_h, double scale_s, unsigned char* tex):light_name("area light"),light_id(0), light_position(position), light_color(color), decayMode(decay),
	looking_at(looking), up_direction(up),texture(tex),area_width(area_w), area_height(area_h),scale(scale_s){}  //convert constructor
  //using default copy constructor
  //using default assignment operator

  ~xy_areaLight() {}//using default destructor
 

  string light_name;
  int light_id;
  Vector3d light_position;
  Vector3d light_color;
  int decayMode;//light attenuation
  Vector3d looking_at;
  int area_width,area_height;
  Vector3d up_direction;
  double scale;
  unsigned char *texture;

  virtual Vector3d getLightColor(Vector3d P, xy_environment &env)
	{
		double shadow = testShadow(P,env,this);
		Vector3d mask_color(0,0,0);
		Vector3d area_u,area_v;
		Vector3d u, ul, view;
		ul = looking_at - light_position;
		if(ul.norm()<SMALL_VALUE)return Vector3d(0,0,0);
                ul = ul.normalize();
		u = light_position - P;
		while(Abs(up_direction.normalize()*ul)>0.999)up_direction = up_direction + Vector3d(rand()%3,rand()%3,rand()%3);
		up_direction = up_direction.normalize();
		area_u = up_direction % ul;
		area_v = ul % area_u;
                u = u.normalize();
		double uv_u = u * area_u;
		double uv_v = u * area_v;
		uv_u = (uv_u + 1)/2 / scale;
		uv_v = (uv_v + 1)/2 / scale;
		int row,col;
		row = uv_v * area_height;
		col = uv_u * area_width;
		while(row<0)row+=area_height;
		while(col<0)col+=area_width;
		row%=area_height;
		col%=area_width;
		mask_color[0] = texture[(col+row*area_width)*4]/255.0;
		mask_color[1] = texture[(col+row*area_width)*4+1]/255.0;
		mask_color[2] = texture[(col+row*area_width)*4+2]/255.0;
		
	
		return shadow*(mask_color^light_color); 
		
	}

};

class xy_environment
{
public:
  std::vector<xy_object*> objects;
  std::vector<xy_light*> lights;
  xy_environment()
  {
         light_mode = 0;//no light
 	 specular_mode = 0;//no spacular light
 	 shadow_mode = 0;//no shadow
  }
  //using default copy constructor
  //using default assignment operator
  Vector3d getColor(Vector3d ray_start, Vector3d ray_end);
  ~xy_environment() 
  {
     int i;
     for(i = 0; i < objects.size(); i++)delete objects[i];
     for(i = 0; i < lights.size(); i++)delete lights[i];
     
  }
  int light_mode;
  int specular_mode;
  int shadow_mode;
  BIH root;
  Collision objTest(BIH *node, Vector3d start, Vector3d end, double dis_max);
  Collision quickTest(BIH *node, Vector3d start, Vector3d end, double dis_max);
  void cutTree(BIH *node);
};

class xy_sphere: public xy_object
{
public:
	xy_sphere():xy_object()
        {
		obj_name = "sphere";
		radius = 0.5;
 		close_inter = FAR_FAR_AWAY;// the closer intersection point
       		far_inter = FAR_FAR_AWAY;// the further intersection point
		ambient_factor = 0.1;
		diffuse_factor = 1.0;
		specular_factor = 1;
		specular_power = 100;
        }
	xy_sphere(Vector3d cent, double r, Vector3d color, int id):xy_object()
	{
                obj_id = id;
		obj_position = cent;
		obj_color = color; 
		radius = r;
		obj_name = "sphere";
		close_inter = FAR_FAR_AWAY;// the closer intersection point
       		far_inter = FAR_FAR_AWAY;// the further intersection point
		ambient_factor = 0.1;
		diffuse_factor = 1.0;
		specular_factor = 1;
		specular_power = 100;
	};
        xy_sphere(Vector3d cent, double r, Vector3d color, int id, Vector4d factors)
	{
                obj_id = id;
		obj_position = cent;
		obj_color = color; 
		radius = r;
		obj_name = "sphere";
		close_inter = FAR_FAR_AWAY;// the closer intersection point
       		far_inter = FAR_FAR_AWAY;// the further intersection point
		ambient_factor = factors.x;
		diffuse_factor = factors.y;
		specular_factor = factors.z;
		specular_power = factors.w;
	};
	double radius;
	
public:
	virtual Collision findIntersection(Vector3d ray_start, Vector3d ray_end)
	{
		Collision result;
		Vector3d t_n;
		result.hit_position = sphereIntersection(ray_start, ray_end, t_n, obj_position, radius, close_inter, far_inter);
		result.hit_normal = t_n.normalize();
		result.pobj = this;
		Vector3d temp_v = result.hit_position - ray_start;
		result.hit_distance = temp_v.norm();
		return result;
		 
	}

       virtual Vector3d getNormal(Vector3d b)
	{
		Vector3d n = b - obj_position;
		if(n.norm() < SMALL_VALUE)n = FAR_FAR_AWAY;
		return n.normalize();
	}
       Vector3d close_inter;// the closer intersection point
       Vector3d far_inter;// the further intersection point
};
class xy_plane: public xy_object
{
public:
	Vector3d plane_normal;
	double plane_dis;
	Vector3d texture_u,texture_v;
	virtual Vector3d getNormal(Vector3d b)
	{
		if(bump_mode == 0||texture == NULL)return plane_normal;
		double u2d,v2d;
		Vector3d dis = b - getCenter();
		u2d = dis*texture_u;
		v2d = dis*texture_v;
		
		Vector4d trans(u2d,v2d,1,1);
		
		trans = transform_matrix * trans;
		u2d = trans[0]+img_w/2; // centered
		v2d = trans[1]+img_h/2; // centered
		
		Vector3d tex_color;
			
		int row, col;
		row = v2d*img_h;
		col = u2d*img_w;
			
		if(bump_mode == 1) //use the texture for bump
		{
			while(row<0)row+=img_h;
			while(col<0)col+=img_w;
			row = row%img_h;
			col = col%img_w;
			tex_color[0] = texture[(col + row*img_w)*4]/255.0 - 0.5;
			tex_color[1] = texture[(col + row*img_w)*4+1]/255.0- 0.5;
			tex_color[2] = texture[(col + row*img_w)*4+2]/255.0- 0.5;
			Vector3d t_n = plane_normal.normalize()+Vector3d(tex_color*bump_depth);	
			return t_n.normalize();
		}
		return plane_normal;
	}
	
	xy_plane(Vector3d p_normal, double p_dis, int id, Vector3d color) :xy_object()
// p_dis is the ditance from plane to the origin
	{
		obj_name = "plane";
		obj_position = Vector3d(0,0,0);
		if(p_normal.norm()<SMALL_VALUE){plane_normal = Vector3d(0,1,0);}
		else	plane_normal = p_normal.normalize();
		obj_color = color;
		plane_dis = p_dis;
		obj_id = id;
		ambient_factor = 0.1;
		diffuse_factor = 1.0;
		specular_factor = 1;
		specular_power = 100;
		if(Abs(uv_up*plane_normal.normalize())>0.999)//the same;
		uv_up = Vector3d(0,1,0);
		Vector3d u = uv_up % plane_normal;
		Vector3d v = plane_normal % u;
		texture_u = u.normalize();
		texture_v = v.normalize();
	}
        xy_plane(Vector3d p_normal, double p_dis, int id, Vector3d color, Vector4d factors) :xy_object()
// p_dis is the ditance from plane to the origin
	{
		obj_name = "plane";
		obj_position = Vector3d(0,0,0);
		if(p_normal.norm()<SMALL_VALUE){plane_normal = Vector3d(0,1,0);}
		else	plane_normal = p_normal.normalize();
		obj_color = color;
		plane_dis = p_dis;
		obj_id = id;
		ambient_factor = factors.x;
		diffuse_factor = factors.y;
		specular_factor = factors.z;
		specular_power = factors.w;
		if(Abs(uv_up*plane_normal.normalize())>0.999)//the same;
		uv_up = Vector3d(0,1,0);
		Vector3d u = uv_up % plane_normal;
		Vector3d v = plane_normal % u;
		texture_u = u.normalize();
		texture_v = v.normalize();
	}
       
	~xy_plane();
	public:
	Collision findIntersection(Vector3d ray_start, Vector3d ray_end)
	{
		Vector3d t_n;
		Collision result;
		result.hit_position = planeIntersection(ray_start, ray_end, t_n, plane_normal, plane_dis);
		result.hit_normal = plane_normal.normalize();
		result.pobj = this;
		Vector3d temp_v = result.hit_position - ray_start;
		result.hit_distance = temp_v.norm();
		return result;
	}
	Vector3d getColor(Vector3d position)
	{
		if(tile_mode == 0||texture == NULL)return getCheckBoardColor(position, obj_color, Vector3d(1,1,1), 0.5);//return check board color
		double u2d,v2d;
		Vector3d dis = position - getCenter();
		u2d = dis*texture_u;
		v2d = dis*texture_v;
		
		Vector4d trans(u2d,v2d,1,1);
		
		trans = transform_matrix * trans;
		u2d = trans[0]+img_w/2; // centered
		v2d = trans[1]+img_h/2; // centered
		
		Vector3d tex_color;
			
		int row, col;
		row = v2d*img_h;
		col = u2d*img_w;
			
		if(tile_mode == 2) //copy
		{
			while(row<0)row+=img_h;
			while(col<0)col+=img_w;
			row = row%img_h;
			col = col%img_w;
			tex_color[0] = texture[(col + row*img_w)*4]/255.0;
			tex_color[1] = texture[(col + row*img_w)*4+1]/255.0;
			tex_color[2] = texture[(col + row*img_w)*4+2]/255.0;
			return tex_color;	
		}
		if(tile_mode == 1) //no copy
		{
			if(row<0||row>=img_h||col<0||col>img_w)return Vector3d(0,0,0);
			tex_color[0] = texture[(col + row*img_w)*4]/255.0;
			tex_color[1] = texture[(col + row*img_w)*4+1]/255.0;
			tex_color[2] = texture[(col + row*img_w)*4+2]/255.0;
			return tex_color;	
		}
		
		
	}
	virtual Vector3d getCenter(){return -1*plane_normal*plane_dis;}
};

class xy_triangle: public xy_object
{
public:
	xy_triangle(Vector3d pp1, Vector3d pp2, Vector3d pp3, int id, Vector3d color, Vector4d factors):xy_object()
 // p_dis is the ditance from plane to the origin
	{
		p1 = pp1;
		p2 = pp2;
		p3 = pp3;
		obj_name = "triangle";
		obj_position = 1/3.0 * (p1 +  p2 +  p3);
		obj_color = color;
		obj_id = id;
		ambient_factor = factors.x;
		diffuse_factor = factors.y;
		specular_factor = factors.z;
	        specular_power = factors.w;
		Vector3d a,c;
		Vector3d t = FAR_FAR_AWAY;
		a = p2 - p1;
		c = p3 - p1;
		if(a.norm()> SMALL_VALUE && c.norm()> SMALL_VALUE)
		{
			t = a%c;
			t = t.normalize();
		}
		temp_normal = t;
	}
	
        Vector3d getNormal(Vector3d b)
	{
		if(smooth_mode == 0)
		{
			Vector3d t_n = (p2-p1)%(p3-p1);
			return t_n.normalize();
		}
		else
		{
			Vector3d uvw = getUVW(b, p1, p2, p3);
			return vn1*uvw[0] + vn2*uvw[1] + vn3*uvw[2];
		}
	}
	
	Vector3d getColor(Vector3d position)
	{
		
		if(texture==NULL)
		return obj_color;
		else
		{
			Vector3d tex_color;
			Vector2d uv;
			int row, col;
			Vector3d uvw = getUVW(position, p1, p2, p3);

			uv = uvw[0]*uv1 + uvw[1]*uv2 + uvw[2]*uv3;
			row = uv[1]*img_h; while(row<0)row+=img_h;
			col = uv[0]*img_w; while(col<0)col+=img_w;
			row = row%img_h;
			col = col%img_w;
			
			tex_color[0] = texture[(col + row*img_w)*4]/255.0;
			tex_color[1] = texture[(col + row*img_w)*4+1]/255.0;
			tex_color[2] = texture[(col + row*img_w)*4+2]/255.0;
			return tex_color;	
			
			
		}
	}//return object color with texture, no shading
	Collision findIntersection(Vector3d ray_start, Vector3d ray_end)
	{
		
		
		Collision result;
		result.hit_position = triangleIntersection(ray_start, ray_end, p1,p2,p3);
		Vector3d t_n = getNormal(result.hit_position);
		result.hit_normal = t_n.normalize();
		result.pobj = this;
		Vector3d temp_v = result.hit_position - ray_start;
		result.hit_distance = temp_v.norm();
		return result;
	}
        virtual Vector3d getCenter(){return 1.0/3*(p1+p2+p3);}
	virtual Vector3d getMin()
	{
		Box min_box;
		min_box.fixBox(p1);
		min_box.fixBox(p2);
		min_box.fixBox(p3);
		return min_box.bounds[0];
	}
        virtual Vector3d getMax()
	{
		Box min_box;
		min_box.fixBox(p1);
		min_box.fixBox(p2);
		min_box.fixBox(p3);
		return min_box.bounds[1];
	}
public:
	Vector3d p1;
	Vector3d p2;
	Vector3d p3;
        Vector3d temp_normal;
};

class xy_box: public xy_object
{
public:
	Vector3d corner_llf; // left low front
	Vector3d corner_llb; // left low back
	Vector3d corner_luf; // left up front
	Vector3d corner_lub; // left up back
	Vector3d corner_rlf; // right low front
	Vector3d corner_rlb; // right low back
	Vector3d corner_ruf; // right up front
	Vector3d corner_rub; // right up back

	Vector3d findSurfaceColor(Vector3d point)
	{
		return obj_color; // just return color
	}
	void initial(Vector3d llf, Vector3d rub) // left low front and right up back
	{
		corner_llf = llf; // left low front
		corner_rub = rub; // right up back
		corner_llb = Vector3d(llf.x,llf.y,rub.z); // left low back
		corner_luf = Vector3d(llf.x,rub.y,llf.z); // left up front
		corner_lub = Vector3d(llf.x,rub.y,rub.z); // left up back
		corner_rlf = Vector3d(rub.x,llf.y,llf.z); // right low front
		corner_rlb = Vector3d(rub.x,llf.y,rub.z); // right low back
		corner_ruf = Vector3d(rub.x,rub.y,llf.z); // right up front
		
	}
	xy_box(Vector3d llf, Vector3d rub,  int id, Vector3d color, Vector4d factors ):xy_object()
	{
		initial(llf, rub);
		obj_name = "box";
		obj_position = 1/2.0 * (llf + rub);
		obj_color = color;
		obj_id = id;
		ambient_factor = factors.x;
		diffuse_factor = factors.y;
		specular_factor = factors.z;
		specular_power = factors.w;
	}
	xy_box(Vector3d corner[8],  int id, Vector3d color, Vector4d factors):xy_object()
	{
		corner_llf = corner[2]; // left low front
		corner_rub = corner[5]; // right up back
		corner_llb = corner[6]; // left low back
		corner_luf = corner[0]; // left up front
		corner_lub = corner[4]; // left up back
		corner_rlf = corner[3]; // right low front
		corner_rlb = corner[7]; // right low back
		corner_ruf = corner[1]; // right up front
		obj_name = "box";
		obj_position = 1/2.0 * (corner_llf + corner_rub);
		obj_color = color;
		obj_id = id;
		ambient_factor = factors.x;
		diffuse_factor = factors.y;
		specular_factor = factors.z;
		specular_power = factors.w;
	}
	Collision findIntersection(Vector3d Ori, Vector3d P);
	Vector3d getNormal(Vector3d b)
	{
		Vector3d n = QUITE_FAR;
		findIntersection(obj_position, b);
		return n;
	}
};

class xy_objfile: public xy_object
{
public:
	xy_objfile(int id, PolySurf *sc):xy_object(),smooth_mode(0),scene(sc)
 // p_dis is the ditance from plane to the origin
	{
		obj_name = "objfile";
		obj_position = scene->Centroid();
		obj_color = Vector3d(0,0,0);
		obj_id = id;
		ambient_factor = 0.0;
		diffuse_factor = 1;
		specular_factor = 1;
	        specular_power = 30;
	}
	Vector2d isOnFace(Vector3d pos)
        {
	    Vector3d a,c;
	    double area;
            Vector3d uvw;
            Vector3d pp1,pp2,pp3;
            int index = -1;
	    Vector2d result(-1,-1);
	    int fi,fj;
	    for(fi = 0; fi < scene->nfaces; fi++)
	    {
		if(result[0]!=-1)break;
		for(fj = 1; fj <= scene->faces[fi].nverts-2; fj++)
		{
			index = scene->faces[fi].verts[0][0];
			pp1 = scene->verts[index];
			index = scene->faces[fi].verts[fj][0];
			pp2 = scene->verts[index];
			index = scene->faces[fi].verts[fj+1][0];
			pp3 = scene->verts[index];
			uvw = getUVW(pos,pp1,pp2,pp3);

			if(uvw.norm() < 1+SMALL_VALUE && uvw.norm()>1-SMALL_VALUE)
			{
				result[0] = index;
				result[1] = fj;	
			}
	
		}
	    }	
	    return result;
        }
        Vector3d getNormal(Vector3d b)
	{
		isOnFace(b);
		return temp_normal;
	}
	
	Collision findIntersection(Vector3d ray_start, Vector3d ray_end)
	{
	    Collision c_result;
	    return c_result;
	    Vector3d a,c;
	    double dis = HUGE_VALUE;
            Vector3d pp1,pp2,pp3,uvw;
            int index = -1;
	    Vector2d result(-1,-1);
            Vector3d temp_normal;
	    int fi,fj;
	    for(fi = 0; fi < scene->nfaces; fi++)
	    {
		for(fj = 1; fj <= scene->faces[fi].nverts-2; fj++)
		{
			index = scene->faces[fi].verts[0][0];
			pp1 = scene->verts[index];
			index = scene->faces[fi].verts[fj][0];
			pp2 = scene->verts[index];
			index = scene->faces[fi].verts[fj+1][0];
			pp3 = scene->verts[index];
			Vector3d t = triangleIntersection(ray_start, ray_end, pp1,pp2,pp3);
			Vector3d t_v = t - ray_start;

			if(t_v.norm()< dis)
			{	
				dis = t_v.norm();
				result[0] = index;
				result[1] = fj;
				c_result.hit_position = t;
				c_result.hit_distance = dis;
				c_result.pobj = this;
				c_result.unused_int[0] = result[0];
				c_result.unused_int[1] = result[1];
				if(smooth_mode==0)
				{
					a = pp2 - pp1,c = pp3 - pp1;
					temp_normal = a%c;
					c_result.hit_normal = temp_normal.normalize();
				}
				else
				{
					temp_normal = 
						uvw[0]*scene->norms[scene->faces[fi].verts[0][1]] + 
						uvw[1]*scene->norms[scene->faces[fi].verts[fj][1]] + 
						uvw[2]*scene->norms[scene->faces[fi].verts[fj+1][1]];
					c_result.hit_normal = temp_normal.normalize();
				}	
			}
	
		}
	    }
		

		return c_result;
	}
public: 
	int smooth_mode;
	PolySurf *scene;
public:
        Vector3d temp_normal;
};

Vector3d findShadeColor(Vector3d B, Vector3d P, xy_environment &env); // find the shade color without knowing whether it hits something
Vector3d RecursiveShade(xy_object *pobj, Vector3d B, Vector3d P, xy_environment &env, int level, double attenuation, double &depth); //find the color using recursive shading
Vector3d AmbientOcclusion(Vector3d P, Vector3d ray, xy_object *pobj, xy_environment &env, int nrays, double max_distance); //global illumination by ambient occlusion
//transform
Matrix4x4 xy_translate(Matrix4x4 m, Vector3d t);
Matrix4x4 xy_scale(Matrix4x4 m, Vector3d t);
Matrix4x4 xy_rotateX(Matrix4x4 m, double angle);
Matrix4x4 xy_rotateY(Matrix4x4 m, double angle);
Matrix4x4 xy_rotateZ(Matrix4x4 m, double angle);



#endif
