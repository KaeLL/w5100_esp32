
#ifndef _W5100_H_
#define _W5100_H_

#include <stdint.h>

#define MR0	  0x0
#define SHAR0 0x9
#define RMSR  0x1A
#define TMSR  0x1B

#define S0_MR	   0x400
#define S0_CR	   0x401
#define S0_IR	   0x402
#define S0_SR	   0x403
#define S0_TX_FSR0 0x420
#define S0_TX_RD0  0x422
#define S0_TX_WR0  0x424
#define S0_RX_RSR0 0x426
#define S0_RX_RD0  0x428
#define S0_RX_WR0  0x42A

#define MR_RST 0x80 /**< reset */

/* S0_MR values */
#define S0_MR_CLOSE	 0x00 /**< unused socket */
#define S0_MR_MACRAW 0x04 /**< MAC LAYER RAW SOCK */
#define S0_MR_MF	 0x40 /**< Mac Filter */

/* S0_CR values */
#define S0_CR_OPEN	0x01 /**< initialize or open socket */
#define S0_CR_CLOSE 0x10 /**< close socket */
#define S0_CR_SEND	0x20 /**< updata txbuf pointer, send data */
#define S0_CR_RECV	0x40 /**< update rxbuf pointer, recv data */

#define S0_IR_SEND_OK 0x10 /**< complete sending */
#define S0_IR_TIMEOUT 0x08 /**< assert timeout */
#define S0_IR_RECV	  0x04 /**< receiving data */
#define S0_IR_DISCON  0x02 /**< closed socket */
#define S0_IR_CON	  0x01 /**< established connection */

/* S0_SR values */
#define SOCK_CLOSED		 0x00 /**< closed */
#define SOCK_INIT		 0x13 /**< init state */
#define SOCK_LISTEN		 0x14 /**< listen state */
#define SOCK_SYNSENT	 0x15 /**< connection state */
#define SOCK_SYNRECV	 0x16 /**< connection state */
#define SOCK_ESTABLISHED 0x17 /**< success to connect */
#define SOCK_FIN_WAIT	 0x18 /**< closing state */
#define SOCK_CLOSING	 0x1A /**< closing state */
#define SOCK_TIME_WAIT	 0x1B /**< closing state */
#define SOCK_CLOSE_WAIT	 0x1C /**< closing state */
#define SOCK_LAST_ACK	 0x1D /**< closing state */
#define SOCK_UDP		 0x22 /**< udp socket */
#define SOCK_IPRAW		 0x32 /**< ip raw mode socket */
#define SOCK_MACRAW		 0x42 /**< mac raw mode socket */
#define SOCK_PPPOE		 0x5F /**< pppoe socket */

#define getS0_TX_FSR() read_uint16_reg( S0_TX_FSR0 )
#define getS0_RX_RSR() read_uint16_reg( S0_RX_RSR0 )

#define SMASK			0x1FFF /**< Variable for Tx buffer MASK in each channel */
#define RMASK			0x1FFF /**< Variable for Rx buffer MASK in each channel */
#define SSIZE			0x2000 /**< Max Tx buffer size by each channel */
#define RSIZE			0x2000 /**< Max Rx buffer size by each channel */
#define SBUFBASEADDRESS 0x4000 /**< Tx buffer base address by each channel */
#define RBUFBASEADDRESS 0x6000 /**< Rx buffer base address by each channel */

void iinchip_init( void );

uint8_t IINCHIP_READ( uint16_t addr );
void IINCHIP_WRITE( uint16_t addr, uint8_t data );
void wiz_read_buf( uint16_t addr, uint8_t *buf, uint16_t len );
void wiz_write_buf( uint16_t addr, uint8_t *buf, uint16_t len );

uint16_t read_uint16_reg( uint16_t addr );
void write_uint16_reg( uint16_t addr, uint16_t data );

uint8_t getS0_IR( void );

void send_data_processing( uint8_t *data, uint16_t len );
void read_data( uint16_t src_addr, uint8_t *dst, uint16_t len );

#endif
