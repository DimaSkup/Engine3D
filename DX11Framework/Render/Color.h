#pragma once

#include <DirectXMath.h>
#include <DirectXPackedVector.h>

typedef unsigned char BYTE;

class Color
{
public:
	Color();
	Color(unsigned int val);
	Color(BYTE r, BYTE g, BYTE b);
	Color(BYTE r, BYTE g, BYTE b, BYTE a);
	Color(const Color & src);

	Color & operator=(const Color & src);
	bool operator==(const Color & rhs) const;
	bool operator!=(const Color & rhs) const;

	const BYTE GetR() const;
	void SetR(BYTE r);

	const BYTE GetG() const;
	void SetG(BYTE g);

	const BYTE GetB() const;
	void SetB(BYTE b);

	const BYTE GetA() const;
	void SetA(BYTE a);

	const BYTE* GetRGBA()
	{
		return rgba;
	}

	DirectX::XMFLOAT4 GetFloat4() const;



private:
	union
	{
		BYTE rgba[4];
		unsigned int color;
	};
};

namespace Colors
{
	const Color UnloadedTextureColor(100, 100, 100);
	const Color UnhandledTextureColor(250, 0, 0);

	// color XM-vectors
	XMGLOBALCONST DirectX::XMVECTORF32 vWhite = { 1.0f, 1.0f, 1.0f, 1.0f };
	XMGLOBALCONST DirectX::XMVECTORF32 vBlack = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMGLOBALCONST DirectX::XMVECTORF32 vRed = { 1.0f, 0.0f, 0.0f, 1.0f };
	XMGLOBALCONST DirectX::XMVECTORF32 vGreen = { 0.0f, 1.0f, 0.0f, 1.0f };
	XMGLOBALCONST DirectX::XMVECTORF32 vBlue = { 0.0f, 0.0f, 1.0f, 1.0f };
	XMGLOBALCONST DirectX::XMVECTORF32 vYellow = { 1.0f, 1.0f, 0.0f, 1.0f };
	XMGLOBALCONST DirectX::XMVECTORF32 vCyan = { 0.0f, 1.0f, 1.0f, 1.0f };
	XMGLOBALCONST DirectX::XMVECTORF32 vMagenta = { 1.0f, 0.0f, 1.0f, 1.0f };
	XMGLOBALCONST DirectX::XMVECTORF32 vSilver = { 0.75f, 0.75f, 0.75f, 1.0f };
	XMGLOBALCONST DirectX::XMVECTORF32 vLightSteelBlue = { 0.69f, 0.77f, 0.87f, 1.0f };


	XMGLOBALCONST DirectX::PackedVector::XMCOLOR White = { 1.0f, 1.0f, 1.0f, 1.0f };
	XMGLOBALCONST DirectX::PackedVector::XMCOLOR Black = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMGLOBALCONST DirectX::PackedVector::XMCOLOR Red= { 1.0f, 0.0f, 0.0f, 1.0f };
	XMGLOBALCONST DirectX::PackedVector::XMCOLOR Green = { 0.0f, 1.0f, 0.0f, 1.0f };
	XMGLOBALCONST DirectX::PackedVector::XMCOLOR Blue = { 0.0f, 0.0f, 1.0f, 1.0f };
	XMGLOBALCONST DirectX::PackedVector::XMCOLOR Yellow = { 1.0f, 1.0f, 0.0f, 1.0f };
	XMGLOBALCONST DirectX::PackedVector::XMCOLOR Cyan = { 0.0f, 1.0f, 1.0f, 1.0f };
	XMGLOBALCONST DirectX::PackedVector::XMCOLOR Magenta = { 1.0f, 0.0f, 1.0f, 1.0f };
	XMGLOBALCONST DirectX::PackedVector::XMCOLOR Silver = { 0.75f, 0.75f, 0.75f, 1.0f };
	XMGLOBALCONST DirectX::PackedVector::XMCOLOR LightSteelBlue = { 0.69f, 0.77f, 0.87f, 1.0f };
}