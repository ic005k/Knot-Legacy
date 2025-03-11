#ifndef QKEYCHAIN_EXPORT_H
#define QKEYCHAIN_EXPORT_H

#include <QtCore/qglobal.h>

// 静态链接模式：禁用导出/导入宏
#if defined(QTKEYCHAIN_STATIC)
    #define QKEYCHAIN_EXPORT
// 动态库编译时导出符号
#elif defined(QTKEYCHAIN_LIBRARY)
    #define QKEYCHAIN_EXPORT Q_DECL_EXPORT
// 动态库使用时导入符号
#else
    #define QKEYCHAIN_EXPORT Q_DECL_IMPORT
#endif

#endif // QKEYCHAIN_EXPORT_H