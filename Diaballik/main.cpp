#include "Diaballik.h"

//Start game
Diaballik::Diaballik()
{
	AddFrame(menu);
	menu.Set(THISBACK(MainMenu));
	fs.Type(FSTYPENAME, FSTYPEEXT);
	CtrlLayout(*this, GAMENAME);
	SeedRandom(GetSysTime().Get());
	gameboardlay.maingame = &maingame;
	gameboardlay.diaballik = this;
	SetTimeCallback(-REFRESH, THISBACK(JudgeGame));
	maingame.ChangeMode(none);
	robo1 = AIPlayer(blue, &maingame, &gameboardlay, this);
	robo2 = AIPlayer(red, &maingame, &gameboardlay, this);
	Thread::Start(THISBACK(PlayAI));	
	
	DrawInterface();
}

//Start new game
void Diaballik::NewGame()
{
	if(robo1.isthinking || robo2.isthinking)
	{
		SetTimeCallback(REFRESH, THISBACK(NewGame));
		return;	
	}
	if(UnsavedMsg())
	{
		maingame.ChangeMode(game);
		DrawInterface();
	}
}

//Saving game
void Diaballik::SaveGame() 
{
	if(robo1.isthinking || robo2.isthinking)
	{	
		PromptOK(CANNOTAI);
		return;
	}
	if(maingame.mode == editor)
	{
		PromptOK(CANNOTINEDIT);	
	}
	else if(maingame.mode != none)
	{
		if(!fs.ExecuteSaveAs())
		{
			return;	
		}
		String filename = fs.Get();
    	FileOut out(filename);
    	if(!out)
    	{
        	PromptOK(FILECANNOTOPEN);
        	return;
    	}
    	out.Put(maingame.currentmaxmove);
    	out.Put((int)maingame.player1.type);
    	out.Put((int)maingame.player2.type);

    	for(int x = 0; x <= maingame.currentmaxmove; x++)
    	{
    		for(int i = 0; i < BOARDSIZE; i++)
    		{
    			for(int j = 0; j < BOARDSIZE; j++)
    			{
    				out.Put((int)maingame.gamehistory.vhistory[x].GameBoardArray[i][j].type);
    			}
    		}
    		
    		out.Put((int)maingame.gamehistory.vhistory[x].movetype);
    		out.Put(maingame.gamehistory.vhistory[x].GetBcords().x);
    		out.Put(maingame.gamehistory.vhistory[x].GetBcords().y);
    		out.Put(maingame.gamehistory.vhistory[x].GetEcords().x);
    		out.Put(maingame.gamehistory.vhistory[x].GetEcords().y);
    		if(!maingame.gamehistory.vhistory[x].end)
    		{
	    		out.Put((int) maingame.gamehistory.vhistory[x].GetColor());
    		}
    		else
    		{
    			if(maingame.gamehistory.vhistory[x].GetColor() == blue)
    			{
    				out.Put((int) red);	
    			}
    			else
    			{
    				out.Put((int) blue);	
    			}
    		}
			out.Put(maingame.gamehistory.vhistory[x].throws);
    		out.Put(maingame.gamehistory.vhistory[x].moves);
    		out.Put(maingame.gamehistory.vhistory[x].end);
    	}
    	maingame.unsaved = false;
    	
	}
	else
	{
		PromptOK(STARTTOSAVE);	
	}
}
	
//Loading game
void Diaballik::LoadGame() 
{
	if(robo1.isthinking || robo2.isthinking)
	{
		SetTimeCallback(REFRESH, THISBACK(LoadGame));
		return;	
	}
	if(UnsavedMsg())
	{
		if(!fs.ExecuteOpen())
		{
			return;	
		}
		String filename = fs.Get();
		FileIn in(filename);
    	if(!in) 
    	{
        	PromptOK(FILECANNOTOPEN);
        	return;
    	}

	    int moves = in.Get();
		if(3 + (BOARDSIZE*BOARDSIZE + 9)*(moves + 1) == in.GetSize())
		{
			maingame.mode = game;
			maingame.SetStartBoard();
	    	maingame.gamehistory.vhistory.Clear();
	    	maingame.arbiter = false;
			maingame.currentmaxmove = 0;
			maingame.currentmove = 0;
			
			
			maingame.player1 = Player((PlayerType)in.Get(), blue);
			maingame.player2 = Player((PlayerType)in.Get(), red);		
		
			for(int x = 0; x <= moves; x++)
			{
				GameBoardField gba[BOARDSIZE][BOARDSIZE];
				for(int i = 0; i < BOARDSIZE; i++)
				{
					for(int j = 0; j < BOARDSIZE; j++)
					{
						gba[i][j] = GameBoardField((GameBoardFieldType)in.Get(), 
													GameBoardPos(i, j));
					}	
				}
				MoveType type = (MoveType) in.Get();
				int bposx = in.Get();
				int bposy = in.Get();
				int eposx = in.Get();
				int eposy = in.Get();
				PlayerColor pc = (PlayerColor) in.Get();
				int throws = in.Get();
				int moves = in.Get();
				maingame.gamehistory.vhistory.Add
				(
					Move(
						gba, 
						type, 
						GameBoardPos(bposx, bposy), 
						GameBoardPos(eposx, eposy), 
						pc, 
						throws, 
						moves
						)
				);
				maingame.gamehistory.vhistory[maingame.currentmove].end = in.Get();	
				
				maingame.currentmove++;
				maingame.currentmaxmove++;
			}
			maingame.currentmove--;
			maingame.currentmaxmove--;
			maingame.arbiter = true;
			
		}
		else
		{
			PromptOK(DMGFILE);	
		}
		DrawInterface();
	}
}

//Function exit game	
void Diaballik::Exit() 
{
	if(UnsavedMsg())
	{
		Break();
	}
}

//Function launch editor	
void Diaballik::Editor() 
{
	if(maingame.mode == none)
	{
		PromptOK(STARTTOEDIT);	
	}
	if(robo1.isthinking || robo2.isthinking)
	{	
		PromptOK(CANNOTAI);
		return;
	}
	if(maingame.currentmaxmove == maingame.currentmove)
	{
		if(maingame.mode == game)
		{
			if(PromptOKCancel(OPENEDITOR))
			{
				maingame.ChangeMode(editor);
				maingame.gamehistory.vhistory[maingame.currentmove].movetype = edit;
				DrawInterface();
			}
		}
	}
	else
	{
		PromptOK(NOTCURRENTMAX);	
	}
}

//Function launch game rules
void Diaballik::GameRules()
{
	LaunchWebBrowser(RULESURL);
}

//Function make undo move	
void Diaballik::Undo()
{
	RichTextUrlClick(Format("%d", maingame.currentmove-1));
	DrawInterface();
}	

//Function show hint
void Diaballik::Hint()
{
	if(maingame.gamehistory.vhistory[maingame.currentmove].GetColor() == blue)
	{
		robo1.Play(true);	
	}
	else
	{
		robo2.Play(true);
	}
}

//Function set submenu
void Diaballik::SubMenu(Bar& bar) 
{
	bar.Add(L_MENU_NEWGAME, THISBACK(NewGame));	
	bar.Add(L_MENU_SAVEGAME, CtrlImg::save(), THISBACK(SaveGame));
	bar.Add(L_MENU_LOADGAME, CtrlImg::open(), THISBACK(LoadGame));
	bar.Add(L_MENU_EXIT, CtrlImg::remove(), THISBACK(Exit));
}
	
//Function set main menu	
void Diaballik::MainMenu(Bar& bar) 
{
	bar.Add(L_MENU_FILE, THISBACK(SubMenu));
	bar.Add(L_MENU_EDITOR, THISBACK(Editor));
	bar.Add(L_MENU_RULES, THISBACK(GameRules));
}
	
//Function show message when game is unsaved, return true when ok pressed
bool Diaballik::UnsavedMsg()
{
	if(maingame.unsaved)
	{
		return PromptOKCancel(L_UNSAVED);
	}
	return true;
}

//Function draw game interface
void Diaballik::DrawInterface()
{
	if(maingame.mode == none)
	{
		hintButton.Disable();
		hintButton.Show();
		undoButton.Disable();
		undoButton.Show();
		endMoveButton.Disable();
		endMoveButton.Show();
		playItButton.Hide();
		playerChange.Hide();
		playerHave.SetInk(Black());
		movesInt.Clear();
		throwsInt.Clear();
		movesInt.SetEditable(false);
		throwsInt.SetEditable(false);		
	}
	if(maingame.mode == game)
	{
		hintButton.Enable();
		hintButton.Show();
		hintButton.WhenPush = THISBACK(Hint);
		
		undoButton.Show();
		endMoveButton.Show();
		
		if( robo1.isthinking || robo2.isthinking || 
			maingame.currentmove != maingame.currentmaxmove)
		{
			hintButton.Disable();	
		}
		
		if(maingame.CurrentPlayer().type == human)
		{
			endMoveButton.WhenPush = THISBACK(EndMove);
			endMoveButton.Enable();
		}
		else
		{
			hintButton.Disable();
			endMoveButton.Disable();
			undoButton.Disable();
		}
	
		if(CanUndo() && maingame.CurrentPlayer().type == human)
		{
			undoButton.Enable();
			endMoveButton.Enable();
			undoButton.WhenPush = THISBACK(Undo);
		}
		else
		{
			undoButton.Disable();	
			endMoveButton.Disable();
		}
		
		playItButton.Hide();
		playerChange.Hide();
	
		movesInt.SetData(maingame.gamehistory.vhistory[maingame.currentmove].moves);
		throwsInt.SetData(maingame.gamehistory.vhistory[maingame.currentmove].throws);
		movesInt.SetEditable(false);
		throwsInt.SetEditable(false);
	}
	if(maingame.mode == editor)
	{
		hintButton.Hide();
		undoButton.Hide();
		endMoveButton.Hide();
		playItButton.Show();
		playerChange.Show();
		movesInt.SetEditable(true);
		throwsInt.SetEditable(true);
		playItButton.WhenPush = THISBACK(LeaveEditor);
		playerChange.WhenPush = THISBACK(ChangePlayerEditor);	
	}
	
	if(maingame.mode != none)
	{
		if(maingame.gamehistory.vhistory[maingame.currentmove].GetColor() == blue)
		{
			playerHave.SetInk(Blue());
		}
		else
		{
			playerHave.SetInk(Red());
		}
	}
	
	DrawGameBoard();
	DrawRichText();
}

//Function launch when game history is clicked
void Diaballik::RichTextUrlClick(const String& url)
{
	if(robo1.isthinking || robo2.isthinking)
	{	
		PromptOK(CANNOTAI);
		return;
	}
	if(maingame.mode == editor)
	{
		PromptOK(CANNOTINEDIT);	
	}
	if(maingame.mode == game)
	{
		WString wurl = WString(url.Begin(), url.End());
		maingame.currentmove = StringToInt(wurl);
		gameboardlay.Unselect();
	}
	DrawInterface();
}
	
//Function fill game history with full game history
void Diaballik::DrawRichText()
{
	gamehistory.WhenLink = THISBACK(RichTextUrlClick);
	gamehistory.NoHyperlinkDecoration();
	gamehistory.SetQTF(maingame.QTFHistory());
	if(maingame.mode == none)
	{
		gamehistory.SetQTF("");		
	}
}		
	
void Diaballik::DrawGameBoard()
{
	gameboardlay.DrawLay(maingame.gamehistory.vhistory[maingame.currentmove].GameBoardArray);
}

//Function turn wstring into int number
int Diaballik::StringToInt(WString url)
{
	int result = 0;
	for(int i = 0; i < url.GetLength(); i++)
	{
		result = result + pow(10, url.GetLength()-i-1)*(url[i]-'0');
	}
	return result;
}

//Function end move when button pressed
void Diaballik::EndMove()
{
	maingame.MakeMove(GameBoardPos(0, 0), GameBoardPos(0, 0), end);
	gameboardlay.Unselect();
	DrawInterface();
}


//Function check if player can make undo
bool Diaballik::CanUndo()
{
	if(maingame.currentmove > 0)
	{
		return !maingame.gamehistory.vhistory[maingame.currentmove].end;
	}
}

//Function launch when player press exit editor button
void Diaballik::LeaveEditor()
{
	maingame.mode = game;
	if((int)movesInt.GetData() <= 0)
	{
		movesInt.SetData(0);
	}
	if((int)throwsInt.GetData() <= 0)
	{
		throwsInt.SetData(0);
	}
	maingame.gamehistory.vhistory[maingame.currentmove].moves = movesInt.GetData();
	maingame.gamehistory.vhistory[maingame.currentmove].throws = throwsInt.GetData();
	DrawInterface();
}

//Function launch when player press change player button in game editor
void Diaballik::ChangePlayerEditor()
{
	if(maingame.gamehistory.vhistory[maingame.currentmaxmove].GetColor() == blue)
	{
		maingame.gamehistory.vhistory[maingame.currentmaxmove].SetColor(red);
	}
	else
	{
		maingame.gamehistory.vhistory[maingame.currentmaxmove].SetColor(blue);
	}
	DrawInterface();
}

//Function judge current game
void Diaballik::JudgeGame()
{
	JudgeMsg msg = maingame.Judge();
	if(msg != nonemsg && !maingame.aithinking)
	{
		if(msg != changeplayer)
		{
			DrawInterface();
			maingame.arbiter = false;
			String winmsg = " ";
			winmsg += winmsg + WON;
			if(msg == draw)
			{
				PromptOK(DRAW);		
			}
			if(msg == bluewin)
			{
				winmsg = BLUE + winmsg;
				PromptOK(winmsg);	
			}
			if(msg == redwin)
			{
				winmsg = RED + winmsg;
				PromptOK(winmsg);	
			}
			maingame.ChangeMode(none);
		}
		DrawInterface();
	}
}

//Function play game using AI
void Diaballik::PlayAI()
{
	Sync();
	ProcessEvents();
	if(maingame.arbiter)
	{
		JudgeGame();
		if(maingame.CurrentPlayer().type == cpu)
		{
			robo1.Play(false);
			robo2.Play(false);
			DrawInterface();
		}
	}
	SetTimeCallback(REFRESH*AITIME, THISBACK(PlayAI));	
}

	
GUI_APP_MAIN
{
	Diaballik().Run();
}
