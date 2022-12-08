#include "testcpp_json.h"
#include "json/json.h"
#include <iostream>
#include <string>
#include <memory>
/** \brief Write the Value object to a stream.
 * Example Usage:
 * $g++ streamWrite.cpp -ljsoncpp -std=c++11 -o streamWrite
 * $./streamWrite
 * {
 *     "Age" : 20,
 *     "Name" : "robin"
 * }
 */
char jsonArray[300];
/*
Json::Value root;
Json::Value type;
Json::Value param;
Json::Value stepM;
Json::Value Led;
int lednr, stepMnr = 0;
int indx = 0;
int n = 0;

int add_dev(char* devName){
	if(strcmp(devName, "Led") == 0){
		Led[lednr][indx] = lednr;
		Led[lednr][indx+=1] = "Hej";
		Led[lednr][indx+=1] = "toggles";
		indx = 0;
		lednr++;

		return 0;
	}
	else if(strcmp(devName, "stepM") == 0){
		stepM[stepMnr][indx] = stepMnr;
		stepM[stepMnr][indx+=1] = "RPM";
		stepM[stepMnr][indx+=1] = "Rots0";
		stepM[stepMnr][indx+=1] = "Dir0";

		//stepM[1][0] = "RPM1";
		//stepM[1][1] = "Rots1";
		indx = 0;
		stepMnr++;
		return 0;
	}
	else {
		return 1;
	}
}
*/

/* Til at tilføje devices i main
Json::Value AddDevice(std::string name)
{
	return devices["devices"][name];
}
*/

char* makeCharMsg(char msg[]/*, std::string b_name*/){

	std::string json_file;

//	  Json::Value board;
	Json::Value board;
    Json::Value devices;

    board["msgType"] = 1;

	board["payload"] = msg;



	Json::FastWriter writer;
	json_file = writer.write(board);


	//lednr = 0;
	return strdup( json_file.c_str());


}

char* makeCharB_info(int b_nr/*, std::string b_name*/){

	std::string json_file;

//	  Json::Value board;
	Json::Value board;
    Json::Value devices;

    board["msgType"] = 23;

//    Json::Value b_Info;
//    Json::Value b_name;
//    Json::Value b_ID;
//    Json::Value vec(Json::arrayValue);



//	vec.append(Json::Value(1));
//	vec.append(Json::Value(2));
//	vec.append(Json::Value(3));

    devices["b_name"] = "StepBoard";
    devices["b_id"] = b_nr;
    //devices["b_IP"] = "192.168.1.123";

    //JValue::Value device = addDevice("type");


    /*
    JSon::Value info;
    info["name"] = "fdfd";

    device.Append(info);
    */
	devices["devices"]["stepMs"][0]["name"] = "Liverpool";
	devices["devices"]["stepMs"][0]["dev_nr"] = 1;
	devices["devices"]["stepMs"][0]["min_max"][0] = 1;
	devices["devices"]["stepMs"][0]["min_max"][1] = 2500;

	devices["devices"]["stepMs"][1]["name"] = "København";
	devices["devices"]["stepMs"][1]["dev_nr"] = 2;
	devices["devices"]["stepMs"][1]["min_max"][0] = 1;
	devices["devices"]["stepMs"][1]["min_max"][1] = 2500;

	devices["devices"]["stepMs"][2]["name"] = "Arsenal";
	devices["devices"]["stepMs"][2]["dev_nr"] = 3;
	devices["devices"]["stepMs"][2]["min_max"][0] = 1;
	devices["devices"]["stepMs"][2]["min_max"][1] = 2500;

/*
	devices["devices"]["stepMs"][1]["name"] = "Arsenal";
	devices["devices"]["stepMs"][1]["dev_nr"] = 1;
	devices["devices"]["stepMs"][1]["min_max"][0] = 1;
	devices["devices"]["stepMs"][1]["min_max"][1] = 2500;
	*/

	devices["devices"]["leds"][0]["name"] = "Manchester";
	devices["devices"]["leds"][0]["dev_nr"] = 1;
	devices["devices"]["leds"][0]["min_max"][0] = 1;
	devices["devices"]["leds"][0]["min_max"][1] = 2500;

	devices["devices"]["Leds"][1]["name"] = "Aarhus";
	devices["devices"]["Leds"][1]["dev_nr"] = 2;
	devices["devices"]["Leds"][1]["min_max"][0] = 1;
	devices["devices"]["Leds"][1]["min_max"][1] = 2500;



//	board["Devices"] = devices;
	//board["B_info"] = b_Info;
	board["payload"] = devices;

//	devices["leds"]["away"]["code"] = 89223;
//	devices["competitors"]["away"]["name"] = "Aston Villa";
//	devices["competitors"]["away"]["code"]=vec;


	Json::FastWriter writer;
	json_file = writer.write(board);


	//lednr = 0;
	return strdup( json_file.c_str());


}

//int maincpp() {
  //constexpr bool shouldUseOldWay = true;
  /*
  Json::Value root;
  Json::Value data;

  root["action"] = "run";
  root["action"] = "run1";
  data["number"] = 1;
  root["data"] = data;

  Json::Value type;
  Json::Value param;
  Json::Value stepM;
  Json::Value Led;
   */



  //type["Devices"] = type;

  //ch[0][1] = "RPM";

  //ch[1]["name"] = "Cheshire Cat";
  //ch[1]["chapter"] = 6;
//ch[2]["name"] = "Mad Hatter";
//ch[2]["chapter"] = 7;


  //Json::Value boardID;
  //boardID["B_ID"] = 1;
/*
  param["StepM"]["RPM"] = 17;
 // param["StepM"]["PWM"] = 18;
 // param["StepM"]["Forward"] = 19;

  param["LED"]["Toggles"] = 20;
  param["LED"]["Duration"] = 21;

  type["Devices"] = param;
  type["BoardID"] = 1;
*/


 /*

  if (shouldUseOldWay) {
    Json::FastWriter writer;
    json_file = writer.write(type);
    std::cout << json_file << std::endl;
  } else {
    Json::StreamWriterBuilder builder;
    json_file = Json::writeString(builder, type);
    std::cout << json_file << std::endl;
  }
*/
  //int n = json_file.length();




  /*for (int i = 0; i < n; i++)
	  std::cout << jsonArray[i];
	*/
  //return EXIT_SUCCESS;
//}

