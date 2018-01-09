/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		implements light_source.h

***********************************************************/

#include <cmath>
#include "light_source.h"

Colour PointLight::shade( Ray3D& ray ) {
	// TODO: implement this function to fill in values for ray.col 
	// using phong shading.  Make sure your vectors are normalized, and
	// clamp colour values to 1.0.
	//
	// It is assumed at this point that the intersection information in ray 
	// is available.  So be sure that traverseScene() is called on the ray 
	// before this function.  

	Intersection intersect = ray.intersection;
	Material* mat = intersect.mat;
	
	Vector3D n = intersect.normal;
	Vector3D s = (_pos - intersect.point);
	s.normalize();
	Vector3D v = -ray.dir;

	Vector3D r = 2*(n.dot(s))*n - s; 
	double nDOTs = fmax(n.dot(s),0);
	double rDOTv = fmax(r.dot(v),0);
	double rDOTvPOWshiny = pow(rDOTv,mat->specular_exp);
	
	Colour ambient = mat->ambient * _col_ambient; 
	Colour diffuse = nDOTs * (mat->diffuse * _col_diffuse);
	Colour specular = rDOTvPOWshiny * (mat->specular * _col_specular);
	
	Colour col = ray.col;
	col = col + ambient + diffuse + specular;
	return col;
}

