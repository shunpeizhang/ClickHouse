#pragma once

namespace ultradb
{
  /// ultradb 无符号char类型
  typedef unsigned char UdbUChar;
  /// ultradb char类型
  typedef char UdbChar;
  /// ultradb 无符号小整数类型
  typedef unsigned char UdbUInt8;
  /// ultradb 小整数类型
  typedef char UdbInt8;
  /// ultradb 无符号短整数类型
  typedef unsigned short UdbUInt16;
  /// ultradb 短整数类型
  typedef short UdbInt16;
  /// ultradb 无符号整数类型
  typedef unsigned int UdbUInt32;
  /// ultradb 整数类型
  typedef int UdbInt32;
  /// ultradb 无符号长整数
  typedef unsigned long int UdbUInt64;
  /// ultradb 长整数
  typedef long int UdbInt64;
  /// ultradb 单精度浮点数
  typedef float UdbFloat32;
  /// ultradb 双精度浮点数
  typedef double UdbFloat64;
  /// ultradb 日期和时间类型
  typedef unsigned int UdbDateTime;
  /// ultradb 日期类型
  typedef unsigned short UdbDate;
  /// ultradb Decimal类型 TODO:需要替换为真正类型
  typedef double UdbDecimal;


#pragma pack(1)
  /// ultradb 字符串
  struct UdbString
  {
    /// 字符串容量
    UdbUInt16 capacity;
    /// 字符串实际长度
    UdbUInt16 size;
    /// 字符串数据
    char data[0];
  };
#pragma pack()

#pragma pack(1)
  /// ultradb 长字符串
  struct UdbLongString
  {
    /// 字符串容量
    UdbUInt32 capacity;
    /// 字符串实际长度
    UdbUInt32 size;
    /// 字符串数据
    char data[0];
  };
#pragma pack()

/// UUID字段4长度
const unsigned short UUID_DATA4_SIZE = 8;

/// @brief
/// ultradb UUID类型
struct UdbUUID
{
  /// 数据段1
  UdbUInt32 Data1;
  /// 数据段2
  UdbUInt16 Data2;
  /// 数据段3
  UdbUInt16 Data3;
  /// 数据段4
  UdbUChar Data4[UUID_DATA4_SIZE];
};


}