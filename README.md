# myler2
多功能命令行音乐播放器
<h3>一 项目介绍</h4>
                    <p>
                        命令行音乐播放器：这是一款基于命令行的，可连网的音乐播放器，它有以下功能：<br>
                        　　1. 音乐播放功能： <br><strong>
                        　　　　命令行参数指定音乐文件，便可播放。支持MP3格式的音频。<br></strong>
                        　　2. 歌词显示功能：<br><strong>
                        　　　　如果存在同名的lrc格式的歌词文件，会在主界面滚动显示当前歌词。</strong><br>
                        　　3. 歌单管理功能：<br><strong>
                        　　　　由命令行参数指定的所有歌曲会被建立一个“临时歌单”；<br>
                        　　　　LocalMusic文件夹里面的所有歌曲会被自动添加到“本地歌单”；</strong><br>
                        　　4. 对歌单/歌曲的基本控制：<br><strong>
                        　　　　支持上一曲，下一曲，暂停/继续，快进/快退，音量调节，重新播放；<br>
                        　　　　支持顺序播放，列表循环，随机播放，单曲循环；<br>
                        　　　　支持播放完成后自动退出；</strong><br>
                        　　5. 网络功能：<br><strong>
                        　　　　支持在线搜索歌曲，下载歌曲；<br>
                        　　　　支持酷狗音乐，QQ音乐，网易云音乐三大搜索引擎，能够下载大部分付费音乐以及歌词文件；<br>
                        　　　　支持歌曲名搜索，歌单搜索，歌词搜索；</strong><br>
                        　　6. 界面管理：<br><strong>
                        　　　　界面上显示软件名，歌单列表，歌词，播放进度条，搜索框等；<br>
                        　　　　支持隐藏一些部件；<br>
                        　　　　支持固定位置的错误消息显示；<br>
                        　　　　支持彩色显示，也可以取消颜色，采用终端默认字符颜色；<br>
                        　　　　支持自定义终端编码，支持GBK编码和UTF-8编码，防止终端乱码。></strong><br>
                        　　7. 按键支持：<br><strong>
                        　　　　可以通过方向键选择歌单中的歌曲，通过各种按键控制播放器。</strong><br>
                        　　8. 命令行参数支持：<strong><br>
                        　　　　命令行参数可以控制播放器的各种播放行为，用于支持自动管理。</strong>
                    </p>
                    <h3>二 项目特点</h4>
                    <p>这是一个用于C语言练手的项目，所以，在实现这个项目的时候，我对自己的要求是：<br><strong>
                    　　1. 用标准C语言(C99)实现它<br>
                    　　2. 不调用第三方库，仅用操作系统提供的底层API和C标准库，自己造轮子，完成这个项目<br>
                    　　3. 跨平台，支持Windows(Win7, Win10) Linux(Ubuntu 16.04+)<br>
                    　　4. 不依赖GUI，基于命令行/终端</strong><br>
                    </p>
                    <h3>三 项目编译</h3>
                    <p>1. Linux下：<br>
                    　　进入终端，在源码顶层目录下执行: make linux<br>
                    　　(系统里需要有GNU工具链，至少包含gcc和make))<br>
                    2.Windows下：<br>
                    　　方案一：进入cmd(或者Power Shell)，在源码顶层目录下执行: make windows<br>
                    　　(系统里需要有GNU工具链，如Mingw，Cygwin或者msys2，至少包含gcc和make，且在PATH目录下，如果是mingw，命令可能是：mingw-make windows))<br>
                    　　方案二：进入cmd(或者Power Shell)，在源码顶层目录下执行: gcc windows/*.c *.c -o myler.exe -I. -Wall -std=c99 -lwinmm -lws2_32<br>
                    　　(系统里需要有gcc，且在PATH目录下)<br>
                    　　方案三：用C/C++ IDE（如VC6.0 VS2017 dev-c++ codeblocks等）创建一个C工程，将顶层目录和windows目录的的所有C文件和头文件添加到工程中，将顶层目录添加到include路径中，然后编译。
                    </p>
                    <h3>更多信息</h3>
                    <p>参见<a href="http://39.108.3.243/myler/index.html">http://39.108.3.243/myler/index.html</a></p>

