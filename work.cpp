#include <stdio.h>
#include <string>
#include <queue>
#include <stack>
#include <math.h>
#include <cstring>
#include <time.h>
#include <malloc.h>

using namespace std;

//代表方向的枚举变量
enum Direction {
	up = 0,
	down = 1,
	right = 2,
	left = 3
};

//n是行数，m是列数
int n, m;

//记录最佳方法最后一步的指针
struct Step* Best_step = NULL;

//记录已经获得的最高分数
int highest_score;

//用于记录已经出现过的状态，配合哈希表使用
struct Store {
	int state;
	Store* next;
	Step* step;
};

//用于存储哈希表的数据块
Store* store[1000];

//用于记录每一步的走法和前一步的位置
struct Step {
	//步数
	int steps;
	//走该步移动的数字
	int numberofstep;
	//该步完成后的数组状态
	char* state;
	//0的行坐标
	char dx;
	//0的列坐标
	char dy;
	//指向上一步的指针
	struct Step* stepbefore;
	//该步的分数
	int score;
	//该步的评估值
	double value;
	
};

//重载仿函数
struct cmp {
	bool operator()(Step* a, Step* b) {
		return a->value > b->value;
	}
};

//存储路径信息的队列
queue<Step*> Mystep;

//存储已探索未固定信息的队列
priority_queue<Step*,vector<Step*>,cmp > Open;

//打印函数，用于输出最终的最佳结果
void print(Step* Beststep)
{
	stack<Step> MyStep;

	while (Beststep->steps != 0)
	{
		MyStep.push(*Beststep);
		Beststep = Beststep->stepbefore;
	}
	while (!MyStep.empty())
	{
		fprintf(stdout,"%d\n", MyStep.top().numberofstep);
		MyStep.pop();
	}

	fprintf(stdout,"Over\n");
}

//启发函数
double G(char* array)
{
	double value = 0;
	for (int i = 0;i < m * n;i++)
	{
		if (array[i])
		{
			value += sqrt(pow(abs((array[i] - 1) / m - i / m),2)) + sqrt(pow(abs((array[i] - 1) % m - i % m),2));
		}
	}
	return 5.1*value;
}

//用于判断当前状态的实时分数
int judgescore(char* array)
{
	int score = 0;
	for (int i = 0;i < m * n - 1;i++)
		if (array[i] == i + 1)
			score += 100;
	return score;
}

//判断一个方向是否可以通行
bool Can_Move(Direction dir, int x, int y)
{
	switch (dir)
	{
	case up:
	{
		if (x > 0)
			return true;
		else
			return false;
		break;
	}
	case down:
	{
		if (x < n - 1)
			return true;
		else
			return false;
		break;
	}
	case right:
	{
		if (y < m - 1)
			return true;
		else
			return false;
		break;
	}
	case left:
	{
		if (y > 0)
			return true;
		else
			return false;
		break;
	}
	default:
		return false;
	}
}

//对于每个状态的hash函数
unsigned int Hash(char* array)
{
	int hash, i;
	for (hash = m * n, i = 0; i < m * n; ++i)
		hash = (hash << 4) ^ (hash >> 28) ^ array[i];
	return hash;
}


//搜寻该状态是否出现过（用于A*）
bool search(unsigned int hashnumber,bool add)
{
	int pos = hashnumber % 1000;
	Store** ptr = &(store[pos]);
	while (*ptr != NULL)
	{
		//若在已有状态中发现该情况已经存在过，返回false
		if ((*ptr)->state == hashnumber)
		{
			/*if ((*ptr)->step->value > step->value)
			{
				Open.push(step);
			}*/
			return false;
		}

		ptr = &((*ptr)->next);
	}
	//若未发现该情况存在，则将该情况存入该数据块中，并返回真值
	if (add)
	{
		*ptr = new Store;
		(*ptr)->next = NULL;
		(*ptr)->state = hashnumber;
	}
	return true;
}


//移动函数，该处移动指的是移动空格，即对“0”进行操作，当成功移动时返回true，否则返回false（用于A*算法）
bool move(char state[], Direction dir, Step* step)
{
	int dx = step->dx;
	int dy = step->dy;
	char temp;

	if (Can_Move(dir, dx, dy))
	{
		switch (dir)
		{
		case up:
		{
			if (state[(dx - 1) * m + dy] == (dx - 1) * m + dy + 1)
			{
				step->score -= 101;


			}
			else if (state[(dx - 1) * m + dy] == dx * m + dy + 1)
			{
				step->score += 99;

			}
			else
			{
				step->score -= 1;
				
			}

			state[dx * m + dy] = state[(dx - 1) * m + dy];
			temp = state[dx * m + dy];
			dx -= 1;
			state[dx * m + dy] = 0;
			step->dx = dx;
			if (search(Hash(state),false))
			{
				step->steps++;
				memcpy(step->state, state, m * n * sizeof(char));
				step->numberofstep = temp;
				step->value = G(state) + step->steps;
				return true;
			}
			return false;
		}
		case down:
		{
			if (state[(dx + 1) * m + dy] == (dx + 1) * m + dy + 1)
			{
				step->score -= 101;

			}
			else if (state[(dx + 1) * m + dy] == dx * m + dy + 1)
			{
				step->score += 99;

			}
			else
			{
				step->score -= 1;

			}
			state[dx * m + dy] = state[(dx + 1) * m + dy];
			temp = state[dx * m + dy];
			dx += 1;
			state[dx * m + dy] = 0;
			step->dx = dx;
			if (search(Hash(state), false))
			{
				step->steps++;
				memcpy(step->state, state, m * n * sizeof(char));
				step->numberofstep = temp;
				step->value = G(state) + step->steps;
				return true;
			}
			else
				return false;
		}

		case left:
		{
			if (state[dx * m + dy - 1] == dx * m + dy)
			{
				step->score -= 101;

			}
			else if (state[dx * m + dy - 1] == dx * m + dy + 1)
			{
				step->score += 99;

			}
			else
			{
				step->score -= 1;

			}
			state[dx * m + dy] = state[dx * m + dy - 1];
			temp = state[dx * m + dy];
			dy -= 1;
			step->dy = dy;
			state[dx * m + dy] = 0;
			if (search(Hash(state), false))
			{
				step->steps++;
				memcpy(step->state, state, m * n * sizeof(char));
				step->numberofstep = temp;
				step->value = G(state) + step->steps;
				return true;
			}
			else
			{
				return false;
			}
		}
		case right:
		{
			if (state[dx * m + dy + 1] == dx * m + dy + 2)
			{
				step->score -= 101;

			}
			else if (state[dx * m + dy + 1] == dx * m + dy + 1)
			{
				step->score += 99;

			}
			else
			{
				step->score -= 1;

			}
			state[dx * m + dy] = state[dx * m + dy + 1];
			temp = state[dx * m + dy];
			dy += 1;
			state[dx * m + dy] = 0;
			step->dy = dy;

			if (search(Hash(state), false))
			{
				step->steps++;
				memcpy(step->state, state, m * n * sizeof(char));
				step->numberofstep = temp;
				step->value = G(state) + step->steps;
				return true;
			}
			else
				return false;
		}
		default:
			throw("Overflow");
			return false;
		}
	}
	else
		return false;

}

//AStar寻路算法
void Findway()
{
	clock_t start = clock();

	while (!Open.empty())
	{
		Step* temp = Open.top();
		search(Hash(temp->state), true);
		Open.pop();
		Step* MyStep[4];
		char* state = (char*)malloc(m * n * sizeof(char));
		for (int i = 0;i < 4;i++)
		{			
			MyStep[i] = new Step;
			memcpy(state, temp->state, m * n * sizeof(char));
			MyStep[i]->steps = temp->steps;
			MyStep[i]->dx = temp->dx;
			MyStep[i]->dy = temp->dy;
			MyStep[i]->stepbefore = temp;
			MyStep[i]->state = new char[m * n];
			MyStep[i]->score = temp->score;
			if (clock() - start >= 1000)
			{
					return;
			}
			if (move(state, (Direction)i, MyStep[i]))
			{
				if (clock() - start >= 1000)
				{
					return;
				}


				int temp_num = MyStep[i]->score;
				
				if (G(MyStep[i]->state) == 0)
				{
				
					Best_step = MyStep[i];
					return;
				}
				if (temp_num > highest_score)
				{
					highest_score = temp_num;
					Best_step = MyStep[i];
				}
				if (MyStep[i]->steps > (m * n - 1) * 100)
				{
					return;
				}
				Open.push(MyStep[i]);
			}

			else
			{
				delete(MyStep[i]);
			}

		}
		free(state);
	}
}


int main()
{
	//输入行数和列数
	scanf("%d %d", &n, &m);

	char* array = new char[m * n];
	Step* MyStep = new Step;
	for (int i = 0, temp;i < m * n;i++)
	{
		scanf("%d", &temp);
		if (!temp)
		{
			MyStep->dx = i / m;
			MyStep->dy = i % m;
		}
		array[i] = temp;
	}

	//将最高分初始化为当前状态分数
	highest_score = judgescore(array);

	MyStep->stepbefore = NULL;
	MyStep->steps = 0;
	MyStep->state = new char[m * n];
	MyStep->score = highest_score;
	MyStep->value = G(array);

	Best_step = MyStep;
	memcpy(MyStep->state, array, m * n * sizeof(char));


		search(Hash(array), false);

		Open.push(MyStep);

		for (int i = 0;i < 1000;i++)
		{
			store[i] = NULL;
		}

		Findway();

	print(Best_step);

	return 0;
}