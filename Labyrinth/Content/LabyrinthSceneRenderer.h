#pragma once


#include "..\Common\DeviceResources.h"
#include "..\Common\StepTimer.h"

#include <string>
#include <fstream>
#include <vector>

#include <direct.h>	// Directory utility

#include "utils.h"

#include "AI/Player.h"
#include "AI/DumbAI.h"
#include "AI/Manual.h"

namespace Labyrinth {

	/**
	* Labyrinth scene renderer
	*
	*	Updates and draws the labyrinth on the screen
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

		Manual* getManual();

		void moveDirection(Directions dir, int player=0);	/// Schedules a move of a player, 1 cell in one direction
		void moveTo(Position pos, int player=0);	/// Schedules a move of a player to another cell

		Cell getCell(Position at);

		int stepOnce();	/// Commits the scheduled moves and returns the turn number
		void augmentFrequency();
		void diminishFrequency();

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
		std::vector< std::vector<Cell> > m_labyrinth;	/// Vector to store the labyrinth data (walls, origin, end...)
		int m_sizeX;	/// The width in cells of the labyrinth
		int m_sizeY;	/// The height in cells of the labyrinth
		Position m_originPosition; /// The starting cell position
		Position m_endPosition;	/// The end cell position

		int m_playerCount;	/// Number of player actually playing
		std::vector<Position> m_playersPosition;	/// The coodinate of each player

		std::vector<Player*> m_players;

		float m_cellWidth;	/// The width of a cell in "pixels"
		float m_cellHeight;	/// The height of a cell in "pixels"

		// Turns
		double m_timeSinceLastTurn;	/// The time in seconds since the last turn
		double m_turnFrequency;	/// The frequency at which the turns elapse
		int m_turnCount;	/// The actual turn number
		std::vector<Directions> m_playersDirection;	/// The next turn's scheduled directions for each player

		// Logging / Debug (UWP does not support stdout)
		void log(std::wstring ws);
		void log(std::string s);
		void log(Platform::String^ s);
		void log(char* s, size_t count);
		void log(char s);
	};
}
