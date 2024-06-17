/*
 * @Description: main函数，执行列车输入情况以及列车最大安全速度与危险等级的输出。
 * @Author: zy
 * @Date: 2024-06-06 17:51:53
 * @LastEditTime: 2024-06-17 18:09:55
 * @LastEditors: zy
 */
    
#include <bits/stdc++.h>
#include "train.h"
using namespace std;
                                        
int  obSituation( );
float GetSafetyDistance( ) ;
float GetSlopePer();
float GetDrivingSpeed();
int GetLoadSituation(); 
int GetDrivingMode();
int GetWeatherCondition();
void Input(int &,float &, float &, float & , int & ,int &, int &);
               
int main(int argc, char *argv[]) {
  float safetyDistance = 0, slopePer = 0, dSpeed = 0;
  int  curState = 0, loadSituation = 0, dMode = 0, dWeather = 0;

  label:
    Input(curState, safetyDistance, slopePer, dSpeed, loadSituation, dMode, dWeather);
    GetSpeed TrainOne;
    float mSpeed = TrainOne.GetSafetySpeed(safetyDistance, dSpeed, dMode, loadSituation, slopePer, curState, dWeather);
  if (mSpeed == -1 || mSpeed <= 0) {
    std::cout << std::endl << "-------------------------------------------------" << std::endl;
    goto label;
  }
  
  string mRisk = TrainOne.GetDrivingRisk(dMode, dSpeed, mSpeed, curState);
  TrainOne.GetBrakingDistance();
    
  return 0;
}

int  obSituation( ) {
  int curState = 1;
  std::cout << "如果发现障碍物输入1 未发现障碍物输入0" << std::endl;
  std::cin >> curState;
  return curState;
}

float GetSafetyDistance(int curState  ) {
  float a01 = 0, a02 = 0, a = 0;
  if(curState == 0) {
    std::cout << "分别输入平直轨道对列车最int 大探测距离 以及 当前点的传感器视距 （单位：米）" << std::endl;
    std::cin >> a01 >> a02;
    return min(a01,a02);
  } else if(curState == 1) {
    std::cout << "请输入最近障碍物距离 (单位:米)" << std::endl;
    std::cin >> a; 
    return a;
  }
};

float GetSlopePer() {
  float slope = 0;
  std::cout << "请输入此刻坡度千分数为‰ " << std::endl;
  std::cin >> slope;
  slope = slope/1000;
  return slope;
};

float GetDrivingSpeed() {
  float speed = 0;
  std::cout << "请输入此刻列车行驶速度km/h" << std::endl;
  std::cin >> speed;
  return speed;
};

int GetLoadSituation() {
  std::cout << "请输入当前列车载荷情况 AW0输入0 AW2输入2 AW3输入3" << std::endl;
  int aw = 0;
  std::cin >> aw;
  return aw;
};

int GetDrivingMode() {
  std::cout << "请输入当前列车风险等级评估模式,后备模式输入0,正常模式输入1" << std::endl;
  int dMode = 0;
  std::cin >> dMode;
  return  dMode; 
}

int GetWeatherCondition(){
  std::cout<<"请输入当前列车行驶的轨道情况: 干燥轨道输入0  湿润轨道输入1  " << std::endl;
  int  dWeather;
  std::cin >> dWeather;
  return dWeather;
}
/**
 * @description: 获取列车基本输入
 * @Author: zy
 * @Date: 2024-06-12 15:27:25
 * @LastEditTime: Do not edit
 * @LastEditors: zy
 * @param {float &} safetyDistance      最大安全距离
 * @param {float &} slopePer                  坡度千分数
 * @param {float &} dSpeed                    列车运行速度
 * @param {int &} loadSituation            列车载荷情况
 */
void Input(int &curState, float &safetyDistance, float &slopePer, float &dSpeed, int &loadSituation, int &dMode, int &dWeather) {
  curState = obSituation();
  safetyDistance = GetSafetyDistance(curState);
  slopePer = GetSlopePer();
  dSpeed = GetDrivingSpeed();
  loadSituation = GetLoadSituation();
  dMode = GetDrivingMode();
  dWeather = GetWeatherCondition();
 }

