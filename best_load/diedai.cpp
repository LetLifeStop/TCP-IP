#include <bits/stdc++.h>
using namespace std;

int ret, i, c1, c2, n, tot, current_weight;

int sto[100 + 10], ans[100 + 10], judge[100 + 10];

void Search(int pos) {
    
    if(pos > n) {
        if(current_weight > ret) {
            for(i = 1; i <= n; i++) {
                ans[i] = judge[i];
            }
            ret = current_weight;
        }
        return ;
    }

    tot -= sto[pos];
    if(current_weight + sto[pos] <= c1) {
        current_weight += sto[pos];
        judge[pos] = 1;
        Search(pos + 1);
        current_weight -= sto[pos];
        judge[pos] = 0;
    }

    if(current_weight + tot > ret) {
        // 当这个节点下存在最优解的时候去搜索他的右子树是否存在最优解（此时左子树已经便利过，所以不需要再对左子树进行遍历）
        judge[pos] = 0;
        Search(pos + 1);
    }

    tot += sto[pos];
}

void init() {
    ret = 0;
    tot = 0;
    current_weight = 0;
    memset(sto, 0, sizeof(sto));
    memset(ans, 0, sizeof(ans));
    memset(judge, 0, sizeof(judge));
}

void Print_Ans() {
    
    int i, sum = 0;
    
    for(i = 1; i <= n; i++) {
        sum += (ans[i] == 0 ? sto[i]: 0);
    }

    if(sum > c2) {
        printf("无法实现!");
        return ;
    }

    for(i = 1; i <= n; i++) {
        printf("船%d装载的货物为%d\n", (ans[i] == 0 ? 2 : 1), i);
    }

    return ;
}

int main() {

    init();
    printf("第一艘船的承重量");
    scanf("%d", &c1);
    printf("第二艘船的承重量");
    scanf("%d", &c2);

    printf("请输入货物的数量");
    scanf("%d", &n);

    for(i = 1; i <= n; i++) {
        printf("第%d件货物的重量为", i);
        scanf("%d", &sto[i]);
        tot += sto[i];
    }

    Search(1);

    Print_Ans();

    return 0;
}
