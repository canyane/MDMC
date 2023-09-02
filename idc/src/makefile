# 开发框架头文件路径。
PUBINCL = -I/home/ubuntu/MDMC/project/public

# 开发框架cpp文件名，这里直接包含进来，没有采用链接库，是为了方便调试。
PUBCPP = /home/ubuntu/MDMC/project/public/_public.cpp

# mysql头文件存放路径
MYSQLINCL = -I/home/ubuntu/MDMC/project/public/db/mysql -I/usr/local/mysql/include  

# mysql库文件保存路径
MYSQLLIB = -L/usr/local/mysql/lib

# mysql链接库
MYSQLLIBS = -lmysqlclient

# 开发框架mysql的cpp文件名，这里直接包含进来，没有采用链接库，是为了方便调试。
MYSQLCPP = /home/ubuntu/MDMC/project/public/db/mysql/_mysql.cpp

# 编译参数。
CFLAGS = -g 

all: crtsurfdata obtcodetodb obtmindtodb

crtsurfdata: crtsurfdata.cpp
	g++ $(CFLAGS) -o crtsurfdata crtsurfdata.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp crtsurfdata ../bin/.

obtcodetodb:obtcodetodb.cpp
	g++ $(CFLAGS) -o obtcodetodb obtcodetodb.cpp $(PUBINCL) $(PUBCPP) $(MYSQLCPP) $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS)  -lm -lc
	cp obtcodetodb ../bin/.

obtmindtodb:obtmindtodb.cpp 
	g++ $(CFLAGS) -c $(MYSQLCPP) $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) -lm -lc
	ar -rc _mysql.a _mysql.o
	g++ $(CFLAGS) -o obtmindtodb obtmindtodb.cpp _mysql.o $(PUBINCL) $(PUBCPP) $(MYSQLINCL) $(MYSQLLIB) $(MYSQLLIBS) -lm -lc
	cp obtmindtodb ../bin/.

clean:
	rm -f crtsurfdata
	rm -f obtcodetodb obtmindtodb