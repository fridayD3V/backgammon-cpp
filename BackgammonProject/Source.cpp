#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h> 
#include <stdlib.h>
#include "conio2.h"
#include <time.h>

#define FIELD_SIZE 24
#define X_BOARD 43
#define Y_BOARD 15

struct s_dices {
	int dice1;
	int dice2;
	int dice1Available;
	int dice2Available;
	int availableMoves;
	int isItDouble;
	int movesAtBegin;
	int isItCheck;
};
typedef struct s_dices dices;

struct s_column {
	int n;			//number of column
	int number;		//number of coins
	int status;		//0 - none, 1 - white, 2 - red
	int direction;	//0 - down, 1 - up
};
typedef struct s_column column;

struct s_gameInf {
	char zn;
	int isGameEnded;
	int whiteOnBoard;
	int redOnBoard;
	int isEveryAtHome;
	int isGameStarted;
	int turn; //0 white, 1 red
	int moveIsNotEnded;
	int isLegalMoveInt;
	int startPos;
	int endPos;
	int whiteOnBar;
	int redOnBar;
	dices dice;
};
typedef struct s_gameInf gameInf;

void createField(column (*field)[FIELD_SIZE]) {
	for (int i = 0; i < FIELD_SIZE / 2; i++) {
		(*field)[i] = { 13+i, 0, 0, 0 };
	}
	for (int i = FIELD_SIZE / 2; i < FIELD_SIZE; i++) {
		(*field)[i] = { FIELD_SIZE-i, 0, 0, 1 };
	}

	(*field)[0].number = 5;
	(*field)[0].status = 1;
	(*field)[4].number = 3;
	(*field)[4].status = 2;
	(*field)[6].number = 5;
	(*field)[6].status = 2;
	(*field)[11].number = 2;
	(*field)[11].status = 1;
	(*field)[12].number = 5;
	(*field)[12].status = 2;
	(*field)[16].number = 3;
	(*field)[16].status = 1;
	(*field)[18].number = 5;
	(*field)[18].status = 1;
	(*field)[23].number = 2;
	(*field)[23].status = 2;
}

void chooseColor(int status) {
	switch (status) {
	case 2:
		textcolor(RED);
		break;
	default:
		textcolor(WHITE);
		break;
	}
}

void drawHLine(char s, char edge) {
	printf("%c", edge);
	for (int x = 0; x < X_BOARD; x++) {
		printf("%c", s);
	}
	printf("%c", edge);
}

void drawVLine(int x) {
	for (int y = 0; y < Y_BOARD; y++) {
		gotoxy(x, y + 3);
		printf("|");
	}
}

void drawHalf(column field[FIELD_SIZE], int x, int y, int start) {
	int space = 0;
	int number = 0;
	int i = start;

	while (i != start + (FIELD_SIZE / 2)) {
		number = field[i].number;
		for (int j = 0; j < FIELD_SIZE / 4; j++) {
			gotoxy(x + space + (3 * (i-start)), y + (j-j*field[i].direction) + (5-(1*j))*field[i].direction);
			if (number != 0) {
				chooseColor(field[i].status);
				printf("()");
				textcolor(WHITE);
				number--;
			}
			else if (number == 0 || field[i].status == 0) {
				printf("::");
			}
		}
		if ((i-start) == 5) {
			space += 6;
		}
		i++;
	}
}

void drawBar(gameInf* game) {
	for (int y = 0; y < Y_BOARD; y++) {
		gotoxy(20, y+3);
		if (y == Y_BOARD/2) {
			printf(" |BAR|");
		}
		else {
			printf(" |   |");
		}
	}
	if (game->whiteOnBar > 0) {
		gotoxy(22, 8);
		printf("(%d)", game->whiteOnBar);
	}

if (game->redOnBar > 0) {
		textcolor(RED);
		gotoxy(22, 12);
		printf("(%d)", game->redOnBar);
		textcolor(WHITE);
	}
}

int dice() {
	int rnum;
	rnum = rand()%6 + 1;
	return rnum;
}

void drawInfo(gameInf *game) {
	gotoxy(1, 21);
	textcolor(YELLOW);
	printf("Turn: ");
	if (game->turn == 0) {
		textcolor(WHITE);
	}
	else {
		textcolor(RED);
	}
	printf("()");
	textcolor(WHITE);
	if (game->isLegalMoveInt == 0) {
		printf("\tIllegal move.");
	}

	gotoxy(1, 22);
	if (game->moveIsNotEnded == 0) {
		textcolor(YELLOW);
		printf("Press ");
		textcolor(GREEN);
		printf("[M]");
		textcolor(YELLOW);
		printf(" to move");
		textcolor(WHITE);
	}
	else if (game->moveIsNotEnded == 1) {
		printf("Dice: [%d] [%d] (Moves: %d)", game->dice.dice1, game->dice.dice2, game->dice.availableMoves);
	}
}

void drawNumbers(int x, int y, int startNumber) {
	for (int i = 0; i < FIELD_SIZE / 2; i++) {
		gotoxy(x, y);
		printf("%d", startNumber + i*(-1 + 2*(startNumber%2)));
		if (i == 5) {
			x += 6;
		}
		x += 3;
	}
}

void drawCmds(int isGameStarted = 1) {
	gotoxy(X_BOARD+20, Y_BOARD/2+2);
	if (isGameStarted) {
		printf("[S]ave");
	}
	else {
		printf("Press [P] to start...");
	}
	gotoxy(X_BOARD + 20, Y_BOARD / 2 + 3);
	printf("[L]oad");
	gotoxy(X_BOARD + 20, Y_BOARD / 2 + 4);
	printf("[Q]uit");
}

void drawField(column field[FIELD_SIZE], gameInf *game) {
	textcolor(WHITE);

	gotoxy(1, 1);
	drawNumbers(3, 1, 13);
	gotoxy(1, 2);
	drawHLine('=', '+');
	drawVLine(1);

	drawHalf(field, 3, 3, 0);
	drawBar(game);
	drawHalf(field, 3, 12, 12);

	gotoxy(1, 18);
	drawHLine('=', '+');
	drawVLine(45);
	drawNumbers(3, 19, 12);

	drawCmds();
	drawInfo(game);
}

int findIndex(gameInf* game, column field[FIELD_SIZE], int number) {
	int index = 0;
	for (int i = 0; i < FIELD_SIZE; i++) {
		if (field[i].n == number) {
			index = i;
			return index;
		}
	}
	return -1;
}

int checkStatusForCapture(gameInf* game, column field[FIELD_SIZE], int status) {
	if (field[findIndex(game, field, game->startPos - (game->dice.dice2) * (3 - (2 * status)))].number == 1 && game->dice.isItDouble) {
		return 1;
	}
	return 0;
}

int checkStatusesWithoutDouble(gameInf* game, column field[FIELD_SIZE], int status) {
	if (field[findIndex(game, field, game->startPos - (game->dice.dice1) * (3 - (2 * status)))].status == status) {
		return 1;
	}
	else if (field[findIndex(game, field, game->startPos - (game->dice.dice1) * (3 - (2 * status)))].status == 0) {
		return 1;
	}
	else if (field[findIndex(game, field, game->startPos - (game->dice.dice2) * (3 - (2 * status)))].status == status) {
		return 1;
	}
	else if (field[findIndex(game, field, game->startPos - (game->dice.dice2) * (3 - (2 * status)))].status == 0) {
		return 1;
	}
	else if (checkStatusForCapture(game, field, status)) {
		return 1;
	}
	return 0;
}

int checkStatuses(gameInf* game, column field[FIELD_SIZE], int numberOfSteps, int status) { // 1 - true
	if (game->dice.isItDouble && numberOfSteps > 1) {
		for (int i = 1; i < numberOfSteps; i++) {
			if (field[findIndex(game, field, game->startPos - i*(game->dice.dice1)*(3-(2*status)))].status != status) {
				if (field[findIndex(game, field, game->startPos - i*(game->dice.dice1)*(3-(2*status)))].status != 0) {
					return 0;
				}
			}
		}
		return 1;
	}
	else if (checkStatusesWithoutDouble(game, field, status)) { //check places between two coins
		return 1;
	}
	return 0;
}

int checkDirection(gameInf* game, column field[FIELD_SIZE], int status) { // 1 - illegal, 0 - legal
	if (game->endPos != game->startPos - (game->dice.dice1 * (3 - (2 * status)))) {
		if (game->endPos != game->startPos - (game->dice.dice2* (3 - (2 * status)))) {
			if (game->endPos != game->startPos - ((game->dice.dice1+game->dice.dice2)*(3 - (2 * status)))) {
				if (game->endPos != game->startPos - ((game->dice.dice1*3)* (3 - (2 * status)))) {
					if (game->endPos != game->startPos - ((game->dice.dice1*4) * (3 - (2 * status)))) {
						return 1;
					}
				}
			}
		}
	}
	return 0;
}

int checkAvailabilityBigMove(gameInf* game, column field[FIELD_SIZE], int status, int step, int numberOfSteps) {
	if (step == game->dice.dice2 + game->dice.dice1 && (game->dice.dice1Available == 1 && game->dice.dice2Available == 1)) {
		if (checkStatuses(game, field, numberOfSteps, status)) {
			game->dice.availableMoves -= 2;
			if (game->dice.isItCheck == 0) {
				game->dice.dice1Available--;
				game->dice.dice2Available--;
			}
			return 1;
		}
	}
	return 0;
}

int checkAvailabilityNotDouble(gameInf* game, column field[FIELD_SIZE], int status, int step, int numberOfSteps) {
	if (step == game->dice.dice1 && game->dice.dice1Available == 1) {
		game->dice.availableMoves--;
		if (game->dice.isItCheck == 0)
			game->dice.dice1Available--;
		return 1;
	}
	else if (step == game->dice.dice2 && game->dice.dice2Available == 1) {
		game->dice.availableMoves--;
		if (game->dice.isItCheck == 0)
			game->dice.dice2Available--;
		return 1;
	}
	else if (checkAvailabilityBigMove(game, field, status, step, numberOfSteps)) {
		return 1;
	}
	return 0;
}

int checkAvailabilityMoreThan2(gameInf* game, column field[FIELD_SIZE], int status, int numberOfSteps) {
	if (game->dice.availableMoves >= numberOfSteps && checkStatuses(game, field, numberOfSteps, status)) {
		game->dice.availableMoves -= numberOfSteps;
		return 1;
	}
	return 0;
}

int checkAvailability(gameInf* game, column field[FIELD_SIZE], int status) {
	int step = game->startPos*(3 - 2 * status) - game->endPos*(3 - 2 * status);
	int numberOfSteps = step / game->dice.dice1;
	if (game->dice.availableMoves > 2 || (game->dice.availableMoves == 2 && game->dice.isItDouble == 1)) {
		if (checkAvailabilityMoreThan2(game, field, status, numberOfSteps)) {
			return 0;
		}
	}
	else {
		if (game->dice.isItDouble == 1) {
			if (game->dice.availableMoves >= numberOfSteps) {
				game->dice.availableMoves -= numberOfSteps;
				return 0;
			}
		}
		else if (checkAvailabilityNotDouble(game, field, status, step, numberOfSteps)) {
			return 0;
		}
	}
	return 1;
}

int checkMove(gameInf* game, column field[FIELD_SIZE], int index1) { // 1 - illegal, 0 - legal
	if (checkDirection(game, field, field[index1].status)) {
		return 1;
	}
	else if (checkAvailability(game, field, field[index1].status)) {
		return 1;
	}
	return 0;
}

void checkHome(gameInf* game, column field[FIELD_SIZE]) {
	int number = 0;
	if (game->turn == 0) {
		for (int i = 18; i < FIELD_SIZE; i++) {
			if (field[i].status == 1) {
				number += field[i].number;
			}
		}
		if (number == game->whiteOnBoard) {
			game->isEveryAtHome = 1;
		}
		else {
			game->isEveryAtHome = 0;
		}
	}
	else if (game->turn == 1) {
		for (int i = FIELD_SIZE/4; i < FIELD_SIZE/2; i++) {
			if (field[i].status == 2) {
				number += field[i].number;
			}
		}
		if (number == game->redOnBoard) {
			game->isEveryAtHome = 1;
		}
	}
}

int checkIsItLessRed2(gameInf* game, column field[FIELD_SIZE]) {
	for (int i = 1; i < game->dice.dice2; i++) {
		if (field[FIELD_SIZE / 2 - game->dice.dice2 + i].number > 0) {
			if (game->startPos == (FIELD_SIZE + 1) - game->dice.dice2 + i && game->dice.dice2Available == 1) {
				game->dice.availableMoves--;
				game->dice.dice2Available--;
				game->redOnBoard--;
				return 1;
			}
			break;
		}
	}
	return 0;
}

int checkIsItLessRed1(gameInf* game, column field[FIELD_SIZE]) {
	for (int i = 1; i < game->dice.dice1; i++) {
		if (field[FIELD_SIZE / 2 - game->dice.dice1 + i].number > 0) {
			if (game->startPos == (FIELD_SIZE + 1) - game->dice.dice1 + i && game->dice.dice1Available == 1) {
				game->dice.availableMoves--;
				game->dice.dice1Available--;
				game->redOnBoard--;
				return 1;
			}
			break;
		}
	}
	return 0;
}

int checkIsItLessWhite2(gameInf* game, column field[FIELD_SIZE]) {
	for (int i = 1; i < game->dice.dice2; i++) {
		if (field[FIELD_SIZE - game->dice.dice2 + i].number > 0) {
			if (game->startPos == game->dice.dice2 - i && game->dice.dice2Available == 1) {
				game->dice.availableMoves--;
				game->dice.dice2Available--;
				game->whiteOnBoard--;
				return 1;
			}
			break;
		}
	}
	return 0;
}

int checkIsItLessWhite1(gameInf* game, column field[FIELD_SIZE]) {
	for (int i = 1; i < game->dice.dice1; i++) {
		if (field[FIELD_SIZE - game->dice.dice1 + i].number > 0) {
			if (game->startPos == game->dice.dice1 - i && game->dice.dice1Available == 1) {
				game->dice.availableMoves--;
				game->dice.dice1Available--;
				game->whiteOnBoard--;
				return 1;
			}
			break;
		}
	}
	return 0;
}

int fromRedHomeDice2(gameInf* game, column field[FIELD_SIZE]) {
	if (game->startPos == (FIELD_SIZE + 1) - game->dice.dice2 && game->dice.dice2Available == 1) {
		game->dice.availableMoves--;
		game->dice.dice2Available--;
		game->redOnBoard--;
		return 1;
	}
	return 0;
}

int checkMoveFromRedHome(gameInf* game, column field[FIELD_SIZE]) {
	if ((field[FIELD_SIZE / 2 - game->dice.dice1].number > 0 && game->dice.dice1Available == 1) || (field[FIELD_SIZE / 2 - game->dice.dice2].number > 0 && game->dice.dice2Available == 1)) {
		if (game->startPos == (FIELD_SIZE + 1) - game->dice.dice1 && game->dice.dice1Available == 1) {
			game->dice.availableMoves--;
			game->dice.dice1Available--;
			game->redOnBoard--;
			return 1;
		}
		else if (fromRedHomeDice2(game, field)) {
			return 1;
		}
	}
	else if (checkIsItLessRed1(game, field) || checkIsItLessRed2(game, field)) {
		return 1;
	}
	return 0;
}

int fromWhiteHomeDice2(gameInf* game, column field[FIELD_SIZE]) {
	if (game->startPos == game->dice.dice2 && game->dice.dice2Available == 1) {
		game->dice.availableMoves--;
		game->dice.dice2Available--;
		game->whiteOnBoard--;
		return 1;
	}
	return 0;
}

int checkMoveFromWhiteHome(gameInf* game, column field[FIELD_SIZE]) {
	if ((field[FIELD_SIZE - game->dice.dice1].number > 0 && game->dice.dice1Available == 1) || (field[FIELD_SIZE - game->dice.dice2].number > 0 && game->dice.dice2Available == 1)) {
		if (game->startPos == game->dice.dice1 && game->dice.dice1Available == 1) {
			game->dice.availableMoves--;
			game->dice.dice1Available--;
			game->whiteOnBoard--;
			return 1;
		}
		else if (fromWhiteHomeDice2(game, field)) {
			return 1;
		}
	}
	else if (checkIsItLessWhite1(game, field) || checkIsItLessWhite2(game, field)) {
		return 1;
	}
	return 0;
}

int checkMoveFromHome(gameInf* game, column field[FIELD_SIZE]) {
	if (game->turn == 0) {
		if (checkMoveFromWhiteHome(game, field)) {
			return 1;
		}
	}
	else {
		if (checkMoveFromRedHome(game, field)) {
			return 1;
		}
	}
	return 0;
}

int checkBarMovesWhiteDice2(gameInf* game, column field[FIELD_SIZE]) {
	if ((game->endPos == game->startPos - game->dice.dice2 && game->whiteOnBar > 0) && game->dice.dice2Available == 1) {
		game->dice.availableMoves--;
		if (game->dice.isItCheck == 0) {
			game->dice.dice2Available--;
			game->whiteOnBar--;
		}
		return 1;
	}
	return 0;
}

int checkBarMovesWhite(gameInf* game, column field[FIELD_SIZE], int indexEnd) {
	if ((field[indexEnd].number < 2) || field[indexEnd].status == 1) {
		if (game->turn == 0 && game->startPos == 25) {
			if ((game->endPos == game->startPos - game->dice.dice1 && game->whiteOnBar > 0) && game->dice.dice1Available == 1) {
				game->dice.availableMoves--;
				if (game->dice.isItCheck == 0) {
					game->dice.dice1Available--;
					game->whiteOnBar--;
				}
				return 1;
			}
			else if (checkBarMovesWhiteDice2(game, field)) {
				return 1;
			}
		}
	}
	return 0;
}

int checkBarMovesRedDice2(gameInf* game, column field[FIELD_SIZE]) {
	if ((game->endPos == game->startPos + game->dice.dice2 && game->redOnBar > 0) && game->dice.dice2Available == 1) {
		game->dice.availableMoves--;
		if (game->dice.isItCheck == 0) {
			game->dice.dice2Available--;
			game->redOnBar--;
		}
		return 1;
	}
	return 0;
}

int checkBarMovesRed(gameInf* game, column field[FIELD_SIZE], int indexEnd) {
	if ((field[indexEnd].number < 2) || field[indexEnd].status == 2) {
		if (game->turn == 1 && game->startPos == 0) {
			if ((game->endPos == game->startPos + game->dice.dice1 && game->redOnBar > 0) && game->dice.dice1Available == 1) {
				game->dice.availableMoves--;
				if (game->dice.isItCheck == 0) {
					game->dice.dice1Available--;
					game->redOnBar--;
				}
				return 1;
			}
			else if (checkBarMovesRedDice2(game, field)) {
				return 1;
			}
		}
	}
	return 0;
}

int isLegalMove6(gameInf* game, column field[FIELD_SIZE], int index1) {
	if (((game->turn + 1) != field[index1].status) && (game->startPos != 0 && game->startPos != 25)) {
		return 1;
	}
	return 0;
}

int isLegalMove5(gameInf* game, column field[FIELD_SIZE], int index1, int index2) {
	if ((field[index1].status != field[index2].status && field[index2].status != 0) && field[index2].number != 1) {
		return 1;
	}
	return 0;
}

int isLegalMove4(gameInf* game, int index1, int index2) {
	if (index1 == -1 || index2 == -1) { // is input good?
		if (game->endPos != 0 && game->endPos != 25) {
			if (game->startPos != 0 && game->startPos != 25) {
				return 1;
			}
		}
	}
	return 0;
}

int isLegalMove3(gameInf* game, column field[FIELD_SIZE], int index2) {
	if (checkBarMovesWhite(game, field, index2)) {
		return 1;
	}
	else if (checkBarMovesRed(game, field, index2)) {
		return 1;
	}
	return 0;
}

int isLegalMove2(gameInf* game, column field[FIELD_SIZE]) {
	checkHome(game, field);
	if (game->isEveryAtHome == 0) {
		return 0;
	}
	else if (checkMoveFromHome(game, field)) {
		return 1;
	}
	return 0;
}

int isLegalMove(gameInf* game, column field[FIELD_SIZE]) { // 0 - illegal, 1 - legal
	int index1 = findIndex(game, field, game->startPos);
	int index2 = findIndex(game, field, game->endPos);
	if (isLegalMove4(game, index1, index2)) { // is input good?
		return 0;
	}

	if (isLegalMove6(game, field, index1)) { //check status, is status the same?
		return 0;
	}
	else if (game->endPos == 0 || game->endPos == 25) {
		if (isLegalMove2(game, field)) {
			return 1;
		}
		return 0;
	}
	else if (game->startPos == 0 || game->startPos == 25) {
		if (isLegalMove3(game, field, index2)) {
			return 1;
		}
		return 0;
	}
	else if (isLegalMove5(game, field, index1, index2)) { //again
		return 0;
	}
	else if (checkMove(game, field, index1)) {
		return 0;
	}
	return 1;
}

int aWMNotDouble(gameInf* game, column field[FIELD_SIZE]) {
	if (game->isEveryAtHome) {
		return 1;
	}
	game->endPos = game->startPos - game->dice.dice1;
	if (isLegalMove(game, field)) {
		return 1;
	}
	game->endPos = game->startPos - game->dice.dice2;
	if (isLegalMove(game, field)) {
		return 1;
	}
	return 0;
}

int aWMFromBar(gameInf* game, column field[FIELD_SIZE]) {
	game->startPos = 25;
	game->endPos = game->startPos - game->dice.dice1;
	if (isLegalMove(game, field)) {
		return 1;
	}
	game->endPos = game->startPos - game->dice.dice2;
	if (isLegalMove(game, field)) {
		return 1;
	}
	return 0;
}

int anyWhiteMoves(gameInf* game, column field[FIELD_SIZE]) {
	if (game->whiteOnBar > 0) {
		if (aWMFromBar(game, field)) {
			return 1;
		}
	}
	else {
		for (int i = 0; i < FIELD_SIZE; i++) {
			if (field[i].status == game->turn + 1 && field[i].number > 0) {
				game->startPos = field[i].n;
				if (game->dice.isItDouble == 0) {
					if (aWMNotDouble(game, field)) {
						return 1;
					}
				}
				else {
					if (game->isEveryAtHome) {
						return 0;
					}
					for (int j = 1; j <= game->dice.availableMoves; j++) {
						game->endPos = game->startPos - j * game->dice.dice2;
						if (isLegalMove(game, field)) {
							return 1;
						}
					}
				}
			}
		}
	}
	return 0;
}

int aRMNotDouble(gameInf* game, column field[FIELD_SIZE]) {
	if (game->isEveryAtHome) {
		return 1;
	}
	game->endPos = game->startPos + game->dice.dice1;
	if (isLegalMove(game, field)) {
		return 1;
	}
	game->endPos = game->startPos + game->dice.dice2;
	if (isLegalMove(game, field)) {
		return 1;
	}
	return 0;
}

int aRMFromBar(gameInf* game, column field[FIELD_SIZE]) {
	game->startPos = 0;
	game->endPos = game->startPos + game->dice.dice1;
	if (isLegalMove(game, field)) {
		return 1;
	}
	game->endPos = game->startPos + game->dice.dice2;
	if (isLegalMove(game, field)) {
		return 1;
	}
	return 0;
}

int anyRedMoves(gameInf* game, column field[FIELD_SIZE]) {
	if (game->redOnBar > 0) {
		if (aRMFromBar(game, field)) {
			return 1;
		}
	}
	else {
		for (int i = 0; i < FIELD_SIZE; i++) {
			if (field[i].status == game->turn + 1 && field[i].number > 0) {
				game->startPos = field[i].n;
				if (game->dice.isItDouble == 0) {
					if (aRMNotDouble(game, field)) {
						return 1;
					}
				}
				else {
					if (game->isEveryAtHome) {
						return 0;
					}
					for (int j = 1; j <= game->dice.availableMoves; j++) {
						game->endPos = game->startPos + j * game->dice.dice2;
						if (isLegalMove(game, field)) {
							return 1;
						}
					}

				}
			}
		}
	}
	return 0;
}

int anyMoves(gameInf* game, column field[FIELD_SIZE]) {
	checkHome(game, field);
	if (game->turn == 0) {
		if (anyWhiteMoves(game, field)) {
			return 1;
		}
	}
	else {
		if (anyRedMoves(game, field)) {
			return 1;
		}
	}
	return 0;
}

void checkCapture(gameInf* game, column(*field)[FIELD_SIZE]) {
	if ((*field)[findIndex(game, *field, game->endPos)].number == 1 && (*field)[findIndex(game, *field, game->endPos)].status != game->turn+1) {
		if ((*field)[findIndex(game, *field, game->endPos)].status == 1) {
			game->whiteOnBar++;
		}
		else if ((*field)[findIndex(game, *field, game->endPos)].status == 2) {
			game->redOnBar++;
		}
		(*field)[findIndex(game, *field, game->endPos)].number--;
	}
}

void moveCheckStartWhite(gameInf* game, column(*field)[FIELD_SIZE]) {
	if ((*field)[findIndex(game, *field, game->endPos)].status == 2 && (*field)[findIndex(game, *field, game->endPos)].number == 1) {
		game->redOnBar++;
		(*field)[findIndex(game, *field, game->endPos)].number--;
	}
	(*field)[findIndex(game, *field, game->endPos)].status = 1;
}

void moveCheckStart(gameInf* game, column(*field)[FIELD_SIZE]) {
	if (game->startPos != 0 && game->startPos != 25) {
		(*field)[findIndex(game, *field, game->startPos)].number--;
	}
	else if (game->startPos == 25) {
		moveCheckStartWhite(game, field);
	}
	else if (game->startPos == 0) {
		if ((*field)[findIndex(game, *field, game->endPos)].status == 1 && (*field)[findIndex(game, *field, game->endPos)].number == 1) {
			game->whiteOnBar++;
			(*field)[findIndex(game, *field, game->endPos)].number--;
		}
		(*field)[findIndex(game, *field, game->endPos)].status = 2;
	}
}

void moveCheckEnd(gameInf* game, column(*field)[FIELD_SIZE]) {
	if (game->endPos != 0 && game->endPos != 25) {
		checkCapture(game, field);
		if (game->startPos != 0 && game->startPos != 25) {
			(*field)[findIndex(game, *field, game->endPos)].status = (*field)[findIndex(game, *field, game->startPos)].status;
		}
		(*field)[findIndex(game, *field, game->endPos)].number++;
	}
}

int move(gameInf *game, column (*field)[FIELD_SIZE]) {
	game->isLegalMoveInt = isLegalMove(game, *field);
	if (game->isLegalMoveInt) {
		moveCheckStart(game, field);
		moveCheckEnd(game, field);

		if ((game->startPos != 0 && game->startPos != 25) && (*field)[findIndex(game, *field, game->startPos)].number == 0) {
			(*field)[findIndex(game, *field, game->startPos)].status = 0;
		}
	}
	else {
		return 0;
	}
	return 1;
}

void input(gameInf* game) {
	gotoxy(1, 23);
	if ((game->turn == 0 && game->whiteOnBar > 0) || (game->turn == 1 && game->redOnBar > 0)) {
		if (game->turn == 0) {
			game->startPos = 25;
		}
		else {
			game->startPos = 0;
		}
		printf("Enter end: ");
		scanf("%d", &game->endPos);
	}
	else {
		printf("Enter start: ");
		scanf("%d", &game->startPos);
		printf("Enter end: ");
		scanf("%d", &game->endPos);
	}
}

void changeToDefault(gameInf* game) {
	game->turn = 1 - (1 * game->turn);
	game->moveIsNotEnded = 0;
	game->dice.availableMoves = 4;
	game->dice.isItDouble = 0;
	game->isEveryAtHome = 0;
}

void dicesToDefault(gameInf* game) {
	game->dice.dice1 = dice();
	game->dice.dice2 = dice();
	game->dice.availableMoves = 2;
	game->dice.isItDouble = 0;
}

void checkIsItNew(gameInf* game, column(*field)[FIELD_SIZE]) {
	if (game->isLegalMoveInt && game->dice.availableMoves == 4) {
		dicesToDefault(game);
		if (game->dice.dice1 == game->dice.dice2) {
			game->dice.availableMoves = 4;
			game->dice.isItDouble = 1;
		}
		game->dice.dice1Available = 1;
		game->dice.dice2Available = 1;
	}
}

void mCommand(gameInf* game, column(*field)[FIELD_SIZE]) {
	game->moveIsNotEnded = 1;

	checkIsItNew(game, field);

	game->dice.movesAtBegin = game->dice.availableMoves;
	game->dice.isItCheck = 1;

	if (anyMoves(game, *field)) {
		game->dice.isItCheck = 0;
		game->dice.availableMoves = game->dice.movesAtBegin;

		drawField(*field, game);
		input(game);

		if (move(game, field)) {
			if (game->redOnBoard == 0 || game->whiteOnBoard == 0) {
				game->isGameEnded = 1;
			}
			else if (game->dice.availableMoves == 0) {
				changeToDefault(game);
			}
		}
	}
	else {
		changeToDefault(game);
	}
}

void loadCommand(gameInf* game, column (*field)[FIELD_SIZE]) {
	FILE* file = fopen("gameSave.txt", "r");
	if (file != NULL) {
		fscanf(file, "turn: %d whiteOnBoard: %d redOnBoard: %d bar: %d %d", &game->turn, &game->whiteOnBoard, &game->redOnBoard, &game->whiteOnBar, &game->redOnBar);
		for (int i = 0; i < FIELD_SIZE; i++) {
			fscanf(file, "%d. %d %d %d", &(*field)[i].n, &(*field)[i].number, &(*field)[i].status, &(*field)[i].direction);
		}
		fclose(file);
	}
}

void saveCommand(gameInf game, column field[FIELD_SIZE]) {
	FILE* file = fopen("gameSave.txt", "w");
	if (file != NULL) {
		fprintf(file, "turn: %d whiteOnBoard: %d redOnBoard: %d bar: %d %d\n", game.turn, game.whiteOnBoard, game.redOnBoard, game.whiteOnBar, game.redOnBar);
		for (int i = 0; i < FIELD_SIZE; i++) {
			fprintf(file, "%d. %d %d %d\n", field[i].n, field[i].number, field[i].status, field[i].direction);
		}
		fclose(file);
	}
}

void setup(gameInf* game, column(*field)[FIELD_SIZE]) {
	settitle("Klim, Kaliasniou, 201250");
	_setcursortype(_NOCURSOR);

	createField(field);
	drawCmds(game->isGameStarted);
}

int main() {
	srand(time(NULL));
	gameInf game = {'0', 0, 15, 15, 0, 0, rand() % 2, 0, 1, 0, 0, 0, 0};
	column field[FIELD_SIZE];
	game.dice.availableMoves = 4;

	setup(&game, &field);

	do {
		if (game.moveIsNotEnded == 0) {
			game.zn = getch();
		}

		switch (game.zn) {
		case 'm':
			system("cls");
			mCommand(&game, &field);
			break;
		case 's':
			saveCommand(game, field);
			break;
		case 'l':
			loadCommand(&game, &field);
			break;
		case 'q':
			return 0;
		}
		system("cls");
		drawField(field, &game);
	} while (game.isGameEnded != 1);
	system("cls");
	if (game.turn == 0) {
		printf("White win");
	}
	else {
		printf("Red win");
	}

	return 0;
}