/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		implements scene_object.h

***********************************************************/

#include <cmath>
#include <iostream>
#include <math.h>
#include <float.h>
#include "scene_object.h"

bool UnitSquare::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {
	// TODO: implement intersection code for UnitSquare, which is
	// defined on the xy-plane, with vertices (0.5, 0.5, 0), 
	// (-0.5, 0.5, 0), (-0.5, -0.5, 0), (0.5, -0.5, 0), and normal
	// (0, 0, 1).
	//
	// Your goal here is to fill ray.intersection with correct values
	// should an intersection occur.  This includes intersection.point, 
	// intersection.normal, intersection.none, intersection.t_value.   
	//
	// HINT: Remember to first transform the ray into object space  
	// to simplify the intersection test.

	// Ray to object space
	Point3D point = worldToModel * ray.origin;;
	Vector3D vector = worldToModel * ray.dir;
	// find t
	double t = -point[2] / vector[2];
	if (t > 0){
		Point3D intersect = point + t * vector;
		double bound = 0.5;
		bool inBound = std::abs(intersect[0]) <= bound && std::abs(intersect[1]) <= bound;
		bool closerIntersec = ray.intersection.none || t < ray.intersection.t_value;
		// new intersection if in bound and no intersection or further old intersection
		if (inBound && closerIntersec){
			//Set ray intersection
			ray.intersection.t_value = t;
			ray.intersection.point = modelToWorld * intersect;
			ray.intersection.normal = transNorm(worldToModel, Vector3D(0.0, 0.0, 1.0));
			ray.intersection.normal.normalize();
			ray.intersection.none = false;
			return true;
		}
	}
	return false;
}

bool UnitSphere::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {
	// TODO: implement intersection code for UnitSphere, which is centred 
	// on the origin.  
	//
	// Your goal here is to fill ray.intersection with correct values
	// should an intersection occur.  This includes intersection.point, 
	// intersection.normal, intersection.none, intersection.t_value.   
	//
	// HINT: Remember to first transform the ray into object space  
	// to simplify the intersection test.
	
	// Ray to object space
	Point3D point = worldToModel * ray.origin;;
	Vector3D vector = worldToModel * ray.dir;
	// Find quadratic coefficient
	int radius = 1;
	Point3D centre(0.0,0.0,0.0);
	Vector3D centre2point = point - centre;
	double a = vector.dot(vector);
	double b = centre2point.dot(vector);
	double c = centre2point.dot(centre2point) - radius;
	double d = b * b - a * c;
	// May intersect when d >= 0
	if (d >= 0){
		// find t
		double lambda1 = -b / a + std::sqrt(d) / a;
		double lambda2 = -b / a - std::sqrt(d) / a;
		double t;
		if (lambda1 > 0 && lambda2 < 0){
			t = lambda1;
		}
		else {
			t = lambda2;
		}
		// Intersect
		if (t > 0){
			Point3D intersect = point + t * vector;
			bool closerIntersec = ray.intersection.none || t < ray.intersection.t_value;
			if (closerIntersec){
				ray.intersection.t_value = t;
				ray.intersection.point = modelToWorld * intersect;
				ray.intersection.normal = transNorm(worldToModel, intersect - centre);
				ray.intersection.normal.normalize();
				ray.intersection.none = false;
				return true;
			}
		}
	}
	
	return false;
}


bool UnitCylinder::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {

	// Ray to object space
	Point3D point = worldToModel * ray.origin;;
	Vector3D vector = worldToModel * ray.dir;
	// Find quadratic coefficient
	double a = vector[0] * vector[0] + vector[2] * vector[2];
	double b = vector[0] * point[0] + vector[2] * point[2];
	double c = point[0] * point[0] + point[2] * point[2] - 1;
	double d = b * b - a * c;
	// Initialize a ray intersection
	Intersection rayIntersection;
	rayIntersection.none = true;
	rayIntersection.t_value = DBL_MAX;
	// If intersect with the body
	double bound = 0.5;
	if (d >= 0){
		// find t
		double lambda1 = -b / a + std::sqrt(d) / a;
		double lambda2 = -b / a - std::sqrt(d) / a;
		double t;
		if (lambda1 > 0 && lambda2 < 0){
			t = lambda1;
		}
		else {
			t = lambda2;
		}
		// Intersect
		if (t > 0){
			Point3D intersect = point + t * vector;
			if (std::abs(intersect[1]) <= bound){
				rayIntersection.t_value = t;
				rayIntersection.point = modelToWorld * intersect;
				Vector3D normal = Vector3D(intersect[0], 0, intersect[2]);
				rayIntersection.normal = transNorm(worldToModel, normal);
				rayIntersection.normal.normalize();
				rayIntersection.none = false;
				return true;
			}
		}
	}
	// If intersect with top/bottom base
	double t[2] = {
		(0.5 - point[1]) / vector[1], // top
		(-0.5 - point[1]) / vector[1] // bottom
	};
	Vector3D normal[2] = {
		Vector3D(0, 1, 0), // top
		Vector3D(0,-1, 0)  // bottom
	};
	// Check intersect with top and bottom bases
	for (int i=0; i<2; i++){
		if (t[i] > 0 && t[i] < rayIntersection.t_value){
			Point3D intersect = point + t[i] * vector;
			// If x^2 + z^2 < 1
			if (intersect[0] * intersect[0] + intersect[2] * intersect[2] < 1){
				rayIntersection.t_value = t[i];
				rayIntersection.point = modelToWorld * intersect;
				rayIntersection.normal = transNorm(worldToModel, normal[i]);
				rayIntersection.normal.normalize();
				rayIntersection.none = false;
			}
		}
	}
	if (!rayIntersection.none && (ray.intersection.none || rayIntersection.t_value < ray.intersection.t_value)){
		ray.intersection = rayIntersection;
		return true;
	}

	return false;
}





