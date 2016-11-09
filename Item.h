#ifndef ITEM_H
#define ITEM_H
#include "struct.h"


/*========================================================================
        Name:		。
    ----------------------------------------------------------
        returns:	返回城市的编码。
    ----------------------------------------------------------
        Remarks:
    ----------------------------------------------------------
        Params:         strCity 城市的名称
==========================================================================*/

//插入到报警链表
static ItemAlarm InsertAlarmItem(SAlarmDevice data,NodeAlarm *ItemAlarm)//头插法
{
    NodeAlarm *q;
    q = (NodeAlarm *)malloc(sizeof(NodeAlarm));
    q->data = data;
    q->next = ItemAlarm;
    ItemAlarm = q;
    return ItemAlarm;
}
//更改设备的报警状态
static int FindItemDevice(SAlarmDevice *data,NodeDevice *ItemDevice)
{
    NodeDevice *q;
    q = ItemDevice;

    while(q->next)
    {
        if(memcmp(q->data.gcIntraAddr,(*data).gcIntraAddr,20) ==0 &&
           memcmp(q->data.gcMacAddr,(*data).gcMacAddr,20) ==0 &&
           (q->data.nState != 0))
        {
            q->data.nAlarmState = 1;
            memcpy((*data).gcType,q->data.gcType,20);
            (*data).nTenementId = q->data.nId;
            return 0;
        }
        q = q->next;
    }
    return 1;
}
//查找报警是否已存在链表
static int FindAlarmItem(SAlarmDevice data,NodeAlarm *ItemAlarm)
{
    NodeAlarm *t;
    t = ItemAlarm;
    while(t->next)
    {
        if(memcmp(t->data.gcIntraAddr,data.gcIntraAddr,20) ==0 &&
                memcmp(t->data.gcMacAddr,data.gcMacAddr,20) ==0 &&
                t->data.nFenceId == data.nFenceId &&
                t->data.nAlarmType == data.nAlarmType)
        {
            return 0;
        }
        else
            t = t->next;
    }
    return 1;
}

////删除指定住户设备
//static ItemDevice DeleteItemDevice(SDevice data,NodeDevice **tItemDevice)
//{
//    NodeDevice *p,*q;

//    q = (NodeDevice *)malloc(sizeof(NodeDevice));
//    p = *tItemDevice;
//    if(memcmp(p->data.gcMacAddr,data.gcMacAddr,20) == 0)
//    {
//        q = p;
//        p = p->next;
//        delete(q);
//        return p;
//    }
//    while(p->next != NULL)              //查找值为x的元素
//    {
//        if(memcmp(p->next->data.gcMacAddr,data.gcMacAddr,20) == 0)
//        {
//            q = p->next;         //删除操作，将其前驱next指向其后继。
//            p->next = q->next;
//            delete(q);
//            return *tItemDevice;
//        }
//        q = p;
//        p = p->next;
//    }
//    return *tItemDevice;
//}

//更新住户设备链表的报警状态值
static ItemDevice AlarmUpdateDeviceItem(SAlarmDevice data,NodeDevice *ItemDevice)
{
    NodeDevice *q;
    q = ItemDevice;
    while(q)
    {
        if(memcmp(q->data.gcIntraAddr,data.gcIntraAddr,20) ==0 &&
                memcmp(q->data.gcMacAddr,data.gcMacAddr,20)== 0)
        {
            q->data.nAlarmState = 0;
            return ItemDevice;
        }
        q = q->next;
    }
    return ItemDevice;
}

//查找设置是否存在报警链表
static int AddrFindAlarmItem(SAlarmDevice data,NodeAlarm *ItemAlarm)
{
    NodeAlarm *t;
    t = ItemAlarm;
    while(t->next)
    {
        if(memcmp(t->data.gcIntraAddr,data.gcIntraAddr,20) ==0 &&
                memcmp(t->data.gcMacAddr,data.gcMacAddr,20) ==0
                )
        {
            return 0;   //假
        }else
            t = t->next;
    }
    return 1;   //真
}

//报警设备删除
static ItemAlarm DeleteItemAlarm(SAlarmDevice *data,NodeAlarm **ItemAlarm)
{
    NodeAlarm *p,*q;
    q = (NodeAlarm *)malloc(sizeof(NodeAlarm));
    p = *ItemAlarm;
    if(memcmp(p->data.gcStime,(*data).gcStime,20) == 0)
    {
        memcpy((*data).gcIntraAddr,p->data.gcIntraAddr,20);
        memcpy((*data).gcIpAddr,p->data.gcIpAddr,20);
        memcpy((*data).gcMacAddr,p->data.gcMacAddr,20);
        (*data).nTenementId = p->data.nTenementId;
        (*data).nAlarmType = p->data.nAlarmType;
        (*data).nFenceId = p->data.nFenceId;
        q = p;
        p = p->next;
        delete(q);
        return p;
    }
    while(p->next != NULL)              //查找值为x的元素
    {
        if(memcmp(p->data.gcStime, (*data).gcStime,20) == 0)
        {
            memcpy((*data).gcIntraAddr,p->data.gcIntraAddr,20);
            memcpy((*data).gcIpAddr,p->data.gcIpAddr,20);
            memcpy((*data).gcMacAddr,p->data.gcMacAddr,20);
            (*data).nTenementId = p->data.nTenementId;
            (*data).nAlarmType = p->data.nAlarmType;
            (*data).nFenceId = p->data.nFenceId;
            q = p->next;         //删除操作，将其前驱next指向其后继。
            p->next = q->next;
            delete(q);
            return *ItemAlarm;
        }
        q = p;  //保存作用
        p = p->next;
    }
    return *ItemAlarm;
}

//查找报警链表里是否存在该报警
static int FindItemAlarm(SAlarmDevice data,NodeAlarm *ItemAlarm)
{
    NodeAlarm *p;
    p = ItemAlarm;
    while(p->next != NULL)              //查找值为x的元素
    {
        if(memcmp(p->data.gcStime, data.gcStime,20) == 0)
        {
            return 1;
        }
        p = p->next;
    }
    return 0;
}
//更新报警链表
static int UpdataItemAlarm(SAlarmDevice data,NodeAlarm *ItemAlarm)
{
    NodeAlarm *q;
    q = ItemAlarm;
    int ret = 0;
    while(q->next)
    {
        if(memcmp(q->data.gcIntraAddr,data.gcIntraAddr,20) ==0 &&
           memcmp(q->data.gcIpAddr,data.gcIpAddr,20) ==0)
        {
            memcpy(q->data.gcEtime,data.gcEtime,20);    //更新报警结束时间
            ret++;
        }
        q = q->next;
    }
    return ret;
}

//创建链表，头插法插入节点1010
static ItemDevice ItemDeviceCreatH(SDevice data,NodeDevice *tItemDevice)
{
    NodeDevice *q;
    q = (NodeDevice *)malloc(sizeof(NodeDevice));
    q->data = data;
    q->next = tItemDevice;
    tItemDevice = q;
    return tItemDevice;
}

static int ItemDeviceFind(SDevice *data,NodeDevice *tItemDevice)
{
    NodeDevice *q;      //data是需要查找的设备      tItenDevice是设备链
    q = tItemDevice;
    while(q->next)
    {
        int ti = memcmp(q->data.gcMacAddr,(*data).gcMacAddr,20);
        if( ti == 0)
        {
            (*data).nId = q->data.nId;
            (*data).nState = q->data.nState;    //连接状态
            if(memcmp(q->data.gcIntraAddr,(*data).gcIntraAddr,20) != 0 )
            {
                return -3;      //更新
            }
            else
            {
                //IP相同或者未连接
                if(memcmp(q->data.gcIpAddr,(*data).gcIpAddr,20) || (q->data.nState <= 0) )
                {
                    (*data).nState = ONLINE_TIME;   //15
                    return q->data.nId;
                }
                else
                {
                    q->data.nState = ONLINE_TIME;   //15
                    return -2;
                }
            }
        }
        q = q->next;
    }
    return -1;//插入
}


//删除中间设备
static ItemDevice DeleteItemDevice(SDevice data,NodeDevice **tItemDevice)
{
    NodeDevice *p,*q;
    q = (NodeDevice *)malloc(sizeof(NodeDevice));
    p = *tItemDevice;
    if(memcmp(p->data.gcMacAddr,data.gcMacAddr,20) == 0)
    {
        q = p;
        p = p->next;
        delete(q);
        return p;
    }
    while(p->next != NULL)              //查找值为x的元素
    {
        if(memcmp(p->next->data.gcMacAddr,data.gcMacAddr,17) == 0)
        {
            q = p->next;         //删除操作，将其前驱next指向其后继。
            p->next = q->next;
            delete(q);
            return *tItemDevice;
        }
        q = p;
        p = p->next;
    }
    return *tItemDevice;
}

//插入新的中间设备
static ItemDevice InsertItemDevice(SDevice data, NodeDevice *tItemDevice)
{
    NodeDevice *p;              //data是插入设备，tItemDevice是设备链
    p = tItemDevice;
    data.nState = ONLINE_TIME;//设备在线
    if(p->next == NULL)
    {
        NodeDevice *q;
        q = (NodeDevice *)malloc(sizeof(NodeDevice));
        q->data = data;
        q->next = p;
        p = q;
        tItemDevice = p;      //tItemDevice = q
        return tItemDevice;
    }
    if(strncmp(p->data.gcIntraAddr,data.gcIntraAddr,20) > 0)
    {
        NodeDevice *q;
        q = (NodeDevice *)malloc(sizeof(NodeDevice));
        q->data = data;
        q->next = p;
        p = q;
        tItemDevice = p;
        return tItemDevice;
    }
    while(p->next)
    {
       if(strncmp(p->next->data.gcIntraAddr,data.gcIntraAddr,20) > 0)
       {
           NodeDevice *q;
           q = (NodeDevice *)malloc(sizeof(NodeDevice));
           q->data = data;
           q->next = p->next;
           p->next = q;
           return tItemDevice;
       }
       else
           p = p->next;
    }
    NodeDevice *q;
    q = (NodeDevice *)malloc(sizeof(NodeDevice));
    q->data = p->data;
    q->next = NULL;
    p->data = data;
    p->next = q;
    return tItemDevice;
}

//更新中间设备信息
static ItemDevice UpdateItemDevice(SDevice data,NodeDevice *tItemDevice)
{
    NodeDevice *q;
    q = tItemDevice;        //tItemDevice设备链
    while(q)
    {
        if(memcmp(q->data.gcMacAddr,data.gcMacAddr,20) == 0)//物理地址相同，表示是同一台设备
        {
            memcpy(q->data.gcAddr,data.gcAddr,45);
            memcpy(q->data.gcIntraAddr,data.gcIntraAddr,20);
            memcpy(q->data.gcIpAddr,data.gcIpAddr,20);
            q->data.nState = ONLINE_TIME;   //15
            return tItemDevice;
        }
        q = q->next;
    }
    return tItemDevice;     //没有相同的，则原样返回
}

#endif // ITEM_H
