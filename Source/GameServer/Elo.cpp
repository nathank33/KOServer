#include "Elo.h"
#include "stdafx.h"

float elo::EloManager::ScoreToOverPass = 0;
float elo::EloManager::BasicRank = 0;

void elo::EloManager::game(EloPlayer& a, EloPlayer&b, EloResult result) {
	if (result == Win) {
		float rankA = a.getRankF();
		a.versus(b.getRankF(), Win);
		b.versus(rankA, Lose);
	} else if (result == Draw) {
		float rankA = a.getRankF();
		a.versus(b.getRankF(), Draw);
		b.versus(rankA, Draw);
	} else {
		game(b, a, Win);
	}
}
void elo::EloManager::setScoreToOverpass(int score) { ScoreToOverPass = static_cast<float>(score); }
void elo::EloManager::setScoreToOverpass(float score) { ScoreToOverPass = score; }
void elo::EloManager::setBasicRank(int rank) { BasicRank = static_cast<float>(rank); }
void elo::EloManager::setBasicRank(float rank) { BasicRank = rank; }
float elo::EloManager::getScoreToOverpass() { return ScoreToOverPass; }
float elo::EloManager::getBasicRank() { return BasicRank; }

elo::EloPlayer::EloPlayer() {
	{ mRank = EloManager::getBasicRank(); mGamePlayed = 0; mOverpassed = false; }
}

elo::EloPlayer::EloPlayer(float rank, int gamePlayed, bool overpassed) : mRank(rank), mGamePlayed(gamePlayed), mOverpassed(overpassed) {}

float elo::EloPlayer::versus(float oRank, EloResult result) {
	float e = getE(oRank);
	float s = getS(result);
	float k = getK();
	mRank = mRank + k * (s - e);
	update();
	return mRank;
}

int elo::EloPlayer::getRank() { return static_cast<int>(mRank); }
float elo::EloPlayer::getRankF() { return mRank; }
int elo::EloPlayer::getGamePlayed() { return mGamePlayed; }
bool elo::EloPlayer::hasOverpassed() { return mOverpassed; }

void elo::EloPlayer::update() {
	mGamePlayed++;
	if (mRank >= EloManager::getScoreToOverpass()) {
		mOverpassed = true;
	}
	mRank = (float) (mRank + 0.5);
}

float elo::EloPlayer::getE(float oRank) {
	return getQ(mRank) / (getQ(mRank) + getQ(oRank));
}

float elo::EloPlayer::getQ(float rank) {
	return (float) pow(10, (double) rank / 400);
}

float elo::EloPlayer::getS(EloResult result) {
	switch (result) {
	case EloResult::Win: return 1;
	case EloResult::Draw: return 0.5;
	default: return 0;
	}
}

float elo::EloPlayer::getK() {
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
elo::EloPlayer David(1200,0,false);
elo::EloPlayer Goliath(2500,50,true);

elo::EloManager::game(Goliath,David);

std::cout << "Goliath win !" << std::endl;
std::cout << "David : " << David.getRank() << std::endl;
std::cout << "Goliath : " << Goliath.getRank() << std::endl;

std::cout << std::endl;

elo::EloManager::game(David,Goliath);

std::cout << "David win !" << std::endl;
std::cout << "David : " << David.getRank() << std::endl;
std::cout << "Goliath : " << Goliath.getRank() << std::endl;


return 0;
}
*/