#include <tuple>
#include <vector>

#include "terrain.h"
#include <iostream>

using namespace std;


float terrain::func(vector<float> A, vector<float> Omega, float point)
{
	float result = 0.0f;
	for (size_t i = 0; i < A.size() && i < Omega.size(); ++i) {
		result += A[i] * sin(Omega[i] * point);
	}
	return result;
}

vector<tuple<float, float>> terrain::generate_terrain(int width, int height)
{
	int interval = 2;
	int num_points = width / interval;
	vector<tuple<float, float>> terrain_points;
	vector<float> A, Omega;

	A = { 3.0f, 1.5f };
	Omega = { 0.01f, 0.02f };

	for (int i = 0; i < num_points; ++i) {
		float x = i * interval;
		float y = func(A, Omega, x) * 10 + height / 4.0f;
		terrain_points.push_back(make_tuple(x, y));
	}

	return terrain_points;
}
