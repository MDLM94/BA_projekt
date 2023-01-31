#include "testcpp_json.h"
#include "json/json.h"
#include <iostream>
#include <string>
#include <memory>
#include <cmath>
#include "defines.h"
#include "gpio.h"

//char jsonArray[300];
Json::Value devices;

int rpm;
int degrees;
int direc;
int LEDblink;
int goZero;

void handleRunMotor(Json::Value root){


	rpm = root["_params"]["RPM"].asInt();

	degrees = root["_params"]["deg"].asInt();

	direc = root["_params"]["dir"].asInt();

	goZero = root["GoZero"].asInt();

}



char* cmd_parser(char* udp_msg, int msg_len){

	Json::Reader reader;
	Json::Value root;

	std::string text(udp_msg, msg_len);




	bool parseErrorCheck = reader.parse(text, root);

	if(!parseErrorCheck){
		//lav  error handling
	}


	//får key value som string
	const std::string checkValue = root["command"].asString();
	std::string msg;
	if(checkValue == "StepM"){
		msg = checkValue;
		handleRunMotor(root);
	}
	else if(checkValue == "LED"){
		msg = checkValue;
		LEDblink = root["_params"]["toggles"].asInt();
	}

	return strdup( msg.c_str());
}


void add_stepM(std::string name, int dev_nr, int min, int max){
	Json::Value info_StepM;
	info_StepM["name"] = name;
	info_StepM["dev_nr"] = dev_nr;
	info_StepM["min_max"][0] = min;
	info_StepM["min_max"][1] = max;

	 devices["devices"]["stepMs"].append(info_StepM);

}


void add_LED(std::string name, int dev_nr, int min, int max){
	Json::Value info_LED;
	info_LED["name"] = name;
	info_LED["dev_nr"] = dev_nr;
	info_LED["min_max"][0] = min;
	info_LED["min_max"][1] = max;

	 devices["devices"]["leds"].append(info_LED);

}


char* makeCharMsg(char* msg){

	std::string json_file;

	Json::Value board;

    board["msgType"] = 1;

    board["info"] = msg;

	//board["payload"] = devices;


	Json::FastWriter writer;
	json_file = writer.write(board);


	//lednr = 0;
	return strdup( json_file.c_str());


}

char* makeCharB_info(int b_nr/*, std::string b_name*/){

	std::string json_file;

	Json::Value board;


    board["msgType"] = 23;

#if switch_board_1 == 1
    devices["b_name"] = "StepBoard_1";
#elif switch_board_2 == 1
    devices["b_name"] = "StepBoard_2";
#endif
    devices["b_id"] = b_nr;
    //hvis ingen value er assignet vil array være tomt
    devices["devices"]["stepMs"] = Json::arrayValue;;
    devices["devices"]["leds"] = Json::arrayValue;;
#if switch_board_1 == 1
    add_stepM("Jytte", 1, 1, 1500);
    add_stepM("Henning", 2, 1, 1500);
    add_LED("Ronaldo", 1, 1, 40000);
#elif switch_board_2 == 1
    add_stepM("FIC1", 1, 1, 1000);
    add_stepM("FIC2", 2, 1, 1200);
    add_LED("FIC3", 1, 1, 3000);
#endif




//	board["Devices"] = devices;
	//board["B_info"] = b_Info;
	board["payload"] = devices;



	Json::FastWriter writer;
	json_file = writer.write(board);


	return strdup( json_file.c_str());


}



