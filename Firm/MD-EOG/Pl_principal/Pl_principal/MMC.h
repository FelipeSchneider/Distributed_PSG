/*
 * MMC.h
 *
 * Created: 12/05/2015 13:04:56
 *  Author: Felipe
 */ 


#ifndef MMC_H_
#define MMC_H_

/* Port controls  (Platform dependent) */
#define CS_LOW()	PORTE_OUTCLR = SS_uSD;				//habilita slave		/* CS=low */
#define	CS_HIGH()	PORTE_OUTSET = SS_uSD;				//Desabilita slave			/* CS=high */
#define	FCLK_SLOW()	SPIE_CTRL |= SPI_PRESCALER_DIV128_gc;	/* Set slow clock (F_CPU / 128) */
#define	FCLK_FAST()	SPIE_CTRL |= SPI_CLK2X_bm;				//drobra a freq da SPI 	/* Set fast clock (F_CPU / 64) */
// #define MMC_CD		(!(PINB & 0x10))	/* Card detected.   yes:true, no:false, default:true */
// #define MMC_WP		(PINB & 0x20)		/* Write protected. yes:true, no:false, default:false */

// /* Definitions for MMC/SDC command */		a função send_CMD já faz a soma com 0x40
#define CMD0	(0)			/* GO_IDLE_STATE */
#define CMD1	(1)			/* SEND_OP_COND (MMC) */
#define	ACMD41	(0x80+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(8)			/* SEND_IF_COND */
#define CMD9	(9)			/* SEND_CSD */
#define CMD10	(10)		/* SEND_CID */
#define CMD12	(12)		/* STOP_TRANSMISSION */
#define ACMD13	(0x80+13)	/* SD_STATUS (SDC) */
#define CMD16	(16)		/* SET_BLOCKLEN */
#define CMD17	(17)		/* READ_SINGLE_BLOCK */
#define CMD18	(18)		/* READ_MULTIPLE_BLOCK */
#define CMD23	(23)		/* SET_BLOCK_COUNT (MMC) */
#define	ACMD23	(0x80+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(24)		/* WRITE_BLOCK */
#define CMD25	(25)		/* WRITE_MULTIPLE_BLOCK */
#define CMD32	(32)		/* ERASE_ER_BLK_START */
#define CMD33	(33)		/* ERASE_ER_BLK_END */
#define CMD38	(38)		/* ERASE */
#define CMD55	(55)		/* APP_CMD */
#define CMD58	(58)		/* READ_OCR */

static volatile
DSTATUS Stat = STA_NOINIT;	/* Disk status */

static
BYTE CardType;	/* Card type of the mounted MMC/SDC */

static void power_on(void);
static void power_off(void);
static BYTE xchg_spi(BYTE dat);
static void xmit_spi_multi(const BYTE *p, UINT cnt);
static void rcvr_spi_multi(BYTE *p,UINT cnt);
static int wait_ready(UINT wt);
static void deselect(void);
static int select (void);	/* 1:Successful, 0:Timeout */
static int rcvr_datablock(BYTE *buff, UINT btr);
static int xmit_datablock(const BYTE *buff,	BYTE token);
static BYTE send_cmd(BYTE cmd, DWORD arg);
DSTATUS disk_initialize (BYTE pdrv);
DSTATUS disk_status (BYTE pdrv);
DRESULT disk_read (BYTE pdrv,BYTE *buff,DWORD sector,UINT count);
DRESULT disk_write (BYTE pdrv, const BYTE *buff, DWORD sector, UINT count);
DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void *buff);

#endif /* MMC_H_ */