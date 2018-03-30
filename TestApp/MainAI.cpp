#include <ISEngine.h>
#include <ISForms.h>
#include <ISGameAI.h>
#include <ISGuiFactory.h>

#include <vld.h>


#ifdef _DEBUG
	#pragma comment (lib, "ISEngine_d.lib")
	#pragma comment (lib, "ISGameAI_d.lib")
#else
	#pragma comment (lib, "ISEngine.lib")
	#pragma comment (lib, "ISGameAI.lib")
#endif


Result __stdcall Init();
Result __stdcall PostInit();
void __stdcall TimerElapsed(LPVOID user);
void __stdcall End();

HINSTANCE __hInst;
int __nCmdShow;


LPWINFORM form1 = NULL;
LPGUIOUTPUTWINDOW wnd;


















#define WALLBIT_NORTH	0x8
#define WALLBIT_EAST	0x4
#define WALLBIT_SOUTH	0x2
#define WALLBIT_WEST	0x1
enum Direction
	{DIR_NORTH, DIR_EAST, DIR_SOUTH, DIR_WEST, DIR_STOP};

LPGUIBITMAP bmpwalls[0x10] = {0}, bmppacman[4], bmpfood;
Size<> layoutsize;

using namespace AI;

#include <set>
struct PacmanState : public State
{
	Point<> pacmanpos;
	std::set<Point<>> foodpos;
};
bool PacmanStatePtrComparator(StatePtr ptr0, StatePtr ptr1)
{
	PacmanState *_ptr0 = (PacmanState*)ptr0.get(), *_ptr1 = (PacmanState*)ptr1.get();
	//return _ptr0->pacmanpos < _ptr1->pacmanpos || (_ptr0->pacmanpos == _ptr1->pacmanpos && _ptr0->foodpos.size() < _ptr1->foodpos.size()); //EDIT: Just comparing size is not enough!
	return _ptr0->pacmanpos < _ptr1->pacmanpos ||
		(_ptr0->pacmanpos == _ptr1->pacmanpos && std::lexicographical_compare(_ptr0->foodpos.begin(), _ptr0->foodpos.end(), _ptr1->foodpos.begin(), _ptr1->foodpos.end()));
}

struct PacmanFoodSearch : public SearchProblem
{
	bool** walls;
	std::shared_ptr<PacmanState> startstate;

	PacmanFoodSearch() : walls(NULL), startstate(new PacmanState()) {}
	StatePtr getStartState()
	{
		return startstate;
	}
	bool isGoalState(StatePtr state) const
	{
		return ((PacmanState*)state.get())->foodpos.empty();
	}
	bool isActionLegal(StatePtr state, Action action) const
	{
		PacmanState* _state = (PacmanState*)state.get();
		switch((Direction)action)
		{
		case DIR_NORTH: return !walls[_state->pacmanpos.y + 0][_state->pacmanpos.x + 1];
		case DIR_EAST:  return !walls[_state->pacmanpos.y + 1][_state->pacmanpos.x + 2];
		case DIR_SOUTH: return !walls[_state->pacmanpos.y + 2][_state->pacmanpos.x + 1];
		case DIR_WEST:  return !walls[_state->pacmanpos.y + 1][_state->pacmanpos.x + 0];
		case DIR_STOP:  return true;
		}
		return false;
	}
	StatePtr generateSuccessor(StatePtr state, Action action, double* cost = 0) const
	{
		PacmanState* _state = (PacmanState*)state.get();
		std::shared_ptr<PacmanState> successor = std::make_shared<PacmanState>();
		successor->foodpos.insert(_state->foodpos.begin(), _state->foodpos.end());

		switch((Direction)action)
		{
		case DIR_NORTH: successor->pacmanpos = Point<>(_state->pacmanpos.x, _state->pacmanpos.y - 1); break;
		case DIR_EAST:  successor->pacmanpos = Point<>(_state->pacmanpos.x + 1, _state->pacmanpos.y); break;
		case DIR_SOUTH: successor->pacmanpos = Point<>(_state->pacmanpos.x, _state->pacmanpos.y + 1); break;
		case DIR_WEST:  successor->pacmanpos = Point<>(_state->pacmanpos.x - 1, _state->pacmanpos.y); break;
		case DIR_STOP:  successor->pacmanpos = Point<>(_state->pacmanpos.x, _state->pacmanpos.y); break;
		}

		std::set<Point<>>::const_iterator iter = successor->foodpos.find(successor->pacmanpos);
		if(iter != successor->foodpos.end())
			successor->foodpos.erase(iter);

		if(cost)
			*cost = 1.0;

		return successor;
	}
	StatePtr nextSuccessor(StatePtr state, Action& lastaction, double* cost = 0) const
	{
		while(++lastaction != DIR_STOP)
			if(isActionLegal(state, lastaction))
				return generateSuccessor(state, lastaction, cost);
		lastaction = (Action)-1;
		return NULL;
	}
	void Draw(const PacmanState* state) const;
};

void PacmanFoodSearch::Draw(const PacmanState* state) const
{
	// Clear screen
	wnd->ClearPrimitives();

	// Draw walls
	int x, y;
	char wallbits;
	for(y = 1; y <= layoutsize.height; y++)
		for(x = 1; x <= layoutsize.width; x++)
			if(walls[y][x])
			{
				wallbits = 0x0;
				if(walls[y - 1][x])
					wallbits |= WALLBIT_NORTH;
				if(walls[y][x + 1])
					wallbits |= WALLBIT_EAST;
				if(walls[y + 1][x])
					wallbits |= WALLBIT_SOUTH;
				if(walls[y][x - 1])
					wallbits |= WALLBIT_WEST;
				if(wallbits == 0)
					Result::PrintLogMessage("Error inside wall layout: Wall without neighbour walls found");
				if(bmpwalls[wallbits] == 0)
					Result::PrintLogMessage(String("Error inside wall layout: Illegal wall bit combination: ") << String((int)wallbits));
				wnd->DrawBitmap(bmpwalls[wallbits], (float)(16 * (x - 1)), (float)(16 * (y - 1)), 16.0f, 16.0f);
			}

	// Draw food
	std::set<Point<>>::const_iterator fooditer;
	LIST_FOREACH(state->foodpos, fooditer)
		wnd->DrawBitmap(bmpfood, (float)(16 * fooditer->x), (float)(16 * fooditer->y), 16.0f, 16.0f);

	// Draw pacman
	wnd->DrawBitmap(bmppacman[DIR_EAST], (float)(16 * state->pacmanpos.x), (float)(16 * state->pacmanpos.y), 16.0f, 16.0f);

	// Refresh screen
	wnd->Invalidate();
}

double __stdcall manhattanHeuristic(StatePtr state, SearchProblem& problem)
{
	PacmanState* _state = (PacmanState*)state.get();
	if(_state->foodpos.empty())
		return 0.0;
	Point<>& xy1 = _state->pacmanpos;
	Point<> xy2 = *_state->foodpos.begin();
	return (double)(abs(xy1.x - xy2.x) + abs(xy1.y - xy2.y));
}


/*template<class Action, class State> struct SearchAgent
{
	virtual Action getAction(const State* state) = 0;
};

struct GoNorthAgent : public SearchAgent<Action, PacmanState>
{
	SearchProblem* problem;

	Action getAction(StatePtr state)
	{
		return problem->isActionLegal(state, DIR_NORTH) ? DIR_NORTH : DIR_STOP;
	}
};*/


PacmanFoodSearch problem;
Path solution = NULL;
std::vector<Action>::const_iterator solution_iter;
StatePtr currentstate;








#include <fstream>
Result ReadLayout(FilePath filename)
{
	std::ifstream fs(filename);
	int x, y;
	char c;

	// Read layout size
	layoutsize = Size<>(-1, 0);
	x = 0;
	while(fs.peek() != -1)
	{
		fs.read(&c, 1);
		if(c == '\n')
		{
			if(layoutsize.width != -1 && x != layoutsize.width)
			{
				fs.close();
				return ERR("Illegal layout: lines with different length found");
			}
			layoutsize.width = x;
			x = 0;
			layoutsize.height++;
		}
		else
			x++;
	}
	if(x != 0) // If the file hasn't ended with a newline
		layoutsize.height++; // Add one last line
	cle->PrintLine(String("layout size = (") << String(layoutsize.width) << String(",  ") << String(layoutsize.height) << String(")"));

	problem.walls = (bool**)new LPVOID[layoutsize.height + 2];
	for(y = 0; y < layoutsize.height + 2; y++)
	{
		problem.walls[y] = new bool[layoutsize.width + 2];
		memset(problem.walls[y], false, (layoutsize.width + 2) * sizeof(bool));
	}

	fs.clear();
	fs.seekg(0, fs.beg);
	x = y = 0;
	problem.startstate->pacmanpos.x = -1;
	while(fs.peek() != -1)
	{
		fs.read(&c, 1);
		switch(c)
		{
		case '\n':
			x = 0;
			y++;
			continue;

		case '%':
			problem.walls[y + 1][x + 1] = true;
			break;

		case 'P':
			problem.startstate->pacmanpos = Point<>(x, y);
			break;

		case '.':
			problem.startstate->foodpos.insert(Point<>(x, y));
			break;
		}
		x++;
	}
	fs.close();

	if(problem.startstate->pacmanpos.x == -1)
		return ERR("Error inside layout: No pacman start position ('P') defined");

	return R_OK;
}


int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR pCmdLine, int nCmdShow)
{
	Result rlt;

__hInst = hInst;
__nCmdShow = nCmdShow;

	// Create Engine, Forms and Direct3D
	if(!(eng = new Engine(hInst)) || !(fms = CreateForms()))
		MessageBox(NULL, "Out of memory", "Iron Sight Engine", 0);

	// Register the engines window class
	IFFAILED(eng->RegisterWinClass(NULL, (HBRUSH)COLOR_WINDOW)) //(HBRUSH)COLOR_WINDOW
		MessageBox(NULL, rlt.GetLastResult(), "Iron Sight Engine", 0);

	CoInitialize(NULL);

	// Enter main loop
	IFFAILED(eng->Run(Init, PostInit, NULL, End))
	{
		OutputDebugString(rlt.GetLastResult());
		OutputDebugString("\n");
	}

	CoUninitialize();

	delete eng;

	return 1;
}

Result __stdcall Init()
{
	Result rlt;

	// Read pacman maze layout
	CHKRESULT(ReadLayout("PacMan\\layouts\\bigMaze.lay"));

	// Init Forms
	fms->Sync(GLOBALVAR_LIST);
	CHKRESULT(fms->Init(__hInst));

	// Create WinForm
	FormSettings frmsettings;
	frmsettings.fbstyle = FBS_DEFAULT;
	frmsettings.clientsize = Size<int>(layoutsize.width * 16, layoutsize.height * 16);
	frmsettings.caption = "Iron Sight Engine - Window 1";
	frmsettings.windowpos = Point<int>(0, 0);
	CHKRESULT(fms->CreateForm(&frmsettings, NULL, &form1));

	// Init GuiFactory
	CHKALLOC(gui = CreateGuiFactory()); gui->Sync(GLOBALVAR_LIST);
	CHKRESULT(gui->Init());

	// Create output window
	CHKRESULT(gui->CreateOutputWindow(fms->GetHwnd(0), &wnd));
	CHKRESULT(wnd->CreateSolidBrush(0.0f, 0.0f, 0.0f, 1.0f, &wnd->bgbrush));

	// Load pacman bitmaps
	CHKRESULT(wnd->CreateImageFromFile("PacMan\\wall_e.png", &bmpwalls[WALLBIT_EAST]));
	CHKRESULT(wnd->CreateImageFromFile("PacMan\\wall_n.png", &bmpwalls[WALLBIT_NORTH]));
	CHKRESULT(wnd->CreateImageFromFile("PacMan\\wall_ne.png", &bmpwalls[WALLBIT_NORTH | WALLBIT_EAST]));
	CHKRESULT(wnd->CreateImageFromFile("PacMan\\wall_ns.png", &bmpwalls[WALLBIT_NORTH | WALLBIT_SOUTH]));
	CHKRESULT(wnd->CreateImageFromFile("PacMan\\wall_nse.png", &bmpwalls[WALLBIT_NORTH | WALLBIT_SOUTH | WALLBIT_EAST]));
	CHKRESULT(wnd->CreateImageFromFile("PacMan\\wall_nsw.png", &bmpwalls[WALLBIT_NORTH | WALLBIT_SOUTH | WALLBIT_WEST]));
	CHKRESULT(wnd->CreateImageFromFile("PacMan\\wall_nswe.png", &bmpwalls[WALLBIT_NORTH | WALLBIT_SOUTH | WALLBIT_WEST | WALLBIT_EAST]));
	CHKRESULT(wnd->CreateImageFromFile("PacMan\\wall_nw.png", &bmpwalls[WALLBIT_NORTH | WALLBIT_WEST]));
	CHKRESULT(wnd->CreateImageFromFile("PacMan\\wall_nwe.png", &bmpwalls[WALLBIT_NORTH | WALLBIT_WEST | WALLBIT_EAST]));
	CHKRESULT(wnd->CreateImageFromFile("PacMan\\wall_s.png", &bmpwalls[WALLBIT_SOUTH]));
	CHKRESULT(wnd->CreateImageFromFile("PacMan\\wall_se.png", &bmpwalls[WALLBIT_SOUTH | WALLBIT_EAST]));
	CHKRESULT(wnd->CreateImageFromFile("PacMan\\wall_sw.png", &bmpwalls[WALLBIT_SOUTH | WALLBIT_WEST]));
	CHKRESULT(wnd->CreateImageFromFile("PacMan\\wall_swe.png", &bmpwalls[WALLBIT_SOUTH | WALLBIT_WEST | WALLBIT_EAST]));
	CHKRESULT(wnd->CreateImageFromFile("PacMan\\wall_w.png", &bmpwalls[WALLBIT_WEST]));
	CHKRESULT(wnd->CreateImageFromFile("PacMan\\wall_we.png", &bmpwalls[WALLBIT_WEST | WALLBIT_EAST]));

	CHKRESULT(wnd->CreateImageFromFile("PacMan\\pacman_n.png", &bmppacman[DIR_NORTH]));
	CHKRESULT(wnd->CreateImageFromFile("PacMan\\pacman_e.png", &bmppacman[DIR_EAST]));
	CHKRESULT(wnd->CreateImageFromFile("PacMan\\pacman_s.png", &bmppacman[DIR_SOUTH]));
	CHKRESULT(wnd->CreateImageFromFile("PacMan\\pacman_w.png", &bmppacman[DIR_WEST]));

	CHKRESULT(wnd->CreateImageFromFile("PacMan\\food.png", &bmpfood));

	return R_OK;
}

Result __stdcall PostInit()
{
	Result rlt;

	// Perform food search
	LARGE_INTEGER tstart, tend, tfreq;
	QueryPerformanceFrequency(&tfreq);
	QueryPerformanceCounter(&tstart);
	//solution = BreadthFirstSearch(problem, PacmanStatePtrComparator);
	//solution = DepthFirstSearch(problem, PacmanStatePtrComparator);
	solution = UniformCostSearch(problem, PacmanStatePtrComparator);
	//solution = AStarSearch(problem, PacmanStatePtrComparator, manhattanHeuristic);
	QueryPerformanceCounter(&tend);
	cle->PrintLine(String("Path of cost ") << String((int)solution->size()) << String(" found in ") << String((int)((tend.QuadPart - tstart.QuadPart) * 1000 / tfreq.QuadPart)) << String("[ms]"));
	solution_iter = solution->begin();
	currentstate = problem.getStartState();

	// Draw first frame
	TimerElapsed(NULL);

	// Create timer
	Timer* tmr;
	eng->CreateTimer(TimerElapsed, 100, true, NULL, &tmr);

	return R_OK;
}

void __stdcall TimerElapsed(LPVOID user)
{
	if(solution_iter != solution->end())
	{
		// Validate next action
		if(problem.isActionLegal(currentstate, *solution_iter))
		{
			// Step solution
			currentstate = problem.generateSuccessor(currentstate, *solution_iter);
			solution_iter++;

			// Draw current state
			problem.Draw((PacmanState*)currentstate.get());
		}
		else
		{
			Result::PrintLogMessage("Solution contains illegal action");
			solution_iter = solution->end();
		}
	}

	/*// Draw game layout
	wnd->ClearPrimitives();
	int x, y;
	char wallbits;
	for(y = 0; y < layoutsize.height; y++)
		for(x = 0; x < layoutsize.width; x++)
		{
			switch(layout[y][x])
			{
			case '%':
				wallbits = 0x0;
				if(y > 0 && layout[y - 1][x] == '%')
					wallbits |= WALLBIT_NORTH;
				if(x < layoutsize.width - 1 && layout[y][x + 1] == '%')
					wallbits |= WALLBIT_EAST;
				if(y < layoutsize.height - 1 && layout[y + 1][x] == '%')
					wallbits |= WALLBIT_SOUTH;
				if(x > 0 && layout[y][x - 1] == '%')
					wallbits |= WALLBIT_WEST;
				if(wallbits == 0)
					Result::PrintLogMessage("Error inside wall layout: Wall without neighbour walls found");
				if(bmpwalls[wallbits] == 0)
					Result::PrintLogMessage(String("Error inside wall layout: Illegal wall bit combination: ") << String((int)wallbits));
				wnd->DrawBitmap(bmpwalls[wallbits], (float)(16 * x), (float)(16 * y), 16.0f, 16.0f);
				break;

			case 'P':
				wnd->DrawBitmap(bmppacman[DIR_EAST], (float)(16 * x), (float)(16 * y), 16.0f, 16.0f);
				break;

			case '.':
				wnd->DrawBitmap(bmpfood, (float)(16 * x), (float)(16 * y), 16.0f, 16.0f);
				break;
			}
		}
		wnd->Invalidate();*/

/*// Move north
if(pacmanpos.y > 0 && layout[pacmanpos.y - 1][pacmanpos.x] != '%')
{
	layout[pacmanpos.y][pacmanpos.x] = ' ';
	pacmanpos.y--;
	layout[pacmanpos.y][pacmanpos.x] = 'P';
}*/
}

void __stdcall End()
{
	RELEASE(gui);
	RELEASE(cle);
	RELEASE(fms);

	if(problem.walls)
	{
		for(int y = 0; y < layoutsize.height + 2; y++)
			delete[] problem.walls[y];
		delete[] problem.walls;
	}
	if(solution)
		delete solution;
}