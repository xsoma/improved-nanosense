#include <cmath>

#include "Vector2D.hpp"

Vector2D::Vector2D(void)
{
}

Vector2D::Vector2D(vec_t X, vec_t Y)
{
    x = X; y = Y;
}

Vector2D::Vector2D(vec_t* clr)
{
    x = clr[0]; y = clr[1];
}

//-----------------------------------------------------------------------------
// initialization
//-----------------------------------------------------------------------------

void Vector2D::Init(vec_t ix, vec_t iy)
{
    x = ix; y = iy;
}

void Vector2D::Random(float minVal, float maxVal)
{
    x = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
    y = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
}

void Vector2DClear(Vector2D& a)
{
    a.x = a.y = 0.0f;
}

//-----------------------------------------------------------------------------
// assignment
//-----------------------------------------------------------------------------

Vector2D& Vector2D::operator=(const Vector2D &vOther)
{
    x = vOther.x; y = vOther.y;
    return *this;
}

//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------

vec_t& Vector2D::operator[](int i)
{
    return ((vec_t*)this)[i];
}

vec_t Vector2D::operator[](int i) const
{
    return ((vec_t*)this)[i];
}

//-----------------------------------------------------------------------------
// Base address...
//-----------------------------------------------------------------------------

vec_t* Vector2D::Base()
{
    return (vec_t*)this;
}

vec_t const* Vector2D::Base() const
{
    return (vec_t const*)this;
}

//-----------------------------------------------------------------------------
// IsValid?
//-----------------------------------------------------------------------------

bool Vector2D::IsValid() const
{
    return !isinf(x) && !isinf(y);
}

//-----------------------------------------------------------------------------
// comparison
//-----------------------------------------------------------------------------

bool Vector2D::operator==(const Vector2D& src) const
{
    return (src.x == x) && (src.y == y);
}

bool Vector2D::operator!=(const Vector2D& src) const
{
    return (src.x != x) || (src.y != y);
}


//-----------------------------------------------------------------------------
// Copy
//-----------------------------------------------------------------------------

void Vector2DCopy(const Vector2D& src, Vector2D& dst)
{
    dst.x = src.x;
    dst.y = src.y;
}

void Vector2D::CopyToArray(float* rgfl) const
{
    rgfl[0] = x; rgfl[1] = y;
}

//-----------------------------------------------------------------------------
// standard Math operations
//-----------------------------------------------------------------------------

void Vector2D::Negate()
{
    x = -x; y = -y;
}

void Vector2DAdd(const Vector2D& a, const Vector2D& b, Vector2D& c)
{
    c.x = a.x + b.x;
    c.y = a.y + b.y;
}

void Vector2DSubtract(const Vector2D& a, const Vector2D& b, Vector2D& c)
{
    c.x = a.x - b.x;
    c.y = a.y - b.y;
}

void Vector2DMultiply(const Vector2D& a, vec_t b, Vector2D& c)
{
    c.x = a.x * b;
    c.y = a.y * b;
}

void Vector2DMultiply(const Vector2D& a, const Vector2D& b, Vector2D& c)
{
    c.x = a.x * b.x;
    c.y = a.y * b.y;
}


void Vector2DDivide(const Vector2D& a, vec_t b, Vector2D& c)
{
    vec_t oob = 1.0f / b;
    c.x = a.x * oob;
    c.y = a.y * oob;
}

void Vector2DDivide(const Vector2D& a, const Vector2D& b, Vector2D& c)
{
    c.x = a.x / b.x;
    c.y = a.y / b.y;
}

void Vector2DMA(const Vector2D& start, float s, const Vector2D& dir, Vector2D& result)
{
    result.x = start.x + s*dir.x;
    result.y = start.y + s*dir.y;
}

// FIXME: Remove
// For backwards compatability
void Vector2D::MulAdd(const Vector2D& a, const Vector2D& b, float scalar)
{
    x = a.x + b.x * scalar;
    y = a.y + b.y * scalar;
}

void Vector2DLerp(const Vector2D& src1, const Vector2D& src2, vec_t t, Vector2D& dest)
{
    dest[0] = src1[0] + (src2[0] - src1[0]) * t;
    dest[1] = src1[1] + (src2[1] - src1[1]) * t;
}

//-----------------------------------------------------------------------------
// dot, cross
//-----------------------------------------------------------------------------
vec_t DotProduct2D(const Vector2D& a, const Vector2D& b)
{
    return(a.x*b.x + a.y*b.y);
}

// for backwards compatability
vec_t Vector2D::Dot(const Vector2D& vOther) const
{
    return DotProduct2D(*this, vOther);
}

vec_t Vector2DNormalize(Vector2D& v)
{
    vec_t l = v.Length();
    if(l != 0.0f) {
        v /= l;
    } else {
        v.x = v.y = 0.0f;
    }
    return l;
}

//-----------------------------------------------------------------------------
// length
//-----------------------------------------------------------------------------
vec_t Vector2DLength(const Vector2D& v)
{
    return (vec_t)sqrt(v.x*v.x + v.y*v.y);
}

vec_t Vector2D::NormalizeInPlace()
{
    return Vector2DNormalize(*this);
}

bool Vector2D::IsLengthGreaterThan(float val) const
{
    return LengthSqr() > val*val;
}

bool Vector2D::IsLengthLessThan(float val) const
{
    return LengthSqr() < val*val;
}

vec_t Vector2D::Length(void) const
{
    return Vector2DLength(*this);
}


void Vector2DMin(const Vector2D &a, const Vector2D &b, Vector2D &result)
{
    result.x = (a.x < b.x) ? a.x : b.x;
    result.y = (a.y < b.y) ? a.y : b.y;
}


void Vector2DMax(const Vector2D &a, const Vector2D &b, Vector2D &result)
{
    result.x = (a.x > b.x) ? a.x : b.x;
    result.y = (a.y > b.y) ? a.y : b.y;
}

//-----------------------------------------------------------------------------
// Computes the closest point to vecTarget no farther than flMaxDist from vecStart
//-----------------------------------------------------------------------------
void ComputeClosestPoint2D(const Vector2D& vecStart, float flMaxDist, const Vector2D& vecTarget, Vector2D *pResult)
{
    Vector2D vecDelta;
    Vector2DSubtract(vecTarget, vecStart, vecDelta);
    float flDistSqr = vecDelta.LengthSqr();
    if(flDistSqr <= flMaxDist * flMaxDist) {
        *pResult = vecTarget;
    } else {
        vecDelta /= sqrt(flDistSqr);
        Vector2DMA(vecStart, flMaxDist, vecDelta, *pResult);
    }
}

//-----------------------------------------------------------------------------
// Returns a Vector2D with the min or max in X, Y, and Z.
//-----------------------------------------------------------------------------

Vector2D Vector2D::Min(const Vector2D &vOther) const
{
    return Vector2D(x < vOther.x ? x : vOther.x, y < vOther.y ? y : vOther.y);
}

Vector2D Vector2D::Max(const Vector2D &vOther) const
{
    return Vector2D(x > vOther.x ? x : vOther.x, y > vOther.y ? y : vOther.y);
}


//-----------------------------------------------------------------------------
// arithmetic operations
//-----------------------------------------------------------------------------

Vector2D Vector2D::operator-(void) const
{
    return Vector2D(-x, -y);
}

Vector2D Vector2D::operator+(const Vector2D& v) const
{
    Vector2D res;
    Vector2DAdd(*this, v, res);
    return res;
}

Vector2D Vector2D::operator-(const Vector2D& v) const
{
    Vector2D res;
    Vector2DSubtract(*this, v, res);
    return res;
}

Vector2D Vector2D::operator*(float fl) const
{
    Vector2D res;
    Vector2DMultiply(*this, fl, res);
    return res;
}

Vector2D Vector2D::operator*(const Vector2D& v) const
{
    Vector2D res;
    Vector2DMultiply(*this, v, res);
    return res;
}

Vector2D Vector2D::operator/(float fl) const
{
    Vector2D res;
    Vector2DDivide(*this, fl, res);
    return res;
}

Vector2D Vector2D::operator/(const Vector2D& v) const
{
    Vector2D res;
    Vector2DDivide(*this, v, res);
    return res;
}

Vector2D operator*(float fl, const Vector2D& v)
{
    return v * fl;
}












































































































// Junk Code By Troll Face & Thaisen's Gen
void gDCPYLyWelbDCgvfiTiCkRmxEbHklyVMnDTBWhPt32573918() {     int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg97942109 = -440139554;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg62456685 = -167402708;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg99308011 = -733063563;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg87213636 = -367797605;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg53611792 = -975953974;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg1184299 = -127384391;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg10503579 = -950398875;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg63295898 = -973705716;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg99781041 = -112470046;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg8980596 = -905830357;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg10241944 = -98401696;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg28946131 = -883934328;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg47606829 = -487070992;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg8549074 = -80317048;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg49720748 = -883742632;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg68757176 = -81995903;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg2227421 = -469342213;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg32043218 = -962346071;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg15885570 = -547228020;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg30969157 = -533052700;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg97391796 = -378259110;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg14605501 = -461724864;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg97822013 = -42145312;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg47291427 = 45154827;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg18791723 = -239350411;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg9242404 = -305550904;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg51054800 = -123136717;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg48426463 = -634117616;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg25997490 = -514462349;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg22354410 = 24931152;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg90753983 = 24640090;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg30529181 = -278758783;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg45865185 = -685717997;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg47855626 = -524858490;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg48435803 = -626902351;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg72602728 = -360791386;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg78821012 = -533516047;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg95467244 = -380011911;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg21575286 = -651417472;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg10937641 = -373694640;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg99935356 = -216055693;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg70770553 = -771194791;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg13232375 = -411914682;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg43738767 = -954761092;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg41904754 = -85507162;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg33977351 = 71582953;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg75441866 = -118080296;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg84380219 = -522184309;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg34590965 = -266992501;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg92122166 = -149114533;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg86088867 = -14217137;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg14977126 = -669460335;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg7338319 = 41091191;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg11718342 = -86313595;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg35182121 = -937628988;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg83336608 = -978414691;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg64634671 = -25257397;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg52016584 = -678218391;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg68421913 = -28447194;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg44369389 = -570403070;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg50129499 = 95752325;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg62077116 = -216281259;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg37298408 = -359243367;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg77426632 = -37401199;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg18226613 = -830470448;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg79712762 = -819642913;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg83080946 = -98216332;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg99751203 = -962212503;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg60113270 = -453414697;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg77118019 = -422951247;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg89936164 = -548479857;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg6760176 = 10669698;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg10467932 = -210928600;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg4947929 = -73533380;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg31033801 = -216997007;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg26621244 = -607064319;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg1373127 = 50189818;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg54083246 = -87384220;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg5386674 = -869338012;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg84814372 = -210933364;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg33800537 = -87470608;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg66674581 = -600952408;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg13835499 = -267125116;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg33875323 = -265347817;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg36265543 = -960851712;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg75776857 = -305899575;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg23190863 = -219849974;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg34146843 = -499404403;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg12673505 = -587229503;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg65099195 = -648487660;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg7968057 = -235533989;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg26804429 = -855297657;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg27045332 = -251564717;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg77205897 = 18985598;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg60808142 = -369446966;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg37858241 = -999774434;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg33472145 = -311951425;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg35805743 = -274513484;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg25512155 = -24290645;    int lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg62191991 = -440139554;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg97942109 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg62456685;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg62456685 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg99308011;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg99308011 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg87213636;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg87213636 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg53611792;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg53611792 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg1184299;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg1184299 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg10503579;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg10503579 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg63295898;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg63295898 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg99781041;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg99781041 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg8980596;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg8980596 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg10241944;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg10241944 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg28946131;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg28946131 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg47606829;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg47606829 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg8549074;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg8549074 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg49720748;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg49720748 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg68757176;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg68757176 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg2227421;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg2227421 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg32043218;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg32043218 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg15885570;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg15885570 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg30969157;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg30969157 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg97391796;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg97391796 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg14605501;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg14605501 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg97822013;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg97822013 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg47291427;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg47291427 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg18791723;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg18791723 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg9242404;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg9242404 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg51054800;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg51054800 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg48426463;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg48426463 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg25997490;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg25997490 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg22354410;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg22354410 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg90753983;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg90753983 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg30529181;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg30529181 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg45865185;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg45865185 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg47855626;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg47855626 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg48435803;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg48435803 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg72602728;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg72602728 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg78821012;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg78821012 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg95467244;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg95467244 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg21575286;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg21575286 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg10937641;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg10937641 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg99935356;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg99935356 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg70770553;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg70770553 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg13232375;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg13232375 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg43738767;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg43738767 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg41904754;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg41904754 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg33977351;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg33977351 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg75441866;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg75441866 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg84380219;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg84380219 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg34590965;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg34590965 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg92122166;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg92122166 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg86088867;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg86088867 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg14977126;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg14977126 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg7338319;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg7338319 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg11718342;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg11718342 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg35182121;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg35182121 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg83336608;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg83336608 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg64634671;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg64634671 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg52016584;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg52016584 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg68421913;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg68421913 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg44369389;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg44369389 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg50129499;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg50129499 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg62077116;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg62077116 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg37298408;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg37298408 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg77426632;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg77426632 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg18226613;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg18226613 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg79712762;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg79712762 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg83080946;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg83080946 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg99751203;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg99751203 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg60113270;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg60113270 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg77118019;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg77118019 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg89936164;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg89936164 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg6760176;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg6760176 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg10467932;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg10467932 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg4947929;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg4947929 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg31033801;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg31033801 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg26621244;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg26621244 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg1373127;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg1373127 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg54083246;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg54083246 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg5386674;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg5386674 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg84814372;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg84814372 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg33800537;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg33800537 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg66674581;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg66674581 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg13835499;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg13835499 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg33875323;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg33875323 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg36265543;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg36265543 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg75776857;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg75776857 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg23190863;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg23190863 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg34146843;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg34146843 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg12673505;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg12673505 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg65099195;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg65099195 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg7968057;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg7968057 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg26804429;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg26804429 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg27045332;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg27045332 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg77205897;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg77205897 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg60808142;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg60808142 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg37858241;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg37858241 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg33472145;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg33472145 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg35805743;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg35805743 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg25512155;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg25512155 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg62191991;     lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg62191991 = lYqwzdUEAhnvusrOQFLwDOlfQOPyNuJAEpRLaqxaHDkDKaRvMElQVSpXnLglNVqFXmqFbg97942109;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void xUQzjPhuCfdKfXLBdASikCjRpIQmnsbORVmkCFAyylwXQTyzqTW2398402() {     float sDwYukyKayvHJEOsJkbyYAUkyGLA27037431 = 41497706;    float sDwYukyKayvHJEOsJkbyYAUkyGLA71879767 = -866395927;    float sDwYukyKayvHJEOsJkbyYAUkyGLA4790544 = -164576469;    float sDwYukyKayvHJEOsJkbyYAUkyGLA3537109 = -649193065;    float sDwYukyKayvHJEOsJkbyYAUkyGLA41215851 = -871498814;    float sDwYukyKayvHJEOsJkbyYAUkyGLA28950593 = -37109617;    float sDwYukyKayvHJEOsJkbyYAUkyGLA89886324 = -277431440;    float sDwYukyKayvHJEOsJkbyYAUkyGLA35670280 = -814523199;    float sDwYukyKayvHJEOsJkbyYAUkyGLA47771131 = -489449876;    float sDwYukyKayvHJEOsJkbyYAUkyGLA42249151 = -681962738;    float sDwYukyKayvHJEOsJkbyYAUkyGLA15656972 = -611264920;    float sDwYukyKayvHJEOsJkbyYAUkyGLA79742642 = -73766266;    float sDwYukyKayvHJEOsJkbyYAUkyGLA41635502 = -767224168;    float sDwYukyKayvHJEOsJkbyYAUkyGLA37928147 = -538275366;    float sDwYukyKayvHJEOsJkbyYAUkyGLA4895992 = -94242549;    float sDwYukyKayvHJEOsJkbyYAUkyGLA16445614 = -193535156;    float sDwYukyKayvHJEOsJkbyYAUkyGLA14777424 = -992484414;    float sDwYukyKayvHJEOsJkbyYAUkyGLA60604682 = -555502;    float sDwYukyKayvHJEOsJkbyYAUkyGLA26245685 = -132540588;    float sDwYukyKayvHJEOsJkbyYAUkyGLA27117865 = 42865559;    float sDwYukyKayvHJEOsJkbyYAUkyGLA24374011 = -787774171;    float sDwYukyKayvHJEOsJkbyYAUkyGLA69701314 = 47646659;    float sDwYukyKayvHJEOsJkbyYAUkyGLA33388198 = -190529509;    float sDwYukyKayvHJEOsJkbyYAUkyGLA20202252 = -587018161;    float sDwYukyKayvHJEOsJkbyYAUkyGLA85224370 = -893496870;    float sDwYukyKayvHJEOsJkbyYAUkyGLA44809879 = 16243823;    float sDwYukyKayvHJEOsJkbyYAUkyGLA20325559 = -459028587;    float sDwYukyKayvHJEOsJkbyYAUkyGLA4568946 = -743994992;    float sDwYukyKayvHJEOsJkbyYAUkyGLA34307589 = -955036881;    float sDwYukyKayvHJEOsJkbyYAUkyGLA89634313 = -356293624;    float sDwYukyKayvHJEOsJkbyYAUkyGLA53433594 = -172662434;    float sDwYukyKayvHJEOsJkbyYAUkyGLA87039672 = -643285061;    float sDwYukyKayvHJEOsJkbyYAUkyGLA65537654 = -295952010;    float sDwYukyKayvHJEOsJkbyYAUkyGLA23035834 = -612043724;    float sDwYukyKayvHJEOsJkbyYAUkyGLA28860230 = -101782212;    float sDwYukyKayvHJEOsJkbyYAUkyGLA16852729 = -978895834;    float sDwYukyKayvHJEOsJkbyYAUkyGLA4324052 = -347013003;    float sDwYukyKayvHJEOsJkbyYAUkyGLA91955471 = -419831356;    float sDwYukyKayvHJEOsJkbyYAUkyGLA66868735 = 87043921;    float sDwYukyKayvHJEOsJkbyYAUkyGLA65391581 = 9155204;    float sDwYukyKayvHJEOsJkbyYAUkyGLA26958241 = -688462996;    float sDwYukyKayvHJEOsJkbyYAUkyGLA75262098 = 17087660;    float sDwYukyKayvHJEOsJkbyYAUkyGLA47704751 = -201941915;    float sDwYukyKayvHJEOsJkbyYAUkyGLA79099696 = -822911395;    float sDwYukyKayvHJEOsJkbyYAUkyGLA56775399 = -601969834;    float sDwYukyKayvHJEOsJkbyYAUkyGLA8886598 = 35739022;    float sDwYukyKayvHJEOsJkbyYAUkyGLA30745494 = 84317525;    float sDwYukyKayvHJEOsJkbyYAUkyGLA85015510 = -651654575;    float sDwYukyKayvHJEOsJkbyYAUkyGLA19934802 = -30667053;    float sDwYukyKayvHJEOsJkbyYAUkyGLA23803488 = -82032827;    float sDwYukyKayvHJEOsJkbyYAUkyGLA10357442 = -549466105;    float sDwYukyKayvHJEOsJkbyYAUkyGLA59423997 = -86551029;    float sDwYukyKayvHJEOsJkbyYAUkyGLA77715631 = -882212153;    float sDwYukyKayvHJEOsJkbyYAUkyGLA46573271 = 50259972;    float sDwYukyKayvHJEOsJkbyYAUkyGLA78024391 = -902486729;    float sDwYukyKayvHJEOsJkbyYAUkyGLA57336116 = 93851046;    float sDwYukyKayvHJEOsJkbyYAUkyGLA38491569 = -575866419;    float sDwYukyKayvHJEOsJkbyYAUkyGLA84588291 = -577558308;    float sDwYukyKayvHJEOsJkbyYAUkyGLA18312739 = -755696195;    float sDwYukyKayvHJEOsJkbyYAUkyGLA96405971 = -787742638;    float sDwYukyKayvHJEOsJkbyYAUkyGLA8625034 = -578081030;    float sDwYukyKayvHJEOsJkbyYAUkyGLA85317378 = -533436449;    float sDwYukyKayvHJEOsJkbyYAUkyGLA1362692 = -859486318;    float sDwYukyKayvHJEOsJkbyYAUkyGLA58136818 = -33156253;    float sDwYukyKayvHJEOsJkbyYAUkyGLA88815556 = -409300304;    float sDwYukyKayvHJEOsJkbyYAUkyGLA28617299 = -967979860;    float sDwYukyKayvHJEOsJkbyYAUkyGLA14204988 = -777814256;    float sDwYukyKayvHJEOsJkbyYAUkyGLA18599669 = -55180445;    float sDwYukyKayvHJEOsJkbyYAUkyGLA9067917 = -336493154;    float sDwYukyKayvHJEOsJkbyYAUkyGLA88043262 = -115346716;    float sDwYukyKayvHJEOsJkbyYAUkyGLA12121562 = -846522153;    float sDwYukyKayvHJEOsJkbyYAUkyGLA22821952 = -472653058;    float sDwYukyKayvHJEOsJkbyYAUkyGLA93735946 = 12400577;    float sDwYukyKayvHJEOsJkbyYAUkyGLA60854103 = -41695793;    float sDwYukyKayvHJEOsJkbyYAUkyGLA159624 = -268671446;    float sDwYukyKayvHJEOsJkbyYAUkyGLA49111912 = -704861831;    float sDwYukyKayvHJEOsJkbyYAUkyGLA21996564 = -750411426;    float sDwYukyKayvHJEOsJkbyYAUkyGLA54288502 = -367618114;    float sDwYukyKayvHJEOsJkbyYAUkyGLA63426852 = -985048328;    float sDwYukyKayvHJEOsJkbyYAUkyGLA76337772 = -829235893;    float sDwYukyKayvHJEOsJkbyYAUkyGLA14064386 = 31926297;    float sDwYukyKayvHJEOsJkbyYAUkyGLA35310048 = -807374013;    float sDwYukyKayvHJEOsJkbyYAUkyGLA84634143 = -613327939;    float sDwYukyKayvHJEOsJkbyYAUkyGLA10504101 = -773004055;    float sDwYukyKayvHJEOsJkbyYAUkyGLA79276871 = -806827520;    float sDwYukyKayvHJEOsJkbyYAUkyGLA94009596 = 13888594;    float sDwYukyKayvHJEOsJkbyYAUkyGLA9324042 = -761072909;    float sDwYukyKayvHJEOsJkbyYAUkyGLA18964383 = -246211983;    float sDwYukyKayvHJEOsJkbyYAUkyGLA45011443 = -709556996;    float sDwYukyKayvHJEOsJkbyYAUkyGLA71524113 = -95633259;    float sDwYukyKayvHJEOsJkbyYAUkyGLA78361159 = -303029416;    float sDwYukyKayvHJEOsJkbyYAUkyGLA19735760 = -769454695;    float sDwYukyKayvHJEOsJkbyYAUkyGLA73642733 = -664135162;    float sDwYukyKayvHJEOsJkbyYAUkyGLA70462763 = -125213442;    float sDwYukyKayvHJEOsJkbyYAUkyGLA56766547 = -412763766;    float sDwYukyKayvHJEOsJkbyYAUkyGLA41640862 = -55026547;    float sDwYukyKayvHJEOsJkbyYAUkyGLA73899406 = -123426023;    float sDwYukyKayvHJEOsJkbyYAUkyGLA89567932 = -68785663;    float sDwYukyKayvHJEOsJkbyYAUkyGLA90284139 = -313611091;    float sDwYukyKayvHJEOsJkbyYAUkyGLA28158100 = 41497706;     sDwYukyKayvHJEOsJkbyYAUkyGLA27037431 = sDwYukyKayvHJEOsJkbyYAUkyGLA71879767;     sDwYukyKayvHJEOsJkbyYAUkyGLA71879767 = sDwYukyKayvHJEOsJkbyYAUkyGLA4790544;     sDwYukyKayvHJEOsJkbyYAUkyGLA4790544 = sDwYukyKayvHJEOsJkbyYAUkyGLA3537109;     sDwYukyKayvHJEOsJkbyYAUkyGLA3537109 = sDwYukyKayvHJEOsJkbyYAUkyGLA41215851;     sDwYukyKayvHJEOsJkbyYAUkyGLA41215851 = sDwYukyKayvHJEOsJkbyYAUkyGLA28950593;     sDwYukyKayvHJEOsJkbyYAUkyGLA28950593 = sDwYukyKayvHJEOsJkbyYAUkyGLA89886324;     sDwYukyKayvHJEOsJkbyYAUkyGLA89886324 = sDwYukyKayvHJEOsJkbyYAUkyGLA35670280;     sDwYukyKayvHJEOsJkbyYAUkyGLA35670280 = sDwYukyKayvHJEOsJkbyYAUkyGLA47771131;     sDwYukyKayvHJEOsJkbyYAUkyGLA47771131 = sDwYukyKayvHJEOsJkbyYAUkyGLA42249151;     sDwYukyKayvHJEOsJkbyYAUkyGLA42249151 = sDwYukyKayvHJEOsJkbyYAUkyGLA15656972;     sDwYukyKayvHJEOsJkbyYAUkyGLA15656972 = sDwYukyKayvHJEOsJkbyYAUkyGLA79742642;     sDwYukyKayvHJEOsJkbyYAUkyGLA79742642 = sDwYukyKayvHJEOsJkbyYAUkyGLA41635502;     sDwYukyKayvHJEOsJkbyYAUkyGLA41635502 = sDwYukyKayvHJEOsJkbyYAUkyGLA37928147;     sDwYukyKayvHJEOsJkbyYAUkyGLA37928147 = sDwYukyKayvHJEOsJkbyYAUkyGLA4895992;     sDwYukyKayvHJEOsJkbyYAUkyGLA4895992 = sDwYukyKayvHJEOsJkbyYAUkyGLA16445614;     sDwYukyKayvHJEOsJkbyYAUkyGLA16445614 = sDwYukyKayvHJEOsJkbyYAUkyGLA14777424;     sDwYukyKayvHJEOsJkbyYAUkyGLA14777424 = sDwYukyKayvHJEOsJkbyYAUkyGLA60604682;     sDwYukyKayvHJEOsJkbyYAUkyGLA60604682 = sDwYukyKayvHJEOsJkbyYAUkyGLA26245685;     sDwYukyKayvHJEOsJkbyYAUkyGLA26245685 = sDwYukyKayvHJEOsJkbyYAUkyGLA27117865;     sDwYukyKayvHJEOsJkbyYAUkyGLA27117865 = sDwYukyKayvHJEOsJkbyYAUkyGLA24374011;     sDwYukyKayvHJEOsJkbyYAUkyGLA24374011 = sDwYukyKayvHJEOsJkbyYAUkyGLA69701314;     sDwYukyKayvHJEOsJkbyYAUkyGLA69701314 = sDwYukyKayvHJEOsJkbyYAUkyGLA33388198;     sDwYukyKayvHJEOsJkbyYAUkyGLA33388198 = sDwYukyKayvHJEOsJkbyYAUkyGLA20202252;     sDwYukyKayvHJEOsJkbyYAUkyGLA20202252 = sDwYukyKayvHJEOsJkbyYAUkyGLA85224370;     sDwYukyKayvHJEOsJkbyYAUkyGLA85224370 = sDwYukyKayvHJEOsJkbyYAUkyGLA44809879;     sDwYukyKayvHJEOsJkbyYAUkyGLA44809879 = sDwYukyKayvHJEOsJkbyYAUkyGLA20325559;     sDwYukyKayvHJEOsJkbyYAUkyGLA20325559 = sDwYukyKayvHJEOsJkbyYAUkyGLA4568946;     sDwYukyKayvHJEOsJkbyYAUkyGLA4568946 = sDwYukyKayvHJEOsJkbyYAUkyGLA34307589;     sDwYukyKayvHJEOsJkbyYAUkyGLA34307589 = sDwYukyKayvHJEOsJkbyYAUkyGLA89634313;     sDwYukyKayvHJEOsJkbyYAUkyGLA89634313 = sDwYukyKayvHJEOsJkbyYAUkyGLA53433594;     sDwYukyKayvHJEOsJkbyYAUkyGLA53433594 = sDwYukyKayvHJEOsJkbyYAUkyGLA87039672;     sDwYukyKayvHJEOsJkbyYAUkyGLA87039672 = sDwYukyKayvHJEOsJkbyYAUkyGLA65537654;     sDwYukyKayvHJEOsJkbyYAUkyGLA65537654 = sDwYukyKayvHJEOsJkbyYAUkyGLA23035834;     sDwYukyKayvHJEOsJkbyYAUkyGLA23035834 = sDwYukyKayvHJEOsJkbyYAUkyGLA28860230;     sDwYukyKayvHJEOsJkbyYAUkyGLA28860230 = sDwYukyKayvHJEOsJkbyYAUkyGLA16852729;     sDwYukyKayvHJEOsJkbyYAUkyGLA16852729 = sDwYukyKayvHJEOsJkbyYAUkyGLA4324052;     sDwYukyKayvHJEOsJkbyYAUkyGLA4324052 = sDwYukyKayvHJEOsJkbyYAUkyGLA91955471;     sDwYukyKayvHJEOsJkbyYAUkyGLA91955471 = sDwYukyKayvHJEOsJkbyYAUkyGLA66868735;     sDwYukyKayvHJEOsJkbyYAUkyGLA66868735 = sDwYukyKayvHJEOsJkbyYAUkyGLA65391581;     sDwYukyKayvHJEOsJkbyYAUkyGLA65391581 = sDwYukyKayvHJEOsJkbyYAUkyGLA26958241;     sDwYukyKayvHJEOsJkbyYAUkyGLA26958241 = sDwYukyKayvHJEOsJkbyYAUkyGLA75262098;     sDwYukyKayvHJEOsJkbyYAUkyGLA75262098 = sDwYukyKayvHJEOsJkbyYAUkyGLA47704751;     sDwYukyKayvHJEOsJkbyYAUkyGLA47704751 = sDwYukyKayvHJEOsJkbyYAUkyGLA79099696;     sDwYukyKayvHJEOsJkbyYAUkyGLA79099696 = sDwYukyKayvHJEOsJkbyYAUkyGLA56775399;     sDwYukyKayvHJEOsJkbyYAUkyGLA56775399 = sDwYukyKayvHJEOsJkbyYAUkyGLA8886598;     sDwYukyKayvHJEOsJkbyYAUkyGLA8886598 = sDwYukyKayvHJEOsJkbyYAUkyGLA30745494;     sDwYukyKayvHJEOsJkbyYAUkyGLA30745494 = sDwYukyKayvHJEOsJkbyYAUkyGLA85015510;     sDwYukyKayvHJEOsJkbyYAUkyGLA85015510 = sDwYukyKayvHJEOsJkbyYAUkyGLA19934802;     sDwYukyKayvHJEOsJkbyYAUkyGLA19934802 = sDwYukyKayvHJEOsJkbyYAUkyGLA23803488;     sDwYukyKayvHJEOsJkbyYAUkyGLA23803488 = sDwYukyKayvHJEOsJkbyYAUkyGLA10357442;     sDwYukyKayvHJEOsJkbyYAUkyGLA10357442 = sDwYukyKayvHJEOsJkbyYAUkyGLA59423997;     sDwYukyKayvHJEOsJkbyYAUkyGLA59423997 = sDwYukyKayvHJEOsJkbyYAUkyGLA77715631;     sDwYukyKayvHJEOsJkbyYAUkyGLA77715631 = sDwYukyKayvHJEOsJkbyYAUkyGLA46573271;     sDwYukyKayvHJEOsJkbyYAUkyGLA46573271 = sDwYukyKayvHJEOsJkbyYAUkyGLA78024391;     sDwYukyKayvHJEOsJkbyYAUkyGLA78024391 = sDwYukyKayvHJEOsJkbyYAUkyGLA57336116;     sDwYukyKayvHJEOsJkbyYAUkyGLA57336116 = sDwYukyKayvHJEOsJkbyYAUkyGLA38491569;     sDwYukyKayvHJEOsJkbyYAUkyGLA38491569 = sDwYukyKayvHJEOsJkbyYAUkyGLA84588291;     sDwYukyKayvHJEOsJkbyYAUkyGLA84588291 = sDwYukyKayvHJEOsJkbyYAUkyGLA18312739;     sDwYukyKayvHJEOsJkbyYAUkyGLA18312739 = sDwYukyKayvHJEOsJkbyYAUkyGLA96405971;     sDwYukyKayvHJEOsJkbyYAUkyGLA96405971 = sDwYukyKayvHJEOsJkbyYAUkyGLA8625034;     sDwYukyKayvHJEOsJkbyYAUkyGLA8625034 = sDwYukyKayvHJEOsJkbyYAUkyGLA85317378;     sDwYukyKayvHJEOsJkbyYAUkyGLA85317378 = sDwYukyKayvHJEOsJkbyYAUkyGLA1362692;     sDwYukyKayvHJEOsJkbyYAUkyGLA1362692 = sDwYukyKayvHJEOsJkbyYAUkyGLA58136818;     sDwYukyKayvHJEOsJkbyYAUkyGLA58136818 = sDwYukyKayvHJEOsJkbyYAUkyGLA88815556;     sDwYukyKayvHJEOsJkbyYAUkyGLA88815556 = sDwYukyKayvHJEOsJkbyYAUkyGLA28617299;     sDwYukyKayvHJEOsJkbyYAUkyGLA28617299 = sDwYukyKayvHJEOsJkbyYAUkyGLA14204988;     sDwYukyKayvHJEOsJkbyYAUkyGLA14204988 = sDwYukyKayvHJEOsJkbyYAUkyGLA18599669;     sDwYukyKayvHJEOsJkbyYAUkyGLA18599669 = sDwYukyKayvHJEOsJkbyYAUkyGLA9067917;     sDwYukyKayvHJEOsJkbyYAUkyGLA9067917 = sDwYukyKayvHJEOsJkbyYAUkyGLA88043262;     sDwYukyKayvHJEOsJkbyYAUkyGLA88043262 = sDwYukyKayvHJEOsJkbyYAUkyGLA12121562;     sDwYukyKayvHJEOsJkbyYAUkyGLA12121562 = sDwYukyKayvHJEOsJkbyYAUkyGLA22821952;     sDwYukyKayvHJEOsJkbyYAUkyGLA22821952 = sDwYukyKayvHJEOsJkbyYAUkyGLA93735946;     sDwYukyKayvHJEOsJkbyYAUkyGLA93735946 = sDwYukyKayvHJEOsJkbyYAUkyGLA60854103;     sDwYukyKayvHJEOsJkbyYAUkyGLA60854103 = sDwYukyKayvHJEOsJkbyYAUkyGLA159624;     sDwYukyKayvHJEOsJkbyYAUkyGLA159624 = sDwYukyKayvHJEOsJkbyYAUkyGLA49111912;     sDwYukyKayvHJEOsJkbyYAUkyGLA49111912 = sDwYukyKayvHJEOsJkbyYAUkyGLA21996564;     sDwYukyKayvHJEOsJkbyYAUkyGLA21996564 = sDwYukyKayvHJEOsJkbyYAUkyGLA54288502;     sDwYukyKayvHJEOsJkbyYAUkyGLA54288502 = sDwYukyKayvHJEOsJkbyYAUkyGLA63426852;     sDwYukyKayvHJEOsJkbyYAUkyGLA63426852 = sDwYukyKayvHJEOsJkbyYAUkyGLA76337772;     sDwYukyKayvHJEOsJkbyYAUkyGLA76337772 = sDwYukyKayvHJEOsJkbyYAUkyGLA14064386;     sDwYukyKayvHJEOsJkbyYAUkyGLA14064386 = sDwYukyKayvHJEOsJkbyYAUkyGLA35310048;     sDwYukyKayvHJEOsJkbyYAUkyGLA35310048 = sDwYukyKayvHJEOsJkbyYAUkyGLA84634143;     sDwYukyKayvHJEOsJkbyYAUkyGLA84634143 = sDwYukyKayvHJEOsJkbyYAUkyGLA10504101;     sDwYukyKayvHJEOsJkbyYAUkyGLA10504101 = sDwYukyKayvHJEOsJkbyYAUkyGLA79276871;     sDwYukyKayvHJEOsJkbyYAUkyGLA79276871 = sDwYukyKayvHJEOsJkbyYAUkyGLA94009596;     sDwYukyKayvHJEOsJkbyYAUkyGLA94009596 = sDwYukyKayvHJEOsJkbyYAUkyGLA9324042;     sDwYukyKayvHJEOsJkbyYAUkyGLA9324042 = sDwYukyKayvHJEOsJkbyYAUkyGLA18964383;     sDwYukyKayvHJEOsJkbyYAUkyGLA18964383 = sDwYukyKayvHJEOsJkbyYAUkyGLA45011443;     sDwYukyKayvHJEOsJkbyYAUkyGLA45011443 = sDwYukyKayvHJEOsJkbyYAUkyGLA71524113;     sDwYukyKayvHJEOsJkbyYAUkyGLA71524113 = sDwYukyKayvHJEOsJkbyYAUkyGLA78361159;     sDwYukyKayvHJEOsJkbyYAUkyGLA78361159 = sDwYukyKayvHJEOsJkbyYAUkyGLA19735760;     sDwYukyKayvHJEOsJkbyYAUkyGLA19735760 = sDwYukyKayvHJEOsJkbyYAUkyGLA73642733;     sDwYukyKayvHJEOsJkbyYAUkyGLA73642733 = sDwYukyKayvHJEOsJkbyYAUkyGLA70462763;     sDwYukyKayvHJEOsJkbyYAUkyGLA70462763 = sDwYukyKayvHJEOsJkbyYAUkyGLA56766547;     sDwYukyKayvHJEOsJkbyYAUkyGLA56766547 = sDwYukyKayvHJEOsJkbyYAUkyGLA41640862;     sDwYukyKayvHJEOsJkbyYAUkyGLA41640862 = sDwYukyKayvHJEOsJkbyYAUkyGLA73899406;     sDwYukyKayvHJEOsJkbyYAUkyGLA73899406 = sDwYukyKayvHJEOsJkbyYAUkyGLA89567932;     sDwYukyKayvHJEOsJkbyYAUkyGLA89567932 = sDwYukyKayvHJEOsJkbyYAUkyGLA90284139;     sDwYukyKayvHJEOsJkbyYAUkyGLA90284139 = sDwYukyKayvHJEOsJkbyYAUkyGLA28158100;     sDwYukyKayvHJEOsJkbyYAUkyGLA28158100 = sDwYukyKayvHJEOsJkbyYAUkyGLA27037431;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void wkATqmhZdgHVOVilJlrWBBFPa19447521() {     long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG81156368 = -449447433;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG85534137 = -33625158;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG9453638 = -877550293;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG89411038 = -676017648;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG55701941 = -965982202;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG65458645 = -403034221;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG27181092 = -25797797;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG99676531 = -721518106;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG79080940 = 42569905;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG65875973 = -551730272;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG58619661 = 86046702;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG18989905 = -792004241;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG62299258 = -170112942;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG6296542 = -20549493;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG52276587 = -611296205;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG88287633 = -85567058;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG42824872 = -302684023;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG5330670 = -476484710;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG1211979 = -129869412;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG50984516 = -388253935;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG68801955 = -444347220;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG37738862 = -823703657;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG60457063 = -83473653;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG83779899 = -983685876;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG80729231 = -641130394;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG52852313 = -255600332;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG12820619 = 20448712;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG36576841 = -855044885;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG85160027 = -81558905;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG58851670 = -164630603;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG28327961 = -279476968;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG49728303 = -209513768;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG7951758 = -16295440;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG64812123 = -718725840;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG74115473 = -234503202;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG72364575 = -123732729;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG73144337 = -357864360;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG79369307 = 75348427;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG47501280 = -297851550;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG35177680 = -44574400;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG32823047 = 27079868;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG58908182 = -636329738;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG93052873 = -997446634;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG54288694 = -192437595;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG33550440 = -648369074;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG61088380 = -687833246;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG85339393 = -207710470;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG43312860 = -101888007;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG9364952 = -897535625;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG18235824 = 93503244;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG17725182 = -132909220;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG46393226 = 64793174;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG91350834 = 30929050;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG12940551 = -327598567;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG50645566 = -184402882;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG43417507 = -625743777;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG25077075 = -950151506;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG25673738 = -893864417;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG8681807 = 65112745;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG2849629 = -610381871;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG52638026 = -323482934;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG90604250 = -170752913;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG14516504 = -539959202;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG20229270 = -792799493;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG37548013 = -172253305;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG8891358 = -704439530;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG11038147 = -675708802;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG97487134 = -451387102;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG32181069 = -786046291;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG79912011 = -387563477;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG15143297 = -727702698;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG63455565 = -278032450;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG57829390 = -78633160;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG66034298 = 14704987;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG18161469 = -315333804;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG9893773 = -808017482;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG44685988 = -826257023;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG6168369 = -891036058;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG50229460 = -235316803;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG19640852 = -953297148;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG67512919 = 52110138;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG69507759 = -877663282;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG27211889 = -957509261;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG66924204 = -75062149;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG41126489 = 68278616;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG81934734 = -244270143;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG58377469 = -140442818;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG95011206 = -688696873;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG14166558 = -434322958;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG30697967 = -608759426;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG47287501 = -173581223;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG47470599 = -463999944;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG70687500 = -989764318;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG44651652 = -687469680;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG82539653 = -721091466;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG42218796 = -802167219;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG44391679 = 3629463;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG72823603 = -104647142;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG16740682 = 79815709;    long qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG24659082 = -449447433;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG81156368 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG85534137;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG85534137 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG9453638;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG9453638 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG89411038;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG89411038 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG55701941;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG55701941 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG65458645;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG65458645 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG27181092;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG27181092 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG99676531;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG99676531 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG79080940;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG79080940 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG65875973;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG65875973 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG58619661;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG58619661 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG18989905;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG18989905 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG62299258;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG62299258 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG6296542;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG6296542 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG52276587;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG52276587 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG88287633;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG88287633 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG42824872;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG42824872 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG5330670;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG5330670 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG1211979;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG1211979 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG50984516;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG50984516 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG68801955;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG68801955 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG37738862;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG37738862 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG60457063;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG60457063 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG83779899;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG83779899 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG80729231;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG80729231 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG52852313;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG52852313 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG12820619;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG12820619 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG36576841;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG36576841 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG85160027;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG85160027 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG58851670;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG58851670 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG28327961;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG28327961 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG49728303;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG49728303 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG7951758;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG7951758 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG64812123;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG64812123 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG74115473;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG74115473 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG72364575;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG72364575 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG73144337;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG73144337 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG79369307;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG79369307 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG47501280;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG47501280 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG35177680;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG35177680 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG32823047;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG32823047 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG58908182;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG58908182 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG93052873;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG93052873 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG54288694;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG54288694 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG33550440;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG33550440 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG61088380;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG61088380 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG85339393;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG85339393 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG43312860;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG43312860 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG9364952;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG9364952 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG18235824;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG18235824 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG17725182;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG17725182 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG46393226;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG46393226 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG91350834;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG91350834 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG12940551;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG12940551 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG50645566;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG50645566 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG43417507;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG43417507 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG25077075;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG25077075 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG25673738;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG25673738 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG8681807;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG8681807 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG2849629;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG2849629 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG52638026;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG52638026 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG90604250;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG90604250 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG14516504;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG14516504 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG20229270;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG20229270 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG37548013;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG37548013 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG8891358;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG8891358 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG11038147;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG11038147 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG97487134;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG97487134 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG32181069;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG32181069 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG79912011;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG79912011 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG15143297;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG15143297 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG63455565;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG63455565 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG57829390;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG57829390 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG66034298;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG66034298 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG18161469;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG18161469 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG9893773;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG9893773 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG44685988;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG44685988 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG6168369;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG6168369 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG50229460;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG50229460 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG19640852;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG19640852 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG67512919;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG67512919 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG69507759;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG69507759 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG27211889;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG27211889 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG66924204;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG66924204 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG41126489;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG41126489 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG81934734;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG81934734 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG58377469;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG58377469 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG95011206;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG95011206 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG14166558;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG14166558 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG30697967;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG30697967 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG47287501;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG47287501 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG47470599;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG47470599 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG70687500;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG70687500 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG44651652;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG44651652 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG82539653;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG82539653 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG42218796;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG42218796 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG44391679;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG44391679 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG72823603;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG72823603 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG16740682;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG16740682 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG24659082;     qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG24659082 = qOthhKAtIqcuQWCsadKdxoHGixavsIvfktAYnswWGcGeG81156368;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void ddDVVVsWBzGJAhvDlkxRLYlSZHXNaHbIEtYFy10769354() {     double UXcBGDPvNxvJddoHYOEsl88465376 = -846676886;    double UXcBGDPvNxvJddoHYOEsl84453547 = -661033018;    double UXcBGDPvNxvJddoHYOEsl94868143 = -824742575;    double UXcBGDPvNxvJddoHYOEsl87350035 = -754433631;    double UXcBGDPvNxvJddoHYOEsl52405909 = -551775027;    double UXcBGDPvNxvJddoHYOEsl47599648 = -687622465;    double UXcBGDPvNxvJddoHYOEsl88325134 = -990118638;    double UXcBGDPvNxvJddoHYOEsl39576612 = -718819895;    double UXcBGDPvNxvJddoHYOEsl91982116 = 45504093;    double UXcBGDPvNxvJddoHYOEsl12696985 = -284326585;    double UXcBGDPvNxvJddoHYOEsl3774801 = -29785251;    double UXcBGDPvNxvJddoHYOEsl55516470 = -822553784;    double UXcBGDPvNxvJddoHYOEsl82351754 = -670605854;    double UXcBGDPvNxvJddoHYOEsl65915176 = 68919529;    double UXcBGDPvNxvJddoHYOEsl25655930 = -242809170;    double UXcBGDPvNxvJddoHYOEsl91800467 = -723117459;    double UXcBGDPvNxvJddoHYOEsl90663033 = -448441818;    double UXcBGDPvNxvJddoHYOEsl68946812 = -138288115;    double UXcBGDPvNxvJddoHYOEsl96896699 = -997547802;    double UXcBGDPvNxvJddoHYOEsl97588554 = -756208915;    double UXcBGDPvNxvJddoHYOEsl12942838 = -759215709;    double UXcBGDPvNxvJddoHYOEsl66334629 = -872510443;    double UXcBGDPvNxvJddoHYOEsl34094399 = -143126613;    double UXcBGDPvNxvJddoHYOEsl27134438 = 23639633;    double UXcBGDPvNxvJddoHYOEsl6150348 = -43068251;    double UXcBGDPvNxvJddoHYOEsl47798484 = -83258215;    double UXcBGDPvNxvJddoHYOEsl3391146 = -784300271;    double UXcBGDPvNxvJddoHYOEsl70826493 = -518511293;    double UXcBGDPvNxvJddoHYOEsl2845178 = -732705553;    double UXcBGDPvNxvJddoHYOEsl42346677 = -655870494;    double UXcBGDPvNxvJddoHYOEsl22744005 = -17488924;    double UXcBGDPvNxvJddoHYOEsl72970496 = -437244156;    double UXcBGDPvNxvJddoHYOEsl47329772 = -732546782;    double UXcBGDPvNxvJddoHYOEsl1140345 = -128278244;    double UXcBGDPvNxvJddoHYOEsl97493887 = -229046087;    double UXcBGDPvNxvJddoHYOEsl44510641 = -945476145;    double UXcBGDPvNxvJddoHYOEsl88097005 = -965846433;    double UXcBGDPvNxvJddoHYOEsl26126827 = -504639491;    double UXcBGDPvNxvJddoHYOEsl14891268 = -805891081;    double UXcBGDPvNxvJddoHYOEsl58402854 = -768839901;    double UXcBGDPvNxvJddoHYOEsl41901348 = -264189664;    double UXcBGDPvNxvJddoHYOEsl80177541 = -83117052;    double UXcBGDPvNxvJddoHYOEsl32405224 = -982939969;    double UXcBGDPvNxvJddoHYOEsl99866725 = -579221299;    double UXcBGDPvNxvJddoHYOEsl89345654 = -41220205;    double UXcBGDPvNxvJddoHYOEsl22125164 = -141779795;    double UXcBGDPvNxvJddoHYOEsl12026590 = -5397850;    double UXcBGDPvNxvJddoHYOEsl84353782 = -474720990;    double UXcBGDPvNxvJddoHYOEsl3469504 = -544068552;    double UXcBGDPvNxvJddoHYOEsl44451187 = -20744687;    double UXcBGDPvNxvJddoHYOEsl92934164 = -183128419;    double UXcBGDPvNxvJddoHYOEsl54953850 = -283804706;    double UXcBGDPvNxvJddoHYOEsl64055673 = -91984576;    double UXcBGDPvNxvJddoHYOEsl50733033 = -34739893;    double UXcBGDPvNxvJddoHYOEsl97826515 = -596829435;    double UXcBGDPvNxvJddoHYOEsl22130747 = -974166443;    double UXcBGDPvNxvJddoHYOEsl50359148 = -417906405;    double UXcBGDPvNxvJddoHYOEsl67733705 = -748382208;    double UXcBGDPvNxvJddoHYOEsl81199687 = -611365380;    double UXcBGDPvNxvJddoHYOEsl4607425 = -368516813;    double UXcBGDPvNxvJddoHYOEsl44208503 = -903322194;    double UXcBGDPvNxvJddoHYOEsl17498642 = -371607346;    double UXcBGDPvNxvJddoHYOEsl36731435 = -986114342;    double UXcBGDPvNxvJddoHYOEsl49635439 = -298625413;    double UXcBGDPvNxvJddoHYOEsl89952980 = -166837662;    double UXcBGDPvNxvJddoHYOEsl30804304 = -592541096;    double UXcBGDPvNxvJddoHYOEsl8186699 = 9992998;    double UXcBGDPvNxvJddoHYOEsl81211409 = -442327611;    double UXcBGDPvNxvJddoHYOEsl68421289 = -702034384;    double UXcBGDPvNxvJddoHYOEsl81145288 = -297333025;    double UXcBGDPvNxvJddoHYOEsl3703463 = -757271026;    double UXcBGDPvNxvJddoHYOEsl64536206 = -943802327;    double UXcBGDPvNxvJddoHYOEsl54055544 = -332397034;    double UXcBGDPvNxvJddoHYOEsl38493846 = -128707902;    double UXcBGDPvNxvJddoHYOEsl55687206 = -392019252;    double UXcBGDPvNxvJddoHYOEsl32765296 = -576098658;    double UXcBGDPvNxvJddoHYOEsl33929406 = -889570475;    double UXcBGDPvNxvJddoHYOEsl34227674 = -563905314;    double UXcBGDPvNxvJddoHYOEsl37788783 = -935140162;    double UXcBGDPvNxvJddoHYOEsl84025184 = -901288457;    double UXcBGDPvNxvJddoHYOEsl35771895 = 22139635;    double UXcBGDPvNxvJddoHYOEsl19037363 = -209579282;    double UXcBGDPvNxvJddoHYOEsl67356989 = -974442741;    double UXcBGDPvNxvJddoHYOEsl58393990 = -611960866;    double UXcBGDPvNxvJddoHYOEsl49412512 = -372742075;    double UXcBGDPvNxvJddoHYOEsl67790154 = -733684218;    double UXcBGDPvNxvJddoHYOEsl8914823 = -245259580;    double UXcBGDPvNxvJddoHYOEsl96596738 = -597806889;    double UXcBGDPvNxvJddoHYOEsl3313829 = -531448809;    double UXcBGDPvNxvJddoHYOEsl75363140 = -254879644;    double UXcBGDPvNxvJddoHYOEsl94151493 = -427569740;    double UXcBGDPvNxvJddoHYOEsl20363300 = -117464226;    double UXcBGDPvNxvJddoHYOEsl44927140 = -893274112;    double UXcBGDPvNxvJddoHYOEsl10283844 = -337374269;    double UXcBGDPvNxvJddoHYOEsl14194351 = -865517707;    double UXcBGDPvNxvJddoHYOEsl24402706 = -892582318;    double UXcBGDPvNxvJddoHYOEsl43446107 = -97002710;    double UXcBGDPvNxvJddoHYOEsl82769784 = -584314556;    double UXcBGDPvNxvJddoHYOEsl9913745 = -312383638;    double UXcBGDPvNxvJddoHYOEsl58541350 = -846676886;     UXcBGDPvNxvJddoHYOEsl88465376 = UXcBGDPvNxvJddoHYOEsl84453547;     UXcBGDPvNxvJddoHYOEsl84453547 = UXcBGDPvNxvJddoHYOEsl94868143;     UXcBGDPvNxvJddoHYOEsl94868143 = UXcBGDPvNxvJddoHYOEsl87350035;     UXcBGDPvNxvJddoHYOEsl87350035 = UXcBGDPvNxvJddoHYOEsl52405909;     UXcBGDPvNxvJddoHYOEsl52405909 = UXcBGDPvNxvJddoHYOEsl47599648;     UXcBGDPvNxvJddoHYOEsl47599648 = UXcBGDPvNxvJddoHYOEsl88325134;     UXcBGDPvNxvJddoHYOEsl88325134 = UXcBGDPvNxvJddoHYOEsl39576612;     UXcBGDPvNxvJddoHYOEsl39576612 = UXcBGDPvNxvJddoHYOEsl91982116;     UXcBGDPvNxvJddoHYOEsl91982116 = UXcBGDPvNxvJddoHYOEsl12696985;     UXcBGDPvNxvJddoHYOEsl12696985 = UXcBGDPvNxvJddoHYOEsl3774801;     UXcBGDPvNxvJddoHYOEsl3774801 = UXcBGDPvNxvJddoHYOEsl55516470;     UXcBGDPvNxvJddoHYOEsl55516470 = UXcBGDPvNxvJddoHYOEsl82351754;     UXcBGDPvNxvJddoHYOEsl82351754 = UXcBGDPvNxvJddoHYOEsl65915176;     UXcBGDPvNxvJddoHYOEsl65915176 = UXcBGDPvNxvJddoHYOEsl25655930;     UXcBGDPvNxvJddoHYOEsl25655930 = UXcBGDPvNxvJddoHYOEsl91800467;     UXcBGDPvNxvJddoHYOEsl91800467 = UXcBGDPvNxvJddoHYOEsl90663033;     UXcBGDPvNxvJddoHYOEsl90663033 = UXcBGDPvNxvJddoHYOEsl68946812;     UXcBGDPvNxvJddoHYOEsl68946812 = UXcBGDPvNxvJddoHYOEsl96896699;     UXcBGDPvNxvJddoHYOEsl96896699 = UXcBGDPvNxvJddoHYOEsl97588554;     UXcBGDPvNxvJddoHYOEsl97588554 = UXcBGDPvNxvJddoHYOEsl12942838;     UXcBGDPvNxvJddoHYOEsl12942838 = UXcBGDPvNxvJddoHYOEsl66334629;     UXcBGDPvNxvJddoHYOEsl66334629 = UXcBGDPvNxvJddoHYOEsl34094399;     UXcBGDPvNxvJddoHYOEsl34094399 = UXcBGDPvNxvJddoHYOEsl27134438;     UXcBGDPvNxvJddoHYOEsl27134438 = UXcBGDPvNxvJddoHYOEsl6150348;     UXcBGDPvNxvJddoHYOEsl6150348 = UXcBGDPvNxvJddoHYOEsl47798484;     UXcBGDPvNxvJddoHYOEsl47798484 = UXcBGDPvNxvJddoHYOEsl3391146;     UXcBGDPvNxvJddoHYOEsl3391146 = UXcBGDPvNxvJddoHYOEsl70826493;     UXcBGDPvNxvJddoHYOEsl70826493 = UXcBGDPvNxvJddoHYOEsl2845178;     UXcBGDPvNxvJddoHYOEsl2845178 = UXcBGDPvNxvJddoHYOEsl42346677;     UXcBGDPvNxvJddoHYOEsl42346677 = UXcBGDPvNxvJddoHYOEsl22744005;     UXcBGDPvNxvJddoHYOEsl22744005 = UXcBGDPvNxvJddoHYOEsl72970496;     UXcBGDPvNxvJddoHYOEsl72970496 = UXcBGDPvNxvJddoHYOEsl47329772;     UXcBGDPvNxvJddoHYOEsl47329772 = UXcBGDPvNxvJddoHYOEsl1140345;     UXcBGDPvNxvJddoHYOEsl1140345 = UXcBGDPvNxvJddoHYOEsl97493887;     UXcBGDPvNxvJddoHYOEsl97493887 = UXcBGDPvNxvJddoHYOEsl44510641;     UXcBGDPvNxvJddoHYOEsl44510641 = UXcBGDPvNxvJddoHYOEsl88097005;     UXcBGDPvNxvJddoHYOEsl88097005 = UXcBGDPvNxvJddoHYOEsl26126827;     UXcBGDPvNxvJddoHYOEsl26126827 = UXcBGDPvNxvJddoHYOEsl14891268;     UXcBGDPvNxvJddoHYOEsl14891268 = UXcBGDPvNxvJddoHYOEsl58402854;     UXcBGDPvNxvJddoHYOEsl58402854 = UXcBGDPvNxvJddoHYOEsl41901348;     UXcBGDPvNxvJddoHYOEsl41901348 = UXcBGDPvNxvJddoHYOEsl80177541;     UXcBGDPvNxvJddoHYOEsl80177541 = UXcBGDPvNxvJddoHYOEsl32405224;     UXcBGDPvNxvJddoHYOEsl32405224 = UXcBGDPvNxvJddoHYOEsl99866725;     UXcBGDPvNxvJddoHYOEsl99866725 = UXcBGDPvNxvJddoHYOEsl89345654;     UXcBGDPvNxvJddoHYOEsl89345654 = UXcBGDPvNxvJddoHYOEsl22125164;     UXcBGDPvNxvJddoHYOEsl22125164 = UXcBGDPvNxvJddoHYOEsl12026590;     UXcBGDPvNxvJddoHYOEsl12026590 = UXcBGDPvNxvJddoHYOEsl84353782;     UXcBGDPvNxvJddoHYOEsl84353782 = UXcBGDPvNxvJddoHYOEsl3469504;     UXcBGDPvNxvJddoHYOEsl3469504 = UXcBGDPvNxvJddoHYOEsl44451187;     UXcBGDPvNxvJddoHYOEsl44451187 = UXcBGDPvNxvJddoHYOEsl92934164;     UXcBGDPvNxvJddoHYOEsl92934164 = UXcBGDPvNxvJddoHYOEsl54953850;     UXcBGDPvNxvJddoHYOEsl54953850 = UXcBGDPvNxvJddoHYOEsl64055673;     UXcBGDPvNxvJddoHYOEsl64055673 = UXcBGDPvNxvJddoHYOEsl50733033;     UXcBGDPvNxvJddoHYOEsl50733033 = UXcBGDPvNxvJddoHYOEsl97826515;     UXcBGDPvNxvJddoHYOEsl97826515 = UXcBGDPvNxvJddoHYOEsl22130747;     UXcBGDPvNxvJddoHYOEsl22130747 = UXcBGDPvNxvJddoHYOEsl50359148;     UXcBGDPvNxvJddoHYOEsl50359148 = UXcBGDPvNxvJddoHYOEsl67733705;     UXcBGDPvNxvJddoHYOEsl67733705 = UXcBGDPvNxvJddoHYOEsl81199687;     UXcBGDPvNxvJddoHYOEsl81199687 = UXcBGDPvNxvJddoHYOEsl4607425;     UXcBGDPvNxvJddoHYOEsl4607425 = UXcBGDPvNxvJddoHYOEsl44208503;     UXcBGDPvNxvJddoHYOEsl44208503 = UXcBGDPvNxvJddoHYOEsl17498642;     UXcBGDPvNxvJddoHYOEsl17498642 = UXcBGDPvNxvJddoHYOEsl36731435;     UXcBGDPvNxvJddoHYOEsl36731435 = UXcBGDPvNxvJddoHYOEsl49635439;     UXcBGDPvNxvJddoHYOEsl49635439 = UXcBGDPvNxvJddoHYOEsl89952980;     UXcBGDPvNxvJddoHYOEsl89952980 = UXcBGDPvNxvJddoHYOEsl30804304;     UXcBGDPvNxvJddoHYOEsl30804304 = UXcBGDPvNxvJddoHYOEsl8186699;     UXcBGDPvNxvJddoHYOEsl8186699 = UXcBGDPvNxvJddoHYOEsl81211409;     UXcBGDPvNxvJddoHYOEsl81211409 = UXcBGDPvNxvJddoHYOEsl68421289;     UXcBGDPvNxvJddoHYOEsl68421289 = UXcBGDPvNxvJddoHYOEsl81145288;     UXcBGDPvNxvJddoHYOEsl81145288 = UXcBGDPvNxvJddoHYOEsl3703463;     UXcBGDPvNxvJddoHYOEsl3703463 = UXcBGDPvNxvJddoHYOEsl64536206;     UXcBGDPvNxvJddoHYOEsl64536206 = UXcBGDPvNxvJddoHYOEsl54055544;     UXcBGDPvNxvJddoHYOEsl54055544 = UXcBGDPvNxvJddoHYOEsl38493846;     UXcBGDPvNxvJddoHYOEsl38493846 = UXcBGDPvNxvJddoHYOEsl55687206;     UXcBGDPvNxvJddoHYOEsl55687206 = UXcBGDPvNxvJddoHYOEsl32765296;     UXcBGDPvNxvJddoHYOEsl32765296 = UXcBGDPvNxvJddoHYOEsl33929406;     UXcBGDPvNxvJddoHYOEsl33929406 = UXcBGDPvNxvJddoHYOEsl34227674;     UXcBGDPvNxvJddoHYOEsl34227674 = UXcBGDPvNxvJddoHYOEsl37788783;     UXcBGDPvNxvJddoHYOEsl37788783 = UXcBGDPvNxvJddoHYOEsl84025184;     UXcBGDPvNxvJddoHYOEsl84025184 = UXcBGDPvNxvJddoHYOEsl35771895;     UXcBGDPvNxvJddoHYOEsl35771895 = UXcBGDPvNxvJddoHYOEsl19037363;     UXcBGDPvNxvJddoHYOEsl19037363 = UXcBGDPvNxvJddoHYOEsl67356989;     UXcBGDPvNxvJddoHYOEsl67356989 = UXcBGDPvNxvJddoHYOEsl58393990;     UXcBGDPvNxvJddoHYOEsl58393990 = UXcBGDPvNxvJddoHYOEsl49412512;     UXcBGDPvNxvJddoHYOEsl49412512 = UXcBGDPvNxvJddoHYOEsl67790154;     UXcBGDPvNxvJddoHYOEsl67790154 = UXcBGDPvNxvJddoHYOEsl8914823;     UXcBGDPvNxvJddoHYOEsl8914823 = UXcBGDPvNxvJddoHYOEsl96596738;     UXcBGDPvNxvJddoHYOEsl96596738 = UXcBGDPvNxvJddoHYOEsl3313829;     UXcBGDPvNxvJddoHYOEsl3313829 = UXcBGDPvNxvJddoHYOEsl75363140;     UXcBGDPvNxvJddoHYOEsl75363140 = UXcBGDPvNxvJddoHYOEsl94151493;     UXcBGDPvNxvJddoHYOEsl94151493 = UXcBGDPvNxvJddoHYOEsl20363300;     UXcBGDPvNxvJddoHYOEsl20363300 = UXcBGDPvNxvJddoHYOEsl44927140;     UXcBGDPvNxvJddoHYOEsl44927140 = UXcBGDPvNxvJddoHYOEsl10283844;     UXcBGDPvNxvJddoHYOEsl10283844 = UXcBGDPvNxvJddoHYOEsl14194351;     UXcBGDPvNxvJddoHYOEsl14194351 = UXcBGDPvNxvJddoHYOEsl24402706;     UXcBGDPvNxvJddoHYOEsl24402706 = UXcBGDPvNxvJddoHYOEsl43446107;     UXcBGDPvNxvJddoHYOEsl43446107 = UXcBGDPvNxvJddoHYOEsl82769784;     UXcBGDPvNxvJddoHYOEsl82769784 = UXcBGDPvNxvJddoHYOEsl9913745;     UXcBGDPvNxvJddoHYOEsl9913745 = UXcBGDPvNxvJddoHYOEsl58541350;     UXcBGDPvNxvJddoHYOEsl58541350 = UXcBGDPvNxvJddoHYOEsl88465376;}
// Junk Finished
