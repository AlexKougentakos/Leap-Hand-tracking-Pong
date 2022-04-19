#pragma once
#include <Leap.h>
#include <string>
#include "Vector2f.h"
#include "Texture.h"
class Game: public Leap::Listener
{
public:
	explicit Game( const Window& window );
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game( Game&& other) = delete;
	Game& operator=(Game&& other) = delete;
	~Game();

	void Update( float elapsedSec );
	void Draw( ) const;


	// Event handling
	void ProcessKeyDownEvent( const SDL_KeyboardEvent& e );
	void ProcessKeyUpEvent( const SDL_KeyboardEvent& e );
	void ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e );
	void ProcessMouseDownEvent( const SDL_MouseButtonEvent& e );
	void ProcessMouseUpEvent( const SDL_MouseButtonEvent& e );

	// Callback functions to respond to events dispatched by the Controller object.
	virtual void onFrame(const Leap::Controller&) override;

private:
	// DATA MEMBERS
	const Window m_Window;
	const Point2f m_WindowCenter;

	// FUNCTIONS
	void Initialize( );
	void Cleanup( );
	void ClearBackground( ) const;
	void DrawHands() const;
	Point2f ToWindowCoordinates(const Leap::Vector& vector) const;

	void HandlePlatformAndBallMovement(float elapsedSec);
	void CountDown(float elapsedSec);
	void HandleCollisions();

	void HandleScoring();
	
	// Leap data
	Leap::Controller m_Controller;
	Leap::Frame m_LeapFrame;
	static const int m_NrBones{ 4 };

	//
	bool m_HasStarted;
	bool m_LeftWon{false};
	bool m_RightWon{false};

	Point2f m_HandPosition;
	Point2f m_HandPosition2; 
	Point2f m_HandPositionArr[2];

	Rectf m_LeftRect;
	Rectf m_RightRect;

	Circlef m_Ball;
	Vector2f m_BallVelocity;
	Color4f bgColour;

	int m_ScoreLeft{};
	int m_ScoreRight{};
	float initialVel;
	float m_WaitTime;
	float m_TimePassed;


	std::string m_FontPath;
	std::string m_ScoreString;
	std::string m_TimeString;
	std::string m_WinningString;

	int m_TimeFontSize;
	int m_ScoreFontSize;
	int m_WinFontSize;

	Texture* m_Score;
	Texture* m_Time;
	Texture* m_Win;

};