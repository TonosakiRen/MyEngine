#pragma once
/**
 * @file MyMath.h
 * @brief 数学構造体定義や演算子のオーバーロード
 */
#define _USE_MATH_DEFINES
#include <cmath>
#include <assert.h>
#include <numbers>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <DirectXMath.h>
using namespace DirectX;

#pragma region	struct定義

struct Vector2 {
	float x = 0.0f;
	float y = 0.0f;
};
struct Vector3 {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
};
struct Vector4 {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 0.0f;
};
struct Matrix4x4 {
	float m[4][4];
};
struct Quaternion {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 1.0f;
};

struct Sphere {
	Vector3 center{};
	float radius = 0.0f;
};
struct Line {
	Vector3 origin{}; //始点
	Vector3 diff{};//終点への差分ベクトル
};
struct Ray {
	Vector3 origin{}; //始点
	Vector3 diff{};//終点への差分ベクトル
};
struct Segment {
	Vector3 origin = {0.0f,0.0f,0.0f}; //始点
	Vector3 diff = {0.0f,1.0f,0.0f};//終点への差分ベクトル
};
struct OBB {
	Vector3 center{};          // 中心点
	Vector3 orientations[3]{}; // 座標軸、正規化、直交座標
	Vector3 size{};            // 座標軸方向の長さの半分。中心から面までの距離
};
struct Plane {
	Vector3 normal{};//法線
	float distance = 0.0f;//距離
};
struct Frustum {
	Plane plane[6]{};
	Vector3 vertex[8]{};
};
struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};
#pragma endregion


#pragma region 数学関係関数
//Clamp
inline float Clamp(float num, float min, float max) {
	if (num < min) {
		return min;
	}
	if (num > max) {
		return max;
	}
	return num;
}
//値を近づける
inline bool CloseValue(float& num, float goal, float speed) {
	if (std::fabs(num - goal) < std::fabs(speed)) {
		num = goal;
		return true;
	}
	if (num < goal) {
		num += speed;
	}
	if (num > goal) {
		num -= speed;
	}
	return false;
}
//ラジアン度数法変換
inline float Radian(float degree) { return degree * std::numbers::pi_v<float> / 180.0f; }
inline float Degree(float radian) { return radian * 180.0f / std::numbers::pi_v<float>; }
//ランダム値出力
inline int Rand(int min, int max) {
	if (min == 0 && max == 0) {
		return 0;
	}
	return min + (int)(rand() * (max - min + 1.0) / (1.0 + RAND_MAX));
}
inline float Rand(float min, float max) {
	if (min == 0 && max == 0) {
		return 0;
	}
	int kmin = int(min * 10000);
	int kmax = int(max * 10000);

	float result = (kmin + (int)(rand() * (kmax - kmin + 1.0) / (1.0 + RAND_MAX))) / 10000.0f;
	return result;
}
inline bool Rand() { return bool(0 + (int)(rand() * (1 - 0 + 1.0) / (1.0 + RAND_MAX))); }

inline Vector3 Rand(const Vector3& min, const Vector3& max) {
	return Vector3{ Rand(min.x,max.x),Rand(min.y,max.y) ,Rand(min.z,max.z) };
}
//時間のシード入力
inline void SRAND() {
	srand((unsigned)time(NULL));
}
//HSVAからRGBA変換
inline Vector4 HSVAtoRGBA(float h, float s, float v, float a) {
	float r = v, g = v, b = v;
	if (s > 0.0f) {
		h = std::fmod(h, 1.0f);
		h *= 6.0f;
		int32_t i = int32_t(h);
		float f = h - float(i);
		switch (i) {
		default:
		case 0:
			g *= 1.0f - s * (1.0f - f);
			b *= 1.0f - s;
			break;
		case 1:
			r *= 1.0f - s * f;
			b *= 1.0f - s;
			break;
		case 2:
			r *= 1.0f - s;
			b *= 1.0f - s * (1.0f - f);
			break;
		case 3:
			r *= 1.0f - s;
			g *= 1.0f - s * f;
			break;
		case 4:
			r *= 1.0f - s * (1.0f - f);
			g *= 1.0f - s;
			break;
		case 5:
			g *= 1.0f - s;
			b *= 1.0f - s * f;
			break;
		}
	}
	return { r, g, b, a };
}
//HSVAからRGBA変換
inline Vector4 HSVAtoRGBA(const Vector4& rgba) {
	return HSVAtoRGBA(rgba.x, rgba.y, rgba.z, rgba.w);
};
//RGBAからHSVA
inline Vector4 RGBAtoHSVA(const Vector4& rgba) {
	float max = (std::max)({ rgba.x, rgba.y, rgba.z });
	float min = (std::min)({ rgba.x, rgba.y, rgba.z });
	float delta = max - min;

	float h = 0.0f, s = 0.0f, v = max;

	if (delta > 0.0f) {
		s = delta / max;

		if (rgba.x == max) {
			h = (rgba.y - rgba.z) / delta + (rgba.y < rgba.z ? 6.0f : 0.0f);
		}
		else if (rgba.y == max) {
			h = (rgba.z - rgba.x) / delta + 2.0f;
		}
		else {
			h = (rgba.x - rgba.y) / delta + 4.0f;
		}

		h /= 6.0f;  // 正規化
	}

	return { h, s, v, rgba.w };
}
//ColorCodeからRGB出力
inline Vector3 ColorCodeToVector3(uint32_t colorCode) {
	float r = ((colorCode >> 16) & 0xFF) / 255.0f;
	float g = ((colorCode >> 8) & 0xFF) / 255.0f;
	float b = (colorCode & 0xFF) / 255.0f;
	return Vector3(r, g, b);
}

#pragma endregion


#pragma region	Vector2
#pragma region 演算子のオーバーロード
#pragma endregion
inline float Cross(Vector2 v1, Vector2 v2) {
	return v1.x * v2.y - v1.y * v2.x;
}
#pragma endregion
#pragma region	Vector3
#pragma region 演算子のオーバーロード
//加算
inline Vector3 Add(const Vector3& v1, const Vector3& v2) {
	Vector3 tmp;
	tmp.x = v1.x + v2.x;
	tmp.y = v1.y + v2.y;
	tmp.z = v1.z + v2.z;
	return tmp;
}
inline Vector3 operator +(const Vector3& v1, const Vector3& v2) {
	return{ v1.x + v2.x,v1.y + v2.y,v1.z + v2.z };
}
inline Vector3 operator +(const Vector3& v1, const float& scalar) {
	return{ v1.x + scalar,v1.y + scalar,v1.z + scalar };
}
//減算
inline Vector3 Subtract(const Vector3& v1, const Vector3& v2) {
	Vector3 tmp;
	tmp.x = v1.x - v2.x;
	tmp.y = v1.y - v2.y;
	tmp.z = v1.z - v2.z;
	return tmp;
}
inline Vector3 operator -(const Vector3& v1, const Vector3& v2) {
	return{ v1.x - v2.x,v1.y - v2.y,v1.z - v2.z };
}
inline Vector3 operator -(const Vector3& v1, const float& scalar) {
	return{ v1.x - scalar,v1.y - scalar,v1.z - scalar };
}
//スカラー倍
inline Vector3 Multiply(float scalar, const Vector3& v) {
	Vector3 tmp;
	tmp.x = v.x * scalar;
	tmp.y = v.y * scalar;
	tmp.z = v.z * scalar;
	return tmp;
}
inline Vector3 operator *(const Vector3& v, const float& scalar) {
	return{ v.x * scalar,v.y * scalar,v.z * scalar };
}
inline Vector3 operator *(const float& scalar, const Vector3& v) {
	return{ v.x * scalar,v.y * scalar,v.z * scalar };
}
//スカラー割り算
inline Vector3 operator /(const Vector3& v, const float& scalar) {
	return{ v.x / scalar,v.y / scalar,v.z / scalar };
}
inline Vector3 operator /(const float& scalar, const Vector3& v) {
	return{ v.x / scalar,v.y / scalar,v.z / scalar };
}
inline Vector3& operator+=(Vector3& v1, const Vector3& v2) {
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
	return v1;
}
inline Vector3& operator-=(Vector3& v1, const Vector3& v2) {
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;
	return v1;
}

inline Vector3 operator -(const Vector3& v1) {
	return{ -v1.x,-v1.y,-v1.z };
}
#pragma endregion

//内積
inline float Dot(const Vector3& v1, const Vector3& v2) {
	return{ v1.x * v2.x + v1.y * v2.y + v1.z * v2.z };
}
//長さ
inline float Length(const Vector3& v) {
	float tmp = v.x * v.x + v.y * v.y + v.z * v.z;
	tmp = sqrtf(tmp);
	return tmp;
}
//距離
inline float Distance(const Vector3& start, const Vector3& end) noexcept {
	return Length((end - start));
}
//正規化
inline Vector3 Normalize(const Vector3& v) {
	float tmp = v.x * v.x + v.y * v.y + v.z * v.z;
	tmp = sqrtf(tmp);
	return { v.x / tmp, v.y / tmp, v.z / tmp };
}
//外積
inline Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	return { (v1.y * v2.z - v1.z * v2.y),(v1.z * v2.x - v1.x * v2.z),(v1.x * v2.y - v1.y * v2.x) };
}
//三点の三角形から法線出力
inline Vector3 MakeNormal(const Vector3& v1, const Vector3& v2, const Vector3& v3) {
	Vector3 start = v2 - v1;
	Vector3 end = v3 - v2;

	Vector3 normal = Normalize(Cross(start, end));
	return normal;
}
//v1をv2に投影
inline Vector3 Project(const Vector3& v1, const Vector3 v2) {
	Vector3 norm = Normalize(v2);
	Vector3 result = norm * (Dot(v1, norm));
	return result;
}
//セグメント上にあるポイントから一番近い点
inline Vector3 ClosestPoint(const Vector3& point, const Segment& segment) {
	Vector3 result = segment.origin + Project(point - segment.origin, segment.diff);
	return result;
}
//成す角の角度
inline float Angle(const Vector3& from, const Vector3& to) {
	float dot = Dot(Normalize(from), Normalize(to));
	if (dot >= 1.0f) {
		return 0.0f;
	}
	if (dot <= -1.0f) {
		return Radian(180.0f);
	}
	return std::acosf(dot);
}
//軸の角度算出
inline float SignedAngle(const Vector3& from, const Vector3& to, const Vector3& axis) {
	return Dot(Cross(from, to), axis) < 0.0f ? Angle(from, to) : -Angle(from, to);
}


// 線形補間
inline Vector3 Lerp(float t,const Vector3& v1, const Vector3& v2) {
	Vector3 result = v1 + (v2 - v1) * t;
	return result;
}
// 球面線形補間
inline Vector3 Slerp(float t,const Vector3& v1, const Vector3& v2) {

	const float epsilon = 0.0001f;

	Vector3 a = Normalize(v1);
	Vector3 b = Normalize(v2);

	float dot = Dot(a, b);

	if (std::abs(dot - 1.0f) < epsilon) {
		return a;
	}
	else if (std::abs(dot + 1.0f) < epsilon) {
		return Lerp(t, v1, v2);
	}

	float theta = std::acos(dot);

	float sinTheta = std::sin(theta);
	float factorA = std::sin((1.0f - t) * theta) / sinTheta;
	float factorB = std::sin(t * theta) / sinTheta;

	return Vector3{
		factorA * a.x + factorB * b.x, factorA * a.y + factorB * b.y,
		factorA * a.z + factorB * b.z };
}
//キャトムルロム曲線
inline Vector3 CatmullRomSpline(float t, const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3) {
	return
		0.5f * ((-p0 + 3.0f * p1 - 3.0f * p2 + p3) * std::pow(t, 3.0f)
			+ (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * std::pow(t, 2.0f)
			+ (-p0 + p2) * t
			+ 2.0f * p1);
}
//二次ベジエ曲線
inline Vector3 QuadraticBezierCurve(float t, const Vector3& p0, const Vector3& p1, const Vector3& p2) {
	float s = 1.0f - t;
	return
		s * s * p0 +
		2.0f * t * s * p1 +
		t * t * p2;
}
//三次ベジエ曲線
inline Vector3 CubicBezierCurve(float t, const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3) {
	float s = 1.0f - t;
	return
		(s * s * s * p0) +
		(3.0f * s * s * t * p1) +
		(3.0f * s * t * t * p2) +
		t * t * t * p3;
}
//Y消去
inline Vector3 LoseY(Vector3 loseY) { return Vector3{ loseY.x, 0.0f, loseY.z }; }
//球からランダムな位置生成
inline Vector3 MakeRandVector3(const Sphere& sphere) {
	Vector3 randomVector = Normalize(Vector3{ Rand(-1.0f,1.0f),Rand(-1.0f,1.0f) ,Rand(-1.0f,1.0f) });
	return randomVector + sphere.center;
}

#pragma endregion
#pragma region	Vector4
#pragma region 演算子のオーバーロード
//加算
inline Vector4 Add(const Vector4& v1, const Vector4& v2) {
	Vector4 tmp;
	tmp.x = v1.x + v2.x;
	tmp.y = v1.y + v2.y;
	tmp.z = v1.z + v2.z;
	tmp.w = v1.w + v2.w;
	return tmp;
}
inline Vector4 operator +(const Vector4& v1, const Vector4& v2) {
	return{ v1.x + v2.x,v1.y + v2.y,v1.z + v2.z ,v1.w + v2.w };
}
inline Vector4 operator +(const Vector4& v1, const float& scalar) {
	return{ v1.x + scalar,v1.y + scalar,v1.z + scalar,v1.w + scalar };
}
//減算
inline Vector4 Subtract(const Vector4& v1, const Vector4& v2) {
	Vector4 tmp;
	tmp.x = v1.x - v2.x;
	tmp.y = v1.y - v2.y;
	tmp.z = v1.z - v2.z;
	tmp.w = v1.z - v2.w;
	return tmp;
}
inline Vector4 operator -(const Vector4& v1, const Vector4& v2) {
	return{ v1.x - v2.x,v1.y - v2.y,v1.z - v2.z,v1.w - v2.w };
}
inline Vector4 operator -(const Vector4& v1, const float& scalar) {
	return{ v1.x - scalar,v1.y - scalar,v1.z - scalar,v1.w - scalar };
}

//スカラー倍
inline Vector4 Multiply(float scalar, const Vector4& v) {
	Vector4 tmp;
	tmp.x = v.x * scalar;
	tmp.y = v.y * scalar;
	tmp.z = v.z * scalar;
	tmp.w = v.w * scalar;
	return tmp;
}
inline Vector4 operator *(const Vector4& v, const float& scalar) {
	return{ v.x * scalar,v.y * scalar,v.z * scalar,v.w * scalar };
}
inline Vector4 operator *(const float& scalar, const Vector4& v) {
	return{ v.x * scalar,v.y * scalar,v.z * scalar,v.w * scalar };
}
//スカラー割り算
inline Vector4 operator /(const Vector4& v, const float& scalar) {
	return{ v.x / scalar,v.y / scalar,v.z / scalar,v.w / scalar };
}
inline Vector4 operator /(const float& scalar, const Vector4& v) {
	return{ v.x / scalar,v.y / scalar,v.z / scalar,v.w / scalar };
}
inline Vector4& operator+=(Vector4& v1, const Vector4& v2) {
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
	v1.w += v2.w;
	return v1;
}
inline Vector4& operator-=(Vector4& v1, const Vector4& v2) {
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;
	v1.w -= v2.w;
	return v1;
}

inline Vector4 operator -(const Vector4& v1) {
	return{ -v1.x,-v1.y,-v1.z,-v1.w };
}
#pragma endregion
// 線形補間
inline Vector4 Lerp(float t, const Vector4& v1, const Vector4& v2) {
	Vector4 result = v1 + (v2 - v1) * t;
	return result;
}

#pragma endregion
#pragma region	Matrix4x4
#pragma region 演算子のオーバーロード
//初期化マトリックス
inline Matrix4x4 MakeIdentity4x4() {
	Matrix4x4 tmp;
	tmp.m[0][0] = 1.0f;
	tmp.m[0][1] = 0.0f;
	tmp.m[0][2] = 0.0f;
	tmp.m[0][3] = 0.0f;

	tmp.m[1][0] = 0.0f;
	tmp.m[1][1] = 1.0f;
	tmp.m[1][2] = 0.0f;
	tmp.m[1][3] = 0.0f;

	tmp.m[2][0] = 0.0f;
	tmp.m[2][1] = 0.0f;
	tmp.m[2][2] = 1.0f;
	tmp.m[2][3] = 0.0f;

	tmp.m[3][0] = 0.0f;
	tmp.m[3][1] = 0.0f;
	tmp.m[3][2] = 0.0f;
	tmp.m[3][3] = 1.0f;

	return tmp;
}
//加算
inline Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 tmp;
	tmp.m[0][0] = m1.m[0][0] + m2.m[0][0];
	tmp.m[0][1] = m1.m[0][1] + m2.m[0][1];
	tmp.m[0][2] = m1.m[0][2] + m2.m[0][2];
	tmp.m[0][3] = m1.m[0][3] + m2.m[0][3];
	tmp.m[1][0] = m1.m[1][0] + m2.m[1][0];
	tmp.m[1][1] = m1.m[1][1] + m2.m[1][1];
	tmp.m[1][2] = m1.m[1][2] + m2.m[1][2];
	tmp.m[1][3] = m1.m[1][3] + m2.m[1][3];
	tmp.m[2][0] = m1.m[2][0] + m2.m[2][0];
	tmp.m[2][1] = m1.m[2][1] + m2.m[2][1];
	tmp.m[2][2] = m1.m[2][2] + m2.m[2][2];
	tmp.m[2][3] = m1.m[2][3] + m2.m[2][3];
	tmp.m[3][0] = m1.m[3][0] + m2.m[3][0];
	tmp.m[3][1] = m1.m[3][1] + m2.m[3][1];
	tmp.m[3][2] = m1.m[3][2] + m2.m[3][2];
	tmp.m[3][3] = m1.m[3][3] + m2.m[3][3];
	return tmp;

}
inline Matrix4x4 operator +(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 tmp;
	tmp.m[0][0] = m1.m[0][0] + m2.m[0][0];
	tmp.m[0][1] = m1.m[0][1] + m2.m[0][1];
	tmp.m[0][2] = m1.m[0][2] + m2.m[0][2];
	tmp.m[0][3] = m1.m[0][3] + m2.m[0][3];
	tmp.m[1][0] = m1.m[1][0] + m2.m[1][0];
	tmp.m[1][1] = m1.m[1][1] + m2.m[1][1];
	tmp.m[1][2] = m1.m[1][2] + m2.m[1][2];
	tmp.m[1][3] = m1.m[1][3] + m2.m[1][3];
	tmp.m[2][0] = m1.m[2][0] + m2.m[2][0];
	tmp.m[2][1] = m1.m[2][1] + m2.m[2][1];
	tmp.m[2][2] = m1.m[2][2] + m2.m[2][2];
	tmp.m[2][3] = m1.m[2][3] + m2.m[2][3];
	tmp.m[3][0] = m1.m[3][0] + m2.m[3][0];
	tmp.m[3][1] = m1.m[3][1] + m2.m[3][1];
	tmp.m[3][2] = m1.m[3][2] + m2.m[3][2];
	tmp.m[3][3] = m1.m[3][3] + m2.m[3][3];
	return tmp;
}
//減算
inline Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 tmp;
	tmp.m[0][0] = m1.m[0][0] - m2.m[0][0];
	tmp.m[0][1] = m1.m[0][1] - m2.m[0][1];
	tmp.m[0][2] = m1.m[0][2] - m2.m[0][2];
	tmp.m[0][3] = m1.m[0][3] - m2.m[0][3];
	tmp.m[1][0] = m1.m[1][0] - m2.m[1][0];
	tmp.m[1][1] = m1.m[1][1] - m2.m[1][1];
	tmp.m[1][2] = m1.m[1][2] - m2.m[1][2];
	tmp.m[1][3] = m1.m[1][3] - m2.m[1][3];
	tmp.m[2][0] = m1.m[2][0] - m2.m[2][0];
	tmp.m[2][1] = m1.m[2][1] - m2.m[2][1];
	tmp.m[2][2] = m1.m[2][2] - m2.m[2][2];
	tmp.m[2][3] = m1.m[2][3] - m2.m[2][3];
	tmp.m[3][0] = m1.m[3][0] - m2.m[3][0];
	tmp.m[3][1] = m1.m[3][1] - m2.m[3][1];
	tmp.m[3][2] = m1.m[3][2] - m2.m[3][2];
	tmp.m[3][3] = m1.m[3][3] - m2.m[3][3];
	return tmp;
}
inline Matrix4x4 operator -(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 tmp;
	tmp.m[0][0] = m1.m[0][0] - m2.m[0][0];
	tmp.m[0][1] = m1.m[0][1] - m2.m[0][1];
	tmp.m[0][2] = m1.m[0][2] - m2.m[0][2];
	tmp.m[0][3] = m1.m[0][3] - m2.m[0][3];
	tmp.m[1][0] = m1.m[1][0] - m2.m[1][0];
	tmp.m[1][1] = m1.m[1][1] - m2.m[1][1];
	tmp.m[1][2] = m1.m[1][2] - m2.m[1][2];
	tmp.m[1][3] = m1.m[1][3] - m2.m[1][3];
	tmp.m[2][0] = m1.m[2][0] - m2.m[2][0];
	tmp.m[2][1] = m1.m[2][1] - m2.m[2][1];
	tmp.m[2][2] = m1.m[2][2] - m2.m[2][2];
	tmp.m[2][3] = m1.m[2][3] - m2.m[2][3];
	tmp.m[3][0] = m1.m[3][0] - m2.m[3][0];
	tmp.m[3][1] = m1.m[3][1] - m2.m[3][1];
	tmp.m[3][2] = m1.m[3][2] - m2.m[3][2];
	tmp.m[3][3] = m1.m[3][3] - m2.m[3][3];
	return tmp;
}
//乗算
inline Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 tmp;
	tmp.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] + m1.m[0][3] * m2.m[3][0];
	tmp.m[0][1] = m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] + m1.m[0][3] * m2.m[3][1];
	tmp.m[0][2] = m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] + m1.m[0][3] * m2.m[3][2];
	tmp.m[0][3] = m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] + m1.m[0][3] * m2.m[3][3];

	tmp.m[1][0] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] + m1.m[1][3] * m2.m[3][0];
	tmp.m[1][1] = m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] + m1.m[1][3] * m2.m[3][1];
	tmp.m[1][2] = m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] + m1.m[1][3] * m2.m[3][2];
	tmp.m[1][3] = m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] + m1.m[1][3] * m2.m[3][3];

	tmp.m[2][0] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] + m1.m[2][3] * m2.m[3][0];
	tmp.m[2][1] = m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] + m1.m[2][3] * m2.m[3][1];
	tmp.m[2][2] = m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] + m1.m[2][3] * m2.m[3][2];
	tmp.m[2][3] = m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] + m1.m[2][3] * m2.m[3][3];

	tmp.m[3][0] = m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] + m1.m[3][3] * m2.m[3][0];
	tmp.m[3][1] = m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] + m1.m[3][3] * m2.m[3][1];
	tmp.m[3][2] = m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] + m1.m[3][3] * m2.m[3][2];
	tmp.m[3][3] = m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] + m1.m[3][3] * m2.m[3][3];
	return tmp;
}
inline Matrix4x4 operator *(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 tmp;
	tmp.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] + m1.m[0][3] * m2.m[3][0];
	tmp.m[0][1] = m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] + m1.m[0][3] * m2.m[3][1];
	tmp.m[0][2] = m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] + m1.m[0][3] * m2.m[3][2];
	tmp.m[0][3] = m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] + m1.m[0][3] * m2.m[3][3];

	tmp.m[1][0] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] + m1.m[1][3] * m2.m[3][0];
	tmp.m[1][1] = m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] + m1.m[1][3] * m2.m[3][1];
	tmp.m[1][2] = m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] + m1.m[1][3] * m2.m[3][2];
	tmp.m[1][3] = m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] + m1.m[1][3] * m2.m[3][3];

	tmp.m[2][0] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] + m1.m[2][3] * m2.m[3][0];
	tmp.m[2][1] = m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] + m1.m[2][3] * m2.m[3][1];
	tmp.m[2][2] = m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] + m1.m[2][3] * m2.m[3][2];
	tmp.m[2][3] = m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] + m1.m[2][3] * m2.m[3][3];

	tmp.m[3][0] = m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] + m1.m[3][3] * m2.m[3][0];
	tmp.m[3][1] = m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] + m1.m[3][3] * m2.m[3][1];
	tmp.m[3][2] = m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] + m1.m[3][3] * m2.m[3][2];
	tmp.m[3][3] = m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] + m1.m[3][3] * m2.m[3][3];
	return tmp;
}
//逆行列算出
inline Matrix4x4 Inverse(const Matrix4x4& m) {
	/*float lal = m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3]
		+ m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1]
		+ m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2]

		- m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1]
		- m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3]
		- m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2]

		- m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3]
		- m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1]
		- m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2]

		+ m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1]
		+ m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3]
		+ m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2]

		+ m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3]
		+ m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1]
		+ m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2]

		- m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1]
		- m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3]
		- m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2]

		- m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0]
		- m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0]
		- m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0]

		+ m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0]
		+ m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0]
		+ m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];

	Matrix4x4 tmp;
	tmp.m[0][0] = (m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[1][3] * m.m[2][1] * m.m[3][2]
		- m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2]) / lal;

	tmp.m[0][1] = (-m.m[0][1] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[2][1] * m.m[3][2]
		+ m.m[0][3] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2]) / lal;

	tmp.m[0][2] = (m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[3][2]
		- m.m[0][3] * m.m[1][2] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2]) / lal;

	tmp.m[0][3] = (-m.m[0][1] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][1] - m.m[0][3] * m.m[1][1] * m.m[2][2]
		+ m.m[0][3] * m.m[1][2] * m.m[2][1] + m.m[0][2] * m.m[1][1] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2]) / lal;


	tmp.m[1][0] = (-m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[1][3] * m.m[2][0] * m.m[3][2]
		+ m.m[1][3] * m.m[2][2] * m.m[3][0] + m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[1][0] * m.m[2][3] * m.m[3][2]) / lal;

	tmp.m[1][1] = (m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] + m.m[0][3] * m.m[2][0] * m.m[3][2]
		- m.m[0][3] * m.m[2][2] * m.m[3][0] - m.m[0][2] * m.m[2][0] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][2]) / lal;

	tmp.m[1][2] = (-m.m[0][0] * m.m[1][2] * m.m[3][3] - m.m[0][2] * m.m[1][3] * m.m[3][0] - m.m[0][3] * m.m[1][0] * m.m[3][2]
		+ m.m[0][3] * m.m[1][2] * m.m[3][0] + m.m[0][2] * m.m[1][0] * m.m[3][3] + m.m[0][0] * m.m[1][3] * m.m[3][2]) / lal;

	tmp.m[1][3] = (m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] + m.m[0][3] * m.m[1][0] * m.m[2][2]
		- m.m[0][3] * m.m[1][2] * m.m[2][0] - m.m[0][2] * m.m[1][0] * m.m[2][3] - m.m[0][0] * m.m[1][3] * m.m[2][2]) / lal;


	tmp.m[2][0] = (m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[1][3] * m.m[2][0] * m.m[3][1]
		- m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1]) / lal;

	tmp.m[2][1] = (-m.m[0][0] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][0] - m.m[0][3] * m.m[2][0] * m.m[3][1]
		+ m.m[0][3] * m.m[2][1] * m.m[3][0] + m.m[0][1] * m.m[2][0] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1]) / lal;

	tmp.m[2][2] = (m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] + m.m[0][3] * m.m[1][0] * m.m[3][1]
		- m.m[0][3] * m.m[1][1] * m.m[3][0] - m.m[0][1] * m.m[1][0] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1]) / lal;

	tmp.m[2][3] = (-m.m[0][0] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] - m.m[0][3] * m.m[1][0] * m.m[2][1]
		+ m.m[0][3] * m.m[1][1] * m.m[2][0] + m.m[0][1] * m.m[1][0] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1]) / lal;


	tmp.m[3][0] = (-m.m[1][0] * m.m[2][1] * m.m[3][2] - m.m[1][1] * m.m[2][2] * m.m[3][0] - m.m[1][2] * m.m[2][0] * m.m[3][1]
		+ m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[1][1] * m.m[2][0] * m.m[3][2] + m.m[1][0] * m.m[2][2] * m.m[3][1]) / lal;

	tmp.m[3][1] = (m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] + m.m[0][2] * m.m[2][0] * m.m[3][1]
		- m.m[0][2] * m.m[2][1] * m.m[3][0] - m.m[0][1] * m.m[2][0] * m.m[3][2] - m.m[0][0] * m.m[2][2] * m.m[3][1]) / lal;

	tmp.m[3][2] = (-m.m[0][0] * m.m[1][1] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[3][0] - m.m[0][2] * m.m[1][0] * m.m[3][1]
		+ m.m[0][2] * m.m[1][1] * m.m[3][0] + m.m[0][1] * m.m[1][0] * m.m[3][2] + m.m[0][0] * m.m[1][2] * m.m[3][1]) / lal;

	tmp.m[3][3] = (m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] + m.m[0][2] * m.m[1][0] * m.m[2][1]
		- m.m[0][2] * m.m[1][1] * m.m[2][0] - m.m[0][1] * m.m[1][0] * m.m[2][2] - m.m[0][0] * m.m[1][2] * m.m[2][1]) / lal;

	return tmp;*/

	// DirectXMathのXMMATRIXに変換
	XMMATRIX xmMat = XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(&m));

	// 行列の逆行列を計算
	XMVECTOR det;
	XMMATRIX xmInverseMat = XMMatrixInverse(&det, xmMat);

	// 結果を自作のMatrix4x4に変換
	Matrix4x4 result;
	XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&result), xmInverseMat);

	return result;
}
//横行列から縦行列変換
inline Matrix4x4 Transpose(const Matrix4x4& m) {
	Matrix4x4 tmp;
	tmp.m[0][0] = m.m[0][0];
	tmp.m[0][1] = m.m[1][0];
	tmp.m[0][2] = m.m[2][0];
	tmp.m[0][3] = m.m[3][0];

	tmp.m[1][0] = m.m[0][1];
	tmp.m[1][1] = m.m[1][1];
	tmp.m[1][2] = m.m[2][1];
	tmp.m[1][3] = m.m[3][1];

	tmp.m[2][0] = m.m[0][2];
	tmp.m[2][1] = m.m[1][2];
	tmp.m[2][2] = m.m[2][2];
	tmp.m[2][3] = m.m[3][2];

	tmp.m[3][0] = m.m[0][3];
	tmp.m[3][1] = m.m[1][3];
	tmp.m[3][2] = m.m[2][3];
	tmp.m[3][3] = m.m[3][3];

	return tmp;
}

inline Vector3 operator *(const Vector3& v, const Matrix4x4& m) {

	Vector3 result;
	result.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + 1.0f * m.m[3][0];
	result.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + 1.0f * m.m[3][1];
	result.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + 1.0f * m.m[3][2];
	float w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + 1.0f * m.m[3][3];

	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;

	return result;
}

inline Matrix4x4& operator*=(Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 tmp;
	tmp.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] +
		m1.m[0][3] * m2.m[3][0];
	tmp.m[0][1] = m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] +
		m1.m[0][3] * m2.m[3][1];
	tmp.m[0][2] = m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] +
		m1.m[0][3] * m2.m[3][2];
	tmp.m[0][3] = m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] +
		m1.m[0][3] * m2.m[3][3];

	tmp.m[1][0] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] +
		m1.m[1][3] * m2.m[3][0];
	tmp.m[1][1] = m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] +
		m1.m[1][3] * m2.m[3][1];
	tmp.m[1][2] = m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] +
		m1.m[1][3] * m2.m[3][2];
	tmp.m[1][3] = m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] +
		m1.m[1][3] * m2.m[3][3];

	tmp.m[2][0] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] +
		m1.m[2][3] * m2.m[3][0];
	tmp.m[2][1] = m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] +
		m1.m[2][3] * m2.m[3][1];
	tmp.m[2][2] = m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] +
		m1.m[2][3] * m2.m[3][2];
	tmp.m[2][3] = m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] +
		m1.m[2][3] * m2.m[3][3];

	tmp.m[3][0] = m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] +
		m1.m[3][3] * m2.m[3][0];
	tmp.m[3][1] = m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] +
		m1.m[3][3] * m2.m[3][1];
	tmp.m[3][2] = m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] +
		m1.m[3][3] * m2.m[3][2];
	tmp.m[3][3] = m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] +
		m1.m[3][3] * m2.m[3][3];
	m1 = tmp;
	return m1;


}

#pragma endregion
#pragma region Vector3算出
//軸出力
inline Vector3 GetXAxis(Matrix4x4 m) {

	return { m.m[0][0],m.m[0][1],m.m[0][2] };
}
inline Vector3 GetYAxis(Matrix4x4 m) {

	return { m.m[1][0],m.m[1][1],m.m[1][2] };
}
inline Vector3 GetZAxis(Matrix4x4 m) {

	return { m.m[2][0],m.m[2][1],m.m[2][2] };
}
//軸入力
inline void SetXAxis(Matrix4x4& m, Vector3 v) {
	m.m[0][0] = v.x;
	m.m[0][1] = v.y;
	m.m[0][2] = v.z;
}
inline void SetYAxis(Matrix4x4& m, Vector3 v) {
	m.m[1][0] = v.x;
	m.m[1][1] = v.y;
	m.m[1][2] = v.z;
}
inline void SetZAxis(Matrix4x4& m, Vector3 v) {
	m.m[2][0] = v.x;
	m.m[2][1] = v.y;
	m.m[2][2] = v.z;
}
//Scale要素出力
inline Vector3 MakeScale(const Matrix4x4& matrix) {
	Vector3 scaleX = { matrix.m[0][0],matrix.m[0][1],matrix.m[0][2] };
	Vector3 scaleY = { matrix.m[1][0],matrix.m[1][1],matrix.m[1][2] };
	Vector3 scaleZ = { matrix.m[2][0],matrix.m[2][1],matrix.m[2][2] };
	Vector3 result;
	result.x = Length(scaleX);
	result.y = Length(scaleY);
	result.z = Length(scaleZ);
	return result;
}
//Translation要素出力
inline Vector3 MakeTranslation(const Matrix4x4& matrix) {
	Vector3 result;
	result.x = matrix.m[3][0];
	result.y = matrix.m[3][1];
	result.z = matrix.m[3][2];
	return result;
}
//オイラー角出力
inline Vector3 MakeEulerAngle(const Matrix4x4& matrix) {
	Vector3 angles;

	// Extract yaw (Y軸回りの回転)
	angles.y = atan2(matrix.m[2][0], matrix.m[2][2]);

	// Extract pitch (X軸回りの回転)
	float sinPitch = -matrix.m[2][1];
	if (fabs(sinPitch) >= 1.0f) {
		angles.x = copysign(3.14159265f / 2.0f, sinPitch);
	}
	else {
		angles.x = asin(sinPitch);
	}

	// Extract roll (Z軸回りの回転)
	angles.z = atan2(matrix.m[0][1], matrix.m[1][1]);

	return angles;
}
//ベクトルにマトリックス変換適用
inline Vector3 TransformVector(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result;
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];

	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;

	return result;

}
#pragma endregion
#pragma region ファクトリ関数
//回転行列生成
inline Matrix4x4 NormalizeMakeRotateMatrix(const Matrix4x4& matrix) {
	Vector3 xAxis = Normalize(GetXAxis(matrix)); // [0][?]
	Vector3 yAxis = Normalize(GetYAxis(matrix)); // [1][?]
	Vector3 zAxis = Normalize(GetZAxis(matrix)); // [2][?]
	Matrix4x4 result = MakeIdentity4x4();
	result.m[0][0] = xAxis.x;
	result.m[0][1] = xAxis.y;
	result.m[0][2] = xAxis.z;
	result.m[1][0] = yAxis.x;
	result.m[1][1] = yAxis.y;
	result.m[1][2] = yAxis.z;
	result.m[2][0] = zAxis.x;
	result.m[2][1] = zAxis.y;
	result.m[2][2] = zAxis.z;

	return result;
}
//移動行列生成
inline Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {
	Matrix4x4 tmp;
	tmp.m[0][0] = 1;
	tmp.m[0][1] = 0;
	tmp.m[0][2] = 0;
	tmp.m[0][3] = 0;

	tmp.m[1][0] = 0;
	tmp.m[1][1] = 1;
	tmp.m[1][2] = 0;
	tmp.m[1][3] = 0;

	tmp.m[2][0] = 0;
	tmp.m[2][1] = 0;
	tmp.m[2][2] = 1;
	tmp.m[2][3] = 0;

	tmp.m[3][0] = translate.x;
	tmp.m[3][1] = translate.y;
	tmp.m[3][2] = translate.z;
	tmp.m[3][3] = 1;

	return tmp;

}
//スケール行列生成
inline Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
	Matrix4x4 tmp;
	tmp.m[0][0] = scale.x;
	tmp.m[0][1] = 0;
	tmp.m[0][2] = 0;
	tmp.m[0][3] = 0;

	tmp.m[1][0] = 0;
	tmp.m[1][1] = scale.y;
	tmp.m[1][2] = 0;
	tmp.m[1][3] = 0;

	tmp.m[2][0] = 0;
	tmp.m[2][1] = 0;
	tmp.m[2][2] = scale.z;
	tmp.m[2][3] = 0;

	tmp.m[3][0] = 0;
	tmp.m[3][1] = 0;
	tmp.m[3][2] = 0;
	tmp.m[3][3] = 1;

	return tmp;
}
//X軸回転行列生成
inline Matrix4x4 MakeRotateXMatrix(float radian) {

	Matrix4x4 tmp;

	tmp.m[0][0] = 1;
	tmp.m[0][1] = 0;
	tmp.m[0][2] = 0;
	tmp.m[0][3] = 0;

	tmp.m[1][0] = 0;
	tmp.m[1][1] = std::cos(radian);
	tmp.m[1][2] = std::sin(radian);
	tmp.m[1][3] = 0;

	tmp.m[2][0] = 0;
	tmp.m[2][1] = -std::sin(radian);
	tmp.m[2][2] = std::cos(radian);
	tmp.m[2][3] = 0;

	tmp.m[3][0] = 0;
	tmp.m[3][1] = 0;
	tmp.m[3][2] = 0;
	tmp.m[3][3] = 1;

	return tmp;
}
//Y軸回転行列生成
inline Matrix4x4 MakeRotateYMatrix(float radian) {

	Matrix4x4 tmp;

	tmp.m[0][0] = std::cos(radian);
	tmp.m[0][1] = 0;
	tmp.m[0][2] = -std::sin(radian);
	tmp.m[0][3] = 0;

	tmp.m[1][0] = 0;
	tmp.m[1][1] = 1;
	tmp.m[1][2] = 0;
	tmp.m[1][3] = 0;

	tmp.m[2][0] = std::sin(radian);
	tmp.m[2][1] = 0;
	tmp.m[2][2] = std::cos(radian);
	tmp.m[2][3] = 0;

	tmp.m[3][0] = 0;
	tmp.m[3][1] = 0;
	tmp.m[3][2] = 0;
	tmp.m[3][3] = 1;

	return tmp;
}
//Z軸回転行列生成
inline Matrix4x4 MakeRotateZMatrix(float radian) {

	Matrix4x4 tmp;

	tmp.m[0][0] = std::cos(radian);
	tmp.m[0][1] = std::sin(radian);
	tmp.m[0][2] = 0;
	tmp.m[0][3] = 0;

	tmp.m[1][0] = -sinf(radian);
	tmp.m[1][1] = std::cos(radian);
	tmp.m[1][2] = 0;
	tmp.m[1][3] = 0;

	tmp.m[2][0] = 0;
	tmp.m[2][1] = 0;
	tmp.m[2][2] = 1;
	tmp.m[2][3] = 0;

	tmp.m[3][0] = 0;
	tmp.m[3][1] = 0;
	tmp.m[3][2] = 0;
	tmp.m[3][3] = 1;

	return tmp;
}
//XYZ回転行列生成
inline Matrix4x4 MakeRotateXYZMatrix(const Vector3& radian) {

	return MakeRotateXMatrix(radian.x) * MakeRotateYMatrix(radian.y) * MakeRotateZMatrix(radian.z);;
}
//Quaternionから回転行列生成
inline Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion) {
	Matrix4x4 result;
	result.m[0][0] = quaternion.w * quaternion.w + quaternion.x * quaternion.x - quaternion.y * quaternion.y - quaternion.z * quaternion.z;
	result.m[0][1] = 2 * (quaternion.x * quaternion.y + quaternion.w * quaternion.z);
	result.m[0][2] = 2 * (quaternion.x * quaternion.z - quaternion.w * quaternion.y);
	result.m[0][3] = 0;

	result.m[1][0] = 2 * (quaternion.x * quaternion.y - quaternion.w * quaternion.z);
	result.m[1][1] = quaternion.w * quaternion.w - quaternion.x * quaternion.x + quaternion.y * quaternion.y - quaternion.z * quaternion.z;
	result.m[1][2] = 2 * (quaternion.y * quaternion.z + quaternion.w * quaternion.x);
	result.m[1][3] = 0;

	result.m[2][0] = 2 * (quaternion.x * quaternion.z + quaternion.w * quaternion.y);
	result.m[2][1] = 2 * (quaternion.y * quaternion.z - quaternion.w * quaternion.x);
	result.m[2][2] = quaternion.w * quaternion.w - quaternion.x * quaternion.x - quaternion.y * quaternion.y + quaternion.z * quaternion.z;
	result.m[2][3] = 0;

	result.m[3][0] = 0;
	result.m[3][1] = 0;
	result.m[3][2] = 0;
	result.m[3][3] = 1;

	return result;
}
//二つのベクトルから回転行列生成
inline Matrix4x4 DirectionToDirection(const Vector3& from, const Vector3& to) {
	Vector3 normalizeFrom = Normalize(from);
	Vector3 normalizeTo = Normalize(to);

	Vector3 cross = Cross(normalizeFrom, normalizeTo);
	float cos = Dot(normalizeFrom, normalizeTo);
	float sin = Length(cross);

	Vector3 n = { 0.0f,0.0f,0.0f };

	if (Length(cross) > 0) {
		n = Normalize(cross);
	}
	else if (normalizeFrom.x != 0.0f || normalizeFrom.z != 0.0f) {
		n.x = normalizeFrom.y;
		n.y = -normalizeFrom.x;
		n.z = 0.0f;
	}
	else if (normalizeFrom.x != 0.0f || normalizeFrom.y != 0.0f) {
		n.x = normalizeFrom.z;
		n.y = 0.0f;
		n.z = -normalizeFrom.x;

	}

	Matrix4x4 RotateMatrix = MakeIdentity4x4();
	RotateMatrix.m[0][0] = n.x * n.x * (1.0f - cos) + cos;
	RotateMatrix.m[0][1] = n.x * n.y * (1.0f - cos) + n.z * sin;
	RotateMatrix.m[0][2] = n.x * n.z * (1.0f - cos) - n.y * sin;

	RotateMatrix.m[1][0] = n.x * n.y * (1.0f - cos) - n.z * sin;
	RotateMatrix.m[1][1] = n.y * n.y * (1.0f - cos) + cos;
	RotateMatrix.m[1][2] = n.y * n.z * (1.0f - cos) + n.x * sin;

	RotateMatrix.m[2][0] = n.x * n.z * (1.0f - cos) + n.y * sin;
	RotateMatrix.m[2][1] = n.y * n.z * (1.0f - cos) - n.x * sin;
	RotateMatrix.m[2][2] = n.z * n.z * (1.0f - cos) + cos;

	return RotateMatrix;
}
//軸と角度から回転行列生成
inline Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle) {
	Matrix4x4 RotateMatrix = MakeIdentity4x4();
	RotateMatrix.m[0][0] = axis.x * axis.x * (1.0f - std::cosf(angle)) + std::cosf(angle);
	RotateMatrix.m[0][1] = axis.x * axis.y * (1.0f - std::cosf(angle)) + axis.z * std::sinf(angle);
	RotateMatrix.m[0][2] = axis.x * axis.z * (1.0f - std::cosf(angle)) - axis.y * std::sinf(angle);

	RotateMatrix.m[1][0] = axis.x * axis.y * (1.0f - std::cosf(angle)) - axis.z * std::sinf(angle);
	RotateMatrix.m[1][1] = axis.y * axis.y * (1.0f - std::cosf(angle)) + std::cosf(angle);
	RotateMatrix.m[1][2] = axis.y * axis.z * (1.0f - std::cosf(angle)) + axis.x * std::sinf(angle);

	RotateMatrix.m[2][0] = axis.x * axis.z * (1.0f - std::cosf(angle)) + axis.y * std::sinf(angle);
	RotateMatrix.m[2][1] = axis.y * axis.z * (1.0f - std::cosf(angle)) - axis.x * std::sinf(angle);
	RotateMatrix.m[2][2] = axis.z * axis.z * (1.0f - std::cosf(angle)) + std::cosf(angle);

	return RotateMatrix;
}
//アフィン変換行列生成
inline Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);
	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);

	Matrix4x4 tmp = scaleMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * translateMatrix;

	return tmp;
}
//アフィン変換行列生成
inline Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Quaternion& rotate, const Vector3& translate) {
	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);
	Matrix4x4 rotateMatrix = MakeRotateMatrix(rotate);
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);

	Matrix4x4 tmp = scaleMatrix * rotateMatrix * translateMatrix;

	return tmp;
}

// 透視投影行列生成
inline Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 tmp;
	float cot = 1.0f / std::tan(fovY / 2.0f);
	tmp.m[0][0] = (1.0f / aspectRatio) * cot;
	tmp.m[0][1] = 0.0f;
	tmp.m[0][2] = 0.0f;
	tmp.m[0][3] = 0.0f;
	tmp.m[1][0] = 0.0f;
	tmp.m[1][1] = cot;
	tmp.m[1][2] = 0.0f;
	tmp.m[1][3] = 0.0f;
	tmp.m[2][0] = 0.0f;
	tmp.m[2][1] = 0.0f;
	tmp.m[2][2] = farClip / (farClip - nearClip);
	tmp.m[2][3] = 1.0f;
	tmp.m[3][0] = 0.0f;
	tmp.m[3][1] = 0.0f;
	tmp.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);
	tmp.m[3][3] = 0.0f;

	return tmp;
}
//正射影行列生成
inline Matrix4x4 MakeOrthograohicmatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	Matrix4x4 tmp;
	tmp.m[0][0] = 2.0f / (right - left);
	tmp.m[0][1] = 0;
	tmp.m[0][2] = 0;
	tmp.m[0][3] = 0;
	tmp.m[1][0] = 0;
	tmp.m[1][1] = 2.0f / (top - bottom);
	tmp.m[1][2] = 0;
	tmp.m[1][3] = 0;
	tmp.m[2][0] = 0;
	tmp.m[2][1] = 0;
	tmp.m[2][2] = 1.0f / (farClip - nearClip);
	tmp.m[2][3] = 0;
	tmp.m[3][0] = (left + right) / (left - right);
	tmp.m[3][1] = (top + bottom) / (bottom - top);
	tmp.m[3][2] = nearClip / (nearClip - farClip);
	tmp.m[3][3] = 1.0f;

	return tmp;

}
//ビューポート変換行列生成
inline Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 tmp;
	tmp.m[0][0] = width / 2.0f;
	tmp.m[0][1] = 0;
	tmp.m[0][2] = 0;
	tmp.m[0][3] = 0;
	tmp.m[1][0] = 0;
	tmp.m[1][1] = -height / 2.0f;
	tmp.m[1][2] = 0;
	tmp.m[1][3] = 0;
	tmp.m[2][0] = 0;
	tmp.m[2][1] = 0;
	tmp.m[2][2] = maxDepth - minDepth;
	tmp.m[2][3] = 0;
	tmp.m[3][0] = left + (width / 2.0f);
	tmp.m[3][1] = top + (height / 2.0f);
	tmp.m[3][2] = minDepth;
	tmp.m[3][3] = 1;
	return tmp;
}
//View行列生成
inline Matrix4x4 MakeViewMatirx(const Vector3& rotate, const Vector3& tranlate) {
	Matrix4x4 cameraWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, rotate, tranlate);

	return Inverse(cameraWorldMatrix);
}
//View行列生成
inline Matrix4x4 MakeViewMatirx(const Quaternion& q, const Vector3& tranlate) {
	Matrix4x4 cameraWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, q, tranlate);
	return Inverse(cameraWorldMatrix);
}
//方向に向かせる回転行列生成
inline Matrix4x4 MakeLookRotationMatrix(const Vector3& direction, const Vector3& up = {0.0f,1.0f,0.0f})  {
	Vector3 z = Normalize(direction);
	Vector3 x = Normalize(Cross(Normalize(up), z));
	Vector3 y = Cross(z, x);
	return {
		x.x, x.y, x.z, 0.0f,
		y.x, y.y, y.z, 0.0f,
		z.x, z.y, z.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f };
}

#pragma endregion
#pragma endregion
#pragma region	Quaternion
//ノルム
inline float Norm(const Quaternion& q) {
	float result = sqrtf(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);
	return result;
}
//共役
inline Quaternion Conjugate(const Quaternion& q) {
	Quaternion result{ -q.x,-q.y,-q.z,q.w };
	return result;
}
//逆Quaternion生成
inline Quaternion Inverse(const Quaternion& q) {
	float norm = Norm(q);
	norm = norm * norm;
	Quaternion conjugate = Conjugate(q);
	Quaternion result;
	result.x = conjugate.x / norm;
	result.y = conjugate.y / norm;
	result.z = conjugate.z / norm;
	result.w = conjugate.w / norm;
	return result;
}

#pragma region 演算子のオーバーロード
//乗算
inline Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs) {

	Quaternion result{
	lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y,
	lhs.w * rhs.y + lhs.y * rhs.w + lhs.z * rhs.x - lhs.x * rhs.z,
	lhs.w * rhs.z + lhs.z * rhs.w + lhs.x * rhs.y - lhs.y * rhs.x,
	lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z };

	return result;
}
//乗算
inline Quaternion operator *(const Quaternion& lhs, const Quaternion& rhs) {

	Quaternion result{
	lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y,
	lhs.w * rhs.y + lhs.y * rhs.w + lhs.z * rhs.x - lhs.x * rhs.z,
	lhs.w * rhs.z + lhs.z * rhs.w + lhs.x * rhs.y - lhs.y * rhs.x,
	lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z };

	return result;
}
//乗算代入
inline Quaternion& operator *=(Quaternion& lhs, const Quaternion& rhs) {
	lhs = lhs * rhs;
	return lhs;
}
//足し算
inline Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs) {
	return Quaternion{ lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w };
}
//スカラー乗算
inline Quaternion operator*(const Quaternion& lhs, float rhs) {
	return Quaternion{ lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs };
}
//スカラー乗算
inline Quaternion operator*(float lhs, const Quaternion& rhs) {
	return Quaternion{ lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, lhs * rhs.w };
}
//ベクトル乗算
inline Vector3 operator *(const Vector3& v, const Quaternion& q) {
	Quaternion vq = { v.x,v.y,v.z, 0 };
	Quaternion inverseQ = Inverse(q);
	Quaternion resultQ = q * vq * inverseQ;
	Vector3 result = { resultQ.x,resultQ.y,resultQ.z };
	return result;
}
//ベクトルの値が近いか
inline bool IsClose(const Vector3& v1, const Vector3& v2, float epsilon = 1e-6f) {
	return (std::fabs(v1.x - v2.x) < epsilon &&
		std::fabs(v1.y - v2.y) < epsilon &&
		std::fabs(v1.z - v2.z) < epsilon);
}

#pragma endregion
#pragma region ファクトリ関数
//初期化
inline Quaternion IdentityQuaternion() {
	Quaternion result{ 0.0f,0.0f,0.0f,1.0f };
	return result;
}
//x軸回転クオータニオン生成
inline Quaternion MakeForXAxis(float angle) {
	return Quaternion(std::sin(angle / 2.0f), 0.0f, 0.0f, std::cos(angle / 2.0f));
}
//y軸回転クオータニオン生成
inline Quaternion MakeForYAxis(float angle) {
	return Quaternion(0.0f, std::sin(angle / 2.0f), 0.0f, std::cos(angle / 2.0f));
}
//z軸回転クオータニオン生成
inline Quaternion MakeForZAxis(float angle) {
	return Quaternion(0.0f, 0.0f, std::sin(angle / 2.0f), std::cos(angle / 2.0f));
}
//x要素のみクオータニオン生成
inline Quaternion MakeXAxisFromQuaternion(Quaternion q) {
	return Quaternion(q.x, 0.0f, 0.0f, q.w);
}
//y要素のみクオータニオン生成
inline Quaternion MakeYAxisFromQuaternion(Quaternion q) {
	return Quaternion(0.0f, q.y, 0.0f, q.w);
}
//z要素のみクオータニオン生成
inline Quaternion MakeZAxisFromQuaternion(Quaternion q) {
	return Quaternion(0.0f, 0.0f, q.z, q.w);
}
//軸と角度からクオータニオン生成
inline Quaternion MakeFromAngleAxis(const Vector3& axis, float angle) {
	Vector3 v = Normalize(axis) * std::sinf(angle * 0.5f);
	Quaternion result;
	result.x = v.x;
	result.y = v.y;
	result.z = v.z;
	result.w = std::cosf(angle * 0.5f);
	return result;
}
//正規直交ベクトルからクオータニオン生成
inline Quaternion MakeFromOrthonormal(const Vector3& x, const Vector3& y, const Vector3& z) {
	float trace = x.x + y.y + z.z;
	if (trace > 0.0f) {
		float s = std::sqrtf(trace + 1.0f) * 0.5f;
		Quaternion result{};
		result.w = s;
		s = 0.25f / s;
		result.x = (y.z - z.y) * s;
		result.y = (z.x - x.z) * s;
		result.z = (x.y - y.x) * s;
		return result;
	}
	else if (x.x > y.y && x.x > z.z) {
		float s = std::sqrtf(1.0f + x.x - y.y - z.z) * 0.5f;
		Quaternion result{};
		result.x = s;
		s = 0.25f / s;
		result.y = (x.y + y.x) * s;
		result.z = (z.x + x.z) * s;
		result.w = (y.z - z.y) * s;
		return result;
	}
	else if (y.y > z.z) {
		float s = std::sqrtf(1.0f - x.x + y.y - z.z) * 0.5f;
		Quaternion result{};
		result.y = s;
		s = 0.25f / s;
		result.x = (x.y + y.x) * s;
		result.z = (y.z + z.y) * s;
		result.w = (z.x - x.z) * s;
		return result;
	}
	Quaternion result{};
	float s = std::sqrtf(1.0f - x.x - y.y + z.z) * 0.5f;
	result.z = s;
	s = 0.25f / s;
	result.x = (z.x + x.z) * s;
	result.y = (y.z + z.y) * s;
	result.w = (x.y - y.x) * s;
	return result;
}
//正規直交マトリックスからクオータニオン生成
inline Quaternion MakeFromOrthonormal(const Matrix4x4& m) {
	Vector3 x = GetXAxis(m);
	Vector3 y = GetYAxis(m);
	Vector3 z = GetZAxis(m);

	float trace = x.x + y.y + z.z;
	if (trace > 0.0f) {
		float s = std::sqrtf(trace + 1.0f) * 0.5f;
		Quaternion result{};
		result.w = s;
		s = 0.25f / s;
		result.x = (y.z - z.y) * s;
		result.y = (z.x - x.z) * s;
		result.z = (x.y - y.x) * s;
		return result;
	}
	else if (x.x > y.y && x.x > z.z) {
		float s = std::sqrtf(1.0f + x.x - y.y - z.z) * 0.5f;
		Quaternion result{};
		result.x = s;
		s = 0.25f / s;
		result.y = (x.y + y.x) * s;
		result.z = (z.x + x.z) * s;
		result.w = (y.z - z.y) * s;
		return result;
	}
	else if (y.y > z.z) {
		float s = std::sqrtf(1.0f - x.x + y.y - z.z) * 0.5f;
		Quaternion result{};
		result.y = s;
		s = 0.25f / s;
		result.x = (x.y + y.x) * s;
		result.z = (y.z + z.y) * s;
		result.w = (z.x - x.z) * s;
		return result;
	}
	Quaternion result{};
	float s = std::sqrtf(1.0f - x.x - y.y + z.z) * 0.5f;
	result.z = s;
	s = 0.25f / s;
	result.x = (z.x + x.z) * s;
	result.y = (y.z + z.y) * s;
	result.w = (x.y - y.x) * s;
	return result;
}
//二つのベクトルからクオータニオン生成
inline Quaternion MakeFromTwoVector(const Vector3& from, const Vector3& to) {
	Vector3 axis = Cross(from, to);
	float angle = Angle(from, to);
	return MakeFromAngleAxis(axis, angle);
}
//matrixからクオータニオン生成
inline Quaternion RotateMatrixToQuaternion(Matrix4x4 m) {
	float px = m.m[0][0] - m.m[1][1] - m.m[2][2] + 1.0f;
	float py = -m.m[0][0] + m.m[1][1] - m.m[2][2] + 1.0f;
	float pz = -m.m[0][0] - m.m[1][1] + m.m[2][2] + 1.0f;
	float pw = m.m[0][0] + m.m[1][1] + m.m[2][2] + 1.0f;

	int selected = 0;
	float max = px;
	if (max < py) {
		selected = 1;
		max = py;
	}
	if (max < pz) {
		selected = 2;
		max = pz;
	}
	if (max < pw) {
		selected = 3;
		max = pw;
	}

	if (selected == 0) {
		float x = std::sqrt(px) * 0.5f;
		float d = 1.0f / (4.0f * x);
		return Quaternion(
			x,
			(m.m[1][0] + m.m[0][1]) * d,
			(m.m[0][2] + m.m[2][0]) * d,
			(m.m[2][1] - m.m[1][2]) * d
		);
	}
	else if (selected == 1) {
		float y = std::sqrt(py) * 0.5f;
		float d = 1.0f / (4.0f * y);
		return Quaternion(
			(m.m[1][0] + m.m[0][1]) * d,
			y,
			(m.m[2][1] + m.m[1][2]) * d,
			(m.m[0][2] - m.m[2][0]) * d
		);
	}
	else if (selected == 2) {
		float z = std::sqrt(pz) * 0.5f;
		float d = 1.0f / (4.0f * z);
		return Quaternion(
			(m.m[0][2] + m.m[2][0]) * d,
			(m.m[2][1] + m.m[1][2]) * d,
			z,
			(m.m[1][0] - m.m[0][1]) * d
		);
	}
	else if (selected == 3) {
		float w = std::sqrtf(pw) * 0.5f;
		float d = 1.0f / (4.0f * w);
		return Quaternion(
			(m.m[2][1] - m.m[1][2]) * d,
			(m.m[0][2] - m.m[2][0]) * d,
			(m.m[1][0] - m.m[0][1]) * d,
			w
		);
	}
	assert(false);
	return Quaternion{};
}
//オイラー角からクオータニオン生成
inline Quaternion MakeFromEulerAngle(const Vector3& euler) {
	Vector3 s = Vector3(std::sin(euler.x * 0.5f), std::sin(euler.y * 0.5f), std::sin(euler.z * 0.5f));
	Vector3 c = Vector3(std::cos(euler.x * 0.5f), std::cos(euler.y * 0.5f), std::cos(euler.z * 0.5f));
	return Quaternion{
		s.x * c.y * c.z - c.x * s.y * s.z,
		c.x * s.y * c.z + s.x * c.y * s.z,
		c.x * c.y * s.z - s.x * s.y * c.z,
		c.x * c.y * c.z + s.x * s.y * s.z };
}
//ベクトルの方向に向かせるクオータニオン生成
inline Quaternion MakeLookRotation(const Vector3& direction, const Vector3& up = { 0.0f,1.0f,0.0f }) {
	Vector3 z = Normalize(direction);
	Vector3 x = Normalize(Cross(up, z));
	Vector3 y = Cross(z, x);
	return MakeFromOrthonormal(x, y, z);
}
#pragma endregion
//クオータニオンからオイラー角生成
inline Vector3 EulerAngle(const Quaternion& q) {
	Vector3 euler{};
	euler.x = std::atan2(2.0f * (q.w * q.x + q.y * q.z), 1.0f - 2.0f * (q.x * q.x + q.y * q.y));
	euler.y = std::asin(2.0f * (q.w * q.y - q.z * q.x));
	euler.z = std::atan2(2.0f * (q.w * q.z + q.x * q.y), 1.0f - 2.0f * (q.y * q.y + q.z * q.z));
	return euler;
}
//正規化
inline Quaternion Normalize(const Quaternion& q) {
	float tmp = q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z;
	tmp = sqrtf(tmp);
	return { q.x / tmp, q.y / tmp, q.z / tmp, q.w / tmp };
}

//ベクトルにクオータニオン回転適用
inline Vector3 RotateVector(const Vector3& v, const Quaternion& q) {
	Quaternion vq = { v.x,v.y,v.z, 0 };
	Quaternion inverseQ = Inverse(q);
	Quaternion resultQ = q * vq * inverseQ;
	Vector3 result = { resultQ.x,resultQ.y,resultQ.z };
	return result;
}

//内席
inline float Dot(const Quaternion& lhs, const Quaternion& rhs) noexcept {
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
}
//線形補間
inline Quaternion Lerp(float t, const Quaternion& start, const Quaternion& end) noexcept {
	return Quaternion{
		start.x + t * (end.x - start.x),
		start.y + t * (end.y - start.y),
		start.z + t * (end.z - start.z),
		start.w + t * (end.w - start.w) };
}
//球面線形補間
inline Quaternion Slerp(float t, const Quaternion& start, const Quaternion& end) noexcept {
	Quaternion s = start;
	float dot = Dot(start, end);
	if (dot > 0.9999f) {
		return Lerp(t, start, end);
	}
	// q1,q2が反対向きの場合
	if (dot < 0) {
		s.x = -s.x;
		s.y = -s.y;
		s.z = -s.z;
		s.w = -s.w;
		dot = -dot;
	}
	// 球面線形補間の計算
	float theta = std::acos(dot);
	return (std::sin((1.0f - t) * theta) * s + std::sin(t * theta) * end) * (1.0f / std::sin(theta));
}

inline bool IsClose(const Quaternion& q1, const Quaternion& q2, float epsilon = 0.0001f) {
	// クォータニオンの各成分の差の2乗の合計を計算する（ユークリッド距離の平方）
	float diff = (q1.w - q2.w) * (q1.w - q2.w)
		+ (q1.x - q2.x) * (q1.x - q2.x)
		+ (q1.y - q2.y) * (q1.y - q2.y)
		+ (q1.z - q2.z) * (q1.z - q2.z);

	// 差が閾値以下であればtrueを返す
	return diff < epsilon * epsilon;
}
#pragma endregion
#pragma region	OBB
inline OBB MakeOBB(Vector3 pos, Matrix4x4 rotation, Vector3 size) {
	OBB result;
	result.center = pos;
	result.orientations[0] = GetXAxis(rotation);
	result.orientations[1] = GetYAxis(rotation);
	result.orientations[2] = GetZAxis(rotation);
	result.size = size;

	return result;
}

inline OBB MakeOBB(Matrix4x4 worldMatrix) {
	OBB result;
	result.center = MakeTranslation(worldMatrix);

	Matrix4x4 rotation = MakeIdentity4x4();
	rotation = NormalizeMakeRotateMatrix(worldMatrix);
	result.orientations[0] = GetXAxis(rotation);
	result.orientations[1] = GetYAxis(rotation);
	result.orientations[2] = GetZAxis(rotation);

	result.size = MakeScale(worldMatrix);

	return result;
}

inline Vector3 MakeRandVector3(OBB box) {
	Vector3 result;
	result = { Rand(-box.size.x ,box.size.x),Rand(-box.size.y ,box.size.y) ,Rand(-box.size.z,box.size.z) };
	Matrix4x4 rotateMatrix = MakeIdentity4x4();
	SetXAxis(rotateMatrix, box.orientations[0]);
	SetYAxis(rotateMatrix, box.orientations[1]);
	SetZAxis(rotateMatrix, box.orientations[2]);
	result = result * rotateMatrix;
	result = result + box.center;
	return result;
}

#pragma endregion
#pragma region	Frustum
//視錐台生成
inline Frustum MakeFrustum(const Vector3& frontLeftTop,const Vector3& frontRightTop,const Vector3& frontLeftBottom,const Vector3& frontRightBottom
, const Vector3& backLeftTop, const Vector3& backRightTop, const Vector3& backLeftBottom, const Vector3& backRightBottom) {
	Frustum frustum;
	//left
	frustum.plane[0].normal = MakeNormal(frontLeftTop, frontLeftBottom, backLeftBottom);
	frustum.plane[0].distance = Dot(frustum.plane[0].normal, frontLeftTop);

	//top
	frustum.plane[1].normal = MakeNormal(frontLeftTop, backLeftTop, backRightTop);
	frustum.plane[1].distance = Dot(frustum.plane[1].normal, frontLeftTop);

	//right
	frustum.plane[2].normal = MakeNormal(frontRightTop, backRightTop, backRightBottom);
	frustum.plane[2].distance = Dot(frustum.plane[2].normal, frontRightTop);

	//bottom
	frustum.plane[3].normal = MakeNormal(frontLeftBottom, frontRightBottom, backRightBottom);
	frustum.plane[3].distance = Dot(frustum.plane[3].normal, frontLeftBottom);

	//front
	frustum.plane[4].normal = MakeNormal(frontLeftTop, frontRightTop, frontRightBottom);
	frustum.plane[4].distance = Dot(frustum.plane[4].normal, frontLeftTop);

	//back
	frustum.plane[5].normal = MakeNormal(backRightTop, backLeftTop, backLeftBottom);
	frustum.plane[5].distance = Dot(frustum.plane[5].normal, backRightTop);

	frustum.vertex[0] = frontLeftTop;
	frustum.vertex[1] = frontRightTop;
	frustum.vertex[2] = frontLeftBottom;
	frustum.vertex[3] = frontRightBottom;
	frustum.vertex[4] = backLeftTop;
	frustum.vertex[5] = backRightTop;
	frustum.vertex[6] = backLeftBottom;
	frustum.vertex[7] = backRightBottom;
	return frustum;
}
//視錐台生成
inline Frustum MakeFrustrum(const Matrix4x4& inverseViewProjection) {
	Frustum result;
	Vector3 vertex[8]{};
	vertex[0] = Vector3{-1.0f,1.0f,0.0f} * inverseViewProjection;
	vertex[1] = Vector3{1.0f,1.0f,0.0f } * inverseViewProjection;
	vertex[2] = Vector3{ -1.0f,-1.0f,0.0f } * inverseViewProjection;
	vertex[3] = Vector3{ 1.0f,-1.0f,0.0f } * inverseViewProjection;

	vertex[4] = Vector3{ -1.0f,1.0f,1.0f } * inverseViewProjection;
	vertex[5] = Vector3{ 1.0f,1.0f,1.0f } * inverseViewProjection;
	vertex[6] = Vector3{ -1.0f,-1.0f,1.0f } * inverseViewProjection;
	vertex[7] = Vector3{ 1.0f,-1.0f,1.0f } * inverseViewProjection;

	result = MakeFrustum(vertex[0], vertex[1], vertex[2], vertex[3],vertex[4], vertex[5], vertex[6], vertex[7]);
	return result;
}

//視錐台生成頂点だけセットされてるとき
inline Frustum MakeFrustum(Frustum& frustum) {
	//left
	frustum.plane[0].normal = MakeNormal(frustum.vertex[0], frustum.vertex[2], frustum.vertex[6]);
	frustum.plane[0].distance = Dot(frustum.plane[0].normal, frustum.vertex[0]);

	//top
	frustum.plane[1].normal = MakeNormal(frustum.vertex[0], frustum.vertex[4], frustum.vertex[5]);
	frustum.plane[1].distance = Dot(frustum.plane[1].normal, frustum.vertex[0]);

	//right
	frustum.plane[2].normal = MakeNormal(frustum.vertex[1], frustum.vertex[5], frustum.vertex[7]);
	frustum.plane[2].distance = Dot(frustum.plane[2].normal, frustum.vertex[1]);

	//bottom
	frustum.plane[3].normal = MakeNormal(frustum.vertex[2], frustum.vertex[3], frustum.vertex[7]);
	frustum.plane[3].distance = Dot(frustum.plane[3].normal, frustum.vertex[2]);

	//front
	frustum.plane[4].normal = MakeNormal(frustum.vertex[0], frustum.vertex[1], frustum.vertex[3]);
	frustum.plane[4].distance = Dot(frustum.plane[4].normal, frustum.vertex[0]);

	//back
	frustum.plane[5].normal = MakeNormal(frustum.vertex[4], frustum.vertex[6], frustum.vertex[7]);
	frustum.plane[5].distance = Dot(frustum.plane[5].normal, frustum.vertex[4]);

	return frustum;
}
//視錐台マトリックス変換適用
inline Frustum operator *(const Frustum& f, const Matrix4x4& m) {

	Frustum result;

	result.vertex[0] = f.vertex[0] * m;
	result.vertex[1] = f.vertex[1] * m;
	result.vertex[2] = f.vertex[2] * m;
	result.vertex[3] = f.vertex[3] * m;
	result.vertex[4] = f.vertex[4] * m;
	result.vertex[5] = f.vertex[5] * m;
	result.vertex[6] = f.vertex[6] * m;
	result.vertex[7] = f.vertex[7] * m;

	MakeFrustum(result);

	return result;
}
#pragma endregion


