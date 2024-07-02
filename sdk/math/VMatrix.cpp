// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "VMatrix.hpp"

//-----------------------------------------------------------------------------
// VMatrix inlines.
//-----------------------------------------------------------------------------
inline VMatrix::VMatrix( ) { } //-V730

inline VMatrix::VMatrix(
	float m00, float m01, float m02, float m03,
	float m10, float m11, float m12, float m13,
	float m20, float m21, float m22, float m23,
	float m30, float m31, float m32, float m33 ) {
	Init(
		m00, m01, m02, m03,
		m10, m11, m12, m13,
		m20, m21, m22, m23,
		m30, m31, m32, m33
	);
}


inline VMatrix::VMatrix( const matrix3x4_t& matrix3x4 ) {
	Init( matrix3x4 );
}


//-----------------------------------------------------------------------------
// Creates a matrix where the X axis = forward
// the Y axis = left, and the Z axis = up
//-----------------------------------------------------------------------------
inline VMatrix::VMatrix( const Vector& xAxis, const Vector& yAxis, const Vector& zAxis ) {
	Init(
		xAxis.x, yAxis.x, zAxis.x, 0.0f,
		xAxis.y, yAxis.y, zAxis.y, 0.0f,
		xAxis.z, yAxis.z, zAxis.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}


inline void VMatrix::Init(
	float m00, float m01, float m02, float m03,
	float m10, float m11, float m12, float m13,
	float m20, float m21, float m22, float m23,
	float m30, float m31, float m32, float m33
) {
	m[ 0 ][ 0 ] = m00;
	m[ 0 ][ 1 ] = m01;
	m[ 0 ][ 2 ] = m02;
	m[ 0 ][ 3 ] = m03;

	m[ 1 ][ 0 ] = m10;
	m[ 1 ][ 1 ] = m11;
	m[ 1 ][ 2 ] = m12;
	m[ 1 ][ 3 ] = m13;

	m[ 2 ][ 0 ] = m20;
	m[ 2 ][ 1 ] = m21;
	m[ 2 ][ 2 ] = m22;
	m[ 2 ][ 3 ] = m23;

	m[ 3 ][ 0 ] = m30;
	m[ 3 ][ 1 ] = m31;
	m[ 3 ][ 2 ] = m32;
	m[ 3 ][ 3 ] = m33;
}


//-----------------------------------------------------------------------------
// Initialize from a 3x4
//-----------------------------------------------------------------------------
inline void VMatrix::Init( const matrix3x4_t& matrix3x4 ) {
	memcpy( m, matrix3x4.Base( ), sizeof( matrix3x4_t ) );

	m[ 3 ][ 0 ] = 0.0f;
	m[ 3 ][ 1 ] = 0.0f;
	m[ 3 ][ 2 ] = 0.0f;
	m[ 3 ][ 3 ] = 1.0f;
}

//-----------------------------------------------------------------------------
// Vector3DMultiplyPosition treats src2 as if it's a point (adds the translation)
//-----------------------------------------------------------------------------
// NJS: src2 is passed in as a full vector rather than a reference to prevent the need
// for 2 branches and a potential copy in the body.  (ie, handling the case when the src2
// reference is the same as the dst reference ).
inline void Vector3DMultiplyPosition( const VMatrix& src1, const Vector& src2, Vector& dst ) {
	dst[ 0 ] = src1[ 0 ][ 0 ] * src2.x + src1[ 0 ][ 1 ] * src2.y + src1[ 0 ][ 2 ] * src2.z + src1[ 0 ][ 3 ];
	dst[ 1 ] = src1[ 1 ][ 0 ] * src2.x + src1[ 1 ][ 1 ] * src2.y + src1[ 1 ][ 2 ] * src2.z + src1[ 1 ][ 3 ];
	dst[ 2 ] = src1[ 2 ][ 0 ] * src2.x + src1[ 2 ][ 1 ] * src2.y + src1[ 2 ][ 2 ] * src2.z + src1[ 2 ][ 3 ];
}

//-----------------------------------------------------------------------------
// Methods related to the basis vectors of the matrix
//-----------------------------------------------------------------------------

inline Vector VMatrix::GetForward( ) const {
	return Vector( m[ 0 ][ 0 ], m[ 1 ][ 0 ], m[ 2 ][ 0 ] );
}

inline Vector VMatrix::GetLeft( ) const {
	return Vector( m[ 0 ][ 1 ], m[ 1 ][ 1 ], m[ 2 ][ 1 ] );
}

inline Vector VMatrix::GetUp( ) const {
	return Vector( m[ 0 ][ 2 ], m[ 1 ][ 2 ], m[ 2 ][ 2 ] );
}

inline void VMatrix::SetForward( const Vector &vForward ) {
	m[ 0 ][ 0 ] = vForward.x;
	m[ 1 ][ 0 ] = vForward.y;
	m[ 2 ][ 0 ] = vForward.z;
}

inline void VMatrix::SetLeft( const Vector &vLeft ) {
	m[ 0 ][ 1 ] = vLeft.x;
	m[ 1 ][ 1 ] = vLeft.y;
	m[ 2 ][ 1 ] = vLeft.z;
}

inline void VMatrix::SetUp( const Vector &vUp ) {
	m[ 0 ][ 2 ] = vUp.x;
	m[ 1 ][ 2 ] = vUp.y;
	m[ 2 ][ 2 ] = vUp.z;
}

inline void VMatrix::GetBasisVectors( Vector &vForward, Vector &vLeft, Vector &vUp ) const {
	vForward.Init( m[ 0 ][ 0 ], m[ 1 ][ 0 ], m[ 2 ][ 0 ] );
	vLeft.Init( m[ 0 ][ 1 ], m[ 1 ][ 1 ], m[ 2 ][ 1 ] );
	vUp.Init( m[ 0 ][ 2 ], m[ 1 ][ 2 ], m[ 2 ][ 2 ] );
}

inline void VMatrix::SetBasisVectors( const Vector &vForward, const Vector &vLeft, const Vector &vUp ) {
	SetForward( vForward );
	SetLeft( vLeft );
	SetUp( vUp );
}


//-----------------------------------------------------------------------------
// Methods related to the translation component of the matrix
//-----------------------------------------------------------------------------

inline Vector VMatrix::GetTranslation( ) const {
	return Vector( m[ 0 ][ 3 ], m[ 1 ][ 3 ], m[ 2 ][ 3 ] );
}

inline Vector& VMatrix::GetTranslation( Vector &vTrans ) const {
	vTrans.x = m[ 0 ][ 3 ];
	vTrans.y = m[ 1 ][ 3 ];
	vTrans.z = m[ 2 ][ 3 ];
	return vTrans;
}

inline void VMatrix::SetTranslation( const Vector &vTrans ) {
	m[ 0 ][ 3 ] = vTrans.x;
	m[ 1 ][ 3 ] = vTrans.y;
	m[ 2 ][ 3 ] = vTrans.z;
}


//-----------------------------------------------------------------------------
// appply translation to this matrix in the input space
//-----------------------------------------------------------------------------
inline void VMatrix::PreTranslate( const Vector &vTrans ) {
	Vector tmp;
	Vector3DMultiplyPosition( *this, vTrans, tmp );
	m[ 0 ][ 3 ] = tmp.x;
	m[ 1 ][ 3 ] = tmp.y;
	m[ 2 ][ 3 ] = tmp.z;
}


//-----------------------------------------------------------------------------
// appply translation to this matrix in the output space
//-----------------------------------------------------------------------------
inline void VMatrix::PostTranslate( const Vector &vTrans ) {
	m[ 0 ][ 3 ] += vTrans.x;
	m[ 1 ][ 3 ] += vTrans.y;
	m[ 2 ][ 3 ] += vTrans.z;
}

inline const matrix3x4_t& VMatrix::As3x4( ) const {
	return *( ( const matrix3x4_t* )this ); //-V1027
}

inline matrix3x4_t& VMatrix::As3x4( ) {
	return *( ( matrix3x4_t* )this ); //-V1027
}

inline void VMatrix::CopyFrom3x4( const matrix3x4_t &m3x4 ) {
	memcpy( m, m3x4.Base( ), sizeof( matrix3x4_t ) ); //-V512
	m[ 3 ][ 0 ] = m[ 3 ][ 1 ] = m[ 3 ][ 2 ] = 0;
	m[ 3 ][ 3 ] = 1;
}

inline void VMatrix::Set3x4( matrix3x4_t& matrix3x4 ) const {
	memcpy( matrix3x4.Base( ), m, sizeof( matrix3x4_t ) ); //-V512
}


//-----------------------------------------------------------------------------
// Matrix Math operations
//-----------------------------------------------------------------------------
inline const VMatrix& VMatrix::operator+=( const VMatrix &other ) {
	for ( int i = 0; i < 4; i++ ) {
		for ( int j = 0; j < 4; j++ ) {
			m[ i ][ j ] += other.m[ i ][ j ];
		}
	}

	return *this;
}

inline VMatrix VMatrix::operator+( const VMatrix &other ) const {
	VMatrix ret;
	for ( int i = 0; i < 16; i++ ) {
		( ( float* )ret.m )[ i ] = ( ( float* )m )[ i ] + ( ( float* )other.m )[ i ];
	}
	return ret;
}

inline VMatrix VMatrix::operator-( const VMatrix &other ) const {
	VMatrix ret;

	for ( int i = 0; i < 4; i++ ) {
		for ( int j = 0; j < 4; j++ ) {
			ret.m[ i ][ j ] = m[ i ][ j ] - other.m[ i ][ j ];
		}
	}

	return ret;
}

inline VMatrix VMatrix::operator-( ) const {
	VMatrix ret;
	for ( int i = 0; i < 16; i++ ) {
		( ( float* )ret.m )[ i ] = -( ( float* )m )[ i ];
	}
	return ret;
}

//-----------------------------------------------------------------------------
// Vector transformation
//-----------------------------------------------------------------------------


inline Vector VMatrix::operator*( const Vector &vVec ) const {
	Vector vRet;
	vRet.x = m[ 0 ][ 0 ] * vVec.x + m[ 0 ][ 1 ] * vVec.y + m[ 0 ][ 2 ] * vVec.z + m[ 0 ][ 3 ];
	vRet.y = m[ 1 ][ 0 ] * vVec.x + m[ 1 ][ 1 ] * vVec.y + m[ 1 ][ 2 ] * vVec.z + m[ 1 ][ 3 ];
	vRet.z = m[ 2 ][ 0 ] * vVec.x + m[ 2 ][ 1 ] * vVec.y + m[ 2 ][ 2 ] * vVec.z + m[ 2 ][ 3 ];

	return vRet;
}

inline Vector VMatrix::VMul4x3( const Vector &vVec ) const {
	Vector vResult;
	Vector3DMultiplyPosition( *this, vVec, vResult );
	return vResult;
}


inline Vector VMatrix::VMul4x3Transpose( const Vector &vVec ) const {
	Vector tmp = vVec;
	tmp.x -= m[ 0 ][ 3 ];
	tmp.y -= m[ 1 ][ 3 ];
	tmp.z -= m[ 2 ][ 3 ];

	return Vector(
		m[ 0 ][ 0 ] * tmp.x + m[ 1 ][ 0 ] * tmp.y + m[ 2 ][ 0 ] * tmp.z,
		m[ 0 ][ 1 ] * tmp.x + m[ 1 ][ 1 ] * tmp.y + m[ 2 ][ 1 ] * tmp.z,
		m[ 0 ][ 2 ] * tmp.x + m[ 1 ][ 2 ] * tmp.y + m[ 2 ][ 2 ] * tmp.z
	);
}

inline Vector VMatrix::VMul3x3( const Vector &vVec ) const {
	return Vector(
		m[ 0 ][ 0 ] * vVec.x + m[ 0 ][ 1 ] * vVec.y + m[ 0 ][ 2 ] * vVec.z,
		m[ 1 ][ 0 ] * vVec.x + m[ 1 ][ 1 ] * vVec.y + m[ 1 ][ 2 ] * vVec.z,
		m[ 2 ][ 0 ] * vVec.x + m[ 2 ][ 1 ] * vVec.y + m[ 2 ][ 2 ] * vVec.z
	);
}

inline Vector VMatrix::VMul3x3Transpose( const Vector &vVec ) const {
	return Vector(
		m[ 0 ][ 0 ] * vVec.x + m[ 1 ][ 0 ] * vVec.y + m[ 2 ][ 0 ] * vVec.z,
		m[ 0 ][ 1 ] * vVec.x + m[ 1 ][ 1 ] * vVec.y + m[ 2 ][ 1 ] * vVec.z,
		m[ 0 ][ 2 ] * vVec.x + m[ 1 ][ 2 ] * vVec.y + m[ 2 ][ 2 ] * vVec.z
	);
}


inline void VMatrix::V3Mul( const Vector &vIn, Vector &vOut ) const {
	float rw;

	rw = 1.0f / ( m[ 3 ][ 0 ] * vIn.x + m[ 3 ][ 1 ] * vIn.y + m[ 3 ][ 2 ] * vIn.z + m[ 3 ][ 3 ] );
	vOut.x = ( m[ 0 ][ 0 ] * vIn.x + m[ 0 ][ 1 ] * vIn.y + m[ 0 ][ 2 ] * vIn.z + m[ 0 ][ 3 ] ) * rw;
	vOut.y = ( m[ 1 ][ 0 ] * vIn.x + m[ 1 ][ 1 ] * vIn.y + m[ 1 ][ 2 ] * vIn.z + m[ 1 ][ 3 ] ) * rw;
	vOut.z = ( m[ 2 ][ 0 ] * vIn.x + m[ 2 ][ 1 ] * vIn.y + m[ 2 ][ 2 ] * vIn.z + m[ 2 ][ 3 ] ) * rw;
}

//-----------------------------------------------------------------------------
// Other random stuff
//-----------------------------------------------------------------------------
inline void VMatrix::Identity( ) {
	m[ 0 ][ 0 ] = 1.0f; m[ 0 ][ 1 ] = 0.0f; m[ 0 ][ 2 ] = 0.0f; m[ 0 ][ 3 ] = 0.0f;
	m[ 1 ][ 0 ] = 0.0f; m[ 1 ][ 1 ] = 1.0f; m[ 1 ][ 2 ] = 0.0f; m[ 1 ][ 3 ] = 0.0f;
	m[ 2 ][ 0 ] = 0.0f; m[ 2 ][ 1 ] = 0.0f; m[ 2 ][ 2 ] = 1.0f; m[ 2 ][ 3 ] = 0.0f;
	m[ 3 ][ 0 ] = 0.0f; m[ 3 ][ 1 ] = 0.0f; m[ 3 ][ 2 ] = 0.0f; m[ 3 ][ 3 ] = 1.0f;
}


inline bool VMatrix::IsIdentity( ) const {
	return
		m[ 0 ][ 0 ] == 1.0f && m[ 0 ][ 1 ] == 0.0f && m[ 0 ][ 2 ] == 0.0f && m[ 0 ][ 3 ] == 0.0f &&
		m[ 1 ][ 0 ] == 0.0f && m[ 1 ][ 1 ] == 1.0f && m[ 1 ][ 2 ] == 0.0f && m[ 1 ][ 3 ] == 0.0f &&
		m[ 2 ][ 0 ] == 0.0f && m[ 2 ][ 1 ] == 0.0f && m[ 2 ][ 2 ] == 1.0f && m[ 2 ][ 3 ] == 0.0f &&
		m[ 3 ][ 0 ] == 0.0f && m[ 3 ][ 1 ] == 0.0f && m[ 3 ][ 2 ] == 0.0f && m[ 3 ][ 3 ] == 1.0f;
}

inline Vector VMatrix::ApplyRotation( const Vector &vVec ) const {
	return VMul3x3( vVec );
}

matrix3x4_t matrix3x4_t::ConcatTransforms(matrix3x4_t in) const {
	matrix3x4_t out;
	out[0][0] = m_flMatVal[0][0] * in[0][0] + m_flMatVal[0][1] * in[1][0] + m_flMatVal[0][2] * in[2][0];
	out[0][1] = m_flMatVal[0][0] * in[0][1] + m_flMatVal[0][1] * in[1][1] + m_flMatVal[0][2] * in[2][1];
	out[0][2] = m_flMatVal[0][0] * in[0][2] + m_flMatVal[0][1] * in[1][2] + m_flMatVal[0][2] * in[2][2];
	out[0][3] = m_flMatVal[0][0] * in[0][3] + m_flMatVal[0][1] * in[1][3] + m_flMatVal[0][2] * in[2][3] + m_flMatVal[0][3];
	out[1][0] = m_flMatVal[1][0] * in[0][0] + m_flMatVal[1][1] * in[1][0] + m_flMatVal[1][2] * in[2][0];
	out[1][1] = m_flMatVal[1][0] * in[0][1] + m_flMatVal[1][1] * in[1][1] + m_flMatVal[1][2] * in[2][1];
	out[1][2] = m_flMatVal[1][0] * in[0][2] + m_flMatVal[1][1] * in[1][2] + m_flMatVal[1][2] * in[2][2];
	out[1][3] = m_flMatVal[1][0] * in[0][3] + m_flMatVal[1][1] * in[1][3] + m_flMatVal[1][2] * in[2][3] + m_flMatVal[1][3];
	out[2][0] = m_flMatVal[2][0] * in[0][0] + m_flMatVal[2][1] * in[1][0] + m_flMatVal[2][2] * in[2][0];
	out[2][1] = m_flMatVal[2][0] * in[0][1] + m_flMatVal[2][1] * in[1][1] + m_flMatVal[2][2] * in[2][1];
	out[2][2] = m_flMatVal[2][0] * in[0][2] + m_flMatVal[2][1] * in[1][2] + m_flMatVal[2][2] * in[2][2];
	out[2][3] = m_flMatVal[2][0] * in[0][3] + m_flMatVal[2][1] * in[1][3] + m_flMatVal[2][2] * in[2][3] + m_flMatVal[2][3];
	return out;
}

Vector matrix3x4_t::operator*(const Vector& vVec) const {
	Vector vRet;
	vRet.x = m_flMatVal[0][0] * vVec.x + m_flMatVal[0][1] * vVec.y + m_flMatVal[0][2] * vVec.z + m_flMatVal[0][3];
	vRet.y = m_flMatVal[1][0] * vVec.x + m_flMatVal[1][1] * vVec.y + m_flMatVal[1][2] * vVec.z + m_flMatVal[1][3];
	vRet.z = m_flMatVal[2][0] * vVec.x + m_flMatVal[2][1] * vVec.y + m_flMatVal[2][2] * vVec.z + m_flMatVal[2][3];

	return vRet;
}

matrix3x4_t matrix3x4_t::operator+(const matrix3x4_t& other) const {
	matrix3x4_t ret;
	for (int i = 0; i < 12; i++) {
		((float*)ret.m_flMatVal)[i] = ((float*)m_flMatVal)[i] + ((float*)other.m_flMatVal)[i];
	}
	return ret;
}

matrix3x4_t matrix3x4_t::operator-(const matrix3x4_t& other) const {
	matrix3x4_t ret;
	for (int i = 0; i < 12; i++) {
		((float*)ret.m_flMatVal)[i] = ((float*)m_flMatVal)[i] - ((float*)other.m_flMatVal)[i];
	}
	return ret;
}

matrix3x4_t matrix3x4_t::operator*(const float& other) const {
	matrix3x4_t ret;
	for (int i = 0; i < 12; i++) {
		((float*)ret.m_flMatVal)[i] = ((float*)m_flMatVal)[i] * other;
	}
	return ret;
}

void matrix3x4_t::AngleMatrix(const Vector& angles)
{
	float sr, sp, sy, cr, cp, cy;
	DirectX::XMScalarSinCosEst(&sy, &cy, DirectX::XMConvertToRadians(angles[1]));
	DirectX::XMScalarSinCosEst(&sp, &cp, DirectX::XMConvertToRadians(angles[0]));
	DirectX::XMScalarSinCosEst(&sr, &cr, DirectX::XMConvertToRadians(angles[2]));

	m_flMatVal[0][0] = cp * cy;
	m_flMatVal[1][0] = cp * sy;
	m_flMatVal[2][0] = -sp;

	float crcy = cr * cy;
	float crsy = cr * sy;
	float srcy = sr * cy;
	float srsy = sr * sy;
	m_flMatVal[0][1] = sp * srcy - crsy;
	m_flMatVal[1][1] = sp * srsy + crcy;
	m_flMatVal[2][1] = sr * cp;

	m_flMatVal[0][2] = (sp * crcy + srsy);
	m_flMatVal[1][2] = (sp * crsy - srcy);
	m_flMatVal[2][2] = cr * cp;

	m_flMatVal[0][3] = 0.0f;
	m_flMatVal[1][3] = 0.0f;
	m_flMatVal[2][3] = 0.0f;
}

void matrix3x4_t::MatrixSetColumn(const Vector& in, int column)
{
	m_flMatVal[0][column] = in.x;
	m_flMatVal[1][column] = in.y;
	m_flMatVal[2][column] = in.z;
}

void matrix3x4_t::AngleMatrix(const Vector& angles, const Vector& position)
{
	AngleMatrix(angles);
	MatrixSetColumn(position, 3);
}

void matrix3x4_t::QuaternionMatrix(const Quaternion& q)
{
	m_flMatVal[0][0] = 1.0 - 2.0 * q.y * q.y - 2.0 * q.z * q.z;
	m_flMatVal[1][0] = 2.0 * q.x * q.y + 2.0 * q.w * q.z;
	m_flMatVal[2][0] = 2.0 * q.x * q.z - 2.0 * q.w * q.y;

	m_flMatVal[0][1] = 2.0f * q.x * q.y - 2.0f * q.w * q.z;
	m_flMatVal[1][1] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.z * q.z;
	m_flMatVal[2][1] = 2.0f * q.y * q.z + 2.0f * q.w * q.x;

	m_flMatVal[0][2] = 2.0f * q.x * q.z + 2.0f * q.w * q.y;
	m_flMatVal[1][2] = 2.0f * q.y * q.z - 2.0f * q.w * q.x;
	m_flMatVal[2][2] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.y * q.y;

	m_flMatVal[0][3] = 0.0f;
	m_flMatVal[1][3] = 0.0f;
	m_flMatVal[2][3] = 0.0f;
}

void matrix3x4_t::QuaternionMatrix(const Quaternion& q, const Vector& pos)
{
	QuaternionMatrix(q);

	m_flMatVal[0][3] = pos.x;
	m_flMatVal[1][3] = pos.y;
	m_flMatVal[2][3] = pos.z;
}