#ifndef __FMHEAD__H__
#define __FMHEAD__H__

#ifdef __GNUC__
//#define ALIGN_START(n)
//#define ALIGN_END(n) __attribute__((aligned (n)))
#define ALIGN_START(n)
#define ALIGN_END(n) __attribute__((packed))
#else
//#define ALIGN_START(n) __align(n)
//#define ALIGN_END(n)
#define ALIGN_START(n) __packed
#define ALIGN_END(n)
#endif
#if 0
#ifndef ntohl
#if __BYTE_ORDER__ ==  __ORDER_LITTLE_ENDIAN__
#define ntohl(x) (((x>>24)&0xFF) | ((x>>8)&0xFF00) | ((x<<8)&0xFF0000) | ((x<<24)&0xFF000000))
#define ntohll(x) (((x>>56)&0xFF) | ((x>>40)&0xFF00) | (((x>>24)&0xFF0000) | ((x>>8)&0xFF000000) | ((x<<8)&0xFF00000000) | ((x<<24)&0xFF0000000000)) \
		 | ((x<<40)&0xFF000000000000) | ((x<<56)&0xFF00000000000000))
#define ntohs(x) ((uint16_t)((x>>8))|(uint16_t)(x<<8))
#define htonl(x) ntohl(x)
#define htonll(x) ntohll(x)
#define htons(x) ntohs(x)
#else
#define ntohl(x) (x)
#define ntohll(x) (x)
#define ntohs(x) (x)
#define htonl(x) (x)
#define htonll(x) (x)
#define htons(x) (x)
#endif
#endif
#endif
#define version_a (1)
#define version_b (1)
#define VERSION() ((version_a<<8)|version_b)

#define FM_STX 0x02
#define FM_ACK 0x06

#define FM_DEV_FM 0
#define FM_DEV_ROM 1
#define FM_DEV_RAM 2

#define FM_ERRCODE_OK_NODATA 0x00
#define FM_ERRCODE_OK_HAVEDATA 0x01
#define FM_ERROCDE_ADDR 0x02
#define FM_ERRCODE_LEN 0x03
#define FM_ERRCODE_CRC 0x04
#define FM_ERRCOIDE_CMD 0x05
#define FM_ERRCODE_RW 0x06
#define FM_ERRCODE_NOTSUPPORT 0x07
#define FM_ERRCODE_UNDEFINE 0x10

typedef enum fm_cmd{
	FM_CMD_00=0,
	FM_CMD_TEST=1,
	FM_CMD_READ_SIZE=2,
	FM_CMD_VER=3,
	FM_CMD_MAX_READ=4,
	FM_CMD_MAX_WRITE=5,
	FM_CMD_ERASE_SIZE=6,
	FM_CMD_READ=7,
	FM_CMD_WRITE=8,
	FM_CMD_ERASE=9,
	FM_CMD_CONTINUE_READ=10,
	FM_CMD_CONTINUE_WRITE=11,
	FM_CMD_CONTINUE_ERASE=12,
	FM_CMD_READ_INFO=13,
}enum_fm_cmd;

typedef ALIGN_START(1) struct fmhead{
	uint8_t stx;
	uint8_t fix0;
	uint16_t len;
	uint16_t rlen;
}ALIGN_END(1) str_fmhead;

typedef ALIGN_START(1) struct fmsend{
	uint8_t stx;
	uint8_t fix0;
	uint16_t len;
	uint16_t rlen;
	uint8_t cmd;
	uint8_t dev;
	uint16_t index;
	uint32_t alt;
	uint8_t ndata[0]; // data + crc
}ALIGN_END(1) str_fmsend;

typedef ALIGN_START(1) struct fmAck{
	uint8_t ack;
	uint8_t errcode;
	uint16_t len;
	uint16_t rlen;
	uint8_t ndata[0]; // data + crc
}ALIGN_END(1) str_fmAck;

typedef ALIGN_START(1) struct fmInfo{
	uint8_t fm_type[16];
	uint32_t fm_size;
	uint32_t fm_version;
	uint32_t fm_single_read_size;
	uint32_t fm_single_write_size;
	uint32_t fm_erase_size;
	uint32_t fm_00;
	uint32_t fm_01;
	uint32_t fm_02;
	uint32_t fm_03;
}ALIGN_END(1) str_fmInfo;

#endif


