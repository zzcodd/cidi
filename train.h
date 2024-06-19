/*
 * @Description: GetSpeed类和载荷-牵引加速度对应表
 * @Author: zy
 * @Date: 2024-06-07 08:53:53
 * @LastEditTime: 2024-06-19 15:13:11
 * @LastEditors: zhang.hq zhang.hq@cidi.ai
 */

#include <bits/stdc++.h>
#ifndef  _TRAIN_H
#define _TRAIN_H

enum WeatherCond {
  DRY = 0,    // 干燥轨道
  MOIST,     // 湿润轨道
};

enum LoadCond {
  NO_LOAD = 0,    // 空载
  FULL_SEATS,     // 满座
  FULL_LOAD,      // 满载
  OVERLOAD        // 超载
};

enum DriveMode {
  NORMAL_MODE = 0,
  Standby_Mode
};

enum RrainRisk {
  LOW_RISK = 0,
  MED_RISK,
  HIGH_RISK
};

/**
 * @description: 三个接口分别得到：最大安全速度、行车风险、刹车距离
 * @brief：基于当前障碍物距离，得到列车最大速度类。
 * @Author: zy
 * @Date: 2024-06-17 14:41:49
 * @LastEditTime: Do not edit
 * @LastEditors: zy
 */
class GetSpeed{
  public:
    float GetSafetySpeed(unsigned long safetyDistance, float dSpeed, 
                         enum LoadCond loadSituation = NO_LOAD, float slopePer = 0.030,
                         enum WeatherCond dWeather = MOIST);    //得到当前输入下最大安全速度
    enum RrainRisk GetDrivingRisk (float dSpeed, float mSpeed, enum DriveMode dMode = NORMAL_MODE, 
                                bool curState = false);//得到当前列车行车风险
    void GetBrakingDistance ();    //考虑不同速度和坡度(默认为0)下各种情况下的刹车距离(天气、载荷情况)      

  private:
    float GetMaxAcc(float dSpeed, enum LoadCond  loadSituation, float slopePer);// 得到当前列车速度下的修正后加速度
    float DistancePhase12(float dSpeed, float maxAcc);            // 计算列车第一阶段和第二阶段运行距离
    float DistancePhase3(float dSpeed, float maxAcc);             // 计算列车第三阶段运行距离
    float DistancePhase4(float dSpeed, float maxAcc, enum WeatherCond dWeather);  // 计算列车第四阶段运行距离
              
    float MTurnToKm(float val); // 单位m/s转换为km/h
    float KmTurnToM(float val); // 单位km/h转换为m/s

    const int maxSpeed = 140;            // 列车最大行驶速度
    const float alarmTime = 3;           // 列车报警反应时间
    const float cutoffTime = 1.1;        // 列车惰行时间
    const float setupTime = 1;           // 列车建立制动时间
    const float rotationFactor = 1.0875; // 旋转质量系数
    const float coastingDec = 0.15;      // 惰行减速度
    const float averageBrakingDec = 1.2; // 平均制动减速度
    const float brakingDecDry = 1.05;    // 制动减速度(干燥轨道最小减速度)
    const float brakingDecMoist = 0.78;  // 制动减速度(湿轨道最小减速度)
    const float riskLevel = 5;           // 风险等级定义系数
};

static std::map<float,float> AW0 = {{0,1}, {7.3,1.031}, {14.7,1.029}, {22.1,1.028}, {29.5,1.026}, {36.9,1.024}, {44.3,1.022},{ 51.6,1.019}, {59,0.998}, {66,0.897},
    {72.3,0.813}, {78,0.748}, {83.3,0.696}, {88.2,0.652}, {92.9,0.615}, {97.2,0.583}, {101.4,0.555}, {105.3,0.53}, {109.1,0.507}, {112.7,0.487}, {116.2,0.468}, {119.5,0.451},
    {122.7,0.425}, {125.7,0.4}, {128.6,0.378}, {131.3,0.357}, {133.8,0.34}, {136.2,0.323}, {138.5,0.308}, {140.7,0.295}, };
        
static std::map<float,float> AW2 = {{0,1}, {7.3,1.032}, {14.7,1.03}, {29.6,1.027}, {36.9,1.025}, {44.3,1.017}, {57.6,0.788}, {63.1,0.713}, {68.2,0.656}, {77.1,0.572},
    {81.2,0.539}, {85,0.511}, {92.1,0.465}, {95.4,0.445}, {98.6,0.428}, {104.6,0.397}, {107.4,0.383}, {110.1,0.37}, {115.4,0.348}, {117.8,0.337}, {120.3,0.327}, {124.8,0.296},
    {126.9,0.282}, {128.9,0.27}, {132.7,0.249}, {134.4,0.239}, {136.1,0.23}, {139.4,0.213}, {140.9,0.206}, {142.4,0.199}};
        
static std::map<float,float> AW3 = {{0,1}, {14.7,1.03}, {29.6,1.027}, {44.3,0.922}, {56.3,0.716}, {65.9,0.603}, {74.2,0.529}, {81.5,0.474}, {88.1,0.432}, {94.1,0.398},
    {99.7,0.37}, {104.9,0.346}, {109.7,0.325}, {114.3,0.307}, {118.6,0.291}, {122.7,0.268}, {126.5,0.246}, {129.9,0.227}, {133.1,0.211}, {136,0.196}, {138.8,0.184}, {141.4,0.172},
    {143.8,0.162}, {146.1,0.152}, {148.2,0.144}, {150.3,0.136}, {152.2,0.129}, {154,0.122}, {155.7,0.116}, {157.4,0.11}};

#endif 
