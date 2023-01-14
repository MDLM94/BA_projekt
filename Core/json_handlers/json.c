/*
 * json.c
 *
 *  Created on: Oct 24, 2022
 *      Author: mdlm
 */

#include "json.h"
#include "testcpp_json.h"
char rota[200];
//using namespace std;
//#include "string.h"
//#include ""

/*
#ifndef __cplusplus
  //#error
#endif*/

/* LwJSON instance and tokens */
static lwjson_token_t tokens[128];
static lwjson_t lwjson;





//Jsonda* jsonda_arr = 0;
int ctr = 0;
/*
void add_device(char* name, enum Emech type){
	ctr++;
	jsonda_arr = (Jsonda*) realloc(jsonda_arr, ctr * sizeof(Jsonda));
	switch(type){
	case stepMotor:
		  jsonda_arr[ctr-1].name = name;
		  jsonda_arr[ctr-1].param_len = 3;
		  jsonda_arr[ctr-1].params[0] = "RPM";
		  jsonda_arr[ctr-1].params[1] = "dist";
		  jsonda_arr[ctr-1].params[2] = "dir";
		break;
	case led:
		  jsonda_arr[ctr-1].name = name;
		  jsonda_arr[ctr-1].param_len = 1;
		  jsonda_arr[ctr-1].params[0] = "toggles";
		break;
	}

}
*/

/*
void send_dev_info(int x){
	char somechar[300] = {0};
	char* p = somechar;
	p += sprintf(p, "[{\"type\":\"%s\"", jsonda_arr[x].name);
	p += sprintf(p,", \"params\":[");
	for( int i = 0; i<jsonda_arr[x].param_len; i++){
		p += sprintf(p, "{\"name\": \"%s\"}", jsonda_arr[x].params[i]);
		if(i < jsonda_arr[x].param_len-1){
			p += sprintf(p, ", ");
		}
		else{
			p += sprintf(p, "]}]");
		}
	}

	send_msg(ip_current_src_addr(), server_port, somechar);

}*/

    //sprintf(somechar, "{\"type\":\"step motor\", \"params\":[{\"name\":\"RPM\", \"rangeMin\":%d, \"rangeMax\":%d}, {\"name\":\"dist\", \"rangeMin\":%d,"
    		//" \"rangeMax\":%d},{\"name\":\"dir\", \"rangeMin\":%d, \"rangeMax\":%d}]}]", eMecha.RPM.min, eMecha.RPM.max, eMecha.dist.min, eMecha.dist.max, eMecha.dir.min, eMecha.dir.max);
   // char somechar[300] = {0};









/* Parse JSON */ //jsarray[]
void jsonHandler(char jsarray[]) {
    lwjson_init(&lwjson, tokens, LWJSON_ARRAYSIZE(tokens));

    if (lwjson_parse(&lwjson, jsarray) == lwjsonOK) {

    	const lwjson_token_t* t;
    		/* Find custom key in JSON */
    		if((t = lwjson_find(&lwjson, "command")) !=NULL ){
    			const char* command = lwjson_get_val_string(t, (size_t*)&t->u.str.token_value_len);


				if(strncmp(command, "StepM",strlen("StepM"))==0){
					snprintf(rota, strlen("Command:  ")+strlen("StepM")+1, "Command:  %s", command); // +1 da %s tager en plads
					send_msg(ip_current_src_addr(), server_port, makeCharMsg(rota));

					handleRunMotor();
					}


				else if(strncmp(command, "runPumpMotor",strlen("runPumpMotor"))==0){
					snprintf(rota, strlen("Command:  ")+strlen("runStepMotor")+1, "Command:  %s", command);
					send_msg(ip_current_src_addr(), server_port, makeCharMsg(rota));
					//næste funktion
				}


				else if(strncmp(command, "LEDinit",strlen("LED"))==0){
					snprintf(rota, strlen("Command:  ")+strlen("LED")+1, "Command:  %s", command);
					char msg = makeCharMsg(rota);
					send_msg(ip_current_src_addr(), server_port, makeCharMsg(rota));
					//næste funktion

					handleLED();

				}

				else{
					send_msg(ip_current_src_addr(), server_port, "No such command available on this board");
				}
			}



		}
    lwjson_free(&lwjson);
}




void handleRunMotor(void){

	//printf("JSON parsed..\r\n");
	//send_test_msg(aptr, test_port, "JSON parsed..");
	const lwjson_token_t* t;
	/* Find custom key in JSON */
	if((t = lwjson_find(&lwjson, "_params.RPM")) != NULL){
		calcPeriod(lwjson_get_val_int(t));
		sprintf(rota, "Key found with data: RPM: %d", (int)lwjson_get_val_int(t));
		//sprintf(rota, *rota + " type: %s", t->type);
		send_msg(ip_current_src_addr(), server_port, makeCharMsg(rota));
	}

	if ((t = lwjson_find(&lwjson, "_params.deg")) != NULL) {
		//printf("Key found with data type: %d\r\n", (int)t->type);
		rot_deg = roundf(lwjson_get_val_int(t) * 1.111)*8;
		sprintf(rota, "Key found with data: Distance: %d", (int)lwjson_get_val_int(t));
		send_msg(ip_current_src_addr(), server_port, makeCharMsg(rota));
	}

	if((t = lwjson_find(&lwjson, "_params.dir")) != NULL){
		direction(lwjson_get_val_int(t));
		sprintf(rota, "Key found with data: Direction: %d", (int)lwjson_get_val_int(t));
		send_msg(ip_current_src_addr(), server_port, makeCharMsg(rota));
	}

}



#if ( LED == 1 )
void handleLED(void){
	const lwjson_token_t* t;
	/* Find custom key in JSON */
	if((t = lwjson_find(&lwjson, "_params.toggles")) != NULL){
		sprintf(rota, "Key found with data. LED toggles: %d", (int)lwjson_get_val_int(t));
		send_msg(ip_current_src_addr(), server_port, rota);
		for(int i=0; i < (int)lwjson_get_val_int(t); i++){
		                   HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7); // A7
		                   HAL_Delay(500);
		               }
	}

}
#endif



