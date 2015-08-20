// constructing priority queues
#include <iostream>     // std::cout
#include <queue>        // std::priority_queue
#include <vector>       // std::vector
#include <functional>   // std::greater
#include <algorithm>	// std::sort
#include <stack>
#include <cstdlib>
#include <stdlib.h>
#include <Windows.h>
#include <cassert>
#include <cmath>
#include <cstring>
#include <limits>

using namespace std;

#define HEIGHT 20
#define WIDTH 20

enum tile_state { route = -3, blocked = -2, unvisited = -1}; //values 0 or greater denote distance from start(?)
enum List { none, open, closed };

struct node {
	int x, y;
	int f, g, h;
	node* parent;
	//bool used, blocked;
	List list;

	node(int x_ = 0, int y_ = 0)
	{
		x = x_;
		y = y_;
		f = unvisited;
		list = none;
	}

	node(const node &a)	//copy constructor
	{
		x = a.x;
		y = a.y;
		g = a.g; h = a.h; f = a.f;
		parent = a.parent;
		list = a.list;
	}

	void Set_parent(node *parent_)
	{
		parent = parent_;
	}

	void Calc_fgh(node &a, const node s, const node g)
	{
		//a.g = (abs(s.x - a.x) + abs(s.y - a.y));
		a.g = parent->g + 1;
		a.h = (abs(g.x - a.x) + abs(g.y - a.y));

		a.f = a.h + a.g;
	}


	bool operator>(const node &a) const {
		return h > a.h;
	}

	bool operator <(const node &a) const {
		if (f != a.f)
			return f < a.f;
		else //if (f == a.f)
			return h < a.h;
	}

	bool operator ==(const node &a) const
	{
		if ((x == a.x) && (y == a.y)){
			return true;
		}
		return false;
	}
};

bool checkNode(node current, int dX, int dY);
void SetListToOpen(node &a);
void SetListToClosed(node &a);

priority_queue< node, vector< node >, std::greater<node> /*std::less<node>*/ > OPEN_list;  // set up a priority queue
vector<node> open_list;
vector<node> closed_list;					// closed list for nodes we already looked at
stack<node*> path;							// set up a stack

vector<vector<node>> map_array;
node start(1, 1);		//set start node
node goal(7, 8);

void Draw_map(vector<vector<node>> &map_array, node s, node g, node c)
{
	system("cls");
	for (int row = 0; row < WIDTH; ++row){
	for (int column = 0; column < HEIGHT; ++column){
		if(map_array[column][row].f == blocked)
			cout << " X ";
		else if (column == c.x && row == c.y)
			cout << " C ";
		else if (column == g.x && row == g.y)
			cout << " G ";
		else if (column == s.x && row == s.y)
			cout << " S ";
		else if (map_array[column][row].f == route)
			cout << " P ";
		else if(map_array[column][row].f == unvisited)
			cout << " - ";
		else
		{
			cout << " "<< map_array[column][row].h;
			if(map_array[column][row].h < 10)
				cout << " ";
		}
	}
			cout << endl;
	}
	Sleep(500);
};


int main()
{
	// Could move Map setup to its own function that randomises obstacles, start & goal

	// Set up sizes. (HEIGHT x WIDTH)
	map_array.resize(HEIGHT);
	for (int i = 0; i < HEIGHT; ++i)
    map_array[i].resize(WIDTH);

	for (int column = 0; column < WIDTH; ++column){
	for (int row = 0; row < HEIGHT; ++row){
		map_array[column][row].x=column;
		map_array[column][row].y=row;
	}}

	//set obstacles
	map_array[2][1].f = blocked;
	map_array[2][2].f = blocked;
	map_array[2][3].f = blocked;
	map_array[2][5].f = blocked;
	map_array[3][5].f = blocked;
	map_array[5][5].f = blocked;
	map_array[6][4].f = blocked;
	map_array[6][8].f = blocked;
	map_array[8][8].f = blocked;
	map_array[7][7].f = blocked;

	//start.Calc_fgh(start,start,goal);		//	calculate f,g,h of s
	start.g = 0;
	SetListToOpen(start);	// add starting node to open list
	map_array[start.x][start.y]=start;

	node c_node = open_list.front();	//set c_node values as closed_list.front values (start)
	open_list.erase(open_list.begin());

	// might be better to move this to it's own function that returns tru if a path is found, false otherwise.

	while ((c_node.y != goal.y) || (c_node.x != goal.x))	//while c_node != goal node
	{	
		SetListToClosed(c_node);	// add the current node to closed list

		// check nodes around our current
		checkNode(c_node,  0, -1);	// up
		checkNode(c_node,  1,  0);	// right
		checkNode(c_node,  0,  1);	// down
		checkNode(c_node, -1,  0);	//left

		Draw_map(map_array, start, goal, c_node);
		std::sort(open_list.begin(), open_list.end());
		c_node = open_list.front();		// set new c_node values as OPEN_list.top value
		open_list.erase(open_list.begin());
	}


	//path creation
	path.push(&c_node);
	while((path.top()->y !=start.y) || (path.top()->x != start.x))
	{
		path.push(path.top()->parent);
		path.top()->f = route;
	}
	
	//print out path
	Draw_map(map_array, start, goal, c_node);
    while ( path.size() > 0 ) {
        cout << path.top()->x << ' ' << path.top()->y << endl;
        path.pop();
    }

	cin.get();

}

bool checkNode(node current, int dX, int dY) // why return node and pass by reference? redundant.
{
	int iX = current.x + dX;
	int iY = current.y + dY;

	if ((iX < 0 || iX >= WIDTH) || (iY < 0 || iY >= HEIGHT)){
		cout << "out of bounds";
		return false;
	}
	else
	{
		node target = map_array[iX][iY];

		if (target.f != blocked && target.list != closed) // if its not blocked or on closed list
		{
			if (target.list != open){	// if its not already been looked at add it to the open list
				target.Set_parent(&map_array[current.x][current.y]);
				target.Calc_fgh(target, start, goal);			// calculate f,g,h values
				SetListToOpen(target);
			}
			else { // if this node has been checked before (so on the open list), see if this path is better 
				if ((current.g + 1) < target.g){ // if the new path from the current node is better than what the target node had previously
					target.Set_parent(&map_array[current.x][current.y]);
					target.Calc_fgh(target, start, goal);			// calculate f,g,h values
				}
			}
		}

		map_array[iX][iY] = target;
		return true;
	}
}

void SetListToOpen(node &a)
{
	if (a.list != open){
		if (a.list == closed){
			for (unsigned i = closed_list.size(); i-- > 0; ) {		// check every member of the closed list
				if (a == closed_list[i])							// if that entry is the same as node a
				{ closed_list.erase(closed_list.begin() + i); }		// remove it form the list
			}
		}
		a.list = open;
		open_list.push_back(a);
			}
}

void SetListToClosed(node &a)
{
	if (a.list != closed){
		if (a.list == open){
			for (unsigned i = open_list.size(); i-- > 0;) {		// check every member of the open list
				if (a == open_list[i])							// if that entry is the same as node a
				{ open_list.erase(open_list.begin() + i); }		// remove it form the list
			}
		}
		a.list = closed;
		closed_list.push_back(a);
	}
}
