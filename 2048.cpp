#include <ncurses.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <deque>
using namespace std;

typedef vector<int> line;
typedef pair<int, int> pos;

typedef struct _WIN_struct {

  int startx, starty;
  int height, width;
}WIN;

enum { LEFT, RIGHT, UP, DOWN };

void initWin(WIN *win)
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

  win->height = 9;
  win->width = 25;
  win->starty = 0;
  win->startx = (COLS - win->width) / 2;
}

void drawNums(const vector<line> &nums, WIN *win)
{
  for (int j=win->starty; j<win->starty + win->height; ++j) {
    for (int i=win->startx; i<win->startx + win->width;) {
      if ((j - win->starty) % 2 == 0) {
        if ((i - win->startx) % 6 == 0) {
          mvaddch(j, i, '+');
          i += 1;
        } else {
          mvprintw(j, i, "-----");
          i += 5;
        }
      } else {
        if ((i - win->startx) % 6 == 0) {
          mvaddch(j, i, '|');
          i += 1;
        } else {
          int x = (j - win->starty) / 2;
          int y = (i - win->startx) / 6;
          if (nums[x][y] == 0) {
            mvprintw(j, i, "     ");
          } else {
            attron(COLOR_PAIR(1));
            mvprintw(j, i, "%5d", nums[x][y]);
            attroff(COLOR_PAIR(1));
          }
          i += 5;
        }
      }
    }
  }
}

void printMsg(char *s)
{
  attron(COLOR_PAIR(2));
  mvprintw(10, 0, s);
  attroff(COLOR_PAIR(2));
  refresh();
}

int randPos()
{
  srand(time(NULL));
  return rand() % 4;
}

void initNums(vector<line> &nums)
{
  int x0 = randPos();
  int y0 = randPos();
  int x1, y1;
  do {
    x1 = randPos();
    y1 = randPos();
  } while(x1 == x0 && y1 == y0);

  nums.clear();
  for (int i=0; i<4; ++i) {
    line l;
    for (int j=0; j<4; ++j)
      l.push_back(0);
    nums.push_back(l);
  }
  nums[x0][y0] = 2;
  nums[x1][y1] = 2;
}

void mergeLeft(vector<line> &nums, vector<pos> &nullPos)
{
  for (int row=0; row<4; ++row) {
    bool lastMerged = false;
    deque<int> que;
    for (int col=0; col<4; ++col) {
      if (!lastMerged && !que.empty() && (que.back() == nums[row][col])) {
        que.pop_back();
        que.push_back(nums[row][col] << 1);
        lastMerged = true;
      } else if (nums[row][col] != 0) {
        que.push_back(nums[row][col]);
        lastMerged = false;
      }
    }
    for (int col=0; col<4; ++col) {
      if (!que.empty()) {
        nums[row][col] = que.front();
        que.pop_front();
      } else {
        nums[row][col] = 0;
        nullPos.push_back(make_pair(row, col));
      }
    }
  }
}

void mergeRight(vector<line> &nums, vector<pos> &nullPos)
{
  for (int row=0; row<4; ++row) {
    bool lastMerged = false;
    deque<int> que;
    for (int col=3; col>=0; --col) {
      if (!lastMerged && !que.empty() && (que.back() == nums[row][col])) {
        que.pop_back();
        que.push_back(nums[row][col] << 1);
        lastMerged = true;
      } else if (nums[row][col] != 0) {
        que.push_back(nums[row][col]);
        lastMerged = false;
      }
    }
    for (int col=3; col>=0; --col) {
      if (!que.empty()) {
        nums[row][col] = que.front();
        que.pop_front();
      } else {
        nums[row][col] = 0;
        nullPos.push_back(make_pair(row, col));
      }
    }
  }
}

void mergeUp(vector<line> &nums, vector<pos> &nullPos)
{
  for (int col=0; col<4; ++col) {
    bool lastMerged = false;
    deque<int> que;
    for (int row=0; row<4; ++row) {
      if (!lastMerged && !que.empty() && (que.back() == nums[row][col])) {
        que.pop_back();
        que.push_back(nums[row][col] << 1);
        lastMerged = true;
      } else if (nums[row][col] != 0) {
        que.push_back(nums[row][col]);
        lastMerged = false;
      }
    }
    for (int row=0; row<4; ++row) {
      if (!que.empty()) {
        nums[row][col] = que.front();
        que.pop_front();
      } else {
        nums[row][col] = 0;
        nullPos.push_back(make_pair(row, col));
      }
    }
  }
}


void mergeDown(vector<line> &nums, vector<pos> &nullPos)
{
  for (int col=0; col<4; ++col) {
    bool lastMerged = false;
    deque<int> que;
    for (int row=3; row>=0; --row) {
      if (!lastMerged && !que.empty() && (que.back() == nums[row][col])) {
        que.pop_back();
        que.push_back(nums[row][col] << 1);
        lastMerged = true;
      } else if (nums[row][col] != 0) {
        que.push_back(nums[row][col]);
        lastMerged = false;
      }
    }
    for (int row=3; row>=0; --row) {
      if (!que.empty()) {
        nums[row][col] = que.front();
        que.pop_front();
      } else {
        nums[row][col] = 0;
        nullPos.push_back(make_pair(row, col));
      }
    }
  }
}

int nextNum()
{
  srand(time(NULL));
  int ri = rand() % 1000 + 1;
  return ri > 100 ? 2 : 4;
}

void merge(vector<line> &nums, int direction)
{
  vector<pos> nullPos;
  switch(direction) {
    case LEFT:
      mergeLeft(nums, nullPos);
      break;
    case RIGHT:
      mergeRight(nums, nullPos);
      break;
    case UP:
      mergeUp(nums, nullPos);
      break;
    case DOWN:
      mergeDown(nums, nullPos);
      break;
  }
  int nullPosSize = nullPos.size();
  if (nullPosSize > 0) {
    srand(time(NULL));
    int randPos = rand() % nullPosSize;
    nums[nullPos[randPos].first][nullPos[randPos].second] = nextNum();
  }
}

int judge(vector<line> &nums)
{
  for (int row=0; row<4; ++row) {
    for (int col=0; col<4; ++col) {
      if (nums[row][col] == 0) {
        return 0; //继续
      } else if (nums[row][col] == 2048) {
        return 1; //完成
      } else {
        if (row - 1 >=0 && nums[row][col] == nums[row-1][col]) return 0;
        if (col - 1 >=0 && nums[row][col] == nums[row][col-1]) return 0;
        if (row + 1 < 4 && nums[row][col] == nums[row+1][col]) return 0;
        if (col + 1 < 4 && nums[row][col] == nums[row][col+1]) return 0;
      }
    }
  }
  return -1;//失败
}

void success()
{
  mvprintw(11, COLS / 2 , "SUCCESS!");
}

void fail()
{
  mvprintw(11, COLS / 2 , "FAIL!");
}

int main()
{
  vector<line> nums;
  

  WIN win;
  initWin(&win);
  initNums(nums);
  drawNums(nums, &win);

  int ch;
  while((ch = getch()) != 'q') {
    switch(ch) {
      case 'h':
      case 'H':
        merge(nums, LEFT);
        drawNums(nums, &win);
        break;
      case 'l':
      case 'L':
        merge(nums, RIGHT);
        drawNums(nums, &win);
        break;
      case 'k':
      case 'K':
        merge(nums, UP);
        drawNums(nums, &win);
        break;
      case 'j':
      case 'J':
        merge(nums, DOWN);
        drawNums(nums, &win);
        break;
      case 'r':
      case 'R':
        initNums(nums);
        drawNums(nums, &win);
        break;
    }
    if (judge(nums) == 1) {
      success();
      getch();
      break;
    } else if (judge(nums) == -1) {
      fail();
      getch();
      break;
    }
  }
  endwin();
  return 0;
}
