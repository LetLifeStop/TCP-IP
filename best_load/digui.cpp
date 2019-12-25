#include <bits/stdc++.h>
using namespace std;
const int maxn = 2e5 + 100;
int ans = 0;
vector<pair<int, bool> >ret;

void Search(int n, int val, int pos, int tot, vector<pair<int, bool>> &sto) {
    
    if(ans < val) {
        ans = val;
        ret = sto;
    }
//    cout << val << endl;
//    if(val == 3) {
//        for(int i = 0; i < n; i++) {
//            cout << i << "  " << sto[i].second << endl;
//        }
//    }
    if(tot <= 0)
        return ;
    
    int i;

    for(i = pos; i < n; i++){
        
        Search(n, val, i + 1, tot, sto);
        
        if(sto[i].first <= tot && !sto[i].second) {
        sto[i].second = 1;
        Search(n, val + 1, i + 1, tot - sto[i].first, sto);
        sto[i].second = 0;
        }
    }

    return ;
}

int main()
{
    int n, i, tot_weight;
    pair<int, bool> tmp;
    printf("总件数为:");
    scanf("%d", &n);
//    sto.reverse(n + 100);

    vector<pair<int, bool> >sto;
    
    for(i = 1; i <= n; i++) {
        printf("第%d件物品重量为:", i);
        scanf("%d", &tmp.first);
        tmp.second = 0;
        sto.push_back(tmp);
    }
    
//    for(i = 0; i < n; i++) {
//        printf("%d %d %d\n", i, sto[i].first, sto[i].second);
//    }
    printf("船能承受的最大重量为");
    scanf("%d", &tot_weight);
    Search(n, 0, 0, tot_weight, sto);
    
    printf("在保证效率最大的前提下，能装上船的货物为\n");
    for(i = 0; i < n; i++) {
       if(!ret[i].second)
            continue;
        printf("第%d件物品\n", i + 1);
    }

    return 0;
}
