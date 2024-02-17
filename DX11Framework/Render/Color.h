#pragma once

#include <DirectXMath.h>

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
	const Color BLUE(0, 172, 255);
}