/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		Implementations of functions in raytracer.h, 
		and the main function which specifies the 
		scene to be rendered.	

***********************************************************/


#include "raytracer.h"
#include "bmp_io.h"
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <ctime>
Raytracer::Raytracer() : _root(new SceneDagNode()) {

}

Raytracer::~Raytracer() {
	// Clean up

	// Delete light sources
	for (int i = 0; i < _lightSource.size(); i++)
	{
		delete _lightSource[i];
	}

	delete _root;
}

SceneDagNode* Raytracer::addObject( SceneDagNode* parent, 
		SceneObject* obj, Material* mat ) {
	SceneDagNode* node = new SceneDagNode( obj, mat );
	node->parent = parent;
	node->next = NULL;
	node->child = NULL;
	
	// Add the object to the parent's child list, this means
	// whatever transformation applied to the parent will also
	// be applied to the child.
	if (parent->child == NULL) {
		parent->child = node;
	}
	else {
		parent = parent->child;
		while (parent->next != NULL) {
			parent = parent->next;
		}
		parent->next = node;
	}
	
	return node;;
}

void Raytracer::addLightSource( LightSource* light ) {
	_lightSource.push_back(light);
}

void Raytracer::rotate( SceneDagNode* node, char axis, double angle ) {
	Matrix4x4 rotation;
	double toRadian = 2*M_PI/360.0;
	int i;
	
	for (i = 0; i < 2; i++) {
		switch(axis) {
			case 'x':
				rotation[0][0] = 1;
				rotation[1][1] = cos(angle*toRadian);
				rotation[1][2] = -sin(angle*toRadian);
				rotation[2][1] = sin(angle*toRadian);
				rotation[2][2] = cos(angle*toRadian);
				rotation[3][3] = 1;
			break;
			case 'y':
				rotation[0][0] = cos(angle*toRadian);
				rotation[0][2] = sin(angle*toRadian);
				rotation[1][1] = 1;
				rotation[2][0] = -sin(angle*toRadian);
				rotation[2][2] = cos(angle*toRadian);
				rotation[3][3] = 1;
			break;
			case 'z':
				rotation[0][0] = cos(angle*toRadian);
				rotation[0][1] = -sin(angle*toRadian);
				rotation[1][0] = sin(angle*toRadian);
				rotation[1][1] = cos(angle*toRadian);
				rotation[2][2] = 1;
				rotation[3][3] = 1;
			break;
		}
		if (i == 0) {
		    node->trans = node->trans*rotation; 	
			angle = -angle;
		} 
		else {
			node->invtrans = rotation*node->invtrans; 
		}	
	}
}

void Raytracer::translate( SceneDagNode* node, Vector3D trans ) {
	Matrix4x4 translation;
	
	translation[0][3] = trans[0];
	translation[1][3] = trans[1];
	translation[2][3] = trans[2];
	node->trans = node->trans*translation; 	
	translation[0][3] = -trans[0];
	translation[1][3] = -trans[1];
	translation[2][3] = -trans[2];
	node->invtrans = translation*node->invtrans; 
}

void Raytracer::scale( SceneDagNode* node, Point3D origin, double factor[3] ) {
	Matrix4x4 scale;
	
	scale[0][0] = factor[0];
	scale[0][3] = origin[0] - factor[0] * origin[0];
	scale[1][1] = factor[1];
	scale[1][3] = origin[1] - factor[1] * origin[1];
	scale[2][2] = factor[2];
	scale[2][3] = origin[2] - factor[2] * origin[2];
	node->trans = node->trans*scale; 	
	scale[0][0] = 1/factor[0];
	scale[0][3] = origin[0] - 1/factor[0] * origin[0];
	scale[1][1] = 1/factor[1];
	scale[1][3] = origin[1] - 1/factor[1] * origin[1];
	scale[2][2] = 1/factor[2];
	scale[2][3] = origin[2] - 1/factor[2] * origin[2];
	node->invtrans = scale*node->invtrans; 
}

Matrix4x4 Raytracer::initInvViewMatrix( Point3D eye, Vector3D view, 
		Vector3D up ) {
	Matrix4x4 mat; 
	Vector3D w;
	view.normalize();
	up = up - up.dot(view)*view;
	up.normalize();
	w = view.cross(up);

	mat[0][0] = w[0];
	mat[1][0] = w[1];
	mat[2][0] = w[2];
	mat[0][1] = up[0];
	mat[1][1] = up[1];
	mat[2][1] = up[2];
	mat[0][2] = -view[0];
	mat[1][2] = -view[1];
	mat[2][2] = -view[2];
	mat[0][3] = eye[0];
	mat[1][3] = eye[1];
	mat[2][3] = eye[2];

	return mat; 
}


void Raytracer::computeTransforms( SceneDagNode* node )
{
    SceneDagNode *childPtr;
    if (node->parent != NULL )
    {
        node->modelToWorld = node->parent->modelToWorld*node->trans;
        node->worldToModel = node->invtrans*node->parent->worldToModel; 
    }
    else
    {
        node->modelToWorld = node->trans;
        node->worldToModel = node->invtrans; 
    }
    // Traverse the children.
    childPtr = node->child;
    while (childPtr != NULL) {
        computeTransforms(childPtr);
        childPtr = childPtr->next;
    }
}

void Raytracer::traverseScene( SceneDagNode* node, Ray3D& ray, Matrix4x4 &_modelToWorld, Matrix4x4 &_worldToModel ) {
	SceneDagNode *childPtr;

	// Applies transformation of the current node to the global
	// transformation matrices.
	Matrix4x4 modelToWorld = _modelToWorld*node->trans;
	Matrix4x4 worldToModel = node->invtrans*_worldToModel; 
	if (node->obj) {
		// Perform intersection.
		if (node->obj->intersect(ray, worldToModel, modelToWorld)) {
			ray.intersection.mat = node->mat;
		}
	}
	// Recusive on the children.
	childPtr = node->child;
	while (childPtr != NULL) {
		traverseScene(childPtr, ray, modelToWorld, worldToModel);
		childPtr = childPtr->next;
	}
}

void Raytracer::computeShading( Ray3D& ray ) {

	Colour color(0, 0, 0);

	for (int i = 0; i < _lightSource.size(); i++)
	{
		LightSource *curLight = _lightSource[i];
		Colour curShade = curLight->shade(ray);
		// Soft shadow
		double theta = 2 * M_PI * generateRandom();
		Vector3D vectorOffset(cos(theta), 0, sin(theta));
		Point3D randomLight = curLight->get_position() + generateRandom() * vectorOffset;
		// Shadow direction
		Vector3D shadowDir = randomLight - ray.intersection.point;
		double lightDistance = shadowDir.length();
		shadowDir.normalize();
		// Shadow origin
		Point3D shadowOrig = ray.intersection.point + 1e-5 * shadowDir;
		// Shadow ray
		Ray3D shadowRay(shadowOrig, shadowDir);
		// Traverse scene
		traverseScene(_root, shadowRay, _modelToWorld, _worldToModel);
		// Check if it's a shadow ray
		if (!shadowRay.intersection.none && shadowRay.intersection.t_value < lightDistance)
		{
			continue;
		}
		
		color = color + curShade;
	}
	color.clamp();
	ray.col = color;
}

void Raytracer::initPixelBuffer() {
    int numbytes = _scrWidth * _scrHeight * sizeof(unsigned char);
    _rbuffer = new unsigned char[numbytes];
    std::fill_n(_rbuffer, numbytes,0);
    _gbuffer = new unsigned char[numbytes];
    std::fill_n(_gbuffer, numbytes,0);
    _bbuffer = new unsigned char[numbytes];
    std::fill_n(_bbuffer, numbytes,0);
}

void Raytracer::flushPixelBuffer( std::string file_name ) {
    bmp_write( file_name.c_str(), _scrWidth, _scrHeight, _rbuffer, _gbuffer, _bbuffer );
    delete _rbuffer;
    delete _gbuffer;
    delete _bbuffer;
}


Colour Raytracer::shadeRay( Ray3D& ray, int rayDepth ) {
	Colour col(0.0, 0.0, 0.0); 

	traverseScene(_root, ray, _modelToWorld, _worldToModel); 
	
	// Don't bother shading if the ray didn't hit 
	// anything.
	if (!ray.intersection.none) {

		computeShading(ray);
		col = ray.col;

		// You'll want to call shadeRay recursively (with a different ray, 
		// of course) here to implement reflection/refraction effects.  

		// Ray not reach to all depths
		if (rayDepth > 0){
			Vector3D norm = ray.intersection.normal;

			// Incidence direction
			Vector3D I = -ray.dir;
			double R = 1.0;
			// Refraction
			if (ray.intersection.mat->transmissive){
				double norm1 = 1.0;
				double norm2 = 1.0;
				// Refraction color
				Colour refrColor(0, 0, 0);
				if (I.dot(norm) > 0){
					norm2 = ray.intersection.mat->refracIndex;
				}
				else{
					norm1 = ray.intersection.mat->refracIndex;
					norm = -norm;
				}
				// Refraction direction
				double n = norm1 / norm2;
				double cosI = norm.dot(I);
				double sinT2 = n * n * (1.0 - cosI * cosI);
				if (sinT2 <= 1){
					double cosT = std::sqrt(1.0 - sinT2);
					Vector3D refractionDir = n * I - (n * cosI + cosT) * norm;
					refractionDir.normalize();
					Point3D refractionOrig = ray.intersection.point + 1e-5 * refractionDir;
					Ray3D refractionRay(refractionOrig, refractionDir);
					double s1 = norm1 * cosI - norm2 * cosT;
					double s2 = norm1 * cosI + norm2 * cosT;
					double Rs = s1 / s2;
					double p1 = norm1 * cosT - norm2 * cosI;
					double p2 = norm1 * cosT + norm2 * cosI;
					double Rp = p1 / p2;
					double r = Rs * Rs + Rp * Rp;
					double R = r / 2.0;
					refrColor = (1.0 - R) * shadeRay(refractionRay, rayDepth - 1);
				}
				// Refraction color
				col = col + refrColor;
			}

			// Primary reflection direction
			Vector3D reflectionDir = 2 * norm.dot(I) * norm - I;
			reflectionDir.normalize();
			Point3D reflectionOrg = ray.intersection.point + 1e-5 * reflectionDir;
			// Reflection color
			Colour reflColor(0, 0, 0);
			// Glossy reflection
			Vector3D u = reflectionDir.cross(norm);
			u.normalize();
			Vector3D v = reflectionDir.cross(u);
			v.normalize();
			// Create random ray direction
			Ray3D refl_ray;
			refl_ray.origin = reflectionOrg;
			// Random sample
			double the = 1.0 / (ray.intersection.mat->specular_exp + 1);
			double theta = acos(pow(generateRandom(), the));
			double phi = 2 * M_PI * generateRandom();
			double x = sin(theta) * cos(phi);
			double y = sin(theta) * sin(phi);
			double z = cos(theta);
			refl_ray.dir = x * u + y * v + z * reflectionDir;
			refl_ray.dir.normalize();
			// Reflection color
			reflColor = reflColor + shadeRay(refl_ray, rayDepth - 1);
			col = col + R * ray.intersection.mat->specular * reflColor;
		}
	}
	col.clamp();
	return col; 
}	

void Raytracer::render( int width, int height, Point3D eye, Vector3D view, Vector3D up, double fov, std::string fileName){
	Matrix4x4 viewToWorld;
	_scrWidth = width;
	_scrHeight = height;
	int rayDepth = 6;
	double a1 = 0.6;
	double f2 = 5.5;
	double g1 = 2.2;
	int s1 = 1;
	initPixelBuffer();
	viewToWorld = initInvViewMatrix(eye, view, up);
	Point3D origin(0, 0, 0);
	
	// Anti-Aliasing and depth of field
	// Divide a single pixel into 2x2 sub-pixels
	int subPixelsEachDim = 4;
	int totalPixels = subPixelsEachDim * subPixelsEachDim;
	// Find width, weight, and factor
	double subPixWidth = 1.0 / subPixelsEachDim;
	double weights = 1.0 / (totalPixels * s1);
	double factor = (height / 2.0) / tan(fov * M_PI / 360.0);
	for (int y = 0; y < _scrHeight; y++){
		// Print progress
		fprintf(stderr, "\rRendering process %5.2f%%",100.0 * y / (_scrHeight - 1));
		for (int x = 0; x < _scrWidth; x++){
			// buffer offset
			int i = y * width + x;
			// Color at pixel (x, y)
			Colour color;
			// normalize all the shades
			for (int sy = 0; sy < subPixelsEachDim; sy++){
				for (int sx = 0; sx < subPixelsEachDim; sx++){
					for (int s = 0; s < s1; s++){
						// Sets up image plane (z = -1)
						double dy = (sy + generateRandom()) * subPixWidth;
						double dx = (sx + generateRandom()) * subPixWidth;
						Point3D imagePlane(
							(-width / 2.0 + x + dx) / factor,
							(-height / 2.0 + y + dy) / factor,
							-1.0
							);

						// Create ray origin randomly
						double phi = 2 * M_PI * generateRandom();
						double r = a1 * sqrt(generateRandom()) / 2.0;
						Point3D castOrigin(r * cos(phi), r * sin(phi), 0);
						Vector3D direct = imagePlane - origin;
						direct.normalize();
						double t = -f2 / direct[2];
						Point3D focal = origin + t * direct;
						Vector3D castDir = focal - castOrigin;
						// Create ray
						Ray3D castRay;
						castRay.origin = viewToWorld * castOrigin;
						castRay.dir = viewToWorld * castDir;
						castRay.dir.normalize();
						// Assign color
						color = color + shadeRay(castRay, rayDepth);
					}
				}
			}
			color = color * Colour(weights,weights,weights);
			color.clamp();
			// Update pixel buffer
			_rbuffer[i] = static_cast<unsigned char>(pow(color[0], 1 / g1) * 255.0 + 0.5);
			_gbuffer[i] = static_cast<unsigned char>(pow(color[1], 1 / g1) * 255.0 + 0.5);
			_bbuffer[i] = static_cast<unsigned char>(pow(color[2], 1 / g1) * 255.0 + 0.5);
		}
	}

	// Flush buffer, save image to file
	flushPixelBuffer(fileName);
}
