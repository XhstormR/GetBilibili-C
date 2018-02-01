# GetBilibili-C

## Build

构建本项目前需安装 Git、CMake、MinGW，生成的可执行文件存放在 `build/bin` 目录下。

```
git clone --recursive https://github.com/XhstormR/GetBilibili-C.git
make.bat
```

## Use

### Console

```
Usage: GetBilibili-C <command> [<switches>...]

<Commands>
  -l <url>            解析链接
  -d <url>            下载并合并
  -m                  只进行合并

<Switches>
  --dir <path>        设置下载文件夹
  --cookie <file>     导入 cookie 文件
  --delete            任务完成后自动删除分段视频
```

### GUI

![](https://i.loli.net/2018/02/01/5a7286d02ce1a.png)

## Note

* 传递给应用的链接格式应该是这样的：

  ```
  http://interface.bilibili.com/playurl?cid=11239800&appkey=84956560bc028eb7&otype=json&type=&quality=112&qn=112&sign=8f52ea2d50e0ae3b17d0ec2c182c4767
  ```

* Console 版本的 `--cookie` 参数需要将 Cookie 写入一个文件，然后传递文件路径。

  GUI 版本直接粘贴 Cookie 即可。

* 只在 Windows 10 x64 平台下进行过测试。
