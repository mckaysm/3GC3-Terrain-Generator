#ifndef MATH_2D_H
#define MATH_2D_H

class point2D;
class vec2D;
class point3D;
class vec3D;
class colour;

class point2D{
public:
	float x, y;
	point2D();
	point2D(float xIn, float yIn);

	
	float distance (point2D p2);
	float fastDistance (point2D p2);
	point2D movePoint (vec2D v);

};

class point3D{
public:
	float x, y, z;
	point3D();
	point3D(float xIn, float yIn, float zIn);

	float distance (point3D p2);
	float fastDistance(point3D p2);
	point3D movePoint(vec3D v);

};

class vec3D{
public:
	float x, y, z, mag;
	vec3D();
	vec3D(float xIn, float yIn, float zIn);

	float length();
	vec3D normalize();
	vec3D crossProduct(vec3D p2);
	vec3D createVector(point3D p1, point3D p2);
	vec3D addVector(vec3D p2);
};


class vec2D{
public:
	float x, y, mag;
	vec2D();
	vec2D(float xIn, float yIn);

	float length();
	vec2D normalize();
	vec2D vectorMultiply(float scalar);
	vec2D createVector(point2D p1, point2D p2);

};


class colour{
public:
	colour();
	colour(float rIn, float gIn, float bIn);

	float r, g, b;

};

#endif