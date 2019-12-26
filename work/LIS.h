#include <iostream>
using namespace std;
class LIS {
 
public:

void zcggzxl(int m,int n,char x[],char y[]) {
    int i,j;
    for(i = 1;i <= m; i++)
        c[i][0] = 0;
    for(j = 1; j <= n; j++)
        c[0][j] = 0;
    for(i = 1;i <= m; i++)
    {
        for(j = 1;j <= n; j++)
        {
            if(x[i-1] == y[j-1]){
                c[i][j] = c[i - 1][j - 1] + 1;
                b[i][j] = 1;
            }
            else if(c[i - 1][j]>c[i][j - 1]){
				c[i][j] = c[i - 1][j];
                b[i][j] = 2;
            }
            else{
                c[i][j] = c[i][j - 1];
}
                b[i][j] = 3;
            }
        }
    }
}
void LCS(int i,int j,char x[]){
    if(i == 0||j == 0){
        return;
    }
    if(b[i][j] == 1){
        LCS(i - 1,j - 1, x);
        cout<<x[i - 1];
    }
    else if(b[i][j] == 2)
        LCS(i - 1, j, x);
    else
        LCS(i, j - 1, x);
    void start() {
        int n, m, i, j;
        char x[100], y[100];
	    
        cout << "输入序列的个数" << endl;
        cin >> m >> n;
	    cout << "输入x序列" << endl;
        
        for(int k = 0;k < m; k++)
            cin >> x[k];
        
        cout << "输入y序列" << endl;
        
        for(int l = 0; l < n; l++)
            cin >> y[l];
        
        zcggzxl(m , n, x, y);
    cout << "输出最长序列个数矩阵" << endl;
    for(i = 0;i <= m; i++){
        for(j = 0;j <= n; j++){
            cout << c[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
    cout << "输出b矩阵" << endl;
    for(i = 1;i <= m; i++){
		for(j = 1;j <= n; j++){
            cout << b[i][j] << " ";
        }
        cout << endl;
    }
    cout << "输出最长公共子序列" << endl;
    LCS(m, n, x);
    cout << endl;
    }
private:
    int c[100][100];
    int b[100][100];

};




