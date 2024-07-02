// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "math.h"

namespace math
{
	float lerp(const float& first, const float& second, const float& progress)
	{
		return first + progress * (second - first);
	}

	Vector lerp(const Vector& first, const Vector& second, const float& progress)
	{
		return first + progress * (second - first);
	}

	Vector interpolate(const Vector& first, const Vector& second, const float& progress)
	{
		if (first == second) //-V550
			return first;

		return second * progress + first * (1.0f - progress);
	}

	float interpolate(const float& first, const float& second, const float& progress)
	{
		if (first == second) //-V550
			return first;

		return second * progress + first * (1.0f - progress);
	}

	float normalize_pitch(const float& pitch)
	{
		auto normalized_pitch = pitch;

		while (normalized_pitch < -89.0f)
			normalized_pitch += 180.0f;

		while (normalized_pitch > 89.0f)
			normalized_pitch -= 180.0f;

		return normalized_pitch;
	}

	float normalize_yaw(const float& yaw)
	{
		auto normalized_yaw = yaw;

		while (normalized_yaw < -180.0f)
			normalized_yaw += 360.0f;

		while (normalized_yaw > 180.0f)
			normalized_yaw -= 360.0f;

		return normalized_yaw;
	}

	Vector normalize_angles(Vector angles)
	{
		Vector final_angles = angles;

		while (final_angles.x < -90.0f)
			final_angles.x += 180.0f;

		while (final_angles.x > 90.0f)
			final_angles.x -= 180.0f;

		while (final_angles.y < -180.0f)
			final_angles.y += 360.0f;

		while (final_angles.y > 180.0f)
			final_angles.y -= 360.0f;

		final_angles.z = clamp(final_angles.z, -50.f, 50.f);

		return final_angles;
	}

	void vector_transform(const Vector& first, const matrix3x4_t& second, Vector& out)
	{
		out = Vector(first.Dot(Vector(second[0][0], second[0][1], second[0][2])) + second[0][3], first.Dot(Vector(second[1][0], second[1][1], second[1][2])) + second[1][3], first.Dot(Vector(second[2][0], second[2][1], second[2][2])) + second[2][3]);
	}

	Vector vector_rotate(const Vector& in1, const matrix3x4_t& in2)
	{
		return Vector(in1.Dot(in2[0]), in1.Dot(in2[1]), in1.Dot(in2[2]));
	}

	Vector vector_rotate(const Vector& in1, const Vector& in2)
	{
		return vector_rotate(in1, angle_matrix(in2));
	}

	void vector_rotate(const Vector& in1, const matrix3x4_t& in2, Vector& out)
	{
		out.x = in1.x * in2[0][0] + in1.y * in2[1][0] + in1.z * in2[2][0];
		out.y = in1.x * in2[0][1] + in1.y * in2[1][1] + in1.z * in2[2][1];
		out.z = in1.x * in2[0][2] + in1.y * in2[1][2] + in1.z * in2[2][2];
	}

	void matrix_copy(const matrix3x4_t& in, matrix3x4_t& out)
	{
		memcpy(out.Base(), in.Base(), sizeof(matrix3x4_t));
	}

	void concat_transforms(const matrix3x4_t& in1, const matrix3x4_t& in2, matrix3x4_t& out)
	{
		if (&in1 == &out)
		{
			matrix3x4_t in1b;
			matrix_copy(in1, in1b);
			concat_transforms(in1b, in2, out);
			return;
		}

		if (&in2 == &out)
		{
			matrix3x4_t in2b;
			matrix_copy(in2, in2b);
			concat_transforms(in1, in2b, out);
			return;
		}

		out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] + in1[0][2] * in2[2][0];
		out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] + in1[0][2] * in2[2][1];
		out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] + in1[0][2] * in2[2][2];
		out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] + in1[0][2] * in2[2][3] + in1[0][3];

		out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] + in1[1][2] * in2[2][0];
		out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] + in1[1][2] * in2[2][1];
		out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] + in1[1][2] * in2[2][2];
		out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] + in1[1][2] * in2[2][3] + in1[1][3];

		out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] + in1[2][2] * in2[2][0];
		out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] + in1[2][2] * in2[2][1];
		out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] + in1[2][2] * in2[2][2];
		out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] + in1[2][2] * in2[2][3] + in1[2][3];
	}

	matrix3x4_t angle_matrix(const Vector& angles)
	{
		matrix3x4_t result;
		m128 angle, sin, cos;

		angle.f[0] = DirectX::XMConvertToRadians(angles.x);
		angle.f[1] = DirectX::XMConvertToRadians(angles.y);
		angle.f[2] = DirectX::XMConvertToRadians(angles.z);

		sincos_ps(angle.v, &sin.v, &cos.v);

		result[0][0] = cos.f[0] * cos.f[1];
		result[1][0] = cos.f[0] * sin.f[1];
		result[2][0] = -sin.f[0];

		auto crcy = cos.f[2] * cos.f[1];
		auto crsy = cos.f[2] * sin.f[1];
		auto srcy = sin.f[2] * cos.f[1];
		auto srsy = sin.f[2] * sin.f[1];

		result[0][1] = sin.f[0] * srcy - crsy;
		result[1][1] = sin.f[0] * srsy + crcy;
		result[2][1] = sin.f[2] * cos.f[0];

		result[0][2] = sin.f[0] * crcy + srsy;
		result[1][2] = sin.f[0] * crsy - srcy;
		result[2][2] = cos.f[2] * cos.f[0];

		return result;
	}

	void SinCos(float radians, float* sine, float* cosine)
	{
		*sine = sin(radians);
		*cosine = cos(radians);
	}

	void AngleMatrix(const Vector& angles, matrix3x4_t& matrix)
	{
#ifdef _VPROF_MATHLIB
		VPROF_BUDGET("AngleMatrix", "Mathlib");
#endif
		//Assert(s_bMathlibInitialized);

		float sr, sp, sy, cr, cp, cy;

		SinCos(DirectX::XMConvertToRadians(angles[1]), &sy, &cy);
		SinCos(DirectX::XMConvertToRadians(angles[0]), &sp, &cp);
		SinCos(DirectX::XMConvertToRadians(angles[2]), &sr, &cr);


		// matrix = (YAW * PITCH) * ROLL
		matrix[0][0] = cp * cy;
		matrix[1][0] = cp * sy;
		matrix[2][0] = -sp;

		float crcy = cr * cy;
		float crsy = cr * sy;
		float srcy = sr * cy;
		float srsy = sr * sy;
		matrix[0][1] = sp * srcy - crsy;
		matrix[1][1] = sp * srsy + crcy;
		matrix[2][1] = sr * cp;

		matrix[0][2] = (sp * crcy + srsy);
		matrix[1][2] = (sp * crsy - srcy);
		matrix[2][2] = cr * cp;

		matrix[0][3] = 0.0f;
		matrix[1][3] = 0.0f;
		matrix[2][3] = 0.0f;
	}


	void MatrixSetColumn(const Vector& in, int column, matrix3x4_t& out)
	{
		out[0][column] = in.x;
		out[1][column] = in.y;
		out[2][column] = in.z;
	}

	void MatrixAngles(const matrix3x4_t& matrix, Vector& angles)
	{
#ifdef _VPROF_MATHLIB
		VPROF_BUDGET("MatrixAngles", "Mathlib");
#endif
		
		float forward[3];
		float left[3];
		float up[3];

		//
		// Extract the basis vectors from the matrix. Since we only need the Z
		// component of the up vector, we don't get X and Y.
		//
		forward[0] = matrix[0][0];
		forward[1] = matrix[1][0];
		forward[2] = matrix[2][0];
		left[0] = matrix[0][1];
		left[1] = matrix[1][1];
		left[2] = matrix[2][1];
		up[2] = matrix[2][2];

		float xyDist = sqrtf(forward[0] * forward[0] + forward[1] * forward[1]);

		// enough here to get angles?
		if (xyDist > 0.001f)
		{
			// (yaw)	y = ATAN( forward.y, forward.x );		-- in our space, forward is the X axis
			angles.y= DirectX::XMConvertToRadians(atan2f(forward[1], forward[0]));

			// (pitch)	x = ATAN( -forward.z, sqrt(forward.x*forward.x+forward.y*forward.y) );
			angles.x = DirectX::XMConvertToRadians(atan2f(-forward[2], xyDist));

			// (roll)	z = ATAN( left.z, up.z );
			angles.z = DirectX::XMConvertToRadians(atan2f(left[2], up[2]));
		}
		else	// forward is mostly Z, gimbal lock-
		{
			// (yaw)	y = ATAN( -left.x, left.y );			-- forward is mostly z, so use right for yaw
			angles.y = DirectX::XMConvertToRadians(atan2f(-left[0], left[1]));

			// (pitch)	x = ATAN( -forward.z, sqrt(forward.x*forward.x+forward.y*forward.y) );
			angles.x = DirectX::XMConvertToRadians(atan2f(-forward[2], xyDist));

			// Assume no roll in this case as one degree of freedom has been lost (i.e. yaw == roll)
			angles.z = 0;
		}
	}

	void MatrixAngles(const matrix3x4_t& matrix, Vector& angles, Vector& position)
	{
		MatrixAngles(matrix, angles);
		MatrixPosition(matrix, position);
	}

	void AngleMatrix(const Vector& angles, const Vector& position, matrix3x4_t& matrix)
	{
		//matrix = angle_matrix(angles);
		AngleMatrix(angles, matrix);
		MatrixSetColumn(position, 3, matrix);
	}

	void angle_vectors(const Vector& angles, Vector* forward, Vector* right, Vector* up)
	{
		float sp, sy, sr, cp, cy, cr;

		DirectX::XMScalarSinCosEst(&sp, &cp, DirectX::XMConvertToRadians(angles.x));
		DirectX::XMScalarSinCosEst(&sy, &cy, DirectX::XMConvertToRadians(angles.y));
		DirectX::XMScalarSinCosEst(&sr, &cr, DirectX::XMConvertToRadians(angles.z));

		if (forward)
		{
			forward->x = cp * cy;
			forward->y = cp * sy;
			forward->z = -sp;
		}

		if (right)
		{
			right->x = -1.0f * sr * sp * cy + -1.0f * cr * -sy;
			right->y = -1.0f * sr * sp * sy + -1.0f * cr * cy;
			right->z = -1.0f * sr * cp;
		}

		if (up)
		{
			up->x = cr * sp * cy + -sr * -sy;
			up->y = cr * sp * sy + -sr * cy;
			up->z = cr * cp;
		}
	}

	void vector_angles(const Vector& forward, Vector& angles)
	{
		Vector view;

		if (!forward[0] && !forward[1])
		{
			view[0] = 0.0f;
			view[1] = 0.0f;
		}
		else
		{
			view[1] = atan2(forward[1], forward[0]) * 180.0f / DirectX::XM_PI;

			if (view[1] < 0.0f)
				view[1] += 360.0f;

			view[2] = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
			view[0] = atan2(forward[2], view[2]) * 180.0f / DirectX::XM_PI;
		}

		angles[0] = -view[0];
		angles[1] = view[1];
		angles[2] = 0.0f;
	}

	Vector calculate_angle(const Vector& start, const Vector& end) 
	{
		auto delta = start - end;
		auto angles = Vector(atan(-delta.z / delta.Length2D()) * -DirectX::XMConvertToDegrees(1.0f), atan(delta.y / delta.x) * DirectX::XMConvertToDegrees(1.0f), 0.0f);

		if (delta.x >= 0.0f)
			angles.y += 180.0f;

		return math::normalize_angles(angles);
	}

	float segment_to_segment(const Vector& s1, const Vector& s2, const Vector& k1, const Vector& k2)
	{
		auto u = s2 - s1;
		auto v = k2 - k1;
		auto w = s1 - k1;

		auto a = u.Dot(u); //-V525
		auto b = u.Dot(v);
		auto c = v.Dot(v);
		auto d = u.Dot(w);
		auto e = v.Dot(w);
		auto D = a * c - b * b;

		auto sc = 0.0f;
		auto sN = 0.0f;
		auto tc = 0.0f;
		auto tN = 0.0f;

		auto sD = D;
		auto tD = D;

		if (D < 0.00000001f)
		{
			sN = 0.0f;
			sD = 1.0f;
			tN = e;
			tD = c;
		}
		else
		{
			sN = b * e - c * d;
			tN = a * e - b * d;

			if (sN < 0.0f)
			{
				sN = 0.0f;
				tN = e;
				tD = c;
			}
			else if (sN > sD)
			{
				sN = sD;
				tN = e + b;
				tD = c;
			}
		}

		if (tN < 0.0f)
		{
			tN = 0.0f;

			if (-d < 0.0f)
				sN = 0.0f;
			else if (-d > a)
				sN = sD;
			else
			{
				sN = -d;
				sD = a;
			}
		}
		else if (tN > tD)
		{
			tN = tD;

			if (-d + b < 0.0f)
				sN = 0.0f;
			else if ((-d + b) > a)
				sN = sD;
			else
			{
				sN = (-d + b);
				sD = a;
			}
		}

		sc = abs(sN) < 0.00000001f ? 0.0f : sN / sD;
		tc = abs(tN) < 0.00000001f ? 0.0f : tN / tD;

		auto dP = w + u * sc - v * tc;
		return std::sqrt(dP.Dot(dP));
	}

	float simple_spline(const float& value)
	{
		auto value_squared = value * value;
		return 3.0f * value_squared - 2.0f * value_squared * value;
	}

	float simple_spline_remap_val_clamped(const float& value, const float& a, const float& b, const float& c, const float& d)
	{
		if (a == b) //-V550
			return value >= b ? d : c;

		auto c_value = clamp((value - a) / (b - a), 0.0f, 1.0f);
		return c + (d - c) * simple_spline(c_value);
	}

	void vector_ma_inline(const float* start, float scale, const float* direction, float* destination)
	{
		destination[0] = start[0] + direction[0] * scale;
		destination[1] = start[1] + direction[1] * scale;
		destination[2] = start[2] + direction[2] * scale;
	}

	void vector_ma_inline(const Vector& start, float scale, const Vector& direction, Vector& destination)
	{
		destination.x = start.x + direction.x * scale;
		destination.y = start.y + direction.y * scale;
		destination.z = start.z + direction.z * scale;
	}

	void vector_ma(const Vector& start, float scale, const Vector& direction, Vector& destination)
	{
		vector_ma_inline(start, scale, direction, destination);
	}

	void vector_ma(const float* start, float scale, const float* direction, float* destination)
	{
		vector_ma_inline(start, scale, direction, destination);
	}

	float get_field_of_view(const Vector& view_angle, const Vector& aim_angle)
	{
		Vector view;
		angle_vectors(aim_angle, &view, nullptr, nullptr);

		Vector aim;
		angle_vectors(view_angle, &aim, nullptr, nullptr);

		return max(DirectX::XMConvertToDegrees(acos(aim.Dot(view) / aim.LengthSqr())), 0.0f);
	}

	void random_seed(const int& seed)
	{
		static auto random_seed_function = reinterpret_cast <void(*)(int)> (GetProcAddress(GetModuleHandle(crypt_str("vstdlib.dll")), crypt_str("RandomSeed")));
		return random_seed_function(seed);
	}

	int random_int(const int& min, const int& max) 
	{
		static auto random_int_function = reinterpret_cast <int(*)(int, int)> (GetProcAddress(GetModuleHandle(crypt_str("vstdlib.dll")), crypt_str("RandomInt")));
		return random_int_function(min, max);
	}

	float random_float(const float& min, const float& max) 
	{
		static auto random_float_function = reinterpret_cast <float(*)(float, float)> (GetProcAddress(GetModuleHandle(crypt_str("vstdlib.dll")), crypt_str("RandomFloat")));
		return random_float_function(min, max);
	}
}