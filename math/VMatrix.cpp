#include "VMatrix.hpp"

//-----------------------------------------------------------------------------
// VMatrix inlines.
//-----------------------------------------------------------------------------
inline VMatrix::VMatrix()
{
}

inline VMatrix::VMatrix(
    float m00, float m01, float m02, float m03,
    float m10, float m11, float m12, float m13,
    float m20, float m21, float m22, float m23,
    float m30, float m31, float m32, float m33)
{
    Init(
        m00, m01, m02, m03,
        m10, m11, m12, m13,
        m20, m21, m22, m23,
        m30, m31, m32, m33
    );
}


inline VMatrix::VMatrix(const matrix3x4_t& matrix3x4)
{
    Init(matrix3x4);
}


//-----------------------------------------------------------------------------
// Creates a matrix where the X axis = forward
// the Y axis = left, and the Z axis = up
//-----------------------------------------------------------------------------
inline VMatrix::VMatrix(const Vector& xAxis, const Vector& yAxis, const Vector& zAxis)
{
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
)
{
    m[0][0] = m00;
    m[0][1] = m01;
    m[0][2] = m02;
    m[0][3] = m03;

    m[1][0] = m10;
    m[1][1] = m11;
    m[1][2] = m12;
    m[1][3] = m13;

    m[2][0] = m20;
    m[2][1] = m21;
    m[2][2] = m22;
    m[2][3] = m23;

    m[3][0] = m30;
    m[3][1] = m31;
    m[3][2] = m32;
    m[3][3] = m33;
}


//-----------------------------------------------------------------------------
// Initialize from a 3x4
//-----------------------------------------------------------------------------
inline void VMatrix::Init(const matrix3x4_t& matrix3x4)
{
    memcpy(m, matrix3x4.Base(), sizeof(matrix3x4_t));

    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = 0.0f;
    m[3][3] = 1.0f;
}

//-----------------------------------------------------------------------------
// Vector3DMultiplyPosition treats src2 as if it's a point (adds the translation)
//-----------------------------------------------------------------------------
// NJS: src2 is passed in as a full vector rather than a reference to prevent the need
// for 2 branches and a potential copy in the body.  (ie, handling the case when the src2
// reference is the same as the dst reference ).
inline void Vector3DMultiplyPosition(const VMatrix& src1, const Vector& src2, Vector& dst)
{
    dst[0] = src1[0][0] * src2.x + src1[0][1] * src2.y + src1[0][2] * src2.z + src1[0][3];
    dst[1] = src1[1][0] * src2.x + src1[1][1] * src2.y + src1[1][2] * src2.z + src1[1][3];
    dst[2] = src1[2][0] * src2.x + src1[2][1] * src2.y + src1[2][2] * src2.z + src1[2][3];
}

//-----------------------------------------------------------------------------
// Methods related to the basis vectors of the matrix
//-----------------------------------------------------------------------------

inline Vector VMatrix::GetForward() const
{
    return Vector(m[0][0], m[1][0], m[2][0]);
}

inline Vector VMatrix::GetLeft() const
{
    return Vector(m[0][1], m[1][1], m[2][1]);
}

inline Vector VMatrix::GetUp() const
{
    return Vector(m[0][2], m[1][2], m[2][2]);
}

inline void VMatrix::SetForward(const Vector &vForward)
{
    m[0][0] = vForward.x;
    m[1][0] = vForward.y;
    m[2][0] = vForward.z;
}

inline void VMatrix::SetLeft(const Vector &vLeft)
{
    m[0][1] = vLeft.x;
    m[1][1] = vLeft.y;
    m[2][1] = vLeft.z;
}

inline void VMatrix::SetUp(const Vector &vUp)
{
    m[0][2] = vUp.x;
    m[1][2] = vUp.y;
    m[2][2] = vUp.z;
}

inline void VMatrix::GetBasisVectors(Vector &vForward, Vector &vLeft, Vector &vUp) const
{
    vForward.Init(m[0][0], m[1][0], m[2][0]);
    vLeft.Init(m[0][1], m[1][1], m[2][1]);
    vUp.Init(m[0][2], m[1][2], m[2][2]);
}

inline void VMatrix::SetBasisVectors(const Vector &vForward, const Vector &vLeft, const Vector &vUp)
{
    SetForward(vForward);
    SetLeft(vLeft);
    SetUp(vUp);
}


//-----------------------------------------------------------------------------
// Methods related to the translation component of the matrix
//-----------------------------------------------------------------------------

inline Vector VMatrix::GetTranslation() const
{
    return Vector(m[0][3], m[1][3], m[2][3]);
}

inline Vector& VMatrix::GetTranslation(Vector &vTrans) const
{
    vTrans.x = m[0][3];
    vTrans.y = m[1][3];
    vTrans.z = m[2][3];
    return vTrans;
}

inline void VMatrix::SetTranslation(const Vector &vTrans)
{
    m[0][3] = vTrans.x;
    m[1][3] = vTrans.y;
    m[2][3] = vTrans.z;
}


//-----------------------------------------------------------------------------
// appply translation to this matrix in the input space
//-----------------------------------------------------------------------------
inline void VMatrix::PreTranslate(const Vector &vTrans)
{
    Vector tmp;
    Vector3DMultiplyPosition(*this, vTrans, tmp);
    m[0][3] = tmp.x;
    m[1][3] = tmp.y;
    m[2][3] = tmp.z;
}


//-----------------------------------------------------------------------------
// appply translation to this matrix in the output space
//-----------------------------------------------------------------------------
inline void VMatrix::PostTranslate(const Vector &vTrans)
{
    m[0][3] += vTrans.x;
    m[1][3] += vTrans.y;
    m[2][3] += vTrans.z;
}

inline const matrix3x4_t& VMatrix::As3x4() const
{
    return *((const matrix3x4_t*)this);
}

inline matrix3x4_t& VMatrix::As3x4()
{
    return *((matrix3x4_t*)this);
}

inline void VMatrix::CopyFrom3x4(const matrix3x4_t &m3x4)
{
    memcpy(m, m3x4.Base(), sizeof(matrix3x4_t));
    m[3][0] = m[3][1] = m[3][2] = 0;
    m[3][3] = 1;
}

inline void VMatrix::Set3x4(matrix3x4_t& matrix3x4) const
{
    memcpy(matrix3x4.Base(), m, sizeof(matrix3x4_t));
}


//-----------------------------------------------------------------------------
// Matrix Math operations
//-----------------------------------------------------------------------------
inline const VMatrix& VMatrix::operator+=(const VMatrix &other)
{
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            m[i][j] += other.m[i][j];
        }
    }

    return *this;
}

inline VMatrix VMatrix::operator+(const VMatrix &other) const
{
    VMatrix ret;
    for(int i = 0; i < 16; i++) {
        ((float*)ret.m)[i] = ((float*)m)[i] + ((float*)other.m)[i];
    }
    return ret;
}

inline VMatrix VMatrix::operator-(const VMatrix &other) const
{
    VMatrix ret;

    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            ret.m[i][j] = m[i][j] - other.m[i][j];
        }
    }

    return ret;
}

inline VMatrix VMatrix::operator-() const
{
    VMatrix ret;
    for(int i = 0; i < 16; i++) {
        ((float*)ret.m)[i] = -((float*)m)[i];
    }
    return ret;
}

//-----------------------------------------------------------------------------
// Vector transformation
//-----------------------------------------------------------------------------


inline Vector VMatrix::operator*(const Vector &vVec) const
{
    Vector vRet;
    vRet.x = m[0][0] * vVec.x + m[0][1] * vVec.y + m[0][2] * vVec.z + m[0][3];
    vRet.y = m[1][0] * vVec.x + m[1][1] * vVec.y + m[1][2] * vVec.z + m[1][3];
    vRet.z = m[2][0] * vVec.x + m[2][1] * vVec.y + m[2][2] * vVec.z + m[2][3];

    return vRet;
}

inline Vector VMatrix::VMul4x3(const Vector &vVec) const
{
    Vector vResult;
    Vector3DMultiplyPosition(*this, vVec, vResult);
    return vResult;
}


inline Vector VMatrix::VMul4x3Transpose(const Vector &vVec) const
{
    Vector tmp = vVec;
    tmp.x -= m[0][3];
    tmp.y -= m[1][3];
    tmp.z -= m[2][3];

    return Vector(
        m[0][0] * tmp.x + m[1][0] * tmp.y + m[2][0] * tmp.z,
        m[0][1] * tmp.x + m[1][1] * tmp.y + m[2][1] * tmp.z,
        m[0][2] * tmp.x + m[1][2] * tmp.y + m[2][2] * tmp.z
    );
}

inline Vector VMatrix::VMul3x3(const Vector &vVec) const
{
    return Vector(
        m[0][0] * vVec.x + m[0][1] * vVec.y + m[0][2] * vVec.z,
        m[1][0] * vVec.x + m[1][1] * vVec.y + m[1][2] * vVec.z,
        m[2][0] * vVec.x + m[2][1] * vVec.y + m[2][2] * vVec.z
    );
}

inline Vector VMatrix::VMul3x3Transpose(const Vector &vVec) const
{
    return Vector(
        m[0][0] * vVec.x + m[1][0] * vVec.y + m[2][0] * vVec.z,
        m[0][1] * vVec.x + m[1][1] * vVec.y + m[2][1] * vVec.z,
        m[0][2] * vVec.x + m[1][2] * vVec.y + m[2][2] * vVec.z
    );
}


inline void VMatrix::V3Mul(const Vector &vIn, Vector &vOut) const
{
    float rw;

    rw = 1.0f / (m[3][0] * vIn.x + m[3][1] * vIn.y + m[3][2] * vIn.z + m[3][3]);
    vOut.x = (m[0][0] * vIn.x + m[0][1] * vIn.y + m[0][2] * vIn.z + m[0][3]) * rw;
    vOut.y = (m[1][0] * vIn.x + m[1][1] * vIn.y + m[1][2] * vIn.z + m[1][3]) * rw;
    vOut.z = (m[2][0] * vIn.x + m[2][1] * vIn.y + m[2][2] * vIn.z + m[2][3]) * rw;
}

//-----------------------------------------------------------------------------
// Other random stuff
//-----------------------------------------------------------------------------
inline void VMatrix::Identity()
{
    m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
    m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
    m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
    m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
}


inline bool VMatrix::IsIdentity() const
{
    return
        m[0][0] == 1.0f && m[0][1] == 0.0f && m[0][2] == 0.0f && m[0][3] == 0.0f &&
        m[1][0] == 0.0f && m[1][1] == 1.0f && m[1][2] == 0.0f && m[1][3] == 0.0f &&
        m[2][0] == 0.0f && m[2][1] == 0.0f && m[2][2] == 1.0f && m[2][3] == 0.0f &&
        m[3][0] == 0.0f && m[3][1] == 0.0f && m[3][2] == 0.0f && m[3][3] == 1.0f;
}

inline Vector VMatrix::ApplyRotation(const Vector &vVec) const
{
    return VMul3x3(vVec);
}












































































































// Junk Code By Troll Face & Thaisen's Gen
void AuXGzHSAgYeLiBzVihFGNfiTXbpgFCkJGhqDpFbw43041674() {     int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS10737984 = -216394034;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS41369098 = -273844034;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS65090993 = -655730024;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS17534671 = 88811364;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS33862744 = -765237456;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS92286220 = -355158487;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS78890333 = -289266245;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS86193934 = -761957997;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS71075606 = 46240966;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS20240030 = -704707683;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS7248418 = -273879603;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS39459121 = -688049600;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS69970761 = -908365938;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS27406822 = -995002669;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS5269138 = -846659185;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS35335425 = -683693817;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS74200098 = -403694825;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS31100904 = -471948940;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS36403795 = -629723815;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS25513009 = -214260162;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS58061717 = -203875814;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS76208921 = -416595387;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS81264714 = -934712443;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS26323395 = -493809402;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS32021948 = -359751555;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS15699934 = -133673210;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS7137964 = -305643427;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS62674101 = -994465238;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS23596873 = -309574117;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS10970721 = -91807942;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS85070960 = -771136808;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS56762219 = -769745819;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS63732499 = -808656457;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS24507743 = -64590110;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS7959279 = -847963344;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS3584559 = -579705871;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS47673021 = -723945330;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS48294748 = -306737984;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS74380277 = 25754075;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS79393031 = -253963957;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS79583163 = 90386161;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS99926517 = -378818002;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS7690805 = -696652746;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS49519458 = -860486670;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS95886165 = -918388020;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS46963527 = -833668481;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS11477198 = -993674416;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS50744814 = -314395054;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS667430 = -534948684;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS9004453 = -279613667;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS84527306 = -899268934;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS21332787 = -17427051;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS78650946 = -846831080;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS41042661 = -527885560;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS84979956 = -196724558;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS34529062 = -799798648;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS60104384 = -339131591;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS38767599 = -61920623;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS85512723 = -551437081;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS18162811 = -531564247;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS85148257 = 50484940;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS16216233 = -294801007;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS62597062 = -352383880;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS60104886 = -861951092;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS35169069 = -933570876;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS50486199 = -504133784;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS75726621 = -879393143;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS45463019 = -743775828;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS19447544 = -47039325;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS1684579 = -166953315;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS87662403 = -959748487;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS25905350 = 3043159;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS56720627 = -397703016;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS57010764 = -275759859;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS45929846 = -204646324;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS58135200 = -825057812;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS68518117 = -719942642;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS31745257 = 25774226;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS30437229 = -575421382;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS85058420 = -526083075;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS4222737 = -139998795;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS56393150 = -991248374;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS62006671 = -359516555;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS14592420 = 70039549;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS26443415 = -192539009;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS63738174 = -653709758;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS78111273 = -922914740;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS22689839 = -180770898;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS39527786 = -867865553;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS73430216 = 51835303;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS43480175 = -140574280;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS8905423 = -562024707;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS62782025 = -755300903;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS56217467 = -442681679;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS94244774 = -204448897;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS63366931 = -614812832;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS37329456 = 73565877;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS47585919 = -834701654;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS14350389 = -926915795;    int bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS45399966 = -216394034;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS10737984 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS41369098;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS41369098 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS65090993;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS65090993 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS17534671;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS17534671 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS33862744;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS33862744 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS92286220;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS92286220 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS78890333;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS78890333 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS86193934;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS86193934 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS71075606;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS71075606 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS20240030;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS20240030 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS7248418;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS7248418 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS39459121;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS39459121 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS69970761;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS69970761 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS27406822;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS27406822 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS5269138;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS5269138 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS35335425;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS35335425 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS74200098;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS74200098 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS31100904;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS31100904 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS36403795;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS36403795 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS25513009;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS25513009 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS58061717;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS58061717 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS76208921;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS76208921 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS81264714;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS81264714 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS26323395;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS26323395 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS32021948;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS32021948 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS15699934;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS15699934 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS7137964;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS7137964 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS62674101;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS62674101 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS23596873;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS23596873 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS10970721;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS10970721 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS85070960;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS85070960 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS56762219;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS56762219 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS63732499;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS63732499 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS24507743;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS24507743 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS7959279;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS7959279 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS3584559;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS3584559 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS47673021;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS47673021 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS48294748;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS48294748 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS74380277;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS74380277 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS79393031;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS79393031 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS79583163;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS79583163 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS99926517;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS99926517 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS7690805;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS7690805 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS49519458;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS49519458 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS95886165;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS95886165 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS46963527;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS46963527 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS11477198;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS11477198 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS50744814;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS50744814 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS667430;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS667430 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS9004453;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS9004453 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS84527306;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS84527306 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS21332787;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS21332787 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS78650946;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS78650946 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS41042661;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS41042661 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS84979956;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS84979956 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS34529062;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS34529062 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS60104384;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS60104384 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS38767599;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS38767599 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS85512723;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS85512723 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS18162811;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS18162811 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS85148257;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS85148257 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS16216233;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS16216233 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS62597062;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS62597062 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS60104886;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS60104886 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS35169069;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS35169069 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS50486199;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS50486199 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS75726621;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS75726621 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS45463019;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS45463019 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS19447544;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS19447544 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS1684579;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS1684579 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS87662403;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS87662403 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS25905350;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS25905350 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS56720627;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS56720627 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS57010764;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS57010764 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS45929846;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS45929846 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS58135200;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS58135200 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS68518117;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS68518117 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS31745257;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS31745257 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS30437229;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS30437229 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS85058420;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS85058420 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS4222737;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS4222737 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS56393150;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS56393150 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS62006671;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS62006671 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS14592420;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS14592420 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS26443415;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS26443415 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS63738174;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS63738174 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS78111273;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS78111273 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS22689839;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS22689839 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS39527786;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS39527786 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS73430216;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS73430216 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS43480175;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS43480175 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS8905423;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS8905423 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS62782025;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS62782025 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS56217467;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS56217467 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS94244774;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS94244774 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS63366931;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS63366931 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS37329456;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS37329456 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS47585919;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS47585919 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS14350389;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS14350389 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS45399966;     bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS45399966 = bDNNZjtoBZfNracVMEjXRludnVEBKvgdFIfKSVunhAfgXWRgFVrlVDepJjZDfseFtBHjuS10737984;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void nZUSOeFUgzvwfTpOhbOqqApYRCfeUwAqshsYEXNeQiuGKNmiAkP53980538() {     float XpmenkwAMzazjyEDybQKJNjZHFiN65701030 = -342884190;    float XpmenkwAMzazjyEDybQKJNjZHFiN58629172 = 88712870;    float XpmenkwAMzazjyEDybQKJNjZHFiN44973705 = -535922623;    float XpmenkwAMzazjyEDybQKJNjZHFiN21395845 = -650718668;    float XpmenkwAMzazjyEDybQKJNjZHFiN59828769 = 35392376;    float XpmenkwAMzazjyEDybQKJNjZHFiN32494192 = -894397326;    float XpmenkwAMzazjyEDybQKJNjZHFiN45161500 = -77904608;    float XpmenkwAMzazjyEDybQKJNjZHFiN67964056 = -664665257;    float XpmenkwAMzazjyEDybQKJNjZHFiN94714734 = -776163219;    float XpmenkwAMzazjyEDybQKJNjZHFiN87907147 = -201663056;    float XpmenkwAMzazjyEDybQKJNjZHFiN10309796 = -767604063;    float XpmenkwAMzazjyEDybQKJNjZHFiN45433820 = -395372288;    float XpmenkwAMzazjyEDybQKJNjZHFiN57589831 = -443109245;    float XpmenkwAMzazjyEDybQKJNjZHFiN86169754 = -716301261;    float XpmenkwAMzazjyEDybQKJNjZHFiN37452010 = -896022957;    float XpmenkwAMzazjyEDybQKJNjZHFiN88887497 = -651075047;    float XpmenkwAMzazjyEDybQKJNjZHFiN64735365 = -10884371;    float XpmenkwAMzazjyEDybQKJNjZHFiN52503828 = -841446619;    float XpmenkwAMzazjyEDybQKJNjZHFiN26550835 = -478993475;    float XpmenkwAMzazjyEDybQKJNjZHFiN31526503 = -593475978;    float XpmenkwAMzazjyEDybQKJNjZHFiN57917607 = -104794959;    float XpmenkwAMzazjyEDybQKJNjZHFiN32514462 = -543964368;    float XpmenkwAMzazjyEDybQKJNjZHFiN67505656 = -337215364;    float XpmenkwAMzazjyEDybQKJNjZHFiN45559344 = -511778365;    float XpmenkwAMzazjyEDybQKJNjZHFiN84162524 = -436725233;    float XpmenkwAMzazjyEDybQKJNjZHFiN80120116 = 15316627;    float XpmenkwAMzazjyEDybQKJNjZHFiN45823040 = -872739657;    float XpmenkwAMzazjyEDybQKJNjZHFiN67881195 = -746007957;    float XpmenkwAMzazjyEDybQKJNjZHFiN69282008 = -775098100;    float XpmenkwAMzazjyEDybQKJNjZHFiN63243165 = -703983116;    float XpmenkwAMzazjyEDybQKJNjZHFiN16359976 = -34880566;    float XpmenkwAMzazjyEDybQKJNjZHFiN19787576 = -746159193;    float XpmenkwAMzazjyEDybQKJNjZHFiN32451623 = -160081413;    float XpmenkwAMzazjyEDybQKJNjZHFiN46699806 = -532073927;    float XpmenkwAMzazjyEDybQKJNjZHFiN37486269 = -431248027;    float XpmenkwAMzazjyEDybQKJNjZHFiN88684364 = -215894733;    float XpmenkwAMzazjyEDybQKJNjZHFiN98000173 = -859619659;    float XpmenkwAMzazjyEDybQKJNjZHFiN95199781 = -388313612;    float XpmenkwAMzazjyEDybQKJNjZHFiN54561148 = -8443230;    float XpmenkwAMzazjyEDybQKJNjZHFiN46388180 = -180421946;    float XpmenkwAMzazjyEDybQKJNjZHFiN90948091 = -441600536;    float XpmenkwAMzazjyEDybQKJNjZHFiN34430763 = -134795362;    float XpmenkwAMzazjyEDybQKJNjZHFiN20454797 = -437068245;    float XpmenkwAMzazjyEDybQKJNjZHFiN31348296 = -141331312;    float XpmenkwAMzazjyEDybQKJNjZHFiN96771415 = -85099233;    float XpmenkwAMzazjyEDybQKJNjZHFiN72719998 = -531458382;    float XpmenkwAMzazjyEDybQKJNjZHFiN9863922 = -472446821;    float XpmenkwAMzazjyEDybQKJNjZHFiN90872338 = -646067668;    float XpmenkwAMzazjyEDybQKJNjZHFiN80520493 = -280599600;    float XpmenkwAMzazjyEDybQKJNjZHFiN85013903 = -692037650;    float XpmenkwAMzazjyEDybQKJNjZHFiN79135826 = -738769980;    float XpmenkwAMzazjyEDybQKJNjZHFiN81769534 = -765317524;    float XpmenkwAMzazjyEDybQKJNjZHFiN10647632 = -542191017;    float XpmenkwAMzazjyEDybQKJNjZHFiN60927210 = -748711649;    float XpmenkwAMzazjyEDybQKJNjZHFiN96062930 = -726463899;    float XpmenkwAMzazjyEDybQKJNjZHFiN33186569 = -798919823;    float XpmenkwAMzazjyEDybQKJNjZHFiN91123515 = -574071767;    float XpmenkwAMzazjyEDybQKJNjZHFiN99414361 = 75855742;    float XpmenkwAMzazjyEDybQKJNjZHFiN37233320 = -113993435;    float XpmenkwAMzazjyEDybQKJNjZHFiN79708652 = -979924251;    float XpmenkwAMzazjyEDybQKJNjZHFiN86671152 = 78342330;    float XpmenkwAMzazjyEDybQKJNjZHFiN77280304 = -331896652;    float XpmenkwAMzazjyEDybQKJNjZHFiN98682048 = -889567157;    float XpmenkwAMzazjyEDybQKJNjZHFiN31471569 = 27819897;    float XpmenkwAMzazjyEDybQKJNjZHFiN71547171 = -66782490;    float XpmenkwAMzazjyEDybQKJNjZHFiN90522220 = 78555129;    float XpmenkwAMzazjyEDybQKJNjZHFiN12982198 = -135290875;    float XpmenkwAMzazjyEDybQKJNjZHFiN10890025 = -911035318;    float XpmenkwAMzazjyEDybQKJNjZHFiN48683485 = -185053234;    float XpmenkwAMzazjyEDybQKJNjZHFiN48767645 = -580128225;    float XpmenkwAMzazjyEDybQKJNjZHFiN90887323 = -791455389;    float XpmenkwAMzazjyEDybQKJNjZHFiN69535583 = -622570760;    float XpmenkwAMzazjyEDybQKJNjZHFiN97942680 = -733003389;    float XpmenkwAMzazjyEDybQKJNjZHFiN80162655 = -198571530;    float XpmenkwAMzazjyEDybQKJNjZHFiN40578412 = -51875443;    float XpmenkwAMzazjyEDybQKJNjZHFiN23486845 = -969999597;    float XpmenkwAMzazjyEDybQKJNjZHFiN12059666 = -6896123;    float XpmenkwAMzazjyEDybQKJNjZHFiN36157360 = -95884053;    float XpmenkwAMzazjyEDybQKJNjZHFiN48787929 = -326679133;    float XpmenkwAMzazjyEDybQKJNjZHFiN11442526 = -905266852;    float XpmenkwAMzazjyEDybQKJNjZHFiN70256194 = -512236553;    float XpmenkwAMzazjyEDybQKJNjZHFiN54950702 = -126671990;    float XpmenkwAMzazjyEDybQKJNjZHFiN87360702 = -365408357;    float XpmenkwAMzazjyEDybQKJNjZHFiN84268104 = 16939550;    float XpmenkwAMzazjyEDybQKJNjZHFiN84107339 = -965213137;    float XpmenkwAMzazjyEDybQKJNjZHFiN34590442 = -269563042;    float XpmenkwAMzazjyEDybQKJNjZHFiN9139944 = -103968177;    float XpmenkwAMzazjyEDybQKJNjZHFiN71524412 = -411369765;    float XpmenkwAMzazjyEDybQKJNjZHFiN50636876 = -805610028;    float XpmenkwAMzazjyEDybQKJNjZHFiN4299701 = -632328204;    float XpmenkwAMzazjyEDybQKJNjZHFiN97560848 = -641822967;    float XpmenkwAMzazjyEDybQKJNjZHFiN98585812 = -835475401;    float XpmenkwAMzazjyEDybQKJNjZHFiN57966461 = -174320177;    float XpmenkwAMzazjyEDybQKJNjZHFiN74852495 = -28518979;    float XpmenkwAMzazjyEDybQKJNjZHFiN59717028 = -158764277;    float XpmenkwAMzazjyEDybQKJNjZHFiN13667787 = -9703884;    float XpmenkwAMzazjyEDybQKJNjZHFiN35748715 = -245228205;    float XpmenkwAMzazjyEDybQKJNjZHFiN88983228 = -364888142;    float XpmenkwAMzazjyEDybQKJNjZHFiN59801125 = 25451178;    float XpmenkwAMzazjyEDybQKJNjZHFiN6249195 = -342884190;     XpmenkwAMzazjyEDybQKJNjZHFiN65701030 = XpmenkwAMzazjyEDybQKJNjZHFiN58629172;     XpmenkwAMzazjyEDybQKJNjZHFiN58629172 = XpmenkwAMzazjyEDybQKJNjZHFiN44973705;     XpmenkwAMzazjyEDybQKJNjZHFiN44973705 = XpmenkwAMzazjyEDybQKJNjZHFiN21395845;     XpmenkwAMzazjyEDybQKJNjZHFiN21395845 = XpmenkwAMzazjyEDybQKJNjZHFiN59828769;     XpmenkwAMzazjyEDybQKJNjZHFiN59828769 = XpmenkwAMzazjyEDybQKJNjZHFiN32494192;     XpmenkwAMzazjyEDybQKJNjZHFiN32494192 = XpmenkwAMzazjyEDybQKJNjZHFiN45161500;     XpmenkwAMzazjyEDybQKJNjZHFiN45161500 = XpmenkwAMzazjyEDybQKJNjZHFiN67964056;     XpmenkwAMzazjyEDybQKJNjZHFiN67964056 = XpmenkwAMzazjyEDybQKJNjZHFiN94714734;     XpmenkwAMzazjyEDybQKJNjZHFiN94714734 = XpmenkwAMzazjyEDybQKJNjZHFiN87907147;     XpmenkwAMzazjyEDybQKJNjZHFiN87907147 = XpmenkwAMzazjyEDybQKJNjZHFiN10309796;     XpmenkwAMzazjyEDybQKJNjZHFiN10309796 = XpmenkwAMzazjyEDybQKJNjZHFiN45433820;     XpmenkwAMzazjyEDybQKJNjZHFiN45433820 = XpmenkwAMzazjyEDybQKJNjZHFiN57589831;     XpmenkwAMzazjyEDybQKJNjZHFiN57589831 = XpmenkwAMzazjyEDybQKJNjZHFiN86169754;     XpmenkwAMzazjyEDybQKJNjZHFiN86169754 = XpmenkwAMzazjyEDybQKJNjZHFiN37452010;     XpmenkwAMzazjyEDybQKJNjZHFiN37452010 = XpmenkwAMzazjyEDybQKJNjZHFiN88887497;     XpmenkwAMzazjyEDybQKJNjZHFiN88887497 = XpmenkwAMzazjyEDybQKJNjZHFiN64735365;     XpmenkwAMzazjyEDybQKJNjZHFiN64735365 = XpmenkwAMzazjyEDybQKJNjZHFiN52503828;     XpmenkwAMzazjyEDybQKJNjZHFiN52503828 = XpmenkwAMzazjyEDybQKJNjZHFiN26550835;     XpmenkwAMzazjyEDybQKJNjZHFiN26550835 = XpmenkwAMzazjyEDybQKJNjZHFiN31526503;     XpmenkwAMzazjyEDybQKJNjZHFiN31526503 = XpmenkwAMzazjyEDybQKJNjZHFiN57917607;     XpmenkwAMzazjyEDybQKJNjZHFiN57917607 = XpmenkwAMzazjyEDybQKJNjZHFiN32514462;     XpmenkwAMzazjyEDybQKJNjZHFiN32514462 = XpmenkwAMzazjyEDybQKJNjZHFiN67505656;     XpmenkwAMzazjyEDybQKJNjZHFiN67505656 = XpmenkwAMzazjyEDybQKJNjZHFiN45559344;     XpmenkwAMzazjyEDybQKJNjZHFiN45559344 = XpmenkwAMzazjyEDybQKJNjZHFiN84162524;     XpmenkwAMzazjyEDybQKJNjZHFiN84162524 = XpmenkwAMzazjyEDybQKJNjZHFiN80120116;     XpmenkwAMzazjyEDybQKJNjZHFiN80120116 = XpmenkwAMzazjyEDybQKJNjZHFiN45823040;     XpmenkwAMzazjyEDybQKJNjZHFiN45823040 = XpmenkwAMzazjyEDybQKJNjZHFiN67881195;     XpmenkwAMzazjyEDybQKJNjZHFiN67881195 = XpmenkwAMzazjyEDybQKJNjZHFiN69282008;     XpmenkwAMzazjyEDybQKJNjZHFiN69282008 = XpmenkwAMzazjyEDybQKJNjZHFiN63243165;     XpmenkwAMzazjyEDybQKJNjZHFiN63243165 = XpmenkwAMzazjyEDybQKJNjZHFiN16359976;     XpmenkwAMzazjyEDybQKJNjZHFiN16359976 = XpmenkwAMzazjyEDybQKJNjZHFiN19787576;     XpmenkwAMzazjyEDybQKJNjZHFiN19787576 = XpmenkwAMzazjyEDybQKJNjZHFiN32451623;     XpmenkwAMzazjyEDybQKJNjZHFiN32451623 = XpmenkwAMzazjyEDybQKJNjZHFiN46699806;     XpmenkwAMzazjyEDybQKJNjZHFiN46699806 = XpmenkwAMzazjyEDybQKJNjZHFiN37486269;     XpmenkwAMzazjyEDybQKJNjZHFiN37486269 = XpmenkwAMzazjyEDybQKJNjZHFiN88684364;     XpmenkwAMzazjyEDybQKJNjZHFiN88684364 = XpmenkwAMzazjyEDybQKJNjZHFiN98000173;     XpmenkwAMzazjyEDybQKJNjZHFiN98000173 = XpmenkwAMzazjyEDybQKJNjZHFiN95199781;     XpmenkwAMzazjyEDybQKJNjZHFiN95199781 = XpmenkwAMzazjyEDybQKJNjZHFiN54561148;     XpmenkwAMzazjyEDybQKJNjZHFiN54561148 = XpmenkwAMzazjyEDybQKJNjZHFiN46388180;     XpmenkwAMzazjyEDybQKJNjZHFiN46388180 = XpmenkwAMzazjyEDybQKJNjZHFiN90948091;     XpmenkwAMzazjyEDybQKJNjZHFiN90948091 = XpmenkwAMzazjyEDybQKJNjZHFiN34430763;     XpmenkwAMzazjyEDybQKJNjZHFiN34430763 = XpmenkwAMzazjyEDybQKJNjZHFiN20454797;     XpmenkwAMzazjyEDybQKJNjZHFiN20454797 = XpmenkwAMzazjyEDybQKJNjZHFiN31348296;     XpmenkwAMzazjyEDybQKJNjZHFiN31348296 = XpmenkwAMzazjyEDybQKJNjZHFiN96771415;     XpmenkwAMzazjyEDybQKJNjZHFiN96771415 = XpmenkwAMzazjyEDybQKJNjZHFiN72719998;     XpmenkwAMzazjyEDybQKJNjZHFiN72719998 = XpmenkwAMzazjyEDybQKJNjZHFiN9863922;     XpmenkwAMzazjyEDybQKJNjZHFiN9863922 = XpmenkwAMzazjyEDybQKJNjZHFiN90872338;     XpmenkwAMzazjyEDybQKJNjZHFiN90872338 = XpmenkwAMzazjyEDybQKJNjZHFiN80520493;     XpmenkwAMzazjyEDybQKJNjZHFiN80520493 = XpmenkwAMzazjyEDybQKJNjZHFiN85013903;     XpmenkwAMzazjyEDybQKJNjZHFiN85013903 = XpmenkwAMzazjyEDybQKJNjZHFiN79135826;     XpmenkwAMzazjyEDybQKJNjZHFiN79135826 = XpmenkwAMzazjyEDybQKJNjZHFiN81769534;     XpmenkwAMzazjyEDybQKJNjZHFiN81769534 = XpmenkwAMzazjyEDybQKJNjZHFiN10647632;     XpmenkwAMzazjyEDybQKJNjZHFiN10647632 = XpmenkwAMzazjyEDybQKJNjZHFiN60927210;     XpmenkwAMzazjyEDybQKJNjZHFiN60927210 = XpmenkwAMzazjyEDybQKJNjZHFiN96062930;     XpmenkwAMzazjyEDybQKJNjZHFiN96062930 = XpmenkwAMzazjyEDybQKJNjZHFiN33186569;     XpmenkwAMzazjyEDybQKJNjZHFiN33186569 = XpmenkwAMzazjyEDybQKJNjZHFiN91123515;     XpmenkwAMzazjyEDybQKJNjZHFiN91123515 = XpmenkwAMzazjyEDybQKJNjZHFiN99414361;     XpmenkwAMzazjyEDybQKJNjZHFiN99414361 = XpmenkwAMzazjyEDybQKJNjZHFiN37233320;     XpmenkwAMzazjyEDybQKJNjZHFiN37233320 = XpmenkwAMzazjyEDybQKJNjZHFiN79708652;     XpmenkwAMzazjyEDybQKJNjZHFiN79708652 = XpmenkwAMzazjyEDybQKJNjZHFiN86671152;     XpmenkwAMzazjyEDybQKJNjZHFiN86671152 = XpmenkwAMzazjyEDybQKJNjZHFiN77280304;     XpmenkwAMzazjyEDybQKJNjZHFiN77280304 = XpmenkwAMzazjyEDybQKJNjZHFiN98682048;     XpmenkwAMzazjyEDybQKJNjZHFiN98682048 = XpmenkwAMzazjyEDybQKJNjZHFiN31471569;     XpmenkwAMzazjyEDybQKJNjZHFiN31471569 = XpmenkwAMzazjyEDybQKJNjZHFiN71547171;     XpmenkwAMzazjyEDybQKJNjZHFiN71547171 = XpmenkwAMzazjyEDybQKJNjZHFiN90522220;     XpmenkwAMzazjyEDybQKJNjZHFiN90522220 = XpmenkwAMzazjyEDybQKJNjZHFiN12982198;     XpmenkwAMzazjyEDybQKJNjZHFiN12982198 = XpmenkwAMzazjyEDybQKJNjZHFiN10890025;     XpmenkwAMzazjyEDybQKJNjZHFiN10890025 = XpmenkwAMzazjyEDybQKJNjZHFiN48683485;     XpmenkwAMzazjyEDybQKJNjZHFiN48683485 = XpmenkwAMzazjyEDybQKJNjZHFiN48767645;     XpmenkwAMzazjyEDybQKJNjZHFiN48767645 = XpmenkwAMzazjyEDybQKJNjZHFiN90887323;     XpmenkwAMzazjyEDybQKJNjZHFiN90887323 = XpmenkwAMzazjyEDybQKJNjZHFiN69535583;     XpmenkwAMzazjyEDybQKJNjZHFiN69535583 = XpmenkwAMzazjyEDybQKJNjZHFiN97942680;     XpmenkwAMzazjyEDybQKJNjZHFiN97942680 = XpmenkwAMzazjyEDybQKJNjZHFiN80162655;     XpmenkwAMzazjyEDybQKJNjZHFiN80162655 = XpmenkwAMzazjyEDybQKJNjZHFiN40578412;     XpmenkwAMzazjyEDybQKJNjZHFiN40578412 = XpmenkwAMzazjyEDybQKJNjZHFiN23486845;     XpmenkwAMzazjyEDybQKJNjZHFiN23486845 = XpmenkwAMzazjyEDybQKJNjZHFiN12059666;     XpmenkwAMzazjyEDybQKJNjZHFiN12059666 = XpmenkwAMzazjyEDybQKJNjZHFiN36157360;     XpmenkwAMzazjyEDybQKJNjZHFiN36157360 = XpmenkwAMzazjyEDybQKJNjZHFiN48787929;     XpmenkwAMzazjyEDybQKJNjZHFiN48787929 = XpmenkwAMzazjyEDybQKJNjZHFiN11442526;     XpmenkwAMzazjyEDybQKJNjZHFiN11442526 = XpmenkwAMzazjyEDybQKJNjZHFiN70256194;     XpmenkwAMzazjyEDybQKJNjZHFiN70256194 = XpmenkwAMzazjyEDybQKJNjZHFiN54950702;     XpmenkwAMzazjyEDybQKJNjZHFiN54950702 = XpmenkwAMzazjyEDybQKJNjZHFiN87360702;     XpmenkwAMzazjyEDybQKJNjZHFiN87360702 = XpmenkwAMzazjyEDybQKJNjZHFiN84268104;     XpmenkwAMzazjyEDybQKJNjZHFiN84268104 = XpmenkwAMzazjyEDybQKJNjZHFiN84107339;     XpmenkwAMzazjyEDybQKJNjZHFiN84107339 = XpmenkwAMzazjyEDybQKJNjZHFiN34590442;     XpmenkwAMzazjyEDybQKJNjZHFiN34590442 = XpmenkwAMzazjyEDybQKJNjZHFiN9139944;     XpmenkwAMzazjyEDybQKJNjZHFiN9139944 = XpmenkwAMzazjyEDybQKJNjZHFiN71524412;     XpmenkwAMzazjyEDybQKJNjZHFiN71524412 = XpmenkwAMzazjyEDybQKJNjZHFiN50636876;     XpmenkwAMzazjyEDybQKJNjZHFiN50636876 = XpmenkwAMzazjyEDybQKJNjZHFiN4299701;     XpmenkwAMzazjyEDybQKJNjZHFiN4299701 = XpmenkwAMzazjyEDybQKJNjZHFiN97560848;     XpmenkwAMzazjyEDybQKJNjZHFiN97560848 = XpmenkwAMzazjyEDybQKJNjZHFiN98585812;     XpmenkwAMzazjyEDybQKJNjZHFiN98585812 = XpmenkwAMzazjyEDybQKJNjZHFiN57966461;     XpmenkwAMzazjyEDybQKJNjZHFiN57966461 = XpmenkwAMzazjyEDybQKJNjZHFiN74852495;     XpmenkwAMzazjyEDybQKJNjZHFiN74852495 = XpmenkwAMzazjyEDybQKJNjZHFiN59717028;     XpmenkwAMzazjyEDybQKJNjZHFiN59717028 = XpmenkwAMzazjyEDybQKJNjZHFiN13667787;     XpmenkwAMzazjyEDybQKJNjZHFiN13667787 = XpmenkwAMzazjyEDybQKJNjZHFiN35748715;     XpmenkwAMzazjyEDybQKJNjZHFiN35748715 = XpmenkwAMzazjyEDybQKJNjZHFiN88983228;     XpmenkwAMzazjyEDybQKJNjZHFiN88983228 = XpmenkwAMzazjyEDybQKJNjZHFiN59801125;     XpmenkwAMzazjyEDybQKJNjZHFiN59801125 = XpmenkwAMzazjyEDybQKJNjZHFiN6249195;     XpmenkwAMzazjyEDybQKJNjZHFiN6249195 = XpmenkwAMzazjyEDybQKJNjZHFiN65701030;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void YynSXLMFXMEoTBVvctpLiSvon61978940() {     long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS23018128 = -624458466;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS84333185 = -485451084;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS53746696 = -811704138;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS37985015 = -678458613;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS45482609 = -614956297;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS71128404 = -454694555;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS95621373 = -586554866;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS71346573 = -481745399;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS54190704 = -856171444;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS38928767 = -663250780;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS30064179 = -384095927;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS64095790 = -206573877;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS87826184 = -91529064;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS3483114 = -745390924;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS4366216 = -574144857;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS24194648 = -597630884;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS42757577 = -932123954;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS12369305 = -281910497;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS21700219 = -24194031;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS38038338 = -746400394;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS2471710 = -231580481;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS38239897 = -230281300;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS95044995 = -98171021;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS84351246 = -203302203;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS99030278 = -130295774;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS29348692 = -477083846;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS73616589 = -201488999;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS57876441 = -198265629;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS21119099 = -893656855;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS76625834 = -500933790;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS69010172 = -939025978;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS62124947 = 65887621;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS15014108 = -898902486;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS82674480 = -370774164;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS7917136 = -101648506;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS7295193 = -662930967;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS23026132 = -518035009;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS4560203 = -974223181;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS27809140 = -450630991;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS4772238 = -567897839;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS15206808 = -897940195;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS73578045 = -439342573;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS9452947 = -273648762;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS77886454 = 18090539;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS97544064 = -261376113;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS83221821 = -495349092;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS51928878 = -218533423;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS12683784 = -532948956;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS26302059 = -197427700;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS36172488 = -222504474;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS87770597 = -215795420;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS22146087 = -361233219;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS64042035 = -525037133;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS35906854 = -65953161;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS59507230 = -342766355;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS84778230 = -294177167;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS89288189 = -287280063;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS69395449 = -508401935;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS38954737 = -448162839;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS16133918 = -37872452;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS97511815 = -153205556;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS37744932 = -288289237;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS50227474 = -588088545;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS77564870 = -255237654;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS69918595 = -724224802;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS67939231 = -349983548;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS49081682 = -307671392;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS5151704 = -720754900;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS95565978 = -543742419;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS97071023 = -911213891;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS1168516 = 20404124;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS38197375 = -957900773;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS84560165 = -831279506;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS16927981 = -456296193;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS22831531 = -848460199;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS28893665 = -792237908;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS28786951 = -956632539;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS17158542 = -16261560;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS86807182 = -941926091;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS15808457 = -634946683;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS77419813 = -158550423;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS60932805 = -668540044;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS31574383 = 99162071;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS84946610 = -571152382;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS88855237 = -185138370;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS46864086 = -477792760;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS98082911 = -409075247;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS79107254 = -732949325;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS23167251 = 71992190;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS23138906 = -807471339;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS18007003 = -275650905;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS53630683 = 90366926;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS65605465 = -426060343;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS11675223 = -312758539;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS7260423 = -314692283;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS77461875 = -509650958;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS23350572 = -851254029;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS31888076 = 81588892;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS7967859 = -257684660;    long geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS29604833 = -624458466;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS23018128 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS84333185;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS84333185 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS53746696;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS53746696 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS37985015;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS37985015 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS45482609;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS45482609 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS71128404;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS71128404 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS95621373;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS95621373 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS71346573;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS71346573 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS54190704;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS54190704 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS38928767;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS38928767 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS30064179;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS30064179 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS64095790;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS64095790 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS87826184;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS87826184 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS3483114;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS3483114 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS4366216;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS4366216 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS24194648;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS24194648 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS42757577;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS42757577 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS12369305;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS12369305 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS21700219;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS21700219 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS38038338;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS38038338 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS2471710;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS2471710 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS38239897;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS38239897 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS95044995;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS95044995 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS84351246;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS84351246 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS99030278;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS99030278 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS29348692;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS29348692 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS73616589;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS73616589 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS57876441;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS57876441 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS21119099;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS21119099 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS76625834;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS76625834 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS69010172;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS69010172 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS62124947;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS62124947 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS15014108;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS15014108 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS82674480;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS82674480 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS7917136;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS7917136 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS7295193;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS7295193 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS23026132;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS23026132 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS4560203;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS4560203 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS27809140;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS27809140 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS4772238;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS4772238 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS15206808;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS15206808 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS73578045;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS73578045 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS9452947;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS9452947 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS77886454;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS77886454 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS97544064;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS97544064 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS83221821;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS83221821 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS51928878;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS51928878 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS12683784;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS12683784 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS26302059;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS26302059 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS36172488;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS36172488 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS87770597;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS87770597 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS22146087;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS22146087 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS64042035;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS64042035 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS35906854;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS35906854 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS59507230;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS59507230 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS84778230;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS84778230 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS89288189;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS89288189 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS69395449;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS69395449 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS38954737;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS38954737 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS16133918;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS16133918 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS97511815;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS97511815 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS37744932;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS37744932 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS50227474;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS50227474 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS77564870;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS77564870 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS69918595;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS69918595 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS67939231;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS67939231 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS49081682;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS49081682 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS5151704;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS5151704 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS95565978;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS95565978 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS97071023;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS97071023 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS1168516;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS1168516 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS38197375;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS38197375 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS84560165;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS84560165 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS16927981;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS16927981 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS22831531;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS22831531 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS28893665;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS28893665 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS28786951;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS28786951 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS17158542;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS17158542 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS86807182;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS86807182 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS15808457;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS15808457 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS77419813;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS77419813 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS60932805;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS60932805 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS31574383;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS31574383 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS84946610;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS84946610 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS88855237;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS88855237 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS46864086;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS46864086 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS98082911;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS98082911 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS79107254;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS79107254 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS23167251;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS23167251 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS23138906;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS23138906 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS18007003;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS18007003 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS53630683;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS53630683 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS65605465;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS65605465 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS11675223;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS11675223 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS7260423;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS7260423 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS77461875;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS77461875 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS23350572;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS23350572 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS31888076;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS31888076 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS7967859;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS7967859 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS29604833;     geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS29604833 = geiKgnMliNDnnLyUQVyUEXEuLqlywvFCyfqgeurZsCkoS23018128;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void bzQlxZdnbqrDqtvMdZDUojYQLOXVmrzoFIuJt53300772() {     double GzjMzthWYvONWsofjhsbz30327136 = 78312081;    double GzjMzthWYvONWsofjhsbz83252594 = -12858943;    double GzjMzthWYvONWsofjhsbz39161202 = -758896420;    double GzjMzthWYvONWsofjhsbz35924012 = -756874595;    double GzjMzthWYvONWsofjhsbz42186577 = -200749123;    double GzjMzthWYvONWsofjhsbz53269407 = -739282799;    double GzjMzthWYvONWsofjhsbz56765416 = -450875707;    double GzjMzthWYvONWsofjhsbz11246653 = -479047188;    double GzjMzthWYvONWsofjhsbz67091880 = -853237255;    double GzjMzthWYvONWsofjhsbz85749779 = -395847093;    double GzjMzthWYvONWsofjhsbz75219318 = -499927880;    double GzjMzthWYvONWsofjhsbz622356 = -237123419;    double GzjMzthWYvONWsofjhsbz7878680 = -592021976;    double GzjMzthWYvONWsofjhsbz63101749 = -655921902;    double GzjMzthWYvONWsofjhsbz77745558 = -205657822;    double GzjMzthWYvONWsofjhsbz27707482 = -135181284;    double GzjMzthWYvONWsofjhsbz90595739 = 22118250;    double GzjMzthWYvONWsofjhsbz75985447 = 56286098;    double GzjMzthWYvONWsofjhsbz17384940 = -891872421;    double GzjMzthWYvONWsofjhsbz84642376 = -14355374;    double GzjMzthWYvONWsofjhsbz46612593 = -546448970;    double GzjMzthWYvONWsofjhsbz66835665 = -279088086;    double GzjMzthWYvONWsofjhsbz68682331 = -157823981;    double GzjMzthWYvONWsofjhsbz27705785 = -295976694;    double GzjMzthWYvONWsofjhsbz24451394 = -632233632;    double GzjMzthWYvONWsofjhsbz24294863 = -304741728;    double GzjMzthWYvONWsofjhsbz64187115 = 93762017;    double GzjMzthWYvONWsofjhsbz92126092 = -961732037;    double GzjMzthWYvONWsofjhsbz38804248 = -444803503;    double GzjMzthWYvONWsofjhsbz60120840 = -992173681;    double GzjMzthWYvONWsofjhsbz63426216 = -677037934;    double GzjMzthWYvONWsofjhsbz85367140 = -161842767;    double GzjMzthWYvONWsofjhsbz54392122 = -515153828;    double GzjMzthWYvONWsofjhsbz19002702 = -880326568;    double GzjMzthWYvONWsofjhsbz31295550 = -96191391;    double GzjMzthWYvONWsofjhsbz79441257 = -384674384;    double GzjMzthWYvONWsofjhsbz37978800 = -26017082;    double GzjMzthWYvONWsofjhsbz51317723 = -454211100;    double GzjMzthWYvONWsofjhsbz95199127 = -958670522;    double GzjMzthWYvONWsofjhsbz27997412 = -192163340;    double GzjMzthWYvONWsofjhsbz24285108 = -89209727;    double GzjMzthWYvONWsofjhsbz94847405 = -986129887;    double GzjMzthWYvONWsofjhsbz48805297 = -259142096;    double GzjMzthWYvONWsofjhsbz23464485 = -368693166;    double GzjMzthWYvONWsofjhsbz53339280 = -754227244;    double GzjMzthWYvONWsofjhsbz44258604 = 50704359;    double GzjMzthWYvONWsofjhsbz78616074 = -16220804;    double GzjMzthWYvONWsofjhsbz53724706 = -905781939;    double GzjMzthWYvONWsofjhsbz20406611 = -943960627;    double GzjMzthWYvONWsofjhsbz62387852 = -336752405;    double GzjMzthWYvONWsofjhsbz62979580 = -266014620;    double GzjMzthWYvONWsofjhsbz30706710 = -709831099;    double GzjMzthWYvONWsofjhsbz36746875 = -647950759;    double GzjMzthWYvONWsofjhsbz73699336 = -873094487;    double GzjMzthWYvONWsofjhsbz6688180 = -755192908;    double GzjMzthWYvONWsofjhsbz63491470 = -642599834;    double GzjMzthWYvONWsofjhsbz14570263 = -855034962;    double GzjMzthWYvONWsofjhsbz11455417 = -362919727;    double GzjMzthWYvONWsofjhsbz11472618 = -24640964;    double GzjMzthWYvONWsofjhsbz17891714 = -896007395;    double GzjMzthWYvONWsofjhsbz89082292 = -733044817;    double GzjMzthWYvONWsofjhsbz64639323 = -489143671;    double GzjMzthWYvONWsofjhsbz72442405 = 65756315;    double GzjMzthWYvONWsofjhsbz6971041 = -861063574;    double GzjMzthWYvONWsofjhsbz22323563 = -718809159;    double GzjMzthWYvONWsofjhsbz89852177 = -238085113;    double GzjMzthWYvONWsofjhsbz46230233 = -721969592;    double GzjMzthWYvONWsofjhsbz88875978 = -711695409;    double GzjMzthWYvONWsofjhsbz31806199 = -459730511;    double GzjMzthWYvONWsofjhsbz98304300 = -820983439;    double GzjMzthWYvONWsofjhsbz89728681 = -9164203;    double GzjMzthWYvONWsofjhsbz39278016 = -523670650;    double GzjMzthWYvONWsofjhsbz80786319 = 14956620;    double GzjMzthWYvONWsofjhsbz89387528 = -599709081;    double GzjMzthWYvONWsofjhsbz60357268 = -925145647;    double GzjMzthWYvONWsofjhsbz51765188 = -560319083;    double GzjMzthWYvONWsofjhsbz18030369 = 80054009;    double GzjMzthWYvONWsofjhsbz45217847 = -789130816;    double GzjMzthWYvONWsofjhsbz74366505 = -541749450;    double GzjMzthWYvONWsofjhsbz80192790 = -582937992;    double GzjMzthWYvONWsofjhsbz45678789 = -188520925;    double GzjMzthWYvONWsofjhsbz10462409 = -456045;    double GzjMzthWYvONWsofjhsbz71719482 = 82228590;    double GzjMzthWYvONWsofjhsbz76416396 = -8051099;    double GzjMzthWYvONWsofjhsbz97141260 = -626159062;    double GzjMzthWYvONWsofjhsbz32719506 = -967206835;    double GzjMzthWYvONWsofjhsbz48620266 = -513892008;    double GzjMzthWYvONWsofjhsbz80692786 = -642059341;    double GzjMzthWYvONWsofjhsbz12314522 = -25133660;    double GzjMzthWYvONWsofjhsbz67804080 = -453591558;    double GzjMzthWYvONWsofjhsbz64870995 = -529639422;    double GzjMzthWYvONWsofjhsbz26523384 = -663097355;    double GzjMzthWYvONWsofjhsbz39845106 = -329570137;    double GzjMzthWYvONWsofjhsbz77307414 = 37336873;    double GzjMzthWYvONWsofjhsbz38915119 = -459118524;    double GzjMzthWYvONWsofjhsbz59645785 = -600066057;    double GzjMzthWYvONWsofjhsbz22405000 = -951886202;    double GzjMzthWYvONWsofjhsbz41834257 = -398078522;    double GzjMzthWYvONWsofjhsbz1140922 = -649884007;    double GzjMzthWYvONWsofjhsbz63487102 = 78312081;     GzjMzthWYvONWsofjhsbz30327136 = GzjMzthWYvONWsofjhsbz83252594;     GzjMzthWYvONWsofjhsbz83252594 = GzjMzthWYvONWsofjhsbz39161202;     GzjMzthWYvONWsofjhsbz39161202 = GzjMzthWYvONWsofjhsbz35924012;     GzjMzthWYvONWsofjhsbz35924012 = GzjMzthWYvONWsofjhsbz42186577;     GzjMzthWYvONWsofjhsbz42186577 = GzjMzthWYvONWsofjhsbz53269407;     GzjMzthWYvONWsofjhsbz53269407 = GzjMzthWYvONWsofjhsbz56765416;     GzjMzthWYvONWsofjhsbz56765416 = GzjMzthWYvONWsofjhsbz11246653;     GzjMzthWYvONWsofjhsbz11246653 = GzjMzthWYvONWsofjhsbz67091880;     GzjMzthWYvONWsofjhsbz67091880 = GzjMzthWYvONWsofjhsbz85749779;     GzjMzthWYvONWsofjhsbz85749779 = GzjMzthWYvONWsofjhsbz75219318;     GzjMzthWYvONWsofjhsbz75219318 = GzjMzthWYvONWsofjhsbz622356;     GzjMzthWYvONWsofjhsbz622356 = GzjMzthWYvONWsofjhsbz7878680;     GzjMzthWYvONWsofjhsbz7878680 = GzjMzthWYvONWsofjhsbz63101749;     GzjMzthWYvONWsofjhsbz63101749 = GzjMzthWYvONWsofjhsbz77745558;     GzjMzthWYvONWsofjhsbz77745558 = GzjMzthWYvONWsofjhsbz27707482;     GzjMzthWYvONWsofjhsbz27707482 = GzjMzthWYvONWsofjhsbz90595739;     GzjMzthWYvONWsofjhsbz90595739 = GzjMzthWYvONWsofjhsbz75985447;     GzjMzthWYvONWsofjhsbz75985447 = GzjMzthWYvONWsofjhsbz17384940;     GzjMzthWYvONWsofjhsbz17384940 = GzjMzthWYvONWsofjhsbz84642376;     GzjMzthWYvONWsofjhsbz84642376 = GzjMzthWYvONWsofjhsbz46612593;     GzjMzthWYvONWsofjhsbz46612593 = GzjMzthWYvONWsofjhsbz66835665;     GzjMzthWYvONWsofjhsbz66835665 = GzjMzthWYvONWsofjhsbz68682331;     GzjMzthWYvONWsofjhsbz68682331 = GzjMzthWYvONWsofjhsbz27705785;     GzjMzthWYvONWsofjhsbz27705785 = GzjMzthWYvONWsofjhsbz24451394;     GzjMzthWYvONWsofjhsbz24451394 = GzjMzthWYvONWsofjhsbz24294863;     GzjMzthWYvONWsofjhsbz24294863 = GzjMzthWYvONWsofjhsbz64187115;     GzjMzthWYvONWsofjhsbz64187115 = GzjMzthWYvONWsofjhsbz92126092;     GzjMzthWYvONWsofjhsbz92126092 = GzjMzthWYvONWsofjhsbz38804248;     GzjMzthWYvONWsofjhsbz38804248 = GzjMzthWYvONWsofjhsbz60120840;     GzjMzthWYvONWsofjhsbz60120840 = GzjMzthWYvONWsofjhsbz63426216;     GzjMzthWYvONWsofjhsbz63426216 = GzjMzthWYvONWsofjhsbz85367140;     GzjMzthWYvONWsofjhsbz85367140 = GzjMzthWYvONWsofjhsbz54392122;     GzjMzthWYvONWsofjhsbz54392122 = GzjMzthWYvONWsofjhsbz19002702;     GzjMzthWYvONWsofjhsbz19002702 = GzjMzthWYvONWsofjhsbz31295550;     GzjMzthWYvONWsofjhsbz31295550 = GzjMzthWYvONWsofjhsbz79441257;     GzjMzthWYvONWsofjhsbz79441257 = GzjMzthWYvONWsofjhsbz37978800;     GzjMzthWYvONWsofjhsbz37978800 = GzjMzthWYvONWsofjhsbz51317723;     GzjMzthWYvONWsofjhsbz51317723 = GzjMzthWYvONWsofjhsbz95199127;     GzjMzthWYvONWsofjhsbz95199127 = GzjMzthWYvONWsofjhsbz27997412;     GzjMzthWYvONWsofjhsbz27997412 = GzjMzthWYvONWsofjhsbz24285108;     GzjMzthWYvONWsofjhsbz24285108 = GzjMzthWYvONWsofjhsbz94847405;     GzjMzthWYvONWsofjhsbz94847405 = GzjMzthWYvONWsofjhsbz48805297;     GzjMzthWYvONWsofjhsbz48805297 = GzjMzthWYvONWsofjhsbz23464485;     GzjMzthWYvONWsofjhsbz23464485 = GzjMzthWYvONWsofjhsbz53339280;     GzjMzthWYvONWsofjhsbz53339280 = GzjMzthWYvONWsofjhsbz44258604;     GzjMzthWYvONWsofjhsbz44258604 = GzjMzthWYvONWsofjhsbz78616074;     GzjMzthWYvONWsofjhsbz78616074 = GzjMzthWYvONWsofjhsbz53724706;     GzjMzthWYvONWsofjhsbz53724706 = GzjMzthWYvONWsofjhsbz20406611;     GzjMzthWYvONWsofjhsbz20406611 = GzjMzthWYvONWsofjhsbz62387852;     GzjMzthWYvONWsofjhsbz62387852 = GzjMzthWYvONWsofjhsbz62979580;     GzjMzthWYvONWsofjhsbz62979580 = GzjMzthWYvONWsofjhsbz30706710;     GzjMzthWYvONWsofjhsbz30706710 = GzjMzthWYvONWsofjhsbz36746875;     GzjMzthWYvONWsofjhsbz36746875 = GzjMzthWYvONWsofjhsbz73699336;     GzjMzthWYvONWsofjhsbz73699336 = GzjMzthWYvONWsofjhsbz6688180;     GzjMzthWYvONWsofjhsbz6688180 = GzjMzthWYvONWsofjhsbz63491470;     GzjMzthWYvONWsofjhsbz63491470 = GzjMzthWYvONWsofjhsbz14570263;     GzjMzthWYvONWsofjhsbz14570263 = GzjMzthWYvONWsofjhsbz11455417;     GzjMzthWYvONWsofjhsbz11455417 = GzjMzthWYvONWsofjhsbz11472618;     GzjMzthWYvONWsofjhsbz11472618 = GzjMzthWYvONWsofjhsbz17891714;     GzjMzthWYvONWsofjhsbz17891714 = GzjMzthWYvONWsofjhsbz89082292;     GzjMzthWYvONWsofjhsbz89082292 = GzjMzthWYvONWsofjhsbz64639323;     GzjMzthWYvONWsofjhsbz64639323 = GzjMzthWYvONWsofjhsbz72442405;     GzjMzthWYvONWsofjhsbz72442405 = GzjMzthWYvONWsofjhsbz6971041;     GzjMzthWYvONWsofjhsbz6971041 = GzjMzthWYvONWsofjhsbz22323563;     GzjMzthWYvONWsofjhsbz22323563 = GzjMzthWYvONWsofjhsbz89852177;     GzjMzthWYvONWsofjhsbz89852177 = GzjMzthWYvONWsofjhsbz46230233;     GzjMzthWYvONWsofjhsbz46230233 = GzjMzthWYvONWsofjhsbz88875978;     GzjMzthWYvONWsofjhsbz88875978 = GzjMzthWYvONWsofjhsbz31806199;     GzjMzthWYvONWsofjhsbz31806199 = GzjMzthWYvONWsofjhsbz98304300;     GzjMzthWYvONWsofjhsbz98304300 = GzjMzthWYvONWsofjhsbz89728681;     GzjMzthWYvONWsofjhsbz89728681 = GzjMzthWYvONWsofjhsbz39278016;     GzjMzthWYvONWsofjhsbz39278016 = GzjMzthWYvONWsofjhsbz80786319;     GzjMzthWYvONWsofjhsbz80786319 = GzjMzthWYvONWsofjhsbz89387528;     GzjMzthWYvONWsofjhsbz89387528 = GzjMzthWYvONWsofjhsbz60357268;     GzjMzthWYvONWsofjhsbz60357268 = GzjMzthWYvONWsofjhsbz51765188;     GzjMzthWYvONWsofjhsbz51765188 = GzjMzthWYvONWsofjhsbz18030369;     GzjMzthWYvONWsofjhsbz18030369 = GzjMzthWYvONWsofjhsbz45217847;     GzjMzthWYvONWsofjhsbz45217847 = GzjMzthWYvONWsofjhsbz74366505;     GzjMzthWYvONWsofjhsbz74366505 = GzjMzthWYvONWsofjhsbz80192790;     GzjMzthWYvONWsofjhsbz80192790 = GzjMzthWYvONWsofjhsbz45678789;     GzjMzthWYvONWsofjhsbz45678789 = GzjMzthWYvONWsofjhsbz10462409;     GzjMzthWYvONWsofjhsbz10462409 = GzjMzthWYvONWsofjhsbz71719482;     GzjMzthWYvONWsofjhsbz71719482 = GzjMzthWYvONWsofjhsbz76416396;     GzjMzthWYvONWsofjhsbz76416396 = GzjMzthWYvONWsofjhsbz97141260;     GzjMzthWYvONWsofjhsbz97141260 = GzjMzthWYvONWsofjhsbz32719506;     GzjMzthWYvONWsofjhsbz32719506 = GzjMzthWYvONWsofjhsbz48620266;     GzjMzthWYvONWsofjhsbz48620266 = GzjMzthWYvONWsofjhsbz80692786;     GzjMzthWYvONWsofjhsbz80692786 = GzjMzthWYvONWsofjhsbz12314522;     GzjMzthWYvONWsofjhsbz12314522 = GzjMzthWYvONWsofjhsbz67804080;     GzjMzthWYvONWsofjhsbz67804080 = GzjMzthWYvONWsofjhsbz64870995;     GzjMzthWYvONWsofjhsbz64870995 = GzjMzthWYvONWsofjhsbz26523384;     GzjMzthWYvONWsofjhsbz26523384 = GzjMzthWYvONWsofjhsbz39845106;     GzjMzthWYvONWsofjhsbz39845106 = GzjMzthWYvONWsofjhsbz77307414;     GzjMzthWYvONWsofjhsbz77307414 = GzjMzthWYvONWsofjhsbz38915119;     GzjMzthWYvONWsofjhsbz38915119 = GzjMzthWYvONWsofjhsbz59645785;     GzjMzthWYvONWsofjhsbz59645785 = GzjMzthWYvONWsofjhsbz22405000;     GzjMzthWYvONWsofjhsbz22405000 = GzjMzthWYvONWsofjhsbz41834257;     GzjMzthWYvONWsofjhsbz41834257 = GzjMzthWYvONWsofjhsbz1140922;     GzjMzthWYvONWsofjhsbz1140922 = GzjMzthWYvONWsofjhsbz63487102;     GzjMzthWYvONWsofjhsbz63487102 = GzjMzthWYvONWsofjhsbz30327136;}
// Junk Finished
