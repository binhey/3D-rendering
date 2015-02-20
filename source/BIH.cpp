#include "BIH.h"
// Optimized method
bool Box::intersect(const Ray &r, float t0, float t1) const {
	float tmin, tmax, tymin, tymax, tzmin, tzmax;
	if (r.direction.x >= 0) {
	tmin = (bounds[0].x - r.origin.x) / r.direction.x;
	tmax = (bounds[1].x - r.origin.x) / r.direction.x;
	}
	else {
	tmin = (bounds[1].x - r.origin.x) / r.direction.x;
	tmax = (bounds[0].x - r.origin.x) / r.direction.x;
	}
	if (r.direction.y >= 0) {
	tymin = (bounds[0].y - r.origin.y) / r.direction.y;
	tymax = (bounds[1].y - r.origin.y) / r.direction.y;
	}
	else {
	tymin = (bounds[1].y - r.origin.y) / r.direction.y;
	tymax = (bounds[0].y - r.origin.y) / r.direction.y;
	}
	if ( (tmin > tymax) || (tymin > tmax) )
	return false;
	if (tymin > tmin)
	tmin = tymin;
	if (tymax < tmax)
	tmax = tymax;
	if (r.direction.z >= 0) {
	tzmin = (bounds[0].z - r.origin.z) / r.direction.z;
	tzmax = (bounds[1].z - r.origin.z) / r.direction.z;
	}
	else {
	tzmin = (bounds[1].z - r.origin.z) / r.direction.z;
	tzmax = (bounds[0].z - r.origin.z) / r.direction.z;
	}
	if ( (tmin > tzmax) || (tzmin > tmax) )
	return false;
	if (tzmin > tmin)
	tmin = tzmin;
	if (tzmax < tmax)
	tmax = tzmax;
	return ( (tmin < t1) && (tmax > t0) );

	
}

