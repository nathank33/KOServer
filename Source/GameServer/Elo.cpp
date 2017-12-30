#include "Elo.h"
#include "stdafx.h"

void elo::Manager::game(Player& a, Player&b, Result result)
{
	if (result == Win)
	{
		float rankA = a.getRankF();
		a.versus(b.getRankF(), Win);
		b.versus(rankA, Lose);
	}
	else if (result == Draw)
	{
		float rankA = a.getRankF();
		a.versus(b.getRankF(), Draw);
		b.versus(rankA, Draw);
	}
	else
	{
		game(b, a, Win);
	}
}
void elo::Manager::setScoreToOverpass(int score) { ScoreToOverPass = static_cast<float>(score); }
void elo::Manager::setScoreToOverpass(float score) { ScoreToOverPass = score; }
void elo::Manager::setBasicRank(int rank) { BasicRank = static_cast<float>(rank); }
void elo::Manager::setBasicRank(float rank) { BasicRank = rank; }
float elo::Manager::getScoreToOverpass() { return ScoreToOverPass; }
float elo::Manager::getBasicRank() { return BasicRank; }

elo::Player::Player()
{
	{ mRank = Manager::getBasicRank(); mGamePlayed = 0; mOverpassed = false; }
}

elo::Player::Player(float rank, int gamePlayed, bool overpassed) : mRank(rank), mGamePlayed(gamePlayed), mOverpassed(overpassed) {}

float elo::Player::versus(float oRank, Manager::Result result)
{
	float e = getE(oRank);
	float s = getS(result);
	float k = getK();
	mRank = mRank + k * (s - e);
	update();
	return mRank;
}

int elo::Player::getRank() { return static_cast<int>(mRank); }
float elo::Player::getRankF() { return mRank; }
int elo::Player::getGamePlayed() { return mGamePlayed; }
bool elo::Player::hasOverpassed() { return mOverpassed; }

void elo::Player::update()
{
	mGamePlayed++;
	if (mRank >= Manager::getScoreToOverpass())
	{
		mOverpassed = true;
	}
	int a = (int)(mRank + 0.5);
	mRank = a;
}

float elo::Player::getE(float oRank) 
{ 
	return getQ(mRank) / (getQ(mRank) + getQ(oRank)); 
}

float elo::Player::getQ(float rank)
{ 
	return pow(10, rank / 400); 
}

float elo::Player::getS(Manager::Result result)
{
	switch (result)
	{
	case Manager::Result::Win: return 1;
	case Manager::Result::Draw: return 0.5;
	default: return 0;
	}
}

float elo::Player::getK()
{
	if (mGamePlayed < 30)
		return 30;
	else if (!mOverpassed)
		return 15;
	return 10;
}

/*
example usage:
#include <iostream>
#include "EloCpp.hpp"

int main()
{
elo::Player David(1200,0,false);
elo::Player Goliath(2500,50,true);

elo::Manager::game(Goliath,David);

std::cout << "Goliath win !" << std::endl;
std::cout << "David : " << David.getRank() << std::endl;
std::cout << "Goliath : " << Goliath.getRank() << std::endl;

std::cout << std::endl;

elo::Manager::game(David,Goliath);

std::cout << "David win !" << std::endl;
std::cout << "David : " << David.getRank() << std::endl;
std::cout << "Goliath : " << Goliath.getRank() << std::endl;


return 0;
}
*/