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
	long long int alpha;
	long long int beta;
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
void proproblePosition(int proBoard1[][15]);
void resetProblePosition();
void show();
void TLGO();
int aPointIsOver(int, int, int[][15], int);
void updateWithInput();
int pixpoi(int);
int evaluate(int[][15], int, int);
int evaluate_white(int[][15], int, int, int);
int pointIsOk(int, int);
void buildGameTree(pTree, int);
void freeTree(pTree);
long long int maxNumber(long long int, long long  int, long long  int);
long long int minNumber(long long int, long long int, long long  int);
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
				treeHead = (pTree)malloc(sizeof(struct treeNode));
				treeHead->alpha = MINV;
				treeHead->beta = MAXV;
				setPosition(treeHead->board, checkerBoard);
				treeHead->chileHead = NULL;
				buildGameTree(treeHead, 0);
				//从treeHead的若干个子节点中选出一个最合适的
				pNode current, temp;
				current = treeHead->chileHead;
				long long int xx = current->treeChild->alpha;
				temp = current;
				//closegraph();
				while (current->next != NULL)
				{
					//printf("(%d,%d)   %d     %d\n", current->treeChild->x1, current->treeChild->y1, current->treeChild->alpha,current->treeChild->beta);
					if (xx <= current->next->treeChild->alpha)
					{
						temp = current->next;
						xx = current->next->treeChild->alpha;

					}
					current = current->next;
				}
				int x = temp->treeChild->x1;
				int y = temp->treeChild->y1;
				checkerBoard[x][y] = 1;
				beSide = -beSide;

			}
		}
		else
		{
			updateWithInput();

		}

	}
	return 0;
}
void buildGameTree(pTree father, int level)
{
	int proBoard1[15][15] = { 0 };
	level++;
	setPosition(proBoard1, father->board);
	proproblePosition(proBoard1);

	for (int i = 0; i < 15; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			if (proBoard1[i][j] == 2)
			{
				setfillcolor(RED);
				fillcircle(boardgap + 50 * i, boardgap + 50 * j, 20);
			}
		}
	}
	FlushBatchDraw();
	Sleep(10);

	pTree current = NULL;
	pNode pre, cur, temp;
	pre = cur = NULL;
	if (level < cen)
	{
		//应注意到，这个for循环中的所有current节点，应该存到father节点的链表之中!
		for (int i = 0; i < 15; i++)     //开始遍历所有可疑点
		{
			for (int j = 0; j < 15; j++)
			{
				if (proBoard1[i][j] == 2) //点(i,j)是当前的一个可疑点
				{
					current = (pTree)malloc(sizeof(struct treeNode));
					current->father = father;
					current->alpha = father->alpha; current->beta = father->beta;
					current->level = level;
					setPosition(current->board, father->board);
					current->x1 = i; current->y1 = j;
					current->score = 0;

					if (level % 2 == 0)
					{
						current->board[i][j] = -1;
						setfillcolor(WHITE);
						fillcircle(boardgap + 50 * i, boardgap + 50 * j, 20);
						FlushBatchDraw();
						Sleep(10);
					}
					else
					{
						current->board[i][j] = 1;
						setfillcolor(BLACK);
						fillcircle(boardgap + 50 * i, boardgap + 50 * j, 20);
						FlushBatchDraw();
						Sleep(10);
					}
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
				//运行到此处，当前可疑点已经被赋值
				temp = father->chileHead;
				while (temp != NULL)
				{
					if (level % 2 == 0) //在min层
					{
						father->beta = minNumber(father->beta, temp->treeChild->alpha, temp->treeChild->beta);
					}
					else //在max层
					{
						father->alpha = maxNumber(father->alpha, temp->treeChild->alpha, temp->treeChild->beta);
					}
					if (father->alpha > father->beta)
					{
						break;
					}
					temp = temp->next;
				}

			}
		}
		//运行到此处，father所有可疑点的子节点的子节点都已经处理完毕，但没有处理father与father自己的子节点的关系；
	}
	else if (level >= cen)
	{
		for (int i = 0; i < 15; i++)
		{
			for (int j = 0; j < 15; j++)
			{
				if (proBoard1[i][j] == 2)
				{
					//在最后一层没有必要对father在建一个链表，每一个节点与father的值比较，满足条件就更新即可
					int score = evaluate(father->board, i, j);
					father->board[i][j] = 0;
					if (score < father->beta)
					{
						father->beta = score;
					}
				}
			}
		}
		return;
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
					/*if (beSide == 1)
					{
						checkerBoard[i][j] = 1;
					}
					else {*/
					checkerBoard[i][j] = -1;
					/*}*/
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
							//if (beSide == 1) //当前是黑的范围
							//{
							//	proBoard1[k][m] = 2;
							//}
							//else
							//{
							//	proBoard1[k][m] = -2;
							//}
							proBoard1[k][m] = 2;
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
int aPointIsOver(int x, int y, int m[][15], int level)
{
	int side;
	if (level % 2 == 0)
	{
		side = -1;
	}
	else
	{
		side = 1;
	}
	m[x][y] = side;

	for (int i = -4; i <= 0; i++)
	{
		int j = x + i;

		if (j >= 0 && j + 4 < 15)
		{
			if (m[j][y] == side && m[j + 1][y] == side && m[j + 2][y] == side && m[j + 3][y] == side && m[j + 4][y] == side)
				return 1;
		}
	}
	//竖
	for (int i = -4; i <= 0; i++)
	{
		int j = y + i;

		if (j >= 0 && j + 4 < 15)
		{
			if (m[x][j] == side && m[x][j + 1] == side && m[x][j + 2] == side && m[x][j + 3] == side && m[x][j + 4] == side)
				return 1;
		}
	}
	//主对角线
	for (int i = -4; i <= 0; i++)
	{
		int q = x + i;
		int n = y + i;
		if (q >= 0 && q + 4 < 15 && n >= 0 && n + 4 < 15)
		{
			if (m[q][n] == side && m[q + 1][n + 1] == side && m[q + 2][n + 2] == side && m[q + 3][n + 3] == side && m[q + 4][n + 4] == side)
				return 1;
		}
	}
	//副对角线
	for (int i = -4, j = 4; i <= 0; i++, j--)
	{
		int q = x + i;
		int n = y + j;
		if (q >= 0 && q + 4 < 15 && n < 15 && n - 4 >= 0)
		{
			if (m[q][n] == side && m[q + 1][n - 1] == side && m[q + 2][n - 2] == side && m[q + 3][n - 3] == side && m[q + 4][n - 4] == side)
				return 1;
		}
	}

	return 0;
}
long long int maxNumber(long long int a, long long  int b, long long  int c)
{
	long long int max = a;
	if (b > max) max = b;
	if (c > max) max = c;
	return max;
}
long long int minNumber(long long int a, long long int b, long long int c)
{
	long long int min = a;
	if (b < min) min = b;
	if (c < min) min = c;
	return min;
}
int evaluate(int m[][15], int x, int y) //当前棋盘为m，当前点为(x,y)
{
	int score = 0, a, b;

	a = evaluate_white(m, x, y, -1);
	b = evaluate_white(m, x, y, 1);
	score = a + b;
	return score;
}
int evaluate_white(int m[][15], int x, int y, int side)
{
	int score = 0;
	int state1 = 0, state2 = 0, state3 = 0, state4 = 0;//1冲四，2活三，3眠三，4活二
	m[x][y] = side;
	//先判断连五
	//横
	for (int i = -4; i <= 0; i++)
	{
		int j = x + i;

		if (j >= 0 && j + 4 < 15)
		{
			if (m[j][y] == side && m[j + 1][y] == side && m[j + 2][y] == side && m[j + 3][y] == side && m[j + 4][y] == side)
				return 10000000;
		}
	}
	//竖
	for (int i = -4; i <= 0; i++)
	{
		int j = y + i;

		if (j >= 0 && j + 4 < 15)
		{
			if (m[x][j] == side && m[x][j + 1] == side && m[x][j + 2] == side && m[x][j + 3] == side && m[x][j + 4] == side)
				return 10000000;
		}
	}
	//主对角线
	for (int i = -4; i <= 0; i++)
	{
		int q = x + i;
		int n = y + i;
		if (q >= 0 && q + 4 < 15 && n >= 0 && n + 4 < 15)
		{
			if (m[q][n] == side && m[q + 1][n + 1] == side && m[q + 2][n + 2] == side && m[q + 3][n + 3] == side && m[q + 4][n + 4] == side)
				return 10000000;
		}
	}
	//副对角线
	for (int i = -4, j = 4; i <= 0; i++, j--)
	{
		int q = x + i;
		int n = y + j;
		if (q >= 0 && q + 4 < 15 && n < 15 && n - 4 >= 0)
		{
			if (m[q][n] == side && m[q + 1][n - 1] == side && m[q + 2][n - 2] == side && m[q + 3][n - 3] == side && m[q + 4][n - 4] == side)
				return 10000000;
		}
	}

	//再判断活4
	//横
	for (int i = -3; i <= 0; i++)
	{
		int j = x + i;

		if (j >= 0 && j + 3 < 15)
		{
			if (m[j][y] == side && m[j + 1][y] == side && m[j + 2][y] == side && m[j + 3][y] == side)
			{
				//活四情况
				if (j - 1 >= 0 && m[j - 1][y] == 0 && j + 4 < 15 && m[j + 4][y] == 0)
				{
					score += 5000000;
				}
				//冲四情况
				else if ((j - 1 >= 0 && m[j - 1][y] == 0 && j + 4 < 15 && m[j + 4][y] == -side) ||
					(j - 1 >= 0 && m[j - 1][y] == -side && j + 4 < 15 && m[j + 4][y] == 0))
				{
					score += 10000;
					state1 = 1;//出现冲4
				}
			}
		}
	}
	//竖
	for (int i = -3; i <= 0; i++)
	{
		int j = x + i;

		if (j >= 0 && j + 3 < 15)
		{
			if (m[x][j] == side && m[x][j + 1] == side && m[x][j + 2] == side && m[x][j + 3] == side)
			{
				//活四情况
				if (j - 1 >= 0 && m[x][j - 1] == 0 && j + 4 < 15 && m[x][j + 4] == 0)
				{
					score += 5000000;
				}
				//冲四情况
				else if ((j - 1 >= 0 && m[x][j - 1] == 0 && j + 4 < 15 && m[x][j + 4] == -side) ||
					(j - 1 >= 0 && m[x][j - 1] == -side && j + 4 < 15 && m[x][j + 4] == 0))
				{
					if (state1 == 1)//已出现冲4
					{
						score += 1000000;
					}
					else
					{
						score += 10000;
						state1 = 1;
					}
				}
			}
		}
	}
	//主对角线
	for (int i = -3; i <= 0; i++)
	{
		int q = x + i;
		int n = y + i;
		if (q >= 0 && q + 3 < 15 && n >= 0 && n + 3 < 15)
		{
			if (m[q][n] == side && m[q + 1][n + 1] == side && m[q + 2][n + 2] == side && m[q + 3][n + 3] == side)
			{
				if (q - 1 >= 0 && n - 1 >= 0 && m[q - 1][n - 1] == 0 && q + 4 < 15 && n + 4 < 15 && m[q + 4][n + 4] == 0)
				{
					score += 5000000;
				}
				//冲4情况
				else if ((q - 1 >= 0 && n - 1 >= 0 && m[q - 1][n - 1] == 0 && q + 4 < 15 && n + 4 < 15 && m[q + 4][n + 4] == -side) ||
					(q - 1 >= 0 && n - 1 >= 0 && m[q - 1][n - 1] == -side && q + 4 < 15 && n + 4 < 15 && m[q + 4][n + 4] == 0))
				{
					if (state1 == 1)//已出现冲4
					{
						score += 1000000;
					}
					else
					{
						score += 10000;
						state1 = 1;
					}
				}

			}
		}
	}
	//副对角线
	for (int i = -3, j = 3; i <= 0; i++, j--)
	{
		int q = x + i;
		int n = y + j;
		if (q >= 0 && q + 3 < 15 && n < 15 && n - 3 >= 0)
		{
			if (m[q][n] == side && m[q + 1][n - 1] == side && m[q + 2][n - 2] == side && m[q + 3][n - 3] == side)
			{
				if (q - 1 >= 0 && n + 1 < 15 && m[q - 1][n + 1] == 0 && q + 4 < 15 && n - 4 >= 0 && m[q + 4][n - 4] == 0)
				{
					score += 5000000;
				}
				//冲4情况
				else if ((q - 1 >= 0 && n + 1 < 15 && m[q - 1][n + 1] == 0 && q + 4 < 15 && n - 4 >= 0 && m[q + 4][n - 4] == -side) ||
					(q - 1 >= 0 && n + 1 < 15 && m[q - 1][n + 1] == -side && q + 4 < 15 && n - 4 >= 0 && m[q + 4][n - 4] == 0))
				{
					if (state1 == 1)//已出现冲4
					{
						score += 1000000; //6个
					}
					else
					{
						score += 10000;
						state1 = 1;
					}
				}

			}
		}
	}


	//3的情况
	//先看连三
	//横
	for (int i = -2; i <= 0; i++)
	{
		int j = x + i;

		if (j >= 0 && j + 2 < 15)
		{
			if (m[j][y] == side && m[j + 1][y] == side && m[j + 2][y] == side)
			{
				//活三情况
				if (j - 1 >= 0 && m[j - 1][y] == 0 && j + 3 < 15 && m[j + 3][y] == 0)
				{
					state2 = 1;
					if (state1 == 1)//冲四活三
					{
						score += 1000000;
					}
					else//仅活三
					{
						score += 50000;
					}
				}
				//眠三情况
				else if ((j - 1 >= 0 && m[j - 1][y] == 0 && j + 3 < 15 && m[j + 3][y] == -side) ||
					(j - 1 >= 0 && m[j - 1][y] == -side && j + 3 < 15 && m[j + 3][y] == 0))
				{
					score += 1000;
				}
			}
		}
	}
	//竖
	for (int i = -2; i <= 0; i++)
	{
		int j = x + i;

		if (j >= 0 && j + 2 < 15)
		{
			if (m[x][j] == side && m[x][j + 1] == side && m[x][j + 2] == side)
			{
				//活三情况
				if (j - 1 >= 0 && m[x][j - 1] == 0 && j + 3 < 15 && m[x][j + 3] == 0)
				{
					if (state2 == 1)//双三
					{
						score += 900000;

					}
					else if (state1 == 1)//冲四活三
					{
						score += 1000000;
					}
					else
					{
						score += 50000;
					}
					state2 = 1;
				}
				//眠三情况
				else if ((j - 1 >= 0 && m[x][j - 1] == 0 && j + 3 < 15 && m[x][j + 3] == -side) ||
					(j - 1 >= 0 && m[x][j - 1] == -side && j + 3 < 15 && m[x][j + 3] == 0))
				{
					score += 1000;
				}
			}
		}
	}
	//主对角线
	for (int i = -2; i <= 0; i++)
	{
		int q = x + i;
		int n = y + i;
		if (q >= 0 && q + 2 < 15 && n >= 0 && n + 2 < 15)
		{
			if (m[q][n] == side && m[q + 1][n + 1] == side && m[q + 2][n + 2] == side)
			{
				//活三
				if (q - 1 >= 0 && n - 1 >= 0 && m[q - 1][n - 1] == 0 && q + 3 < 15 && n + 3 < 15 && m[q + 3][n + 3] == 0)
				{
					if (state2 == 1)//双三
					{
						score += 900000;

					}
					else if (state1 == 1)//冲四活三
					{
						score += 1000000;
					}
					else
					{
						score += 50000;
					}
					state2 = 1;
				}
				//面三情况
				else if ((q - 1 >= 0 && n - 1 >= 0 && m[q - 1][n - 1] == 0 && q + 3 < 15 && n + 3 < 15 && m[q + 3][n + 3] == -side) ||
					(q - 1 >= 0 && n - 1 >= 0 && m[q - 1][n - 1] == -side && q + 3 < 15 && n + 3 < 15 && m[q + 3][n + 3] == 0))
				{
					score += 1000;
				}

			}
		}
	}
	//副对角线
	for (int i = -2, j = 2; i <= 0; i++, j--)
	{
		int q = x + i;
		int n = y + j;
		if (q >= 0 && q + 2 < 15 && n < 15 && n - 2 >= 0)
		{
			if (m[q][n] == side && m[q + 1][n - 1] == side && m[q + 2][n - 2] == side)
			{
				if (q - 1 >= 0 && n + 1 < 15 && m[q - 1][n + 1] == 0 && q + 3 < 15 && n - 3 >= 0 && m[q + 3][n - 3] == 0)
				{
					if (state2 == 1)//双三
					{
						score += 900000;

					}
					else if (state1 == 1)//冲四活三
					{
						score += 1000000;
					}
					else
					{
						score += 50000;
					}
					state2 = 1;
				}
				//眠三情况
				else if ((q - 1 >= 0 && n + 1 < 15 && m[q - 1][n + 1] == 0 && q + 3 < 15 && n - 3 >= 0 && m[q + 3][n - 3] == -side) ||
					(q - 1 >= 0 && n + 1 < 15 && m[q - 1][n + 1] == -side && q + 3 < 15 && n - 3 >= 0 && m[q + 3][n - 3] == 0))
				{
					score += 1000;
				}

			}
		}
	}
	//再来判断跳活三的情况
	//横
	for (int i = -3; i <= 0; i++)
	{
		int j = x + i;
		if (j > 0 && j + 3 < 14) //元组再棋盘范围内
		{
			if (m[j][y] == side && m[j + 3][y] == side) //最左与最右是同色棋子
			{
				if ((m[j + 1][y] == 0 && m[j + 2][y] == side) || (m[j + 1][y] == side && m[j + 2][y] == 0)) //存在跳三原型，判断是活还是眠
				{
					if (m[j - 1][y] == 0 && m[j + 4][y] == 0) //活
					{
						if (state2 == 1)//双三
						{
							score += 500000;

						}
						else if (state1 == 1)//冲四活三
						{
							score += 1000000;
						}
						else
						{
							score += 50000;
						}
						state2 = 1;
					}
					else if ((m[j - 1][y] == 0 && m[j + 4][y] == -side) || (m[j - 1][y] == -side && m[j + 4][y] == 0))
					{
						score += 1000;
					}
				}
			}
		}
	}
	//竖
	for (int i = -3; i <= 0; i++)
	{
		int j = x + i;
		if (j > 0 && j + 3 < 14) //元组再棋盘范围内
		{
			if (m[x][j] == side && m[x][j + 3] == side) //最左与最右是同色棋子
			{
				if ((m[x][j + 1] == 0 && m[x][j + 2] == side) || (m[x][j + 1] == side && m[x][j + 2] == 0)) //存在跳三原型，判断是活还是眠
				{
					if (m[x][j - 1] == 0 && m[x][j + 4] == 0) //活
					{
						if (state2 == 1)//双三
						{
							score += 500000;

						}
						else if (state1 == 1)//冲四活三
						{
							score += 1000000;
						}
						else
						{
							score += 50000;
						}
						state2 = 1;
					}
					else if ((m[x][j - 1] == 0 && m[x][j + 4] == -side) || (m[x][j - 1] == -side && m[x][j + 4] == 0))
					{
						score += 1000;
					}
				}
			}
		}
	}
	//主对角线
	for (int i = -3, j = -3; i <= 0; i++, j++)
	{
		int q = x + i;
		int n = y + i;
		if (q > 0 && q + 3 < 14 && n > 0 && n + 3 < 14) //元组再棋盘范围内
		{
			if (m[q][n] == side && m[q + 3][n + 3] == side) //最左与最右是同色棋子
			{
				if ((m[q + 1][n + 1] == 0 && m[q + 2][n + 2] == side) || (m[q + 1][n + 1] == side && m[q + 2][n + 2] == 0)) //存在跳三原型，判断是活还是眠
				{
					if (m[q - 1][n - 1] == 0 && m[q + 4][n + 4] == 0) //活
					{
						if (state2 == 1)//双三
						{
							score += 500000;

						}
						else if (state1 == 1)//冲四活三
						{
							score += 1000000;
						}
						else
						{
							score += 50000;
						}
						state2 = 1;
					}
					else if ((m[q - 1][n - 1] == 0 && m[q + 4][n + 4] == -side) || (m[q - 1][n - 1] == -side && m[q + 4][n + 4] == 0))
					{
						score += 1000;
					}
				}
			}
		}
	}
	//副对角线
	for (int i = -3, j = 3; i <= 0; i++, j--)
	{
		int q = x + i;
		int n = y + i;
		if (q > 0 && q + 3 < 14 && n < 14 && n - 3 >0)//元组再棋盘范围内
		{
			if (m[q][n] == side && m[q + 3][n - 3] == side) //最左与最右是同色棋子
			{
				if ((m[q + 1][n - 1] == 0 && m[q + 2][n - 2] == side) || (m[q + 1][n - 1] == side && m[q + 2][n - 2] == 0)) //存在跳三原型，判断是活还是眠
				{
					if (m[q - 1][n + 1] == 0 && m[q + 4][n - 4] == 0) //活
					{
						if (state2 == 1)//双三
						{
							score += 500000;

						}
						else if (state1 == 1)//冲四活三
						{
							score += 1000000;
						}
						else
						{
							score += 50000;
						}
						state2 = 1;
					}
					else if ((m[q - 1][n + 1] == 0 && m[q + 4][n - 4] == -side) || (m[q - 1][n + 1] == -side && m[q + 4][n - 4] == 0))
					{
						score += 1000;
					}
				}
			}
		}
	}
	/*----------------------------再来分析二的情况-------------------------------------*/
	//连二
	//横
	for (int i = -1; i <= 0; i++)
	{
		int j = x + i;

		if (j >= 0 && j + 2 < 15)
		{
			if (m[j][y] == side && m[j + 1][y] == side)
			{
				//活二情况
				if (j - 1 >= 0 && m[j - 1][y] == 0 && j + 2 < 15 && m[j + 2][y] == 0)
				{
					score += 100;
				}
				//眠二情况
				else if ((j - 1 >= 0 && m[j - 1][y] == 0 && j + 2 < 15 && m[j + 2][y] == -side) ||
					(j - 1 >= 0 && m[j - 1][y] == -side && j + 2 < 15 && m[j + 2][y] == 0))
				{
					score += 10;
				}
			}
		}
	}
	//竖
	for (int i = -1; i <= 0; i++)
	{
		int j = x + i;

		if (j >= 0 && j + 1 < 15)
		{
			if (m[x][j] == side && m[x][j + 1] == side)
			{
				//活二情况
				if (j - 1 >= 0 && m[x][j - 1] == 0 && j + 2 < 15 && m[x][j + 2] == 0)
				{
					score += 100;
				}
				//眠三情况
				else if ((j - 1 >= 0 && m[x][j - 1] == 0 && j + 2 < 15 && m[x][j + 2] == -side) ||
					(j - 1 >= 0 && m[x][j - 1] == -side && j + 2 < 15 && m[x][j + 2] == 0))
				{
					score += 10;
				}
			}
		}
	}
	//主对角线
	for (int i = -1; i <= 0; i++)
	{
		int q = x + i;
		int n = y + i;
		if (q >= 0 && q + 1 < 15 && n >= 0 && n + 1 < 15)
		{
			if (m[q][n] == side && m[q + 1][n + 1] == side)
			{
				//活二
				if (q - 1 >= 0 && n - 1 >= 0 && m[q - 1][n - 1] == 0 && q + 2 < 15 && n + 2 < 15 && m[q + 2][n + 2] == 0)
				{
					score += 100;
				}
				//面三情况
				else if ((q - 1 >= 0 && n - 1 >= 0 && m[q - 1][n - 1] == 0 && q + 2 < 15 && n + 2 < 15 && m[q + 2][n + 2] == -side) ||
					(q - 1 >= 0 && n - 1 >= 0 && m[q - 1][n - 1] == -side && q + 3 < 15 && n + 2 < 15 && m[q + 2][n + 2] == 0))
				{
					score += 10;
				}

			}
		}
	}
	//副对角线
	for (int i = -3, j = 3; i <= 0; i++, j--)
	{
		int q = x + i;
		int n = y + j;
		if (q >= 0 && q + 2 < 15 && n < 15 && n - 1 >= 0)
		{
			if (m[q][n] == side && m[q + 1][n - 1] == side && m[q + 1][n - 1] == side)
			{
				if (q - 1 >= 0 && n + 1 < 15 && m[q - 1][n + 1] == 0 && q + 2 < 15 && n - 2 >= 0 && m[q + 2][n - 2] == 0)
				{
					score += 100;
				}
				//眠三情况
				else if ((q - 1 >= 0 && n + 1 < 15 && m[q - 1][n + 1] == 0 && q + 2 < 15 && n - 2 >= 0 && m[q + 2][n - 2] == -side) ||
					(q - 1 >= 0 && n + 1 < 15 && m[q - 1][n + 1] == -side && q + 2 < 15 && n - 2 >= 0 && m[q + 2][n - 2] == 0))
				{
					score += 10;
				}

			}
		}
	}
	return score;
}
//应先释放链表内的树节点，然后再释放这个链表节点
void freeTree(pTree head)
{
	if (head->chileHead != NULL)
	{
		freeTree(head->chileHead->treeChild);
		pNode cur, pre;
		cur = head->chileHead;
		//
	}
	else
	{
		free(head);
	}
}
void startup()
{
	steps = 0;
	treeHead = NULL;
	initgraph(Width, High);
	BeginBatchDraw();
}
