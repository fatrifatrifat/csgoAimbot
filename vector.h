#pragma once

#include <cmath>
#include <numbers>

struct Vec3D
{
	float x, y, z;

	Vec3D operator+(Vec3D other)
	{
		return { this->x + other.x, this->y + other.y, this->z + other.z };
	}

	Vec3D operator-(Vec3D other)
	{
		return { this->x - other.x, this->y - other.y, this->z - other.z };
	}

	
	Vec3D relativeAngle(Vec3D other)
	{
		
		float dx = other.x - this->x;
		float dy = other.y - this->y;
		float dz = other.z - this->z;

		
		float hypotenus = this->distanceCalculate(other);

		//float pitch = -asin(delta.z / hypotenuse) * (180 / static_cast<float>(PI));
		//float yaw = atan2(delta.y, delta.x) * (180 / static_cast<float>(PI));
		
		float yaw = std::atan2(dy, dx) * (180.0f / std::numbers::pi);
		
		float pitch = -(std::asin(dz / hypotenus) * (180.0f / std::numbers::pi));

		return { pitch, yaw, 0.0f };

	}
	


	float distanceCalculate(Vec3D other)
	{
		float distance;
		distance = std::sqrt(pow((other.x - this->x), 2) + pow((other.y - this->y), 2) + pow((other.z - this->z), 2));
		return distance;
	}
};