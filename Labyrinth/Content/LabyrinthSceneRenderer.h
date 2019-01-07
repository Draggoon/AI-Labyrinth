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
		void update(DX::StepTimer const& timer);	/// Not used yet. Called every frame.
		void render();	/// Display the current state of the game
		
		void addPlayer();	/// Add a player to the game
		void removePlayer(int player=-1);	/// Remove one player (the last added if -1)
		void reloadFromFile();	/// Reload the pattern from the file and reset the positions

		void moveUp(int player=0);	/// Move the player 1 cell up
		void moveDown(int player=0);	/// Move the player 1 cell down
		void moveLeft(int player=0);	/// Move the player 1 cell to the left
		void moveRight(int player=0);	/// Move the player 1 cell to the right
		void moveTo(int x, int y, int player=0);	/// Move the cursor of a player to another cell

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
		void loadLabyrinthFromFile(std::string filename);	/// Load the labyrinth patter from a file
		std::string m_labyrinthPatternFileName;	/// The default filename to load
		std::vector< std::vector<char> > m_labyrinth;	/// Vector to store the labyrinth data (walls, origin, end...)
		int m_sizeX;	/// The width in cells of the labyrinth
		int m_sizeY;	/// The height in cells of the labyrinth
		int m_originX;	/// The starting cell X coordinate
		int m_originY;	/// The starting cell Y coordinate
		int m_playerCount;	/// Number of player actually playing
		std::vector<int> m_cursorsX;	/// The X coodinate of each player
		std::vector<int> m_cursorsY;	/// The Y coordinate of each player

		float m_cellWidth;	/// The width of a cell in "pixels"
		float m_cellHeight;	/// The height of a cell in "pixels"

		// Logging / Debug (UWP does not support stdout)
		void log(std::wstring ws);
		void log(std::string s);
		void log(Platform::String^ s);
		void log(char* s, size_t count);
		void log(char s);
	};
}
