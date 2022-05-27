//
// Created by 79069 on 2022/3/15.
//
#ifndef OS_GLOBAL_H
#define OS_GLOBAL_H
#include <windows.h>
#include<stdlib.h>
#include<vector>
#include<queue>
#include<list>
#include<map>
#include<ctime>
#include<string>
#include<random>
#include <conio.h>
#include<iostream>
#include<fstream>
#include<algorithm>
#include<sstream>
#include<string.h>
#include<cstring>
#include <time.h>
#define TABLE_LEN 1000
#define SEQ_LEN 20
#define USERMODE 10
#define KERNELMODE 11
#define IN 1
#define OUT 0
#define MIDTERM_SWITCH_OUT 0
#define MIDTERM_SWITCH_IN 1
#define PREEMPTION_SWITCH 2
#define PROCESS_CREATING 0   //���̴��� 
#define PROCESS_READY 1  //���̾���̬ 
#define PROCESS_RUNNING 2//��������̬ 
#define PROCESS_BLOCK 3//��������̬ 
#define PROCESS_SUSPEND_BLOCK 5//���̹���ȴ� 
#define PROCESS_SUSPEND_READY 4//���̹������ 
#define PROCESS_DEAD 6//���̽����˳� 
#endif  //OS_GLOBAL_H
#pragma once
