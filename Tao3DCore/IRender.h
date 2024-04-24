#pragma once

namespace Tao3D
{
class IRender
{
public:
	virtual ~IRender() { }
	virtual void init(int width, int height) = 0;
	virtual void resize(int widht, int height) {};
	virtual void update()					 = 0;
	virtual bool render()					 = 0;
};
} // namespace Tao3D