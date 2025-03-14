#include "Quaternion.h"

Quaternion EulerToQuaternion(float pitch, float yaw, float roll) {
	// オイラー角をクォータニオンに変換
	float cy = cos(yaw * 0.5f);
	float sy = sin(yaw * 0.5f);
	float cp = cos(pitch * 0.5f);
	float sp = sin(pitch * 0.5f);
	float cr = cos(roll * 0.5f);
	float sr = sin(roll * 0.5f);

	Quaternion q;
	q.w = cr * cp * cy + sr * sp * sy;
	q.x = sr * cp * cy - cr * sp * sy;
	q.y = cr * sp * cy + sr * cp * sy;
	q.z = cr * cp * sy - sr * sp * cy;

	return q;
}