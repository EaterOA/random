/*
An implementation and sample demonstration of A* on a 2D grid map.
The grid size is determined by FSIZE, and the start and end points are random.
Ten walls of random length will be placed around the grid. The demo will show
each node that the algorithm looks at and, in the end, marks the shortest path
from start to end.
*/

#include <iostream>
#include <time.h>
#include <string.h>
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <queue>
#include <math.h>
#include <set>
#include <map>

using namespace std;

#define FSIZE 30
#define RAND(a,b) (rand() % ((b)-(a)+1) + (a))

const int DR[] = {0, 1, 0, -1};
const int DC[] = {1, 0, -1, 0};

struct Node
{
    Node(int rr, int cc, int h , int d): r(rr), c(cc), heuristic(h), dist(d)
    {
        priority = heuristic + dist;
        fromR = -1;
        fromC = -1;
    }
    int r, c;
    int heuristic;
    int dist;
    int priority;
    int fromR, fromC;

    // This comparison is the core of algorithm. Each node is classified with
    // a distance from start and a heuristic - in this case, simply the
    // Manhatten distance to the end. Comparing first by priority, which is
    // dist + heuristic, ensures optimal shortest paths, and comparing next
    // by heuristic ensures that you will always be moving towards the goal.
    bool operator<(const Node& other) const
    {
        if (priority != other.priority)
            return priority < other.priority;
        if (heuristic != other.heuristic)
            return heuristic < other.heuristic;
        if (r != other.r)
            return r < other.r;
        return c < other.c;
    }
};

inline bool isValid(int r, int c)
{
    return (r >= 0 && r < FSIZE && c >= 0 && c < FSIZE);
}

inline int getDist(int r1, int c1, int r2, int c2)
{
    return abs(r1-r2) + abs(c1-c2);
}

inline int hash(int r, int c)
{
    return r*FSIZE + c;
}

void placeWall(char field[FSIZE][FSIZE], int row, int col, int height, int width)
{
    for (int i = row; i < row+height; i++)
        for (int j = col; j < col+width; j++)
            field[i][j] = '@';
}

void placeRandomWall(char field[FSIZE][FSIZE])
{
    int height = RAND(FSIZE/7, FSIZE-1);
    int width = RAND(1, FSIZE/12);
    if (RAND(0,1)) {
        swap(height, width);
    }
    int row = RAND(0, FSIZE-height);
    int col = RAND(0, FSIZE-width);
    placeWall(field, row, col, height, width);
}

void printField(char field[FSIZE][FSIZE])
{
    for (int i = 0; i < FSIZE; i++) {
        for (int j = 0; j < FSIZE; j++)
            cout << field[i][j];
        cout << "\n";
    }
}

int main()
{
    srand((unsigned)time(0));
    char field[FSIZE][FSIZE];
    memset(field, '-', FSIZE*FSIZE);

    // Generate walls
    for (int i = 0; i < 10; i++)
        placeRandomWall(field);

    // Generate start and end location
    int curR = -1, curC = -1, endR = -1, endC = -1;
    while (curR == endR && curC == endC || field[curR][curC] == '@' || field[endR][endC] == '@') {
        curR = RAND(0,FSIZE-1);
        curC = RAND(0,FSIZE-1);
        endR = RAND(0,FSIZE-1);
        endC = RAND(0,FSIZE-1);
    }
    field[curR][curC] = '*';
    field[endR][endC] = 'x';

    map<int, Node> seen;
    set<Node> next;
    Node n(curR, curC, getDist(curR, curC, endR, endC), 0);
    next.insert(n);
    bool found = false;

    // This A* implementation uses the std::set implementation from
    // dijkstra.cpp, guaranteeing O(ElogV) performance
    while (!next.empty()) {

        set<Node>::iterator first = next.begin();
        n = *first;
        next.erase(first);

        // Prevent duplicate traversals
        if (seen.find(hash(n.r, n.c)) != seen.end())
            continue;
        seen.insert(pair<int,Node>(hash(n.r, n.c), n));

        // Reached destination
        if (getDist(n.r, n.c, endR, endC) == 0) {
            found = true;
            break;
        }

        // Look for neighboring nodes
        for (int i = 0; i < 4; i++) {
            int nextR = n.r + DR[i];
            int nextC = n.c + DC[i];
            if (!isValid(nextR, nextC) || field[nextR][nextC] == '@' || seen.find(hash(nextR, nextC)) != seen.end())
                continue;
            int d = 1 + n.dist;
            int h = getDist(nextR, nextC, endR, endC);
            Node nextNode(nextR, nextC, h, d);
            nextNode.fromR = n.r;
            nextNode.fromC = n.c;
            next.insert(nextNode);
        }

        // Update grid appearance and draw it
        field[curR][curC] = '#';
        curR = n.r;
        curC = n.c;
        field[curR][curC] = '*';
        printField(field);
        getchar();
        const string separator(80, '=');
        cout << separator << "\n";
    }

    // If a path is found, mark the shortest path
    if (found) {
        while (curR != -1) {
            field[curR][curC] = '*';
            Node n = seen.find(hash(curR, curC))->second;
            curR = n.fromR;
            curC = n.fromC;
        }
        printField(field);
    }
}
