/*
 *  程序名：ftpgetfiles.cpp，本程序是通用的功能模块，用于把远程ftp服务器的文件下载到本地目录
 *  作者：CarlYim
*/

#include "_public.h"
#include "_ftp.h"

// 帮助文档
void _help();

CLogFile logfile;

Cftp ftp;

struct st_arg
{
    char host[31];           // 远程服务器的IP和端口
    int  mode;               // 传输模式，1-被动模式，2-主动模式，缺省采用被动模式
    char username[31];       // 远程服务器ftp的用户名
    char password[31];       // 远程服务器ftp的密码
    char remotepath[301];    // 远程服务器存放文件的目录
    char localpath[301];     // 本地文件存放的目录
    char matchname[101];     // 待下载文件匹配的规则
    char listfilename[301];  // 下载前列出服务器文件名的文件
    int  ptype;              // 下载后服务器文件的处理方式：1-什么也不做；2-删除；3-备份
    char remotepathbak[301]; // 下载后服务器文件的备份目录
    char okfilename[301];    // 已下载成功文件名清单
    bool checkmtime;         // 是否需要检查服务端文件的时间，true-需要，false-不需要，缺省为false
    int  timeout;            // 进程心跳的超时时间
    char pname[51];          // 进程名，建议用"ftpgetfiles_后缀"的方式
} starg;

// 文件信息的结构体
struct st_fileinfo
{
  char filename[301];   // 文件名
  char mtime[21];       // 文件时间
};

// 已下载成功文件名的容器，从okfilename中加载
vector<struct st_fileinfo> vlistfile1;    

// 下载前列出服务器文件名的容器，从nlist文件中加载
vector<struct st_fileinfo> vlistfile2;    

// 本次不需要下载的文件的容器
vector<struct st_fileinfo> vlistfile3;    

// 本次需要下载的文件的容器
vector<struct st_fileinfo> vlistfile4;    

// 加载okfilename文件中的内容到容器vlistfile1中
bool LoadOKFile();

// 比较vlistfile2和vlistfile1，得到vlistfile3和vlistfile4
bool CompVector();

// 把容器vlistfile3中的内容写入okfilename文件，覆盖之前的旧okfilename文件
bool WriteToOKFile();

// 如果ptype==1，把下载成功的文件记录追加到okfilename文件中
bool AppendToOKFile(struct st_fileinfo *stfileinfo);

// 把xml解析到参数starg结构中
bool _xmltoarg(char *strxmlbuffer);

// 程序退出和信号2、15的处理函数
void EXIT(int sig);

// 下载文件功能的主函数
bool _ftpgetfiles();

// 把ftp.nlist()方法获取到的list文件加载到容器vlistfile2中
bool LoadListFile();

// 进程心跳
CPActive PActive;  

int main(int argc, char *argv[])
{
    // 小目标，把ftp服务上某目录中的文件下载到本地的目录中
    if (argc != 3) {
        _help();
        /*../bin/ftpgetfiles4 ../../idc/log/ftpgetfiles4.log "<host>127.0.0.1:21</host><mode>1</mode>
        <username>wydxry</username><password>1</password><remotepath>/home/wydxry/demo/idc/tmp/server</remotepath>
        <localpath>/home/wydxry/demo/idc/tmp/client</localpath><matchname>*</matchname><ptype>3</ptype>
        <remotepathbak>/home/wydxry/demo/idc/tmp/backup</remotepathbak>
        <okfilename>/home/wydxry/demo/idc/tmp/data/ftpgetfiles4_surfdata.xml</okfilename>
        <listfilename>/home/wydxry/demo/idc/tmp/ftplist/ftpgetfiles4_surfdata.list</listfilename>"
        */
        return -1;
    }

    // 关闭全部的信号和输入输出
    // 设置信号,在shell状态下可用 "kill + 进程号" 正常终止些进程
    // 但请不要用 "kill -9 +进程号" 强行终止
    // CloseIOAndSignal(true); 
    signal(SIGINT, EXIT);  
    signal(SIGTERM, EXIT);

    if (logfile.Open(argv[1], "a+") == false) {
        printf("打开日志文件失败（%s）。\n", argv[1]); 
        return -1;
    }

    // 解析xml，得到程序运行的参数。
    if (_xmltoarg(argv[2]) == false) return -1;

    // 把进程的心跳信息写入共享内存。
    PActive.AddPInfo(starg.timeout, starg.pname);  

    // 登录ftp服务器
    if (ftp.login(starg.host, starg.username, starg.password, starg.mode) == false) {
        logfile.Write("ftp.login(%s,%s,%s) failed.\n", starg.host, starg.username, starg.password); 
        return -1;
    }

    logfile.Write("ftp.login ok.\n");

    // 下载文件功能的主函数
    _ftpgetfiles();

    // 进入ftp服务器存放文件的目录

    // 调用ftp.nlist()方法列出服务器目录中的文件，结果存放到本地文件中

    // 把ftp.nlist()方法获取到的list文件加载到容器vfilelist中

    // 遍历容器vfilelist。

    /*
    for (int i=0;i<vlistfile.size();i++)
    {
        // 调用ftp.get()方法从服务器下载文件。
    }
    */

    ftp.logout();
    
    return 0;
}

void _help()
{
    printf("\n");
    printf("Using:/demo/tools/bin/ftpgetfiles logfilename xmlbuffer\n\n");

    printf("Sample:/demo/tools/bin/procctl 30 /demo/tools/bin/ftpgetfiles /demo/idc/log/ftpgetfiles_surfdata.log \"<host>127.0.0.1:21</host><mode>1</mode><username>wydxry</username><password>wydxry</password><localpath>/idcdata/surfdata</localpath><remotepath>/tmp/idc/surfdata</remotepath><matchname>SURF_ZH*.XML,SURF_ZH*.CSV</matchname><listfilename>/idcdata/ftplist/ftpgetfiles_surfdata.list</listfilename><ptype>3</ptype><remotepathbak>/tmp/idc/surfdatabak</remotepathbak><okfilename>/idcdata/ftplist/ftpgetfiles_surfdata.xml</okfilename>\"\n\n");

    printf("本程序是通用的功能模块，用于把远程ftp服务器的文件下载到本地目录。\n");
    printf("logfilename是本程序运行的日志文件。\n");
    printf("xmlbuffer为文件下载的参数，如下：\n");
    printf("<host>127.0.0.1:21</host> 远程服务器的IP和端口。\n");
    printf("<mode>1</mode> 传输模式，1-被动模式，2-主动模式，缺省采用被动模式。\n");
    printf("<username>wydxry</username> 远程服务器ftp的用户名。\n");
    printf("<password>wydxrypwd</password> 远程服务器ftp的密码。\n");
    printf("<remotepath>/tmp/idc/surfdata</remotepath> 远程服务器存放文件的目录。\n");
    printf("<localpath>/idcdata/surfdata</localpath> 本地文件存放的目录。\n");
    printf("<matchname>SURF_ZH*.XML,SURF_ZH*.CSV</matchname> 待下载文件匹配的规则。"\
            "不匹配的文件不会被下载，本字段尽可能设置精确，不建议用*匹配全部的文件。\n");
    printf("<listfilename>/idcdata/ftplist/ftpgetfiles_surfdata.list</listfilename> 下载前列出服务器文件名的文件。\n");
    printf("<ptype>1</ptype> 文件下载成功后，远程服务器文件的处理方式：1-什么也不做；2-删除；3-备份，如果为3，还要指定备份的目录。\n");
    printf("<remotepathbak>/tmp/idc/surfdatabak</remotepathbak> 文件下载成功后，服务器文件的备份目录，此参数只有当ptype=3时才有效。\n");
    printf("<okfilename>/idcdata/ftplist/ftpgetfiles_surfdata.xml</okfilename> 已下载成功文件名清单，此参数只有当ptype=1时才有效。\n");
    printf("<checkmtime>true</checkmtime> 是否需要检查服务端文件的时间，true-需要，false-不需要，此参数只有当ptype=1时才有效，缺省为false。\n");
    printf("<timeout>80</timeout> 下载文件超时时间，单位：秒，视文件大小和网络带宽而定。\n");
    printf("<pname>ftpgetfiles_surfdata</pname> 进程名，尽可能采用易懂的、与其它进程不同的名称，方便故障排查。\n\n\n");
}

// 把xml解析到参数starg结构中
bool _xmltoarg(char *strxmlbuffer)
{
    memset(&starg, 0, sizeof(struct st_arg));

    // 远程服务器的IP和端口
    GetXMLBuffer(strxmlbuffer, "host", starg.host, 30);   
    if (strlen(starg.host) == 0) { 
        logfile.Write("host is null.\n");  
        return false; 
    }

    // 传输模式，1-被动模式，2-主动模式，缺省采用被动模式
    GetXMLBuffer(strxmlbuffer, "mode", &starg.mode);   
    if (starg.mode != 2) {
        starg.mode=1;
    }

    // 远程服务器ftp的用户名
    GetXMLBuffer(strxmlbuffer, "username", starg.username, 30);  
    if (strlen(starg.username) == 0) {
        logfile.Write("username is null.\n");  
        return false; 
    } 

    // 远程服务器ftp的密码
    GetXMLBuffer(strxmlbuffer, "password", starg.password, 30);  
    if (strlen(starg.password) == 0) {
        logfile.Write("password is null.\n");  
        return false;
    }

    // 远程服务器存放文件的目录
    GetXMLBuffer(strxmlbuffer, "remotepath", starg.remotepath, 300);  
    if (strlen(starg.remotepath) == 0) {
        logfile.Write("remotepath is null.\n");  
        return false;
    }

    // 本地文件存放的目录
    GetXMLBuffer(strxmlbuffer, "localpath", starg.localpath, 300);   
    if (strlen(starg.localpath) == 0) {
        logfile.Write("localpath is null.\n");  
        return false;
    }

    // 待下载文件匹配的规则
    GetXMLBuffer(strxmlbuffer, "matchname", starg.matchname, 100);   
    if (strlen(starg.matchname) == 0) {
        logfile.Write("matchname is null.\n");  
        return false;
    }

    // 下载前列出服务器文件名的文件
    GetXMLBuffer(strxmlbuffer, "listfilename", starg.listfilename, 300);   
    if (strlen(starg.listfilename) == 0) {
        logfile.Write("listfilename is null.\n");  
        return false;
    }

    // 下载后服务器文件的处理方式：1-什么也不做；2-删除；3-备份
    GetXMLBuffer(strxmlbuffer, "ptype", &starg.ptype);   
    if ((starg.ptype != 1) && (starg.ptype != 2) && (starg.ptype != 3)) {
        logfile.Write("ptype is error.\n"); 
        return false;
    }

    // 下载后服务器文件的备份目录
    GetXMLBuffer(strxmlbuffer, "remotepathbak", starg.remotepathbak, 300);
    if ((starg.ptype == 3) && (strlen(starg.remotepathbak) == 0)) {
        logfile.Write("remotepathbak is null.\n");  
        return false;
    }

    // 已下载成功文件名清单
    GetXMLBuffer(strxmlbuffer, "okfilename", starg.okfilename, 300);
    if ((starg.ptype == 1) && (strlen(starg.okfilename) == 0)) {
        logfile.Write("okfilename is null.\n");  
        return false; 
    }

    // 是否需要检查服务端文件的时间，true-需要，false-不需要，此参数只有当ptype=1时才有效，缺省为false
    GetXMLBuffer(strxmlbuffer, "checkmtime", &starg.checkmtime);

    // 进程心跳的超时时间
    GetXMLBuffer(strxmlbuffer, "timeout", &starg.timeout);
    if (starg.timeout <= 0) {
        logfile.Write("timeout is null.\n");  
        return false;
    }

    // 进程名
    GetXMLBuffer(strxmlbuffer, "pname", starg.pname, 50); 
    if (strlen(starg.pname) == 0) {
        logfile.Write("pname is null.\n");  
        return false;
    }

    return true;
}

void EXIT(int sig)
{
    printf("程序退出，sig=%d\n\n",sig);

    exit(0);
}

// 下载文件功能的主函数
bool _ftpgetfiles() 
{
    // 进入ftp服务器存放文件的目录
    if (ftp.chdir(starg.remotepath) == false) {
        logfile.Write("ftp.chdir(%s) failed.\n", starg.remotepath); 
        return false;
    }

    // 调用ftp.nlist()方法列出服务器目录中的文件，结果存放到本地文件中
    if (ftp.nlist(".", starg.listfilename) == false) {
        logfile.Write("ftp.nlist(%s) failed.\n", starg.remotepath); return false;
    }
    
    // 更新进程的心跳
    PActive.UptATime();   

    // 把ftp.nlist()方法获取到的list文件加载到容器vlistfile中
    if (LoadListFile() == false) {
        logfile.Write("LoadListFile() failed.\n");  
        return false;
    }

    // 更新进程的心跳
    PActive.UptATime();   

    if (starg.ptype == 1) {
        // 加载okfilename文件中的内容到容器vlistfile1中
        LoadOKFile();

        // 比较vlistfile2和vlistfile1，得到vlistfile3和vlistfile4
        CompVector();

        // 把容器vlistfile3中的内容写入okfilename文件，覆盖之前的旧okfilename文件
        WriteToOKFile();

        // 把vlistfile4中的内容复制到vlistfile2中
        vlistfile2.clear(); 
        vlistfile2.swap(vlistfile4);
    }

    char strremotefilename[301], strlocalfilename[301];
    
    // 遍历容器vlistfile2
    for (int i = 0; i < vlistfile2.size(); i++) {
        SNPRINTF(strremotefilename, sizeof(strremotefilename), 300, "%s/%s", starg.remotepath, vlistfile2[i].filename);
        SNPRINTF(strlocalfilename, sizeof(strlocalfilename), 300, "%s/%s", starg.localpath, vlistfile2[i].filename);

        // 调用ftp.get()方法从服务器下载文件
        logfile.Write("get %s ...\n", strremotefilename);

        // 调用ftp.get()方法从服务器下载文件
        if (ftp.get(strremotefilename, strlocalfilename) == false) {
            logfile.WriteEx("failed.\n"); 
            break;
        }

        // 更新进程的心跳
        PActive.UptATime();   

        // 如果ptype==1，把下载成功的文件记录追加到okfilename文件中
        if (starg.ptype == 1) AppendToOKFile(&vlistfile2[i]);

        // 删除服务端的文件
        if (starg.ptype == 2) {
            if (ftp.ftpdelete(strremotefilename) == false) {
                logfile.Write("ftp.ftpdelete(%s) failed.\n", strremotefilename); 
                return false;
            }
        }

        // 转存到备份目录
        if (starg.ptype == 3) {
            char strremotefilenamebak[301];
            // 安全的snprintf函数。
            // 将可变参数(...)按照fmt描述的格式输出到dest字符串中。
            SNPRINTF(strremotefilenamebak, sizeof(strremotefilenamebak), 300, "%s/%s", starg.remotepathbak, vlistfile2[i].filename);
            if (ftp.ftprename(strremotefilename, strremotefilenamebak) == false) {
                logfile.Write("ftp.ftprename(%s,%s) failed.\n", strremotefilename, strremotefilenamebak); 
                return false;
            }
        }

    }

    logfile.Write("_ftpgetfiles() ok.\n");

    return true;
}

// 把ftp.nlist()方法获取到的list文件加载到容器vlistfile2中
bool LoadListFile() 
{
    vlistfile2.clear();
    
    CFile File;

    if (File.Open(starg.listfilename, "r") == false) {
        logfile.Write("File.Open(%s) 失败。\n", starg.listfilename); 
        return false;
    }

    struct st_fileinfo stfileinfo;

    while (1) {
        memset(&stfileinfo, 0, sizeof(struct st_fileinfo));

        if (File.Fgets(stfileinfo.filename, 300, true) == false) break;

        if (MatchStr(stfileinfo.filename, starg.matchname) == false) continue;

        if ((starg.ptype == 1) && (starg.checkmtime == true)) {
            // 获取ftp服务器上文件的时间
            if (ftp.mtime(stfileinfo.filename) == false) {
                logfile.Write("ftp.mtime(%s) failed.\n", stfileinfo.filename);
                return false;
            }

            // cout << ftp.m_mtime << endl;

            strcpy(stfileinfo.mtime, ftp.m_mtime);
        }
        
        vlistfile2.push_back(stfileinfo);
    }

    for (int i = 0; i < vlistfile2.size(); i++) {
        logfile.Write("filename = %s mtime = %s\n", vlistfile2[i].filename, vlistfile2[i].mtime);
    }

    return true;
}

// 加载okfilename文件中的内容到容器vlistfile1中
bool LoadOKFile() 
{
    vlistfile1.clear();

    CFile File;

    // 注意：如果程序是第一次下载，okfilename是不存在的，并不是错误，所以也返回true
    if ((File.Open(starg.okfilename, "r")) == false) return true;

    char strbuffer[501];

    struct st_fileinfo stfileinfo;

    while (1) {
        memset(&stfileinfo, 0, sizeof(struct st_fileinfo));

        if (File.Fgets(strbuffer, 300, true) == false) break;

        GetXMLBuffer(strbuffer, "filename", stfileinfo.filename);
        GetXMLBuffer(strbuffer, "mtime", stfileinfo.mtime);

        vlistfile1.push_back(stfileinfo);
    }   

    return true;
}

// 比较vlistfile2和vlistfile1，得到vlistfile3和vlistfile4
bool CompVector()
{
    vlistfile3.clear();
    vlistfile4.clear();

    for (int i = 0; i < vlistfile2.size(); i++) {
        bool flag = false;
        for (int j = 0; j < vlistfile1.size(); j++) {
            // 如果找到了，把记录放入vlistfile3
            if ((strcmp(vlistfile1[j].filename, vlistfile2[i].filename) == 0) && (strcmp(vlistfile1[j].mtime, vlistfile2[i].mtime) == 0)) {
                vlistfile3.push_back(vlistfile2[i]);
                flag = true;
                break;
            }
        }
        // 如果没有找到，把记录放入vlistfile4
        if (!flag) {
            vlistfile4.push_back(vlistfile2[i]);
        }
    }

    return true;
}

// 把容器vlistfile3中的内容写入okfilename文件，覆盖之前的旧okfilename文件
bool WriteToOKFile()
{
    CFile File;
    
    if (File.Open(starg.okfilename, "w") == false) {
        logfile.Write("File.Open(%s) failed.\n", starg.okfilename); 
        return false;
    }

    for (int i = 0; i < vlistfile3.size(); i++) {
        File.Fprintf("<filename>%s</filename><mtime>%s</mtime>\n", vlistfile3[i].filename, vlistfile3[i].mtime);
    }

    return true;
}

// 如果ptype==1，把下载成功的文件记录追加到okfilename文件中
bool AppendToOKFile(struct st_fileinfo *stfileinfo)
{
    CFile File;
    
    if (File.Open(starg.okfilename, "a") == false) {
        logfile.Write("File.Open(%s) failed.\n", starg.okfilename); 
        return false;
    }

    File.Fprintf("<filename>%s</filename><mtime>%s</mtime>\n", stfileinfo->filename, stfileinfo->mtime);

    return true;
}
