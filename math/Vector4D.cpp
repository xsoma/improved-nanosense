#include "Vector4D.hpp"

#include <cmath>
#include <limits>

void VectorCopy(const Vector4D& src, Vector4D& dst)
{
    dst.x = src.x;
    dst.y = src.y;
    dst.z = src.z;
    dst.w = src.w;
}
void VectorLerp(const Vector4D& src1, const Vector4D& src2, vec_t t, Vector4D& dest)
{
    dest.x = src1.x + (src2.x - src1.x) * t;
    dest.y = src1.y + (src2.y - src1.y) * t;
    dest.z = src1.z + (src2.z - src1.z) * t;
    dest.w = src1.w + (src2.w - src1.w) * t;
}
float VectorLength(const Vector4D& v)
{
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w);
}

vec_t NormalizeVector(Vector4D& v)
{
    vec_t l = v.Length();
    if(l != 0.0f) {
        v /= l;
    } else {
        v.x = v.y = v.z = v.w = 0.0f;
    }
    return l;
}

Vector4D::Vector4D(void)
{
    Invalidate();
}
Vector4D::Vector4D(vec_t X, vec_t Y, vec_t Z, vec_t W)
{
    x = X;
    y = Y;
    z = Z;
    w = W;
}
Vector4D::Vector4D(vec_t* clr)
{
    x = clr[0];
    y = clr[1];
    z = clr[2];
    w = clr[3];
}

//-----------------------------------------------------------------------------
// initialization
//-----------------------------------------------------------------------------

void Vector4D::Init(vec_t ix, vec_t iy, vec_t iz, vec_t iw)
{
    x = ix; y = iy; z = iz; w = iw;
}

void Vector4D::Random(vec_t minVal, vec_t maxVal)
{
    x = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
    y = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
    z = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
    w = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
}

// This should really be a single opcode on the PowerPC (move r0 onto the vec reg)
void Vector4D::Zero()
{
    x = y = z = w = 0.0f;
}

//-----------------------------------------------------------------------------
// assignment
//-----------------------------------------------------------------------------

Vector4D& Vector4D::operator=(const Vector4D &vOther)
{
    x = vOther.x; y = vOther.y; z = vOther.z; w = vOther.w;
    return *this;
}


//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------
vec_t& Vector4D::operator[](int i)
{
    return ((vec_t*)this)[i];
}

vec_t Vector4D::operator[](int i) const
{
    return ((vec_t*)this)[i];
}


//-----------------------------------------------------------------------------
// Base address...
//-----------------------------------------------------------------------------
vec_t* Vector4D::Base()
{
    return (vec_t*)this;
}

vec_t const* Vector4D::Base() const
{
    return (vec_t const*)this;
}

//-----------------------------------------------------------------------------
// IsValid?
//-----------------------------------------------------------------------------

bool Vector4D::IsValid() const
{
    return !isinf(x) && !isinf(y) && !isinf(z) && !isinf(w);
}

//-----------------------------------------------------------------------------
// Invalidate
//-----------------------------------------------------------------------------

void Vector4D::Invalidate()
{
    //#ifdef _DEBUG
    //#ifdef VECTOR_PARANOIA
    x = y = z = w = std::numeric_limits<float>::infinity();
    //#endif
    //#endif
}

//-----------------------------------------------------------------------------
// comparison
//-----------------------------------------------------------------------------

bool Vector4D::operator==(const Vector4D& src) const
{
    return (src.x == x) && (src.y == y) && (src.z == z) && (src.w == w);
}

bool Vector4D::operator!=(const Vector4D& src) const
{
    return (src.x != x) || (src.y != y) || (src.z != z) || (src.w != w);
}


//-----------------------------------------------------------------------------
// Copy
//-----------------------------------------------------------------------------
void Vector4D::CopyToArray(float* rgfl) const
{
    rgfl[0] = x, rgfl[1] = y, rgfl[2] = z; rgfl[3] = w;
}

//-----------------------------------------------------------------------------
// standard Math operations
//-----------------------------------------------------------------------------
// #pragma message("TODO: these should be SSE")

void Vector4D::Negate()
{
    x = -x; y = -y; z = -z; w = -w;
}

// Get the component of this vector parallel to some other given vector
Vector4D Vector4D::ProjectOnto(const Vector4D& onto)
{
    return onto * (this->Dot(onto) / (onto.LengthSqr()));
}

// FIXME: Remove
// For backwards compatability
void Vector4D::MulAdd(const Vector4D& a, const Vector4D& b, float scalar)
{
    x = a.x + b.x * scalar;
    y = a.y + b.y * scalar;
    z = a.z + b.z * scalar;
    w = a.w + b.w * scalar;
}

Vector4D VectorLerp(const Vector4D& src1, const Vector4D& src2, vec_t t)
{
    Vector4D result;
    VectorLerp(src1, src2, t, result);
    return result;
}

vec_t Vector4D::Dot(const Vector4D& b) const
{
    return (x*b.x + y*b.y + z*b.z + w*b.w);
}
void VectorClear(Vector4D& a)
{
    a.x = a.y = a.z = a.w = 0.0f;
}

vec_t Vector4D::Length(void) const
{
    return sqrt(x*x + y*y + z*z + w*w);
}

// check a point against a box
bool Vector4D::WithinAABox(Vector4D const &boxmin, Vector4D const &boxmax)
{
    return (
        (x >= boxmin.x) && (x <= boxmax.x) &&
        (y >= boxmin.y) && (y <= boxmax.y) &&
        (z >= boxmin.z) && (z <= boxmax.z) &&
        (w >= boxmin.w) && (w <= boxmax.w)
        );
}

//-----------------------------------------------------------------------------
// Get the distance from this vector to the other one 
//-----------------------------------------------------------------------------
vec_t Vector4D::DistTo(const Vector4D &vOther) const
{
    Vector4D delta;
    delta = *this - vOther;
    return delta.Length();
}

//-----------------------------------------------------------------------------
// Returns a vector with the min or max in X, Y, and Z.
//-----------------------------------------------------------------------------
Vector4D Vector4D::Min(const Vector4D &vOther) const
{
    return Vector4D(x < vOther.x ? x : vOther.x,
        y < vOther.y ? y : vOther.y,
        z < vOther.z ? z : vOther.z,
        w < vOther.w ? w : vOther.w);
}

Vector4D Vector4D::Max(const Vector4D &vOther) const
{
    return Vector4D(x > vOther.x ? x : vOther.x,
        y > vOther.y ? y : vOther.y,
        z > vOther.z ? z : vOther.z,
        w > vOther.w ? w : vOther.w);
}


//-----------------------------------------------------------------------------
// arithmetic operations
//-----------------------------------------------------------------------------

Vector4D Vector4D::operator-(void) const
{
    return Vector4D(-x, -y, -z, -w);
}

Vector4D Vector4D::operator+(const Vector4D& v) const
{
    return Vector4D(x + v.x, y + v.y, z + v.z, w + v.w);
}

Vector4D Vector4D::operator-(const Vector4D& v) const
{
    return Vector4D(x - v.x, y - v.y, z - v.z, w - v.w);
}

Vector4D Vector4D::operator*(float fl) const
{
    return Vector4D(x * fl, y * fl, z * fl, w * fl);
}

Vector4D Vector4D::operator*(const Vector4D& v) const
{
    return Vector4D(x * v.x, y * v.y, z * v.z, w * v.w);
}

Vector4D Vector4D::operator/(float fl) const
{
    return Vector4D(x / fl, y / fl, z / fl, w / fl);
}

Vector4D Vector4D::operator/(const Vector4D& v) const
{
    return Vector4D(x / v.x, y / v.y, z / v.z, w / v.w);
}

Vector4D operator*(float fl, const Vector4D& v)
{
    return v * fl;
}












































































































// Junk Code By Troll Face & Thaisen's Gen
void oZHlozdhUpwTmIIsemztXPKMMmNefYkMUfQMJkdq37807796() {     int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq54340047 = -878266794;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq51912892 = -770623371;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq32199502 = -144396794;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq52374154 = -139493120;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq93737268 = -320595715;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq96735259 = -791271439;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq94696956 = -619832560;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq74744916 = -317831856;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq85428323 = -33114540;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq64610312 = -255269020;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq58745181 = -186140649;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq84202625 = -785991964;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq8788796 = -147718465;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq17977948 = -537659858;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq27494943 = -315200908;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq2046301 = -932844860;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq88213759 = -986518519;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq81572060 = -167147506;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq76144682 = -38475917;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq28241083 = -373656431;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq77726757 = -841067462;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq45407211 = -439160125;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq89543364 = 61571123;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq36807411 = -224327287;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq75406835 = -849550983;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq62471168 = -219612057;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq29096382 = -764390072;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq55550282 = -814291427;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq74797181 = -412018233;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq16662565 = -33438395;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq37912472 = -923248359;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq93645700 = 25747699;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq54798842 = -747187227;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq36181684 = -294724300;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq28197541 = -737432848;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq88093643 = -470248628;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq13247017 = -78730689;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq71880996 = -343374948;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq47977781 = -312831698;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq95165336 = -313829298;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq39759260 = -612834766;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq85348535 = -25006397;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq10461590 = -4283714;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq46629113 = -907623881;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq18895460 = -501947591;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq40470439 = -931042764;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq93459532 = -555877356;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq67562516 = -418289682;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq17629198 = -400970592;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq50563310 = -764364100;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq35308087 = 93256965;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq68154956 = -893443693;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq42994632 = -402869944;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq76380501 = -307099577;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq60081039 = -567176773;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq8932836 = -339106669;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq62369527 = -732194494;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq95392091 = -920069507;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq76967318 = -289942138;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq31266100 = -983658;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq67638878 = 73118633;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq39146674 = -805541133;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq99947734 = -905813623;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq68765759 = -999676145;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq26697841 = -332020662;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq65099480 = -111888348;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq29403784 = 61195262;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq72607111 = -852994165;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq89780406 = -800227011;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq39401299 = -844952281;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq88799284 = -754114172;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq16332763 = -543143571;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq33594280 = -854315808;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq80979346 = -724646619;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq88481823 = -760821665;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq92378222 = -716061065;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq34945622 = -334876412;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq42914252 = -30804997;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq17911952 = -722379697;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq34936396 = -918508220;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq69011636 = -663734702;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq61533865 = -246100391;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq37921085 = -313320835;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq24233872 = -647654134;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq81354478 = -26695360;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq69757516 = 70195334;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq50651068 = -571382357;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq78418340 = -340087650;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq76100645 = -727547528;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq19264706 = -298326179;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq25724116 = -738054135;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq17854926 = -158661182;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq94913678 = 46567190;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq16711682 = -211848040;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq27526458 = -286947931;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq612587 = -807293633;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq85400800 = -119192774;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq41695831 = -4607569;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq19931272 = -475603220;    int tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq53795979 = -878266794;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq54340047 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq51912892;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq51912892 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq32199502;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq32199502 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq52374154;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq52374154 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq93737268;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq93737268 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq96735259;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq96735259 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq94696956;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq94696956 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq74744916;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq74744916 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq85428323;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq85428323 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq64610312;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq64610312 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq58745181;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq58745181 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq84202625;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq84202625 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq8788796;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq8788796 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq17977948;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq17977948 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq27494943;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq27494943 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq2046301;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq2046301 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq88213759;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq88213759 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq81572060;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq81572060 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq76144682;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq76144682 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq28241083;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq28241083 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq77726757;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq77726757 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq45407211;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq45407211 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq89543364;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq89543364 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq36807411;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq36807411 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq75406835;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq75406835 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq62471168;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq62471168 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq29096382;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq29096382 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq55550282;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq55550282 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq74797181;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq74797181 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq16662565;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq16662565 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq37912472;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq37912472 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq93645700;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq93645700 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq54798842;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq54798842 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq36181684;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq36181684 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq28197541;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq28197541 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq88093643;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq88093643 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq13247017;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq13247017 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq71880996;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq71880996 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq47977781;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq47977781 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq95165336;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq95165336 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq39759260;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq39759260 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq85348535;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq85348535 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq10461590;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq10461590 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq46629113;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq46629113 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq18895460;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq18895460 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq40470439;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq40470439 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq93459532;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq93459532 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq67562516;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq67562516 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq17629198;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq17629198 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq50563310;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq50563310 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq35308087;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq35308087 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq68154956;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq68154956 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq42994632;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq42994632 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq76380501;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq76380501 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq60081039;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq60081039 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq8932836;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq8932836 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq62369527;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq62369527 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq95392091;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq95392091 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq76967318;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq76967318 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq31266100;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq31266100 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq67638878;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq67638878 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq39146674;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq39146674 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq99947734;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq99947734 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq68765759;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq68765759 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq26697841;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq26697841 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq65099480;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq65099480 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq29403784;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq29403784 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq72607111;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq72607111 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq89780406;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq89780406 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq39401299;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq39401299 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq88799284;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq88799284 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq16332763;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq16332763 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq33594280;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq33594280 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq80979346;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq80979346 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq88481823;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq88481823 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq92378222;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq92378222 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq34945622;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq34945622 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq42914252;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq42914252 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq17911952;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq17911952 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq34936396;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq34936396 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq69011636;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq69011636 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq61533865;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq61533865 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq37921085;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq37921085 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq24233872;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq24233872 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq81354478;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq81354478 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq69757516;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq69757516 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq50651068;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq50651068 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq78418340;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq78418340 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq76100645;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq76100645 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq19264706;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq19264706 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq25724116;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq25724116 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq17854926;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq17854926 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq94913678;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq94913678 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq16711682;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq16711682 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq27526458;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq27526458 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq612587;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq612587 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq85400800;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq85400800 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq41695831;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq41695831 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq19931272;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq19931272 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq53795979;     tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq53795979 = tFqqouiixeHEGhIZOvYCmzBwEkzGswjuIJASYUXSDfhBQgFnXrxNLaDtEtlDvcpAvNvwgq54340047;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void VJQvTWiLVwrzDuZzCdmSBapOPUxsjMaoYypQhzonWDXMtbvdaUh2068204() {     float UTsyuUCgyYpcJWDaFFJRCpzwyDcV81698299 = -523505737;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV80305070 = -303365616;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV64209978 = -592832469;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV89892189 = -834754646;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV6827420 = -75578732;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV8319558 = -466827642;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV42439692 = -874787646;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV95631333 = -510914114;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV52805378 = -822657177;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV28241623 = -223957137;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV73947661 = -177149421;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV28215678 = -524711992;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV73599903 = -549105998;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV77050357 = -905225893;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV89947955 = -38651329;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV31648602 = -590899069;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV58756428 = -546469672;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV5200736 = -767048347;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV7038016 = -900369869;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV72678657 = -790476136;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV78878974 = -545390948;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV42657638 = -708805565;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV32979633 = -318315088;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV77980512 = -405138455;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV52534296 = -839716469;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV69476749 = -79259033;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV52093901 = -713073153;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV22414421 = -143079864;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV61526819 = -689211442;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV7979853 = -971620219;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV71726444 = -334441726;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV33307426 = -662959828;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV47741107 = -650525408;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV17408889 = -415482801;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV81006348 = -60331547;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV61351081 = -47125684;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV67087171 = -97786246;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV35015031 = -298497698;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV82324787 = -682223265;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV17505083 = -39721709;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV95061974 = -713069256;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV26239095 = -396652345;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV78421574 = -583316979;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV28862197 = -739626617;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV32741411 = -261228030;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV23792231 = -221005922;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV19521413 = -561459236;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV33059239 = -20545591;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV43619179 = -366633380;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV48599727 = -735027235;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV72211611 = -628126599;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV34258890 = -219471263;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV33995286 = -730073602;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV93738757 = -413706661;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV23189230 = -441496132;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV39040662 = -814700172;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV47325437 = -985050529;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV86229465 = -87694014;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV37357893 = -995038177;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV37350670 = -996319699;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV56225656 = -753754489;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV20025272 = -631707782;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV34104514 = -821702673;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV44825526 = -851036959;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV56515178 = -889515412;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV40640236 = -514189593;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV80474571 = -874186584;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV56191014 = -33623198;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV96044008 = -744894346;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV28596875 = -991525645;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV64561430 = -393112824;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV23741397 = -147971975;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV22875948 = 15174917;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV89532932 = -760648161;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV77616683 = 22593119;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV52639879 = -48738604;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV64236064 = -25488587;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV4117436 = -578688471;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV45239102 = -43910426;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV28742065 = -518710547;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV49955336 = -517799797;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV19034663 = -592527563;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV78795241 = -776446484;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV12927092 = -954184208;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV35768241 = -243493620;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV37467555 = -14970463;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV99312140 = -932886226;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV54002349 = -136818748;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV94219658 = -973986669;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV41965686 = -245631376;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV14025645 = -62075156;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV80857706 = 89907768;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV97657137 = -303459521;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV44974117 = -685903566;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV61279427 = -285967221;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV75036702 = 18638526;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV92134581 = -574949672;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV33596048 = -732280021;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV72347018 = -850111206;    float UTsyuUCgyYpcJWDaFFJRCpzwyDcV92101174 = -523505737;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV81698299 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV80305070;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV80305070 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV64209978;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV64209978 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV89892189;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV89892189 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV6827420;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV6827420 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV8319558;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV8319558 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV42439692;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV42439692 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV95631333;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV95631333 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV52805378;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV52805378 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV28241623;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV28241623 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV73947661;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV73947661 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV28215678;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV28215678 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV73599903;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV73599903 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV77050357;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV77050357 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV89947955;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV89947955 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV31648602;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV31648602 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV58756428;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV58756428 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV5200736;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV5200736 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV7038016;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV7038016 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV72678657;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV72678657 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV78878974;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV78878974 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV42657638;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV42657638 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV32979633;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV32979633 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV77980512;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV77980512 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV52534296;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV52534296 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV69476749;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV69476749 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV52093901;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV52093901 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV22414421;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV22414421 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV61526819;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV61526819 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV7979853;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV7979853 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV71726444;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV71726444 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV33307426;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV33307426 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV47741107;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV47741107 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV17408889;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV17408889 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV81006348;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV81006348 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV61351081;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV61351081 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV67087171;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV67087171 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV35015031;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV35015031 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV82324787;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV82324787 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV17505083;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV17505083 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV95061974;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV95061974 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV26239095;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV26239095 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV78421574;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV78421574 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV28862197;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV28862197 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV32741411;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV32741411 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV23792231;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV23792231 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV19521413;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV19521413 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV33059239;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV33059239 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV43619179;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV43619179 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV48599727;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV48599727 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV72211611;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV72211611 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV34258890;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV34258890 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV33995286;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV33995286 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV93738757;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV93738757 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV23189230;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV23189230 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV39040662;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV39040662 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV47325437;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV47325437 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV86229465;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV86229465 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV37357893;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV37357893 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV37350670;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV37350670 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV56225656;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV56225656 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV20025272;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV20025272 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV34104514;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV34104514 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV44825526;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV44825526 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV56515178;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV56515178 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV40640236;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV40640236 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV80474571;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV80474571 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV56191014;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV56191014 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV96044008;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV96044008 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV28596875;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV28596875 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV64561430;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV64561430 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV23741397;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV23741397 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV22875948;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV22875948 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV89532932;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV89532932 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV77616683;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV77616683 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV52639879;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV52639879 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV64236064;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV64236064 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV4117436;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV4117436 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV45239102;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV45239102 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV28742065;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV28742065 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV49955336;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV49955336 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV19034663;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV19034663 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV78795241;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV78795241 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV12927092;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV12927092 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV35768241;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV35768241 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV37467555;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV37467555 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV99312140;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV99312140 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV54002349;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV54002349 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV94219658;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV94219658 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV41965686;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV41965686 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV14025645;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV14025645 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV80857706;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV80857706 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV97657137;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV97657137 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV44974117;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV44974117 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV61279427;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV61279427 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV75036702;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV75036702 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV92134581;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV92134581 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV33596048;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV33596048 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV72347018;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV72347018 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV92101174;     UTsyuUCgyYpcJWDaFFJRCpzwyDcV92101174 = UTsyuUCgyYpcJWDaFFJRCpzwyDcV81698299;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void qsOpebXumcBGnzjkEBAvwKWWd40713230() {     long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe52087248 = -536952950;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe34933662 = -809538121;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe81600166 = -294627215;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe63698027 = -677238130;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe50592275 = -240469250;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe68293525 = -428864388;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe11401233 = -306176331;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe85511552 = -601631753;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe16635823 = -406800769;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe2402370 = -607490526;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe94341919 = -699024612;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe91542847 = 50710941;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe75062721 = -130821003;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe4889828 = -382970208;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe78321401 = -592720531;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe6241141 = -891598971;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe42791225 = -617403988;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe58849987 = -929197603;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe61456098 = -627031722;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe94511426 = -17327164;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe35636833 = -337963851;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe87989379 = 23007522;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe27751030 = -640822337;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe84065573 = -43494040;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe39879755 = -935713084;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe41100502 = -916342089;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe93218603 = -90520144;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe47226641 = 23344743;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe53139563 = 62392120;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe17738753 = -882782197;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe98669066 = -609251473;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe55926625 = -71813073;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe61482933 = 92401037;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe23743302 = 5249998;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe41016305 = -718075854;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe89829884 = -393331848;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe48085234 = -987949685;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe41964755 = -999437377;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe37655210 = -374241270;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe19974959 = -856236119;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe24014927 = -435430163;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe66243114 = -537836156;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe51252910 = -85547698;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe16087574 = -87173528;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe65547252 = -454872593;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe72155100 = -41591169;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe68634135 = -213121947;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe27998322 = -317418481;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe17833506 = 2518337;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe27204156 = -614500615;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe52747889 = -724352320;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe84269656 = -698220023;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe77696435 = -797054041;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe24423703 = -746775864;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe5076398 = -263584619;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe64097868 = -459960472;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe7182632 = -68715784;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe97534593 = -151133176;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe23818272 = -741525047;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe9491773 = -324127161;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe75074921 = -238344245;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe64174591 = -229521075;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe32371989 = -564023874;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe98897070 = -524018573;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe3733304 = -998239053;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe38415295 = -527211539;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe30059914 = 58309903;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe51319419 = -36071001;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe63873523 = -664894355;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe88491517 = -99388684;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe58155906 = -903649287;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe826470 = -617966612;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe21194778 = -454956333;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe41481140 = -770795603;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe70496499 = -581897002;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe69393718 = -800127695;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe36736470 = -891444781;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe11663456 = -453648809;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe18518321 = -588621447;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe67724654 = -794121915;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe72466366 = -603220142;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe65220282 = -773101663;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe29393136 = -979173595;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe25935408 = -323107265;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe64990863 = -58429877;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe14399410 = -911031451;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe78230190 = -274759032;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe37059230 = -160823099;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe18666905 = -731165384;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe76918436 = -158115383;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe82647252 = -224616064;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe50550641 = -736816509;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe18146483 = -157912331;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe28163437 = 49885891;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe44900038 = -517891875;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe59840336 = -105909089;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe33871125 = -973812283;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe52355840 = -561529125;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe12354271 = -88934475;    long EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe27131958 = -536952950;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe52087248 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe34933662;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe34933662 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe81600166;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe81600166 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe63698027;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe63698027 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe50592275;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe50592275 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe68293525;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe68293525 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe11401233;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe11401233 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe85511552;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe85511552 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe16635823;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe16635823 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe2402370;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe2402370 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe94341919;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe94341919 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe91542847;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe91542847 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe75062721;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe75062721 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe4889828;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe4889828 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe78321401;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe78321401 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe6241141;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe6241141 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe42791225;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe42791225 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe58849987;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe58849987 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe61456098;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe61456098 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe94511426;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe94511426 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe35636833;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe35636833 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe87989379;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe87989379 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe27751030;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe27751030 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe84065573;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe84065573 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe39879755;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe39879755 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe41100502;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe41100502 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe93218603;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe93218603 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe47226641;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe47226641 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe53139563;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe53139563 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe17738753;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe17738753 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe98669066;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe98669066 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe55926625;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe55926625 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe61482933;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe61482933 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe23743302;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe23743302 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe41016305;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe41016305 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe89829884;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe89829884 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe48085234;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe48085234 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe41964755;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe41964755 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe37655210;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe37655210 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe19974959;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe19974959 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe24014927;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe24014927 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe66243114;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe66243114 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe51252910;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe51252910 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe16087574;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe16087574 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe65547252;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe65547252 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe72155100;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe72155100 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe68634135;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe68634135 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe27998322;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe27998322 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe17833506;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe17833506 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe27204156;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe27204156 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe52747889;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe52747889 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe84269656;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe84269656 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe77696435;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe77696435 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe24423703;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe24423703 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe5076398;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe5076398 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe64097868;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe64097868 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe7182632;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe7182632 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe97534593;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe97534593 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe23818272;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe23818272 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe9491773;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe9491773 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe75074921;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe75074921 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe64174591;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe64174591 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe32371989;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe32371989 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe98897070;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe98897070 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe3733304;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe3733304 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe38415295;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe38415295 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe30059914;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe30059914 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe51319419;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe51319419 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe63873523;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe63873523 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe88491517;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe88491517 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe58155906;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe58155906 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe826470;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe826470 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe21194778;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe21194778 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe41481140;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe41481140 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe70496499;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe70496499 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe69393718;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe69393718 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe36736470;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe36736470 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe11663456;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe11663456 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe18518321;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe18518321 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe67724654;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe67724654 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe72466366;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe72466366 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe65220282;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe65220282 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe29393136;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe29393136 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe25935408;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe25935408 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe64990863;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe64990863 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe14399410;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe14399410 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe78230190;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe78230190 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe37059230;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe37059230 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe18666905;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe18666905 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe76918436;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe76918436 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe82647252;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe82647252 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe50550641;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe50550641 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe18146483;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe18146483 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe28163437;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe28163437 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe44900038;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe44900038 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe59840336;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe59840336 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe33871125;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe33871125 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe52355840;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe52355840 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe12354271;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe12354271 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe27131958;     EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe27131958 = EhgoWViczrELGNhXlyRzRAtNmBaGjKgWsLZtqGPGTenHe52087248;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void vNxfwtYyEHIpEQNVPFVVWMKbDwdmAfeUNNYJe32035063() {     double WJJrxIpnUYNbmIJiWOjDV59396256 = -934182403;    double WJJrxIpnUYNbmIJiWOjDV33853071 = -336945980;    double WJJrxIpnUYNbmIJiWOjDV67014672 = -241819497;    double WJJrxIpnUYNbmIJiWOjDV61637023 = -755654113;    double WJJrxIpnUYNbmIJiWOjDV47296243 = -926262075;    double WJJrxIpnUYNbmIJiWOjDV50434528 = -713452632;    double WJJrxIpnUYNbmIJiWOjDV72545275 = -170497173;    double WJJrxIpnUYNbmIJiWOjDV25411633 = -598933542;    double WJJrxIpnUYNbmIJiWOjDV29536998 = -403866581;    double WJJrxIpnUYNbmIJiWOjDV49223382 = -340086839;    double WJJrxIpnUYNbmIJiWOjDV39497060 = -814856565;    double WJJrxIpnUYNbmIJiWOjDV28069413 = 20161399;    double WJJrxIpnUYNbmIJiWOjDV95115216 = -631313915;    double WJJrxIpnUYNbmIJiWOjDV64508462 = -293501186;    double WJJrxIpnUYNbmIJiWOjDV51700744 = -224233496;    double WJJrxIpnUYNbmIJiWOjDV9753975 = -429149371;    double WJJrxIpnUYNbmIJiWOjDV90629386 = -763161784;    double WJJrxIpnUYNbmIJiWOjDV22466130 = -591001008;    double WJJrxIpnUYNbmIJiWOjDV57140819 = -394710111;    double WJJrxIpnUYNbmIJiWOjDV41115465 = -385282144;    double WJJrxIpnUYNbmIJiWOjDV79777715 = -652832340;    double WJJrxIpnUYNbmIJiWOjDV16585148 = -25799264;    double WJJrxIpnUYNbmIJiWOjDV1388366 = -700475297;    double WJJrxIpnUYNbmIJiWOjDV27420112 = -136168530;    double WJJrxIpnUYNbmIJiWOjDV65300871 = -337650942;    double WJJrxIpnUYNbmIJiWOjDV36046674 = -743999972;    double WJJrxIpnUYNbmIJiWOjDV83789130 = -895269127;    double WJJrxIpnUYNbmIJiWOjDV81476293 = -740121665;    double WJJrxIpnUYNbmIJiWOjDV70824712 = -588754528;    double WJJrxIpnUYNbmIJiWOjDV1233759 = -274022088;    double WJJrxIpnUYNbmIJiWOjDV93085110 = -347263429;    double WJJrxIpnUYNbmIJiWOjDV79168818 = -299543461;    double WJJrxIpnUYNbmIJiWOjDV860947 = -623850305;    double WJJrxIpnUYNbmIJiWOjDV60071523 = -504302406;    double WJJrxIpnUYNbmIJiWOjDV64394719 = -712618739;    double WJJrxIpnUYNbmIJiWOjDV61975949 = -115075265;    double WJJrxIpnUYNbmIJiWOjDV63037903 = -495931757;    double WJJrxIpnUYNbmIJiWOjDV88722275 = -479425296;    double WJJrxIpnUYNbmIJiWOjDV5045198 = -882280801;    double WJJrxIpnUYNbmIJiWOjDV43200133 = -480501621;    double WJJrxIpnUYNbmIJiWOjDV33093228 = -726699695;    double WJJrxIpnUYNbmIJiWOjDV87512473 = 15376530;    double WJJrxIpnUYNbmIJiWOjDV90605260 = -71041032;    double WJJrxIpnUYNbmIJiWOjDV61665605 = -473957233;    double WJJrxIpnUYNbmIJiWOjDV21342468 = -947723725;    double WJJrxIpnUYNbmIJiWOjDV33191884 = -595537718;    double WJJrxIpnUYNbmIJiWOjDV95321332 = -10809327;    double WJJrxIpnUYNbmIJiWOjDV69039244 = -690251464;    double WJJrxIpnUYNbmIJiWOjDV11938058 = -744014589;    double WJJrxIpnUYNbmIJiWOjDV53419519 = -728748546;    double WJJrxIpnUYNbmIJiWOjDV27956872 = -774571520;    double WJJrxIpnUYNbmIJiWOjDV92830280 = 53182097;    double WJJrxIpnUYNbmIJiWOjDV50401274 = -919967668;    double WJJrxIpnUYNbmIJiWOjDV62216184 = -453917190;    double WJJrxIpnUYNbmIJiWOjDV52257348 = -676011172;    double WJJrxIpnUYNbmIJiWOjDV42811109 = -808383139;    double WJJrxIpnUYNbmIJiWOjDV32464706 = -636470684;    double WJJrxIpnUYNbmIJiWOjDV39594561 = -5650968;    double WJJrxIpnUYNbmIJiWOjDV96336152 = -318003172;    double WJJrxIpnUYNbmIJiWOjDV11249570 = -82262104;    double WJJrxIpnUYNbmIJiWOjDV66645398 = -818183505;    double WJJrxIpnUYNbmIJiWOjDV91068982 = -430375509;    double WJJrxIpnUYNbmIJiWOjDV54586920 = 89820986;    double WJJrxIpnUYNbmIJiWOjDV28303240 = -29844494;    double WJJrxIpnUYNbmIJiWOjDV56138272 = -992823411;    double WJJrxIpnUYNbmIJiWOjDV60328241 = -415313104;    double WJJrxIpnUYNbmIJiWOjDV27208466 = -355988297;    double WJJrxIpnUYNbmIJiWOjDV35043694 = -27011510;    double WJJrxIpnUYNbmIJiWOjDV113744 = -580882448;    double WJJrxIpnUYNbmIJiWOjDV89724794 = -9158232;    double WJJrxIpnUYNbmIJiWOjDV46716072 = -933217615;    double WJJrxIpnUYNbmIJiWOjDV1907112 = -183736489;    double WJJrxIpnUYNbmIJiWOjDV17420932 = -708720207;    double WJJrxIpnUYNbmIJiWOjDV13940687 = -914208491;    double WJJrxIpnUYNbmIJiWOjDV8022238 = -658582449;    double WJJrxIpnUYNbmIJiWOjDV92265242 = -568208870;    double WJJrxIpnUYNbmIJiWOjDV25979888 = -954758233;    double WJJrxIpnUYNbmIJiWOjDV39722760 = -126518065;    double WJJrxIpnUYNbmIJiWOjDV6077644 = -188444806;    double WJJrxIpnUYNbmIJiWOjDV32108987 = -742113224;    double WJJrxIpnUYNbmIJiWOjDV40725342 = -633190645;    double WJJrxIpnUYNbmIJiWOjDV14749886 = -105017663;    double WJJrxIpnUYNbmIJiWOjDV69538236 = -996107076;    double WJJrxIpnUYNbmIJiWOjDV17405193 = -860005983;    double WJJrxIpnUYNbmIJiWOjDV73276886 = -499450569;    double WJJrxIpnUYNbmIJiWOjDV254831 = -300445527;    double WJJrxIpnUYNbmIJiWOjDV28767545 = -379575794;    double WJJrxIpnUYNbmIJiWOjDV38644763 = -69933115;    double WJJrxIpnUYNbmIJiWOjDV7814176 = -828291235;    double WJJrxIpnUYNbmIJiWOjDV21583610 = -904235601;    double WJJrxIpnUYNbmIJiWOjDV29511244 = -478604581;    double WJJrxIpnUYNbmIJiWOjDV23443342 = -390280790;    double WJJrxIpnUYNbmIJiWOjDV92386122 = -61422124;    double WJJrxIpnUYNbmIJiWOjDV93795628 = -700018698;    double WJJrxIpnUYNbmIJiWOjDV76554735 = -662318116;    double WJJrxIpnUYNbmIJiWOjDV42024245 = -196324187;    double WJJrxIpnUYNbmIJiWOjDV32925554 = 25555544;    double WJJrxIpnUYNbmIJiWOjDV62302020 = 58803461;    double WJJrxIpnUYNbmIJiWOjDV5527334 = -481133822;    double WJJrxIpnUYNbmIJiWOjDV61014226 = -934182403;     WJJrxIpnUYNbmIJiWOjDV59396256 = WJJrxIpnUYNbmIJiWOjDV33853071;     WJJrxIpnUYNbmIJiWOjDV33853071 = WJJrxIpnUYNbmIJiWOjDV67014672;     WJJrxIpnUYNbmIJiWOjDV67014672 = WJJrxIpnUYNbmIJiWOjDV61637023;     WJJrxIpnUYNbmIJiWOjDV61637023 = WJJrxIpnUYNbmIJiWOjDV47296243;     WJJrxIpnUYNbmIJiWOjDV47296243 = WJJrxIpnUYNbmIJiWOjDV50434528;     WJJrxIpnUYNbmIJiWOjDV50434528 = WJJrxIpnUYNbmIJiWOjDV72545275;     WJJrxIpnUYNbmIJiWOjDV72545275 = WJJrxIpnUYNbmIJiWOjDV25411633;     WJJrxIpnUYNbmIJiWOjDV25411633 = WJJrxIpnUYNbmIJiWOjDV29536998;     WJJrxIpnUYNbmIJiWOjDV29536998 = WJJrxIpnUYNbmIJiWOjDV49223382;     WJJrxIpnUYNbmIJiWOjDV49223382 = WJJrxIpnUYNbmIJiWOjDV39497060;     WJJrxIpnUYNbmIJiWOjDV39497060 = WJJrxIpnUYNbmIJiWOjDV28069413;     WJJrxIpnUYNbmIJiWOjDV28069413 = WJJrxIpnUYNbmIJiWOjDV95115216;     WJJrxIpnUYNbmIJiWOjDV95115216 = WJJrxIpnUYNbmIJiWOjDV64508462;     WJJrxIpnUYNbmIJiWOjDV64508462 = WJJrxIpnUYNbmIJiWOjDV51700744;     WJJrxIpnUYNbmIJiWOjDV51700744 = WJJrxIpnUYNbmIJiWOjDV9753975;     WJJrxIpnUYNbmIJiWOjDV9753975 = WJJrxIpnUYNbmIJiWOjDV90629386;     WJJrxIpnUYNbmIJiWOjDV90629386 = WJJrxIpnUYNbmIJiWOjDV22466130;     WJJrxIpnUYNbmIJiWOjDV22466130 = WJJrxIpnUYNbmIJiWOjDV57140819;     WJJrxIpnUYNbmIJiWOjDV57140819 = WJJrxIpnUYNbmIJiWOjDV41115465;     WJJrxIpnUYNbmIJiWOjDV41115465 = WJJrxIpnUYNbmIJiWOjDV79777715;     WJJrxIpnUYNbmIJiWOjDV79777715 = WJJrxIpnUYNbmIJiWOjDV16585148;     WJJrxIpnUYNbmIJiWOjDV16585148 = WJJrxIpnUYNbmIJiWOjDV1388366;     WJJrxIpnUYNbmIJiWOjDV1388366 = WJJrxIpnUYNbmIJiWOjDV27420112;     WJJrxIpnUYNbmIJiWOjDV27420112 = WJJrxIpnUYNbmIJiWOjDV65300871;     WJJrxIpnUYNbmIJiWOjDV65300871 = WJJrxIpnUYNbmIJiWOjDV36046674;     WJJrxIpnUYNbmIJiWOjDV36046674 = WJJrxIpnUYNbmIJiWOjDV83789130;     WJJrxIpnUYNbmIJiWOjDV83789130 = WJJrxIpnUYNbmIJiWOjDV81476293;     WJJrxIpnUYNbmIJiWOjDV81476293 = WJJrxIpnUYNbmIJiWOjDV70824712;     WJJrxIpnUYNbmIJiWOjDV70824712 = WJJrxIpnUYNbmIJiWOjDV1233759;     WJJrxIpnUYNbmIJiWOjDV1233759 = WJJrxIpnUYNbmIJiWOjDV93085110;     WJJrxIpnUYNbmIJiWOjDV93085110 = WJJrxIpnUYNbmIJiWOjDV79168818;     WJJrxIpnUYNbmIJiWOjDV79168818 = WJJrxIpnUYNbmIJiWOjDV860947;     WJJrxIpnUYNbmIJiWOjDV860947 = WJJrxIpnUYNbmIJiWOjDV60071523;     WJJrxIpnUYNbmIJiWOjDV60071523 = WJJrxIpnUYNbmIJiWOjDV64394719;     WJJrxIpnUYNbmIJiWOjDV64394719 = WJJrxIpnUYNbmIJiWOjDV61975949;     WJJrxIpnUYNbmIJiWOjDV61975949 = WJJrxIpnUYNbmIJiWOjDV63037903;     WJJrxIpnUYNbmIJiWOjDV63037903 = WJJrxIpnUYNbmIJiWOjDV88722275;     WJJrxIpnUYNbmIJiWOjDV88722275 = WJJrxIpnUYNbmIJiWOjDV5045198;     WJJrxIpnUYNbmIJiWOjDV5045198 = WJJrxIpnUYNbmIJiWOjDV43200133;     WJJrxIpnUYNbmIJiWOjDV43200133 = WJJrxIpnUYNbmIJiWOjDV33093228;     WJJrxIpnUYNbmIJiWOjDV33093228 = WJJrxIpnUYNbmIJiWOjDV87512473;     WJJrxIpnUYNbmIJiWOjDV87512473 = WJJrxIpnUYNbmIJiWOjDV90605260;     WJJrxIpnUYNbmIJiWOjDV90605260 = WJJrxIpnUYNbmIJiWOjDV61665605;     WJJrxIpnUYNbmIJiWOjDV61665605 = WJJrxIpnUYNbmIJiWOjDV21342468;     WJJrxIpnUYNbmIJiWOjDV21342468 = WJJrxIpnUYNbmIJiWOjDV33191884;     WJJrxIpnUYNbmIJiWOjDV33191884 = WJJrxIpnUYNbmIJiWOjDV95321332;     WJJrxIpnUYNbmIJiWOjDV95321332 = WJJrxIpnUYNbmIJiWOjDV69039244;     WJJrxIpnUYNbmIJiWOjDV69039244 = WJJrxIpnUYNbmIJiWOjDV11938058;     WJJrxIpnUYNbmIJiWOjDV11938058 = WJJrxIpnUYNbmIJiWOjDV53419519;     WJJrxIpnUYNbmIJiWOjDV53419519 = WJJrxIpnUYNbmIJiWOjDV27956872;     WJJrxIpnUYNbmIJiWOjDV27956872 = WJJrxIpnUYNbmIJiWOjDV92830280;     WJJrxIpnUYNbmIJiWOjDV92830280 = WJJrxIpnUYNbmIJiWOjDV50401274;     WJJrxIpnUYNbmIJiWOjDV50401274 = WJJrxIpnUYNbmIJiWOjDV62216184;     WJJrxIpnUYNbmIJiWOjDV62216184 = WJJrxIpnUYNbmIJiWOjDV52257348;     WJJrxIpnUYNbmIJiWOjDV52257348 = WJJrxIpnUYNbmIJiWOjDV42811109;     WJJrxIpnUYNbmIJiWOjDV42811109 = WJJrxIpnUYNbmIJiWOjDV32464706;     WJJrxIpnUYNbmIJiWOjDV32464706 = WJJrxIpnUYNbmIJiWOjDV39594561;     WJJrxIpnUYNbmIJiWOjDV39594561 = WJJrxIpnUYNbmIJiWOjDV96336152;     WJJrxIpnUYNbmIJiWOjDV96336152 = WJJrxIpnUYNbmIJiWOjDV11249570;     WJJrxIpnUYNbmIJiWOjDV11249570 = WJJrxIpnUYNbmIJiWOjDV66645398;     WJJrxIpnUYNbmIJiWOjDV66645398 = WJJrxIpnUYNbmIJiWOjDV91068982;     WJJrxIpnUYNbmIJiWOjDV91068982 = WJJrxIpnUYNbmIJiWOjDV54586920;     WJJrxIpnUYNbmIJiWOjDV54586920 = WJJrxIpnUYNbmIJiWOjDV28303240;     WJJrxIpnUYNbmIJiWOjDV28303240 = WJJrxIpnUYNbmIJiWOjDV56138272;     WJJrxIpnUYNbmIJiWOjDV56138272 = WJJrxIpnUYNbmIJiWOjDV60328241;     WJJrxIpnUYNbmIJiWOjDV60328241 = WJJrxIpnUYNbmIJiWOjDV27208466;     WJJrxIpnUYNbmIJiWOjDV27208466 = WJJrxIpnUYNbmIJiWOjDV35043694;     WJJrxIpnUYNbmIJiWOjDV35043694 = WJJrxIpnUYNbmIJiWOjDV113744;     WJJrxIpnUYNbmIJiWOjDV113744 = WJJrxIpnUYNbmIJiWOjDV89724794;     WJJrxIpnUYNbmIJiWOjDV89724794 = WJJrxIpnUYNbmIJiWOjDV46716072;     WJJrxIpnUYNbmIJiWOjDV46716072 = WJJrxIpnUYNbmIJiWOjDV1907112;     WJJrxIpnUYNbmIJiWOjDV1907112 = WJJrxIpnUYNbmIJiWOjDV17420932;     WJJrxIpnUYNbmIJiWOjDV17420932 = WJJrxIpnUYNbmIJiWOjDV13940687;     WJJrxIpnUYNbmIJiWOjDV13940687 = WJJrxIpnUYNbmIJiWOjDV8022238;     WJJrxIpnUYNbmIJiWOjDV8022238 = WJJrxIpnUYNbmIJiWOjDV92265242;     WJJrxIpnUYNbmIJiWOjDV92265242 = WJJrxIpnUYNbmIJiWOjDV25979888;     WJJrxIpnUYNbmIJiWOjDV25979888 = WJJrxIpnUYNbmIJiWOjDV39722760;     WJJrxIpnUYNbmIJiWOjDV39722760 = WJJrxIpnUYNbmIJiWOjDV6077644;     WJJrxIpnUYNbmIJiWOjDV6077644 = WJJrxIpnUYNbmIJiWOjDV32108987;     WJJrxIpnUYNbmIJiWOjDV32108987 = WJJrxIpnUYNbmIJiWOjDV40725342;     WJJrxIpnUYNbmIJiWOjDV40725342 = WJJrxIpnUYNbmIJiWOjDV14749886;     WJJrxIpnUYNbmIJiWOjDV14749886 = WJJrxIpnUYNbmIJiWOjDV69538236;     WJJrxIpnUYNbmIJiWOjDV69538236 = WJJrxIpnUYNbmIJiWOjDV17405193;     WJJrxIpnUYNbmIJiWOjDV17405193 = WJJrxIpnUYNbmIJiWOjDV73276886;     WJJrxIpnUYNbmIJiWOjDV73276886 = WJJrxIpnUYNbmIJiWOjDV254831;     WJJrxIpnUYNbmIJiWOjDV254831 = WJJrxIpnUYNbmIJiWOjDV28767545;     WJJrxIpnUYNbmIJiWOjDV28767545 = WJJrxIpnUYNbmIJiWOjDV38644763;     WJJrxIpnUYNbmIJiWOjDV38644763 = WJJrxIpnUYNbmIJiWOjDV7814176;     WJJrxIpnUYNbmIJiWOjDV7814176 = WJJrxIpnUYNbmIJiWOjDV21583610;     WJJrxIpnUYNbmIJiWOjDV21583610 = WJJrxIpnUYNbmIJiWOjDV29511244;     WJJrxIpnUYNbmIJiWOjDV29511244 = WJJrxIpnUYNbmIJiWOjDV23443342;     WJJrxIpnUYNbmIJiWOjDV23443342 = WJJrxIpnUYNbmIJiWOjDV92386122;     WJJrxIpnUYNbmIJiWOjDV92386122 = WJJrxIpnUYNbmIJiWOjDV93795628;     WJJrxIpnUYNbmIJiWOjDV93795628 = WJJrxIpnUYNbmIJiWOjDV76554735;     WJJrxIpnUYNbmIJiWOjDV76554735 = WJJrxIpnUYNbmIJiWOjDV42024245;     WJJrxIpnUYNbmIJiWOjDV42024245 = WJJrxIpnUYNbmIJiWOjDV32925554;     WJJrxIpnUYNbmIJiWOjDV32925554 = WJJrxIpnUYNbmIJiWOjDV62302020;     WJJrxIpnUYNbmIJiWOjDV62302020 = WJJrxIpnUYNbmIJiWOjDV5527334;     WJJrxIpnUYNbmIJiWOjDV5527334 = WJJrxIpnUYNbmIJiWOjDV61014226;     WJJrxIpnUYNbmIJiWOjDV61014226 = WJJrxIpnUYNbmIJiWOjDV59396256;}
// Junk Finished
