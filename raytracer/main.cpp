/***********************************************************
Starter code for Assignment 3

This code was originally written by Jack Wang for
CSC418, SPRING 2005

***********************************************************/


#include "raytracer.h"
#include <cstdlib>

int main(int argc, char* argv[])
{
	// Build your scene and setup your camera here, by calling 
	// functions from Raytracer.  The code here sets up an example
	// scene and renders it from two different view points, DO NOT
	// change this if you're just implementing part one of the 
	// assignment.  
	Raytracer raytracer;
	int width = 320;
	int height = 240;

	if (argc == 3) {
		width = atoi(argv[1]);
		height = atoi(argv[2]);
	}

	// Define materials for shading
	Material gold(Colour(0.3, 0.3, 0.3), 
		Colour(0.75164, 0.60648, 0.22648),
		Colour(0.628281, 0.555802, 0.366065),
		51.2, 1, false);

	Material jade(Colour(0, 0, 0), 
		Colour(0.54, 0.89, 0.63),
		Colour(0.316228, 0.316228, 0.316228),
		12.8, 1, false);

	Material red(Colour(0.1, 0.0, 0.0), 
		Colour(0.4, 0.4, 0.4),
		Colour(0.6, 0.05, 0.05),
		1, 1, false);

	Material blue(Colour(0.0, 0.0, 0.1), 
		Colour(0.4, 0.4, 0.4),
		Colour(0.05, 0.05, 0.6),
		1, 1, false);

	Material white(Colour(0.01, 0.01, 0.01), 
		Colour(0.5, 0.5, 0.5),
		Colour(0.5, 0.5, 0.5),
		1, 1, false);

	Material silver(Colour(0.19125, 0.19125, 0.19125), 
		Colour(0.50754, 0.50754, 0.50754),
		Colour(0.508273, 0.508273, 0.508273),
		100, 1.3, false);

	Material glass(Colour(0.001, 0.001, 0.001), 
		Colour(0.0, 0.0, 0.0),
		Colour(0.999, 0.999, 0.999),
		10000, 1.5, true);

	Material glossyMirror(Colour(0.01, 0.01, 0.01), 
		Colour(0.1, 0.1, 0.1),
		Colour(0.9, 0.9, 0.9),
		1000, 1.5, false);

		Point3D eye(0, 2, 10);
		Vector3D view(0, 0, -1);
		Vector3D up(0, 1, 0);
		double fov = 60;

		// Defines a point light source.
		raytracer.addLightSource(new PointLight(Point3D(0, 6, 3),
			Colour(0.2, 0.2, 0.2), Colour(0.8, 0.8, 0.8), Colour(0.8, 0.8, 0.8)));

		raytracer.addLightSource(new PointLight(Point3D(-4, 0, -2),
			Colour(0.1, 0.1, 0.1), Colour(0.9, 0.9, 0.9), Colour(0.9, 0.9, 0.9)));

		// Construct scene
		SceneDagNode* floor = raytracer.addObject(new UnitSquare(), &white);
		SceneDagNode* ceiling = raytracer.addObject(new UnitSquare(), &glossyMirror);
		SceneDagNode* leftWall = raytracer.addObject(new UnitSquare(), &gold);
		SceneDagNode* rightWall = raytracer.addObject(new UnitSquare(), &gold);
		SceneDagNode* backWall = raytracer.addObject(new UnitSquare(), &red);
		// SceneDagNode* cylinder = raytracer.addObject(new UnitCylinder(), &gold);
		SceneDagNode* glassSphere = raytracer.addObject(new UnitSphere(), &blue);

		// Apply transformations
		double wallScale[3] = { 100.0, 100.0, 100.0 };
		raytracer.translate(floor, Vector3D(0, -3, 0));
		raytracer.rotate(floor, 'x', -90);
		raytracer.scale(floor, Point3D(0, 0, 0), wallScale);

		raytracer.translate(backWall, Vector3D(0, 0, -7));
		raytracer.scale(backWall, Point3D(0, 0, 0), wallScale);

		raytracer.translate(leftWall, Vector3D(-7, 0, 0));
		raytracer.rotate(leftWall, 'y', 90);
		raytracer.scale(leftWall, Point3D(0, 0, 0), wallScale);

		raytracer.translate(rightWall, Vector3D(7, 0, 0));
		raytracer.rotate(rightWall, 'y', -90);
		raytracer.scale(rightWall, Point3D(0, 0, 0), wallScale);

		raytracer.translate(ceiling, Vector3D(0, 7, 0));
		raytracer.rotate(ceiling, 'x', 90);
		raytracer.scale(ceiling, Point3D(0, 0, 0), wallScale);

		// double cylinderScale[3] = { 1.5, 2.0, 1.5 };
		// raytracer.translate(cylinder, Vector3D(-4, -2, -4));
		// raytracer.scale(cylinder, Point3D(0, 0, 0), cylinderScale);

		double sphereScale[3] = { 2.0, 2.0, 2.0 };

		raytracer.translate(glassSphere, Vector3D(1, -0.9, -0.5));
		raytracer.scale(glassSphere, Point3D(0, 0, 0), sphereScale);


		// SceneDagNode* ground = raytracer.addObject(new UnitSquare(), &silver);
		// SceneDagNode* roof = raytracer.addObject(new UnitSquare(), &gold);
		// SceneDagNode* wall1 = raytracer.addObject(new UnitSquare(), &glossyMirror);
		// SceneDagNode* wall2 = raytracer.addObject(new UnitSquare(), &red);
		// SceneDagNode* wall3 = raytracer.addObject(new UnitSquare(), &silver);

		// double groundScale[3] = { 30, 30, 100 };
		// raytracer.rotate(ground, 'x', -90);
		// raytracer.scale(ground, Point3D(), groundScale);

		// double wallScale[3] = { 100.0, 100.0, 100.0 };
		// raytracer.translate(wall3, Vector3D(0, 0, -7));
		// raytracer.scale(wall3, Point3D(0, 0, 0), wallScale);

		// raytracer.translate(wall1, Vector3D(-7, 0, 0));
		// raytracer.rotate(wall1, 'y', 90);
		// raytracer.scale(wall1, Point3D(0, 0, 0), wallScale);

		// raytracer.translate(wall2, Vector3D(7, 0, 0));
		// raytracer.rotate(wall2, 'y', -90);
		// raytracer.scale(wall2, Point3D(0, 0, 0), wallScale);

		// raytracer.translate(roof, Vector3D(0, 7, 0));
		// raytracer.rotate(roof, 'x', 90);
		// raytracer.scale(roof, Point3D(0, 0, 0), wallScale);

		// SceneDagNode* glassSphere = raytracer.addObject(new UnitSphere(), &glass);
		// raytracer.translate(glassSphere, Vector3D(0.4, 0.0, -2.0));

		// // SceneDagNode* cylinder = raytracer.addObject(new UnitCylinder(), &red);
		// // double cylinderScale[3] = { 0.8, 2.0, 0.8 };
		// // raytracer.translate(cylinder, Vector3D(2.0, 2, -1));
		// // raytracer.scale(cylinder, Point3D(), cylinderScale);

	raytracer.render(width, height, eye, view, up, fov, "scene.bmp");
	return 0;
}