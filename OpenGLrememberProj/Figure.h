#define _USE_MATH_DEFINES

#include "Render.h"
#include <Windows.h>
#include <math.h>
#include <GL\GL.h>
#include <GL\GLU.h>
#include <iostream>

#pragma once
const double ACCURACY_CIRCLE = 180;
const double k = 0.2;

class Figure
{
public:
private: 
	static double* getVertexNormale(double* vertexA, double* vertexB, double* vertexC, bool reverce = false) {
		if (reverce) {
			return getVertexNormale(vertexC, vertexB, vertexA);
		}
		double* vectorA = new double[] {
			vertexB[0] - vertexA[0],
			vertexB[1] - vertexA[1],
			vertexB[2] - vertexA[2]
		};
		double* vectorB = new double[] {
			vertexC[0] - vertexA[0],
			vertexC[1] - vertexA[1],
			vertexC[2] - vertexA[2]
		};

		return new double[] {
			vectorA[1] * vectorB[2] - vectorB[1] * vectorA[2],
			-vectorA[0] * vectorB[2] + vectorB[0] * vectorA[2],
			vectorA[0] * vectorB[1] - vectorB[0] * vectorA[1],
		};
	}

	static double* getTextureCoord(double* vertex) {
		return new double[] {
			vertex[0] * k, vertex[1] * k
		};
	}

	// Нахождение длины вектора 
	static double getLenght(double* A, double* B) {
		return std::sqrt(std::pow(B[0] - A[0], 2) + std::pow(B[1] - A[1], 2));
	}

	// Нахождение центра окружности по 3 точкам
	static double* getCenterCircle(double* vertexA, double* vertexB, double* vertexC) {
		double A, B, C, D, E, F, G;

		A = vertexB[0] - vertexA[0];
		B = vertexB[1] - vertexA[1];
		C = vertexC[0] - vertexA[0];
		D = vertexC[1] - vertexA[1];
		E = A * (vertexA[0] + vertexB[0]) + B * (vertexA[1] + vertexB[1]);
		F = C * (vertexA[0] + vertexC[0]) + D * (vertexA[1] + vertexC[1]);
		G = 2 * (A * (vertexC[1] - vertexB[1]) - B * (vertexC[0] - vertexB[0]));

		return new double[] {
			(D * E - B * F) / G,
			(A * F - C * E) / G,
			vertexA[2]
		};
	}

	// Нахождение точки N для дальнейшего нахождения угла 
	static double getStartN(double* startVertex, double* paramCircle) {
		return (std::acos((startVertex[0] - paramCircle[0]) / paramCircle[3]) * ACCURACY_CIRCLE) / -M_PI;
	}

	// Нахождение координат точки на окружности
	static double* getVertexCircle(double x, double y, double z, double r, double n) {
		double angle = -M_PI * n / ACCURACY_CIRCLE;
		double dx = r * cos(angle) + x;
		double dy = r * sin(angle) + y;
		double* A = new double[] { dx, dy, z };

		// dx = r * cos(angle) + x
		// dx - x = r * cos(angle)
		// cos(angle) = (dx - x) / r
		// angle = acos((dx - x) / r)
		// n = (acos((dx - x) / r) * step) / -M_PI

		return A;
	}

	// Построение полуокружности по точкам
	static void render_circle(double x, double y, double z, double r, double* baseVertex, double* startVertex, double* endVertex, bool normaleRevert) {
		
		double startI = getStartN(startVertex, new double[] {x, y, z, r});
		double endI = getStartN(endVertex, new double[] {x, y, z, r});
		for (double i = std::abs(startI); i < std::abs(endI); i += 0.1)
		{
			double* A = getVertexCircle(x, y, z, r, i);
			double* B = getVertexCircle(x, y, z, r, i + 1);

			glNormal3dv(getVertexNormale(B, baseVertex, A, normaleRevert));
			glTexCoord2dv(getTextureCoord(A));
			glVertex3dv(A);
			glTexCoord2dv(getTextureCoord(B));
			glVertex3dv(B);
			glTexCoord2dv(getTextureCoord(baseVertex));
			glVertex3dv(baseVertex);
		}
	}

	static void render_rectangle(double* A, double* B, double h = 0) {
		double C[] = { B[0], B[1], h };
		double D[] = { A[0], A[1], h };
		glBegin(GL_QUADS);

		glNormal3dv(getVertexNormale(A, B, C));
		glVertex3dv(A);
		glVertex3dv(B);
		glVertex3dv(C);
		glVertex3dv(D);

		glEnd();
	}

	// Построение стенки для выпуклости
	static void render_rectangle_convex(double z) {

		glBegin(GL_QUADS);
		for (int i = 0; i < 180; i++)
		{
			double* A = getVertexCircle(3.5, 0, 0, 1.5, i);
			double* B = getVertexCircle(3.5, 0, 0, 1.5, i + 1);
			double* C = getVertexCircle(3.5, 0, z, 1.5, i + 1);
			double* D = getVertexCircle(3.5, 0, z, 1.5, i);

			glNormal3dv(getVertexNormale(C, B, A));
			glVertex3dv(A);
			glVertex3dv(B);
			glVertex3dv(C);
			glVertex3dv(D);
		}
		glEnd();
	}

	// Построение стенки для вогнутости
	static void render_rectangle_bulge(double x, double y, double z, double r, double* startVertex, double* endVertex) {
		double startI = getStartN(startVertex, new double[] {x, y, z, r});
		double endI = getStartN(endVertex, new double[] {x, y, z, r});

		glBegin(GL_QUADS);
		for (double i = std::abs(startI); i < std::abs(endI); i += 0.1)
		{
			double* A = getVertexCircle(x, y, 0, r, i);
			double* B = getVertexCircle(x, y, 0, r, i + 1);
			double* C = getVertexCircle(x, y, z, r, i + 1);
			double* D = getVertexCircle(x, y, z, r, i);

			glNormal3dv(getVertexNormale(A, B, C));
			glVertex3dv(A);
			glVertex3dv(B);
			glVertex3dv(C);
			glVertex3dv(D);
		}
		glEnd();
	}

	static void figure_base(double* A1, double* A2, double* A3, double* A4, double* A5, double* A6, double* A7, double* A8, double* M, double z = 0) {
		double A1Copy[] = { A1[0], A1[1], z };
		double A2Copy[] = { A2[0], A2[1], z };
		double A3Copy[] = { A3[0], A3[1], z };
		double A4Copy[] = { A4[0], A4[1], z };
		double A5Copy[] = { A5[0], A5[1], z };
		double A6Copy[] = { A6[0], A6[1], z };
		double A7Copy[] = { A7[0], A7[1], z };
		double A8Copy[] = { A8[0], A8[1], z };

		glBegin(GL_TRIANGLES);
		render_circle(3.5, 0, z, 1.5, A3Copy, A2Copy, A1Copy, z == 0 ? false : true);

		glNormal3dv(getVertexNormale(A5Copy, A4Copy, A3Copy, z == 0 ? false : true));
		glTexCoord2dv(getTextureCoord(A3Copy));
		glVertex3dv(A3Copy);
		glTexCoord2dv(getTextureCoord(A4Copy));
		glVertex3dv(A4Copy);
		glTexCoord2dv(getTextureCoord(A5Copy));
		glVertex3dv(A5Copy);

		double* centerCircle = getCenterCircle(A6Copy, A7Copy, M);
		render_circle(centerCircle[0], centerCircle[1], centerCircle[2], getLenght(A6Copy, centerCircle), A5Copy, A6Copy, A7Copy, z == 0 ? true : false);

		glNormal3dv(getVertexNormale(A7Copy, A5Copy, A8Copy, z == 0 ? false : true));
		glTexCoord2dv(getTextureCoord(A7Copy));
		glVertex3dv(A7Copy);
		glTexCoord2dv(getTextureCoord(A5Copy));
		glVertex3dv(A5Copy);
		glTexCoord2dv(getTextureCoord(A8Copy));
		glVertex3dv(A8Copy);

		glNormal3dv(getVertexNormale(A8Copy, A5Copy, A3Copy, z == 0 ? false : true));
		glTexCoord2dv(getTextureCoord(A3Copy));
		glVertex3dv(A3Copy);
		glTexCoord2dv(getTextureCoord(A5Copy));
		glVertex3dv(A5Copy);
		glTexCoord2dv(getTextureCoord(A8Copy));
		glVertex3dv(A8Copy);

		glNormal3dv(getVertexNormale(A8Copy, A3Copy, A1Copy, z == 0 ? false : true));
		glTexCoord2dv(getTextureCoord(A1Copy));
		glVertex3dv(A1Copy);
		glTexCoord2dv(getTextureCoord(A3Copy));
		glVertex3dv(A3Copy);
		glTexCoord2dv(getTextureCoord(A8Copy));
		glVertex3dv(A8Copy);
		glEnd();
	}

public:
	static void figure(GLuint texId_, GLuint texId2_) {
		double A1[] = { 2, 0, 0 };
		double A2[] = { 5, 0, 0 };
		double A3[] = { 4, 2.5, 0 };
		double A4[] = { 7.5, 4, 0 };
		double A5[] = { 4, 4, 0 };
		double A6[] = { 3, 7, 0 };
		double A7[] = { 0, 6, 0 };
		double A8[] = { 3, 3, 0 };
		double M[] = { 2, 5.9, 0 };
		double h = 2;

		double* centerCircle = getCenterCircle(A6, A7, M);

		figure_base(A1, A2, A3, A4, A5, A6, A7, A8, M);
		render_rectangle_convex(h);
		render_rectangle(A2, A3, h);
		render_rectangle(A3, A4, h);
		render_rectangle(A4, A5, h);
		render_rectangle(A5, A6, h);
		render_rectangle_bulge(centerCircle[0], centerCircle[1], h, getLenght(A6, centerCircle), A6, A7);
		render_rectangle(A7, A8, h);
		render_rectangle(A8, A1, h);
		figure_base(A1, A2, A3, A4, A5, A6, A7, A8, M, h);
	}
};