/***************************************************************/
/*Created by WangXiaoqiang.
/*CopyRight Reserved.
/****************************************************************/

#ifndef __KEY_H__
#define __KEY_H__

#define KEY1 (0 << 2) //GPF1��key1����Ȼ����Ϊ��������
#define KEY2 (0 << 8)
#define KEY3 (0 << 4)
#define KEY4 (0 << 0)

extern void Key_Init() ;
extern int Key_Scan() ;

#endif

