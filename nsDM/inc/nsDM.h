#pragma once

#include "nsDefine.h"

/*!
 * \brief
 * 参数1：skin路径
 * 参数2：主窗口的xml id
 * 参数3：主窗口控件名
 *
 * \note
 * 初始化GUI模块
 *
 * \return 主窗口句柄
 */
NSISFUNC(NSInitSkinEngine);

/*!
 * \brief
 * 参数1：control名字
 *
 * \note
 * 寻找指定的control
 *
 * \return 找到返回控件句柄，否则返回0
 */
NSISFUNC(NSFindControl);

/*!
 * \brief
 * 参数1：control的名字
 * 参数2：回调函数地址
 *
 * \note
 * 为控件绑定通知响应函数
 */
NSISFUNC(NSRegisterCommandNotify);

/*!
 * \brief
 * 参数1：弹出框xml id
 * 参数2：标题控件id
 * 参数3：标题内容
 * 参数4：内容控件id
 * 参数5：协议文件路径
 *
 * \note
 * 显示协议窗口
 */
NSISFUNC(NSShowLicense);

/*!
 * \brief
 * 参数1：标题
 *
 * \note
 * 显示文件选择框
 * 
 * \return 点击确定，返回用户选择的路径，否则返回0
 */
NSISFUNC(NSSelectFolderDialog);

/*!
 * \brief
 * 参数1：control的名字
 * 参数2：赋给control的数据
 * 参数3：数据类型(1. DM库支持的属性; 2. 插件支持的属性 )
 *
 * \note
 * 设置控件属性
 */
NSISFUNC(NSSetControlAttr);

/*!
 * \brief
 * 参数1：control的名字
 * 参数2：数据的类型 (1. text; 2. check)
 *
 * \note
 * 获取控件属性
 *
 * \return 控件属性
 */
NSISFUNC(NSGetControlAttr);

/*!
 * \brief
 * 参数1：赋给窗口的数据
 * 参数2：数据类型(1. DM库支持的属性; 2. 插件支持的属性 )
 *
 * \note
 * 设置主窗口属性
 */
NSISFUNC(NSSetWindowAttr);

/*!
 * \brief
 * 参数1：数据的类型
 *
 * \note
 * 获取主窗口属性
 *
 * \return 主窗口属性
 */
NSISFUNC(NSGetWindowAttr);

/*!
 * \brief
 * 参数1：TimerID
 * 参数2：interval
 *
 * \note
 * 创建定时器
 */
NSISFUNC(NSCreateTimer);

/*!
 * \brief
 * 参数1：TimerID
 *
 * \note
 * 删除定时器
 */
NSISFUNC(NSKillTimer);

/*!
 * \brief
 * 参数1：消息ID(字符串)
 * 参数2：WPARAM
 * 参数3：LPARAM
 *
 * \note
 * 发消息
 */
NSISFUNC(NSSendMessage);

/*!
 * \brief
 * 无参数
 *
 * \note
 * 显示界面，进入消息循环（注意：一定是最后才Show出来）
 */
NSISFUNC(NSRun);

/*!
 * \brief
 * 无参数
 *
 * \note
 * 退出安装
 */
NSISFUNC(NSUninitSkinEngine);

/*!
 * \brief
 * 参数1：弹出框xml id
 * 参数2：标题控件id
 * 参数3：标题内容
 * 参数4：内容控件id
 * 参数5：内容
 * 参数6：类型(MBOK, MBOKCANCEL ...)
 *
 * \note
 * 弹出MessageBox
 *
 * \return 返回用户点击的按钮ID (IDOK, IDCANCEL, IDCLOSE, IDRETRY ...)
 */
NSISFUNC(NSMessageBox);

/*!
 * \brief
 * 参数1：要检测的字符串
 *
 * \note
 * 检测字符串是否合法，只能包含于[a-zA-Z0-9_]
 *
 * \return 检测结果
 */
NSISFUNC(NSVerifyCharaters);

/*!
 * \brief
 * 参数1：页面名字
 *
 * \note
 * 跳转到指定页面
 *
 */
NSISFUNC(NSSelectPage);

/*!
 * \brief
 *
 * \note
 * 跳转到上一页(页面顺序在xml文件中指定)
 *
 */
NSISFUNC(NSPrevPage);

/*!
 * \brief
 *
 * \note
 * 跳转到下一页(页面顺序在xml文件中指定)
 *
 */
NSISFUNC(NSNextPage);