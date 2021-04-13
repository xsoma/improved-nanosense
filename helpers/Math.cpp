#include "Math.hpp"

namespace Math
{
	void NormalizeAngles(QAngle& angles)
	{
		for (auto i = 0; i < 3; i++) {
			while (angles[i] < -180.0f) angles[i] += 360.0f;
			while (angles[i] > 180.0f) angles[i] -= 360.0f;
		}
	}

	void NormalizeVector(Vector& vec)
	{
		for (auto i = 0; i < 3; i++) {
			while (vec[i] < -180.0f) vec[i] += 360.0f;
			while (vec[i] > 180.0f) vec[i] -= 360.0f;
		}
		vec[2] = 0.f;
	}

	//--------------------------------------------------------------------------------
	float Clamp(float in, float min, float max)
	{
		if (in < min)
			return min;
		else if (in > max)
			return max;
		else
			return in;
	}
	void ClampAngles(QAngle& angles)
	{
		if (angles.pitch > 89.0f) angles.pitch = 89.0f;
		else if (angles.pitch < -89.0f) angles.pitch = -89.0f;

		if (angles.yaw > 180.0f) angles.yaw = 180.0f;
		else if (angles.yaw < -180.0f) angles.yaw = -180.0f;

		angles.roll = 0;
	}
	//--------------------------------------------------------------------------------
	void VectorTransform(const Vector& in1, const matrix3x4_t& in2, Vector& out)
	{
		out[0] = in1.Dot(in2[0]) + in2[0][3];
		out[1] = in1.Dot(in2[1]) + in2[1][3];
		out[2] = in1.Dot(in2[2]) + in2[2][3];
	}
	//--------------------------------------------------------------------------------
	void AngleVectors(const QAngle& angles, Vector& forward)
	{
		float	sp, sy, cp, cy;

		SinCos(DEG2RAD(angles[1]), &sy, &cy);
		SinCos(DEG2RAD(angles[0]), &sp, &cp);

		forward.x = cp * cy;
		forward.y = cp * sy;
		forward.z = -sp;
	}
	//--------------------------------------------------------------------------------
	void AngleVectors(const QAngle& angles, Vector& forward, Vector& right, Vector& up)
	{
		float sr, sp, sy, cr, cp, cy;

		SinCos(DEG2RAD(angles[1]), &sy, &cy);
		SinCos(DEG2RAD(angles[0]), &sp, &cp);
		SinCos(DEG2RAD(angles[2]), &sr, &cr);

		forward.x = (cp * cy);
		forward.y = (cp * sy);
		forward.z = (-sp);
		right.x = (-1 * sr * sp * cy + -1 * cr * -sy);
		right.y = (-1 * sr * sp * sy + -1 * cr * cy);
		right.z = (-1 * sr * cp);
		up.x = (cr * sp * cy + -sr * -sy);
		up.y = (cr * sp * sy + -sr * cy);
		up.z = (cr * cp);
	}
	//--------------------------------------------------------------------------------
	void VectorAngles(const Vector& forward, QAngle& angles)
	{
		if (forward[1] == 0.0f && forward[0] == 0.0f)
		{
			angles[0] = (forward[2] > 0.0f) ? 270.0f : 90.0f;
			angles[1] = 0.0f;
		}
		else
		{
			angles[0] = atan2(-forward[2], forward.Length2D()) * -180 / PI;
			angles[1] = atan2(forward[1], forward[0]) * 180 / PI;

			if (angles[1] > 90) angles[1] -= 180;
			else if (angles[1] < 90) angles[1] += 180;
			else if (angles[1] == 90) angles[1] = 0;
		}

		angles[2] = 0.0f;
	}

	Vector CrossProduct(const Vector& a, const Vector& b)
	{
		return Vector(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
	}

	void VectorAngles(const Vector& forward, Vector& up, QAngle& angles)
	{
		Vector left = CrossProduct(up, forward);
		left.NormalizeInPlace();

		float forwardDist = forward.Length2D();

		if (forwardDist > 0.001f)
		{
			angles.pitch = atan2f(-forward.z, forwardDist) * 180 / PI_F;
			angles.yaw = atan2f(forward.y, forward.x) * 180 / PI_F;

			float upZ = (left.y * forward.x) - (left.x * forward.y);
			angles.roll = atan2f(left.z, upZ) * 180 / PI_F;
		}
		else
		{
			angles.pitch = atan2f(-forward.z, forwardDist) * 180 / PI_F;
			angles.yaw = atan2f(-left.x, left.y) * 180 / PI_F;
			angles.roll = 0;
		}
	}
	//--------------------------------------------------------------------------------
	bool screen_transform(const Vector& in, Vector& out)
	{
		static auto& w2sMatrix = g_EngineClient->WorldToScreenMatrix();

		out.x = w2sMatrix.m[0][0] * in.x + w2sMatrix.m[0][1] * in.y + w2sMatrix.m[0][2] * in.z + w2sMatrix.m[0][3];
		out.y = w2sMatrix.m[1][0] * in.x + w2sMatrix.m[1][1] * in.y + w2sMatrix.m[1][2] * in.z + w2sMatrix.m[1][3];
		out.z = 0.0f;

		float w = w2sMatrix.m[3][0] * in.x + w2sMatrix.m[3][1] * in.y + w2sMatrix.m[3][2] * in.z + w2sMatrix.m[3][3];

		if (w < 0.001f) {
			out.x *= 100000;
			out.y *= 100000;
			return false;
		}

		out.x /= w;
		out.y /= w;

		return true;
	}
	//--------------------------------------------------------------------------------
	bool WorldToScreen(const Vector& in, Vector& out)
	{
		if (screen_transform(in, out)) {
			int w, h;
			g_EngineClient->GetScreenSize(w, h);

			out.x = (w / 2.0f) + (out.x * w) / 2.0f;
			out.y = (h / 2.0f) - (out.y * h) / 2.0f;

			return true;
		}
		return false;
	}
	//--------------------------------------------------------------------------------
	void SinCos(float a, float* s, float* c)
	{
		*s = sin(a);
		*c = cos(a);
	}

	float GetFov(const QAngle& viewAngle, const QAngle& aimAngle)
	{
		Vector ang, aim;

		AngleVectors(viewAngle, aim);
		AngleVectors(aimAngle, ang);

		return RAD2DEG(acos(aim.Dot(ang) / aim.LengthSqr()));
	}

	float GetDistance(Vector src, Vector dst)
	{
		return sqrtf(pow(src.x - dst.x, 2) + pow(src.y - dst.y, 2) + pow(src.z - dst.z, 2));
	}

	QAngle CalcAngle(Vector src, Vector dst)
	{
		QAngle angles;
		Vector delta = src - dst;

		VectorAngles(delta, angles);

		angles.Normalize();

		return angles;
	}

	void SmoothAngle(QAngle src, QAngle& dst, float factor)
	{
		QAngle delta = dst - src;

		NormalizeAngles(delta);

		dst = src + delta / factor;
	}

	float DistanceToRay(const Vector& pos, const Vector& rayStart, const Vector& rayEnd, float* along, Vector* pointOnRay)
	{
		Vector to = pos - rayStart;
		Vector dir = rayEnd - rayStart;
		float length = dir.Normalize();

		float rangeAlong = dir.Dot(to);
		if (along)
			* along = rangeAlong;

		float range;

		if (rangeAlong < 0.0f)
		{
			range = -(pos - rayStart).Length();

			if (pointOnRay)
				* pointOnRay = rayStart;
		}
		else if (rangeAlong > length)
		{
			range = -(pos - rayEnd).Length();

			if (pointOnRay)
				* pointOnRay = rayEnd;
		}
		else
		{
			Vector onRay = rayStart + rangeAlong * dir;
			range = (pos - onRay).Length();

			if (pointOnRay)
				* pointOnRay = onRay;
		}

		return range;
	}

	void AngleMatrix(const QAngle& angles, matrix3x4_t& matrix)
	{
		float sr, sp, sy, cr, cp, cy;

		SinCos(DEG2RAD(angles[1]), &sy, &cy);
		SinCos(DEG2RAD(angles[0]), &sp, &cp);
		SinCos(DEG2RAD(angles[2]), &sr, &cr);

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

	void AngleMatrix(const QAngle& angles, const Vector& position, matrix3x4_t& matrix)
	{
		AngleMatrix(angles, matrix);
		MatrixSetColumn(position, 3, matrix);
	}

	float __fastcall AngleDiff(float a1, float a2)
	{
		float val = fmodf(a1 - a2, 360.0);

		while (val < -180.0f) val += 360.0f;
		while (val > 180.0f) val -= 360.0f;

		return val;
	}

	float ComputeBodyPitch(float viewangle)
	{
		if (viewangle > 180.0f)
			viewangle -= 360.0f;

		return Math::Clamp(viewangle, -90.0f, 90.0f);
	}

	bool IsFakeAngle(float viewangle, float goal)
	{
		float NormalizedAngle = abs(Math::ClampPitch(viewangle));

		if (abs(NormalizedAngle - goal) < 1 && ComputeBodyPitch(viewangle) != NormalizedAngle)
			return true;

		return false;
	}

	bool IsRealAngle(float viewangle, float goal)
	{
		float ComputedAngle = ComputeBodyPitch(viewangle);

		if (abs(ComputedAngle - goal) < 0.125 && ComputedAngle != abs(Math::ClampPitch(viewangle)))
			return true;

		return false;
	}

	float FindSmallestFake(float goal, int skip)
	{
		float last_angle = goal;
		for (float Angle = 270.0f; Angle < 10000.0f * skip; Angle += 1.0f)
		{
			if (IsFakeAngle(Angle, abs(goal)))
			{
				last_angle = Angle;
				if (!skip)
				{
					if (goal < 0.0f)
						return -Angle;
					else
						return Angle;
				}
				skip--;
			}
		}
		return last_angle * (((goal > 0.0f) * 2) - 1);
	}

	float FindSmallestFake2(float goal, int skip)
	{
		float last_angle = goal;
		for (float Angle = 270.0f; Angle < 10000.0f * skip; Angle += 1.0f)
		{
			last_angle = Angle;
			if (!skip)
			{
				if (IsRealAngle(Angle, abs(goal)))
				{
					if (goal < 0.0f)
						return -Angle;
					else
						return Angle;
				}
			}
			skip--;
		}
		return last_angle * (((goal > 0.0f) * 2) - 1);
	}
}

float QAngle::RealYawDifference(QAngle dst, QAngle add)
{
	dst += add;

	dst.pitch = 0.f;

	Vector ang, aim;

	Math::AngleVectors(*this, aim);
	Math::AngleVectors(dst, ang);

	return RAD2DEG(acos(aim.Dot(ang) / aim.LengthSqr()));
}

float QAngle::Difference(QAngle dst, QAngle add)
{
	dst += add;

	Vector ang, aim;

	Math::AngleVectors(*this, aim);
	Math::AngleVectors(dst, ang);

	return RAD2DEG(acos(aim.Dot(ang) / aim.LengthSqr()));
}
















// Junk Code By Troll Face & Thaisen's Gen
void YSMMGYJmEANxodMZOqwhytTvxuVVggZFabCxiXfQ59395874() {     int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO43008500 = -533255584;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO34733059 = -883568231;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO46397755 = -272896487;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO81422993 = -550613101;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO5720110 = -712438034;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO74174785 = -773984539;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO23561630 = -366903377;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO5128155 = -939840927;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO20316804 = -809593330;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO12788674 = -872364184;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO38157550 = -922875738;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO39175046 = -755989215;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO50853438 = -632359685;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO836391 = 93179036;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO76171879 = -44946677;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO93564774 = -315788013;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO96282130 = -690207548;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO31220808 = -435234906;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO36128631 = -471442104;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO28594400 = -979629;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO51518286 = -925238469;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO74498159 = -373277240;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO96002024 = -565896351;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO79426922 = -788397099;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO28012972 = -347758956;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO10350848 = -839384807;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO76927676 = -952501357;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO92309888 = -909579151;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO50262766 = -572526716;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO8204015 = -624554357;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO75779345 = 54853436;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO37850710 = -333260112;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO7623494 = -184858469;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO59633529 = -32243203;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO25055052 = -872413219;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO27804137 = 77576782;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO72967112 = -241597310;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO72687046 = -315410194;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO59184996 = -148807786;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO97257265 = -521332684;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO12857360 = -25014381;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO55243954 = -31545357;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO12999116 = -370062353;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO79453788 = -558320465;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO45877936 = -455132439;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO73982863 = -264206665;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO1804616 = -201681235;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO41881658 = -781131757;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO89221097 = -153080244;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO46739658 = -584100259;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO44141944 = -412130655;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO29590050 = -460600878;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO44340372 = -418808303;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO33046738 = -432131310;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO7877589 = -573479485;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO68510340 = -59978345;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO38731035 = -217671880;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO66970833 = -484499388;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO53410021 = -102854145;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO95369261 = -873053227;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO97247108 = -821483183;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO31251742 = -457324227;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO54865389 = -267314211;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO12112789 = -85038974;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO37009329 = -827217620;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO306841 = -489615627;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO31551553 = -471130746;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO91219909 = -500116483;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO75781338 = -34407746;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO48367742 = -22523460;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO20597662 = 25809297;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO23595084 = -274797355;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO72035812 = -186427120;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO38871365 = -950109421;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO15737040 = -975965248;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO96274332 = -793693113;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO61499044 = 96785112;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO16548236 = 92424113;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO33548986 = -233264661;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO54030109 = 16447708;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO8546232 = -537703572;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO35046019 = -71369600;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO3088792 = -656498908;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO3523108 = -988426458;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO64062071 = -112423702;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO46189295 = -484545687;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO93510338 = -914451810;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO74576756 = -752727159;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO51755941 = -458763719;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO56544711 = -712434875;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO89073102 = -704751338;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO5996280 = -757097923;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO19277026 = -112556049;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO63815734 = -275754560;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO10158 = -699849502;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO81605618 = -567690155;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO378566 = -764231146;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO886327 = -185023379;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO42444460 = -731102845;    int ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO45571095 = -533255584;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO43008500 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO34733059;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO34733059 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO46397755;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO46397755 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO81422993;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO81422993 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO5720110;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO5720110 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO74174785;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO74174785 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO23561630;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO23561630 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO5128155;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO5128155 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO20316804;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO20316804 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO12788674;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO12788674 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO38157550;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO38157550 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO39175046;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO39175046 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO50853438;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO50853438 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO836391;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO836391 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO76171879;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO76171879 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO93564774;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO93564774 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO96282130;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO96282130 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO31220808;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO31220808 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO36128631;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO36128631 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO28594400;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO28594400 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO51518286;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO51518286 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO74498159;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO74498159 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO96002024;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO96002024 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO79426922;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO79426922 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO28012972;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO28012972 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO10350848;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO10350848 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO76927676;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO76927676 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO92309888;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO92309888 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO50262766;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO50262766 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO8204015;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO8204015 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO75779345;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO75779345 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO37850710;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO37850710 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO7623494;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO7623494 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO59633529;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO59633529 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO25055052;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO25055052 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO27804137;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO27804137 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO72967112;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO72967112 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO72687046;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO72687046 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO59184996;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO59184996 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO97257265;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO97257265 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO12857360;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO12857360 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO55243954;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO55243954 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO12999116;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO12999116 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO79453788;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO79453788 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO45877936;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO45877936 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO73982863;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO73982863 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO1804616;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO1804616 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO41881658;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO41881658 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO89221097;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO89221097 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO46739658;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO46739658 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO44141944;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO44141944 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO29590050;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO29590050 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO44340372;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO44340372 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO33046738;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO33046738 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO7877589;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO7877589 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO68510340;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO68510340 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO38731035;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO38731035 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO66970833;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO66970833 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO53410021;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO53410021 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO95369261;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO95369261 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO97247108;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO97247108 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO31251742;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO31251742 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO54865389;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO54865389 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO12112789;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO12112789 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO37009329;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO37009329 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO306841;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO306841 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO31551553;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO31551553 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO91219909;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO91219909 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO75781338;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO75781338 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO48367742;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO48367742 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO20597662;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO20597662 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO23595084;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO23595084 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO72035812;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO72035812 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO38871365;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO38871365 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO15737040;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO15737040 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO96274332;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO96274332 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO61499044;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO61499044 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO16548236;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO16548236 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO33548986;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO33548986 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO54030109;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO54030109 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO8546232;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO8546232 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO35046019;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO35046019 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO3088792;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO3088792 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO3523108;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO3523108 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO64062071;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO64062071 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO46189295;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO46189295 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO93510338;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO93510338 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO74576756;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO74576756 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO51755941;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO51755941 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO56544711;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO56544711 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO89073102;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO89073102 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO5996280;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO5996280 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO19277026;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO19277026 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO63815734;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO63815734 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO10158;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO10158 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO81605618;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO81605618 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO378566;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO378566 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO886327;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO886327 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO42444460;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO42444460 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO45571095;     ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO45571095 = ardtLZnSnlHGKhVezROLCoGXaHXNaPrqKgHGPZjYHDhuYTvtGXyQJTNlkFTEBarYuQveZO43008500;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void dYOBstusvGwYqKuzgxDledEQUgUrMrsyKgPxylxlxoQDcYeCXjG99234127() {     float zTaszTygLYCgENxOJuwBvzSoqNBn49710232 = -289738502;    float zTaszTygLYCgENxOJuwBvzSoqNBn98380957 = -576613520;    float zTaszTygLYCgENxOJuwBvzSoqNBn24424221 = -521884163;    float zTaszTygLYCgENxOJuwBvzSoqNBn67819638 = -646141860;    float zTaszTygLYCgENxOJuwBvzSoqNBn3990016 = -485281195;    float zTaszTygLYCgENxOJuwBvzSoqNBn21863394 = -522534199;    float zTaszTygLYCgENxOJuwBvzSoqNBn79335972 = -676485104;    float zTaszTygLYCgENxOJuwBvzSoqNBn71082728 = -14239083;    float zTaszTygLYCgENxOJuwBvzSoqNBn53883926 = 83976809;    float zTaszTygLYCgENxOJuwBvzSoqNBn50933158 = -542562104;    float zTaszTygLYCgENxOJuwBvzSoqNBn26351325 = -298586634;    float zTaszTygLYCgENxOJuwBvzSoqNBn48360285 = -530554221;    float zTaszTygLYCgENxOJuwBvzSoqNBn9726845 = -315454016;    float zTaszTygLYCgENxOJuwBvzSoqNBn41444931 = -182223577;    float zTaszTygLYCgENxOJuwBvzSoqNBn39783955 = -690681734;    float zTaszTygLYCgENxOJuwBvzSoqNBn71561845 = -378455373;    float zTaszTygLYCgENxOJuwBvzSoqNBn14861542 = -755684500;    float zTaszTygLYCgENxOJuwBvzSoqNBn76806388 = -518773268;    float zTaszTygLYCgENxOJuwBvzSoqNBn25635384 = -539634814;    float zTaszTygLYCgENxOJuwBvzSoqNBn18300587 = -884451368;    float zTaszTygLYCgENxOJuwBvzSoqNBn57286816 = 46267404;    float zTaszTygLYCgENxOJuwBvzSoqNBn44075020 = -969131287;    float zTaszTygLYCgENxOJuwBvzSoqNBn65153282 = -997157798;    float zTaszTygLYCgENxOJuwBvzSoqNBn69488067 = -737497753;    float zTaszTygLYCgENxOJuwBvzSoqNBn87348062 = -707040144;    float zTaszTygLYCgENxOJuwBvzSoqNBn74189406 = 18098215;    float zTaszTygLYCgENxOJuwBvzSoqNBn69330596 = -731606448;    float zTaszTygLYCgENxOJuwBvzSoqNBn77944445 = -739969061;    float zTaszTygLYCgENxOJuwBvzSoqNBn64358750 = -214914444;    float zTaszTygLYCgENxOJuwBvzSoqNBn42416609 = -760914640;    float zTaszTygLYCgENxOJuwBvzSoqNBn27580830 = -448226172;    float zTaszTygLYCgENxOJuwBvzSoqNBn21543868 = -437536797;    float zTaszTygLYCgENxOJuwBvzSoqNBn31709716 = -567693203;    float zTaszTygLYCgENxOJuwBvzSoqNBn75707887 = -771983319;    float zTaszTygLYCgENxOJuwBvzSoqNBn11608152 = -542850582;    float zTaszTygLYCgENxOJuwBvzSoqNBn73189457 = -304898036;    float zTaszTygLYCgENxOJuwBvzSoqNBn16971808 = -421799693;    float zTaszTygLYCgENxOJuwBvzSoqNBn85466852 = -482866846;    float zTaszTygLYCgENxOJuwBvzSoqNBn91483911 = -821981779;    float zTaszTygLYCgENxOJuwBvzSoqNBn3398384 = -711690496;    float zTaszTygLYCgENxOJuwBvzSoqNBn98978540 = -82187917;    float zTaszTygLYCgENxOJuwBvzSoqNBn56924769 = -779146297;    float zTaszTygLYCgENxOJuwBvzSoqNBn2204660 = -831689257;    float zTaszTygLYCgENxOJuwBvzSoqNBn74602496 = 13928438;    float zTaszTygLYCgENxOJuwBvzSoqNBn76783367 = -535711035;    float zTaszTygLYCgENxOJuwBvzSoqNBn81219797 = 70133829;    float zTaszTygLYCgENxOJuwBvzSoqNBn72508638 = 97846217;    float zTaszTygLYCgENxOJuwBvzSoqNBn73301856 = -662828389;    float zTaszTygLYCgENxOJuwBvzSoqNBn98763418 = -630801958;    float zTaszTygLYCgENxOJuwBvzSoqNBn1382657 = 37976820;    float zTaszTygLYCgENxOJuwBvzSoqNBn72800672 = -170858354;    float zTaszTygLYCgENxOJuwBvzSoqNBn14732923 = -929018037;    float zTaszTygLYCgENxOJuwBvzSoqNBn11851630 = -462254424;    float zTaszTygLYCgENxOJuwBvzSoqNBn17865393 = -551796785;    float zTaszTygLYCgENxOJuwBvzSoqNBn41947311 = -154532388;    float zTaszTygLYCgENxOJuwBvzSoqNBn5635213 = -320607216;    float zTaszTygLYCgENxOJuwBvzSoqNBn33227675 = -579455722;    float zTaszTygLYCgENxOJuwBvzSoqNBn54936153 = -784386410;    float zTaszTygLYCgENxOJuwBvzSoqNBn80471576 = -939101716;    float zTaszTygLYCgENxOJuwBvzSoqNBn29800610 = -403379410;    float zTaszTygLYCgENxOJuwBvzSoqNBn52532797 = -790927752;    float zTaszTygLYCgENxOJuwBvzSoqNBn1391527 = -936516043;    float zTaszTygLYCgENxOJuwBvzSoqNBn6723979 = -799324640;    float zTaszTygLYCgENxOJuwBvzSoqNBn11467318 = -155108551;    float zTaszTygLYCgENxOJuwBvzSoqNBn23352329 = 5664067;    float zTaszTygLYCgENxOJuwBvzSoqNBn4807457 = -861049838;    float zTaszTygLYCgENxOJuwBvzSoqNBn16650570 = -962861019;    float zTaszTygLYCgENxOJuwBvzSoqNBn34018958 = -543470697;    float zTaszTygLYCgENxOJuwBvzSoqNBn29836780 = -639372995;    float zTaszTygLYCgENxOJuwBvzSoqNBn66594497 = -285783698;    float zTaszTygLYCgENxOJuwBvzSoqNBn54590038 = -956655681;    float zTaszTygLYCgENxOJuwBvzSoqNBn29394690 = -172817654;    float zTaszTygLYCgENxOJuwBvzSoqNBn85322477 = -696791490;    float zTaszTygLYCgENxOJuwBvzSoqNBn22237001 = -827944318;    float zTaszTygLYCgENxOJuwBvzSoqNBn19322046 = -702263452;    float zTaszTygLYCgENxOJuwBvzSoqNBn362048 = -174586299;    float zTaszTygLYCgENxOJuwBvzSoqNBn41870361 = -37442031;    float zTaszTygLYCgENxOJuwBvzSoqNBn90550786 = -911086236;    float zTaszTygLYCgENxOJuwBvzSoqNBn92704699 = -101786719;    float zTaszTygLYCgENxOJuwBvzSoqNBn6128266 = -677173974;    float zTaszTygLYCgENxOJuwBvzSoqNBn1680768 = 20251998;    float zTaszTygLYCgENxOJuwBvzSoqNBn96028740 = 31221940;    float zTaszTygLYCgENxOJuwBvzSoqNBn79181027 = -9167104;    float zTaszTygLYCgENxOJuwBvzSoqNBn62976093 = -152891264;    float zTaszTygLYCgENxOJuwBvzSoqNBn69615937 = -490056287;    float zTaszTygLYCgENxOJuwBvzSoqNBn12847908 = -519208135;    float zTaszTygLYCgENxOJuwBvzSoqNBn9692239 = -975282373;    float zTaszTygLYCgENxOJuwBvzSoqNBn13844323 = 84103582;    float zTaszTygLYCgENxOJuwBvzSoqNBn33760576 = -517450932;    float zTaszTygLYCgENxOJuwBvzSoqNBn5972940 = -122243367;    float zTaszTygLYCgENxOJuwBvzSoqNBn39961783 = -725442314;    float zTaszTygLYCgENxOJuwBvzSoqNBn62035655 = -637413284;    float zTaszTygLYCgENxOJuwBvzSoqNBn4995276 = -543765131;    float zTaszTygLYCgENxOJuwBvzSoqNBn61683301 = -318602369;    float zTaszTygLYCgENxOJuwBvzSoqNBn50865586 = -920762745;    float zTaszTygLYCgENxOJuwBvzSoqNBn97587013 = -145671874;    float zTaszTygLYCgENxOJuwBvzSoqNBn50200791 = -979821658;    float zTaszTygLYCgENxOJuwBvzSoqNBn90737342 = -576580706;    float zTaszTygLYCgENxOJuwBvzSoqNBn51250168 = -991735630;    float zTaszTygLYCgENxOJuwBvzSoqNBn71975910 = -289738502;     zTaszTygLYCgENxOJuwBvzSoqNBn49710232 = zTaszTygLYCgENxOJuwBvzSoqNBn98380957;     zTaszTygLYCgENxOJuwBvzSoqNBn98380957 = zTaszTygLYCgENxOJuwBvzSoqNBn24424221;     zTaszTygLYCgENxOJuwBvzSoqNBn24424221 = zTaszTygLYCgENxOJuwBvzSoqNBn67819638;     zTaszTygLYCgENxOJuwBvzSoqNBn67819638 = zTaszTygLYCgENxOJuwBvzSoqNBn3990016;     zTaszTygLYCgENxOJuwBvzSoqNBn3990016 = zTaszTygLYCgENxOJuwBvzSoqNBn21863394;     zTaszTygLYCgENxOJuwBvzSoqNBn21863394 = zTaszTygLYCgENxOJuwBvzSoqNBn79335972;     zTaszTygLYCgENxOJuwBvzSoqNBn79335972 = zTaszTygLYCgENxOJuwBvzSoqNBn71082728;     zTaszTygLYCgENxOJuwBvzSoqNBn71082728 = zTaszTygLYCgENxOJuwBvzSoqNBn53883926;     zTaszTygLYCgENxOJuwBvzSoqNBn53883926 = zTaszTygLYCgENxOJuwBvzSoqNBn50933158;     zTaszTygLYCgENxOJuwBvzSoqNBn50933158 = zTaszTygLYCgENxOJuwBvzSoqNBn26351325;     zTaszTygLYCgENxOJuwBvzSoqNBn26351325 = zTaszTygLYCgENxOJuwBvzSoqNBn48360285;     zTaszTygLYCgENxOJuwBvzSoqNBn48360285 = zTaszTygLYCgENxOJuwBvzSoqNBn9726845;     zTaszTygLYCgENxOJuwBvzSoqNBn9726845 = zTaszTygLYCgENxOJuwBvzSoqNBn41444931;     zTaszTygLYCgENxOJuwBvzSoqNBn41444931 = zTaszTygLYCgENxOJuwBvzSoqNBn39783955;     zTaszTygLYCgENxOJuwBvzSoqNBn39783955 = zTaszTygLYCgENxOJuwBvzSoqNBn71561845;     zTaszTygLYCgENxOJuwBvzSoqNBn71561845 = zTaszTygLYCgENxOJuwBvzSoqNBn14861542;     zTaszTygLYCgENxOJuwBvzSoqNBn14861542 = zTaszTygLYCgENxOJuwBvzSoqNBn76806388;     zTaszTygLYCgENxOJuwBvzSoqNBn76806388 = zTaszTygLYCgENxOJuwBvzSoqNBn25635384;     zTaszTygLYCgENxOJuwBvzSoqNBn25635384 = zTaszTygLYCgENxOJuwBvzSoqNBn18300587;     zTaszTygLYCgENxOJuwBvzSoqNBn18300587 = zTaszTygLYCgENxOJuwBvzSoqNBn57286816;     zTaszTygLYCgENxOJuwBvzSoqNBn57286816 = zTaszTygLYCgENxOJuwBvzSoqNBn44075020;     zTaszTygLYCgENxOJuwBvzSoqNBn44075020 = zTaszTygLYCgENxOJuwBvzSoqNBn65153282;     zTaszTygLYCgENxOJuwBvzSoqNBn65153282 = zTaszTygLYCgENxOJuwBvzSoqNBn69488067;     zTaszTygLYCgENxOJuwBvzSoqNBn69488067 = zTaszTygLYCgENxOJuwBvzSoqNBn87348062;     zTaszTygLYCgENxOJuwBvzSoqNBn87348062 = zTaszTygLYCgENxOJuwBvzSoqNBn74189406;     zTaszTygLYCgENxOJuwBvzSoqNBn74189406 = zTaszTygLYCgENxOJuwBvzSoqNBn69330596;     zTaszTygLYCgENxOJuwBvzSoqNBn69330596 = zTaszTygLYCgENxOJuwBvzSoqNBn77944445;     zTaszTygLYCgENxOJuwBvzSoqNBn77944445 = zTaszTygLYCgENxOJuwBvzSoqNBn64358750;     zTaszTygLYCgENxOJuwBvzSoqNBn64358750 = zTaszTygLYCgENxOJuwBvzSoqNBn42416609;     zTaszTygLYCgENxOJuwBvzSoqNBn42416609 = zTaszTygLYCgENxOJuwBvzSoqNBn27580830;     zTaszTygLYCgENxOJuwBvzSoqNBn27580830 = zTaszTygLYCgENxOJuwBvzSoqNBn21543868;     zTaszTygLYCgENxOJuwBvzSoqNBn21543868 = zTaszTygLYCgENxOJuwBvzSoqNBn31709716;     zTaszTygLYCgENxOJuwBvzSoqNBn31709716 = zTaszTygLYCgENxOJuwBvzSoqNBn75707887;     zTaszTygLYCgENxOJuwBvzSoqNBn75707887 = zTaszTygLYCgENxOJuwBvzSoqNBn11608152;     zTaszTygLYCgENxOJuwBvzSoqNBn11608152 = zTaszTygLYCgENxOJuwBvzSoqNBn73189457;     zTaszTygLYCgENxOJuwBvzSoqNBn73189457 = zTaszTygLYCgENxOJuwBvzSoqNBn16971808;     zTaszTygLYCgENxOJuwBvzSoqNBn16971808 = zTaszTygLYCgENxOJuwBvzSoqNBn85466852;     zTaszTygLYCgENxOJuwBvzSoqNBn85466852 = zTaszTygLYCgENxOJuwBvzSoqNBn91483911;     zTaszTygLYCgENxOJuwBvzSoqNBn91483911 = zTaszTygLYCgENxOJuwBvzSoqNBn3398384;     zTaszTygLYCgENxOJuwBvzSoqNBn3398384 = zTaszTygLYCgENxOJuwBvzSoqNBn98978540;     zTaszTygLYCgENxOJuwBvzSoqNBn98978540 = zTaszTygLYCgENxOJuwBvzSoqNBn56924769;     zTaszTygLYCgENxOJuwBvzSoqNBn56924769 = zTaszTygLYCgENxOJuwBvzSoqNBn2204660;     zTaszTygLYCgENxOJuwBvzSoqNBn2204660 = zTaszTygLYCgENxOJuwBvzSoqNBn74602496;     zTaszTygLYCgENxOJuwBvzSoqNBn74602496 = zTaszTygLYCgENxOJuwBvzSoqNBn76783367;     zTaszTygLYCgENxOJuwBvzSoqNBn76783367 = zTaszTygLYCgENxOJuwBvzSoqNBn81219797;     zTaszTygLYCgENxOJuwBvzSoqNBn81219797 = zTaszTygLYCgENxOJuwBvzSoqNBn72508638;     zTaszTygLYCgENxOJuwBvzSoqNBn72508638 = zTaszTygLYCgENxOJuwBvzSoqNBn73301856;     zTaszTygLYCgENxOJuwBvzSoqNBn73301856 = zTaszTygLYCgENxOJuwBvzSoqNBn98763418;     zTaszTygLYCgENxOJuwBvzSoqNBn98763418 = zTaszTygLYCgENxOJuwBvzSoqNBn1382657;     zTaszTygLYCgENxOJuwBvzSoqNBn1382657 = zTaszTygLYCgENxOJuwBvzSoqNBn72800672;     zTaszTygLYCgENxOJuwBvzSoqNBn72800672 = zTaszTygLYCgENxOJuwBvzSoqNBn14732923;     zTaszTygLYCgENxOJuwBvzSoqNBn14732923 = zTaszTygLYCgENxOJuwBvzSoqNBn11851630;     zTaszTygLYCgENxOJuwBvzSoqNBn11851630 = zTaszTygLYCgENxOJuwBvzSoqNBn17865393;     zTaszTygLYCgENxOJuwBvzSoqNBn17865393 = zTaszTygLYCgENxOJuwBvzSoqNBn41947311;     zTaszTygLYCgENxOJuwBvzSoqNBn41947311 = zTaszTygLYCgENxOJuwBvzSoqNBn5635213;     zTaszTygLYCgENxOJuwBvzSoqNBn5635213 = zTaszTygLYCgENxOJuwBvzSoqNBn33227675;     zTaszTygLYCgENxOJuwBvzSoqNBn33227675 = zTaszTygLYCgENxOJuwBvzSoqNBn54936153;     zTaszTygLYCgENxOJuwBvzSoqNBn54936153 = zTaszTygLYCgENxOJuwBvzSoqNBn80471576;     zTaszTygLYCgENxOJuwBvzSoqNBn80471576 = zTaszTygLYCgENxOJuwBvzSoqNBn29800610;     zTaszTygLYCgENxOJuwBvzSoqNBn29800610 = zTaszTygLYCgENxOJuwBvzSoqNBn52532797;     zTaszTygLYCgENxOJuwBvzSoqNBn52532797 = zTaszTygLYCgENxOJuwBvzSoqNBn1391527;     zTaszTygLYCgENxOJuwBvzSoqNBn1391527 = zTaszTygLYCgENxOJuwBvzSoqNBn6723979;     zTaszTygLYCgENxOJuwBvzSoqNBn6723979 = zTaszTygLYCgENxOJuwBvzSoqNBn11467318;     zTaszTygLYCgENxOJuwBvzSoqNBn11467318 = zTaszTygLYCgENxOJuwBvzSoqNBn23352329;     zTaszTygLYCgENxOJuwBvzSoqNBn23352329 = zTaszTygLYCgENxOJuwBvzSoqNBn4807457;     zTaszTygLYCgENxOJuwBvzSoqNBn4807457 = zTaszTygLYCgENxOJuwBvzSoqNBn16650570;     zTaszTygLYCgENxOJuwBvzSoqNBn16650570 = zTaszTygLYCgENxOJuwBvzSoqNBn34018958;     zTaszTygLYCgENxOJuwBvzSoqNBn34018958 = zTaszTygLYCgENxOJuwBvzSoqNBn29836780;     zTaszTygLYCgENxOJuwBvzSoqNBn29836780 = zTaszTygLYCgENxOJuwBvzSoqNBn66594497;     zTaszTygLYCgENxOJuwBvzSoqNBn66594497 = zTaszTygLYCgENxOJuwBvzSoqNBn54590038;     zTaszTygLYCgENxOJuwBvzSoqNBn54590038 = zTaszTygLYCgENxOJuwBvzSoqNBn29394690;     zTaszTygLYCgENxOJuwBvzSoqNBn29394690 = zTaszTygLYCgENxOJuwBvzSoqNBn85322477;     zTaszTygLYCgENxOJuwBvzSoqNBn85322477 = zTaszTygLYCgENxOJuwBvzSoqNBn22237001;     zTaszTygLYCgENxOJuwBvzSoqNBn22237001 = zTaszTygLYCgENxOJuwBvzSoqNBn19322046;     zTaszTygLYCgENxOJuwBvzSoqNBn19322046 = zTaszTygLYCgENxOJuwBvzSoqNBn362048;     zTaszTygLYCgENxOJuwBvzSoqNBn362048 = zTaszTygLYCgENxOJuwBvzSoqNBn41870361;     zTaszTygLYCgENxOJuwBvzSoqNBn41870361 = zTaszTygLYCgENxOJuwBvzSoqNBn90550786;     zTaszTygLYCgENxOJuwBvzSoqNBn90550786 = zTaszTygLYCgENxOJuwBvzSoqNBn92704699;     zTaszTygLYCgENxOJuwBvzSoqNBn92704699 = zTaszTygLYCgENxOJuwBvzSoqNBn6128266;     zTaszTygLYCgENxOJuwBvzSoqNBn6128266 = zTaszTygLYCgENxOJuwBvzSoqNBn1680768;     zTaszTygLYCgENxOJuwBvzSoqNBn1680768 = zTaszTygLYCgENxOJuwBvzSoqNBn96028740;     zTaszTygLYCgENxOJuwBvzSoqNBn96028740 = zTaszTygLYCgENxOJuwBvzSoqNBn79181027;     zTaszTygLYCgENxOJuwBvzSoqNBn79181027 = zTaszTygLYCgENxOJuwBvzSoqNBn62976093;     zTaszTygLYCgENxOJuwBvzSoqNBn62976093 = zTaszTygLYCgENxOJuwBvzSoqNBn69615937;     zTaszTygLYCgENxOJuwBvzSoqNBn69615937 = zTaszTygLYCgENxOJuwBvzSoqNBn12847908;     zTaszTygLYCgENxOJuwBvzSoqNBn12847908 = zTaszTygLYCgENxOJuwBvzSoqNBn9692239;     zTaszTygLYCgENxOJuwBvzSoqNBn9692239 = zTaszTygLYCgENxOJuwBvzSoqNBn13844323;     zTaszTygLYCgENxOJuwBvzSoqNBn13844323 = zTaszTygLYCgENxOJuwBvzSoqNBn33760576;     zTaszTygLYCgENxOJuwBvzSoqNBn33760576 = zTaszTygLYCgENxOJuwBvzSoqNBn5972940;     zTaszTygLYCgENxOJuwBvzSoqNBn5972940 = zTaszTygLYCgENxOJuwBvzSoqNBn39961783;     zTaszTygLYCgENxOJuwBvzSoqNBn39961783 = zTaszTygLYCgENxOJuwBvzSoqNBn62035655;     zTaszTygLYCgENxOJuwBvzSoqNBn62035655 = zTaszTygLYCgENxOJuwBvzSoqNBn4995276;     zTaszTygLYCgENxOJuwBvzSoqNBn4995276 = zTaszTygLYCgENxOJuwBvzSoqNBn61683301;     zTaszTygLYCgENxOJuwBvzSoqNBn61683301 = zTaszTygLYCgENxOJuwBvzSoqNBn50865586;     zTaszTygLYCgENxOJuwBvzSoqNBn50865586 = zTaszTygLYCgENxOJuwBvzSoqNBn97587013;     zTaszTygLYCgENxOJuwBvzSoqNBn97587013 = zTaszTygLYCgENxOJuwBvzSoqNBn50200791;     zTaszTygLYCgENxOJuwBvzSoqNBn50200791 = zTaszTygLYCgENxOJuwBvzSoqNBn90737342;     zTaszTygLYCgENxOJuwBvzSoqNBn90737342 = zTaszTygLYCgENxOJuwBvzSoqNBn51250168;     zTaszTygLYCgENxOJuwBvzSoqNBn51250168 = zTaszTygLYCgENxOJuwBvzSoqNBn71975910;     zTaszTygLYCgENxOJuwBvzSoqNBn71975910 = zTaszTygLYCgENxOJuwBvzSoqNBn49710232;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void KNAWgCGQIIRVTCVFjQogvicBG34384683() {     long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd97432850 = -99425366;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd87936042 = -229973307;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd20867520 = 90757398;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd92263085 = -671135719;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd76140604 = -568034010;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd54119128 = -299713552;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd90300528 = -4283659;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd56336449 = -101063520;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd28861412 = -359947399;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd19770385 = -328689257;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd15730625 = -73668040;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd28778134 = -862864970;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd11245407 = -327280698;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd11923397 = -770866630;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd48097328 = -685598899;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd16473605 = -161439407;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd42959461 = -143804160;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd91253400 = -865633136;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd60235497 = -341220174;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd76876871 = -771961018;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd1462445 = -869880700;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd36736791 = -910548370;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd91281198 = -54078916;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd82637204 = -344453223;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd44127139 = -562799632;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd99859554 = -912633304;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd91228679 = -635675865;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd93977641 = 31396604;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd13241887 = -657363004;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd23303343 = -592024228;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd46963537 = -60378947;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd24935015 = -760316545;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd93827056 = -451081349;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd29087410 = -314629193;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd6512148 = -500212595;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd2503342 = -145336251;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd73380746 = -37523064;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd28987516 = -25508356;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd86885561 = 7707331;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd95988563 = -97927520;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd68055526 = -322880005;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd29568456 = 69695931;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd60252727 = -245042380;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd7093175 = -613493863;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd5563189 = -322354995;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd16821499 = 27198446;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd52160424 = -186064563;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd4571013 = -339766109;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd75490738 = -97751474;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd82362493 = -374481321;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd77634349 = 32863182;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd94887506 = -183154040;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd45968432 = 42861416;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd67007945 = -850889379;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd32922239 = -967675938;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd60696060 = -188876996;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd96654844 = -75894391;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd38230316 = -564789379;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd48135946 = -8336088;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd76281049 = -655400707;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd62890448 = -664037688;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd96322887 = 64319737;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd43094563 = -443700516;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd5558069 = -767923171;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd72806847 = -168310310;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd90795610 = -313351495;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd34951078 = -311783622;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd82157997 = 87348494;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd5411250 = -170654036;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd45593986 = -440262649;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd43092858 = -23916343;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd13971946 = -18295804;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd4367840 = -773340468;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd64246933 = -143292654;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd8821345 = -349081014;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd71893989 = -839576631;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd76484063 = -565505991;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd84188023 = -440585054;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd77074015 = 77901772;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd27305640 = -489998078;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd47699131 = -626568741;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd86657666 = -195909756;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd18486903 = -870851923;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd30879393 = -182881684;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd45668994 = -524887411;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd52076031 = -877224908;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd78966583 = -703177961;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd26819111 = -600191970;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd96165171 = -346953256;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd45816088 = -211335599;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd5848498 = 30558140;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd35150431 = -472733686;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd80851569 = 82827731;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd10604512 = -336891962;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd33098116 = -433889833;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd71732638 = -287199742;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd86473893 = -486603553;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd54694658 = -477119210;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd34286328 = -345183554;    long XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd14767579 = -99425366;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd97432850 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd87936042;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd87936042 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd20867520;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd20867520 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd92263085;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd92263085 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd76140604;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd76140604 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd54119128;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd54119128 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd90300528;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd90300528 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd56336449;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd56336449 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd28861412;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd28861412 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd19770385;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd19770385 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd15730625;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd15730625 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd28778134;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd28778134 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd11245407;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd11245407 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd11923397;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd11923397 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd48097328;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd48097328 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd16473605;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd16473605 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd42959461;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd42959461 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd91253400;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd91253400 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd60235497;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd60235497 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd76876871;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd76876871 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd1462445;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd1462445 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd36736791;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd36736791 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd91281198;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd91281198 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd82637204;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd82637204 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd44127139;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd44127139 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd99859554;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd99859554 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd91228679;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd91228679 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd93977641;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd93977641 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd13241887;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd13241887 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd23303343;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd23303343 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd46963537;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd46963537 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd24935015;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd24935015 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd93827056;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd93827056 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd29087410;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd29087410 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd6512148;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd6512148 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd2503342;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd2503342 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd73380746;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd73380746 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd28987516;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd28987516 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd86885561;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd86885561 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd95988563;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd95988563 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd68055526;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd68055526 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd29568456;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd29568456 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd60252727;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd60252727 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd7093175;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd7093175 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd5563189;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd5563189 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd16821499;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd16821499 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd52160424;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd52160424 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd4571013;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd4571013 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd75490738;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd75490738 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd82362493;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd82362493 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd77634349;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd77634349 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd94887506;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd94887506 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd45968432;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd45968432 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd67007945;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd67007945 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd32922239;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd32922239 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd60696060;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd60696060 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd96654844;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd96654844 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd38230316;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd38230316 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd48135946;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd48135946 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd76281049;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd76281049 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd62890448;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd62890448 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd96322887;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd96322887 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd43094563;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd43094563 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd5558069;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd5558069 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd72806847;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd72806847 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd90795610;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd90795610 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd34951078;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd34951078 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd82157997;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd82157997 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd5411250;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd5411250 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd45593986;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd45593986 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd43092858;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd43092858 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd13971946;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd13971946 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd4367840;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd4367840 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd64246933;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd64246933 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd8821345;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd8821345 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd71893989;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd71893989 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd76484063;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd76484063 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd84188023;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd84188023 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd77074015;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd77074015 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd27305640;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd27305640 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd47699131;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd47699131 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd86657666;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd86657666 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd18486903;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd18486903 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd30879393;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd30879393 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd45668994;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd45668994 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd52076031;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd52076031 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd78966583;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd78966583 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd26819111;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd26819111 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd96165171;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd96165171 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd45816088;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd45816088 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd5848498;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd5848498 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd35150431;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd35150431 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd80851569;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd80851569 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd10604512;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd10604512 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd33098116;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd33098116 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd71732638;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd71732638 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd86473893;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd86473893 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd54694658;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd54694658 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd34286328;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd34286328 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd14767579;     XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd14767579 = XOwlLGRVturNqwXRItJpyjddGHykvablzQInJexBAzZHd97432850;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void sCsalXSvdEiKotnwumwjJSfONStdvSVlgGLXU25706516() {     double ElGBlMOGVDrGUGDFKlaXd4741859 = -496654819;    double ElGBlMOGVDrGUGDFKlaXd86855451 = -857381166;    double ElGBlMOGVDrGUGDFKlaXd6282026 = -956434884;    double ElGBlMOGVDrGUGDFKlaXd90202081 = -749551702;    double ElGBlMOGVDrGUGDFKlaXd72844572 = -153826836;    double ElGBlMOGVDrGUGDFKlaXd36260131 = -584301796;    double ElGBlMOGVDrGUGDFKlaXd51444572 = -968604500;    double ElGBlMOGVDrGUGDFKlaXd96236528 = -98365309;    double ElGBlMOGVDrGUGDFKlaXd41762588 = -357013211;    double ElGBlMOGVDrGUGDFKlaXd66591396 = -61285570;    double ElGBlMOGVDrGUGDFKlaXd60885764 = -189499993;    double ElGBlMOGVDrGUGDFKlaXd65304699 = -893414512;    double ElGBlMOGVDrGUGDFKlaXd31297902 = -827773610;    double ElGBlMOGVDrGUGDFKlaXd71542032 = -681397608;    double ElGBlMOGVDrGUGDFKlaXd21476671 = -317111864;    double ElGBlMOGVDrGUGDFKlaXd19986439 = -798989807;    double ElGBlMOGVDrGUGDFKlaXd90797622 = -289561956;    double ElGBlMOGVDrGUGDFKlaXd54869543 = -527436541;    double ElGBlMOGVDrGUGDFKlaXd55920219 = -108898564;    double ElGBlMOGVDrGUGDFKlaXd23480910 = -39915998;    double ElGBlMOGVDrGUGDFKlaXd45603328 = -84749189;    double ElGBlMOGVDrGUGDFKlaXd65332558 = -959355157;    double ElGBlMOGVDrGUGDFKlaXd64918534 = -113731876;    double ElGBlMOGVDrGUGDFKlaXd25991743 = -437127714;    double ElGBlMOGVDrGUGDFKlaXd69548255 = 35262510;    double ElGBlMOGVDrGUGDFKlaXd94805726 = -740291187;    double ElGBlMOGVDrGUGDFKlaXd81799205 = -340424848;    double ElGBlMOGVDrGUGDFKlaXd28227293 = -732069804;    double ElGBlMOGVDrGUGDFKlaXd30927036 = -208509653;    double ElGBlMOGVDrGUGDFKlaXd6798350 = 16735880;    double ElGBlMOGVDrGUGDFKlaXd41379582 = -898390903;    double ElGBlMOGVDrGUGDFKlaXd48177208 = -988046933;    double ElGBlMOGVDrGUGDFKlaXd33205071 = -67332691;    double ElGBlMOGVDrGUGDFKlaXd65415630 = -824181596;    double ElGBlMOGVDrGUGDFKlaXd29890562 = -494755479;    double ElGBlMOGVDrGUGDFKlaXd74649407 = -967079668;    double ElGBlMOGVDrGUGDFKlaXd88333414 = -645505137;    double ElGBlMOGVDrGUGDFKlaXd75745036 = -605496275;    double ElGBlMOGVDrGUGDFKlaXd54275549 = -500332200;    double ElGBlMOGVDrGUGDFKlaXd19213738 = -822193021;    double ElGBlMOGVDrGUGDFKlaXd77133827 = -614149537;    double ElGBlMOGVDrGUGDFKlaXd50837815 = -477091383;    double ElGBlMOGVDrGUGDFKlaXd99605077 = -230535715;    double ElGBlMOGVDrGUGDFKlaXd52671206 = 99722433;    double ElGBlMOGVDrGUGDFKlaXd61358404 = -815206127;    double ElGBlMOGVDrGUGDFKlaXd77858282 = -526748103;    double ElGBlMOGVDrGUGDFKlaXd78847620 = 16248057;    double ElGBlMOGVDrGUGDFKlaXd45611935 = -712599092;    double ElGBlMOGVDrGUGDFKlaXd69595290 = -844284401;    double ElGBlMOGVDrGUGDFKlaXd8577857 = -488729252;    double ElGBlMOGVDrGUGDFKlaXd52843333 = -17356018;    double ElGBlMOGVDrGUGDFKlaXd3448131 = -531751920;    double ElGBlMOGVDrGUGDFKlaXd18673271 = -80052210;    double ElGBlMOGVDrGUGDFKlaXd4800428 = -558030706;    double ElGBlMOGVDrGUGDFKlaXd80103188 = -280102490;    double ElGBlMOGVDrGUGDFKlaXd39409300 = -537299663;    double ElGBlMOGVDrGUGDFKlaXd21936918 = -643649291;    double ElGBlMOGVDrGUGDFKlaXd80290283 = -419307171;    double ElGBlMOGVDrGUGDFKlaXd20653827 = -684814213;    double ElGBlMOGVDrGUGDFKlaXd78038845 = -413535649;    double ElGBlMOGVDrGUGDFKlaXd54460925 = -143876948;    double ElGBlMOGVDrGUGDFKlaXd23217279 = -136534697;    double ElGBlMOGVDrGUGDFKlaXd65309493 = -889855657;    double ElGBlMOGVDrGUGDFKlaXd34964238 = -273749092;    double ElGBlMOGVDrGUGDFKlaXd25211815 = -162894667;    double ElGBlMOGVDrGUGDFKlaXd12708557 = -201453060;    double ElGBlMOGVDrGUGDFKlaXd32099629 = -726081822;    double ElGBlMOGVDrGUGDFKlaXd65882272 = 96407985;    double ElGBlMOGVDrGUGDFKlaXd41651470 = -86642129;    double ElGBlMOGVDrGUGDFKlaXd46827263 = -350032197;    double ElGBlMOGVDrGUGDFKlaXd31653024 = -53484671;    double ElGBlMOGVDrGUGDFKlaXd15052587 = -684065681;    double ElGBlMOGVDrGUGDFKlaXd593994 = 72895658;    double ElGBlMOGVDrGUGDFKlaXd36706481 = -286705543;    double ElGBlMOGVDrGUGDFKlaXd46347083 = -425766461;    double ElGBlMOGVDrGUGDFKlaXd94765512 = -607657806;    double ElGBlMOGVDrGUGDFKlaXd65727481 = -628819443;    double ElGBlMOGVDrGUGDFKlaXd12247328 = -113454310;    double ElGBlMOGVDrGUGDFKlaXd64633339 = -621921587;    double ElGBlMOGVDrGUGDFKlaXd91689973 = -437989387;    double ElGBlMOGVDrGUGDFKlaXd15958106 = -656539244;    double ElGBlMOGVDrGUGDFKlaXd36187271 = -627825757;    double ElGBlMOGVDrGUGDFKlaXd58632002 = -887785404;    double ElGBlMOGVDrGUGDFKlaXd22349179 = -719780401;    double ElGBlMOGVDrGUGDFKlaXd53955017 = -965908102;    double ElGBlMOGVDrGUGDFKlaXd37931451 = -266638984;    double ElGBlMOGVDrGUGDFKlaXd29503937 = -807994723;    double ElGBlMOGVDrGUGDFKlaXd28404643 = -509301986;    double ElGBlMOGVDrGUGDFKlaXd85312442 = -444079106;    double ElGBlMOGVDrGUGDFKlaXd90481261 = -957455817;    double ElGBlMOGVDrGUGDFKlaXd52712490 = -223430378;    double ElGBlMOGVDrGUGDFKlaXd8043132 = -126197967;    double ElGBlMOGVDrGUGDFKlaXd55091209 = -920682062;    double ElGBlMOGVDrGUGDFKlaXd76236703 = 13203449;    double ElGBlMOGVDrGUGDFKlaXd64752813 = -578316074;    double ElGBlMOGVDrGUGDFKlaXd53916548 = -377614841;    double ElGBlMOGVDrGUGDFKlaXd85528321 = -587235727;    double ElGBlMOGVDrGUGDFKlaXd64640839 = -956786623;    double ElGBlMOGVDrGUGDFKlaXd27459391 = -737382900;    double ElGBlMOGVDrGUGDFKlaXd48649847 = -496654819;     ElGBlMOGVDrGUGDFKlaXd4741859 = ElGBlMOGVDrGUGDFKlaXd86855451;     ElGBlMOGVDrGUGDFKlaXd86855451 = ElGBlMOGVDrGUGDFKlaXd6282026;     ElGBlMOGVDrGUGDFKlaXd6282026 = ElGBlMOGVDrGUGDFKlaXd90202081;     ElGBlMOGVDrGUGDFKlaXd90202081 = ElGBlMOGVDrGUGDFKlaXd72844572;     ElGBlMOGVDrGUGDFKlaXd72844572 = ElGBlMOGVDrGUGDFKlaXd36260131;     ElGBlMOGVDrGUGDFKlaXd36260131 = ElGBlMOGVDrGUGDFKlaXd51444572;     ElGBlMOGVDrGUGDFKlaXd51444572 = ElGBlMOGVDrGUGDFKlaXd96236528;     ElGBlMOGVDrGUGDFKlaXd96236528 = ElGBlMOGVDrGUGDFKlaXd41762588;     ElGBlMOGVDrGUGDFKlaXd41762588 = ElGBlMOGVDrGUGDFKlaXd66591396;     ElGBlMOGVDrGUGDFKlaXd66591396 = ElGBlMOGVDrGUGDFKlaXd60885764;     ElGBlMOGVDrGUGDFKlaXd60885764 = ElGBlMOGVDrGUGDFKlaXd65304699;     ElGBlMOGVDrGUGDFKlaXd65304699 = ElGBlMOGVDrGUGDFKlaXd31297902;     ElGBlMOGVDrGUGDFKlaXd31297902 = ElGBlMOGVDrGUGDFKlaXd71542032;     ElGBlMOGVDrGUGDFKlaXd71542032 = ElGBlMOGVDrGUGDFKlaXd21476671;     ElGBlMOGVDrGUGDFKlaXd21476671 = ElGBlMOGVDrGUGDFKlaXd19986439;     ElGBlMOGVDrGUGDFKlaXd19986439 = ElGBlMOGVDrGUGDFKlaXd90797622;     ElGBlMOGVDrGUGDFKlaXd90797622 = ElGBlMOGVDrGUGDFKlaXd54869543;     ElGBlMOGVDrGUGDFKlaXd54869543 = ElGBlMOGVDrGUGDFKlaXd55920219;     ElGBlMOGVDrGUGDFKlaXd55920219 = ElGBlMOGVDrGUGDFKlaXd23480910;     ElGBlMOGVDrGUGDFKlaXd23480910 = ElGBlMOGVDrGUGDFKlaXd45603328;     ElGBlMOGVDrGUGDFKlaXd45603328 = ElGBlMOGVDrGUGDFKlaXd65332558;     ElGBlMOGVDrGUGDFKlaXd65332558 = ElGBlMOGVDrGUGDFKlaXd64918534;     ElGBlMOGVDrGUGDFKlaXd64918534 = ElGBlMOGVDrGUGDFKlaXd25991743;     ElGBlMOGVDrGUGDFKlaXd25991743 = ElGBlMOGVDrGUGDFKlaXd69548255;     ElGBlMOGVDrGUGDFKlaXd69548255 = ElGBlMOGVDrGUGDFKlaXd94805726;     ElGBlMOGVDrGUGDFKlaXd94805726 = ElGBlMOGVDrGUGDFKlaXd81799205;     ElGBlMOGVDrGUGDFKlaXd81799205 = ElGBlMOGVDrGUGDFKlaXd28227293;     ElGBlMOGVDrGUGDFKlaXd28227293 = ElGBlMOGVDrGUGDFKlaXd30927036;     ElGBlMOGVDrGUGDFKlaXd30927036 = ElGBlMOGVDrGUGDFKlaXd6798350;     ElGBlMOGVDrGUGDFKlaXd6798350 = ElGBlMOGVDrGUGDFKlaXd41379582;     ElGBlMOGVDrGUGDFKlaXd41379582 = ElGBlMOGVDrGUGDFKlaXd48177208;     ElGBlMOGVDrGUGDFKlaXd48177208 = ElGBlMOGVDrGUGDFKlaXd33205071;     ElGBlMOGVDrGUGDFKlaXd33205071 = ElGBlMOGVDrGUGDFKlaXd65415630;     ElGBlMOGVDrGUGDFKlaXd65415630 = ElGBlMOGVDrGUGDFKlaXd29890562;     ElGBlMOGVDrGUGDFKlaXd29890562 = ElGBlMOGVDrGUGDFKlaXd74649407;     ElGBlMOGVDrGUGDFKlaXd74649407 = ElGBlMOGVDrGUGDFKlaXd88333414;     ElGBlMOGVDrGUGDFKlaXd88333414 = ElGBlMOGVDrGUGDFKlaXd75745036;     ElGBlMOGVDrGUGDFKlaXd75745036 = ElGBlMOGVDrGUGDFKlaXd54275549;     ElGBlMOGVDrGUGDFKlaXd54275549 = ElGBlMOGVDrGUGDFKlaXd19213738;     ElGBlMOGVDrGUGDFKlaXd19213738 = ElGBlMOGVDrGUGDFKlaXd77133827;     ElGBlMOGVDrGUGDFKlaXd77133827 = ElGBlMOGVDrGUGDFKlaXd50837815;     ElGBlMOGVDrGUGDFKlaXd50837815 = ElGBlMOGVDrGUGDFKlaXd99605077;     ElGBlMOGVDrGUGDFKlaXd99605077 = ElGBlMOGVDrGUGDFKlaXd52671206;     ElGBlMOGVDrGUGDFKlaXd52671206 = ElGBlMOGVDrGUGDFKlaXd61358404;     ElGBlMOGVDrGUGDFKlaXd61358404 = ElGBlMOGVDrGUGDFKlaXd77858282;     ElGBlMOGVDrGUGDFKlaXd77858282 = ElGBlMOGVDrGUGDFKlaXd78847620;     ElGBlMOGVDrGUGDFKlaXd78847620 = ElGBlMOGVDrGUGDFKlaXd45611935;     ElGBlMOGVDrGUGDFKlaXd45611935 = ElGBlMOGVDrGUGDFKlaXd69595290;     ElGBlMOGVDrGUGDFKlaXd69595290 = ElGBlMOGVDrGUGDFKlaXd8577857;     ElGBlMOGVDrGUGDFKlaXd8577857 = ElGBlMOGVDrGUGDFKlaXd52843333;     ElGBlMOGVDrGUGDFKlaXd52843333 = ElGBlMOGVDrGUGDFKlaXd3448131;     ElGBlMOGVDrGUGDFKlaXd3448131 = ElGBlMOGVDrGUGDFKlaXd18673271;     ElGBlMOGVDrGUGDFKlaXd18673271 = ElGBlMOGVDrGUGDFKlaXd4800428;     ElGBlMOGVDrGUGDFKlaXd4800428 = ElGBlMOGVDrGUGDFKlaXd80103188;     ElGBlMOGVDrGUGDFKlaXd80103188 = ElGBlMOGVDrGUGDFKlaXd39409300;     ElGBlMOGVDrGUGDFKlaXd39409300 = ElGBlMOGVDrGUGDFKlaXd21936918;     ElGBlMOGVDrGUGDFKlaXd21936918 = ElGBlMOGVDrGUGDFKlaXd80290283;     ElGBlMOGVDrGUGDFKlaXd80290283 = ElGBlMOGVDrGUGDFKlaXd20653827;     ElGBlMOGVDrGUGDFKlaXd20653827 = ElGBlMOGVDrGUGDFKlaXd78038845;     ElGBlMOGVDrGUGDFKlaXd78038845 = ElGBlMOGVDrGUGDFKlaXd54460925;     ElGBlMOGVDrGUGDFKlaXd54460925 = ElGBlMOGVDrGUGDFKlaXd23217279;     ElGBlMOGVDrGUGDFKlaXd23217279 = ElGBlMOGVDrGUGDFKlaXd65309493;     ElGBlMOGVDrGUGDFKlaXd65309493 = ElGBlMOGVDrGUGDFKlaXd34964238;     ElGBlMOGVDrGUGDFKlaXd34964238 = ElGBlMOGVDrGUGDFKlaXd25211815;     ElGBlMOGVDrGUGDFKlaXd25211815 = ElGBlMOGVDrGUGDFKlaXd12708557;     ElGBlMOGVDrGUGDFKlaXd12708557 = ElGBlMOGVDrGUGDFKlaXd32099629;     ElGBlMOGVDrGUGDFKlaXd32099629 = ElGBlMOGVDrGUGDFKlaXd65882272;     ElGBlMOGVDrGUGDFKlaXd65882272 = ElGBlMOGVDrGUGDFKlaXd41651470;     ElGBlMOGVDrGUGDFKlaXd41651470 = ElGBlMOGVDrGUGDFKlaXd46827263;     ElGBlMOGVDrGUGDFKlaXd46827263 = ElGBlMOGVDrGUGDFKlaXd31653024;     ElGBlMOGVDrGUGDFKlaXd31653024 = ElGBlMOGVDrGUGDFKlaXd15052587;     ElGBlMOGVDrGUGDFKlaXd15052587 = ElGBlMOGVDrGUGDFKlaXd593994;     ElGBlMOGVDrGUGDFKlaXd593994 = ElGBlMOGVDrGUGDFKlaXd36706481;     ElGBlMOGVDrGUGDFKlaXd36706481 = ElGBlMOGVDrGUGDFKlaXd46347083;     ElGBlMOGVDrGUGDFKlaXd46347083 = ElGBlMOGVDrGUGDFKlaXd94765512;     ElGBlMOGVDrGUGDFKlaXd94765512 = ElGBlMOGVDrGUGDFKlaXd65727481;     ElGBlMOGVDrGUGDFKlaXd65727481 = ElGBlMOGVDrGUGDFKlaXd12247328;     ElGBlMOGVDrGUGDFKlaXd12247328 = ElGBlMOGVDrGUGDFKlaXd64633339;     ElGBlMOGVDrGUGDFKlaXd64633339 = ElGBlMOGVDrGUGDFKlaXd91689973;     ElGBlMOGVDrGUGDFKlaXd91689973 = ElGBlMOGVDrGUGDFKlaXd15958106;     ElGBlMOGVDrGUGDFKlaXd15958106 = ElGBlMOGVDrGUGDFKlaXd36187271;     ElGBlMOGVDrGUGDFKlaXd36187271 = ElGBlMOGVDrGUGDFKlaXd58632002;     ElGBlMOGVDrGUGDFKlaXd58632002 = ElGBlMOGVDrGUGDFKlaXd22349179;     ElGBlMOGVDrGUGDFKlaXd22349179 = ElGBlMOGVDrGUGDFKlaXd53955017;     ElGBlMOGVDrGUGDFKlaXd53955017 = ElGBlMOGVDrGUGDFKlaXd37931451;     ElGBlMOGVDrGUGDFKlaXd37931451 = ElGBlMOGVDrGUGDFKlaXd29503937;     ElGBlMOGVDrGUGDFKlaXd29503937 = ElGBlMOGVDrGUGDFKlaXd28404643;     ElGBlMOGVDrGUGDFKlaXd28404643 = ElGBlMOGVDrGUGDFKlaXd85312442;     ElGBlMOGVDrGUGDFKlaXd85312442 = ElGBlMOGVDrGUGDFKlaXd90481261;     ElGBlMOGVDrGUGDFKlaXd90481261 = ElGBlMOGVDrGUGDFKlaXd52712490;     ElGBlMOGVDrGUGDFKlaXd52712490 = ElGBlMOGVDrGUGDFKlaXd8043132;     ElGBlMOGVDrGUGDFKlaXd8043132 = ElGBlMOGVDrGUGDFKlaXd55091209;     ElGBlMOGVDrGUGDFKlaXd55091209 = ElGBlMOGVDrGUGDFKlaXd76236703;     ElGBlMOGVDrGUGDFKlaXd76236703 = ElGBlMOGVDrGUGDFKlaXd64752813;     ElGBlMOGVDrGUGDFKlaXd64752813 = ElGBlMOGVDrGUGDFKlaXd53916548;     ElGBlMOGVDrGUGDFKlaXd53916548 = ElGBlMOGVDrGUGDFKlaXd85528321;     ElGBlMOGVDrGUGDFKlaXd85528321 = ElGBlMOGVDrGUGDFKlaXd64640839;     ElGBlMOGVDrGUGDFKlaXd64640839 = ElGBlMOGVDrGUGDFKlaXd27459391;     ElGBlMOGVDrGUGDFKlaXd27459391 = ElGBlMOGVDrGUGDFKlaXd48649847;     ElGBlMOGVDrGUGDFKlaXd48649847 = ElGBlMOGVDrGUGDFKlaXd4741859;}
// Junk Finished
