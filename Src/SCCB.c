#include "SCCB.h"
#include "stm32f4xx.h"

/*
-----------------------------------------------
   ����: ��ʼ��ģ��SCCB�ӿ�
   ����: ��
 ����ֵ: ��
-----------------------------------------------
*/
void SCCB_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
   /* Enable GPIOA  clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  GPIO_InitStructure.Pin =  SCCB_SIC_BIT|SCCB_SID_BIT;	//SCCB_SIC_BIT=GPIO_Pin_7; SCCB_SID_BIT=GPIO_Pin_6
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;		  //GPIO_Mode_AIN = 0x0,     //ģ������
  GPIO_InitStructure.Speed =GPIO_SPEED_HIGH;		  //GPIO_Mode_IN_FLOATING = 0x04, //��������
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);				  //GPIO_Mode_IPD = 0x28,    //��������
														  //GPIO_Mode_IPU = 0x48,    //��������
														  //GPIO_Mode_Out_OD = 0x14, //��©���
														  //GPIO_Mode_Out_PP = 0x10,  //�������
														  //GPIO_Mode_AF_OD = 0x1C,   //��©����
														  //GPIO_Mode_AF_PP = 0x18    //���츴�� 
}
void SCCB_SID_GPIO_OUTPUT(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
   /* Enable GPIOA  clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  GPIO_InitStructure.Pin =  SCCB_SID_BIT;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStructure.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
}
void SCCB_SID_GPIO_INPUT(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
   /* Enable GPIOA  clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  GPIO_InitStructure.Pin =  SCCB_SID_BIT;
  GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
  GPIO_InitStructure.Pull = GPIO_PULLDOWN;
  //GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*
-----------------------------------------------
   ����: start����,SCCB����ʼ�ź�
   ����: ��
 ����ֵ: ��
-----------------------------------------------
*/
void startSCCB(void)
{
    SCCB_SID_H();     //�����߸ߵ�ƽ
    HAL_Delay(500);

    SCCB_SIC_H();	   //��ʱ���߸ߵ�ʱ���������ɸ�����
    HAL_Delay(500);
 
    SCCB_SID_L();
    HAL_Delay(500);

    SCCB_SIC_L();	 //�����߻ָ��͵�ƽ��������������Ҫ
    HAL_Delay(500);


}
/*
-----------------------------------------------
   ����: stop����,SCCB��ֹͣ�ź�
   ����: ��
 ����ֵ: ��
-----------------------------------------------
*/
void stopSCCB(void)
{
    SCCB_SID_L();
    HAL_Delay(500);
 
    SCCB_SIC_H();	
    HAL_Delay(500);
  

    SCCB_SID_H();	
    HAL_Delay(500);
   
}

/*
-----------------------------------------------
   ����: noAck,����������ȡ�е����һ����������
   ����: ��
 ����ֵ: ��
-----------------------------------------------
*/
void noAck(void)
{
	
	SCCB_SID_H();	
	HAL_Delay(500);
	
	SCCB_SIC_H();	
	HAL_Delay(500);
	
	SCCB_SIC_L();	
	HAL_Delay(500);
	
	SCCB_SID_L();	
	HAL_Delay(500);

}

/*
-----------------------------------------------
   ����: д��һ���ֽڵ����ݵ�SCCB
   ����: д������
 ����ֵ: ���ͳɹ�����1������ʧ�ܷ���0
-----------------------------------------------
*/
unsigned char SCCBwriteByte(unsigned char m_data)
{
	unsigned char j,tem;

	for(j=0;j<8;j++) //ѭ��8�η�������
	{
		if((m_data<<j)&0x80)
		{
			SCCB_SID_H();	
		}
		else
		{
			SCCB_SID_L();	
		}
		HAL_Delay(500);
		SCCB_SIC_H();	
		HAL_Delay(500);
		SCCB_SIC_L();	
		HAL_Delay(500);

	}
	HAL_Delay(100);
	SCCB_SID_GPIO_INPUT();/*����SDAΪ����*/
	HAL_Delay(500);
	SCCB_SIC_H();	
	HAL_Delay(100);
	if(SCCB_SID_STATE)
	{
		tem=0;
	}   //SDA=1����ʧ�ܣ�����0}
	else 
	{
		tem=1;
	}   //SDA=0���ͳɹ�������1
	SCCB_SIC_L();	
	HAL_Delay(500);	
        SCCB_SID_OUT;/*����SDAΪ���*/

	return (tem);  
}

/*
-----------------------------------------------
   ����: һ���ֽ����ݶ�ȡ���ҷ���
   ����: ��
 ����ֵ: ��ȡ��������
-----------------------------------------------
*/
unsigned char SCCBreadByte(void)
{
	unsigned char read,j;
	read=0x00;
	
	SCCB_SID_GPIO_INPUT();/*����SDAΪ����*/
	HAL_Delay(500);
	for(j=8;j>0;j--) //ѭ��8�ν�������
	{		     
		HAL_Delay(500);
		SCCB_SIC_H();
		HAL_Delay(500);
		read=read<<1;
		if(SCCB_SID_STATE) 
		{
			read=read+1;
		}
		SCCB_SIC_L();
		HAL_Delay(500);
	}	
	return(read);
}
