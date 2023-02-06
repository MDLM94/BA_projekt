#include "udp_tester.h"
#include "testcpp_json.h"

#if LWIP_UDP
#include "lwip/timeouts.h"
#include "gpio.h"
#include "tim.h"
#include "math.h"
#include <string.h>
#include <stdio.h>
#include "lwjson.h"
#include <stdint.h>
#include "json.h"
#include "defines.h"
#define TFTP_MAX_PAYLOAD_SIZE 512
#define TFTP_HEADER_LENGTH    4

#define TFTP_RRQ   1
#define TFTP_WRQ   2
#define TFTP_DATA  3
#define TFTP_ACK   4
#define TFTP_ERROR 5





bool ARR_flag = false;
bool rampD_flag = false;
u8_t flip = 0;
char* str_ptr;
int jsonlen = 0;
int top_counter;
int PeriodGlobal = 0;
int RPMGlobal = 0;
int Period = 0;
float rot_deg = 0;
int cntr = 0;
u8_t GoZeroFlag;
int globalRot = 0;
int limit_flag = 0;
int toggles = 0;
char toggle_cnt[15];

void blinkLED(int blinks);
void direction(u8_t dir);
void calcPeriod(u16_t RPM);
void send_msg(const ip_addr_t *addr, u16_t port, const char *str);




struct data_struct{
	struct udp_pcb *upcb;

};
static struct data_struct data_struct;



/// disse 3 linjer skal nok fjernes
//u32_t test_addr = 2114037952; //126.1.168.192
//u32_t *aptr = &test_addr;
//u16_t test_port = 73;


void send_msg(const ip_addr_t *addr, u16_t port, const char *str)
{

  int str_length = strlen(str);
  struct pbuf *p;
  u16_t *payload;

  //p = pbuf_alloc(PBUF_RAW, (u16_t)(str_length), PBUF_RAM);
  p = pbuf_alloc(PBUF_TRANSPORT, (u16_t)(str_length), PBUF_RAM);
  if (p == NULL) {
    return;
  }

 payload = (u16_t *) p->payload;


  MEMCPY(&payload[0], str, str_length);

 udp_sendto(data_struct.upcb, p, addr, port);
  pbuf_free(p);

}




static void
recv(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{



	//char *str_msg;
	cntr = 0;
    rot_cnt = 0;
    GoZeroFlag = 0;
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(upcb);

    //int* checkip = ip_current_src_addr();

   char* command = cmd_parser(p->payload, p->len);
   char* b_info_msg;


    // hvis broadcast
    if(ip_addr_isbroadcast(ip_current_dest_addr(), ip_current_netif()) == 1){
    	b_info_msg = makeCharB_info(1);
     	send_msg(addr, 73, b_info_msg);
     	free(b_info_msg);

   }
   //else if(*checkip == 2114037952){
   else{
	   //send_msg(addr, 73, makeCharMsg(cmd_parser(p->payload, p->len)));
	   ///strcmp

	   if( strcmp("StepM", command) == 0){
			if( goZero == 1 ){
				ReturnToZero();
				char* charmsg =  makeCharMsg("Returning to reference point");
				send_msg(addr, 73, charmsg);
				free(charmsg);
			}
			else{
				rot_deg = round(degrees * 1.111)*8;
				calcPeriod(rpm);
				direction(direc);
			    char stri_msg[1472];
				sprintf(stri_msg, "Degrees = %d, RPM = %d, Direction = %d", degrees, rpm, direc);
				char* charmsg = makeCharMsg(stri_msg);
				send_msg(addr, 73, charmsg);
				free(charmsg);

			}

	   }
	   else if ( strcmp("LED", command) == 0 ){
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
				  __HAL_TIM_SET_COUNTER(&htim5, 0);
				  __HAL_TIM_SET_AUTORELOAD(&htim5,96000000);
					HAL_TIM_Base_Start_IT(&htim5);
			    char stri_msg[1472];
				sprintf(stri_msg, "LED on for %d seconds", LEDblink);
				 char* charmsg = makeCharMsg(stri_msg);
				send_msg(addr, 73, charmsg);
				free(charmsg);

//#if switch_board_2 == 1

//#endif


	   }
	   else{
		   char stri_msg[1472];

		 /*  if(limit_flag == 1){
			   sprintf(stri_msg, "Antal pakker modtaget = %d", limit_cnt);

			   char* charmsg = makeCharMsg(stri_msg);
			   send_msg(addr, 73, charmsg);
			   free(charmsg);
			   limit_flag = 0;

			   lim_counter = 0;
		   }*/
		   char* charmsg = makeCharMsg("Uknown command");
		   free(charmsg);


	   }


   }


	free(command);
    pbuf_free(p);

}


err_t
SPE_data_init()
{
  err_t ret;

 /* LWIP_ASSERT_CORE_LOCKED(); is checked by udp_new() */
  struct udp_pcb *pcb = udp_new_ip_type(IPADDR_TYPE_ANY);
  if (pcb == NULL) {
    return ERR_MEM;
  }

 ret = udp_bind(pcb, IP_ANY_TYPE, client_port);
  if (ret != ERR_OK) {
    udp_remove(pcb);
    return ret;
  }

 data_struct.upcb = pcb;
 udp_recv(pcb, recv, NULL);

 return ERR_OK;
}




void blinkLED(int blinks){


	//for(int i = 0; i < blinks; i++){
		//HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7); //find ny pin til dette
		//HAL_Delay(500);
	//}
}


void direction(u8_t dir)
{

	if(dir == 1)
	{
	  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_SET);
	}
	if(dir == 0)
	{
	  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_RESET);
	}
#if switch_board_1 == 1
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
#endif
	HAL_TIM_Base_Start_IT(&htim2);
}




int led_ctr = 0;
/// Ryd op
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
#if switch_board_2 == 1
	if(htim == &htim5){
		led_ctr++;
		if(led_ctr == LEDblink){
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
			HAL_TIM_Base_Stop_IT(&htim5);
			__HAL_TIM_SET_COUNTER(&htim5, 0);
			led_ctr = 0;
		}
	}
#endif
	//limit test
	/*if (htim==&htim5){
		limit_flag = 1;
	   HAL_TIM_Base_Stop_IT(&htim5);
	   __HAL_TIM_SET_COUNTER(&htim5, 0);
	}*/

    if (htim==&htim2){
        if(GoZeroFlag == 1){
        	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
			cntr = 0;
        }
        else
        {
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
            cntr++;

           if(RPMGlobal > 300){
        	   if(rampD_flag != true && cntr % 10 == 0){
        		   //accelerer
				   if(PeriodGlobal > Period){
					 //PeriodGlobal --;
					 PeriodGlobal *= 0.999;
					 top_counter = cntr;
				   }
				   //
				   else if(PeriodGlobal <= Period){
					rampD_flag = true;
				   }
				   __HAL_TIM_SET_COUNTER(&htim2, 0);
				   __HAL_TIM_SET_AUTORELOAD(&htim2,PeriodGlobal);
        	   }
        	   //de-accelerer
        	   else if(rampD_flag == true){
        		   if(cntr % 10 == 0){
        			  if(cntr > (rot_deg - top_counter)){
    					  //PeriodGlobal ++;
        				  PeriodGlobal *= 1.0017;
    					  __HAL_TIM_SET_COUNTER(&htim2, 0);
						  __HAL_TIM_SET_AUTORELOAD(&htim2,PeriodGlobal);
        			  }
        		   }
               }


       }
// cntr bliver inkrementeret for hvert step og når equality er opnået stoppes motor.
	   if (cntr == rot_deg){
			HAL_TIM_Base_Stop_IT(&htim2);
			__HAL_TIM_SET_COUNTER(&htim2, 0);
			rampD_flag = false;
#if switch_board_1 == 1
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
#endif
	   }
    }
  }
}


void calcPeriod(u16_t RPM){
   // Period = 48000000.0/((RPM/60.0)*1600.0);
    Period = 96000000.0/((RPM/60.0)*3200.0);
    //PeriodGlobal = Period;
    if(RPM > 300){
        PeriodGlobal = 15000;
        //hvis RPM er for høj til at starte step motor sættes perioden til tælleperioden til 15000
        __HAL_TIM_SET_AUTORELOAD(&htim2,PeriodGlobal);
        RPMGlobal = RPM;
    } else {
        __HAL_TIM_SET_AUTORELOAD(&htim2,Period);
        RPMGlobal = RPM;
    }
}


// få den til at virke
void ReturnToZero(){
#if	switch_board_1 == 1
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
#endif
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_SET);
    __HAL_TIM_SET_AUTORELOAD(&htim2,70000);
    HAL_TIM_Base_Start_IT(&htim2);
    GoZeroFlag = 1;
}



#endif /* LWIP_UDP */
