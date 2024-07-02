#pragma once

#include <d3dx9math.h>
#include <DirectXMath.h>
#include "math\Vector.hpp"
#include "math\VMatrix.hpp"
#include "math\m128.h"
#include "..\protect\crypt_ptr.h"

#define TIME_TO_TICKS(t) ((int)(0.5f + (float)(t) / globals->intervalpertick))
#define TICKS_TO_TIME(t) ((float)(globals->intervalpertick * (float)(t)))

namespace math
{
	float lerp(const float& first, const float& second, const float& progress);
	Vector lerp(const Vector& first, const Vector& second, const float& progress);
	Vector interpolate(const Vector& first, const Vector& second, const float& progress);
	float interpolate(const float& first, const float& second, const float& progress); 
	float normalize_pitch(const float& pitch);
	float normalize_yaw(const float& yaw);
	Vector normalize_angles(Vector angles);
	void vector_transform(const Vector& first, const matrix3x4_t& second, Vector& out);
	Vector vector_rotate(const Vector& in1, const matrix3x4_t& in2);
	Vector vector_rotate(const Vector& in1, const Vector& in2);
	void vector_rotate(const Vector& in1, const matrix3x4_t& in2, Vector& out);
	void matrix_copy(const matrix3x4_t& in, matrix3x4_t& out);
	void concat_transforms(const matrix3x4_t& in1, const matrix3x4_t& in2, matrix3x4_t& out);
	matrix3x4_t angle_matrix(const Vector& angles);
	void AngleMatrix(const Vector& angles, const Vector& position, matrix3x4_t& matrix);
	void angle_vectors(const Vector& angles, Vector* forward, Vector* right, Vector* up);
	void vector_angles(const Vector& forward, Vector& angles);
	Vector calculate_angle(const Vector& start, const Vector& end);
	float segment_to_segment(const Vector& s1, const Vector& s2, const Vector& k1, const Vector& k2);
	float simple_spline(const float& value);
	float simple_spline_remap_val_clamped(const float& value, const float& a, const float& b, const float& c, const float& d);
	void vector_ma_inline(const float* start, float scale, const float* direction, float* destination);
	void vector_ma_inline(const Vector& start, float scale, const Vector& direction, Vector& destination);
	void vector_ma(const Vector& start, float scale, const Vector& direction, Vector& destination);
	void vector_ma(const float* start, float scale, const float* direction, float* destination);
	float get_field_of_view(const Vector& view_angle, const Vector& aim_angle);
	void random_seed(const int& seed);
	int random_int(const int& min, const int& max);
	float random_float(const float& min, const float& max);
	void MatrixAngles(const matrix3x4_t& matrix, Vector& angles, Vector& position);
	void MatrixAngles(const matrix3x4_t& matrix, Vector& angles);

}