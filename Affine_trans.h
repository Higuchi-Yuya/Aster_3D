#pragma once
#include"Matrix4.h"
#include"Vector3.h"
#include <math.h>
class Affine_trans
{
	public:
		Affine_trans();
		~Affine_trans();

		double Rad(double degree);

		void identity_matrix(Matrix4& mat);

		void scale(Matrix4& mat, float expansion_rate_x, float expansion_rate_y, float expansion_rate_z);

		void translation(Matrix4& mat, float Tx, float Ty, float Tz);

		void rotate(Matrix4& mat, float frequency_x, float frequency_y, float frequency_z);

		void scale(Matrix4& mat, Vector3& scale);

		void translation(Matrix4& mat, Vector3& translation);

		void rotate(Matrix4& mat, Vector3& rotate);

		void Affine_Trans(Matrix4& mat, Vector3& scale, Vector3& rotate, Vector3& translation);

		void Affine_Mul_Vel(Matrix4& mat, Vector3& scale, Vector3& rotate, Vector3& translation, Vector3& velocity);

		void rotateX(Matrix4& mat, float frequency_x);

		void rotateY(Matrix4& mat, float frequency_y);

		void rotateZ(Matrix4& mat, float frequency_z);

		void Inverse(Matrix4& mat);
	private:
	
};

