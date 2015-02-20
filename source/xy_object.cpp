#include "xy_object.h"

/*
  Function to flip an array in y coordinate to make it upside down
*/
void image_flip(unsigned char *pixmap, int w, int h)
{ 
   int i,j,channel;
   unsigned char temp;
   for(j = 0; j <= (h-1)/2; j++)
     for(i = 0; i < w; i++)
       for(channel = 0; channel < 4; channel++)
   {
      //exchange data
      temp = pixmap[(j*w + i)*4 + channel];
      pixmap[(j*w+i)*4 + channel] = pixmap[((h-1-j)*w+i)*4 + channel];
      pixmap[((h-1-j)*w+i)*4 + channel] = temp;
   }

}
/*
  Routine to write the current framebuffer to an image file
*/
void writeimage(){
  string outfilename;
  cout << "Please Enter output image filename: "<<endl;
  cin >> outfilename;
  writeimage(outfilename);
 
}
void writeimage(string outfilename){
  int w = glutGet(GLUT_WINDOW_WIDTH);
  int h = glutGet(GLUT_WINDOW_HEIGHT);
  unsigned char pixmap[4 * w * h];
  cout <<"Creating image:"<<'\"'<<outfilename<<'\"'<<endl;
  glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixmap);
  image_flip(pixmap, w, h);
  Magick::Image image(w, h, "RGBA", Magick::CharPixel, (void *)pixmap);
  image_flip(pixmap, w, h);
  image.write(outfilename);
  cout <<"Job done"<<endl;
}
void writeimage(string outfilename, unsigned char *img, int img_w, int img_h)
{
  if(img == NULL || img_w <= 0 || img_h <=0){cout<<"Error in writing image."<<endl;return;}
  cout <<"Creating image:"<<'\"'<<outfilename<<'\"'<<endl;
  glReadPixels(0, 0, img_w, img_h, GL_RGBA, GL_UNSIGNED_BYTE, img);
  image_flip(img, img_w, img_h);
  Magick::Image image(img_w, img_h, "RGBA", Magick::CharPixel, img);
  image_flip(img, img_w, img_h);
  image.write(outfilename);
  cout <<"Job done"<<endl;

}
unsigned char* readimage(string img_filename, int &img_w, int &img_h){
  unsigned char* img= NULL;
  //check the image file name
  FILE *in;
  in = fopen(img_filename.c_str(),"r");
  if(in == NULL)
  {
   
     cout<<"Could not open image file."<<endl;
     return img;
  }
  fclose(in);
 // create the Image class
  Magick::Image image_read(img_filename);
  img_w = image_read.columns();
  img_h = image_read.rows();
 

 //allocate memory for the img pointer

  img = new unsigned char[4 * img_w * img_h];
  //get the indirect access to the image canvas
  image_read.modifyImage(); // lock it
  Magick::Pixels my_pixel_cache(image_read); // allocate an image pixel cache associated with image_read
  Magick::PixelPacket* pixels;// 'pixels' is a pointer to a PixelPacket array
  // define the view area that will be accessed via the image pixel cache
  int start_x = 0, start_y = 0, size_x = img_w, size_y = img_h;

  // return a pointer to the pixels of the defined pixel cache
  pixels = my_pixel_cache.get(start_x, start_y, size_x, size_y);
  //flip the cache and assign to the pixel map array
  int row, col;
  for(row = 0; row < img_h; row++)
  for(col = 0; col < img_w; col++)
  {
    img[((row) * img_w + col)*4] = (unsigned char)pixels[(row * img_w + col)].red;
    img[((row) * img_w + col)*4+1] = (unsigned char)pixels[(row * img_w + col)].green;
    img[((row) * img_w + col)*4+2] = (unsigned char)pixels[(row * img_w + col)].blue;
    img[((row) * img_w + col)*4+3] = (unsigned char)pixels[(row * img_w + col)].opacity;
  }
  image_flip(img,img_w,img_h);
  //delete [] pixels;// do not delete because it is got from Image cache
  // my_pixel_cache.sync();//update the image using the current cache if the data has been changed
  return img;
}

Vector3d jitter(double r)
{
	return Vector3d(r*(-1.0 + 2.0 * rand()/RAND_MAX),r*(-1.0 + 2.0 * rand()/RAND_MAX),r*(-1.0 + 2.0 * rand()/RAND_MAX));
}




/* intersection detection for sphere

	ray_start, the start point of the ray
        ray_end, the destination point of the ray
        normal, the variable to store surface normal vector
	center, the center of the sphere
	radius, the radius of the sphere
	close_i, the variable to store the closer intersection
	far_i, the variable to store the further intersection
        The function will look for the first intersection point on the sphere, according to the 
          ray "SE = ray_end -ray_start". It will return the first intersection 
          point(according to the ray_start position and its direction), and update 
          the normal vector on that point.
        If there is no intersection, it will return the FAR_FAR_AWAY point.
*/
Vector3d sphereIntersection(Vector3d ray_start, Vector3d ray_end, Vector3d &normal, Vector3d center, double radius, Vector3d &close_i, Vector3d &far_i)
{
	
	Vector3d I1,I2; // intersection points
	Vector3d SE;  // the ray vector(start to end)
        Vector3d u; // the normalized vector of SE
	Vector3d SC;  // vector from ray_star to the center of the sphere
	Vector3d x_close;    // middle of the two intersection point, the closest point to the center
        bool outside_tag = true; // 1 : outside. 0: inside or on the surface
	Vector3d d; // distance vector from the x_close to center
	//condition test
	normal = FAR_FAR_AWAY; // initialize using the far point	
	SE = ray_end - ray_start;
	if(radius <= SMALL_VALUE || SE.norm() <= SMALL_VALUE)//unable to calculate and return "no intersection"
	{
		close_i = FAR_FAR_AWAY;
		far_i = FAR_FAR_AWAY;
		return FAR_FAR_AWAY;
	}

	SC = center - ray_start;
	if(SC.norm() < radius)
	{
		outside_tag = false;// the start point of the ray is inside the sphere
	}
        else
	{
		outside_tag = true; // the start point is outside or on the surface
		if(SC*SE < 0) // there is no intersection if the ray starts outside and goes to the wrong direction
		//the start point of the ray is even further than I2. 
		{
			close_i = FAR_FAR_AWAY;
			far_i = FAR_FAR_AWAY;
			return FAR_FAR_AWAY;
		}
	}
	//start calculating
	ray_start = ray_start - SE.normalize() * 100 * radius; // move it back and make sure that it is outside the sphere
        SE = ray_end - ray_start; // a new ray which is longer and start outside the sphere
        u = SE.normalize();
	
        SC = center - ray_start;
	
	x_close = ray_start + u*(u * SC);
        d = center - x_close;
	
	
        if(d.norm() > radius)// return "no intersection"
        {
		close_i = FAR_FAR_AWAY;
		far_i = FAR_FAR_AWAY;
		return FAR_FAR_AWAY;
	}
        else
	{
		double a = sqrt(radius*radius - d.norm()*d.norm());
		
		I1 = x_close - a * u; // intersection 1
		I2 = x_close + a * u; // intersection 2
                if(outside_tag == true)//outside
		{
			
			close_i = I1; // the ray find I first
			far_i = I2; // then I2
			normal = I1 - center;
			normal = normal.normalize();
			return I1; // return the first one
		}
		else//inside
		{
			close_i = FAR_FAR_AWAY; // the ray can not go back to find it
			far_i = I2; // the ray go through out and find it
			normal = I2 - center;
			normal = normal.normalize();
			return I2; // return the only one
		}	
	}
}
/*intersection detection for plane
	plane_normal, the normal vector of the plane
	plane_dis, the distance from the plane to the origin point, on the direction of the normal
	for example, n = (0,1,0), dis = 1 is the plane under the xz plane with a distance of 1
	
*/

Vector3d planeIntersection(Vector3d ray_start, Vector3d ray_end, Vector3d &normal, Vector3d plane_normal, double plane_dis)
{
	Vector3d SE;
	double dis_start, dis_p, cos_theta; // dis_start is the distance from plane to the point of ray_start
	SE = ray_end - ray_start;
	normal = plane_normal;
	if(plane_normal*SE > 0)normal = -1.0 * normal;
	if(plane_normal.norm()<SMALL_VALUE || SE.norm() <SMALL_VALUE)return FAR_FAR_AWAY;
	normal = normal.normalize();
	dis_start = (plane_normal.normalize() * ray_start + plane_dis); // ray_start = ray_start - origin = OS
	
	cos_theta = (plane_normal * SE) / SE.norm()/plane_normal.norm();
		
	if(cos_theta * dis_start < 0)
	{
		dis_p = -1.0 * dis_start / cos_theta;
		return SE.normalize() * dis_p + ray_start;
	}
	else return FAR_FAR_AWAY;
}



Vector3d triangleIntersection(Vector3d ori, Vector3d P, Vector3d p1, Vector3d p2, Vector3d p3)
{
        Vector3d a,b,normal; // intersection point
	Vector3d OP,OB,B,OP1,OP2,OP3;  // sight line
	double area, area1,area2,area3;
	double dis_OB, dis_ori; // distanse between O and the triangle
	double theta;
	Vector3d tri_edge1, tri_edge2;
	a = p2 - p1; // edge a
	b = p3 - p1; // edge b
	OP = P - ori;
	if(OP.norm() < SMALL_VALUE)
	{
		return FAR_FAR_AWAY;
	}
	normal = b % a; // cross product
	if(normal.norm() < SMALL_VALUE) // the triangle has small area
	{
		return FAR_FAR_AWAY;
	}
	normal = normal.normalize();
	if(OP * normal > 0 )normal = -1 * normal;
	if( fabs(OP * normal) < SMALL_VALUE )// the sight line is parallel to the triangle
	{
		return FAR_FAR_AWAY;
	}
	OP1 = p1 - ori;
	OP2 = p2 - ori;
	OP3 = p3 - ori;
	if(OP1.norm()<SMALL_VALUE)return FAR_FAR_AWAY;
	dis_ori = OP1 * normal;
	dis_OB = dis_ori / (OP.normalize() * normal);
	dis_OB = fabs(dis_OB);
	OB = OP.normalize() * dis_OB;
	B = ori + OB;
	// the area of the triangle
	tri_edge1 = a;
	tri_edge2 = b;
	if(tri_edge1.norm() < SMALL_VALUE || tri_edge2.norm() < SMALL_VALUE)area = 0;
	else
	{
			theta = acos(tri_edge1.normalize()*tri_edge2.normalize());
			area = 0.5 * tri_edge1.norm() * tri_edge2.norm() * sin(theta);
	}

	// the area of the child triangle1
	tri_edge1 = a;
	tri_edge2 = B - p1;
	if(tri_edge1.norm() < SMALL_VALUE || tri_edge2.norm() < SMALL_VALUE)area1 = 0;
	else
	{
			theta = acos(tri_edge1.normalize()*tri_edge2.normalize());
			area1 = 0.5 * tri_edge1.norm() * tri_edge2.norm() * sin(theta);
	}
	// the area of the child triangle2
	tri_edge1 = b;
	tri_edge2 = B - p1;
	if(tri_edge1.norm() < SMALL_VALUE || tri_edge2.norm() < SMALL_VALUE)area2 = 0;
	else
	{
			theta = acos(tri_edge1.normalize()*tri_edge2.normalize());
			area2 = 0.5 * tri_edge1.norm() * tri_edge2.norm() * sin(theta);
	}
	// the area of the child triangle3
	tri_edge1 = B - p2;
	tri_edge2 = B - p3;
	if(tri_edge1.norm() < SMALL_VALUE || tri_edge2.norm() < SMALL_VALUE)area3 = 0;
	else
	{
			theta = acos(tri_edge1.normalize()*tri_edge2.normalize());
			area3 = 0.5 * tri_edge1.norm() * tri_edge2.norm() * sin(theta);
	}
	if(area1 + area2 + area3 < area + 0.001)	return B;
	else return FAR_FAR_AWAY;	
	

	

}
/*
	ray start point, ray end point, normal, environment, recursion loops control level, object pointer
	return a color
	this function was used to the former project without indirect lights.
*/
Vector3d findShadeColor(Vector3d B, Vector3d P, xy_environment &env)
{
	
	Vector3d ray, ray_p, ray_v;
	Vector3d Out; // point where the ray that goes out at
	Vector3d N_out; // normal of the out point
	Vector3d intersection, temp_normal, inter_normal,B2, temp_vector;
	Vector3d result;
	double dis, dis2;
	int cur_i = -1;
	ray = P - B;
	if(ray.norm()<SMALL_VALUE)return Vector3d(0.1,0.1,0.1);
	ray = ray.normalize(); // normalization

	if(1)//reflection
	{
		intersection = FAR_FAR_AWAY;
		inter_normal = FAR_FAR_AWAY;
		for(int i = 0; i < env.objects.size(); i++)
		{
			Collision c_result = env.objects[i]->findIntersection(B, P);
			B2 = c_result.hit_position;
		        
			if(B2.norm() >= HUGE_VALUE)continue; // no intersection found
			else
			{
				temp_vector = intersection - B;
				dis = temp_vector.norm();
				temp_vector = B2 - B;
				dis2 = temp_vector.norm();
				if(dis2 < dis)
				{
					intersection = B2;
					inter_normal = temp_normal;
					cur_i = i;
				}
			}
		}
		if(intersection.norm() >= HUGE_VALUE)
		{
			return Vector3d(0.1,0.1,0.1);
		}
		else
		{
		
			if(cur_i >= 0)result = env.objects[cur_i]->getShadeColor(B,intersection, env);
			return result;
		}
		
	}
	/*//refraction
	{}*/
		

	return Vector3d(0.1,0.1,0.1);		



}

//return object color within a shading environment
Vector3d xy_object::getShadeColor(Vector3d p, Vector3d b, xy_environment &env)
{
	double dis, attenuation,Ka = 1, Kd, Ks;
        Vector3d final_color= ambient_factor*Ka*(getColor(b)^get_ambient_color(b));
	final_color= final_color - 0.85 * Vector3d(GLOBAL_ILLUMINATION_FACTOR,GLOBAL_ILLUMINATION_FACTOR,GLOBAL_ILLUMINATION_FACTOR) + GLOBAL_ILLUMINATION_FACTOR * AmbientOcclusion(b, p-b, this, env, GLOBAL_ILLUMINATION_RAYS, GLOBAL_ILLUMINATION_DISTANCE);
	Vector3d N = getNormal(b).normalize();
	
	Vector3d V = p - b, L;
	if(V * N < 0) N = -1 * N;
	Vector3d diffuseColor, specularColor;
	
	if(N == FAR_FAR_AWAY || N.norm() < SMALL_VALUE|| V.norm()<SMALL_VALUE)return final_color;
	else 
	{	
		for(int i = 0; i < env.lights.size(); i++)
		{
			//diffuse color
			L = env.lights[i]->light_position - b;
			dis  = L.norm();
			attenuation = env.lights[i]->getDecayFactor(dis);
			Vector3d R = -1* L + 2.0 * (L*N)*N;
			if(V * N <= 0 ) continue;
			Kd = L.normalize() * N.normalize();
			if(Kd < 0)continue;
			Vector3d lightColor = env.lights[i]->getLightColor(b + N*0.01,env);
			diffuseColor = attenuation * diffuse_factor * Kd * ((getColor(b)^get_diffuse_color(b)) ^ lightColor);
			if(R.normalize()*V.normalize()<0)Ks = 0;
			else Ks = pow(R.normalize()*V.normalize(), specular_power);
			
			if(L * N <=0)specularColor = Vector3d(0,0,0);
				else specularColor = attenuation * (specular_factor) * Ks * (get_specular_color(b)^lightColor);
			dis  = V.norm();
			attenuation = env.lights[i]->getDecayFactor(dis);
			final_color = final_color + attenuation* diffuseColor + attenuation * specularColor;

		}
	
	}
	
	
	return final_color;

}


Collision xy_box::findIntersection(Vector3d Ori, Vector3d P)
{
	Collision result;
	Vector3d normal = FAR_FAR_AWAY;	

	Vector3d B, B1, B2;
	Vector3d a, b;

	Vector3d box_center, square_center, surface_norm;
	box_center = ( corner_llf +  corner_rub)/2.0;
	B = FAR_FAR_AWAY;
	normal = FAR_FAR_AWAY;
	b = B - Ori;

	//front
	square_center =  ( corner_llf +  corner_ruf)/2.0;
	B1 = triangleIntersection(Ori, P, corner_llf, corner_luf, corner_ruf);
	a = B1 - Ori;
	if(a.norm() < b.norm()) 
	{
		B = B1;
		b = B - Ori;
		normal = square_center - box_center; 
		normal = normal.normalize();
		
	}
	B2 = triangleIntersection(Ori, P,  corner_llf,  corner_ruf,  corner_rlf);
	a = B2 - Ori;
	if(a.norm() < b.norm())
	{
		B = B2;
		b = B - Ori;
		normal = square_center - box_center; 
		normal = normal.normalize();
	}

	//back
	square_center =  ( corner_llb +  corner_rub)/2.0;
	B1 = triangleIntersection(Ori, P,  corner_llb,  corner_rub,  corner_lub);
	a = B1 - Ori;
	if(a.norm() < b.norm()) 
	{
		B = B1;
		b = B - Ori;
		normal = square_center - box_center; 
		normal = normal.normalize();
	}
	B2 = triangleIntersection(Ori, P,  corner_llb,  corner_rlb,  corner_rub);
	a = B2 - Ori;
	if(a.norm() < b.norm()) 
	{
		B = B2;
		b = B - Ori;
		normal = square_center - box_center; 
		normal = normal.normalize();
	}

	//left
	square_center =  ( corner_llf +  corner_lub)/2.0;
	B1 = triangleIntersection(Ori, P,  corner_llf,  corner_lub,  corner_luf);
	a = B1 - Ori;
	if(a.norm() < b.norm()) 
	{
		B = B1;	
		b = B - Ori;	
		normal = square_center - box_center; 
		normal = normal.normalize();
	}
	B2 = triangleIntersection(Ori, P,  corner_llf,  corner_llb,  corner_lub);
	a = B2 - Ori;
	if(a.norm() < b.norm()) 
	{
		B = B2;
		b = B - Ori;
		normal = square_center - box_center; 
		normal = normal.normalize();
	}
	//right
	square_center =  ( corner_rlf +  corner_rub)/2.0;
	B1 = triangleIntersection(Ori, P,  corner_rlf,  corner_ruf,  corner_rub);
	a = B1 - Ori;
	if(a.norm() < b.norm()) 
	{
		B = B1;
		b = B - Ori;
		normal = square_center - box_center; 
		normal = normal.normalize();
	}
	B2 = triangleIntersection(Ori, P,  corner_rlf,  corner_rub,  corner_rlb);
	a = B2 - Ori;
	if(a.norm() < b.norm())
	{
		B = B2;
		b = B - Ori;
		normal = square_center - box_center; 
		normal = normal.normalize();
	}
	
	//up
	square_center =  ( corner_luf +  corner_rub)/2.0;
	B1 = triangleIntersection(Ori, P,  corner_luf,  corner_lub,  corner_rub);
	a = B1 - Ori;
	if(a.norm() < b.norm()) 
	{
		B = B1;
		b = B - Ori;
		normal = square_center - box_center; 
		normal = normal.normalize();
	}
	B2 = triangleIntersection(Ori, P,  corner_luf,  corner_rub,  corner_ruf);
	a = B2 - Ori;
	if(a.norm() < b.norm()) 
	{
		B = B2;
		b = B - Ori;
		normal = square_center - box_center; 
		normal = normal.normalize();
	}
	//low
	square_center =  ( corner_llf +  corner_rlb)/2.0;
	B1 = triangleIntersection(Ori, P,  corner_llf,  corner_rlb,  corner_llb);
	a = B1 - Ori;
	if(a.norm() < b.norm())
	{
		B = B1;
		b = B - Ori;
		normal = square_center - box_center; 
		normal = normal.normalize();
	}
	B2 = triangleIntersection(Ori, P,  corner_llf,  corner_rlf,  corner_rlb);
	a = B2 - Ori;
	if(a.norm() < b.norm())
	{
		B = B2;
		b = B - Ori;
		normal = square_center - box_center; 
		normal = normal.normalize();
	}
	
	if(b.norm()<HUGE_VALUE)
	{
		result.hit_position = B;
		result.hit_normal = normal.normalize();
		result.pobj = this;
		result.hit_distance = b.norm();
		return result;
	}
	else return result;
}

/*
	Test whether the point P is in the shadow, for the light p_light, given an environment
        Calculate the soft shadow color and caustic color by recursively calling it self.

*/
Vector3d AmbientOcclusion(Vector3d P, Vector3d ray, xy_object *pobj, xy_environment &env, int nrays, double max_distance) 
{
	Collision c_result;
	Vector3d ambient_color(0,0,0);
	Vector3d normal = pobj->getNormal(P);
	if(normal*ray>0)normal = -1*normal;
	normal = normal.normalize();
	Vector3d up = Vector3d(1,0,0);
	if(Abs(up*normal)>0.999)up = Vector3d(0,1,0);//change an up direction
	Vector3d u = up%normal;
	Vector3d v = u%normal;
	Vector3d a_ray;
	for(int i = 0; i < nrays; i++)
	{
		a_ray = (rand()%1000) * normal + (rand()%500 - 1000)*u + (rand()%500 - 1000)*v;
		a_ray = a_ray.normalize();
		c_result = env.quickTest(&(env.root), P, P+a_ray, max_distance);
		if(c_result.hit_distance>max_distance)//FAR_AWAY
		{
			ambient_color = ambient_color + Vector3d(1,1,1);
		}
		else
		{	
			ambient_color = ambient_color + c_result.pobj->getColor(c_result.hit_position);
		}
	}
	return ambient_color/nrays;
	
}



/*
	Test whether the point P is in the shadow, for the light p_light, given an environment
        Calculate the soft shadow color and caustic color by recursively calling it self.

*/
double testShadow(Vector3d P, xy_environment &env, xy_light *p_light) 
{
	Collision c_result;
	Vector3d inter = FAR_FAR_AWAY, inter2 = FAR_FAR_AWAY;
	Vector3d u1 = p_light->light_position - P, u2,u3;
	double decay1=0,decay2=0, decay3 = 0;
	c_result = env.objTest(&(env.root), P, P+u1, u1.norm());
        //shadow part
	inter = c_result.hit_position;
		if(!(inter == FAR_FAR_AWAY)) //there is an intersection
		{
			u2 = inter - P;
			if(u2.norm() + 0.0001 < u1.norm())//the intersection pointer is closer to P than the light position
			{
				double dis = u2.norm();
				
				decay1 = 1-p_light->getDecayShadow(dis,1);
				double dis2 = 0;
				Vector3d u_inverse = -1*u1;
				//find the furthest intersection
				if(c_result.pobj->refraction_index < 1)//no transparency
				{
					Collision c_inverse = env.objTest(&(env.root), p_light->light_position, p_light->light_position -u1, u1.norm()-dis);
					inter2 = c_inverse.hit_position;
					if(inter2 == FAR_FAR_AWAY)dis2 = 0;
					else
					{
						u3 = inter2 - P;
						if(u3.norm() + 0.0001 < u2.norm())dis2 = 0;
						else // something with thickness
						{
							Vector thickness = inter2 - inter;
							dis2 = thickness.norm();
							decay2 = p_light->getDecayShadow(dis2,2);
							
						}
					}
				}
				else // caustic
				{
					
					Vector3d ray = u1;
					Vector3d ray2, ray3, a, b;
					double nt = c_result.pobj->refraction_index; // refraction_index
					double nr = 1.0; // the air

					double cos_theta = -1 * ray.normalize() * c_result.hit_normal;
					double sin_theta;
					double cos_phi;
					double sin_phi;
					if(cos_theta < 0)
					{
						c_result.hit_normal = -1 * c_result.hit_normal;
						cos_theta = -1 * cos_theta;
					}
				
					//calculating refraction ray
				
					if(cos_theta > 1 - SMALL_VALUE)ray2 = ray;
	 				else
					{
						a = -1 * c_result.hit_normal;	
						b = ray - (ray*a)* a;
						b = b.normalize();
						sin_theta = sqrt(1 - cos_theta * cos_theta);
						sin_phi = (nr/nt)*sin_theta;
						cos_phi = sqrt(1-sin_phi*sin_phi);
						ray2 = cos_phi * a + sin_phi * b;
								
					}
					//calculate the ray going out
			
					ray = ray2;
					Collision intersection =  env.objTest(&(env.root), inter + ray * TOO_CLOSE, inter + ray,u1.norm()-dis);
		
					if(intersection.hit_normal * ray < 0 )intersection.hit_normal = -1 * intersection.hit_normal;  // the direction out
					if(intersection.hit_position.norm() > HUGE_VALUE)//a single plane or polygon
					{
						ray3 = ray2;
						inter2 = inter;	
						decay2 = 0;
					}
					else //an object with transparency
					{
						nt = intersection.pobj->refraction_index; // the object
	
						nr = 1.0; // the air
			
						a = intersection.hit_normal.normalize();
						b = ray - (ray*a)* a;
						b = b.normalize();
						cos_phi = ray*intersection.hit_normal.normalize();
						sin_phi = sqrt(1 - cos_phi * cos_phi);
						sin_theta = (nt/nr)*sin_phi;
			
		
						if(sin_theta >= 1) // total reflection
						{
							decay2 = 0;
						}
						else
						{
			
							cos_theta = sqrt(1 - sin_theta * sin_theta);
				
							ray3 = cos_theta * a + sin_theta * b;
							inter2 = intersection.hit_position;
							double cos_x = ray3.normalize()*u1.normalize();
							if(cos_x <= 0.01)decay2 = 0; 
							else decay2 = 4.0*pow(cos_x,100) * testShadow(inter2+ray3*TOO_CLOSE, env, p_light);
						}
					}
					
				}
				return (decay1)*(1+decay2);
			}
			
		}

	return 1;
}

/*
	Recrusively finding the shading color, including refraction and reflection.
	pobj is a pointer to the object that the ray is going inside. if pobj == NULL, the ray is going in the air
	B is the ray starting point, P is the ending point, env gives the environment encluding objects and lights.
	If level drops to 0 or attenuation drops below 0.01, the function will return a black color, otherwise it will return the color recursively.
*/

Vector3d RecursiveShade(xy_object *pobj, Vector3d B, Vector3d P, xy_environment &env, int level, double attenuation,double &depth)
{
	Collision c_result;
	Collision intersection;
	Vector3d finalColor(0,0,0);
	if(level <= 0 || attenuation <= MIN_ATTENUATION)return Vector3d(0.1,0.1,0.1);
	
	//else, the ray is marching in the air
	Vector3d ray, ray_reflection, ray_refraction;

	Vector3d B2, temp_vector;
	
	Vector3d shadingColor, reflectionColor, refractionColor;
	double reflection_rate, refraction_rate;
	Vector3d a,b;
	double sin_theta, sin_phi, cos_phi;
	double dis, dis2;
	int cur_i = -1;
	ray = P - B;
	double R0, Rt, Rl, cos_theta, nt, nr;
	if(ray.norm()<SMALL_VALUE)return finalColor;
	ray = ray.normalize(); // normalization
	

	if(pobj == NULL)//reflection
	{
		intersection = Collision();

		c_result = env.objTest(&(env.root), B, P,1000);
	
		if(c_result.hit_distance > TOO_CLOSE && c_result.hit_distance + TOO_CLOSE < intersection.hit_distance)	
		{
			intersection = c_result;
			if(ray*intersection.hit_normal>0)intersection.hit_normal = -1 * intersection.hit_normal;
			cur_i = intersection.pobj->obj_id;
			
		}
		
		for(int i = env.objects.size() - 1; i >=0; i--) // for the plane objects
		{
			
			if(env.objects[i]->obj_name == "triangle" )break;
			c_result = env.objects[i]->findIntersection(B, P);
			
			B2 = c_result.hit_position;
		        
			if(B2.norm() == FAR_FAR_AWAY)continue; // no intersection found
			else
			{
				
				dis = intersection.hit_distance;
				temp_vector = B2 - B;
				dis2 = temp_vector.norm();
				if(dis2>TOO_CLOSE)//just to avoid it self
				if(intersection.hit_distance == HUGE_VALUE || dis2 + TOO_CLOSE < dis)
				{
					intersection = c_result;
					intersection.hit_normal = c_result.hit_normal.normalize();
					
					if(ray*intersection.hit_normal>0)intersection.hit_normal = -1 * intersection.hit_normal;
					intersection.pobj = env.objects[i];
					cur_i = i;
				}
			}
		}
		
			
	
		
		if(intersection.hit_distance >= HUGE_VALUE) // the ray hits nothing
		{
			
			return finalColor;
		}
		else // the ray hits an object
		{
			if(depth<=0){depth = c_result.hit_distance;
			if(intersection.pobj->obj_name=="plane" )depth*=1.2;}
			if(level == 1)return env.objects[cur_i]->getColor(intersection.hit_position);
			if(cur_i >= 0)shadingColor = env.objects[cur_i]->getShadeColor(B,intersection.hit_position, env);
			

			if(env.objects[cur_i]->refraction_index > 1)//crystal
			{
				
				nt = env.objects[cur_i]->refraction_index; // the object
				nr = 1.0; // the air
				R0 = (nt - nr)/(nt + nr);
				R0 *= R0; // Fresnel approximation
				cos_theta = -1 * ray.normalize() * intersection.hit_normal;
				Rl = R0 + (1 - R0)* pow(1-cos_theta, 5);
				Rt = 1 - Rl;
				finalColor = shadingColor* sqrt(Rl);
				reflection_rate = Rl;
				refraction_rate = Rt;
				ray_reflection = ray - 2.0 * (ray*intersection.hit_normal)*intersection.hit_normal;
			
			
		
				finalColor = finalColor + reflection_rate * 
					RecursiveShade(NULL, intersection.hit_position - TOO_CLOSE * ray, 
					  intersection.hit_position + ray_reflection, env, level-1, attenuation * reflection_rate,depth);
				
				//calculating refraction ray
				
				if(cos_theta > 1 - SMALL_VALUE)ray_refraction = ray;
 				else
				{
					a = -1 * intersection.hit_normal;	
					b = ray - (ray*a)* a;
					b = b.normalize();
					sin_theta = sqrt(1 - cos_theta * cos_theta);
					sin_phi = (nr/nt)*sin_theta;
					cos_phi = sqrt(1-sin_phi*sin_phi);
					ray_refraction = cos_phi * a + sin_phi * b;
								
				}
				
				finalColor = finalColor + refraction_rate * 
					RecursiveShade(env.objects[cur_i], intersection.hit_position + TOO_CLOSE * ray, 
					  intersection.hit_position + ray_refraction, env, level-1, attenuation * refraction_rate,depth);
			}
			else
			{
				Rl = env.objects[cur_i]->get_alpha(intersection.hit_position);
				if(Rl>1)Rl = 1;
				Rt = 1 - Rl;
				finalColor = Rl * shadingColor;
								
				
				reflection_rate = 0.8*Rl * env.objects[cur_i]->get_specular_factor(intersection.hit_position);
				ray_reflection = ray - 2.0 * (ray*intersection.hit_normal)*intersection.hit_normal;
				ray_refraction = ray;
				finalColor = finalColor + reflection_rate *
					RecursiveShade(NULL, intersection.hit_position - TOO_CLOSE * ray, 
					  intersection.hit_position + ray_reflection, env, level-1, attenuation * reflection_rate,depth);
			//	finalColor = finalColor + (1-reflection_rate) *
			//		RecursiveShade(NULL, intersection.hit_position + TOO_CLOSE * ray, 
			//		  intersection.hit_position + ray_refraction, env, level-1, attenuation * (1-reflection_rate),depth);
				
				return finalColor;
				
				
			}
			return finalColor;
		}
		
	}
	else  // the ray goes inside the object
	{	
	
		intersection =  env.objTest(&(env.root), B + ray * TOO_CLOSE, B + ray,1000);
		
		if(intersection.hit_normal * ray < 0 )intersection.hit_normal = -1 * intersection.hit_normal;  // the direction out
		if(intersection.hit_position.norm() > HUGE_VALUE)//a single plane or polygon
		{
			finalColor = finalColor + RecursiveShade(NULL, B , P, env, level, attenuation,depth);	
		}
		else //an object with transparency
		{
			nt = (env.objects[intersection.pobj->obj_id])->refraction_index; // the object
	
			nr = 1.0; // the air
			
			a = intersection.hit_normal.normalize();
			b = ray - (ray*a)* a;
			b = b.normalize();
			cos_phi = ray*intersection.hit_normal.normalize();
			sin_phi = sqrt(1 - cos_phi * cos_phi);
			sin_theta = (nt/nr)*sin_phi;
			
		
			if(sin_theta >= 1) // total reflection
			{
				ray_refraction = sin_phi * b - cos_phi * a;
				finalColor = finalColor + RecursiveShade(intersection.pobj, intersection.hit_position - TOO_CLOSE * ray, 
					intersection.hit_position + ray_refraction, env, level-1, attenuation,depth);
			}
			else
			{
			
				cos_theta = sqrt(1 - sin_theta * sin_theta);
				
				ray_refraction = cos_theta * a + sin_theta * b;
			
				//ray_refraction = ray;
				finalColor = finalColor + RecursiveShade(NULL, intersection.hit_position + TOO_CLOSE * ray, 
					intersection.hit_position + ray_refraction, env, level-1, attenuation,depth);
			}
		}
		
						
	}
	return finalColor;	
	
}

Vector3d getUVW(Vector3d x, Vector3d pp1, Vector3d pp2, Vector3d pp3)
{
	Vector3d a,b,temp;
	double area, u,v,w;
	a = pp3 - pp1;
	b = pp2 - pp1;
	temp = a%b;
	area = temp.norm(); // ignore the 0.5 * norm()
	if(area<SMALL_VALUE)return FAR_FAR_AWAY;

	a = pp3 - x;
	b = pp2 - x;
	temp = a%b;
	u = temp.norm()/area;
	
	a = pp3 - x;
	b = pp1 - x;
	temp = a%b;
	v = temp.norm()/area;
	
	a = pp1 - x;
	b = pp2 - x;
	temp = a%b;
	w = temp.norm()/area;
	
	return Vector3d(u,v,w);	

}
Collision xy_environment::objTest(BIH *node, Vector3d start, Vector3d end, double dis_max) // please test the planes before doing triangle test
{
	
	Collision result;
	if(node == NULL || dis_max < 10 * TOO_CLOSE)return result;
	Collision temp;
	Vector3d direction = end - start;
	Ray r(start, direction.normalize());
	if(node->box.intersect(r,0,dis_max) == true) // intersect with box
	{
		if(node->lchild!=NULL && node->rchild != NULL) // test children
		{
			BIH *c1, *c2;
			if(r.direction[node->axis]>0)
			{
				c1 = node->lchild;
				c2 = node->rchild;
			}
			else
			{		
				c1 = node->rchild;
				c2 = node->lchild;
			}
			temp = objTest(c1, start, end, dis_max);
			if(temp.hit_distance > 10.0*TOO_CLOSE && temp.hit_distance < result.hit_distance)
			{
				result = temp;
				dis_max = temp.hit_distance;
			}
			Vector3d left_bound = node->lchild->box.bounds[1];
			Vector3d right_bound = node->rchild->box.bounds[0];
			if(left_bound[node->axis] > right_bound[node->axis] || temp.hit_position == FAR_FAR_AWAY) // interupt or no intersection
			{
				temp = objTest(c2, start, end, dis_max);
				if(temp.hit_distance > 10.0*TOO_CLOSE && temp.hit_distance < result.hit_distance)
				{
					result = temp;
				}
			}
		

		}
		else // test triangles
		{
		
			for(int i=0; i < node->objs.size(); i++)
			{
				int id = node->objs[i];
				
				if(objects[id]->obj_name == "triangle")
				{
					temp = objects[id]->findIntersection(start, end);
					if(temp.hit_distance > 10.0*TOO_CLOSE && temp.hit_distance < result.hit_distance)
					{
						result = temp;
					}
				}	
			}
		}
	}

	return result;	
}
/*
	Search the BIH tree for an intersection, which may not be the nearest. Once it found one, it returns a collision result structure.
*/
Collision xy_environment::quickTest(BIH *node, Vector3d start, Vector3d end, double dis_max) // please test the planes before doing triangle test
{
	Collision result;
	if(node == NULL)return result;
	Collision temp;
	
	Vector3d dis = end - start;
	Ray r(start, dis.normalize());
	if(node->box.intersect(r,0,dis_max) == true) // intersect with box
	{
		if(node->lchild!=NULL && node->rchild != NULL) // test children
		{
			BIH *c1, *c2;
				c1 = node->lchild;
				c2 = node->rchild;
			temp = quickTest(c1, start, end, dis_max);
			if(temp.hit_position == FAR_FAR_AWAY)
			{
				temp = quickTest(c2, start, end, dis_max);
			}
			return temp;
		

		}
		else // test triangles
		{
			for(int i=0; i < node->objs.size(); i++)
			{
				int id = node->objs[i];
				
				if(objects[id]->obj_name == "triangle")
				{
					temp = objects[id]->findIntersection(start, end);
					if(temp.hit_distance > 10*TOO_CLOSE && temp.hit_distance < result.hit_distance)
					{
						return temp; // return it immidiately without further test closer intersections
					}
				}	
			}
		}
	}
	else return result;

	return result;	
}

void xy_environment::cutTree(BIH *node)
{
	if(node->objs.size()<3)return;
	vector<xy_object*> obj_vec;
	int i;
	obj_vec.reserve(node->objs.size()+1);
	for(i = 0; i < node->objs.size(); i++)
	{
		int id = node->objs[i];
		obj_vec.push_back(objects[id]);		
	}
	int ax = node->axis;
	switch(ax)
	{
		case 0: std::sort(obj_vec.begin(),obj_vec.end(),sortByX());break;
		case 1: std::sort(obj_vec.begin(),obj_vec.end(),sortByY());break;
		case 2: std::sort(obj_vec.begin(),obj_vec.end(),sortByZ());break;
	}
	ax = (ax+1)%3;
//	ax = 0;
//	Vector3d temp = node->box.bounds[1] - node->box.bounds[0];
//	if(temp.y>temp.x && temp.y>temp.z)ax = 1;
//	if(temp.z>temp.x && temp.z>temp.y)ax = 2;
	BIH *lc,*rc;
	//build left tree
	lc = new BIH();
	lc->objs.reserve(node->objs.size()/2+1);
      	lc->axis = ax;
	lc->lchild = NULL;
	lc->rchild = NULL;
	for(i = 0; i < obj_vec.size()/2; i++)
	{
		lc->objs.push_back(obj_vec[i]->obj_id);
		if(obj_vec[i]->obj_name == "triangle")
		{
			lc->box.fixBox(  obj_vec[i]->getMin());
			lc->box.fixBox(  obj_vec[i]->getMax());
		}
	}
	//build right tree
	rc = new BIH();
	rc->objs.reserve(node->objs.size()/2+1);
      	rc->axis = ax;
	rc->lchild = NULL;
	rc->rchild = NULL;
	for(i = obj_vec.size()/2; i < obj_vec.size(); i++)
	{
		rc->objs.push_back(obj_vec[i]->obj_id);
		if(obj_vec[i]->obj_name == "triangle")
		{
			rc->box.fixBox(  obj_vec[i]->getMin());
			rc->box.fixBox(  obj_vec[i]->getMax());
		}
	}
	std::vector<int>::iterator iter = node->objs.begin();
	//clear the old data
	while(iter!=node->objs.end()){iter = node->objs.erase(iter);}
	node->lchild = lc;
	node->rchild = rc;
	
	cutTree(lc);
	
	cutTree(rc);
	

}


Matrix4x4 xy_translate(Matrix4x4 m, Vector3d t)
{
	Matrix4x4 r;
	r.set
	(
		1,0,0,t[0],
		0,1,0,t[1],
		0,0,1,t[2],
		0,0,0,1
	);
	r = r * m;
	return r;
}

Matrix4x4 xy_scale(Matrix4x4 m, Vector3d t)
{
	Matrix4x4 r;
	r.set
	(
		t[0],0,0,0,
		0,t[1],0,0,
		0,0,t[2],0,
		0,0,0,1
	);
	r = r * m;
	return r;
}

Matrix4x4 xy_rotateX(Matrix4x4 m, double angle)
{
	Matrix4x4 r;
	r.set
	(
		1, 0,          0,             0,
		0, cos(angle), -1*sin(angle), 0,
		0, sin(angle), cos(angle),    0,
		0, 0,          0,             1
	);
	r = r * m;
	return r;
}

Matrix4x4 xy_rotateY(Matrix4x4 m, double angle)
{
	Matrix4x4 r;
	r.set
	(
		sin(angle), 0, cos(angle),  0,
		0,          1, 0,           0,
		cos(angle), 0, -sin(angle), 0,
		0, 0,          0,           1
	);
	r = r * m;
	return r;
}

Matrix4x4 xy_rotateZ(Matrix4x4 m, double angle)
{
	Matrix4x4 r;
	r.set
	(
		cos(angle), -sin(angle), 0, 0,
		sin(angle), cos(angle),  0, 0,
		0,          0,           1, 0,
		0,          0,           0, 1
	);
	r = r * m;
	return r;
}



