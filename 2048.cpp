#include <ncurses.h>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <vector>
#include <deque>
#include <iostream>
using namespace std;

typedef vector<int> line;
typedef pair<int, int> pos;

typedef struct _WIN_struct {

  int startx, starty;
  int height, width;
}WIN;




class Game
{
  public:
    Game(int n, int target): _target(target), _n(n), _status(NORMAL) {
      srand(time(NULL));
    }
    void initData();
    void initWin();
    void processInput();
    void draw();
    bool isQuit() { return _status == QUIT; }
    void clearWin() { endwin(); }
  private:
    int _target;
    int _n;
    enum { NORMAL, QUIT, OVER, SUCCESS };
    int _status;
    vector<line> _data;
    const char *DESC    = "QUIT(Q),RESTART(R),UP(W),DOWN(S),LEFT(A),RIGHT(D)";
    const char *success = "YOU WIN";
    const char *over    = "GAME OVER";

    WIN win;

    bool moveLeft();
    bool isOver();
    void rotateData();
    void randNew();
    void printMsg(const char *msg, int starty);
};

void Game::initWin()
{
  initscr();
  if(has_colors() == FALSE)
  {
    endwin();
    printf("Your terminal does not support color\n");
    exit(1);
  }

  start_color();
  init_pair(1, COLOR_RED, COLOR_BLACK);
  cbreak();
  noecho();

  win.height = _n * 2 + 1;
  win.width = _n * 6 + 1; // _n * 5 + _n + 1
  win.starty = 0;
  win.startx = (COLS - win.width) / 2;

  printMsg(DESC, win.height + 1);
}

void Game::initData() {
  _data.clear();
  for (int row = 0; row < _n; ++row) {
    line l;
    for (int col = 0; col < _n; ++col) {
      l.push_back(0);
    }
    _data.push_back(l);
  }
  randNew();
  randNew();
}

void Game::draw()
{
  if (_status == NORMAL) {
    for (int j=win.starty; j<win.starty + win.height; ++j) {
      for (int i=win.startx; i<win.startx + win.width;) {
        if ((j - win.starty) % 2 == 0) {
          if ((i - win.startx) % 6 == 0) {
            mvaddch(j, i, '+');
            i += 1;
          } else {
            mvprintw(j, i, "-----");
            i += 5;
          }
        } else {
          if ((i - win.startx) % 6 == 0) {
            mvaddch(j, i, '|');
            i += 1;
          } else {
            int x = (j - win.starty) / 2;
            int y = (i - win.startx) / 6;
            if (_data[x][y] == 0) {
              mvprintw(j, i, "     ");
            } else {
              attron(COLOR_PAIR(1));
              mvprintw(j, i, "%5d", _data[x][y]);
              attroff(COLOR_PAIR(1));
            }
            i += 5;
          }
        }
      }
    }
    printMsg(DESC, win.height + 1);
  } else if (_status == OVER) {
    printMsg(over, win.height / 2);
  } else if (_status == SUCCESS) {
    printMsg(success, win.height / 2);
  }
}

void Game::processInput() {
  char ch;
  bool changed;
  ch = getch();
  if (ch >= 'A' && ch <= 'Z')
    ch -= ('A' - 'a');

  if (ch == 'r') {
    initData();
  } else if (ch == 'q') {
    _status = QUIT;
  }

  if (_status == NORMAL) {
    switch(ch) {
      case 'w':
        rotateData();
        changed = moveLeft();
        rotateData();
        rotateData();
        rotateData();
        break;
      case 'a':
        changed = moveLeft();
        break;
      case 's':
        rotateData();
        rotateData();
        rotateData();
        changed = moveLeft();
        rotateData();
        break;
      case 'd':
        rotateData();
        rotateData();
        changed = moveLeft();
        rotateData();
        rotateData();
        break;
    }
    if (changed) {
      randNew();
    } else if(isOver()) {
      _status = OVER;
    }
  }
}

bool Game::moveLeft() {
  int lastValue;
  int curWritePos;
  vector<line> tmpData(_data);

  for (int row = 0; row < _n; ++row) {
    lastValue = 0;
    curWritePos = 0;
    for (int col = 0; col < _n; ++col) {
      if (_data[row][col] == 0) {
        continue;
      }

      if (lastValue == 0) {
        lastValue = _data[row][col];
      } else {
        if (lastValue == _data[row][col]) {
          _data[row][curWritePos++] = lastValue << 1;
          if (_data[row][curWritePos] == _target) {
            _status = SUCCESS;
          }
          lastValue = 0;
        } else {
          _data[row][curWritePos++] = lastValue;
          lastValue = _data[row][col];
        }
      }
      _data[row][col] = 0;
    }
    if (lastValue != 0) {
      _data[row][curWritePos] = lastValue;
    }
  }

  for (int row = 0; row < _n; ++row) {
    for (int col = 0; col < _n; ++col) {
      if (_data[row][col] != tmpData[row][col]) return true;
    }
  }
  return false;
}

//逆时针旋转90度
void Game::rotateData()
{
  vector<line> tmpData(_data);
  for (int row = 0; row < _n; ++row) {
    for (int col = 0; col < _n; ++col) {
      tmpData[row][col] = _data[col][_n - 1 - row];
    }
  }
  for (int row = 0; row < _n; ++row) {
    for (int col = 0; col < _n; ++col) {
      _data[row][col] = tmpData[row][col];
    }
  }
}

void Game::randNew() {
  vector<int> emptyPos;
  for (int row = 0; row < _n; ++row) {
    for (int col = 0; col < _n; ++col) {
      if (_data[row][col] == 0) {
        emptyPos.push_back(row * _n + col);
      }
    }
  }
  if (emptyPos.size() > 0) {
    int randPosSum = emptyPos[rand() % emptyPos.size()];
    _data[randPosSum / _n][randPosSum % _n] = rand() % 10 == 1 ? 4 : 2;
  }
}

bool Game::isOver()
{
  for (int row = 0; row < _n; ++row) {
    for (int col = 0; col < _n; ++col) {
      if ((row + 1 < _n) && ((_data[row][col] * _data[row+1][col] == 0) || (_data[row+1][col] == _data[row][col]))) return false;
      if ((col + 1 < _n) && ((_data[row][col] * _data[row][col+1] == 0) || (_data[row][col+1] == _data[row][col]))) return false;
    }
  }
  return true;
}

void Game::printMsg(const char *msg, int starty)
{
  mvprintw(starty, (COLS - strlen(msg)) / 2, msg);
}

int main()
{
  Game game(4, 2048);
  game.initData();
  game.initWin();

  do {
    game.draw();
    game.processInput();
  } while (!game.isQuit());

  getch();
  game.clearWin();
  return 0;
}
