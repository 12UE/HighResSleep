#include<iostream>
#include<chrono>
#include<Windows.h>
#include<mmsystem.h>
#pragma comment(lib,"winmm.lib")
#include<algorithm>
#include<deque>
#include<thread>
#include<stdio.h>
#define TEST_TIMES 5000000//测试次数
#define DELAY_TIME 16.84
#define BUFFER_SIZE 128
#define error_buffersize 10
using namespace std;
class SimpleClock {
	using  mili=std::chrono::microseconds;
	using chrono_clock=std::chrono::system_clock;
	using microseconds=std::chrono::duration<double, std::micro>;
public:
	SimpleClock() {
		timeBeginPeriod(1);
	}
	~SimpleClock() {
		timeEndPeriod(1);
	}
	int HighResSleep(double miliseconds) {
		int Rotation_Count=1;
		if (miliseconds < 0.0) return 0;
		auto start=chrono_clock::now();
		if (miliseconds > 1.0) {
			double ready_sleep_time=floorf(miliseconds);
			if (miliseconds - ready_sleep_time < 0.5) {
				SleepEvent(ready_sleep_time);//较为精确的sleep
			} else {
				Sleep(ready_sleep_time);//有误差的sleep
			}		
		}
		auto elapsed=std::chrono::duration_cast<mili>(chrono_clock::now() - start).count();
		if (elapsed <= (long)(miliseconds * 1000)) {
			microseconds waitTime(miliseconds * 1000 - elapsed);
			start=chrono_clock::now();
			while (std::chrono::duration_cast<mili>(chrono_clock::now() - start) < waitTime) Rotation_Count++;
		} return Rotation_Count;
	}
	void SleepEvent(int interval) {
		HANDLE evt=CreateEvent(NULL, TRUE, FALSE, NULL);
		WaitForSingleObject(evt, interval);
		CloseHandle(evt);
	}
};

int main() {
	
	SimpleClock chrnonclock;
	double sleeptime=6;
	double runtime_socre=0;
	for (int k=0; k < 10; k++) 		{
		
		volatile DWORD64 totalError=0;
		DWORD64 totalSleep=0;
		DWORD64 sum_rotation=0;
		for (int i=0; i < TEST_TIMES; i++) {
			auto start=std::chrono::high_resolution_clock::now();
			sum_rotation+=chrnonclock.HighResSleep(sleeptime);
			auto end=std::chrono::high_resolution_clock::now();
			auto used_time=std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
			totalSleep+=used_time;
			totalError+=abs(used_time - sleeptime * 1000);
		}
		cout << "________" << k << "_________" << endl;
		cout << "测试次数：" << TEST_TIMES << "次" << endl;
		cout << "设置睡眠：" << sleeptime << "毫秒" << endl;
		cout << "平均睡眠：" << totalSleep / TEST_TIMES / 1000.0f << "毫秒" << endl;
		cout << "平均自旋次数：" << sum_rotation / TEST_TIMES << "次" << endl;
		cout << "平均误差：" << totalError / TEST_TIMES << "微秒" << endl;
		cout << "累计误差：" << totalError / 1000.0f << "毫秒" << endl;
		double score=sum_rotation / TEST_TIMES * totalError / TEST_TIMES;
		printf("%f评分为%f\n", sleeptime,score);
		runtime_socre+=score;
		sleeptime+=0.1;
	}
	cout << "各个时段平均评分" << runtime_socre/10 << endl;
	system("pause");
	return 0;
}
