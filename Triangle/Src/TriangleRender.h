#pragma once
#include "DXResources.h"
#include "IRender.h"
#include "IDeviceNotify.h"
#include <DirectXTK/CommonStates.h>
#include <DirectXTK/Effects.h>
#include <DirectXTK/PrimitiveBatch.h>
#include <DirectXTK/VertexTypes.h>
#include <memory>
namespace Tao3D
{
using Microsoft::WRL::ComPtr;
class TriangleRender : public IRender, public IDeviceNotify
{
public:
	TriangleRender();
	~TriangleRender();
	void init(int width, int height) override;
	void update() override;
	bool render() override;

	void onDeviceLost() override;
	void onDeviceRestore() override;

private:
	std::unique_ptr<DirectX::CommonStates>								   m_states			= nullptr;
	std::unique_ptr<DirectX::BasicEffect>								   m_effect			= nullptr;
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch = nullptr;
	ComPtr<ID3D11InputLayout>											   m_inputLayout;

	int m_width = 0;
	int m_height = 0;
};
} // namespace Tao3D
