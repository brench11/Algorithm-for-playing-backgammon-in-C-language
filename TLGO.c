/*

*/
#define _CRT_SECURE_NO_WARNINGS
#include<graphics.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#define Width 960
#define High 800
#define MAXV 10000000000
#define MINV -10000000000
int boardgap = 50;
int cen = 3; //第cen层开始统计分数
int checkerBoard[15][15] = { 0 };
int proBoard[15][15] = { 0 };
int steps;
struct listNode;
typedef struct listNode* pNode;
struct treeNode
{
	int level;
	int alpha;
	int beta;
	int x1, y1;
	int score;
	int board[15][15];
	struct treeNode* father;
	pNode chileHead;
};
typedef struct treeNode* pTree;

struct listNode {   //一个指针链表，存每一个叶子节点;
	pTree treeChild;
	pNode next;
};
int beSide = 1;//1为黑方，-1为白方,给  checkerboard 赋值后，视为落子，回合结束，置换beSide的值
pTree treeHead = NULL;
void startup();
void setPosition(int[][15], int[][15]);
void problePosition(int proBoard1[][15]);
void resetProblePosition();
void show();
void TLGO();
int aPointIsOver(int, int, int [][15], int);
void updateWithInput();
int pixpoi(int);
int evaluate();
int pointIsOk(int, int);
pTree buildGameTree(pTree, int);
int maxNumber(int, int, int);
int minNumber(int, int, int);
int main() noexcept
{
	startup();
	while (1)
	{
		show();
		if (beSide == 1)//轮到黑
		{
			if (steps == 0)
			{
				steps++;
				beSide = -beSide;
				checkerBoard[7][7] = 1;
			}
			else
			{
				TLGO();
			}
		}
		else
		{
			updateWithInput();

		}
	}
	return 0;
}
pTree buildGameTree(pTree father, int level)
{
	int proBoard1[15][15] = { 0 };
	level++;
	setPosition(proBoard1, father->board);
	proproblePosition(proBoard1);
	pTree current;
	pNode pre, cur,temp;
	pre = cur = NULL;
	if (level < cen)
	{
		//应注意到，这个for循环中的所有current节点，应该存到father节点的链表之中!
		for (int i = 0; i < 15; i++)     //开始遍历所有可疑点
		{
			for (int j = 0; j < 15; j++)
			{
				if (proBoard1[i][j] == 2 ) //点(i,j)是当前的一个可疑点
				{		
					current = (pTree)malloc(sizeof(struct treeNode));
					current->father = father;
					current->alpha = father->alpha; current->beta = father->beta;
					current->level = level;
					setPosition(current->board, father->board);
					current->x1 = i; current->y1 = j;
					current->score = 0;
					current->chileHead = NULL;
					//不管这个点怎么样都加入到father的链表中，区别只是不会继续向下搜索
					cur = (pNode)malloc(sizeof(struct listNode));
					cur->treeChild = current;
					cur->next = NULL;
					if (father->chileHead == NULL) //current将会是father的第一个子节点
					{
						father->chileHead = cur;
						pre = cur;
					}
					else
					{
						pre->next = cur;
						pre = cur;
					}
					if (aPointIsOver(i, j, father->board, level) != 1)//这个点不会造成比赛结束
					{
						buildGameTree(current, level);
					}
					else //这个点会终结比赛
					{
						if (level % 2 == 0) //当前这个点，可能会让白子终结比赛,即在min层
						{
							current->beta = -10000000;
						}
						else
						{
							current->alpha = 10000000;
						}
					}

				}
				//运行到此处，当前可疑点current的所有子节点应已经添加完毕
				temp = current->chileHead;
				while (temp != NULL)
				{
					if (level % 2 == 0) //在min层
					{
						current->beta = minNumber(current->beta, temp->treeChild->alpha, temp->treeChild->beta);
					}
					else //在max层
					{
						current->alpha = maxNumber(current->alpha, temp->treeChild->alpha, temp->treeChild->beta);
					}
					if (current->alpha > current->beta) {
						break;
					}
					temp = temp->next;
				}
				//对于current已经完成alpha，beta的复制，以及current子节点的剪枝
			}
		}
		//运行到此处，father所有可疑点的子节点的子节点都已经处理完毕，但没有处理father与father自己的子节点的关系；
	}
	else if (level == cen)
	{
		for (int i = 0; i < 15; i++)
		{
			for (int j = 0; j < 15; j++)
			{
				if (proBoard1[i][j] == 2)
				{
					//在最后一层没有必要对father在建一个链表，每一个节点与father的值比较，满足条件就更新即可
					int score = evaluate();
					if (score < father->beta)
					{
						father->beta = score;
					}
				}
			}
		}
		
	}

}

void TLGO()
{

}
int pixpoi(int a)
{
	return 50 + 50 * a;
}
int pointIsOk(int x, int y)
{
	for (int i = 0; i < 15; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			int s = (x - pixpoi(i)) * (x - pixpoi(i)) + (y - pixpoi(j)) * (y - pixpoi(j));
			if (s < 25 * 25) //(i,j)就是当前玩家要下的点
			{
				if (checkerBoard[i][j] == 0)//当前点可以被选择
				{
					if (beSide == 1)
					{
						checkerBoard[i][j] = 1;
					}
					else {
						checkerBoard[i][j] = -1;
					}
					beSide = -beSide;
					steps++;
					return 1;
				}
			}
		}
	}
	return 0;
}
void updateWithInput()
{
	MOUSEMSG m;
	if (MouseHit())
	{
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN)
		{
			int x = m.x;
			int y = m.y;//鼠标按下，玩家确定当前要下的点.
			//判断当前点是否有效，如果有效，按下。如果无效，忽略这次鼠标信息，下次继续判断。
			//无效判断依据，当前点已经有子了。
			pointIsOk(x, y);
		}
	}

}
void show()//构建15*15的棋盘
{
	int const step = 50;
	setfillcolor(RGB(237, 145, 33));
	fillrectangle(0, 0, Width, High);
	setlinecolor(BLACK);
	//if (beSide == -1) //白方下棋
	//{
	//	setfillcolor(WHITE);
	//	fillcircle(800, 50, 25);
	//}
	for (int i = 0; i < 15; i++)
	{
		line(boardgap, boardgap + i * step, boardgap + 14 * step, boardgap + i * step);
		line(boardgap + i * step, boardgap, boardgap + i * step, boardgap + 14 * step);
	}
	for (int i = 0; i < 15; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			if (checkerBoard[i][j] == 1)
			{
				setfillcolor(BLACK);
				fillcircle(boardgap + 50 * i, boardgap + 50 * j, 20);
			}
			else if (checkerBoard[i][j] == -1)
			{
				setfillcolor(WHITE);
				fillcircle(boardgap + 50 * i, boardgap + 50 * j, 20);
			}
		}
	}
	FlushBatchDraw();
}
void resetProblePosition()
{
	for (int i = 0; i < 15; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			if (proBoard[i][j] != 1 && proBoard[i][j] != -1)
			{
				proBoard[i][j] = 0;
			}
		}
	}
}
void proproblePosition(int proBoard1[][15])
{
	for (int i = 0; i < 15; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			if (proBoard1[i][j] == 1 || proBoard1[i][j] == -1) //判断当前点(i,j)是否可行
			{
				for (int k = i - 3; k <= i + 3; k++)
				{
					for (int m = j - 3; m <= j + 3; m++)
					{
						if (k >= 0 && k < 15 && m >= 0 && m < 15 && proBoard1[k][m] == 0)
						{
							if (beSide == 1) //当前是黑的范围
							{
								proBoard1[k][m] = 2;
							}
							else
							{
								proBoard1[k][m] = -2;
							}
						}
					}
				}
			}
		}
	}
}
void setPosition(int proBoard[][15], int checkerBoard1[][15])
{
	for (int i = 0; i < 15; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			proBoard[i][j] = checkerBoard1[i][j];
		}
	}
}
int aPointIsOver(int a, int b, int m[][15], int level)
{

}
int maxNumber(int a, int b, int c)
{

}
int minNumber(int a, int b, int c)
{

}
int evaluate()
{

}
void startup()
{
	steps = 0;
	treeHead = NULL;
	initgraph(Width, High);
	BeginBatchDraw();
}
