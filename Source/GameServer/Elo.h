#pragma once

#ifndef ELO
#define ELO

#include <iostream>
#include <cmath>

namespace elo {
	enum EloResult {
		Win,
		Draw,
		Lose,
	};

	class EloPlayer {
	public:
		EloPlayer();
		EloPlayer(float rank, int gamePlayed, bool overpassed);

		float versus(float oRank, EloResult EloResult);

		int getRank();
		float getRankF();
		int getGamePlayed();
		bool hasOverpassed();

	private:
		float getE(float oRank);
		float getS(EloResult EloResult);
		float getK();
		float getQ(float rank);
		void update();

	private:
		float mRank;
		int mGamePlayed;
		bool  mOverpassed;

	};

	class EloManager {
	private:
		static float ScoreToOverPass;
		static float BasicRank;
	public:
		static void game(EloPlayer& a, EloPlayer&b, EloResult EloResult);
		static void setScoreToOverpass(int score);
		static void setScoreToOverpass(float score);
		static void setBasicRank(int rank);
		static void setBasicRank(float rank);
		static float getScoreToOverpass();
		static float getBasicRank();

	};
}
#endif 
