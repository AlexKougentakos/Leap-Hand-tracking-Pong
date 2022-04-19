#include "pch.h"
#include "Game.h"
#include "utils.h"
Game::Game( const Window& window ) 
	:m_Window{ window }
	, m_WindowCenter{window.width/2,window.height/2}
	, m_LeftRect{50, m_Window.height/2, 50, 100}
	,m_RightRect{ m_Window.width - 50, m_WindowCenter.y, 50, 100 }
	,m_Ball{ window.width / 2,window.height / 2,20 }
	, bgColour{0.1f, 0.1f, 0.1f, 1}
	, initialVel{500.f}
	, m_ScoreString{"0 - 0"}
	,m_WaitTime{3}
	,m_TimePassed{0}
	, m_HasStarted{false}
	, m_FontPath{ "Resources/DIN-Light.otf" }
	, m_TimeFontSize{200}
	, m_ScoreFontSize{40}
	, m_WinFontSize {100}
	, m_WinningString{" wom"}
	
{
	Initialize( );
}

Game::~Game( )
{
	Cleanup( );
}

void Game::Initialize( )
{
	// Have the sample listener receive events from the controller
	m_Controller.addListener(*this);
	m_BallVelocity = Vector2f(initialVel, 50);
	m_Score = new Texture{ m_ScoreString, m_FontPath ,m_ScoreFontSize ,Color4f{1,1,1,1} };

	m_TimeString = std::to_string(int(m_WaitTime));
	m_Time = new Texture(m_TimeString, m_FontPath, m_TimeFontSize, Color4f{ 1,1,1,1 });

	m_Win = new Texture(m_WinningString, m_FontPath, m_WinFontSize, Color4f{ 1,1,1,1 });
}

void Game::Cleanup( )
{
	// Remove the sample listener when done
	m_Controller.removeListener(*this);
	delete m_Score;
	m_Score = nullptr;

	delete m_Time;
	m_Time = nullptr;

	delete m_Win;
	m_Win = nullptr;
}

void Game::Update( float elapsedSec )
{
	CountDown(elapsedSec);
	HandlePlatformAndBallMovement(elapsedSec);
	HandleScoring();
	HandleCollisions();
	if (m_ScoreLeft == 10)
	{
		m_LeftWon = false;
		m_HasStarted = false;
		m_WaitTime = 3;
		m_TimePassed = 0;
		m_ScoreLeft = 0;
		m_ScoreRight = 0;
		m_Ball.center = m_WindowCenter;
		bgColour = Color4f{ 0.1f, 0.1f, 0.1f, 1 };

		m_BallVelocity = Vector2f(initialVel, 50);
		delete m_Score;
		m_ScoreString = "0 - 0";
		m_Score = new Texture{ m_ScoreString, m_FontPath ,m_ScoreFontSize ,Color4f{1,1,1,1} };

		delete m_Time;
		m_TimeFontSize = 200;
		m_TimeString = std::to_string(int(m_WaitTime));
		m_Time = new Texture(m_TimeString, m_FontPath, m_TimeFontSize, Color4f{ 1,1,1,1 });
	}
	if (m_ScoreRight == 10)
	{
		m_RightWon = true;
		m_HasStarted = false;
		m_WaitTime = 3;
		m_TimePassed = 0;
		m_ScoreLeft = 0;
		m_ScoreRight = 0;
		m_Ball.center = m_WindowCenter;
		bgColour = Color4f{ 0.1f, 0.1f, 0.1f, 1 };

		m_BallVelocity = Vector2f(initialVel, 50);
		delete m_Score;
		m_ScoreString = "0 - 0";
		m_Score = new Texture{ m_ScoreString, m_FontPath ,m_ScoreFontSize ,Color4f{1,1,1,1} };

		delete m_Time;
		m_TimeFontSize = 200;
		m_TimeString = std::to_string(int(m_WaitTime));
		m_Time = new Texture(m_TimeString, m_FontPath, m_TimeFontSize, Color4f{ 1,1,1,1 });
	}
	
	
	// Check keyboard state
	//const Uint8 *pStates = SDL_GetKeyboardState( nullptr );
	//if ( pStates[SDL_SCANCODE_RIGHT] )
	//{
	//	std::cout << "Right arrow key is down\n";
	//}
	//if ( pStates[SDL_SCANCODE_LEFT] && pStates[SDL_SCANCODE_UP])
	//{
	//	std::cout << "Left and up arrow keys are down\n";
	//}
}

void Game::HandlePlatformAndBallMovement(float elapsedSec)
{

	int test{};
	for (const Leap::Hand& hand : m_LeapFrame.hands())
	{
		m_HandPositionArr[test] = ToWindowCoordinates(hand.palmPosition());
		m_LeftRect = Rectf{ 50 , m_HandPositionArr[0].y, 20, 80 };

		m_RightRect = Rectf{ m_Window.width - 50 , m_HandPositionArr[1].y, 20, 80 };
		test++;
	}

	if (m_HasStarted)
	{
		m_Ball.center.x += m_BallVelocity.x * elapsedSec;
		m_Ball.center.y += m_BallVelocity.y * elapsedSec;
	}
}

void Game::CountDown(float elapsedSec)
{
	m_TimePassed += elapsedSec;
	if (m_TimePassed >= 1 && !m_HasStarted)
	{
		m_TimePassed = 0;
		m_WaitTime -= 1;

		m_TimeString = std::to_string(int(m_WaitTime));
		m_Time = new Texture(m_TimeString, m_FontPath, int(m_TimeFontSize - 25 * (3 - m_WaitTime)), Color4f{ 1,1,1,1 });
		if (m_WaitTime == 0)
		{
			m_HasStarted = true;
			m_LeftWon = false;
			m_LeftWon = false;
		}
	}

	if (m_LeftWon && !m_HasStarted)
	{
		m_WinningString = "LEFT PLAYER WON!";
		m_Win = new Texture(m_WinningString, m_FontPath, m_WinFontSize, Color4f{ 1,1,1,1 });
	}
	else if (m_RightWon && !m_HasStarted)
	{
		m_WinningString = "RIGHT PLAYER WON!";
		m_Win = new Texture(m_WinningString, m_FontPath, m_WinFontSize, Color4f{ 1,1,1,1 });
	}
}

void Game::HandleCollisions()
{
	if (m_Ball.center.x - m_Ball.radius <= 0.f || m_Ball.center.x + m_Ball.radius >= m_Window.width
		|| utils::IsOverlapping(m_RightRect, m_Ball) || utils::IsOverlapping(m_LeftRect, m_Ball))
	{
		m_BallVelocity.x *= -1.05f;
	}
	if (m_Ball.center.y - m_Ball.radius <= 0.f || m_Ball.center.y + m_Ball.radius >= m_Window.height
		|| utils::IsOverlapping(m_RightRect, m_Ball) || utils::IsOverlapping(m_LeftRect, m_Ball))
	{
		m_BallVelocity.y = 50 * (rand() % 601 - 300) / 100.f;
	}
}

void Game::HandleScoring()
{
	if (m_Ball.center.x <= m_LeftRect.left - 20)
	{
		m_Ball.center = m_WindowCenter;
		m_ScoreRight++;
		m_BallVelocity.x = initialVel;
		bgColour = Color4f{ 0.1f, 0.6f, 0.2f, 1 };


		m_ScoreString = std::to_string(m_ScoreLeft) + "-" + std::to_string(m_ScoreRight);
		delete m_Score;
		m_Score = nullptr;
		m_Score = new Texture{ m_ScoreString,m_FontPath,m_ScoreFontSize,Color4f{ 1,1,1,1 } };


	}
	if (m_Ball.center.x >= m_RightRect.left + 20)
	{
		m_Ball.center = m_WindowCenter;
		m_ScoreLeft++;
		m_BallVelocity.x = initialVel;
		bgColour = Color4f{ 0.7f, 0.1f, 0.2f, 1 };

		m_ScoreString = std::to_string(m_ScoreLeft) + "-" + std::to_string(m_ScoreRight);
		delete m_Score;
		m_Score = nullptr;
		m_Score = new Texture{ m_ScoreString,m_FontPath, m_ScoreFontSize ,Color4f{ 1,1,1,1 } };
	}
}

void Game::Draw( ) const
{
	ClearBackground( );

	m_Score->Draw(Point2f{ m_WindowCenter.x - m_Score->GetWidth() / 2,m_Window.height - (1.5f * m_ScoreFontSize)});
	DrawHands();
	utils::SetColor(Color4f{ 0,1,1,1 });
	utils::FillEllipse(m_Ball.center,m_Ball.radius, m_Ball.radius);
	if(!m_HasStarted)
	m_Time->Draw(Point2f{ m_WindowCenter.x - m_Time->GetWidth() / 2, m_WindowCenter.y - m_Time->GetHeight() / 2 });
	//m_Win->Draw(Point2f{ m_WindowCenter.x - m_Win->GetWidth() / 2, m_WindowCenter.y -  m_Win->GetHeight() });
}

void Game::ProcessKeyDownEvent( const SDL_KeyboardEvent & e )
{
	//std::cout << "KEYDOWN event: " << e.keysym.sym << std::endl;
}

void Game::ProcessKeyUpEvent( const SDL_KeyboardEvent& e )
{
	//std::cout << "KEYUP event: " << e.keysym.sym << std::endl;
	//switch ( e.keysym.sym )
	//{
	//case SDLK_LEFT:
	//	//std::cout << "Left arrow key released\n";
	//	break;
	//case SDLK_RIGHT:
	//	//std::cout << "`Right arrow key released\n";
	//	break;
	//case SDLK_1:
	//case SDLK_KP_1:
	//	//std::cout << "Key 1 released\n";
	//	break;
	//}
}

void Game::ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e )
{
	//std::cout << "MOUSEMOTION event: " << e.x << ", " << e.y << std::endl;
}

void Game::ProcessMouseDownEvent( const SDL_MouseButtonEvent& e )
{
	//std::cout << "MOUSEBUTTONDOWN event: ";
	//switch ( e.button )
	//{
	//case SDL_BUTTON_LEFT:
	//	std::cout << " left button " << std::endl;
	//	break;
	//case SDL_BUTTON_RIGHT:
	//	std::cout << " right button " << std::endl;
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	std::cout << " middle button " << std::endl;
	//	break;
	//}
}

void Game::ProcessMouseUpEvent( const SDL_MouseButtonEvent& e )
{
	//std::cout << "MOUSEBUTTONUP event: ";
	//switch ( e.button )
	//{
	//case SDL_BUTTON_LEFT:
	//	std::cout << " left button " << std::endl;
	//	break;
	//case SDL_BUTTON_RIGHT:
	//	std::cout << " right button " << std::endl;
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	std::cout << " middle button " << std::endl;
	//	break;
	//}
}

void Game::ClearBackground( ) const
{
	glClearColor(bgColour.r, bgColour.g, bgColour.b, bgColour.a );
	glClear( GL_COLOR_BUFFER_BIT );
}

void Game::onFrame(const Leap::Controller& leapController)
{
	// Get the most recent frame 
	m_LeapFrame = leapController.frame();
}

void Game::DrawHands() const
{
	int test{};
	Leap::HandList hands = m_LeapFrame.hands();
	//bool isRed{ true };
	for (const Leap::Hand& hand : m_LeapFrame.hands())
	{
		// Color: alternate between red and green
		
		if (m_HandPositionArr[test].x < m_Window.width / 2.f )
		{
			utils::SetColor(Color4f{ 1.f,0.f,0.f,1.f });
			utils::FillRect(m_LeftRect);
		}
		else 
		{
			utils::SetColor(Color4f{ 0.f,1.f,0.f,1.f });
			utils::FillRect(m_RightRect);
		}
		//isRed = !isRed;

		// Draw palm position 
		
		//utils::FillEllipse(m_HandPosition, 10.f, 10.f);
		test++;

		// Draw the fingers
		const Leap::FingerList fingers = hand.fingers();
	/*	for (const Leap::Finger& finger : fingers)
		{
			for (int boneIdx{}; boneIdx < m_NrBones; ++boneIdx)
			{
				Leap::Bone::Type boneType = static_cast<Leap::Bone::Type>(boneIdx);
				Leap::Bone bone = finger.bone(boneType);
				if (bone.isValid())
				{
					Leap::Vector prevJoint = bone.prevJoint();
					Leap::Vector nextJoint = bone.nextJoint();
					Point2f bonePos1{ ToWindowCoordinates(prevJoint) };
					Point2f bonePos2{ ToWindowCoordinates(nextJoint) };
					utils::DrawLine(bonePos1, bonePos2, 2.f);
				}
			}
		}*/

		// Draw arm
		/*Leap::Arm arm = hand.arm();
		Point2f wristPos{ ToWindowCoordinates(arm.wristPosition()) };
		Point2f elbowPos{ ToWindowCoordinates(arm.elbowPosition()) };
		utils::DrawLine(wristPos, elbowPos, 4.f);*/

	}
}

Point2f Game::ToWindowCoordinates(const Leap::Vector& position) const
{
	return Point2f { m_WindowCenter.x + position.x, m_Window.height - (m_WindowCenter.y + position.z) };
}
