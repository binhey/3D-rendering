#ifndef __BIH__
#define __BIH__

#include "Vector.h"
#include <vector>
//function objects





class Ray {
public:
	Ray(Vector3d o, Vector3d d) { // original dierection
	origin = o;
	if(d.norm()<0.00001)d = Vector3d(0.00001,0,0);
	direction = d;
	
	inv_direction = Vector3d(1/d.x, 1/d.y, 1/d.z);
	sign[0] = (inv_direction.x < 0);
	sign[1] = (inv_direction.y < 0);
	sign[2] = (inv_direction.z < 0);
	}
	Vector3d origin;
	Vector3d direction;
	Vector3d inv_direction;
	int sign[3];
};

class Box {
public:
	Box(const Vector3d min, const Vector3d max) {
		if(min.x < max.x)
		{
			bounds[0] = min;
			bounds[1] = max;
		}
		else
		{

			bounds[1] = min;
			bounds[0] = max;
		}
	}
	Box() {
		
			bounds[1] = Vector3d(-9999,-9999,-9999); //initial reverse
			bounds[0] = Vector3d(9999,9999,9999);
	}
	bool intersect(const Ray &, float t0, float t1) const;
	void fixBox(Vector3d p)
	{
		if(p.x<bounds[0].x) bounds[0].x = p.x;
		if(p.y<bounds[0].y) bounds[0].y = p.y;
		if(p.z<bounds[0].z) bounds[0].z = p.z;
		if(p.x>bounds[1].x) bounds[1].x = p.x;
		if(p.y>bounds[1].y) bounds[1].y = p.y;
		if(p.z>bounds[1].z) bounds[1].z = p.z;
	}
	Vector3d bounds[2];
};



class BIH
{
public:
	BIH():lchild(NULL),rchild(NULL),axis(0){}
	~BIH()
	{
		if(lchild!=NULL)delete lchild;
		if(rchild!=NULL)delete rchild;
	}
	int axis;
	std::vector<int> objs;
        Box box;
	BIH *lchild;
	BIH *rchild;

};




#endif
