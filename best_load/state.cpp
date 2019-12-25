#include <bits/stdc++.h>

using namespace std;

const int maxn = 2e5 + 100;

int sto[maxn];

bool judge(int state, int n, int c1, int c2) {

    int i;
    int sum1 = 0, sum2 = 0;

    for(i = 1; i <= n; i++) {
        if(state & (1 << (i - 1)))
            sum1 += sto[i];
        else 
            sum2 += sto[i];
    }
    return (sum1 <= c1 && sum2 <= c2);
}

int main() {

    memset(sto, 0, sizeof(sto));

    int maxstate, i, c1, c2, n, ans = -1;

     printf("第一艘船的承重量");
     scanf("%d", &c1);                                                                                                                                                                                          
     printf("第二艘船的承重量");
     scanf("%d", &c2);
  
     printf("请输入货物的数量");
     scanf("%d", &n);
  
     for(i = 1; i <= n; i++) {
         printf("第%d件货物的重量为", i);
         scanf("%d", &sto[i]);
    }
    
    maxstate = (1<<n) - 1;
    
    for(i = 0; i <= maxstate; i++) {
        if(judge(i, n, c1, c2)) {
            ans = i;
            break;
        }
    }

    for(i = 1; i <= n; i++) {
        if(ans & (1<<(i - 1)))
            printf("第%d件物品在第1个船上\n", i);
        else 
            printf("第%d件物品在第2个船上\n", i);
    }

    return 0;
}
