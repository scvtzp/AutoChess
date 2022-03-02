#pragma once
#include <crtdbg.h>
#include <assert.h>
#include <Windows.h>
#include <math.h>

// dirxtx���� �����ϴ� �Լ����� SIMD ������ �����ϱ� �����̴�.
// SIMD float 4���� �Ѳ����� ���ϰų� ���Ѵ�.
#include <DirectXCollision.h>
#include <DirectXCollision.inl>
#include <DirectXPackedVector.h>	// SIMD ������ ����� ���Ϳ� �Լ��� ������ ���.
#include <d3d11_4.h>				// ���̷�Ʈ ����̽� �� �Լ����� ����ִ� ���
#include <d3dcompiler.h>			// ���̴������� �Լ� 

#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "dxguid")

// �̷��� �־���� ���̷�Ʈ�� ����Ҽ� �ִ�.

#ifdef _DEBUG
#define MATHMSG(MSSAGE) _wassert(MSSAGE, _CRT_WIDE(__FILE__), (unsigned)(__LINE__))
#else
#define MATHMSG(MSSAGE) assert(false)
#endif


class HMATRIX;
class HGAMEMATH 
{
public:
	// PI ������
	// D ��׸� => 360�� ����
	// R ���� => ȣ������ ����
	// ���п����� �ٵ� 3.14�� ���ؼ� ������ �� ����� �����.
	// ���� 90���� ȸ����Ű�� �ʹ�.
	// ȸ����Ű�� �Լ�(������ �־��ּ���)
	// ȸ����Ű�� �Լ�(90 * 3.14 / 180/0f)
	// �⺻������ ������ 180 => 3.14�� ǥ���Ѵ�.
	static const float PI;
	static const float DTOR;
	static const float RTOD;
	static const float PI2;
	static const float PI2D1;
};

class Game_Vector 
{
public:
	const static Game_Vector FORWARD;
	const static Game_Vector BACK;
	const static Game_Vector RIGHT;
	const static Game_Vector LEFT;
	const static Game_Vector UP;
	const static Game_Vector DOWN;

	const static Game_Vector RED;
	const static Game_Vector BLUE;
	const static Game_Vector WHITE;
	const static Game_Vector BLACK;
	const static Game_Vector GREEN;
	const static Game_Vector MAGENTA;
	const static Game_Vector NONE;
	const static Game_Vector ZERO;
	const static Game_Vector ONE;


public:
	void ZEROCHECK() const
	{
		if (x == 0.0F)		{ MATHMSG(L"X ũ�� �Ӽ��� 0�Դϴ�");		}
		if (y == 0.0F)		{ MATHMSG(L"Y ũ�� �Ӽ��� 0�Դϴ�");		}
		if (z == 0.0F)		{ MATHMSG(L"Z ũ�� �Ӽ��� 0�Դϴ�");		}
	}

	static Game_Vector MatrixToQuaternion(const HMATRIX& _Mat);

	bool IsNearlyZero() const
	{
		// 8.3123162721622e-d
		return fabsf(x) <= 1.e-4f && fabsf(y) <= 1.e-4f && fabsf(z) <= 1.e-4f;
	}

	// ������ ������.
	static float InvSqrt(float f)
	{
		const __m128 fOneHalf = _mm_set_ss(0.5f);
		__m128 Y0, X0, X1, X2, FOver2;
		float temp;

		Y0 = _mm_set_ss(f);
		X0 = _mm_rsqrt_ss(Y0);	// 1/sqrt estimate (12 bits)
		FOver2 = _mm_mul_ss(Y0, fOneHalf);

		// 1st Newton-Raphson iteration
		X1 = _mm_mul_ss(X0, X0);
		X1 = _mm_sub_ss(fOneHalf, _mm_mul_ss(FOver2, X1));
		X1 = _mm_add_ss(X0, _mm_mul_ss(X0, X1));

		// 2nd Newton-Raphson iteration
		X2 = _mm_mul_ss(X1, X1);
		X2 = _mm_sub_ss(fOneHalf, _mm_mul_ss(FOver2, X2));
		X2 = _mm_add_ss(X1, _mm_mul_ss(X1, X2));

		_mm_store_ss(&temp, X2);
		return temp;
	}

	// ���ͷ� �÷��� ǥ���Ϸ��� 0~1������ ���� �־��ּ���
	static unsigned int ColorToUint(float _R, float _G, float _B, float _A)
	{
		unsigned int uColor = 0;
		__int8* Ptr = (__int8*)&uColor;
		//       0.5 * 255
		Ptr[0] = (__int8)(_R * 255.0f);
		Ptr[1] = (__int8)(_G * 255.0f);
		Ptr[2] = (__int8)(_B * 255.0f);
		Ptr[3] = (__int8)(_A * 255.0f);
		return uColor;
	}

	unsigned int ColorToUint()
	{
		unsigned int uColor = 0;
		__int8* Ptr = (__int8*)&uColor;
		//       0.5 * 255
		Ptr[0] = (__int8)(R * 255.0f);
		Ptr[1] = (__int8)(G * 255.0f);
		Ptr[2] = (__int8)(B * 255.0f);
		Ptr[3] = (__int8)(A * 255.0f);
		return uColor;
	}

	static Game_Vector Lerp(Game_Vector _Start, Game_Vector _End, float _Ratio)
	{
		return DirectX::XMVectorLerp(_Start.DXVEC, _End.DXVEC, _Ratio);
	}

	static Game_Vector QSLerp(Game_Vector _Start, Game_Vector _End, float _Ratio)
	{
		return DirectX::XMQuaternionSlerp(_Start.DXVEC, _End.DXVEC, _Ratio);
	}

	static unsigned int ColorToUintInt8(unsigned int _R, unsigned int _G, unsigned int _B, unsigned int _A)
	{
		unsigned int uColor = 0;
		__int8* Ptr = (__int8*)&uColor;
		//       0.5 * 255
		Ptr[0] = (__int8)_R;
		Ptr[1] = (__int8)_G;
		Ptr[2] = (__int8)_B;
		Ptr[3] = (__int8)_A;
		return uColor;
	}

public:
	// 2���������� X�� Y�� ǥ���Ҽ� �ְԵȴ�.

	union 
	{
		struct 
		{
			float x; // r
			float y; // g
			float z; // b
			float w; // a
		};

		struct
		{
			float R; // r
			float G; // g
			float B; // b
			float A; // a
		};

		struct
		{
			float Arr[4]; // a
		};

		struct
		{
			float Arr2D[1][4]; // a
		};

		DirectX::XMVECTOR DXVEC;
	};


	// �������� ��ο� �Լ����� ��κ�
	// int�� �䱸�ϹǷ� ��ȯ�Լ��� ����ڴ�.
	// �׷��� �� �׷� ���ۺ��� int�� �������
	// float���� �����Ѵ���
	// int �� ��ȯ�ϳ���???

	// �̷������� ����� �ִ�.
	// int�� 0.1�� �̵��ߴٴ°��� ǥ���Ҽ� �������?
	// �Ұ����ϴ�.
	// �׷��Ƿ� ���� ��ġ�� ũ���
	// float���� ǥ���ϰ�
	// �װ��� �������� �ȼ������� ��������
	// int�� ��ȯ�ؼ� ����Ѵ�.

public:
	static float DirToDirAngleDeg2D(const Game_Vector _Left, const Game_Vector& _Right)
	{
		return DirToDirAngleRad2D(_Left, _Right) * HGAMEMATH::RTOD;
	}

	static float DirToDirAngleRad2D(const Game_Vector _Left, const Game_Vector& _Right)
	{
		Game_Vector Left = _Left;
		Game_Vector Right = _Right;

		Left.z = 0.0F;
		Right.z = 0.0F;
		Left.NORMAL3D();
		Right.NORMAL3D();
		// ���� ���̰� 1�� �ƴ�.

		// 0~180������ �����ϴ� �Լ���.

		//  1     1
		// |A| * |B| * Cos��Ÿ
		float CosS = Dot(Left, Right);

		float RadAngle = acosf(CosS);

		if (_Right.y < _Left.y)
		{
			RadAngle = HGAMEMATH::PI2 - RadAngle;
		}


		// ���Լ�
		return RadAngle;
	}




	static Game_Vector WindowPosTo3DPos(Game_Vector _WindowPos, const Game_Vector& _ScreenSize) 
	{
		Game_Vector ConPos = _WindowPos;
		ConPos.y *= -1.0f;
		ConPos += _ScreenSize.HalfVector();
		return ConPos;
	}

	static Game_Vector ReversWindowPosTo3DPos(Game_Vector _WindowPos, const Game_Vector& _ScreenSize)
	{
		Game_Vector ConPos = _WindowPos;
		ConPos.y *= -1.0f;
		ConPos -= _ScreenSize.HalfVector();
		return ConPos;
	}


	static Game_Vector Corss3D(const Game_Vector& _Left, const Game_Vector& _Right)
	{
		return Game_Vector(
			_Left.y * _Right.z - _Left.z * _Right.y, 
			_Left.z * _Right.x - _Left.x * _Right.z,
			_Left.x * _Right.y - _Left.y * _Right.x
		);
	}


	static float Dot(const Game_Vector& _Left, const Game_Vector& _Right)
	{
		DirectX::XMVECTOR DXVEC = DirectX::XMVector3Dot(_Left.DXVEC, _Right.DXVEC);
		return DXVEC.m128_f32[0];
	}

	operator DirectX::XMVECTOR() const
	{
		return DXVEC;
	}

	operator DirectX::XMFLOAT3() const
	{
		return DirectX::XMFLOAT3(x, y, z);
	}

	operator DirectX::XMFLOAT4() const
	{
		return DirectX::XMFLOAT4(x, y, z, w);
	}

	float Dot3D(const Game_Vector& _Other)
	{
		Game_Vector Result = DirectX::XMVector3Dot(DXVEC, _Other);
		return Result.x;
	}

	float Dot4D(const Game_Vector& _Other)
	{
		Game_Vector Result = DirectX::XMVector4Dot(DXVEC, _Other);
		return Result.y;
	}

	float DirToRadZ()
	{
		Game_Vector Dir = *this;
		Dir.NORMAL3D();
		return acosf(Dir.x);
	}

	float DirToDegZ()
	{
		return DirToRadZ() * HGAMEMATH::RTOD;
	}

	// � 

	static Game_Vector RadToDirZ(float _Rad) 
	{
		// ������ �־��ָ�
		// �׿� �´� ���� ���͸� ������ִ°�.
		// ���̰� 1�� ����

		// CalPos.X = (1 * cosf(_Rad)) ;
		// CalPos.Y = (1 * sinf(_Rad)) ;
		return { cosf(_Rad) , sinf(_Rad)};
	}

	int IX() const {		return (int)x;	}
	int IY() const {		return (int)y;	}

	unsigned int UIX() const { return (unsigned int)x; }
	unsigned int UIY() const { return (unsigned int)y; }

	// �����⺸�� ���ϱⰡ �׻� ������.
	// ���ϱ� 0.5f�� ó���Ѵ�.
	float HX() const { return x * 0.5f; }
	float HY() const { return y * 0.5f; }

	int HIX() const { return (int)HX(); }
	int HIY() const { return (int)HY(); }

	// ���ʹ� => �ﰢ��
	// ��Ÿ����� ������ ����ϰ�
	// ������ �����ִ� �Լ��� ����ϱ⸸ �ϸ�
	// �ս��� ������ ���̸� �˼� �ִ�.
	float LEN3D() const 
	{ 
		DirectX::XMVECTOR Vec = DirectX::XMVector3Length(DXVEC);
		return Vec.m128_f32[0];
	}

	// ������ ���̷�
	// X�� Y�� ������ �Ǹ�
	// ���̰� 1�� ���Ͱ� ������ �ǰ�
	// �������Ͷ�� �Ѵ�.

	Game_Vector HalfVector() const
	{
		Game_Vector HV = *this;
		// ���� ���Ͱ� �ȴ�.
		HV *= 0.5f;
		return HV;
	}

	Game_Vector AbsVector() const
	{
		Game_Vector V = *this;
		// ���Լ��� �Ⱦ��� �ȴ�. ����� �Ǵϱ�.
		// abs�� ���� �Լ���?

		V.x = fabsf((float)V.x);
		V.y = fabsf((float)V.y);
		V.z = fabsf((float)V.z);
		return V;
	}

	Game_Vector DegToQ() const
	{
		Game_Vector Vector = *this;
		Vector *= HGAMEMATH::DTOR;
		return DirectX::XMQuaternionRotationRollPitchYaw(Vector.x, Vector.y, Vector.z);
	}

	Game_Vector RadToQ() const
	{
		return DirectX::XMQuaternionRotationRollPitchYaw(x, y, z);
	}

	// ������ ���̸� 1�� �����ϴ� 
	void NORMAL3D()
	{
		DXVEC = DirectX::XMVector3Normalize(DXVEC);
		return;
	}

	Game_Vector NORMAL3DTORETURN() const
	{
		Game_Vector V = DirectX::XMVector3Normalize(DXVEC);
		return V;
	}

	Game_Vector& operator=(const Game_Vector& _Value)
	{
		x = _Value.x;
		y = _Value.y;
		z = _Value.z;
		w = _Value.w;

		return *this;
	}


	bool operator==(const Game_Vector& _Value) const
	{
		return x == _Value.x && y == _Value.y && z == _Value.z;
	}

	bool operator!=(const Game_Vector& _Value) const
	{
		return x != _Value.x || y != _Value.y || z != _Value.z;
	}

	Game_Vector operator+(const Game_Vector& _Value) const
	{
		return { x + _Value.x, y + _Value.y, z + _Value.z };
	}

	Game_Vector operator*(const Game_Vector& _Value) const
	{
		return { x * _Value.x, y * _Value.y, z * _Value.z };
	}

	Game_Vector operator/(const float _Value) const
	{
		return { x / _Value, y / _Value, z / _Value };
	}

	Game_Vector operator-(const Game_Vector& _Value) const
	{
		return { x - _Value.x, y - _Value.y,  z - _Value.z };
	}

	Game_Vector operator/(const Game_Vector& _Value) const
	{
		return { x / _Value.x, y / _Value.y,  z / _Value.z };
	}

	Game_Vector operator-()
	{
		return { -x, -y, -z};
	}


	Game_Vector& operator-=(const Game_Vector& _Value)
	{
		x -= _Value.x;
		y -= _Value.y;
		z -= _Value.z;

		return *this;
	}

	Game_Vector& operator+=(const Game_Vector& _Value)
	{
		x += _Value.x;
		y += _Value.y;
		z += _Value.z;

		return *this;
	}

	Game_Vector& operator*=(const HMATRIX& _Value);
	Game_Vector operator*(const HMATRIX& _Value);

	Game_Vector& operator*=(const float _Value)
	{
		x *= _Value;
		y *= _Value;
		z *= _Value;

		return *this;
	}

	Game_Vector& operator*=(const Game_Vector& _Value)
	{
		x *= _Value.x;
		y *= _Value.y;
		z *= _Value.z;

		return *this;
	}

	Game_Vector operator*(const float& _Value) const
	{
		return { x * _Value, y * _Value, z * _Value };
	}

	operator POINT() 
	{
		return POINT{IX(), IY()};
	}

	void Set3D(const Game_Vector& _Other) 
	{
		x = _Other.x;
		y = _Other.y;
		z = _Other.z;
	}

	Game_Vector RotDegReturn(const Game_Vector& _Deg) {
		return RotRadReturn(_Deg * HGAMEMATH::DTOR);
	}
	Game_Vector RotRadReturn(const Game_Vector& _Rad) 
	{
		return DirectX::XMVector3Rotate(*this, _Rad.RadToQ());
	}

	Game_Vector RotDegZReturn(float _Rad) {
		return RotRadZReturn(_Rad * HGAMEMATH::DTOR);
	}
	Game_Vector RotRadZReturn(float _Rad)
	{
		Game_Vector CalPos = *this;
		CalPos.x = (x * cosf(_Rad)) - (y * sinf(_Rad));
		CalPos.y = (x * sinf(_Rad)) + (y * cosf(_Rad));
		return CalPos;
	}

	Game_Vector RotDegYReturn(float _Rad) {
		return RotRadYReturn(_Rad * HGAMEMATH::DTOR);
	}
	Game_Vector RotRadYReturn(float _Rad)
	{
		Game_Vector CalPos = *this;
		CalPos.x = (x * cosf(_Rad)) - (z * sinf(_Rad));
		CalPos.z = (x * sinf(_Rad)) + (z * cosf(_Rad));
		return CalPos;
	}

	Game_Vector RotDegXReturn(float _Rad) {
		return RotRadXReturn(_Rad * HGAMEMATH::DTOR);
	}
	Game_Vector RotRadXReturn(float _Rad)
	{
		Game_Vector CalPos = *this;
		CalPos.y = (y * cosf(_Rad)) - (z * sinf(_Rad));
		CalPos.z = (y * sinf(_Rad)) + (z * cosf(_Rad));
		return CalPos;
	}

	void RotDegZ(float _Rad) {
		RotRadZ(_Rad * HGAMEMATH::DTOR);
	}
	void RotRadZ(float _Rad) 
	{
		Game_Vector CalPos = *this;
		CalPos.x = (x * cosf(_Rad)) - (y * sinf(_Rad));
		CalPos.y = (x * sinf(_Rad)) + (y * cosf(_Rad));
		*this = CalPos;
	}

	void RotDegY(float _Rad) {
		RotRadY(_Rad * HGAMEMATH::DTOR);
	}
	void RotRadY(float _Rad)
	{
		Game_Vector CalPos = *this;
		CalPos.x = (x * cosf(_Rad)) - (z * sinf(_Rad));
		CalPos.z = (x * sinf(_Rad)) + (z * cosf(_Rad));
		*this = CalPos;
	}

	void RotDegX(float _Rad) {
		RotRadX(_Rad * HGAMEMATH::DTOR);
	}
	void RotRadX(float _Rad)
	{
		Game_Vector CalPos = *this;
		CalPos.y = (y * cosf(_Rad)) - (z * sinf(_Rad));
		CalPos.z = (y * sinf(_Rad)) + (z * cosf(_Rad));
		*this = CalPos;
	}

public:
	Game_Vector() : x(0.0f), y(0.0f), z(0.0f), w(1.0f)	{	}
	Game_Vector(const int _X, const int _Y) : x((float)_X), y((float)_Y), z(0.0f), w(1.0f) {	}
	Game_Vector(const float _X, const  float _Y) : x(_X), y(_Y), z(0.0f), w(1.0f) {	}
	Game_Vector(const float _X, const  float _Y, const  float _Z) : x(_X), y(_Y), z(_Z), w(1.0f) {	}
	Game_Vector(const float _X, const  float _Y, const  float _Z, const  float _W) : x(_X), y(_Y), z(_Z), w(_W) {	}
	Game_Vector(const Game_Vector& _Other) : x(_Other.x), y(_Other.y), z(_Other.z), w(_Other.w) 	{		}
	Game_Vector(const DirectX::XMVECTOR& _Other) : DXVEC(_Other) {		}
};

class HMATRIX
{
public:
	union 
	{
		struct 
		{
			float Arr2D[4][4];
		};

		struct
		{
			float ArrF1D[16];
		};


		struct
		{
			Game_Vector ArrV[4];
		};

		DirectX::XMMATRIX DXMAT;
	};

public:
	HMATRIX() { IDEN(); }
	~HMATRIX() {}

	HMATRIX(const HMATRIX& _Other) : Arr2D{ 0, }
	{
		memcpy_s(this, sizeof(HMATRIX), &_Other, sizeof(HMATRIX));
	}

	HMATRIX(const DirectX::XMMATRIX& _Other) 
	{
		DXMAT = _Other;
	}

public:
	static HMATRIX Affine(Game_Vector _Scale, Game_Vector _RotQ, Game_Vector _Pos)
	{
		//                                                   ���ʹϿ� 0 0 0 1
		//                                                   ���ʹϿ� 0 0 0 1�� ��������� ȸ�������
		//                                                   1 0 0 0          
		//                                                   0 1 0 0 
		//                                                   0 0 1 0 
		//                                                   0 0 0 1
		return DirectX::XMMatrixAffineTransformation(_Scale, Game_Vector::ZERO, _RotQ, _Pos);
	}

	static HMATRIX Transformation(Game_Vector _Scale, Game_Vector _RotQ, Game_Vector _Pos)
	{
		return DirectX::XMMatrixTransformation(Game_Vector::ZERO, Game_Vector::ZERO, _Scale.DXVEC, Game_Vector::ZERO, _RotQ, _Pos);
	}


	Game_Vector ExtractScaling()
	{
		Game_Vector ret = Game_Vector::ZERO;

		float Tolerance = 1.e-8f;

		const float SquareSum0 = (Arr2D[0][0] * Arr2D[0][0]) + (Arr2D[0][1] * Arr2D[0][1]) + (Arr2D[0][2] * Arr2D[0][2]);
		const float SquareSum1 = (Arr2D[1][0] * Arr2D[1][0]) + (Arr2D[1][1] * Arr2D[1][1]) + (Arr2D[1][2] * Arr2D[1][2]);
		const float SquareSum2 = (Arr2D[2][0] * Arr2D[2][0]) + (Arr2D[2][1] * Arr2D[2][1]) + (Arr2D[2][2] * Arr2D[2][2]);

		if (SquareSum0 > Tolerance)
		{
			float Scale0 = sqrtf(SquareSum0);
			ret.x = Scale0;
			float InvScale0 = 1.f / Scale0;
			Arr2D[0][0] *= InvScale0;
			Arr2D[0][1] *= InvScale0;
			Arr2D[0][2] *= InvScale0;
		}
		else
		{
			ret.x = 0;
		}

		if (SquareSum1 > Tolerance)
		{
			float Scale1 = sqrtf(SquareSum1);
			ret.y = Scale1;
			float InvScale1 = 1.f / Scale1;
			Arr2D[1][0] *= InvScale1;
			Arr2D[1][1] *= InvScale1;
			Arr2D[1][2] *= InvScale1;
		}
		else
		{
			ret.y = 0;
		}

		if (SquareSum2 > Tolerance)
		{
			float Scale2 = sqrtf(SquareSum2);
			ret.z = Scale2;
			float InvScale2 = 1.f / Scale2;
			Arr2D[2][0] *= InvScale2;
			Arr2D[2][1] *= InvScale2;
			Arr2D[2][2] *= InvScale2;
		}
		else
		{
			ret.z = 0;
		}

		return ret;
	}

	inline float Determinant() const
	{
		return	Arr2D[0][0] * (
			Arr2D[1][1] * (Arr2D[2][2] * Arr2D[3][3] - Arr2D[2][3] * Arr2D[3][2]) -
			Arr2D[2][1] * (Arr2D[1][2] * Arr2D[3][3] - Arr2D[1][3] * Arr2D[3][2]) +
			Arr2D[3][1] * (Arr2D[1][2] * Arr2D[2][3] - Arr2D[1][3] * Arr2D[2][2])
			) -
			Arr2D[1][0] * (
				Arr2D[0][1] * (Arr2D[2][2] * Arr2D[3][3] - Arr2D[2][3] * Arr2D[3][2]) -
				Arr2D[2][1] * (Arr2D[0][2] * Arr2D[3][3] - Arr2D[0][3] * Arr2D[3][2]) +
				Arr2D[3][1] * (Arr2D[0][2] * Arr2D[2][3] - Arr2D[0][3] * Arr2D[2][2])
				) +
			Arr2D[2][0] * (
				Arr2D[0][1] * (Arr2D[1][2] * Arr2D[3][3] - Arr2D[1][3] * Arr2D[3][2]) -
				Arr2D[1][1] * (Arr2D[0][2] * Arr2D[3][3] - Arr2D[0][3] * Arr2D[3][2]) +
				Arr2D[3][1] * (Arr2D[0][2] * Arr2D[1][3] - Arr2D[0][3] * Arr2D[1][2])
				) -
			Arr2D[3][0] * (
				Arr2D[0][1] * (Arr2D[1][2] * Arr2D[2][3] - Arr2D[1][3] * Arr2D[2][2]) -
				Arr2D[1][1] * (Arr2D[0][2] * Arr2D[2][3] - Arr2D[0][3] * Arr2D[2][2]) +
				Arr2D[2][1] * (Arr2D[0][2] * Arr2D[1][3] - Arr2D[0][3] * Arr2D[1][2])
				);
	}
	operator DirectX::XMMATRIX() const
	{
		return DXMAT;
	}

	void IDEN()
	{
		memset(this, 0, sizeof(HMATRIX));
		DXMAT = DirectX::XMMatrixIdentity();
	}

	void RotDeg(const Game_Vector& _Value)
	{
		RotRad(_Value * HGAMEMATH::DTOR);
	}

	void RotRad(const Game_Vector& _Value)
	{
		DXMAT = DirectX::XMMatrixRotationRollPitchYaw(_Value.x, _Value.y, _Value.z);
	}

	void RotXDeg(float _Angle) 
	{
		RotXRad(_Angle * HGAMEMATH::DTOR);
	}

	void RotXRad(float _Angle)
	{
		IDEN();
		DXMAT = DirectX::XMMatrixRotationX(_Angle);
	}

	void RotYDeg(float _Angle) 
	{
		RotYRad(_Angle * HGAMEMATH::DTOR);
	}

	void RotYRad(float _Angle)
	{
		IDEN();
		DXMAT = DirectX::XMMatrixRotationY(_Angle);
	}

	void RotZDeg(float _Angle) 
	{
		RotZRad(_Angle * HGAMEMATH::DTOR);
	}

	void RotZRad(float _Angle)
	{
		IDEN();
		DXMAT = DirectX::XMMatrixRotationZ(_Angle);
	}

	void Pos3D(const Game_Vector& _Value)
	{
		IDEN();

		// ����� �̰� 64��Ʈ������ �ȴ�.
		// 32��Ʈ�� �����ϸ� �ȵȴ�.
		DXMAT = DirectX::XMMatrixTranslation(_Value.x, _Value.y, _Value.z);
		// DXMAT = DirectX::XMLoadFloat4x4(DirectX::XMMatrixTranslation(_Value.X, _Value.Y, _Value.Z));
		// #ifdef X64
		// #else
		// #endif
		// ArrV[3].Set3D(_Value);
	}

	void Scale3D(const Game_Vector& _Value)
	{
		IDEN();
		_Value.ZEROCHECK();
		DXMAT = DirectX::XMMatrixScaling(_Value.x, _Value.y, _Value.z);
	}

	Game_Vector MulOne(Game_Vector _Vector)
	{
		_Vector.w = 1.0f;

		return DirectX::XMVector4Transform(_Vector, DXMAT);
	}

	Game_Vector MulZero(Game_Vector _Vector)
	{
		_Vector.w = 0.0f;
		return DirectX::XMVector4Transform(_Vector, DXMAT);
	}


	void VIEWLH(const Game_Vector& _Pos, const Game_Vector& _EyeDir, const Game_Vector& _EyeUp)
	{
		IDEN();
		// ���̸� 1�� ����� ����


		// LookAt�� ��ġ��ü�� �־��ָ� �ȴ�. 
		// �̳༮�� ���������� 
		DXMAT = DirectX::XMMatrixLookToLH(_Pos, _EyeDir, _EyeUp);
		return;
	}

	// _FOV : ���� �þ߰�
	// _Width : ����ϰ� ���� ȭ���� �ʺ�
	// _Height : ����ϰ� ���� ȭ���� ����
	// _Near : ������� �Ÿ�
	// _Far : ������� �Ÿ�

	void Inverse()
	{
		DXMAT = DirectX::XMMatrixInverse(nullptr, DXMAT);
	}

	HMATRIX InverseReturn()
	{
		HMATRIX ReturnMat = DirectX::XMMatrixInverse(nullptr, DXMAT);
		return ReturnMat;
	}

	void VIEWPORT(float _StartX, float _StartY, float _W, float _H, float _MinZ, float _MaxZ) 
	{
		IDEN();

		Arr2D[0][0] = _W / 2;
		Arr2D[1][1] = -(_H / 2);
		Arr2D[2][2] = _MinZ - _MaxZ;

		Arr2D[3][0] = _StartX + Arr2D[0][0];
		Arr2D[3][1] = _StartY + (_H / 2);
		Arr2D[3][2] = _MinZ;
	}

	void ORTHLH(const Game_Vector& _Size, float _Near, float _Far)
	{
		IDEN();
		DXMAT = DirectX::XMMatrixOrthographicLH(_Size.x, _Size.y, _Near, _Far);
		return;
	}

	void PERSDEG(float _Fov, const Game_Vector& _Size, float _Near, float _Far)
	{
		PERSRAD(_Fov, _Size.x, _Size.y, _Near, _Far);
	}

	void World(const Game_Vector& _Scale, const Game_Vector& _EulerRot, const Game_Vector& _Pos)
	{
		HMATRIX Scale;
		HMATRIX Rot;
		HMATRIX Pos;

		Scale.Scale3D(_Scale);
		Rot.RotDeg(_EulerRot);
		Pos.Pos3D(_Pos);

		*this = Scale * Rot * Pos;
	}

	void PERSRAD(float _Fov, float _Width, float _Height, float _Near, float _Far)
	{
		// � ȭ�鿡 �츮�� ������ �𸣴ϱ�
		// ȭ�� ��ü�� -1~1

		// 1.28 / 0.72
		// 0.72 / 1.28;

		// 2 / 1
		// x 0.5  y
		// ���� �þ߰�.

		IDEN();
		DXMAT = DirectX::XMMatrixPerspectiveFovLH(_Fov * HGAMEMATH::DTOR, _Width/ _Height, _Near, _Far);


		return;
	}


	void TransPos() 
	{
		DXMAT = DirectX::XMMatrixTranspose(DXMAT);
	}

public:
	HMATRIX& operator=(const HMATRIX& _Value)
	{
		memcpy_s(this, sizeof(HMATRIX),&_Value, sizeof(HMATRIX));

		return *this;
	}

	HMATRIX operator*(const HMATRIX& _Value) const
	{
		HMATRIX Mat = DirectX::XMMatrixMultiply(DXMAT, _Value.DXMAT);
		return Mat;
	}

};

enum class MATTYPE
{
	MATTYPE_POS,
	MATTYPE_SCALE,
	MATTYPE_ROT,
	MATTYPE_REVOL,
	MATTYPE_PARENT,
	MATTYPE_LWORLD,
	MATTYPE_WWORLD,
	MATTYPE_VIEW,
	MATTYPE_PROJ,
	MATTYPE_WV,
	MATTYPE_VP,
	MATTYPE_WVP,
	MATTYPE_MAX,
};

class HTRANSDATA
{
public:
	union
	{
		struct
		{
			HMATRIX POS;
			HMATRIX SCALE;
			HMATRIX ROT;
			HMATRIX REVOL;
			HMATRIX PARENT;
			HMATRIX LWORLD;
			HMATRIX WWORLD;
			HMATRIX VIEW;
			HMATRIX PROJ;
			HMATRIX WV;
			HMATRIX VP;
			HMATRIX WVP;
		};

		HMATRIX ARRMAT[(int)MATTYPE::MATTYPE_MAX];
	};

public:
	void CALIDEN()
	{
		for (auto& _Value : ARRMAT)
		{
			_Value.IDEN();
		}
	}

	void CALLWORLD()
	{
		LWORLD = SCALE * ROT * POS * REVOL;
	}

	void CALWWORLD( )
	{
		WWORLD = LWORLD * PARENT;
	}

	void SETVIEWPROJ(HMATRIX _VIEW, HMATRIX _PROJ)
	{
		VIEW = _VIEW;
		PROJ = _PROJ;
	}

	void CALWVP() 
	{
		WV = WWORLD * VIEW;
		VP = VIEW * PROJ;

		// �̰� ���� �߿��� ���.
		// WWORLD == �÷��̾��� ��ġ�� ��� �ִ�. ũ�� ȸ��
		// View == ī�޶��� ��ġ�� �ٶ󺸴� ����� ȸ����
		WVP = WWORLD * VIEW * PROJ;
	}

	void CALTrans() 
	{
		CALLWORLD();
		CALWWORLD();
		CALWVP();
	}

	HTRANSDATA() : LWORLD()
	{

	}

	HTRANSDATA(const HTRANSDATA& _Other) 
	{
		memcpy_s(this, sizeof(HTRANSDATA), &_Other, sizeof(HTRANSDATA));
	}

	~HTRANSDATA()
	{

	}
};


struct int2
{
	union
	{
		struct 
		{
			int x;
			int y;
		};

		__int64 Key;

		struct
		{
			int Arr[2];
		};
	};

};

struct int4
{
	union
	{
		struct
		{
			int x;
			int y;
			int z;
			int w;
		};
		struct
		{
			int Arr[4];
		};
	};

};

typedef Game_Vector float4;

