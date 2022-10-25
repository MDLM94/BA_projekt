/*
 * json.c
 *
 *  Created on: Oct 24, 2022
 *      Author: mdlm
 */

//includes
#include "json.h"
#include "gpio.h"

void handleLED(void);

/* LwJSON instance and tokens */
static lwjson_token_t tokens[128];
static lwjson_t lwjson;


//for()



/* Parse JSON */ //jsarray[]
void jsonHandler(char jsarray[]) {
    lwjson_init(&lwjson, tokens, LWJSON_ARRAYSIZE(tokens));

    if (lwjson_parse(&lwjson, jsarray) == lwjsonOK) {

    	const lwjson_token_t* t;
    		/* Find custom key in JSON */
    		if((t = lwjson_find(&lwjson, "command")) !=NULL ){
    			const char* command = lwjson_get_val_string(t, (size_t*)&t->u.str.token_value_len);

#if (STEPMOTOR == 1)
				if(strncmp(command, "StepM",strlen("StepM"))==0){
					snprintf(rota, strlen("Command:  ")+strlen("StepM")+1, "Command:  %s", command); // +1 da %s tager en plads
					send_msg(ip_current_src_addr(), server_port, rota);

					handleRunMotor();
					}
#endif
#if ( PUMPMOTOR == 1)
				if(strncmp(command, "runPumpMotor",strlen("runPumpMotor"))==0){
					snprintf(rota, strlen("Command:  ")+strlen("runStepMotor")+1, "Command:  %s", command);
					send_msg(ip_current_src_addr(), server_port, rota);
					//næste funktion
				}
#endif
#if ( LED == 1)
				if(strncmp(command, "LEDinit",strlen("LED"))==0){
					snprintf(rota, strlen("Command:  ")+strlen("LED")+1, "Command:  %s", command);
					send_msg(ip_current_src_addr(), server_port, rota);
					//næste funktion

					handleLED();

				}
#endif
				else{
					send_msg(ip_current_src_addr(), server_port, "No such command available on this board");
				}
			}



		}
    lwjson_free(&lwjson);
}


#if (STEPMOTOR == 1)

void handleRunMotor(void){

	//printf("JSON parsed..\r\n");
	//send_test_msg(aptr, test_port, "JSON parsed..");
	const lwjson_token_t* t;
	/* Find custom key in JSON */
	if((t = lwjson_find(&lwjson, "_params.RPM")) != NULL){
		calcPeriod(lwjson_get_val_int(t));
		sprintf(rota, "Key found with data. RPM: %d", (int)lwjson_get_val_int(t));
		send_msg(ip_current_src_addr(), server_port, rota);
	}

	if ((t = lwjson_find(&lwjson, "_params.deg")) != NULL) {
		//printf("Key found with data type: %d\r\n", (int)t->type);
		rot_deg = roundf(lwjson_get_val_int(t) * 1.111)*8;
		sprintf(rota, "Key found with data. Distance: %d", (int)lwjson_get_val_int(t));
		send_msg(ip_current_src_addr(), server_port, rota);
	}

	if((t = lwjson_find(&lwjson, "_params.dir")) != NULL){
		direction(lwjson_get_val_int(t));
		sprintf(rota, "Key found with data. Direction: %d", (int)lwjson_get_val_int(t));
		send_msg(ip_current_src_addr(), server_port, rota);
	}

}

#endif

#if ( LED == 1 )
void handleLED(void){
	const lwjson_token_t* t;
	/* Find custom key in JSON */
	if((t = lwjson_find(&lwjson, "_params.toggles")) != NULL){
		sprintf(rota, "Key found with data. LED toggles: %d", (int)lwjson_get_val_int(t));
		send_msg(ip_current_src_addr(), server_port, rota);
		for(int i=0; i < (int)lwjson_get_val_int(t); i++){
		                   HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13); // A7
		                   HAL_Delay(500);
		               }
	}

}
#endif



