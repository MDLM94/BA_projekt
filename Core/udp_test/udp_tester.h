#ifndef LWIP_HDR_APPS_TFTP_SERVER_H
#define LWIP_HDR_APPS_TFTP_SERVER_H



#include "lwip/apps/tftp_opts.h"
#include "lwip/err.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/timeouts.h"
#include "lwip/debug.h"

#include "ports.h"

#ifdef __cplusplus
extern "C" {
#endif
extern bool ARR_flag;
extern int PeriodGlobal;
extern float rot_deg;
extern TIM_HandleTypeDef htim1;
extern char* str_ptr;
extern char jsonfile[];
void direction(u8_t dir);
void calcPeriod(u16_t RPM);
void ReturnToZero(void);
void sent_that_shet(void);
extern void send_msg(const ip_addr_t *addr, u16_t port, const char *str);
err_t SPE_data_init();
extern u8_t GoZeroFlag;
/** @ingroup tftp
* TFTP context containing callback functions for TFTP transfers
*/
struct tftp_context {
  /**
   * Open file for read/write.
   * @param fname Filename
   * @param mode Mode string from TFTP RFC 1350 (netascii, octet, mail)
   * @param write Flag indicating read (0) or write (!= 0) access
   * @returns File handle supplied to other functions
   */
  void* (*open)(const char* fname, const char* mode, u8_t write);
  /**
   * Close file handle
   * @param handle File handle returned by open()
   */
  void (*close)(void* handle);
  /**
   * Read from file
   * @param handle File handle returned by open()
   * @param buf Target buffer to copy read data to
   * @param bytes Number of bytes to copy to buf
   * @returns &gt;= 0: Success; &lt; 0: Error
   */
  int (*read)(void* handle, void* buf, int bytes);
  /**
   * Write to file
   * @param handle File handle returned by open()
   * @param pbuf PBUF adjusted such that payload pointer points
   *             to the beginning of write data. In other words,
   *             TFTP headers are stripped off.
   * @returns &gt;= 0: Success; &lt; 0: Error
   */
  int (*write)(void* handle, struct pbuf* p);
};



err_t tftp_init();
void tftp_cleanup(void);



#ifdef __cplusplus
}
#endif



#endif /* LWIP_HDR_APPS_TFTP_SERVER_H */
