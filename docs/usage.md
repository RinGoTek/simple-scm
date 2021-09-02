# 用法

&nbsp;

## 安装

1. 安装Python3，并设置其启动命令为python、pip
2. 单击 [下载安装包](https://github.com/We-are-ACMen/simple-scm/releases)
3. 进入安装包所在文件夹，打开终端。
4. 输入`sh simple-scm_0.1.2_amd64.sh`
5. 稍等片刻即可安装完成
6. 若您是首次安装，或从V0.1.1升级而来，请按照安装程序的指引，在`~/.bashrc`中添加环境变量
7. [老用户注意]本次升级请删除旧版本所在文件夹的环境变量，否则将产生冲突～
8. 安装完成！



## 编译安装

1. clone本项目的仓库到本地
2. 安装依赖项
3. 使用cmake进行编译
4. 得到可执行文件
5. 安装到自定义的文件夹并设置环境变量

&nbsp;

## 运行

在要使用simple-scm来进行代码管理的目录下，运行：

```
simple-scm
```

查看用法：

```
simple-scm --usage
```

初始化存储库：

```
simple-scm init
```

从当前节点创建新分支：

```
simple-scm new-branch <BranchName>
```

显示分支列表：

```
simple-scm list
```

忽略路径：

```
simple-scm ignore <path>
```

取消忽略路径：

```
simple-scm ignore -d <path>
```

添加路径到AddList：

```
simple-scm add <path>
```


提交本地内容至新节点：

```
simple-scm commit <Message>
```

切换分支

```
simple-scm checkout <branch_name>
```

切换到指定节点

```
simple-scm checkout -n <sha1>
```

将当前分支的头节点reset为指定节点

```
simple-scm reset <sha1>
```

将另一个节点合并入当前分支的头节点

```
simple-scm merge <sha1>
```

编辑提交文本

```
simple-scm edit -n <SHA1> -t <TEXT>  
```

迁移存储库版本（当存储库版本过低时）

```
simple-scm migrate
```

查询当前版本号

```
simple-scm -V   
```

显示软件信息

```
simple-scm about
```

检查软件更新

```
simple-scm update
```

