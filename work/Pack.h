#include <iostream>
using namespace std;
class Pack {
private:
    int max(int a,int b)
    {
    if(a > b)
        return a;
    else
        return b;
    }
    void knapsack(int w[] ,int v[] ,int n, int c)
    {
        memset(m, 0, sizeof(m));
        
        for(int j = 0; j <= c; j++)
        {
            if(w[ n - 1] <= j)
                m[n][j] = v[n - 1];
            else
                m[n][j] = 0;
        }

        for(int i = n - 1; i > 1; i--)
        {
            for(int j = 0;j <= c; j++)
            {
            if(w[i - 1] > j)
                m[i][j] = m[i + 1][j];
            else
                m[i][j]=max(m[i + 1][j], m[i + 1][j - w[i - 1]] + v[i - 1]);
            }
        }

        m[1][c] = m[2][c];
        
        if(w[0] <= c)
        {
        m[1][c]=max(m[2][c], m[2][c - w[0]] + v[0]);
        }
    }   

    void Traceback(int w[], int n, int c, int x[])
    {
        for(int i = 1; i < n; i++)
        {
            if(m[i][c] == m[i + 1][c])
                x[i]=0;
            else
            {
                x[i] = 1;
                c = c - w[i - 1];
            }
        }
        if(m[n][c] != 0)
            x[n]=1;
        else
            x[n] = 0;
    }

    void start() {

	int n;
	cout << "输入物品的个数" << endl;
	cin >> n;

	int x[100];
	int w[100];
	int v[100];
	int c;
	cout << "输入背包的容量" << endl;
	cin >> c;
    int l;
	cout << "输入物品的质量" << endl;
	for(l = 0; l < n; l++)
		cin >> w[l];
	cout << "输入物品的价值" << endl;
	
    for(l = 0; l<n; l++)
		cin >> v[l];
    
    knapsack(w, v, n, c);
    int i, j;
    
    for(i = 1; i <= n; i++)
    {
        for(j = 0; j <= c; j++)
            cout << m[i][j] << '\t';
        cout << endl;
    }

    cout << endl;
    Traceback(w, n, c, x);
    
    for(int k = 1; k <= n; k++)
        cout << x[k] << " ";
    }
    }
    private:
    int m[100][100];
};



