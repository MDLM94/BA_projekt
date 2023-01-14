#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
//#include "udp_tester.h"
//#include "send_message.h"
//#include "ports.h"
extern char jsonArray[];
extern int rpm;
extern int degrees;
extern int direc;
extern int goZero;
extern int LEDblink;
int maincpp();
//int add_dev(char* devName);
char* makeCharB_info(int b_nr);
char* makeCharMsg(char msg[]);
char* cmd_parser(char* udp_msg, int msg_len);


#ifdef __cplusplus
}
#endif


