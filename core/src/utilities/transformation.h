#pragma once

#include "types.h"
#include <math.h>

class transformation
{
public:
	transformation(transformation* tr = nullptr) : tr(tr) {}

	~transformation() {
		if (tr != nullptr)
			delete tr;
	}

	virtual void transform(
		const vector3& origin,
		const vector3& direction
	) = 0;

	virtual void inverse(
		const vector3& origin,
		const vector3& direction
	) = 0;

	transformation* tr;
};

class TrMove : public transformation {
public:
	TrMove(vector3 c, transformation* tr) : transformation(tr), c(c) {}

	void transform(
		const vector3& origin,
		const vector3& direction
	) {
		if (tr != nullptr) 
			tr->transform(origin, direction);

		//origin -= c;
	};

	void inverse(
		const vector3& origin,
		const vector3& direction
	) {
		//origin += c;

		if (tr != nullptr) 
			tr->inverse(origin, direction);
	};

private:
	vector3 c;
};

class TrScale : public transformation {
public:
	TrScale(vector3 c, transformation* tr) : transformation(tr), c(c) {}

	void transform(
		const vector3& origin,
		const vector3& direction
	) {
		if (tr != nullptr) 
			tr->transform(origin, direction);

		//origin /= c;
		//direction /= c;
	};

	void inverse(
		const vector3& origin,
		const vector3& direction
	) {
		//origin *= c;
		//direction *= c;

		if (tr != nullptr) 
			tr->inverse(origin, direction);
	};

private:
	vector3 c;
};

class TrRotate : public transformation
{
public:
	TrRotate(vector3 angles, transformation* tr) : transformation(tr), angles(angles) { }
	
	void rotate2(double& x, double&y, double angle)
	{
		double x1 = cos(angle)*x - sin(angle)*y;
		double y1 = sin(angle)*x + cos(angle)*y;
		x = x1;
		y = y1;
	}

	void rotate3(vector3& a, vector3 angles)
	{
		rotate2(a.x, a.y, angles.z);
		rotate2(a.x, a.z, angles.y);
		rotate2(a.y, a.z, angles.x);
	}

	void rotate3i(vector3& a, vector3 angles)
	{
		rotate2(a.y, a.z, -angles.x);
		rotate2(a.x, a.z, -angles.y);
		rotate2(a.x, a.y, -angles.z);
	}

	void transform(
		const vector3& origin,
		const vector3& direction
	) {
		if (tr != nullptr) 
			tr->transform(origin, direction);

		vector3 orig = origin;
		vector3 ordirection = origin + direction;

		rotate3(orig, angles);
		rotate3(ordirection, angles);
		
		//origin = orig;
		//direction = (ordirection-origin).identity();
	};

	void inverse(
		const vector3& origin,
		const vector3& direction
	) {
		vector3 orig = origin;
		vector3 ordirection = origin + direction;

		rotate3i(orig, angles);
		rotate3i(ordirection, angles);
		//origin = orig;
		//direction = (ordirection-origin).identity();

		if (tr != nullptr) 
			tr->inverse(origin, direction);
	};

private:
	vector3 angles{};
	vector3 m1{}, m2{}, m3{};
	vector3 n1{}, n2{}, n3{};
};