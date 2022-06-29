#include "Vector3.h"

Vector3& Vector3::operator*(const Matrix4& m) {
	Vector3 temp(*this);

	temp.x = this->x * m.m[0][0] + this->y * m.m[1][0] + this->z * m.m[2][0];
	temp.y = this->x * m.m[0][1] + this->y * m.m[1][1] + this->z * m.m[2][1];
	temp.z = this->x * m.m[0][2] + this->y * m.m[1][2] + this->z * m.m[2][2];

	return temp;
}
