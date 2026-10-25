#ifndef __ECNT_HSDMA_H
#define __ECNT_HSDMA_H

#define ECNT_HSDMA_TX				0x1
#define ECNT_HSDMA_RX				0x2
#define ECNT_HSDMA_CPU				0x4
#define ECNT_HSDMA_DMA				0x8

#define ECNT_HSDMA_USEC_POLL		20
#define ECNT_HSDMA_TIMEOUT_POLL		200000
#define ECNT_HSDMA_DMA_BUSWIDTHS	(BIT(DMA_SLAVE_BUSWIDTH_1_BYTE)		| \
									 BIT(DMA_SLAVE_BUSWIDTH_2_BYTES)	| \
									 BIT(DMA_SLAVE_BUSWIDTH_3_BYTES)	| \
									 BIT(DMA_SLAVE_BUSWIDTH_4_BYTES)	| \
									 BIT(DMA_SLAVE_BUSWIDTH_8_BYTES)	| \
									 BIT(DMA_SLAVE_BUSWIDTH_16_BYTES))

/* The default number of virtual channel */
#define ECNT_HSDMA_NR_VCHANS		2

/* Only two physical channel supported */
#define ECNT_HSDMA_NR_MAX_PCHANS	2

/* Macro for physical descriptor (PD) manipulation */
/* The number of PD which must be 2 of power */
#define ECNT_DMA_SIZE				2048
#define ECNT_HSDMA_NEXT_DESP_IDX(x, y)	(((x) + 1) & ((y) - 1))
#define ECNT_HSDMA_LAST_DESP_IDX(x, y)	(((x) - 1) & ((y) - 1))
#define ECNT_HSDMA_MAX_LEN			0xFFFF
#define ECNT_HSDMA_ALIGN_SIZE		DMAENGINE_ALIGN_1_BYTE
#define ECNT_HSDMA_PLEN_MASK		0xFFFF
#define ECNT_HSDMA_DESC_PLEN(x)		((x) & ECNT_HSDMA_PLEN_MASK)
#define ECNT_HSDMA_DESC_PLEN_GET(x)	((x) & ECNT_HSDMA_PLEN_MASK)

#define ECNT_HSDMA_PHYS				0x1FA01800

/* Registers for underlying ring manipulation */
#define ECNT_HSDMA_TX_BASE			0x0
#define ECNT_HSDMA_TX_CNT			0x4
#define ECNT_HSDMA_TX_CPU			0x8
#define ECNT_HSDMA_TX_DMA			0xC
#define ECNT_HSDMA_TX_BASE_1		0x10
#define ECNT_HSDMA_TX_CNT_1			0x14
#define ECNT_HSDMA_TX_CPU_1			0x18
#define ECNT_HSDMA_TX_DMA_1			0x1C

#define ECNT_HSDMA_RX_BASE			0x100
#define ECNT_HSDMA_RX_CNT			0x104
#define ECNT_HSDMA_RX_CPU			0x108
#define ECNT_HSDMA_RX_DMA			0x10C
#define ECNT_HSDMA_RX_BASE_1		0x110
#define ECNT_HSDMA_RX_CNT_1			0x114
#define ECNT_HSDMA_RX_CPU_1			0x118
#define ECNT_HSDMA_RX_DMA_1			0x11c

#define ECNT_HSDMA_INFO				0x200

/* Registers for global setup */
#define ECNT_HSDMA_GLO				0x204
#define ECNT_HSDMA_GLO_BYTE_SWAP	BIT(29)
#define ECNT_HSDMA_GLO_READ_BACK	BIT(11)
#define ECNT_HSDMA_TX_WB_DDONE		BIT(6)
#define ECNT_HSDMA_BURST_16BYTES	(0x0 << 4)
#define ECNT_HSDMA_BURST_32BYTES	(0x1 << 4)
#define ECNT_HSDMA_BURST_64BYTES	(0x2 << 4)
#define ECNT_HSDMA_BURST_128BYTES	(0x3 << 4)
#define ECNT_HSDMA_GLO_RX_BUSY		BIT(3)
#define ECNT_HSDMA_GLO_RX_DMA		BIT(2)
#define ECNT_HSDMA_GLO_TX_BUSY		BIT(1)
#define ECNT_HSDMA_GLO_TX_DMA		BIT(0)
#define ECNT_HSDMA_GLO_DMA			(ECNT_HSDMA_GLO_TX_DMA		| \
									 ECNT_HSDMA_GLO_RX_DMA)
#define ECNT_HSDMA_GLO_BUSY			(ECNT_HSDMA_GLO_RX_BUSY		| \
									 ECNT_HSDMA_GLO_TX_BUSY)
#define ECNT_HSDMA_GLO_DEFAULT		(ECNT_HSDMA_GLO_TX_DMA		| \
									 ECNT_HSDMA_GLO_RX_DMA		| \
									 ECNT_HSDMA_TX_WB_DDONE		| \
									 ECNT_HSDMA_BURST_128BYTES	| \
									 ECNT_HSDMA_GLO_READ_BACK)

/* Registers for reset */
#define ECNT_HSDMA_RESET			0x208
#define ECNT_HSDMA_RST_TX			BIT(0)
#define ECNT_HSDMA_RST_TX_1			BIT(1)
#define ECNT_HSDMA_RST_RX			BIT(16)
#define ECNT_HSDMA_RST_RX_1			BIT(17)
#define ECNT_HSDMA_RST_ALL			(ECNT_HSDMA_RST_RX			| \
									 ECNT_HSDMA_RST_RX_1		| \
									 ECNT_HSDMA_RST_TX			| \
									 ECNT_HSDMA_RST_TX_1)

/* Registers for free queue thershold */
#define ECNT_HSDMA_FRQ_THR			0x210

/* Registers for interrupt control */
#define ECNT_HSDMA_DLYINT			0x20C
#define ECNT_HSDMA_TXDLY_INT_EN		BIT(31)
#define ECNT_HSDMA_RXDLY_INT_EN		BIT(15)

/* Interrupt fires when the pending number's more than the specified */
#define ECNT_HSDMA_TXMAX_PINT(x)	(((x) & 0x7f) << 24)
#define ECNT_HSDMA_RXMAX_PINT(x)	(((x) & 0x7f) << 8)

/* Interrupt fires when the pending time's more than the specified in 20 us */
#define ECNT_HSDMA_TXMAX_PTIME(x)	(((x) & 0xff) << 16)
#define ECNT_HSDMA_RXMAX_PTIME(x)	((x) & 0xff)
#define ECNT_HSDMA_DLYINT_DEFAULT	(ECNT_HSDMA_RXDLY_INT_EN	| \
									 ECNT_HSDMA_RXMAX_PINT(20)	| \
									 ECNT_HSDMA_RXMAX_PTIME(20))

#define ECNT_HSDMA_INT_STATUS		0x220
#define ECNT_HSDMA_INT_ENABLE		0x228
#define ECNT_HSDMA_INT_TXDONE		BIT(0)
#define ECNT_HSDMA_INT_TXDONE_1		BIT(1)
#define ECNT_HSDMA_INT_RXDONE		BIT(16)
#define ECNT_HSDMA_INT_RXDONE_1		BIT(17)
#define ECNT_HSDMA_INT_TXDLY		BIT(28)
#define ECNT_HSDMA_INT_TXCOHER		BIT(29)
#define ECNT_HSDMA_INT_RXDLY		BIT(30)
#define ECNT_HSDMA_INT_RXCOHER		BIT(31)
#define ECNT_HSDMA_INT_ALL			(ECNT_HSDMA_INT_TXDONE		| \
									 ECNT_HSDMA_INT_TXDONE_1	| \
									 ECNT_HSDMA_INT_RXDONE		| \
									 ECNT_HSDMA_INT_RXDONE_1	| \
									 ECNT_HSDMA_INT_TXDLY		| \
									 ECNT_HSDMA_INT_TXCOHER		| \
									 ECNT_HSDMA_INT_RXDLY		| \
									 ECNT_HSDMA_INT_RXCOHER)

#define ECNT_HSDMA_DBG0				0x230
#define ECNT_HSDMA_DBG1				0x234
#define ECNT_HSDMA_DBG2				0x238
#define ECNT_HSDMA_DBG3				0x23C

enum ecnt_hsdma_vdesc_flag {
	ECNT_HSDMA_VDESC_FINISHED	= 0x01,
};

#define IS_ECNT_HSDMA_VDESC_FINISHED(x) ((x) == ECNT_HSDMA_VDESC_FINISHED)

/**
 * struct ecnt_hsdma_pdesc - This is the struct holding info describing physical
 *			    descriptor (PD) and its placement must be kept at
 *			    4-bytes alignment in little endian order.
 * @desc[1-4]:		    The control pad used to indicate hardware how to
 *			    deal with the descriptor such as source and
 *			    destination address and data length. The maximum
 *			    data length each pdesc can handle is 0x3f80 bytes
 */
struct ecnt_hsdma_pdesc {
	__le32 desc1;
	__le32 desc2;
	__le32 desc3;
	__le32 desc4;
	__le32 desc5;
	__le32 desc6;
	__le32 desc7;
	__le32 desc8;
} __packed __aligned(32);

/**
 * struct ecnt_hsdma_vdesc - This is the struct holding info describing virtual
 *			    descriptor (VD)
 * @vd:			    An instance for struct virt_dma_desc
 * @len:		    The total data size device wants to move
 * @residue:		    The remaining data size device will move
 * @dest:		    The destination address device wants to move to
 * @src:		    The source address device wants to move from
 */
struct ecnt_hsdma_vdesc {
	struct virt_dma_desc vd;
	size_t len;
	size_t residue;
	dma_addr_t dest;
	dma_addr_t src;
};

/**
 * struct ecnt_hsdma_cb - This is the struct holding extra info required for RX
 *			 ring to know what relevant VD the the PD is being
 *			 mapped to.
 * @vd:			 Pointer to the relevant VD.
 * @flag:		 Flag indicating what action should be taken when VD
 *			 is completed.
 */
struct ecnt_hsdma_cb {
	struct virt_dma_desc *vd;
	enum ecnt_hsdma_vdesc_flag flag;
};

/**
 * struct ecnt_hsdma_ring - This struct holds info describing underlying ring
 *			   space
 * @txd:		   The descriptor TX ring which describes DMA source
 *			   information
 * @rxd:		   The descriptor RX ring which describes DMA
 *			   destination information
 * @cb:			   The extra information pointed at by RX ring
 * @tphys:		   The physical addr of TX ring
 * @rphys:		   The physical addr of RX ring
 * @cur_tptr:		   Pointer to the next free descriptor used by the host
 * @cur_rptr:		   Pointer to the last done descriptor by the device
 */
struct ecnt_hsdma_ring {
	struct ecnt_hsdma_pdesc *txd;
	struct ecnt_hsdma_pdesc *rxd;
	struct ecnt_hsdma_cb *cb;
	dma_addr_t tphys;
	dma_addr_t rphys;
	u16 cur_tptr;
	u16 cur_rptr;
};

/**
 * struct ecnt_hsdma_pchan - This is the struct holding info describing physical
 *			   channel (PC)
 * @ring:		   An instance for the underlying ring
 * @sz_ring:		   Total size allocated for the ring
 * @nr_free:		   Total number of free rooms in the ring. It would
 *			   be accessed and updated frequently between IRQ
 *			   context and user context to reflect whether ring
 *			   can accept requests from VD.
 */
struct ecnt_hsdma_pchan {
	struct ecnt_hsdma_ring ring;
	int chan_num;
	size_t sz_ring;
	atomic_t nr_free;

	/* Lock used to protect against multiple VCs access PC */
	spinlock_t lock;

	u32 tx_base;
	u32 tx_cnt;
	u32 tx_cpu;
	u32 tx_dma;
	u32 rx_base;
	u32 rx_cnt;
	u32 rx_cpu;
	u32 rx_dma;
	u32 reset;
	u32 intr;
};

/**
 * struct ecnt_hsdma_vchan - This is the struct holding info describing virtual
 *			   channel (VC)
 * @vc:			   An instance for struct virt_dma_chan
 * @issue_completion:	   The wait for all issued descriptors completited
 * @issue_synchronize:	   Bool indicating channel synchronization starts
 * @desc_hw_processing:	   List those descriptors the hardware is processing,
 *			   which is protected by vc.lock
 */
struct ecnt_hsdma_vchan {
	struct virt_dma_chan vc;
	struct completion issue_completion;
	bool issue_synchronize;
	struct list_head desc_hw_processing;
};

/**
 * struct ecnt_hsdma_soc - This is the struct holding differences among SoCs
 * @ddone:		  Bit mask for DDONE
 * @nls:		  Bit mask for NLS
 */
struct ecnt_hsdma_soc {
	__le32 ddone;
	__le32 nls;
};

/**
 * struct ecnt_hsdma_device - This is the struct holding info describing HSDMA
 *			     device
 * @ddev:		     An instance for struct dma_device
 * @base:		     The mapped register I/O base
 * @irq:		     The IRQ that device are using
 * @dma_requests:	     The number of VCs the device supports to
 * @vc:			     The pointer to all available VCs
 * @pc:			     The pointer to the underlying PC
 * @pc_refcnt:		     Track how many VCs are using the PC
 * @lock:		     Lock protect agaisting multiple VCs access PC
 * @soc:		     The pointer to area holding differences among
 *			     vaious platform
 */
struct ecnt_hsdma_device {
	struct dma_device ddev;
	void __iomem *base;
	u32 irq;

	u32 dma_requests;
	struct ecnt_hsdma_vchan *vc;
	struct ecnt_hsdma_pchan *pc;
	atomic_t pc_refcnt;

	const struct ecnt_hsdma_soc *soc;
};

extern void ecnt_hsdma_reg_dump(void);

extern struct ecnt_hsdma_ring *ecnt_hsdma_get_ring(int chan_num);
extern void ecnt_hsdma_disable_test_mode(u32 old_cfg, u32 old_intr_dly);
extern void ecnt_hsdma_enable_test_mode(u32 *old_cfg, u32 *old_intr_dly, u32 *old_intr, __le32 *ddone, __le32 *nls);
extern void ecnt_hsdma_free_dma(u32 buf_size, u8 *src_unc, dma_addr_t src_phys);
extern u8 *ecnt_hsdma_alloc_dma(u32 buf_size, dma_addr_t *src_phys);
extern u32 ecnt_hsdma_get_cfg(void);
extern void ecnt_hsdma_disable_dly_intr(int drct);
extern void ecnt_hsdma_enable_dly_intr(int drct, u8 count, u8 times);
extern void ecnt_hsdma_disable_rdbk(void);
extern void ecnt_hsdma_enable_rdbk(void);
extern void ecnt_hsdma_disable_tx_wbd(void);
extern void ecnt_hsdma_enable_tx_wbd(void);
extern void ecnt_hsdma_disable_swap(void);
extern void ecnt_hsdma_enable_swap(void);
extern void ecnt_hsdma_set_burst_size(u32 size);
extern void ecnt_hsdma_clear_intr_sts(u32 intr);
extern u32 ecnt_hsdma_get_intr_sts(void);
extern u32 ecnt_hsdma_get_intr(void);
extern void ecnt_hsdma_disable_intr(u32 intr);
extern void ecnt_hsdma_enable_intr(u32 intr);
extern void ecnt_hsdma_set_idx(int chan_num, int drct, int type, u32 idx);
extern u32 ecnt_hsdma_get_idx(int chan_num, int drct, int type);
extern void ecnt_hsdma_reset(void);
extern void ecnt_hsdma_stop_dma(u32 value);
extern void ecnt_hsdma_start_dma(u32 value);
#endif

