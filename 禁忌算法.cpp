#include<iostream>
#include<string>
#include<fstream>
#include<cmath>
#include<ctime>
#include<cstdlib>
#include<bits/stdc++.h>
using namespace std;

int MAX_GEN;//迭代次数
int NeighborNum;//邻居数目
int TabuLen;//禁忌长度
int CityNum;//城市数量

int** Distance;//距离矩阵
int bestT;//最佳出现代数

int* Ghh;//初始路径编码
int* bestGh;//最好路径编码
int bestEvaluation;//最好路径长度
int* LocalGhh;//当代最好路径编码
int localEvaluation;//当代最后路径长度
int* tempGhh;//临时编码
int tempEvaluation;//临时路径长度

int** TabuList;//禁忌表
int t;//当前代数

string filename;

int DEBUG = 0;// for debug

void init(int argc, char** argv);
void solve();
void initGroup();
int evaluate(int* arr);
void changeneighbor(int* Gh, int*tempGh);
bool in_TabuList(int* tempGh);
void pushTabuList(int* arr);
void printResult();
void printDebug(int* arr, string message = "");

int main(int argc, char** argv) {
    init(argc, argv);
    clock_t start, finish;
    start = clock();
    solve();
    finish = clock();
    double run_time = (double)(finish - start) / CLOCKS_PER_SEC;
    printResult();
    cout << "Runtime: " << run_time << " seconds" << endl;
    system("pause");
    return 0;
}

// 初始化各种参数
void init(int argc, char** argv){

    MAX_GEN = 1000;
    NeighborNum = 200;
    CityNum = 14;
    TabuLen = 100;

    Distance = new int* [CityNum];
    for(int i = 0; i < CityNum; i++){
        Distance[i] = new int[CityNum];
    }
    // 读入点坐标 计算距离矩阵
        double x[CityNum] = {16.47, 16.47, 20.09, 22.39, 25.23, 22.00, 20.47, 17.20, 16.30, 14.05, 16.53, 21.52, 19.41, 20.09};
        double y[CityNum] = {96.10, 94.44, 92.54, 93.37, 97.24, 96.05, 97.02, 96.29, 97.38, 98.12, 97.38, 95.59, 97.13, 94.55};
        for(int i = 0; i < CityNum - 1; i++){
            Distance[i][i] = 0;
            for(int j = i + 1; j < CityNum; j++){
                double Rij = sqrt(pow(x[i] - x[j], 2) + pow(y[i] - y[j], 2));
                Distance[i][j] = Distance[j][i] = (int)(Rij + 0.5);//四舍五入
            }
        }
        Distance[CityNum - 1][CityNum - 1] = 0;

    // 申请空间 最佳路径无穷大
    Ghh = new int[CityNum];
    bestGh = new int[CityNum];
    bestEvaluation = INT_MAX;
    LocalGhh = new int[CityNum];
    localEvaluation = INT_MAX;
    tempGhh = new int[CityNum + 100];
    tempEvaluation = INT_MAX;
    // 申请空间 迭代次数初始化0
    TabuList = new int*[TabuLen];
    for(int i = 0; i < TabuLen; i++){
        TabuList[i] = new int[CityNum];
    }
    bestT = t = 0;
    // 设置随机数种子
    srand((unsigned int)time(0));
}

// 求解TSP问题
void solve(){
    initGroup();//初始化路径编码

    // 假设为最优，如有更优则更新
    memcpy(bestGh, Ghh, sizeof(int)*CityNum);
    bestEvaluation = evaluate(Ghh);

    // 有限次数迭代
    int nn;
    while(t < MAX_GEN){
        nn = 0;
        localEvaluation = INT_MAX;// 初始化无穷大
        while(nn < NeighborNum){
            changeneighbor(Ghh, tempGhh);// 得到当前编码Ghh的邻域编码tempGhh
            //if(++DEBUG < 10)printDebug(tempGhh, "after_change");
            if(!in_TabuList(tempGhh)){// 禁忌表中不存在
                tempEvaluation = evaluate(tempGhh);
                if(tempEvaluation < localEvaluation){// 局部更新
                    memcpy(LocalGhh, tempGhh, sizeof(int)*CityNum);
                    localEvaluation = tempEvaluation;
                }
                nn++;
            }
        }
        if(localEvaluation < bestEvaluation){// 最优更新
            bestT = t;
            memcpy(bestGh, LocalGhh, sizeof(int)*CityNum);
            bestEvaluation = localEvaluation;
        }
        memcpy(Ghh, LocalGhh, sizeof(int)*CityNum);// 可能更差，但同样更新

        pushTabuList(LocalGhh);// 加入禁忌表
        t++;
    }

    //printResult();// 输出结果
}

// 初始化编码Ghh
void initGroup(){
    // 默认从0号城市开始
    for(int i = 0; i < CityNum; i++){
        Ghh[i] = i;
    }
    //printDebug(Ghh, "init_Ghh");
}

// 计算路径距离
int evaluate(int* arr){
    int len = 0;
    for(int i = 1; i < CityNum; i++){
        len += Distance[arr[i - 1]][arr[i]];
    }
    len += Distance[arr[CityNum - 1]][arr[0]];
    return len;
}

// 得到当前编码Ghh的邻域编码tempGhh
void changeneighbor(int* Gh, int* tempGh){

    int ran1 = rand() % CityNum;
    while(ran1 == 0){
        ran1 = rand() % CityNum;
    }
    int ran2 = rand() % CityNum;
    while(ran1 == ran2 || ran2 == 0){
        ran2 = rand() % CityNum;
    }

    int ran3 = rand() % 3;
    // 随机交换一个数
    if(ran3 == 0){
        memcpy(tempGh, Gh, sizeof(int)*CityNum);
        swap(tempGh[ran1], tempGh[ran2]);
    }
    // 随机交换中间一段距离
    else if(ran3 == 1){
        if(ran1 > ran2){
            swap(ran1, ran2);
        }
        int Tsum = ran1 + ran2;
        for(int i = 0; i < CityNum; i++){
            if(i >= ran1&&i <= ran2){
                tempGh[i] = Gh[Tsum - i];
            }
            else{
                tempGh[i] = Gh[i];
            }
        }
    }
    // 随机交换一段距离
    else if(ran3 == 2){
        if(ran1 > ran2){
            swap(ran1, ran2);
        }
        int index = 0;
        for(int i = 0; i < ran1; i++){
            tempGh[index++] = Gh[i];
        }
        for(int i = ran2 + 1; i < CityNum; i++){
            tempGh[index++] = Gh[i];
        }
        for(int i = ran1; i <= ran2; i++){
            tempGh[index++] = Gh[i];
        }
    }
}

// 判读编码是否在禁忌表中
bool in_TabuList(int* tempGh){
    int i;
    int flag = 0;
    for(i = 0; i < TabuLen; i++){
        flag = 0;
        for(int j = 0; j < CityNum; j++){
            if(tempGh[j] != TabuList[i][j]){
                flag = 1;
                break;
            }
        }
        if(flag == 0)   
            break;
    }
    return !(i == TabuLen);
}

// 加入禁忌表
void pushTabuList(int* arr){
    // 删除队列第一个编码
    for(int i = 0; i < TabuLen - 1; i++){
        for(int j = 0; j < CityNum; j++){
            TabuList[i][j] = TabuList[i + 1][j];
        }
    }
    // 加入队列尾部
    for(int k = 0; k < CityNum; k++){
        TabuList[TabuLen - 1][k] = arr[k];
    }
}

// 输出结果
void printResult(){

    cout << "最佳长度出现代数：" << bestT << endl;

    cout << "最佳路径长度： " << bestEvaluation << endl;
    cout << "最佳路径：" << endl;
    for(int i = 0; i < CityNum; i++){
        cout << bestGh[i] << "->";
    }
    cout << 0 << endl;
}

// Only for Debug
void printDebug(int* arr, string message){
    cout << message << ": ";
    for(int i = 0; i < CityNum; i++){
        cout << arr[i] << " ";
    }
    cout << endl;
}
