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
CFLAGS = -g -no-pie

all: procctl gzipfiles deletefiles ftpgetfiles ftpputfiles \
	tcpputfiles fileserver tcpgetfiles

procctl: procctl.cpp
	g++ -o procctl procctl.cpp
	cp procctl ../bin/.

gzipfiles: gzipfiles.cpp
	g++ $(CFLAGS) -o gzipfiles gzipfiles.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp gzipfiles ../bin/.

deletefiles: deletefiles.cpp
	g++ $(CFLAGS) -o deletefiles deletefiles.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp deletefiles ../bin/.

ftpgetfiles: ftpgetfiles.cpp
	g++ $(CFLAGS) -o ftpgetfiles ftpgetfiles.cpp $(PUBINCL) $(PUBCPP) /home/ubuntu/MDMC/project/public/libftp.a /home/ubuntu/MDMC/project/public/_ftp.cpp -lm -lc
	cp ftpgetfiles ../bin/.

ftpputfiles: ftpputfiles.cpp
	g++ $(CFLAGS) -o ftpputfiles ftpputfiles.cpp $(PUBINCL) $(PUBCPP) /home/ubuntu/MDMC/project/public/libftp.a /home/ubuntu/MDMC/project/public/_ftp.cpp -lm -lc
	cp ftpputfiles ../bin/.

tcpputfiles: tcpputfiles.cpp
	g++ $(CFLAGS) -o tcpputfiles tcpputfiles.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp tcpputfiles ../bin/.

fileserver: fileserver.cpp
	g++ $(CFLAGS) -o fileserver fileserver.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp fileserver ../bin/.

tcpgetfiles: tcpgetfiles.cpp
	g++ $(CFLAGS) -o tcpgetfiles tcpgetfiles.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp tcpgetfiles ../bin/.

clean:
	rm -f procctl gzipfiles deletefiles 
	rm -f ftpgetfiles ftpputfiles
	rm -f tcpputfiles fileserver tcpgetfiles