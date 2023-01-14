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



/*
enum tftp_error {
  TFTP_ERROR_FILE_NOT_FOUND    = 1,
  TFTP_ERROR_ACCESS_VIOLATION  = 2,
  TFTP_ERROR_DISK_FULL         = 3,
  TFTP_ERROR_ILLEGAL_OPERATION = 4,
  TFTP_ERROR_UNKNOWN_TRFR_ID   = 5,
  TFTP_ERROR_FILE_EXISTS       = 6,
  TFTP_ERROR_NO_SUCH_USER      = 7
};
*/

/* LwJSON instance and tokens */
//static lwjson_token_t tokens[128];
//static lwjson_t lwjson;

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
/*struct tftp_state {
  const struct tftp_context *ctx;
  void *handle;
  struct pbuf *last_data;
  struct udp_pcb *upcb;
  ip_addr_t addr;
  u16_t port;
  int timer;
  int last_pkt;
  u16_t blknum;
  u8_t retries;
  u8_t mode_write;
};*/
int toggles = 0;
char toggle_cnt[15];
//void send_msg(const ip_addr_t *addr, u16_t port, const char *str);
void blinkLED(int blinks);
void direction(u8_t dir);
void calcPeriod(u16_t RPM);
void send_msg(const ip_addr_t *addr, u16_t port, const char *str);

//void delay(uint16_t us);
//static struct tftp_state tftp_state;

//static void tftp_tmr(void *arg);



struct data_struct{
	struct udp_pcb *upcb;

};
static struct data_struct data_struct;



/// disse 3 linjer skal nok fjernes
u32_t test_addr = 2114037952; //126.1.168.192
u32_t *aptr = &test_addr;
u16_t test_port = 73;

/*
///////////////////////////////////////////////////////////////
void send_test_msg(u32_t *t_address, u16_t t_port, char *str){
  int str_length = strlen(str);
  struct pbuf *p;
  u16_t *payload;

  p = pbuf_alloc(PBUF_TRANSPORT, (u16_t)(str_length), PBUF_RAM);
   if (p == NULL) {
     return;
   }

  payload = (u16_t *) p->payload;

  MEMCPY(&payload[0], str, str_length);

  udp_sendto(data_struct.upcb, p, t_address, t_port);
   pbuf_free(p);
}

/////////////////////////////////////////////////////////////
*/
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



	char *str_msg;
	cntr = 0;
    rot_cnt = 0;
    GoZeroFlag = 0;
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(upcb);

    int* checkip = ip_current_src_addr();





    if(ip_addr_isbroadcast(ip_current_dest_addr(), ip_current_netif()) == 1){

     	send_msg(addr, 73, makeCharB_info(1));

   }
   else if(*checkip == 2114037952){

	   //send_msg(addr, 73, makeCharMsg(cmd_parser(p->payload, p->len)));
	   if(strlen("Command = StepM") == strlen(cmd_parser(p->payload, p->len))){
		   char* whatsthis = (cmd_parser(p->payload, p->len));
			if( goZero == 1 ){
				ReturnToZero();
				send_msg(addr, 73, makeCharMsg("going home"));
			}
			else{
				rot_deg = round(degrees * 1.111)*8;
				calcPeriod(rpm);
				direction(direc);
				int sz = (strlen("Degrees = , RPM = , Direction = ")+strlen(degrees)+strlen(rpm)+strlen(direc));
				str_msg = (char *) malloc(sizeof(char) * sz);
				sprintf(str_msg, "Degrees = %d, RPM = %d, Direction = %d", degrees, rpm, direc);
				send_msg(addr, 73, makeCharMsg(str_msg));
			    free(str_msg);
			}

	   }
	   else if (strlen("Command = LED") == strlen(cmd_parser(p->payload, p->len))){
		   	   	blinkLED(LEDblink);
				int sz = strlen("LED blinks = ") + strlen(LEDblink);
				str_msg = (char *) malloc(sizeof(char) * sz);
				sprintf(str_msg, "LED blinks = %d", LEDblink);
				send_msg(addr, 73, makeCharMsg(str_msg));
				free(str_msg);

	   }


   }

		else{
		  send_msg(addr, 73, makeCharMsg("error message"));
		}



    pbuf_free(p);
}

 /// Ryd op og ændre navn på funk
err_t
tftp_init()
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
	for(int i = 0; i < blinks; i++){
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7); //find ny pin til dette
		HAL_Delay(500);
	}
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
	HAL_TIM_Base_Start_IT(&htim2);
}





/// Ryd op
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){


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
        		   //de-acceleration hvis PeriodGlobal opnår
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
        	   //accelerer
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
	   }
    }
  }
}


void calcPeriod(u16_t RPM){
    Period = 48000000.0/((RPM/60.0)*1600.0);
    //PeriodGlobal = Period;
    if(RPM > 300){
        PeriodGlobal = 15000;
        //hvis RPM er for høj til at starte step motor sættes perioden til tælleperioden til 15000
        __HAL_TIM_SET_AUTORELOAD(&htim2,PeriodGlobal);
        RPMGlobal = RPM;
    } else {
        __HAL_TIM_SET_AUTORELOAD(&htim2,Period);
        RPMGlobal = RPM; // skal formentlig fjernes, da den ikke er brugbar her
    }
}


// få den til at virke
void ReturnToZero(){
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_SET);
    __HAL_TIM_SET_AUTORELOAD(&htim2,70000);
    HAL_TIM_Base_Start_IT(&htim2);
    GoZeroFlag = 1;
}



#endif /* LWIP_UDP */
