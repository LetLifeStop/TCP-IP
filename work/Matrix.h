#include <bits/stdc++.h>
using namespace std;
class Matrix {

public:

    void matrix(int n, int p[])
    {
        memset(m, 0, sizeof(m));
        memset(s, 0, sizeof(s));
        for(int r = 2; r <= n; r++)
        {
            for(int i = 1; i <= n - r + 1; i++)
            {
                int j = r + i - 1;
                m[i][j] = m[i + 1][j] + p[i - 1] * p[i] * p[j];
                s[i][j] = i;
                for(int k = i + 1; k < j; k++)
                {
                    int u = m[i][k] + m[k + 1][j] + p[i - 1] * p[k] * p[j];
                    if(u < m[i][j])
                    {
                        m[i][j] = u;
                        s[i][j] = k;
                    }
                }
            }
        }
        return ;
    }

    void Traceback(int i, int j) {
	    if(i == j)
	    {
	        cout << "A[" << i << "]";
	        return;
	    }
	    cout << "(";
	    Traceback(i , s[i][j]);
	    Traceback(s[i][j] + 1, j);
	    cout << ")";
        return ;
    }
    
    void start() {

        int n;
	    cout << "输入矩阵的个数" << endl;
	    cin >> n;
	    int p[100];
	    cout << "输入矩阵的维度" << endl;
	    for(int k = 0; k <= n; k++)
		    cin >> p[k];
	    matrix(n, p);
        int i, j;
	    cout << "输出矩阵运算次序最小的矩阵" << endl;
        
        for(i = 1; i <= n; i++)
        {
            for(j = 1; j <= n; j++)
            cout << m[i][j] << '\t';
        cout << endl;
        }

        cout << endl;
	    cout << "输出s矩阵" << endl;
        
        for(i = 1; i <= n; i++)
        {
            for(j = 1; j <= n; j++)
               cout << s[i][j] << '\t';
        cout << endl;
        }
	
        cout << "输出矩阵组合次序" << endl;
        Traceback(1 , n);
    	cout << endl;
     }

    private:
    int m[100][100];
    int s[100][100];
};

