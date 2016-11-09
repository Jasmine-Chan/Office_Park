#ifndef STRUCT_H
#define STRUCT_H
#include <QtGui>

typedef unsigned long long	U64;
typedef signed long long	I64;
typedef unsigned long		U32;
typedef signed long		I32;
typedef unsigned short 		U16;
typedef signed short 		I16;
typedef          char		I8;
typedef unsigned char		U8;
#define RECV_SIZE 8100
#define ONLINE_TIME 15
#define ASK_MESSAGE_TIME 15
#define CARD_MAXLEN 5

struct SList
{
    int Id;
    QString strNo;//编号
    QString strName;//姓名
};

struct SList1
{
    int Id;
    QString strName;//姓名
};

struct SFullCard
{
    QString strNo;  //编号
    QString strName;    //姓名
    QDate   cardDate;   //补卡日期
    QString strType;    //补卡类型
};

struct SUser
{
    QString strName;//姓名
    QString strUser;//用户名
    QString strPass;//密码
    QString strCity;//城市
    QString strCityNum;//城市编码
    U16 nGrade;//权限
};

struct SStaff
{
    U16 nSex;               //性别
    U16 nNation;            //民族
    U16 nPolitics;          //政治面貌
    U16 nEducation;         //学历
    U16 nMaritalStatus;     //婚姻状态
    QString strPhone;       //电话
    QString strNo;          //员工编号
    QString strName;        //姓名
    QString strDepartment;  //所属部门
    QString strPost;        //职位
    QString strEnter;       //入职时间
    QString strIdCard;      //身份证号
    QString strNative;      //籍贯
    QString strBirthday;    //生日
    QString strGraduationDate;//毕业时间
    QString strEmail;       //邮箱
    QString strAdd;         //住址
    QString strGraduationSchool;//学校
    QString strMajor;       //专业
    QString strRemark;      //备注
    QString strImagePath;   //头像路径
};

struct SWeather
{
    QString strName;
    QString strWindDirection;   //风向
    U16 nWeather;       //天气, 1晴, 2多云, 3雨, 4雪
    U16 nWind;          //风力
    I16 nMinTemp;       //最低温
    I16 nMaxTemp;       //最高温
    I16 nNowTemp;       //现在温度
    U16 nHumidity;      //湿度
};

struct SMiddleDevice
{
    U16 nCheckedState:1;//选中状态
    U16 nAskTime:3;
    U16 nState:4;//连接状态
    U16 nCardState;//ic卡下发状态  0为初始值 1为需要下发 2为已经下发成功
    U32 nId;
    I8 gcIntraAddr[20];//内部地址
    I8 gcOldAddr[20];//用来判断是否添加新字段到开门权限
    I8 gcAddr[20];//地址
    I8 gcMacAddr[20];//网卡地址
    I8 gcIpAddr[20];//IP地址
    I8 gcType[20];//终端类型
    I8 gcAddrExplain[64];//地址说明
};

Q_DECLARE_METATYPE(SMiddleDevice);

struct SAlarmDevice
{
    U16 nAlarmType:4;//报警类型
    U16 nFenceId:4;//报警防区
    U32 nId;//编号
    U32 nTenementId;//对应在线表ID
    I8 gcIntraAddr[20];//内部地址
    I8 gcPhone1[20];//电话
    I8 gcMacAddr[20];//网卡地址
    I8 gcIpAddr[20];//IP地址
    I8 gcStime[20];
    I8 gcEtime[20];
    I8 gcType[20];
    I8 gcAddrExplain[64];
};

struct SAlarmDeal
{
    QString QAlarmIPAdd;
    QString QAlarmAdd;
    QString QAlarmNo;
    QString QAlarmDeviceType;
    QString QAlarmStime;
    QString QAlarmEtime;
};

struct SDevice
{
    U16 nAlarmState:1;//报警状态
    U16 nFenceState:2;//布防状态
    U16 nOldFenceState:2;
    U16 nState:4;//连接状态
    U32 nId;//编号
    I8  gcIntraAddr[20];//内部地址
    I8  gcMacAddr[20];//网卡地址
    I8  gcIpAddr[20];//IP地址
    I8  gcType[20];//终端类型
    I8  gcAddr[45];//地址
};

Q_DECLARE_METATYPE(SDevice);

typedef struct NodeAlarm
{
    SAlarmDevice data;              //单链表中的数据域
    struct NodeAlarm *next;          //单链表的指针域
}NodeAlarm,*ItemAlarm;

typedef struct NodeMiddle
{
    SMiddleDevice data;              //单链表中的数据域
    struct NodeMiddle *next;          //单链表的指针域
}NodeMiddle,*ItemMiddle;

typedef struct NodeDevice
{
    SDevice data;                     //单链表中的数据域
    struct NodeDevice *next;          //单链表的指针域
}NodeDevice,*ItemDevice;

#endif // STRUCT_H
