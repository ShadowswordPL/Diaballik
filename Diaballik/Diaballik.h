#ifndef _Diaballik_Diaballik_h
#define _Diaballik_Diaballik_h

#include <Painter/Painter.h>
#include <CtrlLib/CtrlLib.h>

#include "LangConsts.h"
#include "ColorConsts.h"
#include "NumberConsts.h"
#include "TypesEnumsStructs.h"

#define IMAGECLASS MyImages
#define IMAGEFILE  <Diaballik/Pawn.iml>
#include <Draw/iml_header.h>

/*
	Diaballik is fun to play game similar to chess.
	Author of program: Andrzej Jackowski
	E-mail: andrzejjac@gmail.com
										~Have Fun
*/


using namespace Upp;

//Class of one field from GameBoard
class GameBoardField 
{

	GameBoardPos position;
	
	public:
		GameBoardFieldType type;
		bool clicked;
		bool possible;
		GameBoardField();
		GameBoardField(GameBoardPos p);
		GameBoardField(GameBoardFieldType t, GameBoardPos p);
		
		
		void DrawField(Size s, PaintingPainter& p);
		static double FieldWidth(Size s);
		static double FieldHeight(Size s);
		
		GameBoardPos GetPositon();
		Image PlayerImage(GameBoardFieldType m, bool clicked);	

};

//Class of move saved in historys
class Move : public Moveable<Move>
{
	GameBoardPos bcords;
	GameBoardPos ecords;
	PlayerColor pcolor;
	public:
		MoveType movetype;
		int throws;
		int moves;
		bool end;
		GameBoardField GameBoardArray[BOARDSIZE][BOARDSIZE];
		
		Move(GameBoardField gba[][BOARDSIZE], MoveType m, GameBoardPos b, GameBoardPos e, 
			PlayerColor c, int t, int mint);	
		
		String moveString();
		GameBoardPos GetBcords();
		GameBoardPos GetEcords();
		PlayerColor GetColor();
		void SetColor(PlayerColor pc);

};

//Class contains full game history
class GameHistory
{
	public:
		Vector<Move> vhistory;

		void AddMove(GameBoardField gba[][BOARDSIZE], MoveType m, GameBoardPos b, GameBoardPos e, int t, int mint);
};

//Class contains info about each player
class Player{
	PlayerColor color;
	public:
		PlayerType type;
		Player();
		Player(PlayerType t, PlayerColor c);
		PlayerColor GetColor();
};

//Class supports playing of Diaballik
class MainGame
{	
	public:
		Mode mode;
		bool unsaved;
		Player player1;
		Player player2;
		
		GameHistory gamehistory;
		int currentmove;
		int currentmaxmove;		
		bool arbiter;		
		bool aithinking;
		
		MainGame();
		void Clear();
		void ChangeMode(Mode m);
		void ChangePlayer(bool end);
		bool NoMoves();
		bool PlayerPawn(GameBoardPos p, PlayerColor c);
		bool HaveTouchedLine(PlayerColor c);
		bool SameState();
		JudgeMsg GameResult();
		bool WillMove();
		GameBoardFieldType FieldType(GameBoardPos x);
		void MakeMove(GameBoardPos b, GameBoardPos e, MoveType m);
		void SetPlayers();
		void SetStartBoard();
		void StartNone();
		void StartNewGame();
		void StartEditor();
		String QTFMove(int i);
		String QTFHistory();
		
		Player CurrentPlayer();
		JudgeMsg Judge();
};

//Class contains gameboard using by AI
class AIGameField : public Moveable<AIGameField>
{
	public: 
		GameBoardField gba[BOARDSIZE][BOARDSIZE];
		int moves;
		int throves;
		PlayerColor pc;	
};

class Diaballik;

//Upgraded Picture class with some new functions
class GameBoardLay : public Picture
{
	public:
		MainGame* maingame;
		Diaballik* diaballik;
		
		void DrawLay(GameBoardField GameBoardArray[][BOARDSIZE]);
		void DrawMargin(Painter &p);
		void operator=(const Painting& p);	
		static String IntToBigLetter(int x);
		static String IntToNumber(int x);
		void Select(GameBoardPos s);
		void Unselect();
		GameBoardPos Selected();
		GameBoardField FieldFromArray(int i, int j);
		bool AnyPawn(GameBoardPos p);
		bool AnyBall(GameBoardPos p);
		bool PlayerPawn(GameBoardPos p, PlayerColor c);
		bool NoEnemyInLine(GameBoardPos b, GameBoardPos e);
		bool PossibleThrow(GameBoardPos b, GameBoardPos e);
		bool CanDoubleMove(GameBoardPos b, GameBoardPos e);
		bool PossibleMove(GameBoardPos b, GameBoardPos e);
		bool PossibleSelect(GameBoardPos b);
		static int BoardDistance(GameBoardPos b, GameBoardPos e);

		void LeftUp(Point p, dword keyflags);
		GameBoardPos ReturnSelectedPos(Point p);
		int SetIterator(int x1, int x2);
};

//Class plays Diaballik like real human!
class AIPlayer
{
	PlayerColor color;
	MainGame* maingame; 
	GameBoardLay* gameboardlay;
	Diaballik* diaballik; 
	
	
	int currentmove;
	public:
		bool isthinking;
		
		AIPlayer();
		AIPlayer(PlayerColor color, MainGame* maingame, 
				GameBoardLay* gameboardlay, Diaballik* diaballik);
		
		String Cords(GameBoardPos b, GameBoardPos e);
		bool Play(bool hint);
		int ValueMove(GameBoardFieldType t, int i, int j);
		int ValueBall(GameBoardFieldType t, int i, int j);
		int GameValue();
		int MinMax(int a, int b, bool max);
		int MoveTo(int x1, int y1, int x2, int y2, int depth, int alpha, int beta, bool max);
		int ThrowTo(int x1, int y1, int x2, int y2, int depth, int alpha, int beta, bool max);
		void RemoveMoves(int depth);
			
		void CheckValue(int value, int &oldvalue, GameBoardPos b, GameBoardPos e, 
						MoveType type, AIMove &mv, bool max);
						
		int AllPossible(int depth, int alpha, int beta, bool max, AIMove &mv);
		int AlphaBeta(int depth, int alfa, int beta);
		AIMove MakeMove();
		AIMove AIThink();
};

#define LAYOUTFILE <Diaballik/Diaballik.lay>
#include <CtrlCore/lay.h>

//Main class of game
class Diaballik : public WithDiaballikLayout<TopWindow> {
public:
	MenuBar menu;	
	MainGame maingame;	
	FileSel fs;
	AIPlayer robo1;
	AIPlayer robo2;
	typedef Diaballik CLASSNAME;
	Diaballik();	
	
	void NewGame();
	void SaveGame();
	void LoadGame();
	void Exit();
	void Editor();
	void GameRules();
	void Undo();
	void Hint();
	void SubMenu(Bar& bar);
	void MainMenu(Bar& bar);
	bool UnsavedMsg();
	void RichTextUrlClick(const String& url);
	void DrawInterface();
	void DrawRichText();
	void DrawGameBoard();
	int StringToInt(WString url);
	
	void EndMove();
	bool CanUndo();
	void LeaveEditor();
	void ChangePlayerEditor();

	void JudgeGame();	
	void PlayAI();
};

class SelectPlayerLayout : public WithSelectPlayerLayout<TopWindow> {
	typedef SelectPlayerLayout CLASSNAME;

public:
	bool Accept();

	SelectPlayerLayout();
};

#endif
