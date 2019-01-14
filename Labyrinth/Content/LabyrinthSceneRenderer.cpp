#include "pch.h"
#include "LabyrinthSceneRenderer.h"

#include "Common/DirectXHelper.h"

using namespace Labyrinth;
/**
* Constructor
*
*	Set the members and initialize the labyrinth
*/
LabyrinthSceneRenderer::LabyrinthSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_deviceResources(deviceResources),
	m_labyrinthPatternFileName("LabyrinthPattern.txt"),
	m_sizeX(0),
	m_sizeY(0),
	m_originPosition(0, 0),
	m_endPosition(0, 0),
	m_playerCount(1),
	m_cellWidth(100.0f),
	m_cellHeight(100.0f),
	m_timeSinceLastTurn(0.0),
	m_turnFrequency(2.0),
	m_turnCount(0) {

	// Create device independent resources

	DX::ThrowIfFailed(
		m_deviceResources->GetD2DFactory()->CreateDrawingStateBlock(&m_stateBlock)
	);

	createDeviceDependentResources();	// init DirectX resources (brushes)


	// Set the first players to 0;0
	for (int i(0); i < m_playerCount; ++i) {
		m_playersPosition.emplace_back(0,0);
		m_playersDirection.push_back(none);
		if (i == 0)
			m_players.push_back(new Manual);
		else
			m_players.push_back(new DumbAI);
	}

	// Load the pattern in memory
	loadLabyrinthFromFile(m_labyrinthPatternFileName);
}

void LabyrinthSceneRenderer::createDeviceDependentResources() {
	DX::ThrowIfFailed(
		m_deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_whiteBrush)
	);

	DX::ThrowIfFailed(
		m_deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green), &m_greenBrush)
	);

	DX::ThrowIfFailed(
		m_deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &m_redBrush)
	);

	DX::ThrowIfFailed(
		m_deviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_blackBrush)
	);
}

void LabyrinthSceneRenderer::releaseDeviceDependentResources() {
	m_whiteBrush.Reset();
	m_greenBrush.Reset();
	m_redBrush.Reset();
	m_blackBrush.Reset();
}

void LabyrinthSceneRenderer::createWindowSizeDependentResources() {

}


//	##     ## ########  ########     ###    ######## ######## 
//	##     ## ##     ## ##     ##   ## ##      ##    ##       
//	##     ## ##     ## ##     ##  ##   ##     ##    ##       
//	##     ## ########  ##     ## ##     ##    ##    ######   
//	##     ## ##        ##     ## #########    ##    ##       
//	##     ## ##        ##     ## ##     ##    ##    ##       
//	 #######  ##        ########  ##     ##    ##    ######## 
void LabyrinthSceneRenderer::update(DX::StepTimer const & timer) {
	m_timeSinceLastTurn += timer.GetElapsedSeconds();
	if (m_timeSinceLastTurn > (1/m_turnFrequency)) {
		for (int player(0); player < m_playerCount; ++player) {
			std::vector<Cell> surroundings{ getCell(Position(m_playersPosition[player].x, m_playersPosition[player].y - 1)),
				getCell(Position(m_playersPosition[player].x, m_playersPosition[player].y + 1)),
				getCell(Position(m_playersPosition[player].x - 1, m_playersPosition[player].y)),
				getCell(Position(m_playersPosition[player].x + 1, m_playersPosition[player].y)) };

			m_playersDirection[player] = m_players[player]->nextMove(m_playersPosition[player], surroundings);
		}
		stepOnce();
		m_timeSinceLastTurn = 0.0;
	}
}


//	########  ######## ##    ## ########  ######## ########  
//	##     ## ##       ###   ## ##     ## ##       ##     ## 
//	##     ## ##       ####  ## ##     ## ##       ##     ## 
//	########  ######   ## ## ## ##     ## ######   ########  
//	##   ##   ##       ##  #### ##     ## ##       ##   ##   
//	##    ##  ##       ##   ### ##     ## ##       ##    ##  
//	##     ## ######## ##    ## ########  ######## ##     ## 
/**
* Render
*
*	Called when the screen is invalidated or after an update
*/
void LabyrinthSceneRenderer::render() {
	ID2D1DeviceContext* context = m_deviceResources->GetD2DDeviceContext();
	Windows::Foundation::Size logicalSize = m_deviceResources->GetLogicalSize();

	// Set up the context to start drawing
	context->SaveDrawingState(m_stateBlock.Get());
	context->BeginDraw();

	// Fit to screen
	D2D1::Matrix3x2F screenScale = D2D1::Matrix3x2F::Scale(logicalSize.Width / (m_sizeX*m_cellWidth), logicalSize.Height / (m_sizeY*m_cellHeight));
	context->SetTransform(screenScale * m_deviceResources->GetOrientationTransform2D());

	// Draw the labyrinth
	D2D1_RECT_F rect;
	for (int i(0); i < m_sizeY; ++i) {
		rect.top = i * m_cellHeight;
		rect.bottom = rect.top + m_cellHeight;
		for (int j(0); j < m_sizeX; ++j) {
			rect.left = j * m_cellWidth;
			rect.right = rect.left + m_cellWidth;
			if (m_labyrinth[i][j] == wall)	// Wall
				context->FillRectangle(rect, m_blackBrush.Get());
			else if (Position(j,i) == m_originPosition)	// Origin
				context->FillRectangle(rect, m_greenBrush.Get());
			else if (Position(j,i) == m_endPosition)	// End
				context->FillRectangle(rect, m_redBrush.Get());
		}
	}

	// Draw the players
	D2D1_POINT_2F p1, p2;
	int sqrtNbPlayer((int)ceil(sqrt(m_playerCount)));	// To place the players on multiple rows if needed
	for (int p(0); p < m_playerCount; ++p) {
		p1.x = m_playersPosition[p].x * m_cellWidth + m_cellWidth / 20.0f + (p%sqrtNbPlayer)*m_cellWidth/sqrtNbPlayer;
		p1.y = m_playersPosition[p].y * m_cellHeight + m_cellHeight / 20.0f + (p/sqrtNbPlayer)*m_cellHeight/sqrtNbPlayer;
		p2.x = m_playersPosition[p].x * m_cellWidth - m_cellWidth / 20.0f + (p%sqrtNbPlayer + 1)*m_cellWidth/sqrtNbPlayer;
		p2.y = m_playersPosition[p].y * m_cellHeight - m_cellHeight / 20.0f+ (p/sqrtNbPlayer + 1)*m_cellHeight/sqrtNbPlayer;
		context->DrawLine(p1, p2, m_blackBrush.Get(), 10.0f/(sqrtNbPlayer*sqrtNbPlayer));
		p1.x = m_playersPosition[p].x * m_cellWidth - m_cellWidth / 20.0f + (p%sqrtNbPlayer + 1)*m_cellWidth / sqrtNbPlayer;
		p1.y = m_playersPosition[p].y * m_cellHeight + m_cellHeight / 20.0f + (p / sqrtNbPlayer)*m_cellHeight / sqrtNbPlayer;
		p2.x = m_playersPosition[p].x * m_cellWidth + m_cellWidth / 20.0f + (p%sqrtNbPlayer)*m_cellWidth / sqrtNbPlayer;
		p2.y = m_playersPosition[p].y * m_cellHeight - m_cellHeight / 20.0f + (p/sqrtNbPlayer + 1)*m_cellHeight / sqrtNbPlayer;
		context->DrawLine(p1, p2, m_blackBrush.Get(), 10.0f/(sqrtNbPlayer*sqrtNbPlayer));
	}

	// Ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
	// is lost. It will be handled during the next call to Present.
	HRESULT hr = context->EndDraw();
	if (hr != D2DERR_RECREATE_TARGET) {
		DX::ThrowIfFailed(hr);
	}

	// Free the context for other functions to draw
	context->RestoreDrawingState(m_stateBlock.Get());
}


//	########  ##          ###    ##    ## ######## ########   ######  
//	##     ## ##         ## ##    ##  ##  ##       ##     ## ##    ## 
//	##     ## ##        ##   ##    ####   ##       ##     ## ##       
//	########  ##       ##     ##    ##    ######   ########   ######  
//	##        ##       #########    ##    ##       ##   ##         ## 
//	##        ##       ##     ##    ##    ##       ##    ##  ##    ## 
//	##        ######## ##     ##    ##    ######## ##     ##  ######  

/**
* Add 1 player
*
*	Add a new player at the origin
*/
void LabyrinthSceneRenderer::addPlayer() {
	m_playersPosition.push_back(m_originPosition);
	m_playersDirection.push_back(none);
	m_players.push_back(new DumbAI);
	++m_playerCount;
}

/**
* Remove 1 player
*
*	Delete a player
*	player: the player ID to remove (last added by default or negative ID)
*/
void LabyrinthSceneRenderer::removePlayer(int player) {
	if (m_playerCount > 1) {
		if (player < 0) {
			m_playersPosition.pop_back();
			m_playersDirection.pop_back();
			delete m_players.back();
			m_players.pop_back();
			--m_playerCount;
		}
		else if (player < m_playerCount) {
			m_playersPosition.erase(m_playersPosition.begin() + player);
			m_playersDirection.erase(m_playersDirection.begin() + player);
			delete m_players[player];
			m_players.erase(m_players.begin() + player);
			--m_playerCount;
		}
	}
}


//	 ######  ######## ######## ########   ######  
//	##    ##    ##    ##       ##     ## ##    ## 
//	##          ##    ##       ##     ## ##       
//	 ######     ##    ######   ########   ######  
//	      ##    ##    ##       ##              ## 
//	##    ##    ##    ##       ##        ##    ## 
//	 ######     ##    ######## ##         ######  

/**
* Move the cursor right by 1 cell
*
*	Schedules a move of a player in one direction.
*	dir: the direction in which the player wants to move
*	player: the player id (0 by default)
*/
void LabyrinthSceneRenderer::moveDirection(Directions dir, int player) {
	if (player >= 0 && player < m_playerCount) {
		m_playersDirection[player] = dir;
	}
}

/**
* Move to cell
*
*	Move to any cell that is free (not a wall) and not out of bounds.
*	x, y: the destination coordinates
*	player: the player to move (0 by default)
*/
void LabyrinthSceneRenderer::moveTo(Position pos, int player) {
	if (player >= 0 && player < m_playerCount) {
		if (pos.x < m_sizeX && pos.x >= 0 && pos.y < m_sizeY && pos.y >= 0) {
			if (m_labyrinth[pos.y][pos.x] != wall) {
				m_playersPosition[player] = pos;
				if (pos == m_endPosition) {
					// END REACHED! THROW SOME CODE HERE
					m_playersPosition[player] = m_originPosition;
				}
			}
		}
	}
}

int LabyrinthSceneRenderer::stepOnce() {
	for (int player(0); player < m_playerCount; ++player) {
		switch (m_playersDirection[player]) {
		case up:
			moveTo(Position(m_playersPosition[player].x, m_playersPosition[player].y-1), player);
			break;
		case down:
			moveTo(Position(m_playersPosition[player].x, m_playersPosition[player].y+1), player);
			break;
		case left:
			moveTo(Position(m_playersPosition[player].x-1, m_playersPosition[player].y), player);
			break;
		case right:
			moveTo(Position(m_playersPosition[player].x+1, m_playersPosition[player].y), player);
			break;
		default:
			;
		}
		m_playersDirection[player] = none;
	}
	return ++m_turnCount;
}

void Labyrinth::LabyrinthSceneRenderer::augmentFrequency() {
	m_turnFrequency *= 2.0;
}

void Labyrinth::LabyrinthSceneRenderer::diminishFrequency() {
	m_turnFrequency /= 2;
}

Cell Labyrinth::LabyrinthSceneRenderer::getCell(Position at) {
	if (at.x >= 0 && at.x < m_sizeX && at.y >= 0 && at.y < m_sizeY)
		return m_labyrinth[at.y][at.x];
	else
		return wall;
}


//	##        #######     ###    ########  
//	##       ##     ##   ## ##   ##     ## 
//	##       ##     ##  ##   ##  ##     ## 
//	##       ##     ## ##     ## ##     ## 
//	##       ##     ## ######### ##     ## 
//	##       ##     ## ##     ## ##     ## 
//	########  #######  ##     ## ########  

/**
* Reload pattern from file
*
*	Calls load from file. Used when F5 pressed.
*/
void LabyrinthSceneRenderer::reloadFromFile() {
	loadLabyrinthFromFile(m_labyrinthPatternFileName);
}

Manual * Labyrinth::LabyrinthSceneRenderer::getManual() {
	return (Manual*)(m_players[0]);
}

/**
* Load data from file
*
*	Fills the labyrinth data in memory from the file supplied
*	filename: a path to a file containing data
*/
void LabyrinthSceneRenderer::loadLabyrinthFromFile(std::string filename) {
	// Retrieving data from the file
	std::string str;
	std::ifstream fstr(filename);
	// Load default if file not accessible
	if (!fstr.is_open()) {
		OutputDebugString(L"ERROR unable to open file. folder is:\n\t");
		char dirc[1024];
		_getcwd(dirc, 1024);
		std::string dirstr(dirc);
		OutputDebugString(std::wstring(dirstr.begin(), dirstr.end()).c_str());
		str =	"##########"
				"#O       #"
				"######## #"
				"#        #"
				"# ########"
				"#        #"
				"######## #"
				"#        #"
				"# ########"
				"#       E#"
				"##########";
	}
	else
		str = std::string((std::istreambuf_iterator<char>(fstr)), std::istreambuf_iterator<char>());

	// Reset labyrinth data
	m_sizeX = 0;
	m_sizeY = 0;
	m_labyrinth.clear();
	int tX = 0;

	// Fill the labyrinth with data from the file
	std::vector<Cell> vec;
	while (str.size() > 0) {
		if (str.size() > 1) {
			m_labyrinth.push_back(vec);
			++m_sizeY;	// Calculate the height of the labyrinth
		}
		while (str.front() != '\n') {
			// Origin found
			switch (str.front()) {
			case 'o':
			case 'O':
				m_originPosition = Position((int)m_labyrinth.back().size(), (int)m_labyrinth.size() - 1);
				for (int i(0); i < m_playerCount; ++i) {
					m_playersPosition[i] = m_originPosition;
				}
				m_labyrinth.back().push_back(empty);
				break;
			case 'e':
			case 'E':
				m_endPosition = Position((int)m_labyrinth.back().size(), (int)m_labyrinth.size() - 1);
				m_labyrinth.back().push_back(empty);
				break;
			case '#':
				m_labyrinth.back().push_back(wall);
				break;
			default:
				m_labyrinth.back().push_back(empty);
			}

			str.erase(0, 1);
			++tX;
			if (str.size() == 0)
				break;
		}
		if (str.size() > 0)
			str.erase(0, 1);
		if (tX > m_sizeX)
			m_sizeX = tX;	// Detect the max size of a line
		tX = 0;
	}

	// Adding walls to the end of the shorter lines
	for (size_t i(0); i < m_labyrinth.size(); ++i) {
		while (m_labyrinth[i].size() < m_sizeX)
			m_labyrinth[i].push_back(wall);
	}

	// fstr automatically closed
}



// ##        #######   ######   
// ##       ##     ## ##    ##  
// ##       ##     ## ##        
// ##       ##     ## ##   #### 
// ##       ##     ## ##    ##  
// ##       ##     ## ##    ##  
// ########  #######   ######   
void LabyrinthSceneRenderer::log(std::wstring ws) {
	OutputDebugString(ws.c_str());
}

void LabyrinthSceneRenderer::log(std::string s) {
	std::wstring ws(s.length(), L' ');
	std::copy(s.begin(), s.end(), ws.begin());
	log(ws);
}

void LabyrinthSceneRenderer::log(Platform::String^ s) {
	OutputDebugString(s->Data());
}

void LabyrinthSceneRenderer::log(char* s, size_t count=0) {
	if (s != nullptr) {
		if (count == 0) {
			std::string str(s);
			log(str);
		}
		else {
			std::string str(s);
			log(str.substr(0,count));;
		}
	}
}

void LabyrinthSceneRenderer::log(char s) {
	std::string str(1,s);
	log(str);
}