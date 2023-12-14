#include "vector.hpp"

vec2::vec2()
{
	x = (0.0f);
	y = (0.0f);
}
vec2::vec2(float x1, float y1)
{
	x = (x1);
	y = (y1);
}

vec2 vec2::extend(vec2 target, float range)
{
	auto source = *this;
	auto dirV = (target - source);
	dirV.normalize();

	return source + (dirV * range);
}
float vec2::angle_between(vec2 p2)
{
	auto p1 = *this;
	auto theta = p1.polar() - p2.polar();
	if (theta < 0)
	{
		theta = theta + 360;
	}
	if (theta > 180)
	{
		theta = 360 - theta;
	}
	return theta;
}
float vec2::polar()
{
	auto v1 = *this;

	if (vec_is_close(v1.x, 0, 0))
	{
		if (v1.y > 0)
		{
			return 90.0f;
		}
		return v1.y < 0.0f ? 270.0f : 0.0f;
	}

	auto theta = M_RAD_TO_DEG(atan((v1.y) / v1.x));
	if (v1.x < 0)
	{
		theta = theta + 180.0f;
	}
	if (theta < 0)
	{
		theta = theta + 360.0f;
	}
	return (float)theta;
}

float vec2::distance(vec2 other, bool squared)
{
	auto dx = (other.x - x);
	auto dy = (other.y - y);

	if (!squared)
		return sqrtf((dx * dx) + (dy * dy));
	else
		return (dx * dx) + (dy * dy);
}


vec2 vec2::operator-(vec2 V) {
	vec2 result;
	result.x = (x - V.x);
	result.y = (y - V.y);

	return result;
}

vec2 vec2::operator+(vec2 V) {
	vec2 result;
	result.x = (x + V.x);
	result.y = (y + V.y);

	return result;
}

float vec2::size() {
	return sqrtf(x * x + y * y);
}
void vec2::normalize() {

	float length = this->size();

	if (length != 0) {
		x = (x / length);
		y = (y / length);
	}
}

vec2 vec2::direction(vec2 other2D)
{
	auto this2D = *this;
	auto dir = other2D - this2D;
	dir.normalize();
	return dir;
}
vec2 vec2::perpendicular()
{
	return vec2(-y, x);
}
vec2 vec2::operator*(float scale) {
	return vec2(x * scale, y * scale);
}
vec2 vec2::operator*(vec2& V)
{
	return vec2(x * V.x, y * V.y);
}
vec2 vec2::operator*(vec2 V)
{
	return vec2(x * V.x, y * V.y);
}
bool vec2::is_zero()
{
	return x == 0.0f && y == 0.0f;
}
