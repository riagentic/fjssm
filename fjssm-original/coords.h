/*
This file was created by Paul Barvinko (pbarvinko@yahoo.com).
This file is distributed "as is", e.g. there are no warranties 
and obligations and you could use it in your applications on your
own risk. 
Your comments and questions are welcome.
If using in your applications, please mention author in credits for your app.
*/
#ifndef _COORDS_H_
#define _COORDS_H_

#include "math.h"

//helper class
class CCoordinates
{
	public:
		CCoordinates(double _x1, double _x2, double _v1, double _v2);

		//operations
		double x1(){return p_x1;};
		double x2(){return p_x2;};
		double v1(){return p_v1;};
		double v2(){return p_v2;};

		double ConvertToWorld(double value){return XtoW(value);};
		double ConvertToScreen(double value){return WtoX(value);};
		double WtoX(double value);
		double XtoW(double value);
		BOOL IsPointInRangeW(double value);
		BOOL IsPointInRangeX(double value);
		static BOOL IsPointInRange(double value, double mx1, double mx2);

		double WInterval(){return (double)fabs(p_v1-p_v2);};
		double XInterval(){return (double)fabs(p_x1-p_x2);};

		void GetPointsForDelta(double Delta, int* num, double** scrpoints, double** wpoints, int maxexp);
		void DivideInterval(int NMax, int* maxexp, double* Delta, int* num, double** scrpoints, double** wpoints);

		static int GetExponent(double value);
		static double fmin(double x, double y);
		static double fmax(double x, double y);

		double CountGrid(int NMax, double mv1, double mv2);

	protected:
		double p_x1, p_x2, p_v1, p_v2, capacity;
};

#endif