#include "train.h"
#include <bits/stdc++.h>

enum LoadSituation {
  NO_LOAD = 0,    // 空载
  FULL_SEATS,     // 满座
  FULL_LOAD,      // 满载
  OVERLOAD        // 超载
};

float GetSpeed::GetAcc(float dSpeed, int loadSituation, float slopePer) {
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
      std::cout << "Invalid load condition" << std::endl;
      return -1; // 返回错误标识或默认值
  }
  float acc = -1;
  for(auto it = AW->begin(); it != AW->end(); it++) {
    if(it->first > dSpeed) {
      acc = (--it)->second;
      break;
    }
  }
  if(acc == -1) std::cout << "The current speed exceeds the value recorded in the load table" << std::endl;
  acc = acc + acc*0.01*slopePer*rotationFactor;
  return acc;
};

float GetSpeed::DistancePhase12 (float dSpeed, float correctAcc) {
  float s12 = dSpeed*(alarmTime+cutoffTime)+0.5*correctAcc*(alarmTime+cutoffTime)*(alarmTime+cutoffTime);
  return s12;   
};

float GetSpeed::DistancePhase3(float dSpeed, float correctAcc) {
  dSpeed = dSpeed +correctAcc*(alarmTime+cutoffTime);
  float s3 = dSpeed*setupTime-0.5*coastingDec*setupTime*setupTime;
  return s3;
};

enum WeatherSituation {
  DRY = 0,    // 干燥轨道
  MOIST,     // 湿润轨道
};

float GetSpeed::DistancePhase4(float dSpeed, float correctAcc, int dWeather) {
  float brakingDec = 0;
  switch(dWeather){
    case DRY:
      brakingDec = brakingDecDry ;
      break;
    case MOIST:
      brakingDec = brakingDecMoist ;
      break;
    default:
      std::cout << "Invalid weather condition" << std::endl;
      return -1; // 返回错误标识或默认值
  }
  dSpeed = dSpeed+correctAcc*(alarmTime+cutoffTime);
  dSpeed = dSpeed-coastingDec*setupTime;
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
 * @param {int} dMode           列车风险等级评估模式
 * @param {float} dSpeed      行车速度
 * @param {float } mSpeed    最大安全速度
 * @param {int} curState         探测障碍物情况
 */
std::string GetSpeed::GetDrivingRisk ( int dMode, float dSpeed, float  mSpeed , int curState){
    //cout<<"列车风险等级评估模式为： "<<drivingMode[dMode]<<endl;
  std::string curMode = drivingMode[dMode];
  std::string dRisk = " ";
  if(curMode == "正常模式" && curState == 0) {
    dRisk = "低风险";
  }
  if( curMode == "后备模式" || curState == 1) {
    if(dSpeed > mSpeed){
      dRisk = "High risk";
    } else if  (dSpeed >= mSpeed-riskLevel ) {
      dRisk = "Medium risk";
    } else if(dSpeed>0)  {
      dRisk = "Low risk";
    }       
  }
  std::cout << "The risk level of the current train is  " << dRisk << std::endl;

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
  std::time_t now_c = std::chrono::system_clock::to_time_t(now - std::chrono::hours(24));
  // 转换为本地时间
  std::tm* now_tm = std::localtime(&now_c);
  // 输出到日志
  std::cout << "[";
  std::cout << std::put_time(now_tm, "%Y-%m-%d %X"); // 格式化时间戳
  std::cout << "] My log message" << std::endl;

  outfile << "[" << std::put_time(now_tm, "%Y-%m-%d %X") <<"]" <<std::endl;

  outfile << "Current slopePer : " << slopePer << std::endl;
  outfile << "The maximum braking distance under different load conditions and weather conditions (Unit: meters) " << std::endl;

  for (dSpeed; dSpeed <= 140; dSpeed += 10) {
    int loadSituation[] = {NO_LOAD, FULL_SEATS, FULL_LOAD, OVERLOAD};
    int weather[] = {DRY, MOIST};
    std::vector<std::string> weather_string = {"DRY_TRACK", "MOIST_TRACK"};

    outfile <<  "----------------------------------------------------------" << std::endl;
    outfile << dSpeed << " km/h" << std::setw(5) <<" " << std::setw(15) << "NO_LOAD" << std::setw(15) << "FULL_LOAD" << std::setw(15) << "OVERLOAD" << std::endl;

    float m_dSpeed = KmTurnToM(dSpeed);
    for (int i : weather) {
      outfile << std::setw(15) << std::left << weather_string[i] << "\t";
      for (int j : loadSituation) {
        if (j == FULL_SEATS) continue; 
        float distance = 0;
        float tempA = GetAcc(dSpeed, j, slopePer);
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
//         float tempA = GetAcc(dSpeed, j, slopePer );
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
 * @param {float} safetyDistance     最大安全距离
 * @param {float} dSpeed                    列车运行速度
 * @param {int} dMode                          列车风险等级评估模式
 * @param {int} loadSituation           列车载荷情况
 * @param {float} slopePer                  坡度千分数
 * @param {int} curState                       探测障碍物情况
 * @param {int} dWeather                     行车天气(轨道状态)
 */
float GetSpeed::GetSafetySpeed(float safetyDistance, float dSpeed, int dMode, int loadSituation, float slopePer, int curState, int dWeather) {
  float  mSpeed = 0;  //km/h
  if(curState !=0 && curState != 1) {
    std::cout<<"The input of whether there are obstacles in front is incorrect (1 indicates obstacles, 0 indicates no obstacles)" <<std::endl;
    return -1;
  }
  if(safetyDistance<=0 || safetyDistance>400){
    std::cout << "The nearest safe distance entered is incorrect (0-400m), please re-enter" << std::endl;
    return -1;
  }
  if(slopePer<-0.035 || slopePer >0.030  ) {
    std::cout << "The input slope exceeds the normal range (-35-30) ‰, please re-enter" << std::endl;
    return -1;
  }
  if(dSpeed > maxSpeed){
    std::cout << "The input speed exceeds the maximum train speed (140km/h), please re-enter" << std::endl;
    return -1;
  }
  if(loadSituation!=0 && loadSituation!=2 && loadSituation!=3){
    std::cout << "Please re-enter the correct load condition (AW0, AW2, AW3)" << std::endl;
    return -1;
  }
  if(dMode !=0 && dMode!=1){          
    std::cout << "Please re-enter the correct train risk level assessment mode (normal mode backup mode)" << std::endl;
    return -1;
  }
  if(dWeather !=0 && dWeather!=1){          
    std::cout << "Please re-enter the correct weather conditions (dry or moist)" << std::endl;
    return -1;
  }
  for(float i=0; i<=140; i+=1){
    float tempAcc = GetAcc(dSpeed, loadSituation, slopePer);
    float tempSpeed = KmTurnToM(i);
    
    float s0 = DistancePhase12(tempSpeed, tempAcc)+DistancePhase3(tempSpeed, tempAcc)+DistancePhase4(tempSpeed, tempAcc, dWeather);
    if(s0>safetyDistance) {
      mSpeed= i-1;
      break;
      };
  }
      
  std::cout << std::endl << "-------------------------------------------------" << std::endl;
  std::cout << "The current speed of the train is  " << std::fixed << std::setprecision(2) << KmTurnToM(dSpeed) << " m/s " << dSpeed << " km/h " << std::endl;
  std::cout << "The maximum safe speed of the current train is " << std::fixed << std::setprecision(2) << KmTurnToM(mSpeed) << " m/s  " <<  mSpeed << " km/h " << std::endl;

  return mSpeed;
};







