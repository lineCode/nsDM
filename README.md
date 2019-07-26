# NSDM
 请从[REDM](https://gitee.com/hgy413/REDM)中导入DMMain库文件到DMMain目录
 

##1. NSDM
NSDM是基于[REDM](https://gitee.com/hgy413/REDM)写的NSIS安装打包工具，此代码工程由lniwn大神提供，本人仅为搬运工

## 2.使用方法
### 2.1 目录结构
`nsdm`  
| -- `3rdParty`  
|-----------| ----------` common ` ==>工具  
|-----------| --------- `easy_libcurl`==> 封装的libcurl库  
|-----------| --------- `nsis`==>NSIS脚本接口  
| -- `nsCurl`==>NSIS脚本用的libcurl插件  
| -- `nsDM`==>NSIS脚本用的DM插件  
| -- `nsUtil`==>NSIS脚本用的工具插件  
| -- `Script`==>通用NSIS脚本  
| -- `publish`==>插件发布目录  
| -- `packages`==>实际项目目录  
            | -- [项目名]  
                     | --------- `addon` ==>需要提前运行/释放的东西，比如license文件  
                     | --------- `product` ==>实际项目目录，目前固定为`product`.7z  
                     | --------- `script` ==>安装脚本  
                     | --------- `skin` ==>UI库使用的配置文件,可以使用设计器  
					 
### 2.2 安装说明
NSIS安装包
`NSIS_v251_Build_20160601%20-%20NSISFANS.exe`  
V2.51 ANSI版本  
注：安装包自带的NSIS/Plugins/NSIS7z.dll版本较旧，需要使用当前目录下的`NSIS7z.dll`替换掉。

重要的第三方工具  
http://muldersoft.com/docs/stdutils_readme.html

### 2.3 脚本结构说明  
TODO

### 2.4 构建步骤说明  
0.  使用的是`publish`目录下的三个dll, 每次编译完，会拷贝三个dll到`publish`目录。  
1.	生成资源包`skin.zip`。用设计器配置完资源图，双击`skin/zip.bat`即可生成。
2.	生成`product.7z`目标文件。将需要释放的文件打包为`product.7z`，放在product文件夹下即可。
3.	生成卸载程序。执行script/uninstall.nsi(工具栏:NSIS编译并运行)，生成uninst_build.exe文件，双击`uninst_build.exe`，会在addon文件下生成`Uninstall.exe`，需要自己手工签名。
4.	生成安装包。执行script/setup.nsi(工具栏:NSIS编译并运行)，会在script目录下生成目标安装包。
