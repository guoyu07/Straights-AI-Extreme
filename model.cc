#include "model.h"
#include "Human.h"
#include "Computer.h"
#include <algorithm>

using namespace std;

Model::Model() : intstable_(4, std::vector<int>(15, 0)) , cardstable_(4, std::vector<Card>(15)) {}

void Model::startGame(int seed, std::vector<char> players) {
  for (int i = 0; i < 4; i++) {
    if (players[i] == 'c') {
      Computer *a = new Computer();
      players_.push_back(a);
    }
    else {
      Human *a = new Human();
      players_.push_back(a);
    }
  }
  deck_ = Deck(seed);
  initializeRound();
}

const std::vector<std::vector<Card> > Model::getCardTable() {
  return cardstable_;
}

const std::vector<std::vector<int> > Model::getIntTable() {
  return intstable_;
}

std::vector<int> Model::getPlayerScores() {
  std::vector<int> scores;
  for (auto player : players_) {
    scores.push_back(player->getTotalScore());
  }
  return scores;
}

std::vector<int> Model::getPlayerDiscards() {
  std::vector<int> discards;
  for (auto player : players_) {
    discards.push_back(player->getDiscards().size());
  }
  return discards;
}

const int Model::getCurrentPlayerIndex() {
  return playerturn_;
}

void Model::rageQuit() {
  Computer *replacement = nullptr;
  int i = getCurrentPlayerIndex();
  replacement = new Computer(*(players_[i]));
  delete players_[i];
  players_[i] = replacement;
  Command c = players_[getCurrentPlayerIndex()]->makeMove(getLegalPlays());
  if (c.type == Command::Type::PLAY) {
    playCard(c.card);
  }
  else {
    discardCard(c.card);
  }
}

void Model::incrementPlayerTurn() {
  if (emptyhands_ == 4) endRound();
  const int max_index = 3;
  playerturn_++;
  if (playerturn_ > max_index) playerturn_ = 0;
  while (players_[playerturn_]->getHand().size() == 0) {
    playerturn_++;
    if (playerturn_ > max_index) playerturn_ = 0;
  }
  if (players_[playerturn_]->getType() == 'c') {
    Command c = players_[getCurrentPlayerIndex()]->makeMove(getLegalPlays());
    if (c.type == Command::Type::PLAY) {
      playCard(c.card);
    }
    else {
      discardCard(c.card);
    }
  }
  else {
    notify();
  }
  return;
}

const Player* Model::getCurrentPlayer() {
  return (players_[playerturn_]);
}

const std::vector<Card> Model::getLegalPlays() {
  std::vector<Card> plays;
  for (auto card : players_[getCurrentPlayerIndex()]->getHand()) {
    int suit = card.suit().suit();
    int rank = card.rank().rank();
    if ((intstable_[suit][rank] == 1 || intstable_[suit][rank + 2] == 1) || (rank == 6)) {
      plays.push_back(card);
    }
    if (suit == 3 && rank == 6) {
      std::vector<Card> single_play;
      single_play.push_back(card);
      return single_play;
    }
  }
  return plays;
}

const std::vector<Card> Model::getPlayerHand() {
  std::vector<Card> hand;
  for (auto card : players_[getCurrentPlayerIndex()]->getHand()) {
    hand.push_back(card);
  }
  return hand;
}

const int Model::getGameState() {
  // 1 = Round has ended 2 = Game has ended
  return gamestate_;
}

void Model::initializeRound() {
  gamestate_ = 0;
  playerturn_ = 0;
  emptyhands_ = 0;
  deck_.shuffle();
  for (auto player : players_) {
    player->emptyHand();
  }
  for (int j = 0; j < players_.size(); j++) {
    for (int i = 0; i < 13; i ++) {
      players_[j]->dealCard(deck_.getCard(i + 13*j));
    }
  }
  for (int i = 0; i < players_.size(); i++) {
    for (auto card : players_[i]->getHand()) {
      if (card.suit().suit() == 3 && card.rank().rank() == 6) playerturn_ = i;
    }
  }
  notify();
}

void Model::endRound() {
  gamestate_ = 1;
  std::vector<int> scores;
  for (auto player : players_) {
    for (auto card : player->getDiscards()) {
      player->setRoundScore(player->getRoundScore() + (card.rank().rank() + 1));
    }
    player->setTotalScore(player->getTotalScore() + player->getRoundScore());
  }
  for (auto player : players_) {
    if (player->getTotalScore() >= 80) gamestate_ = 2;
  }
  for (int i = 0; i < intstable_.size(); i++) {
    for (int j = 0; j < intstable_[i].size(); j++) {
      intstable_[i][j] = 0;
    }
  }
  notify();
}

Model::~Model() {}

void Model::playCard(Card c) {
  int suit = c.suit().suit();
  int rank = c.rank().rank();
  players_[getCurrentPlayerIndex()]->play(c);
  intstable_[suit][rank + 1] = 1;
  cardstable_[suit][rank + 1] = c;
  if (players_[getCurrentPlayerIndex()]->getHand().size() == 0) emptyhands_++;
  incrementPlayerTurn();
  return;
}

void Model::discardCard(Card c) {
  int suit = c.suit().suit();
  int rank = c.rank().rank();
  players_[getCurrentPlayerIndex()]->discard(c);
  if (players_[getCurrentPlayerIndex()]->getHand().size() == 0) emptyhands_++;
  incrementPlayerTurn();
  return;
}

const std::vector<Card> Model::getDeck() {
  return deck_.getDeck();
}
