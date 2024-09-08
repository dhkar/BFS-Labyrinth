#include "stdio.h"
#include "stdlib.h"

#define MAX_DIM_DIGIT 10

// Data Structures
typedef struct cell
{
    int xcoord, ycoord;
    struct cell *next;
    int isVisited;
    int isATrack;
    char celltype;

} cell;

typedef struct graph
{
    cell *cells;
    cell *start;
    cell *end;

} Graph;

typedef struct queue
{
    cell *head, *tail;
    int num;
} Queue;

// Function for Finding Neighbor
int cekKanan(char **_mapArray, int i, int j, int *xd, int *yd)
{
    if (j + 1 == *xd)
    {
        return -1;
    }
    return (_mapArray[i][j + 1] != '#') ? (*xd * i + (j + 1)) : -1;
}
int cekKiri(char **_mapArray, int i, int j, int *xd, int *yd)
{
    if (j - 1 < 0)
    {
        return -1;
    }
    return (_mapArray[i][j - 1] != '#') ? (*xd * i + (j - 1)) : -1;
}
int cekAtas(char **_mapArray, int i, int j, int *xd, int *yd)
{
    if (i - 1 < 0)
    {
        return -1;
    }
    return (_mapArray[i - 1][j] != '#') ? (*xd * (i - 1) + j) : -1;
}
int cekBawah(char **_mapArray, int i, int j, int *xd, int *yd)
{
    if (i + 1 == *yd)
    {
        return -1;
    }
    return (_mapArray[i + 1][j] != '#') ? (*xd * (i + 1) + j) : -1;
}

// Recursive to determine power
int r_pow(int a, int b)
{
    return (!b) ? 1 : (a * (r_pow(a, b - 1)));
}

// Convert String to Int
int stringToInt(char *arr)
{
    // Using ASCII concepts
    int num_of_digit = 0, temp = 0, res = 0;
    while (arr[num_of_digit] != '\n')
    {
        num_of_digit++;
    }
    for (int j = 0; j < num_of_digit; ++j)
    {
        temp = (int)arr[j];
        temp = temp - 48;
        res += temp * r_pow(10, num_of_digit - j - 1);
    }

    return res;
}

// Convert mapfile to Array
char **getLabyrinthMap(FILE **mapfile, int *xd, int *yd)
{

    // Get Map Dimesion x : width and y : height
    int temp = 0, x = 0, y = 0, i = 0;
    int xdigit = 0, ydigit = 0;
    char _x[MAX_DIM_DIGIT];
    char _y[MAX_DIM_DIGIT];
    char buf;

    // Assume the maximum dimension is 10^10-1
    fgets(_y, MAX_DIM_DIGIT, *mapfile);
    fgets(_x, MAX_DIM_DIGIT, *mapfile);

    // Convert string to Integer
    y = stringToInt(_y);
    x = stringToInt(_x);

    // Memory Allocation
    char **mapArray;
    mapArray = malloc(y * sizeof(*mapArray));
    for (int j = 0; j < y + 1; ++j)
    {
        mapArray[j] = malloc((x) * sizeof(mapArray[0]));
        fgets(mapArray[j], x + 2, *mapfile);
    }

    // Save dimension address to pointer to integer
    *xd = x;
    *yd = y;
    return mapArray;
}

// Initialize Graph from cells (node) structure data, data are from mapArray
Graph initGraph(Graph _graph, char **_mapArray, int *xd, int *yd)
{
    cell *tmp = malloc((*xd) * (*yd) * sizeof(struct cell));
    Graph graph;
    int idx = 0;

    // Inserts data from map Array to cells in graph
    for (int i = 0; i < *yd; ++i)
    {
        for (int j = 0; j < *xd; ++j)
        {
            tmp[idx].xcoord = j;
            tmp[idx].ycoord = i;
            tmp[idx].isATrack = 0;
            tmp[idx].isVisited = 0;
            tmp[idx].celltype = _mapArray[i][j];
            tmp[idx].next = NULL;
            if (_mapArray[i][j] != '#')
            {
                tmp[idx].isATrack = 1;
                if (_mapArray[i][j] == 's')
                {
                    graph.start = &tmp[idx];
                }
                if (_mapArray[i][j] == 'e')
                {
                    graph.end = &tmp[idx];
                }
            }

            idx++;
        }
    }

    graph.cells = tmp;
    return graph;
}

// Initizlize Queue for BFS
void initQueue(Queue *_queue)
{

    _queue->head = NULL;
    _queue->tail = NULL;
    _queue->num = 0;
}

// To add cells to queue
Queue *enqueue(Queue *_queue, cell *attr)
{
    cell *tmp = malloc(sizeof(struct cell));
    tmp = attr;
    Queue *temp = malloc(sizeof(struct queue));
    temp = _queue;
    if (temp->tail == NULL)
    {
        temp->head = tmp;
        temp->tail = tmp;
    }
    else
    {
        temp->tail->next = tmp;
        temp->tail = tmp;
    }

    temp->num = temp->num + 1;
    return temp;
}

// To remove cells (head) to queue
int dequeue(Queue *_queue, int *xd)
{
    int n = 0;
    cell *tmp;
    if (_queue->head == NULL)
        return 0;
    else
    {
        n += _queue->head->xcoord;
        n += (*xd) * _queue->head->ycoord;

        _queue->head = _queue->head->next;
        _queue->num--;
        if (_queue->head == NULL)
        {
            _queue->tail == NULL;
        }
    }
    return n;
}

// index conversion from cell coordinate
int curridx(cell *attr, int *xd)
{
    return attr->xcoord + (*xd) * attr->ycoord;
}

// START BREADTH FIRST SEARCH
void doBFS(Graph _graph, char **mapArray, int *xd, int *yd)
{

    // Initialization, of visited and history array
    Queue *q_bfs;
    int cell_traverse;
    int left = 0, right = 0, up = 0, down = 0;
    q_bfs = malloc(sizeof(struct queue));
    initQueue(q_bfs);
    cell *curr_cell;
    cell *temp;
    int *visited = malloc((*xd) * (*yd) * sizeof(int));
    int *history = malloc((*xd) * (*yd) * sizeof(int));
    char *solution = malloc((*xd) * (*yd) * sizeof(char));
    for (int i = 0; i < (*xd) * (*yd); ++i)
    {
        visited[i] = 0;
        history[i] = 9999;
        solution[i] = '-';
    }

    // curr_cell = Start node/cell
    curr_cell = _graph.start;
    q_bfs = enqueue(q_bfs, curr_cell);

    // label start as visited
    visited[curridx(curr_cell, xd)] = 1;

    while ((q_bfs->num) != 0)
    {

        cell_traverse = curridx(q_bfs->head, xd);

        // Find Neighbors
        right = cekKanan(mapArray, cell_traverse / (*xd), cell_traverse % (*xd), xd, yd);
        left = cekKiri(mapArray, cell_traverse / (*xd), cell_traverse % (*xd), xd, yd);
        up = cekAtas(mapArray, cell_traverse / (*xd), cell_traverse % (*xd), xd, yd);
        down = cekBawah(mapArray, cell_traverse / (*xd), cell_traverse % (*xd), xd, yd);

        // Enqueue cells, and mark as visited
        if (!(visited[right]) && (right != -1))
        {
            q_bfs = enqueue(q_bfs, &_graph.cells[right]);
            visited[right] = 1;
            history[right] = cell_traverse;
        }

        if (!(visited[up]) && (up != -1))
        {
            q_bfs = enqueue(q_bfs, &_graph.cells[up]);
            visited[up] = 1;
            history[up] = cell_traverse;
        }

        if (!(visited[left]) && (left != -1))
        {
            q_bfs = enqueue(q_bfs, &_graph.cells[left]);
            visited[left] = 1;
            history[left] = cell_traverse;
        }

        if (!(visited[down]) && (down != -1))
        {
            q_bfs = enqueue(q_bfs, &_graph.cells[down]);
            visited[down] = 1;
            history[down] = cell_traverse;
        }
        cell_traverse = dequeue(q_bfs, xd);
    }

    // Determine solution path from history array, and Print solution
    int i = curridx(_graph.end, xd);
    int isExist = 1;
    int tr = 0;
    while (i != curridx(_graph.start, xd) && (tr < (*xd) * (*yd)))
    {
        tr++;
        solution[i] = '*';

        if (history[i] == 9999)
        {
            isExist = 0;
            break;
        }
        i = history[i];
    }
    if (isExist)
    {
        solution[i] = '*';
        printf("\nSolution: \n");

        for (int i = 0; i < (*xd) * (*yd); ++i)
        {
            if ((i % (*xd) == 0) && i != 0)
            {
                printf("\n");
            }
            printf("%c", solution[i]);
        }
    }
    else
    {
        printf("Solution doesn't exist.");
    }
}

int main()
{
    FILE *file;
    char filename[20];
    char **mapArray;
    int X, Y, xs, ys, xe, ye;
    printf("Enter maze file path: ");
    scanf("%s", filename);
    file = fopen(filename, "r");

    // Convert mapfile to mapArray
    mapArray = getLabyrinthMap(&file, &X, &Y);

    // Initialize graph from map Array
    Graph graph;
    graph = initGraph(graph, mapArray, &X, &Y);

    // For User Interface Purpose
    for (int i = 0; i < X * Y; i++)
    {
        if (graph.cells[i].celltype == 'e')
        {
            xe = graph.cells[i].xcoord;
            ye = graph.cells[i].ycoord;
        }
        if (graph.cells[i].celltype == 's')
        {
            xs = graph.cells[i].xcoord;
            ys = graph.cells[i].ycoord;
        }
    }
    printf("Width: %d\n", X);
    printf("Height: %d\n", Y);
    printf("Starting point: (%d,%d)\n", xs, ys);
    printf("Ending point: (%d,%d)\n\n", xe, ye);
    printf("Map contents: \n");
    for (int i = 0; i < Y; i++)
    {
        for (int j = 0; j < X; j++)
        {
            printf("%c", mapArray[i][j]);
        }
        printf("\n");
    }

    // START BFS, print solution
    doBFS(graph, mapArray, &X, &Y);
}