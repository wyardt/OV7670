#ifndef _SCCB_H
#define _SCCB_H

#define SCCB_SIC_BIT     GPIO_PIN_4	//SCL PA5
#define SCCB_SID_BIT     GPIO_PIN_5	 //SDA PA6

#define SCCB_SIC_H()     HAL_GPIO_WritePin(GPIOA, SCCB_SIC_BIT, GPIO_PIN_SET);  //对端口A特定位的设置	,这里设置高
#define SCCB_SIC_L()     HAL_GPIO_WritePin(GPIOA, SCCB_SIC_BIT, GPIO_PIN_RESET);  //这里设置低

#define SCCB_SID_H()     HAL_GPIO_WritePin(GPIOA, SCCB_SID_BIT, GPIO_PIN_SET);  //同上
#define SCCB_SID_L()     HAL_GPIO_WritePin(GPIOA, SCCB_SID_BIT, GPIO_PIN_RESET);
/**/
#define SCCB_SID_IN      SCCB_SID_GPIO_INPUT();
#define SCCB_SID_OUT     SCCB_SID_GPIO_OUTPUT();


#define SCCB_SID_STATE	 HAL_GPIO_ReadPin(GPIOA, SCCB_SID_BIT)

///////////////////////////////////////////	 
void SCCB_GPIO_Config(void);
void SCCB_SID_GPIO_OUTPUT(void);
void SCCB_SID_GPIO_INPUT(void);
void startSCCB(void);
void stopSCCB(void);
void noAck(void);
unsigned char SCCBwriteByte(unsigned char m_data);
unsigned char SCCBreadByte(void);


#endif /* _SCCB_H */


