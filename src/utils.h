#include <string>
#include <fstream>
#include <sstream>
#include <streambuf>
#include <iostream>
void read_entire_file(std::string fname, std::string &contents);

#define VNAME(x) 					#x
#define SGN(val) 					((0 < val) - (val < 0))
#define SQ(x) 						(x*x)
#define CLAMP(x, low, high)  		(((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#define INEQ(l,type1,x,type2,h) 	(l type1 x && x type2 h)
#define DEG(x)						(x*M_PI/180)
#define RAD(x)						(x/M_PI*180)
#define FLEQUAL(a,b) 				(std::abs(a - b) < 0.001)
#define FLEQUALTOL(a,b,t) 			(std::abs(a - b) < t)
#define USE(x)						((void) x)
#define INRANGE(x,low,high)			INEQ(low,<=,x,<=,high)
#define IMPLIES(x,y)				(!(x) || y)
#define	_BV(x)						(1 << x)
