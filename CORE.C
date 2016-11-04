/*****************************************************************************/
/*       FileName  :   CORE.C                                                */
/*       Content   :   DSA-208 CORE Module                                   */
/*       Date      :   Fri  02-22-2002                                       */
/*                     DSASoftWare(c)                                        */
/*                     CopyRight 2002             DSA-GROUP                  */
/*****************************************************************************/




#pragma  regconserve

#include _SFR_H_
#include _FUNCS_H_

#include "common.h"
#include "device.h"
#include "reg.h"
#include "ram.h"
#include "rom.h"
#include "comuse.h"





/************************************************/
/* Core_ObjectNo_To_UnitAddr      function      */
/************************************************/
//-�ж�ң�ض�����Ƿ����,ң�غ����õ�ԭ����:���й���ͨѶ�������ϵ��豸����Ψһ��
//-ң�غ�,ͨ���������žͿ����������ԺͶ˿ں�
/*===========================================================================*/
BYTE Ex_YK_CDTBB_ObjectNo_To_UnitAddr(void)
{
  unsigned short temp_rnd,temp_rnd1;

  for(temp_rnd=0;temp_rnd<14;temp_rnd++)	//-�������ҵ�ԭ�����ĳ�黯����
  {
      if(  (exchange_buf[5] >= Ex_YK_start[temp_rnd])
         &&(exchange_buf[5] <= Ex_YK_end[temp_rnd]) )	//-��������Ҫ�������ң�غ�,���뻥��,��ÿ�����Ա���
      {	//-���ȫ�����׵Ļ�,�����Ӧ���Ѿ��ҵ���ң�ص��豸���ڵĶ˿ں�
          for(temp_rnd1  = byte0(port_info[temp_rnd].mirror_unit);
              temp_rnd1 <= byte1(port_info[temp_rnd].mirror_unit);
              temp_rnd1++ )
          {	//-���ҵ��Ķ˿���,��һ�εļ������˿ڵ������������õ�Ԫ
              if(   (exchange_buf[5] >= unit_info[temp_rnd1].yk_start)
                  &&(exchange_buf[5] < (unit_info[temp_rnd1].yk_start+unit_info[temp_rnd1].yk_num))
                )
              {	//-ֻ�еõ��Ķ����,������һ��Ԫ�ط�Χ��,�Ż�����ȷ��ң�غź��ն�װ��������ʵ�ʵ�ַ
                  exchange_buf[17] = exchange_buf[5] - unit_info[temp_rnd1].yk_start;     // yk_no in unit
                  exchange_buf[5]  = temp_rnd1-byte0(port_info[temp_rnd].mirror_unit);    // unit addr
                  exchange_target_port=temp_rnd;	//-����ط�ʹ����һ����Ϊ�򵥵ķ���,������ӳ�䱨�ķ���,����ֱ�Ӱѱ��Ĵ洢��ӳ��ڵ�Ԫ,û���ȷ��ڱ�ͨ����Ԫ��
                  return YES;
              }
          }
      }
  }
  
  return NO;
}



/************************************************/
/* Ex_Produce_Transmit_Info     function        */
/* Input :    exchange_buf[xxx]                 */
/* Output:    port_transmit[port][xxx]          */
/************************************************/
/*===========================================================================*/
BYTE Ex_Produce_Transmit_Info(void)	//-����ÿ���˿ڹ̶�һ�ֹ�Լ,����˿��¹ҽӵ�������Ϣ�����ڸ�����Լ�ļ��д���,�������ε����þ��ǰѱ��Ļ��ڷ���
{
    unsigned short temp_rnd;
    unsigned short temp_rnd1;
    unsigned char *temp_pt;

    if(exchange_target_port<14)
    {
        if(port_transmit_flag[exchange_target_port] == 0x55)	//-ֵΪ55˵��û�����ݱ����Ѿ�����,���Կ��Է���,,���ܴ��巢�ͱ���
        {
            temp_pt = &port_transmit[exchange_target_port][0]; 
            for(temp_rnd=1;temp_rnd<16;temp_rnd++)
            {
                temp_pt[temp_rnd]=exchange_buf[temp_rnd];	//-�Ѵ��ݵ���Ϣ�Ѿ����뵽��Ŀ�Ķ˿ڻ���������
            }

            byte0(temp_rnd1)=exchange_buf[9];	//-��ʾ��16��Ԫ��ʼ���м�����Ԫ���������Ҫ����
            byte1(temp_rnd1)=exchange_buf[10];
            temp_rnd1=temp_rnd1+16;
            for(temp_rnd=16;temp_rnd<temp_rnd1;temp_rnd++)
            {
                temp_pt[temp_rnd]=exchange_buf[temp_rnd];	//-Ҳ�������û���κι�Լ���Ծ���һ������������
            }
            port_transmit_flag[exchange_target_port] = 0xaa;	//-��ʾ���˿ڽ��յ��˴�����Ϣ,��Ҫ����
            return YES;
        }   
        else
            return NO;
    }
    else
    {
        if(set_transmit_flag == 0x55)
        {
            temp_pt = &set_transmit[0]; 
            for(temp_rnd=1;temp_rnd<16;temp_rnd++)
            {
                temp_pt[temp_rnd]=exchange_buf[temp_rnd];
            }

            byte0(temp_rnd1)=exchange_buf[9];
            byte1(temp_rnd1)=exchange_buf[10];
            if(temp_rnd1>250) 
                return NO;
            temp_rnd1=temp_rnd1+16;
            for(temp_rnd=16;temp_rnd<temp_rnd1;temp_rnd++)
            {
                temp_pt[temp_rnd]=exchange_buf[temp_rnd];
            }
            set_transmit_flag = 0xaa;
            return YES;
        }   
        else
            return NO;
    }
}





void Ex_scan_yk_range_for_port()
{
  unsigned short temp_rnd,temp_rnd1;
  unsigned char  the_start_flag[14],the_end_flag[14];
  
  for(temp_rnd=0;temp_rnd<14;temp_rnd++)
  { 
     the_start_flag[temp_rnd] = 0;
     the_end_flag[temp_rnd]   = 0;
     
     Ex_YK_start[temp_rnd]   = 1;
     Ex_YK_end[temp_rnd]     = 0;
  }
  //-һ̨װ�ö�Ӧ���ݿ��е�Ψһ����,��ͬһ��Լ��װ�ÿ��Խӵ�һ���˿���
  for(temp_rnd=0;temp_rnd<256;temp_rnd++)	//-������ݿ����Ա�������ֵ
  {
      if(unit_info[temp_rnd].yk_num!=0)	//-��������ң�ع��ܾͽ��������ж�
      {
          for(temp_rnd1=0;temp_rnd1<14;temp_rnd1++)	//-��ô���ҵ��˿�����
          {
              if(  (temp_rnd>=byte0(port_info[temp_rnd1].mirror_unit))
                 &&(temp_rnd<=byte1(port_info[temp_rnd1].mirror_unit)) )	//-ȷ�����ݿ�Ԫ���ڶ˿�������,��Ϊÿ�����ݿ�Ԫ�������ڶ˿����������µ�
              {	//-ÿ���˿ڶ�Ӧ���ڲ����ݿⵥԪ��ֵ�϶��ǲ�ͬ��
                  if(the_start_flag[temp_rnd1]==0)	//-�Ҵ��˶˿ں�
                  {
                      the_start_flag[temp_rnd1]=0xff;	//-�Ǳ�ʾ����˿���ң�����ô�����
                      Ex_YK_start[temp_rnd1]=unit_info[temp_rnd].yk_start;	//-һ���˿��������豸ң�ؿ�ʼ�ĺ�
                  }
                  
                  if(unit_info[temp_rnd].yk_start<Ex_YK_start[temp_rnd1])
                  {
                      Ex_YK_start[temp_rnd1]=unit_info[temp_rnd].yk_start;	//-ȷ����¼����һ���˿���������ң���豸�ĺŵ���Сֵ
                  }

                  if(the_end_flag[temp_rnd1]==0)	//-0�ͱ�ʾ��û�з���
                  {
                      the_end_flag[temp_rnd1]=0xff;	//-��ʾ������ʶ��Ч
                      Ex_YK_end[temp_rnd1] = unit_info[temp_rnd].yk_start + unit_info[temp_rnd].yk_num-1;	//-��¼���Ƕ˿���ң�ؽ�����
                  }
                  if((unit_info[temp_rnd].yk_start+unit_info[temp_rnd].yk_num)>Ex_YK_end[temp_rnd1]) 
                      Ex_YK_end[temp_rnd1] = unit_info[temp_rnd].yk_start + unit_info[temp_rnd].yk_num-1;    //-������Щ���ǰ���˳�����õ�,���Ա����ҵ���������
              }
          }
      }
  }
}

void core_init()
{
  yx_chg_in_pt = 0;
  yx_soe_in_pt = 0;
  for(temp_loop=0;temp_loop<4;temp_loop++)
  {
     yx_chg_tr_ex_pt[temp_loop] = 0;
     yx_soe_tr_ex_pt[temp_loop] = 0;
     yx_chg_tr_in_pt[temp_loop] = 0;
     yx_soe_tr_in_pt[temp_loop] = 0;
  }

  BH_Report_Bank_Sav_Ptr=0;
  
  for(temp_loop=0;temp_loop<12;temp_loop++)
  {
      BH_Report_Bank_Tak_Ptr[temp_loop]=0;
  } 
}

void Ex_init()	//-Ϊ������Ϣ���ĳ�ʼ��׼��
{
  unsigned char temp_rnd;

  core_init();
  Ex_scan_yk_range_for_port();

  for(temp_rnd=0;temp_rnd<14;temp_rnd++)
  {
     port_transmit_flag[temp_rnd] = 0x55;
     port_transmit[temp_rnd][0]   = PORT_EXCHANGE_STA_IDLE;
  }  
}


//Ӧ�� core  ǰ�����ͷ� temp_loop,temp_loop1,temp_int,temp_lp_int 

void core_get_bh_bank_report(BYTE the_port)
{
	BYTE need_refind;
	if(Core_Src_Unit>=BH_REPORT_BANK_SIZE)  Core_Src_Unit=0;
	if(the_port>11)  
	{
		Core_Src_Unit++;
		Core_Src_Len=0;
          	return;
		
	}	
    Core_Src_Pt_B=&BH_Saved_Report_Bank[Core_Src_Unit][0];
    need_refind=NO;
re_find:
    while((Core_Src_Pt_B[4]!=( (Core_Src_Pt_B[0]+Core_Src_Pt_B[1]+Core_Src_Pt_B[2]+Core_Src_Pt_B[3])&0xff ))
        ||(Core_Src_Pt_B[2]!=0)||(need_refind==YES))
    {//-��������Ƿ����
    	need_refind=NO;
        Core_Src_Unit++;
        if(Core_Src_Unit>=BH_REPORT_BANK_SIZE) Core_Src_Unit=0;
        if(Core_Src_Unit==BH_Report_Bank_Sav_Ptr)
        {
            Core_Src_Len=0;
            return;
        }
        Core_Src_Pt_B=&BH_Saved_Report_Bank[Core_Src_Unit][0];
    }
    
    byte1(Core_Temp_Loop)=Core_Src_Pt_B[6];
    if(byte1(Core_Temp_Loop)>10) 
    {
    	need_refind=YES;
    	goto re_find;
    }	
    for(byte0(Core_Temp_Loop)=0;byte0(Core_Temp_Loop)<byte1(Core_Temp_Loop);byte0(Core_Temp_Loop)++)
    {
    	if((Core_Src_Pt_B[2]!=byte0(Core_Temp_Loop))||(Core_Src_Pt_B[3]>26)
    	 ||(Core_Src_Pt_B[4]!=( (Core_Src_Pt_B[0]+Core_Src_Pt_B[1]+Core_Src_Pt_B[2]+Core_Src_Pt_B[3])&0xff )))  // frame_NO err
    	{//-֡������,������
	    	need_refind=YES;
    		goto re_find;
    	}

		
   		if(byte0(Core_Temp_Loop)==0)
   		{
   			if(Core_Src_Pt_B[3]>2)  //-���ĳ���
   				byte1(Core_Temp_ShortInt)=Core_Src_Pt_B[3]-2;
   			else
   				byte1(Core_Temp_ShortInt)=0;
   			for(byte0(Core_Temp_ShortInt)=0;byte0(Core_Temp_ShortInt)<byte1(Core_Temp_ShortInt);byte0(Core_Temp_ShortInt)++)
   			{//-���ܿ���ѱ������ݸ��Ƶ���Ӧͨ������,�ݴ����ݹ�������
   				BH_Bank_Report[the_port][byte0(Core_Temp_ShortInt)]=Core_Src_Pt_B[byte0(Core_Temp_ShortInt)+8];
   			}
   			Core_Src_Len=byte1(Core_Temp_ShortInt);
   		    BH_Bank_Report[the_port][509]=Core_Src_Pt_B[1];
   		    BH_Bank_Report[the_port][510]=Core_Src_Pt_B[0];
   		    BH_Bank_Report[the_port][511]=Core_Src_Pt_B[5];
   		}
   		else
   		{
   			if((Core_Src_Pt_B[0]!=BH_Bank_Report[the_port][510])||(Core_Src_Pt_B[5]!=BH_Bank_Report[the_port][511])
   			 ||(Core_Src_Pt_B[1]!=BH_Bank_Report[the_port][509]))
   			{
   		    	need_refind=YES;
       			goto re_find;
   			}
   			
   			byte1(Core_Temp_ShortInt)=Core_Src_Pt_B[3];
   			for(byte0(Core_Temp_ShortInt)=0;byte0(Core_Temp_ShortInt)<byte1(Core_Temp_ShortInt);byte0(Core_Temp_ShortInt)++)
   			{
   				BH_Bank_Report[the_port][Core_Src_Len+byte0(Core_Temp_ShortInt)]=Core_Src_Pt_B[byte0(Core_Temp_ShortInt)+6];
   			}
   			Core_Src_Len+=byte1(Core_Temp_ShortInt);
   		}	

        Core_Src_Unit++;
        if(Core_Src_Unit>=BH_REPORT_BANK_SIZE) Core_Src_Unit=0;
        Core_Src_Pt_B=&BH_Saved_Report_Bank[Core_Src_Unit][0];
    }
    
//    if((Core_Src_Pt_B[0]==(PROTOCOL_LSA%0x100))&&(Core_Src_Pt_B[5]==(PROTOCOL_LSA/0x100)))
//    {
//        if((Core_Src_Pt_B[2]==0)&&(Core_Src_Pt_B[6]==1)&&(Core_Src_Pt_B[3]<=24))
//        {
//            Core_Src_Len=Core_Src_Pt_B[3];
//            for(Core_Temp_Loop=0;Core_Temp_Loop<Core_Src_Len;Core_Temp_Loop++)
//            {
//                BH_Bank_Report[the_port][Core_Temp_Loop]=Core_Src_Pt_B[Core_Temp_Loop+8];
//            }
//            BH_Bank_Report[the_port][510]=(PROTOCOL_LSA%0x100);
//            BH_Bank_Report[the_port][511]=(PROTOCOL_LSA/0x100);
//        }
//        else
//        {
//           Core_Src_Len=0;
//        }
//    }
//    else  // other protocol
//    {
//        Core_Src_Len=0;
//    }
    
//    Core_Src_Unit++;
//    if(Core_Src_Unit>=BH_REPORT_BANK_SIZE) Core_Src_Unit=0;
}

void core_get_yx_set_unit()
{
  if((Core_Src_Unit>0x1f)&&(Core_Src_Unit<0x30))
     Core_Temp_Loop = 4;
  else
     Core_Temp_Loop = unit_info[Core_Src_Unit].yx_num;
  //Core_Src_Unit Core_Src_Pt_B
  Core_Temp_ShortInt= unit_info[Core_Src_Unit].yx_start;
  for(Core_Temp_Loop1=0;Core_Temp_Loop1<Core_Temp_Loop;Core_Temp_Loop1++)
  {
     *Core_Src_Pt_B =  byte0(YX_property[Core_Temp_ShortInt + Core_Temp_Loop1]);
      Core_Src_Pt_B ++;
     *Core_Src_Pt_B = byte1(YX_property[Core_Temp_ShortInt + Core_Temp_Loop1]);
      Core_Src_Pt_B ++;    
  }
  for(Core_Temp_Loop1=0;Core_Temp_Loop1<Core_Temp_Loop;Core_Temp_Loop1++)
  {
     *Core_Src_Pt_B =  byte0(YX_YM[Core_Temp_ShortInt+Core_Temp_Loop1]);
      Core_Src_Pt_B ++;
     *Core_Src_Pt_B = byte1(YX_YM[Core_Temp_ShortInt+Core_Temp_Loop1]);
      Core_Src_Pt_B ++;    
  }

  Core_Temp_ShortInt = Core_Temp_ShortInt*8;
  for(Core_Temp_Loop1=0;Core_Temp_Loop1<(Core_Temp_Loop*8);Core_Temp_Loop1++)
  {
     if((Core_Temp_ShortInt + Core_Temp_Loop1)<256)
     {
        *Core_Src_Pt_B =  byte0(YX_double[Core_Temp_ShortInt + Core_Temp_Loop1]);
         Core_Src_Pt_B ++;
        *Core_Src_Pt_B = byte1(YX_double[Core_Temp_ShortInt + Core_Temp_Loop1]);
         Core_Src_Pt_B ++;    
     }
     else
     {
        *Core_Src_Pt_B =  0xc8;
         Core_Src_Pt_B ++;
        *Core_Src_Pt_B =  0xc8;
         Core_Src_Pt_B ++;    
     }
  }

  Core_Temp_ShortInt = Core_Temp_ShortInt*2;
  for(Core_Temp_Loop1=0;Core_Temp_Loop1<(Core_Temp_Loop*16);Core_Temp_Loop1++)
  {
     if((Core_Temp_ShortInt + Core_Temp_Loop1)<512)
     {
        *Core_Src_Pt_B =  byte0(YX_delay[Core_Temp_ShortInt + Core_Temp_Loop1]);
         Core_Src_Pt_B ++;
        *Core_Src_Pt_B = byte1(YX_delay[Core_Temp_ShortInt + Core_Temp_Loop1]);    
         Core_Src_Pt_B ++;
     }
     else
     {
        *Core_Src_Pt_B =  0xc8;
         Core_Src_Pt_B ++;
        *Core_Src_Pt_B =  0x00;    
         Core_Src_Pt_B ++;
     }
  }
}

void core_get_yc_set_unit()
{
  if((Core_Src_Unit>0x0f)&&(Core_Src_Unit<0x20))
      Core_Temp_Loop = 6;
  else
      Core_Temp_Loop = unit_info[Core_Src_Unit].yc_line_num;

  //Core_Src_Unit Core_Src_Pt_B
  Core_Temp_ShortInt = unit_info[Core_Src_Unit].yc_line_start;
  for(Core_Temp_Loop1=0;Core_Temp_Loop1<Core_Temp_Loop;Core_Temp_Loop1++)
  {
     if(Core_Temp_Loop1<unit_info[Core_Src_Unit].yc_line_num)
     {
       *Core_Src_Pt_B =  YC_coef_table[(Core_Temp_ShortInt + Core_Temp_Loop1)*4];
        Core_Src_Pt_B ++;
       *Core_Src_Pt_B =  YC_coef_table[(Core_Temp_ShortInt + Core_Temp_Loop1)*4 + 1];
        Core_Src_Pt_B ++;    
     }
     else
     {
       *Core_Src_Pt_B =  0;
        Core_Src_Pt_B ++;
       *Core_Src_Pt_B =  0;
        Core_Src_Pt_B ++;    
     }
  }
}

void core_update_YC()
{
  Core_Temp_Loop = unit_info[Core_Src_Unit].yc_val_num;
  Core_Det_Pt = (unsigned short *)&(YC_State[unit_info[Core_Src_Unit].yc_val_start]);
  if(Core_Temp_Loop>Core_Src_Len)
     Core_Temp_Loop = Core_Src_Len;
  for(Core_Temp_Loop1=0;Core_Temp_Loop1<Core_Temp_Loop;Core_Temp_Loop1++)
  {
     *Core_Det_Pt   =  (*Core_Src_Pt_B) + (*(Core_Src_Pt_B+1))*256;
     Core_Src_Pt_B += 2;
     Core_Det_Pt    ++;
  }
    
}

void core_update_DC()
{
  Core_Temp_Loop = unit_info[Core_Src_Unit].dc_num;
  Core_Det_Pt = (unsigned short *)&(YC_State[unit_info[Core_Src_Unit].dc_start]);
  if(Core_Temp_Loop>Core_Src_Len)
     Core_Temp_Loop = Core_Src_Len;
  for(Core_Temp_Loop1=0;Core_Temp_Loop1<Core_Temp_Loop;Core_Temp_Loop1++)
  {
     *Core_Det_Pt   =  (*Core_Src_Pt_B) + (*(Core_Src_Pt_B+1))*256;
     Core_Src_Pt_B += 2;
     Core_Det_Pt    ++;
  }
    
}

/************************************************/
/* core_update_YX     function                  */
/* Input :  Core_Src_Len                        */
/*          Core_Src_Pt_B                       */
/*          Core_Src_Unit                       */
/* Output:                                      */
/* REG varible used: Core_Det_Pt                */
/*                   Core_Src_Pt                */
/*                   Core_Temp_Loop             */
/*                   Core_Temp_Loop1            */
/*                   Core_Temp_ShortInt         */
/************************************************/
//-����ң��ֵ�ƺ����Ǽ򵥵�ң��״̬,���Ǳ仯��.��ʵ��Ҫ�ǿ���̨����ô������
//-���ڿ�������,���ݿ��ֵȫ��������,ֻ���б仯�Ļ����ڱ仯�ռ��ڼ�¼�仯��Ϣ
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void core_update_YX()
{
  unsigned short chg_buf0,chg_buf1;
  unsigned short  yx_buf0;

  Core_Det_Pt = (unsigned short *)&(YX_State[unit_info[Core_Src_Unit].yx_start]);	//-ȡ��ʵ��ֵ,��ʵ����������ָ����

  Core_Temp_Loop   = unit_info[Core_Src_Unit].yx_num;	//-��λ����
  Core_Temp_Loop   = Core_Temp_Loop * 2;
  if(Core_Temp_Loop>Core_Src_Len)	//-��ʵ,��Դһ��,ֻ�ǿɿ�,,����������ģ�黯���˼�������,���ں��ظ�����һ��
      Core_Temp_Loop = Core_Src_Len;
  Core_Temp_Loop &= 0xfe;  //avoid odd
  
  if(HOST_ZF_enable!=YES)	//-�ڻ�û�п������̨ת����ʱ��ֱ�Ӹ������ݿ�
  {
     Core_Temp_Loop = Core_Temp_Loop/2;
     for(Core_Temp_Loop1=0;Core_Temp_Loop1<Core_Temp_Loop;Core_Temp_Loop1++)
     {
        *Core_Det_Pt = (*(Core_Src_Pt_B+1))*256 + (*Core_Src_Pt_B);
         Core_Det_Pt ++;
         Core_Src_Pt_B +=2;
     }    
     return;	//-������γ�����
  }
  //-�����е�������λ�������ȶ�ʱ��û�е���֮ǰ,ֱ�Ӹ������ݿ�,����ʱ����֮��,������һ����¼ң�ű仯�Ĺ���
  Core_Src_Len = 0;
  while(Core_Temp_Loop>0)	//-ң���ֽ���
  {
      Core_Src_Pt = &yx_change[yx_chg_in_pt].offset_no;	//-����洢�ռ��Դ�����,���Բ���Ҫ���ⶨλ,���δ洢����
     *Core_Src_Pt =  unit_info[Core_Src_Unit].yx_start + Core_Src_Len;	//-��¼ң�ŵĿ�ʼ��ַ,˵���˻���ƫ����,����Ϊ��λ

      Core_Temp_ShortInt = 0x5555;	//-��ʾû�г��ֱ仯ң��

      chg_buf0  = 0;
      chg_buf1  = 0;

      yx_buf0   = (*(Core_Src_Pt_B+1))*256 + (*Core_Src_Pt_B);	//-��������������ֵ
      if(yx_buf0 != *Core_Det_Pt)	//-��ԭ������ֵ���бȽ�
      {
          Core_Temp_ShortInt = 0xaaaa;
          chg_buf0           = yx_buf0 ^ (*Core_Det_Pt);
      }
     *Core_Det_Pt     = yx_buf0;	//-��¼�仯���ֵ,����͸��������ݿ�
      Core_Src_Pt_B   += 2;
      Core_Det_Pt     ++;
      Core_Temp_Loop  -= 2;
      Core_Src_Len    ++;	//-ȷ����λYX��ƫ����,������Ϊ��λ��,����ÿ����¼��Ԫ�ֿ��Լ�¼�����ֵ���

      if((*Core_Src_Pt & 1) != 0)	//-����϶����ж���ż��,
      {	//-�������ʾ������,ֻ����һ�ε�
          if(Core_Temp_ShortInt == 0xaaaa)	//-���������ֵ��ʾû�з��ͱ仯,��Ȼ�Ͳ���Ҫ��¼
          {
             *Core_Src_Pt = *Core_Src_Pt / 2;
             Core_Src_Pt++;
             *Core_Src_Pt = *(Core_Det_Pt-2);	//-���������ԭ������ֵ
             Core_Src_Pt++;
             *Core_Src_Pt = *(Core_Det_Pt-1);	//-����һ������Ч��Ϣ��16Ϊ,���������Ԫ��һ����32Ϊ,����ÿ�ν���ʹ��һ��,��һ����Ч
             Core_Src_Pt++;
             *Core_Src_Pt = 0;               //-����˵����,���ң��û��ʹ��
             Core_Src_Pt++; 
             *Core_Src_Pt = chg_buf0;	//-1��ʾ�б仯
             yx_chg_in_pt ++;	//-���������ʵ�Ϳ�����Ϊ�Զ����͵�һ������
             yx_chg_in_pt &= 0xff;         
             Core_Temp_ShortInt = 0x5555;
          }
      }
      else
      {	//-��������ż��,,���ڼ�¼����һ��ż��+һ������,���Ե��ж�Ϊż���ĵ�ʱ��,����������Ļ����ж�һ����һ��û�о�����
          if(Core_Temp_Loop>0)
          {
               yx_buf0 = ((*(Core_Src_Pt_B+1)))*256 + (*Core_Src_Pt_B);
               if(yx_buf0 != (*Core_Det_Pt))
               {
                   Core_Temp_ShortInt = 0xaaaa;
                   chg_buf1           = yx_buf0 ^ (*Core_Det_Pt);
               }
              *Core_Det_Pt     = yx_buf0;	//-��¼������ֵ,���������ݿ�
               Core_Src_Pt_B   = Core_Src_Pt_B + 2;
               Core_Temp_Loop -= 2;
               Core_Src_Len    ++;
          }

          Core_Det_Pt++;	//-Ϊ�����ģ�����,ǰ��̶�Ϊż������������

          if(Core_Temp_ShortInt == 0xaaaa)
          {
             *Core_Src_Pt = *Core_Src_Pt / 2;	//-����2��Ŀ��������һ��,����ƫ����Ҫ����2
             Core_Src_Pt++;
             *Core_Src_Pt = *(Core_Det_Pt-2);	//-ң�Ÿոո��µ�ֵ
             Core_Src_Pt++;
             *Core_Src_Pt = *(Core_Det_Pt-1);	//-ң�Ÿոո��µ�ֵ,���仯֮���ֵ
             Core_Src_Pt++;
             *Core_Src_Pt = chg_buf0;           //-��־��Щλң�ŷ����˱仯
             Core_Src_Pt++; 
             *Core_Src_Pt = chg_buf1;			//-��λ��Ϣ
             yx_chg_in_pt ++;	//-�仯����ռ���������
             yx_chg_in_pt &= 0xff;         
             Core_Temp_ShortInt = 0x5555;
          }
      }
  }
}

void core_insert_SOECOS()
{
   // before insert SOE & COS, must fill YX_State with New YX value to generate COS
   
   Core_Det_Pt = (unsigned short *)&yx_event[yx_soe_in_pt].soe_ms;
  //ms
   *Core_Det_Pt = (*Core_Src_Pt_B) + (*(Core_Src_Pt_B+1))*256;
   Core_Src_Pt_B += 2;
   Core_Det_Pt++;
  //sec min
   *Core_Det_Pt = (*Core_Src_Pt_B) + (*(Core_Src_Pt_B+1))*256;
   Core_Src_Pt_B += 2;
   Core_Det_Pt++;
  //hour day
   *Core_Det_Pt = (*Core_Src_Pt_B) + (*(Core_Src_Pt_B+1))*256;
   Core_Src_Pt_B += 2;
   Core_Det_Pt++;
  // channel state

   *Core_Det_Pt = (*Core_Src_Pt_B) + (*(Core_Src_Pt_B+1))*256;
   Core_Temp_ShortInt=((*Core_Det_Pt) & 0x8fff) + (unit_info[Core_Src_Unit].yx_start*16 & 0x0fff);
   *Core_Det_Pt = Core_Temp_ShortInt;

   yx_soe_in_pt++;
   if(yx_soe_in_pt>1023)
     yx_soe_in_pt = 0;

   Core_Temp_ShortInt&=0x0fff;
   Core_Temp_Loop=Core_Temp_ShortInt/16;
   Core_Det_Pt = (WORD *)&YX_State[Core_Temp_Loop];
   Core_Src_Pt = &yx_change[yx_chg_in_pt].offset_no;
  *Core_Src_Pt = Core_Temp_Loop/2;           Core_Src_Pt++; // offset
   if(((Core_Temp_Loop)& 0x01)==0)
   {
       *Core_Src_Pt = *(Core_Det_Pt+0x00);   Core_Src_Pt++; // YX0
       *Core_Src_Pt = *(Core_Det_Pt+0x01);   Core_Src_Pt++; // YX1
       *Core_Src_Pt = (WORD)(1<<(Core_Temp_ShortInt%16));    Core_Src_Pt++;   // CHG0
       *Core_Src_Pt = 0x00;                                 // CHG1
   } 
   else
   {
       *Core_Src_Pt = *(Core_Det_Pt-0x01);   Core_Src_Pt++; // YX0
       *Core_Src_Pt = *(Core_Det_Pt+0x00);   Core_Src_Pt++; // YX1
       *Core_Src_Pt = 0x00;                  Core_Src_Pt++; // CHG0
       *Core_Src_Pt = (WORD)(1<<(Core_Temp_ShortInt%16));   // CHG1
   }
   
   yx_chg_in_pt ++;
   yx_chg_in_pt &= 0xff;         
}

void core_insert_SOE()
{
/*   unsigned short	the_ram_ax;	
   the_ram_ax=Core_Src_Pt_B[6]+(Core_Src_Pt_B[7]&0x0f)*256;
   if(the_ram_ax>=(unit_info[Core_Src_Unit].yx_num*16))
   return;
   the_ram_ax+=unit_info[Core_Src_Unit].yx_start*16;
   if(the_ram_ax>0xfff)
   return;*/
   Core_Det_Pt = (unsigned short *)&yx_event[yx_soe_in_pt].soe_ms;
  //ms
   *Core_Det_Pt = (*Core_Src_Pt_B) + (*(Core_Src_Pt_B+1))*256;
   Core_Src_Pt_B += 2;
   Core_Det_Pt++;
  //sec min
   *Core_Det_Pt = (*Core_Src_Pt_B) + (*(Core_Src_Pt_B+1))*256;
   Core_Src_Pt_B += 2;
   Core_Det_Pt++;
  //hour day
   *Core_Det_Pt = (*Core_Src_Pt_B) + (*(Core_Src_Pt_B+1))*256;
   Core_Src_Pt_B += 2;
   Core_Det_Pt++;
  // channel state

   *Core_Det_Pt = (*Core_Src_Pt_B) + (*(Core_Src_Pt_B+1))*256;
   *Core_Det_Pt = ((*Core_Det_Pt) & 0x8fff) + (unit_info[Core_Src_Unit].yx_start*16 & 0x0fff);

   yx_soe_in_pt++;
   if(yx_soe_in_pt>1023)
     yx_soe_in_pt = 0;
}

void core_update_YM()
{
  Core_Temp_Loop = unit_info[Core_Src_Unit].ym_num;
  if(*Core_Src_Pt_B>Core_Temp_Loop)
     return;
  
  Core_Det_Pt = (unsigned short *)&(YM_State[unit_info[Core_Src_Unit].ym_start+(*Core_Src_Pt_B)]);
  if(Core_Temp_Loop<(Core_Src_Len+(*Core_Src_Pt_B)))
     Core_Temp_Loop = Core_Temp_Loop - *Core_Src_Pt_B;
  else
     Core_Temp_Loop = Core_Src_Len;

  Core_Temp_Loop = Core_Temp_Loop*2;
  Core_Src_Pt_B  ++;
  for(Core_Temp_Loop1=0;Core_Temp_Loop1<Core_Temp_Loop;Core_Temp_Loop1++)
  {
     *Core_Det_Pt    = (*Core_Src_Pt_B) + (*(Core_Src_Pt_B+1))*256;
      Core_Src_Pt_B += 2;
      Core_Det_Pt   ++;
  }
    
}