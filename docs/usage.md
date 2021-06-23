# 用法

&nbsp;

## 安装

1. 下载编译好的可执行文件
2. 在home目录下创建simple-scm文件夹，并把可执行文件放进去
3. 将simple-scm文件夹添加到环境变量，并source一下
4. 安装完成！



## 编译安装

1. clone本项目的仓库到本地
2. 安装依赖项
3. 使用cmake进行编译
4. 得到可执行文件
5. 执行上述安装步骤

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

