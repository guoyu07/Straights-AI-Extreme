
/*
 *  Represents a command to the game.
 *
 *  Last updated: 2017-05-24
 *  Copyright 2017 UW. All rights reserved.
 *
 */
#ifndef _COMMAND_
#define _COMMAND_

#include "Card.h"
#include <istream>

using std::string;

struct Command {
    enum class Type { NEXT_ROUND, NOTHING, COMPUTER, CLICK, PLAY, DISCARD, DECK, QUIT, RAGEQUIT, BAD_COMMAND };
	Type type;
	Card card;

	Command();

	Command(std::string cmd, Card card);

	// Ensures: Command is set to invalid state, card is set to AC
};

std::istream & operator>>( std::istream &, Command & );
// Modifies: input stream and Command
// Ensures: Command is set to a valid command and has its associated card argument
// Throws: fails assertion if unable to read in a valid command and its associated card argument,
//    if applicable.
// Returns: input stream

#endif
