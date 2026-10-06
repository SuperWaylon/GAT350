#pragma once
#include "Framework/Game.h"
#include "Renderer/TextRenderer.h"
#include "Resources/ResourceManager.h"

class SpaceGame : public nu::Game
{
public:
	enum class GameState
	{
		Title,
		StartGame,
		StartLevel,
		Game,
		GameOver
	};

public:
	SpaceGame() = default;

	bool Initialize() override;

	void Update(float dt) override;
	void Draw(class nu::Renderer& renderer) override;

	void OnPlayerDead();
	void AddPoints(int points) { m_score += points; }

private:
	void SpawnPlayer();
	void SpawnEnemy();

private:
	int m_score{ 0 };
	int m_lives{ 0 };

	float m_stateTimer = 0.0f;

	float m_spawnTimer = 0.0f;
	float m_spawnTime = 5.0f;
	int m_spawnCount = 0;

	GameState m_gameState = GameState::Title;
		
	nu::TextRenderer* m_titleText{ nullptr };
	nu::TextRenderer* m_gameOverText{ nullptr };

	nu::TextRenderer* m_scoreText{ nullptr };
	nu::TextRenderer* m_livesText{ nullptr };
};
