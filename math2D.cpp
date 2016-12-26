#include <math.h>
#include "math2D.h"

point2D::point2D() {

}

point2D::point2D(float xIn, float yIn){
	x = xIn;
	y = yIn;
}

float point2D::distance(point2D p2){
	float dist = sqrt(pow(p2.x-x,2) + pow(p2.y-y, 2));
	return dist;
}

float point2D::fastDistance(point2D p2){
	float fDist = pow(p2.x-x,2) + pow(p2.y-y, 2);
	return fDist;
}

point2D point2D::movePoint(vec2D v){
	point2D n(x,y);
	n.x = x + v.x;
	n.y = y + v.y;
	return n;

}

point3D::point3D() {

}

point3D::point3D(float xIn, float yIn, float zIn){
	x = xIn;
	y = yIn;
	z = zIn;
}

float point3D::distance(point3D p2){
	float dist = sqrt(pow(p2.x-x,2) + pow(p2.y-y, 2) + pow(p2.z-z, 2));
	return dist;
}

float point3D::fastDistance(point3D p2){
	float fDist = pow(p2.x-x,2) + pow(p2.y-y, 2) + pow(p2.z-z, 2);
	return fDist;
}

point3D point3D::movePoint(vec3D v){
	point3D n(x,y,z);
	n.x = x + v.x;
	n.y = y + v.y;
	n.z = z + v.z;
	return n;

}



vec2D::vec2D(){

}

vec2D::vec2D(float xIn, float yIn){
	x = xIn;
	y = yIn;
	length();
}

float vec2D::length(){
	float L = sqrt((pow(x,2) + pow(y,2)));
	mag = L;
	return L;
}

vec2D vec2D::normalize(){
	vec2D normVec(x,y);
	normVec.x = x/mag;
	normVec.y = y/mag;
	if (normVec.length() == 1){
		return normVec;
	}
	return normVec;
}

vec2D vec2D::vectorMultiply(float scalar){
	vec2D multVec(x,y);
	multVec.x = x*scalar;
	multVec.y = y*scalar;
	multVec.length();
	return multVec;
}

vec2D vec2D::createVector(point2D p1, point2D p2){
	x = p2.x - p1.x;
	y = p2.y - p1.y;
	length();
	vec2D newVec(x,y);
	return newVec;
}

vec3D::vec3D(){

}

vec3D::vec3D(float xIn, float yIn, float zIn){
	x = xIn;
	y = yIn;
	z = zIn;
	length();
}

float vec3D::length(){
	float L = sqrt((pow(x,2) + pow(y,2) + pow(z,2)));
	mag = L;
	return L;
}

vec3D vec3D::normalize(){
	vec3D normVec(x,y,z);
	normVec.x = x/length();
	normVec.y = y/length();
	normVec.z = z/length();
	if (normVec.length() == 1){
		return normVec;
	}
	return normVec;
}

vec3D vec3D::crossProduct(vec3D v2){
	vec3D cross = vec3D(x,y,z);
	cross.x = (y * v2.z) - (z * v2.y);
	cross.y = (z * v2.x) - (x * v2.z);
	cross.z = (x * v2.y) - (y * v2.x);	
	return cross;															
}



vec3D vec3D::createVector(point3D p1, point3D p2){
	x = p2.x - p1.x;
	y = p2.y - p1.y;
	z = p2.z - p1.z;
	length();
	vec3D newVec(x,y,z);
	return newVec;
}

vec3D vec3D::addVector(vec3D v2) {
	vec3D sum = vec3D(x, y, z); 
	sum.x += v2.x;
	sum.y += v2.y;
	sum.z += v2.z;
	return sum;
}

colour::colour(){

}


// int main(int argc, char** argv)
// {
// 	point2D a = point2D(0, 1);
// 	point2D b = point2D(1, 2);
// 	float d = a.distance(b);
// 	printf("Distance between points: %f", d);
// 	return 0;

// }

