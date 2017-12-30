#pragma once

#ifndef ELO
#define ELO

#include <iostream>
#include <cmath>


namespace elo
{

	class Player
	{
	public:
		Player();
		Player(float rank, int gamePlayed, bool overpassed) : mRank(rank), mGamePlayed(gamePlayed), mOverpassed(overpassed) {}

		float versus(float oRank, Manager::Result result);

		int getRank();
		float getRankF();
		int getGamePlayed();
		bool hasOverpassed();

	private:
		float getE(float oRank);
		float getS(Manager::Result result);
		float getK();
		float getQ(float rank);
		void update();

	private:
		float mRank;
		int mGamePlayed;
		bool  mOverpassed;

	};

	class Manager
	{
	private:
		static float ScoreToOverPass;
		static float BasicRank;

	public:

		enum Result
		{
			Win,
			Draw,
			Lose,
		};


			static void game(Player& a, Player&b, Result result);
			static void setScoreToOverpass(int score);
			static void setScoreToOverpass(float score);
			static void setBasicRank(int rank);
			static void setBasicRank(float rank);
			static float getScoreToOverpass();
			static float getBasicRank();

	};

}

#endif // ELOCPP_HPP_INCLUDED
