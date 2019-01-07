#pragma once


#include "..\Common\DeviceResources.h"
#include "..\Common\StepTimer.h"

#include <string>
#include <fstream>
#include <vector>

#include <direct.h>	// Directory utility

namespace Labyrinth {

	/*	Labyrinth scene renderer
	**
	**	Updates and draws the labyrinth on the screen
	*/
	class LabyrinthSceneRenderer {
	public:
		LabyrinthSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);

		void createDeviceDependentResources();
		void createWindowSizeDependentResources();
		void releaseDeviceDependentResources();
		void update(DX::StepTimer const& timer);
		void render();
		
		void reloadFromFile();

		void moveUp();
		void moveDown();
		void moveLeft();
		void moveRight();
		void moveTo(int x, int y);

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// Resources related to rendering.
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>    m_whiteBrush;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>    m_greenBrush;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>    m_redBrush;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>    m_blackBrush;
		Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock1> m_stateBlock;

		// Labyrinth resources
		void loadLabyrinthFromFile(std::string filename);
		std::string m_labyrinthPatternFileName;
		std::vector< std::vector<char> > m_labyrinth;
		int m_sizeX;
		int m_sizeY;
		float m_cellWidth;
		float m_cellHeight;

		int m_cursorX;
		int m_cursorY;

		// Logging / Debug
		void log(std::wstring ws);
		void log(std::string s);
		void log(Platform::String^ s);
		void log(char* s, size_t count);
		void log(char s);
	};
}
