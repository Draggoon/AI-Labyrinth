#include "pch.h"
#include "LabyrinthSceneRenderer.h"

#include "Common/DirectXHelper.h"

using namespace Labyrinth;

LabyrinthSceneRenderer::LabyrinthSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_deviceResources(deviceResources),
	m_labyrinthPatternFileName("LabyrinthPattern.txt"),
	m_sizeX(0),
	m_sizeY(0),
	m_cellWidth(100.0f),
	m_cellHeight(100.0f),
	m_originX(0),
	m_originY(0),
	m_playerCount(4) {

	// Create device independent resources

	DX::ThrowIfFailed(
		m_deviceResources->GetD2DFactory()->CreateDrawingStateBlock(&m_stateBlock)
	);

	createDeviceDependentResources();

	for (int i(0); i < m_playerCount; ++i) {
		m_cursorsX.push_back(0);
		m_cursorsY.push_back(0);
	}

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

}


//	########  ######## ##    ## ########  ######## ########  
//	##     ## ##       ###   ## ##     ## ##       ##     ## 
//	##     ## ##       ####  ## ##     ## ##       ##     ## 
//	########  ######   ## ## ## ##     ## ######   ########  
//	##   ##   ##       ##  #### ##     ## ##       ##   ##   
//	##    ##  ##       ##   ### ##     ## ##       ##    ##  
//	##     ## ######## ##    ## ########  ######## ##     ## 
void LabyrinthSceneRenderer::render() {
	ID2D1DeviceContext* context = m_deviceResources->GetD2DDeviceContext();
	Windows::Foundation::Size logicalSize = m_deviceResources->GetLogicalSize();

	context->SaveDrawingState(m_stateBlock.Get());
	context->BeginDraw();

	// Fit to screen
	D2D1::Matrix3x2F screenScale = D2D1::Matrix3x2F::Scale(logicalSize.Width / (m_sizeX*m_cellWidth), logicalSize.Height / (m_sizeY*m_cellHeight));

	context->SetTransform(screenScale * m_deviceResources->GetOrientationTransform2D());

	// Draw
	D2D1_RECT_F rect;
	for (int i(0); i < m_sizeY; ++i) {
		rect.top = i * m_cellHeight;
		rect.bottom = rect.top + m_cellHeight;
		for (int j(0); j < m_sizeX; ++j) {
			rect.left = j * m_cellWidth;
			rect.right = rect.left + m_cellWidth;
			if (m_labyrinth[i][j] == '#')
				context->FillRectangle(rect, m_blackBrush.Get());
			else if (m_labyrinth[i][j] == 'O')
				context->FillRectangle(rect, m_greenBrush.Get());
			else if (m_labyrinth[i][j] == 'E')
				context->FillRectangle(rect, m_redBrush.Get());
		}
	}
	D2D1_POINT_2F p1, p2;
	int sqrtNbPlayer(ceil(sqrt(m_playerCount)));
	for (int p(0); p < m_playerCount; ++p) {
		p1.x = m_cursorsX[p] * m_cellWidth + m_cellWidth / 20.0f + (p%sqrtNbPlayer)*m_cellWidth/sqrtNbPlayer;
		p1.y = m_cursorsY[p] * m_cellHeight + m_cellHeight / 20.0f + (p/sqrtNbPlayer)*m_cellHeight/sqrtNbPlayer;
		p2.x = m_cursorsX[p] * m_cellWidth - m_cellWidth / 20.0f + (p%sqrtNbPlayer + 1)*m_cellWidth/sqrtNbPlayer;
		p2.y = m_cursorsY[p] * m_cellHeight - m_cellHeight / 20.0f+ (p/sqrtNbPlayer + 1)*m_cellHeight/sqrtNbPlayer;
		context->DrawLine(p1, p2, m_blackBrush.Get(), 10.0f/(sqrtNbPlayer*sqrtNbPlayer));
		p1.x = m_cursorsX[p] * m_cellWidth - m_cellWidth / 20.0f + (p%sqrtNbPlayer + 1)*m_cellWidth / sqrtNbPlayer;
		p1.y = m_cursorsY[p] * m_cellHeight + m_cellHeight / 20.0f + (p / sqrtNbPlayer)*m_cellHeight / sqrtNbPlayer;
		p2.x = m_cursorsX[p] * m_cellWidth + m_cellWidth / 20.0f + (p%sqrtNbPlayer)*m_cellWidth / sqrtNbPlayer;
		p2.y = m_cursorsY[p] * m_cellHeight - m_cellHeight / 20.0f + (p/sqrtNbPlayer + 1)*m_cellHeight / sqrtNbPlayer;
		context->DrawLine(p1, p2, m_blackBrush.Get(), 10.0f/(sqrtNbPlayer*sqrtNbPlayer));
	}

	// Ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
	// is lost. It will be handled during the next call to Present.
	HRESULT hr = context->EndDraw();
	if (hr != D2DERR_RECREATE_TARGET) {
		DX::ThrowIfFailed(hr);
	}

	context->RestoreDrawingState(m_stateBlock.Get());
}

void LabyrinthSceneRenderer::moveUp(int player) {
	moveTo(m_cursorsX[player], m_cursorsY[player] -1, player);
}
void LabyrinthSceneRenderer::moveDown(int player) {
	moveTo(m_cursorsX[player], m_cursorsY[player] + 1, player);
}
void LabyrinthSceneRenderer::moveLeft(int player) {
	moveTo(m_cursorsX[player]-1, m_cursorsY[player], player);
}
void LabyrinthSceneRenderer::moveRight(int player) {
	moveTo(m_cursorsX[player]+1, m_cursorsY[player], player);
}

void LabyrinthSceneRenderer::moveTo(int x, int y, int player) {
	if (x < m_sizeX && x >= 0 && y < m_sizeY && y >= 0) {
		if (m_labyrinth[y][x] != '#') {
			m_cursorsX[player] = x;
			m_cursorsY[player] = y;
			if (m_labyrinth[y][x] == 'E')
				reloadFromFile();
		}
	}
}

void LabyrinthSceneRenderer::addPlayer() {
	m_cursorsX.push_back(m_originX);
	m_cursorsY.push_back(m_originY);
	++m_playerCount;
}

void LabyrinthSceneRenderer::removePlayer(int player) {
	if (m_playerCount > 1) {
		if (player == -1) {
			m_cursorsX.pop_back();
			m_cursorsY.pop_back();
			--m_playerCount;
		}
		else if (player < m_playerCount) {
			m_cursorsX.erase(m_cursorsX.begin() + player);
			m_cursorsY.erase(m_cursorsY.begin() + player);
			--m_playerCount;
		}
	}
}


//	##        #######     ###    ########  
//	##       ##     ##   ## ##   ##     ## 
//	##       ##     ##  ##   ##  ##     ## 
//	##       ##     ## ##     ## ##     ## 
//	##       ##     ## ######### ##     ## 
//	##       ##     ## ##     ## ##     ## 
//	########  #######  ##     ## ########  
void LabyrinthSceneRenderer::reloadFromFile() {
	loadLabyrinthFromFile(m_labyrinthPatternFileName);
}

void LabyrinthSceneRenderer::loadLabyrinthFromFile(std::string filename) {
	// Retrieving data from the file
	std::string str;
	std::ifstream fstr(filename);
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
	std::vector<char> vec;
	while (str.size() > 0) {
		if (str.size() > 1) {
			m_labyrinth.push_back(vec);
			++m_sizeY;
		}
		while (str.front() != '\n') {
			if (str.front() == 'o' || str.front() == 'O') {
				for (int i(0); i < m_playerCount; ++i) {
					m_originX = (int)m_labyrinth.back().size();
					m_originY = (int)m_labyrinth.size() - 1;
					m_cursorsX[i] = m_originX;
					m_cursorsY[i] = m_originY;
				}
				str.front() = 'O';
			}
			if (str.front() == 'e' || str.front() == 'E') {
				str.front() = 'E';
			}
			m_labyrinth.back().push_back(str.front());
			str.erase(0, 1);
			++tX;
			if (str.size() == 0)
				break;
		}
		if (str.size() > 0)
			str.erase(0, 1);
		if (tX > m_sizeX)
			m_sizeX = tX;
		tX = 0;
	}

	// Adding walls to the end of the shorts lines
	for (size_t i(0); i < m_labyrinth.size(); ++i) {
		while (m_labyrinth[i].size() < m_sizeX)
			m_labyrinth[i].push_back('#');
	}

	// Display to the console
	log("Size : x=" + m_sizeX + ", y=" + m_sizeY + "\n");
	for (int i(0); i < m_labyrinth.size(); ++i) {
		for (int j(0); j < m_labyrinth[i].size(); ++j)
			log(m_labyrinth[i][j]);
		log('\n');
	}
	log("cursor at (" + m_cursorsX[0] + ";" + m_cursorsY[0] + ").\n");

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