
#include "train.h"
#include <bits/stdc++.h>
using namespace std;

float GetSpeed::GetAcc(float dSpeed,int loadSituation,float slopePer) {
    map<float,float> AW;
    switch(loadSituation) {
        case 0: 
            AW = AW0; break;
        case 2:
            AW = AW2; break;
        case 3:
            AW = AW3; break;
        default:
            cout<<"请输入正确的载荷情况对应的数字"<<endl;break;
    }
    float acc = -1;
    for(auto it=AW.begin();it!=AW.end();it++) {
        if(it->first>dSpeed){
            
            acc=(--it)->second;
            break;
        }
    }
    if(acc == -1) cout<<"当前车速超过载荷表记载数值，速度过高"<<endl;
    acc=acc+acc*0.01*slopePer*rotationFactor;
    return acc;
};

float GetSpeed::DistancePhase12 (float dSpeed, float correctAcc) {
    float s12=dSpeed*(alarmTime+cutoffTime)+0.5*correctAcc*(alarmTime+cutoffTime)*(alarmTime+cutoffTime);
    return s12;   
};

float GetSpeed::DistancePhase3(float dSpeed,float correctAcc) {
    dSpeed=dSpeed+correctAcc*(alarmTime+cutoffTime);
    float s3=dSpeed*setupTime-0.5*coastingDec*setupTime*setupTime;
    return s3;
};

float GetSpeed::DistancePhase4(float dSpeed,float correctAcc) {
    dSpeed=dSpeed+correctAcc*(alarmTime+cutoffTime);
    dSpeed=dSpeed-coastingDec*setupTime;
    return dSpeed*dSpeed/2*averageBrakingDec;
};

float GetSpeed::MTurnToKm(float val) {
    return val*3600/1000;
};

float GetSpeed::KmTurnToM(float val) {
    return val*1000/3600;
};

 string GetSpeed::GetDrivingRisk ( int dMode, float dSpeed, float  mSpeed , int curState){
    //cout<<"列车风险等级评估模式为： "<<drivingMode[dMode]<<endl;
    string curMode=drivingMode[dMode];
    if(curMode=="正常模式" && curState==0) {
        return "低风险";
    }
    if( curMode=="后备模式" || curState==1)
            if(dSpeed>mSpeed){
                return "高风险";
            } else if  (dSpeed>=mSpeed-riskLevel ) {
                return "中风险";
            }else if( dSpeed>0 ){
                return "低风险";
            }       
 };
/**
 * @description: 通过输入得到列车当前最大安全速度，并分析风险等级。
 * @return {*} 返回最大安全速度
 * @Author: zy
 * @Date: 2024-06-12 15:28:41
 * @LastEditTime: Do 
    
    vector<string>  drivingMode = {"后备模式","正常模式"};     //列车not edit
 * @LastEditors: zy
 * @param {float} safetyDistance     最大安全距离
 * @param {float} dSpeed                    列车运行速度
 * @param {int} loadSituation           列车载荷情况
 * @param {float} slopePer                  坡度千分数
 * @param {int} dMode                          列车风险等级评估模式
 */
float GetSpeed::GetSafetySpeed(float safetyDistance, float dSpeed,int dMode, int loadSituation,float slopePer, int curState) {
    float  mSpeed=0;  //km/h

    if(safetyDistance<=0 || safetyDistance>400){
        cout<<"输入的最近安全距离有误，请重新输入"<<endl;
        return -1;
    }
    if((slopePer<-0.035 || slopePer >0.030 ) ){
        cout<<"输入的坡度超过正常范围（-35-30),请重新输入"<<endl;
        return -1;
    }
    if(dSpeed>maxSpeed){
        cout<<"输入车速超过列车最大速度(140km/h)，请重新输入"<<endl;
        return -1;
    }
    if(loadSituation!=0 && loadSituation!=2 && loadSituation!=3){
        cout<<"请重新输入正确的载荷情况"<<endl;
        return -1;
    }
    if(dMode !=0 && dMode!=1){
        cout<<"请重新输入正确的列车风险等级评估模式"<<endl;
        return -1;
    }

    for(float i=0;i<=140;i+=1){
        float tempAcc=GetAcc(dSpeed,loadSituation,slopePer);
        float tempSpeed=KmTurnToM(i);
        
        float s0=DistancePhase12(tempSpeed,tempAcc)+DistancePhase3(tempSpeed,tempAcc)+DistancePhase4(tempSpeed,tempAcc);
        if(s0>safetyDistance) {
            mSpeed= i-1;
            break;
            };
    }
    cout<<"----------------------------------"<<endl;
    cout<<"当前列车的行驶速度为  "<<fixed<<setprecision(2)<<KmTurnToM(dSpeed)<<" m/s "<<dSpeed<<" km/h "<<endl;
    cout<<"当前列车最大安全速度为 "<<fixed<<setprecision(2)<<KmTurnToM(mSpeed)<<" m/s  "<< mSpeed<<" km/h "<<endl;

    string dRisk=GetDrivingRisk(dMode, dSpeed, mSpeed, curState);
    cout<<"当前列车的风险等级为  "<<dRisk <<endl;
    return mSpeed;
};







