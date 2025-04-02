#include <TGUI/Backend/SFML-Graphics.hpp>
#include <vector>

#ifndef MATHSUTILS_H
#define MATHSUTILS_H
using namespace std;

class Utils {

	public:
		struct vec3d {
			float x, y, z;
		};
		struct triangle {
			vec3d p[3];

			float light;
			bool grid;
			sf::Color color;
		};
		struct mesh {
			vector<triangle> tris;
		};
		struct mat4x4 {
			float m[4][4] = { 0 };
		};

	    int factorial(int input);
		void MultiplyMatrixVector(vec3d& i, vec3d& o, mat4x4& m);

		long double pi = 3.14159265358979323846264338327950288419716939937510L;

		mat4x4 DefineProjectionMatrix(float ScreenHeight, float ScreenWidth);
		mat4x4 MultiplyMatrix(mat4x4& i, mat4x4& o);
		mat4x4 MatRotationZ(float zRot);
		mat4x4 MatRotationY(float yRot);
		mat4x4 MatRotationX(float xRot);
};

#endif 