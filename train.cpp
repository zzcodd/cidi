#include "train.h"
#include <bits/stdc++.h>

float GetSpeed::GetMaxAcc(float dSpeed, enum LoadCond loadSituation, float slopePer) {
  std::map<float,float>* AW = nullptr;
  switch (loadSituation) {
    case NO_LOAD:
      AW = &AW0;
      break;
    case FULL_LOAD:
      AW = &AW2;
      break;
    case OVERLOAD:
      AW = &AW3;
      break;
    default:
      AW = &AW0;
  }
  float acc = -1;
  for(auto it = AW->begin(); it != AW->end(); it++) {
    if(it->first > dSpeed) {
      acc = (--it)->second;
      break;
    }
    else
      acc = it->second;
  }
  
  if(acc < 0) 
    std::cout << "The current speed exceeds the value recorded in the load table" << std::endl;
  acc = acc + acc*0.01*slopePer*rotationFactor;
  return acc;
};

float GetSpeed::DistancePhase12 (float dSpeed, float maxAcc) {
  float s12 = dSpeed*(alarmTime+cutoffTime) + 
              0.5*maxAcc*(alarmTime+cutoffTime)*(alarmTime+cutoffTime);
  return s12;   
};

float GetSpeed::DistancePhase3(float dSpeed, float maxAcc) {
  dSpeed = dSpeed + maxAcc*(alarmTime+cutoffTime);
  float s3 = dSpeed*setupTime - 0.5*coastingDec*setupTime*setupTime;
  return s3;
};

float GetSpeed::DistancePhase4(float dSpeed, float maxAcc, enum WeatherCond dWeather) {
  float brakingDec = 0;
  switch(dWeather){
    case DRY:
      brakingDec = brakingDecDry;
      break;
    case MOIST:
      brakingDec = brakingDecMoist;
      break;
    default:
      brakingDec = brakingDecMoist;
  }
  dSpeed = dSpeed + maxAcc*(alarmTime+cutoffTime);
  dSpeed = dSpeed - coastingDec*setupTime;
  return (dSpeed*dSpeed) / (2*brakingDec);
};

float GetSpeed::MTurnToKm(float val) {
  return val*3600/1000;
};

float GetSpeed::KmTurnToM(float val) {
  return val*1000/3600;
};
/**
 * @description: 根据风险识别模式，得到当前行车风险等级。
 * @return {*} 行车风险等级。
 * @Author: zy
 * @Date: 2024-06-17 14:50:47
 * @LastEditTime: Do not edit
 * @LastEditors: zy
 * @param {float} dSpeed      行车速度
 * @param {float } mSpeed     最大安全速度
 * @param {int} dMode         列车风险等级评估模式
 * @param {bool} curState     探测障碍物情况
 */
enum RrainRisk GetSpeed::GetDrivingRisk(float dSpeed, float mSpeed, enum DriveMode dMode, bool curState){
  enum RrainRisk dRisk = LOW_RISK;
  if(curState || dMode == Standby_Mode) {
    if(dSpeed > mSpeed){
      dRisk = HIGH_RISK;
    } else if(dSpeed >= mSpeed - riskLevel) {
      dRisk = MED_RISK;
    } else {
      dRisk = LOW_RISK;
    }   
  }
  return dRisk;
 };
 
 /**
  * @description: 得到不同速度和坡度下，不同载荷情况，不同行车天气下的刹车距离
  * @return {*} 打印刹车距离，无返回值。
  * @Author: zy
  * @Date: 2024-06-17 14:49:36
  * @LastEditTime: Do not edit
  * @LastEditors: zy
  */
void GetSpeed::GetBrakingDistance() {
  float dSpeed = 10, slopePer = 0;
  std::string filename = "braking_distances.txt"; // 输出文件的名称
  std::ofstream outfile(filename); // 创建ofstream对象并打开文件

  if (!outfile.is_open()) { // 检查文件是否成功打开
    std::cerr << "无法打开文件 " << filename << " 进行写入。\n";
    return;
  }
  std::cout << std::endl << "-------------------------------------------------" << std::endl;
  std::cout << "请输入你想要求什么坡度千分数下的刹车距离 (默认为0)" << std::endl;
  std::cin >> slopePer;

  // 获取当前时间点
  auto now = std::chrono::system_clock::now();
  // 转换为time_t格式
  std::time_t now_c = std::chrono::system_clock::to_time_t(now);
  // 转换为本地时间
  std::tm* now_tm = std::localtime(&now_c);
  // 输出到日志
  std::cout << "[" << std::put_time(now_tm, "%Y-%m-%d %X")  << "] My log message" << std::endl;
  outfile << "[" << std::put_time(now_tm, "%Y-%m-%d %X") <<"]" <<std::endl;

  outfile << "Current slopePer (‰) : " << slopePer << std::endl;
  outfile << "The maximum braking distance under different load conditions and weather conditions (Unit: meters) " << std::endl;

  for (dSpeed; dSpeed <= 140; dSpeed += 10) {
    enum LoadCond loadSituation[] = {NO_LOAD, FULL_SEATS, FULL_LOAD, OVERLOAD};
    enum WeatherCond weather[] = {DRY, MOIST};
    std::vector<std::string> weather_string = {"DRY_TRACK", "MOIST_TRACK"};

    outfile <<  "----------------------------------------------------------" << std::endl;
    outfile << dSpeed << " km/h" << std::setw(5) <<" " << std::setw(15) << "NO_LOAD" << std::setw(15) << "FULL_LOAD" << std::setw(15) << "OVERLOAD" << std::endl;

    float m_dSpeed = KmTurnToM(dSpeed);
    for (enum WeatherCond i : weather) {
      outfile << std::setw(15) << std::left << weather_string[i] << "\t";
      for (enum LoadCond j : loadSituation) {
        if (j == FULL_SEATS) continue; 
        float distance = 0;
        float tempA = GetMaxAcc(dSpeed, j, slopePer);
        distance = DistancePhase12(m_dSpeed, tempA) + DistancePhase3(m_dSpeed, tempA) + DistancePhase4(m_dSpeed, tempA, i);
        outfile << std::fixed << std::setprecision(2) << std::setw(18) << distance << "\t";
      }
      outfile << std::endl;
    }
    outfile << std::endl;
  }

  outfile.close(); // 关闭文件
}

//  void GetSpeed::GetBrakingDistance ( ) {
//   float dSpeed = 10, slopePer = 0;
//   std::cout <<  "请输入坡度千分数 (默认为0)" <<std::endl;
//   for(dSpeed; dSpeed<=140; dSpeed+=10){
//     int  loadSituation[] = {NO_LOAD, FULL_SEATS,  FULL_LOAD, OVERLOAD};
//     int weather[] = {DRY, MOIST,};
//     std::vector<std::string> weather_string = {"DRY_TRACK", "MOIST_TRACK"};
//     std::cout << std::endl << "-------------------------------------------------" << std::endl;
//     std::cout << "Current speed : " << dSpeed << "km/h   " <<"Current sloperPer :  " << slopePer << std::endl;
//     std::cout << "The maximum braking distance under different load conditions and weather conditions " << std::endl;
//     std::cout << std::setw(15) << std::left <<"(Unit: meters)" << std::setw(10) << "NO_LOAD" << std::setw(10) << "FULL_LOAD" << std::setw(10) << "OVERLOAD" ;
//     float m_dSpeed = KmTurnToM(dSpeed);
//     for(int i : weather){
//       std::cout<<std::endl;
//       std::cout << std::setw(15) <<std::left << weather_string[i] << " " ;
//       for(int j : loadSituation) {
//         if(j==1 ) continue;
//         float distance = 0;
//         float tempA = GetMaxAcc(dSpeed, j, slopePer );
//         distance = DistancePhase12(m_dSpeed, tempA) + DistancePhase3(m_dSpeed, tempA) + DistancePhase4(m_dSpeed, tempA, i);
//         std::cout << std::fixed << std::setprecision(2) << std::setw(10) <<distance ; 
//       }
//     }
//     std::cout<<std::endl;
//   }
//  }  
/**
 * @description: 通过输入得到列车当前最大安全速度，并分析风险等级。
 * @return {*} 返回最大安全速度
 * @Author: zy
 * @Date: 2024-06-12 15:28:41
 * @LastEditors: zy
 * @param {unsigned long} safetyDistance     最大安全距离(发现障碍物则为最近障碍物距离，否则为激光可视距离)
 * @param {float} dSpeed                     列车运行速度
 * @param {int} dMode                        列车风险等级评估模式
 * @param {int} loadSituation                列车载荷情况
 * @param {float} slopePer                   坡度千分数
 * @param {int} dWeather                     行车天气(轨道状态)
 */
float GetSpeed::GetSafetySpeed (unsigned long safetyDistance, float dSpeed, 
                                enum LoadCond loadSituation, 
                                float slopePer, enum WeatherCond dWeather) {
  float  mSpeed = 0;  //km/h
  if (safetyDistance > 1000){
    std::cout << "The safe distance is incorrect (0-1000m): " << safetyDistance << std::endl;
    return -1;
  }
  if (slopePer<-0.035 || slopePer > 0.030) {
    std::cout << "The slope exceeds the normal range (-35-30) ‰: "<< slopePer << std::endl;
    return -1;
  }
  // if (dSpeed > maxSpeed){
  //   std::cout << "The speed exceeds the maximum speed (140km/h): " << dSpeed <<std::endl;
  //   return -1;
  // }

  for (float i=0; i<=140; i+=1){
    float tempAcc = GetMaxAcc(dSpeed, loadSituation, slopePer);
    float tempSpeed = KmTurnToM(i);
    
    float s0 = DistancePhase12(tempSpeed, tempAcc) + DistancePhase3(tempSpeed, tempAcc) +
               DistancePhase4(tempSpeed, tempAcc, dWeather);
    if (s0 > safetyDistance) {
      mSpeed = i-1;
      break;
    }
  }

  return mSpeed;
};







