#include "udp_tester.h"

#if LWIP_UDP
#include "ports.h"
#include "lwip/udp.h"
#include "lwip/timeouts.h"
#include "lwip/debug.h"
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


/*static void
close_handle(void)
{
  tftp_state.port = 0;
  ip_addr_set_any(0, &tftp_state.addr);

 if (tftp_state.last_data != NULL) {
    pbuf_free(tftp_state.last_data);
    tftp_state.last_data = NULL;
  }

 sys_untimeout(tftp_tmr, NULL);

 if (tftp_state.handle) {
    tftp_state.ctx->close(tftp_state.handle);
    tftp_state.handle = NULL;
    LWIP_DEBUGF(TFTP_DEBUG | LWIP_DBG_STATE, ("tftp: closing\n"));
  }
}*/

u32_t test_addr = 2114037952; //126.1.168.192
u32_t *aptr = &test_addr;
u16_t test_port = 73;

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

void send_msg(const ip_addr_t *addr, u16_t port, const char *str)
{

  int str_length = strlen(str);
  struct pbuf *p;
  u16_t *payload;



 p = pbuf_alloc(PBUF_TRANSPORT, (u16_t)(str_length), PBUF_RAM);
  if (p == NULL) {
    return;
  }

 payload = (u16_t *) p->payload;
  /*
  payload[0] = PP_HTONS(TFTP_ERROR);
  payload[1] = lwip_htons(code);
  */
  MEMCPY(&payload[0], str, str_length);

 udp_sendto(data_struct.upcb, p, addr, port);
  pbuf_free(p);
}


typedef struct {
	u8_t addrr;
    u8_t op1;
    int op2;
    u16_t value;
} UdpPack;






static void
recv(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{

	ip_addr_t toaddr;
	toaddr.addr = 0x7E01a8c0;
	//adresse = p->payload;
    jsonlen = p->len;

	UdpPack *sbuf = (UdpPack*) p->payload;

	char jsonfile[p->len+1];
	snprintf(jsonfile, p->len+1, "%s", p->payload);
    str_ptr = p->payload;

	cntr = 0;
    rot_cnt = 0;
    GoZeroFlag = 0;
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(upcb);



    int* checkip = ip_current_src_addr();

   if(ip_addr_isbroadcast(ip_current_dest_addr(), ip_current_netif()) == 1){
		#if (STEPMOTOR == 1 && LED == 1)
	   	   send_msg(addr, 73, "0, 1");
		#elif (LED == 1)
          send_msg(addr, 73, "0");
		#elif (STEPMOTOR == 1)
          send_msg(addr, 73, "1");
		#else
          send_msg(addr, 73, "-1");
		#endif
   }
   else if(*checkip == 2114037952){
	   jsonHandler(jsonfile);
   }
   else{
	   if(sbuf -> addrr == 1){

		}
		else if(sbuf -> addrr == 2){
			if(sbuf->op1 == 2){
				ReturnToZero();
			}
			else{

			rot_deg = roundf(sbuf->op2 * 1.111)*8;
			calcPeriod(sbuf->value);

			direction(sbuf->op1);


			}
		}
		else if(sbuf -> addrr == 3){




			//send_msg(addr, port, "Gaffel sensor");
		}
		else if(sbuf -> addrr == 4){
			//__HAL_TIM_GET_COUNTER(&htim5);
			sprintf(rota, "Rotations : %lu", __HAL_TIM_GET_COUNTER(&htim5));
			send_msg(addr, port, rota);
			HAL_TIM_Base_Stop(&htim5);
			__HAL_TIM_SET_COUNTER(&htim5, 0);

		}
		else if(sbuf -> addrr == 9){
			sprintf(rota, "48 MHz timer count : %lu", __HAL_TIM_GET_COUNTER(&htim5));
			send_test_msg(aptr, test_port, rota);
			HAL_TIM_Base_Stop(&htim5);
			__HAL_TIM_SET_COUNTER(&htim5, 0);
			 HAL_TIM_Base_Stop_IT(&htim2);
		   __HAL_TIM_SET_COUNTER(&htim2, 0);

		}

		else{
		  send_msg(addr, port, "Invalid key. Try again");
		  //HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
		}

	}
    toggles = 0;
    pbuf_free(p);
}


/*static void
tftp_tmr(void *arg)
{
  LWIP_UNUSED_ARG(arg);

 tftp_state.timer++;

 if (tftp_state.handle == NULL) {
    return;
  }
 sys_timeout(TFTP_TIMER_MSECS, tftp_tmr, NULL);

 if ((tftp_state.timer - tftp_state.last_pkt) > (TFTP_TIMEOUT_MSECS / TFTP_TIMER_MSECS)) {
    if ((tftp_state.last_data != NULL) && (tftp_state.retries < TFTP_MAX_RETRIES)) {
      LWIP_DEBUGF(TFTP_DEBUG | LWIP_DBG_STATE, ("tftp: timeout, retrying\n"));*/
      //resend_data();
     /* tftp_state.retries++;
    } else {
      LWIP_DEBUGF(TFTP_DEBUG | LWIP_DBG_STATE, ("tftp: timeout\n"));
      close_handle();
    }
  }
}*/



/** @ingroup tftp
* Initialize TFTP server.
* @param ctx TFTP callback struct
*/
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

 //tftp_state.handle    = NULL;
  //tftp_state.port      = 0;
  //tftp_state.ctx       = ctx;
  //tftp_state.timer     = 0;
  //tftp_state.last_data = NULL;
  //tftp_state.upcb      = pcb;
 data_struct.upcb = pcb;
 udp_recv(pcb, recv, NULL);

 return ERR_OK;
}

/** @ingroup tftp
* Deinitialize ("turn off") TFTP server.
*/
/*void tftp_cleanup(void)
{
  LWIP_ASSERT("Cleanup called on non-initialized TFTP", tftp_state.upcb != NULL);
  udp_remove(tftp_state.upcb);
  close_handle();
  memset(&tftp_state, 0, sizeof(tftp_state));
}*/


void direction(u8_t dir)
{

	if(dir == 1)
	{
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
	}
	if(dir == 0)
	{
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
	}
	HAL_TIM_Base_Start_IT(&htim2);
}






void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	/*if(htim == &htim5){
		__HAL_TIM_GET_COUNTER(&htim5);
		HAL_TIM_Base_Stop_IT(&htim5);
	}*/
    if (htim==&htim2){
        if(GoZeroFlag == 1){
        	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_9);
			cntr = 0;
        }
        else
        {
            HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_9);
            cntr++;

           if(RPMGlobal > 300){
        	   if(rampD_flag != true && cntr % 10 == 0){

                           if(PeriodGlobal > Period){
                           	 //PeriodGlobal --;
                        	   PeriodGlobal *= 0.999;
                           	 top_counter = cntr;
                           }
                           else if(PeriodGlobal <= Period){
                           	rampD_flag = true;
                           }
                           __HAL_TIM_SET_COUNTER(&htim2, 0);
                           __HAL_TIM_SET_AUTORELOAD(&htim2,PeriodGlobal);
        	   }
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
        __HAL_TIM_SET_AUTORELOAD(&htim2,PeriodGlobal);
        RPMGlobal = RPM;
    } else {
        __HAL_TIM_SET_AUTORELOAD(&htim2,Period);
        RPMGlobal = RPM;
    }
}



void ReturnToZero(){
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
    __HAL_TIM_SET_AUTORELOAD(&htim2,70000);
    HAL_TIM_Base_Start_IT(&htim2);
    GoZeroFlag = 1;
}



#endif /* LWIP_UDP */
