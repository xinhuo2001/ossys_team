### 思路记录
- 先序遍历存储树
  - 左子为孩子 右子为兄弟
  - 层序遍历存储
    - 1 2 . 5 3 . 6 7 4 . . . . 8 . . .
    - 

### 目录初始条件
```sh
root
|_______bin
        |_______a.sh
        |_______b.sh
|_______user
        |_______Desktop
|_______user2
        |_______Document
```

层序存储如下
```sh
#标号不重要 0表示空
1 root
2 bin
0 
5 a.sh
3 user
0 
6 b.sh
7 Desktop
4 user2
0
0
0
0
8 Document
0
0
0
```

### 运行
```sh
make
./main
```