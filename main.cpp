//Created by Timothy Russell-Wagner, Karl Kneil, Brenton Lin, Patrick Bradley

#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <algorithm>

#define infinity 99999999

using namespace std;

//Calculates Levenshtein distance between two strings (i.e. charms)
int incantations(string charm1, string charm2)
{
	long length1, length2;

	//Gets longer length, and switches charms if the second string is longer as protection against array sizing errors
	if (charm1.size() > charm2.size())
	{
		length1 = charm1.size();
		length2 = charm2.size();
	}
	else
	{
		length1 = charm2.size();
		length2 = charm1.size();
		string temp = charm2;
		charm2 = charm1;
		charm1 = temp;
	}

	int * distance = new int[length1 + 1];
	for (int i = 0; i <= length1; i++)
	{
		distance[i] = i;
	}

	int i = 0;

	//Levenshtein implementation
	for (string::const_iterator iter1 = charm1.begin(); iter1 != charm1.end(); iter1++, i++)
	{
		distance[0] = i + 1;
		int end = i;
		int k = 0;
		for (string::const_iterator iter2 = charm2.begin(); iter2 != charm2.end(); iter2++, k++)
		{
			int top = distance[k + 1];
			if (*iter1 == *iter2)
			{
				//If chars are the same, distance is the same as previous chars
				distance[k + 1] = end;
			}
			else
			{
				//Otherwise, increment based on deletion, insertion, or substitution operations
				int t = (top < end ? top : end);
				distance[k + 1] = (distance[k] < t ? distance[k] : t) + 1;
			}

			end = top;
		}
	}
	return distance[length2];
}

//Returns minimum levenshtein distance to all adjacent realms
int minVertDist(int lvsDist[], bool flag[], vector<string> charms)
{
	int min = infinity, mIndex = 0;
	long numVert = charms.size();

	for (int v = 0; v < numVert; v++)
	{
		if (flag[v] == false && lvsDist[v] <= min)
		{
			min = lvsDist[v];
			mIndex = v;
		}
	}
	return mIndex;
}

//Ensures that travel is possible within the realms given under the constraints of the problem
//Prints required output, i.e. magi and gems or IMPOSSIBLE
void canTravel(string charm1, string charm2, vector<string> charms, unordered_map<string, vector<int>> realm, int N)
{
	int graph[N][N];
	int tempIncant;

	//Constructing weight array representation of graph based on Levenshtein distances
	//Weight is 0 for any impossible travels
	for (int i = 0; i < N; i++)
	{
		long rowMax = realm.find(charms.at(i))->second.size();

		for (int k = 0; k < N; k++)
		{
			if (i == k)
			{
				graph[i][k] = 0;
			}
			else
			{
				tempIncant = incantations(charms.at(i), charms.at(k));

				if (rowMax < tempIncant)
				{
					tempIncant = 0;
				}

				graph[i][k] = tempIncant;
			}
		}
	}

	long numVert = charms.size();
	int lvsDist[numVert];
	int gems[numVert];
	bool flag[numVert];

	//Array initialization
	for (int i = 0; i < numVert; i++)
	{
		lvsDist[i] = infinity, flag[i] = false;
		gems[i] = 0;
	}

	long src = find(charms.begin(), charms.end(), charm1) - charms.begin();

	lvsDist[src] = 0;

	//Dijkstra's algorithm implementation, gets optimal path in terms of magi and pulls the proper number of gems corresponding to each magus
	for (int count = 0; count < numVert - 1; count++)
	{
		int u = minVertDist(lvsDist, flag, charms);
		flag[u] = true;

		for (int v = 0; v < numVert; v++)
		{
			if (!flag[v] && graph[u][v] && lvsDist[u] != infinity && lvsDist[u] + graph[u][v] < lvsDist[v])
			{
				lvsDist[v] = lvsDist[u] + graph[u][v];
				int weight = graph[u][v];
				vector<int>tempGem = realm.find(charms.at(u))->second;
				int sum = 0;

				for (int i = 0; i < weight; i++)
				{
					sum += tempGem.at(i);
				}

				gems[v] = gems[u] + sum;
			}
		}
	}


	long src1 = find(charms.begin(), charms.end(), charm2) - charms.begin();
	if (gems[src1] == 0 && lvsDist[src1] != 0)
	{
		cout << "IMPOSSIBLE" << endl;
	}
	else
	{
		cout << lvsDist[src1] << " " << gems[src1] << endl;
	}

}

//Finds a longest sequence of gems in a realm
vector<int> seq(vector<int>array)
{
	long length = array.size();
	vector<int> temp1;
	vector<int> seqMax;
	long seqMaxLength = 0;

	for (int i = 0; i < length; i++)
	{
		for (int k = i; k < length; k++)
		{
			if (k == i)
			{
				temp1.push_back(array.at(k));
			}
			else if (temp1.back() < array.at(k))
			{
				temp1.push_back(array.at(k));
			}
		}
		if (temp1.size() > seqMax.size())
		{
			seqMaxLength = temp1.size();
			seqMax = temp1;
		}
		temp1.clear();
	}

	return seqMax;
}

int main()
{
	int N, magiNum;
	string charm, start, end;
	vector<string> charms;
	vector<int> magis;
	unordered_map<string, vector<int>> realm;

	cin >> N;

	for (int i = 0; i < N; i++)
	{
		cin >> charm;
		charms.push_back(charm);
		cin >> magiNum;
		magis.resize(magiNum);

		for (int j = 0; j < magiNum; j++)
		{
			cin >> magis.at(j);
		}

		realm.emplace(charm, seq(magis));
	}

	cin >> start;
	cin >> end;

	canTravel(start, end, charms, realm, N);
	canTravel(end, start, charms, realm, N);

	return 0;
}
