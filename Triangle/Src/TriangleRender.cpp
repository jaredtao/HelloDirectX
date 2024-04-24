#include "TriangleRender.h"

#include "Application.h"
#include "Common.h"

#include <DirectXColors.h>
#include <DirectXTK/DirectXHelpers.h>
#include <algorithm>
#include <chrono>
namespace Tao3D
{
TriangleRender::TriangleRender() { }

TriangleRender::~TriangleRender()
{
	gApp.unRegisterDeviceNotify(this);
}
void TriangleRender::init(int width, int height)
{
	m_width			= width;
	m_height		= height;
	m_states		= std::make_unique<CommonStates>(gDevice);
	auto projection = Matrix::CreateScale(2.f / float(width), -2.f / float(height), 1.f) * Matrix::CreateTranslation(-1.f, 1.f, 0.f);

	m_effect = std::make_unique<BasicEffect>(gDevice);
	m_effect->SetProjection(projection);
	m_effect->SetVertexColorEnabled(true);
	void const* shaderByteCode;
	size_t		byteCodeLength;
	m_effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
	ThrowIfFailed(
		gDevice->CreateInputLayout(
			VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, shaderByteCode, byteCodeLength, m_inputLayout.GetAddressOf()),
		"CreateInputLayout");
	m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(gContext);

	gApp.registerDeviceNotify(this);
}
void TriangleRender::update()
{
	auto kb = gKeyboard.GetState();
	if (kb.Escape)
	{
		gApp.unRegisterDeviceNotify(this);
		gApp.quit();
	}
}
void TriangleRender::onDeviceLost() 
{
	m_states = nullptr;
	m_effect = nullptr;
	m_primitiveBatch = nullptr;
	m_inputLayout	 = nullptr;
}
void TriangleRender::onDeviceRestore()
{
	init(m_width, m_height);
}
bool TriangleRender::render()
{
	if (!gResource.ensureDeviceValid())
	{
		return true;
	}

	gContext->OMSetBlendState(m_states->Opaque(), nullptr, 0xffffffff);
	gContext->OMSetDepthStencilState(m_states->DepthNone(), 0);
	gContext->RSSetState(m_states->CullCounterClockwise());

	m_effect->Apply(gContext);
	gContext->IASetInputLayout(m_inputLayout.Get());

	m_primitiveBatch->Begin();
	// VertexPositionColor v1(Vector3(0.f, 0.5f, 0.5f), Colors::SeaGreen);
	// VertexPositionColor v2(Vector3(0.5f, -0.5f, 0.5f), Colors::SeaGreen);
	// VertexPositionColor v3(Vector3(-0.5f, -0.5f, 0.5f), Colors::SeaGreen);
	auto seconds = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
	int					x1		= 400 + seconds.count() % 6 * 10;
	VertexPositionColor v1(Vector3((float)x1, 150.f, 0.f), Colors::PaleVioletRed);
	VertexPositionColor v2(Vector3(700.f, 450.f, 0.f), Colors::SeaGreen);
	VertexPositionColor v3(Vector3(300.f, 450.f, 0.f), Colors::LightGoldenrodYellow);
	m_primitiveBatch->DrawTriangle(v1, v2, v3);
	m_primitiveBatch->End();
	return true;
}
} // namespace Tao3D
