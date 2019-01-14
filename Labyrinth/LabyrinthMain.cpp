#include "pch.h"
#include "LabyrinthMain.h"
#include "Common\DirectXHelper.h"

#include "Content/AI/Manual.h"

using namespace Labyrinth;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Concurrency;

// Loads and initializes application assets when the application is loaded.
LabyrinthMain::LabyrinthMain(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_deviceResources(deviceResources)
{
	// Register to be notified if the Device is lost or recreated
	m_deviceResources->RegisterDeviceNotify(this);

	m_fpsTextRenderer = std::unique_ptr<SampleFpsTextRenderer>(new SampleFpsTextRenderer(m_deviceResources));
	m_labyrinthSceneRenderer = std::unique_ptr<LabyrinthSceneRenderer>(new LabyrinthSceneRenderer(m_deviceResources));

}

LabyrinthMain::~LabyrinthMain()
{
	// Deregister device notification
	m_deviceResources->RegisterDeviceNotify(nullptr);
}

// Updates application state when the window size changes (e.g. device orientation change)
void LabyrinthMain::CreateWindowSizeDependentResources() 
{
	m_labyrinthSceneRenderer->createWindowSizeDependentResources();
}

// Updates the application state once per frame.
void LabyrinthMain::Update() 
{
	// Update scene objects.
	m_timer.Tick([&]()
	{
		m_labyrinthSceneRenderer->update(m_timer);
		m_fpsTextRenderer->Update(m_timer);
	});
}

// Renders the current frame according to the current application state.
// Returns true if the frame was rendered and is ready to be displayed.
bool LabyrinthMain::Render() 
{
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
	{
		return false;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();

	// Reset the viewport to target the whole screen.
	auto viewport = m_deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	// Reset render targets to the screen.
	ID3D11RenderTargetView *const targets[1] = { m_deviceResources->GetBackBufferRenderTargetView() };
	context->OMSetRenderTargets(1, targets, m_deviceResources->GetDepthStencilView());

	// Clear the back buffer and depth stencil view.
	context->ClearRenderTargetView(m_deviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::CornflowerBlue);
	context->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Render the scene objects.
	m_labyrinthSceneRenderer->render();
	m_fpsTextRenderer->Render();

	return true;
}

/**
* keyPressed event
*
*	Called when a keypress event is captured by the application
*/
void LabyrinthMain::keyPressed(Windows::UI::Core::KeyEventArgs^ args) {
	if (args->VirtualKey == Windows::System::VirtualKey::Up)
		m_labyrinthSceneRenderer->getManual()->moveDirection(up);
	if (args->VirtualKey == Windows::System::VirtualKey::Down)
		m_labyrinthSceneRenderer->getManual()->moveDirection(down);
	if (args->VirtualKey == Windows::System::VirtualKey::Left)
		m_labyrinthSceneRenderer->getManual()->moveDirection(left);
	if (args->VirtualKey == Windows::System::VirtualKey::Right)
		m_labyrinthSceneRenderer->getManual()->moveDirection(right);
	if (args->VirtualKey == Windows::System::VirtualKey::F5)
		m_labyrinthSceneRenderer->reloadFromFile();
	if (args->VirtualKey == Windows::System::VirtualKey::Add)
		m_labyrinthSceneRenderer->addPlayer();
	if (args->VirtualKey == Windows::System::VirtualKey::Subtract)
		m_labyrinthSceneRenderer->removePlayer(-1);
	if (args->VirtualKey == Windows::System::VirtualKey::Multiply)
		m_labyrinthSceneRenderer->augmentFrequency();
	if (args->VirtualKey == Windows::System::VirtualKey::Divide)
		m_labyrinthSceneRenderer->diminishFrequency();
}

// Notifies renderers that device resources need to be released.
void LabyrinthMain::OnDeviceLost()
{
	m_labyrinthSceneRenderer->releaseDeviceDependentResources();
	m_fpsTextRenderer->ReleaseDeviceDependentResources();
}

// Notifies renderers that device resources may now be recreated.
void LabyrinthMain::OnDeviceRestored()
{
	m_labyrinthSceneRenderer->createDeviceDependentResources();
	m_fpsTextRenderer->CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}
