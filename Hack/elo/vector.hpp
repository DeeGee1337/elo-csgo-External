#pragma once

#include <limits>
#include <math.h>
#include <cfloat>

#define M_RAD_TO_DEG(rad) ( rad * 57.295779513)

#define PI 3.14159265358979323846f
#define DEG2RAD( x ) ( ( float )( x ) * ( float )( ( float )( PI ) / 180.0f ) )
#define RAD2DEG( x ) ( ( float )( x ) * ( float )( 180.0f / ( float )( PI ) ) )


static bool vec_is_close(float a, float b, float eps)
{
	if (abs(eps) < std::numeric_limits<float>::epsilon())
	{
		eps = (float)1e-9;
	}
	return abs(a - b) <= eps;
}



class vec2
{
public:

	vec2();
	vec2(float x1, float y1);
	vec2 extend(vec2 target, float range);
	float angle_between(vec2 p2);
	float polar();
	float distance(vec2 other, bool squared = false);
	vec2 operator-(vec2 V);
	vec2 operator+(vec2 V);
	float size();
	void normalize();
	vec2 direction(vec2 other2D);
	vec2 perpendicular();
	vec2 operator*(float scale);
	vec2 operator*(vec2& V);
	vec2 operator*(vec2 V);
	bool is_zero();


	float x;
	float y;

};

struct ProjectionInfo
{
	ProjectionInfo(bool isOnSegment, vec2 segmentPoint, vec2 linePoint)
	{
		IsOnSegment = isOnSegment;
		SegmentPoint = segmentPoint;
		LinePoint = linePoint;
	}

	bool IsOnSegment;
	vec2 LinePoint;
	vec2 SegmentPoint;
};

static bool v_intersection(vec2 lineSegment1Start, vec2 lineSegment1End, vec2 lineSegment2Start, vec2 lineSegment2End)
{
	auto deltaACy = lineSegment1Start.y - lineSegment2Start.y;
	auto deltaDCx = lineSegment2End.x - lineSegment2Start.x;
	auto deltaACx = lineSegment1Start.x - lineSegment2Start.x;
	auto deltaDCy = lineSegment2End.y - lineSegment2Start.y;
	auto deltaBAx = lineSegment1End.x - lineSegment1Start.x;
	auto deltaBAy = lineSegment1End.y - lineSegment1Start.y;

	auto denominator = deltaBAx * deltaDCy - deltaBAy * deltaDCx;
	auto numerator = deltaACy * deltaDCx - deltaACx * deltaDCy;

	if (abs(denominator) < FLT_EPSILON)
	{
		if (abs(numerator) < FLT_EPSILON)
		{
			// collinear. Potentially infinite intersection points.
			// Check and return one of them.
			if (lineSegment1Start.x >= lineSegment2Start.x && lineSegment1Start.x <= lineSegment2End.x)
			{
				return true;
			}
			if (lineSegment2Start.x >= lineSegment1Start.x && lineSegment2Start.x <= lineSegment1End.x)
			{
				return true;
			}
			return false;
		}
		// parallel
		return false;
	}

	auto r = numerator / denominator;
	if (r < 0 || r > 1)
	{
		return false;
	}

	auto s = (deltaACy * deltaBAx - deltaACx * deltaBAy) / denominator;
	if (s < 0 || s > 1)
	{
		return false;
	}

	return true;
}


static ProjectionInfo project_on(vec2 point, vec2 segmentStart, vec2 segmentEnd)
{
	auto cx = point.x;
	auto cy = point.y;
	auto ax = segmentStart.x;
	auto ay = segmentStart.y;
	auto bx = segmentEnd.x;
	auto by = segmentEnd.y;
	auto rL = ((cx - ax) * (bx - ax) + (cy - ay) * (by - ay)) / (powf(bx - ax, 2) + powf(by - ay, 2));
	auto pointLine = vec2(ax + rL * (bx - ax), ay + rL * (by - ay));
	float rS;
	if (rL < 0)
	{
		rS = 0;
	}
	else if (rL > 1)
	{
		rS = 1;
	}
	else
	{
		rS = rL;
	}

	auto isOnSegment = rS == rL;
	auto pointSegment = isOnSegment ? pointLine : vec2(ax + rS * (bx - ax), ay + rS * (by - ay));
	return ProjectionInfo(isOnSegment, pointSegment, pointLine);
}
#undef max
static float segment_distance(vec2 point, vec2 segmentStart, vec2 segmentEnd, bool onlyIfOnSegment = false, bool squared = false)
{
	auto objects = project_on(point, segmentStart, segmentEnd);

	if (objects.IsOnSegment || onlyIfOnSegment == false)
	{
		return objects.SegmentPoint.distance(point, squared);
	}
	return std::numeric_limits<float>::max();
}
class vec3
{
public:
	vec3()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}
	bool is_valid()
	{
		return !is_zero();
	}
	bool is_zero()
	{
		return x == 0.0f && y == 0.0f && z == 0.0f;
	}

	static vec3 zero()
	{
		return vec3(0.0f, 0.0f, 0.0f);
	}

	vec3(float x1, float y1, float z1)
	{
		x = x1;
		y = y1;
		z = z1;
	}


	float distance(vec3 other, bool squared = false)
	{
		auto dx = (other.x - x);
		auto dy = (other.y - y);
		auto dz = (other.z - z);

		if (!squared)
		{
			return sqrtf((dx * dx) + (dy * dy) + (dz * dz));
		}
		else
		{
			return (dx * dx) + (dy * dy) + (dz * dz);
		}
	}
	vec3 perpendicular()
	{
		return vec3(-y, z, x);
	}
	vec3 extend(vec3 dirvec, float range)
	{
		vec3 result = *this;
		vec3 dir = (dirvec - result).normalize();
		return result + (dir * range);
	}

	vec3 direction(vec3 other)
	{
		auto dir = other - *this;

		return dir.normalize();
	}
	vec2 to_2d()
	{
		return vec2(x, y);
	}

	auto operator -(vec3 other) -> vec3 {
		return vec3(x - other.x, y - other.y, z - other.z);
	}
	auto operator == (vec3& other) -> bool {
		return other.x == x &&
			other.y == y &&
			other.z == z;
	}
	bool equals(vec3 other)
	{
		return other.x == x &&
			other.y == y &&
			other.z == z;
	}

	auto operator != (vec3& other) -> bool {
		return other.x != x ||
			other.y != y ||
			other.z != z;
	}

	float& operator[] (int index) 
	{
		return this->values[index];
	}

	vec3 operator*(float scale) {
		return vec3(x * scale, y * scale, z * scale);
	}
	vec3 operator+(vec3 V)
	{

		vec3 result;
		result.x = (x + V.x);
		result.y = (y + V.y);
		result.z = (z + V.z);

		return result;
	}
	float size()
	{
		return sqrtf(x * x + y * y + z * z);
	}

	void normalize_in_place()
	{
		auto normalized = normalize();

		x = normalized.x;
		y = normalized.y;
		z = normalized.z;
	}

	vec3 normalize()
	{
		vec3 vector;
		float length = this->size();

		if (length != 0) {
			vector.x = (x / length);
			vector.y = (y / length);
			vector.z = (z / length);
		}

		return vector;
	}

	vec3 cross(vec3* other) // not tested
	{
		float x = 0.0, y = 0.0, z = 0.0;

		x = (this->y * other->z - this->z * other->y);
		y = (this->z * other->x - this->x * other->z);
		z = (this->x * other->y - this->y * other->x);

		return vec3(x, y, z);
	}
	float dot(vec3 other)
	{
		return (x * other.x) + (y * other.y) + (z + other.z);
	}
	float angle_between(vec3 toVector3)
	{
		auto vector3 = *this;

		auto magnitudeA = sqrtf((vector3.x * vector3.x) + (vector3.y * vector3.y) + (vector3.z * vector3.z));
		auto magnitudeB = sqrtf((toVector3.x * toVector3.x) + (toVector3.y * toVector3.y) + (toVector3.z * toVector3.z));

		auto dotProduct = dot(toVector3);
		return cosf(dotProduct / magnitudeA * magnitudeB);
	}
	union
	{
	public:
		float values[3];
		struct
		{
			float x;
			float y;
			float z;
		};
	};
};
