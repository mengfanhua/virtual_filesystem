# virtual_filesystem

2020年6月11日 22点02分 author：meng

增添format函数，对原有数据结构做了部分修改。

2020年6月12日 20点05分 author：meng

增添init函数，对原有数据结构做了部分修改。

2020年6月13日 14点47分 author：meng

增添登录相关功能函数，对部分全局变量进行修改。

2020年6月14日 00点47分 author：meng

增添文件打开、保存和关闭函数；增添磁盘读写函数；增加i节点和数据区的分配与释放函数。

2020年6月14日 20点27分 author：meng

增添创建文件夹、创建文件、删除文件夹或文件（迭代式）函数。目前待实现的函数有：iget，iput，及dirfile.c文件中的默认授权函数、共享函数、权限检验函数等，目前不理解chdir函数具体要做什么，故暂时放置，修改了部分全局变量。

2020年6月15日 10点08分 author：meng

增添授权函数，权限检验函数；修改了权限的分配方法。

2020年6月17日 20点58分 author：meng

增加iget、iput函数；修改部分dir数据结构，同步修改其他函数对于dir的处理；更正了之前对于dinode连接数概念的误解，修复了连接数的更改方法；增加共享函数。