/*
 * json.h
 *
 *  Created on: Oct 24, 2022
 *      Author: mdlm
 */

#ifndef JSON_HANDLERS_JSON_H_
#define JSON_HANDLERS_JSON_H_


//includes
#include "defines.h"
#include "udp_tester.h"
#include "lwjson.h"
#include "math.h"
#include "ports.h"
#include "gpio.h"

enum Emech{
		stepMotor,
		pumpMotor,
		led,
};

typedef struct{
	char* name;
	enum Emech type;
	int param_len;
	char* params[10];

}Jsonda;

#ifdef __cplusplus
extern "C" {
#endif

void add_device(char* name, enum Emech type);
void jsonHandler(char jsarray[]);
void handleRunMotor(void);
void handleLED(void);
void send_dev_info(int x);

#ifdef __cplusplus
}
#endif


extern int ctr;



#endif /* JSON_HANDLERS_JSON_H_ */
