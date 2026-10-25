#ifndef AIR_ETH_XSGMII_EOSCAN
#define AIR_ETH_XSGMII_EOSCAN

// en7581 pma split function
#define Pon_Eye_Scan_Result  0
#define LAB_PRINT_PON 0
#define LAB_PON_Test  0
#define Pon_PR_WK 0

#define fm_Async_GPON     0   //fm_DS_2.48832G  /  US_1.24416G
#define fm_Sync_EPON_1    1   //fm_DS_1.25G     /  US_1.25G
#define fm_Sync_EPON_2    8   //fm_DS_1.25G     /  US_1.25G
#define fm_Async_XEPON    6   //fm_DS_10.3125G  /  US_1.25G
#define fm_Sync_XEPON     7   //fm_DS_10.3125G  /  US_10.3125G
#define fm_Async_XGPON    9   //fm_DS_9.95328G  /  US_2.48832G
#define fm_Sync_XGSPON_1  10   //fm_DS_9.95328G  /  US_9.95328G
#define fm_Sync_XGSPON_2  11   //fm_DS_9.95328G  /  US_9.95328G

#define fm_Sync_GPON      99   //fm_DS_2.48832G  /  US_2.48832G

#define scu_Async_GPON    1   //DS_2.48832G  /  US_1.24416G
#define scu_Sync_EPON     2   //DS_1.25G      /  US_1.25G
#define scu_Async_XEPON   3   //DS_10.3125G  /  US_1.25G
#define scu_Sync_XEPON    4   //DS_10.3125G  /  US_10.3125G
#define scu_Async_XGPON   5   //DS_9.95328G  /  US_2.48832G
#define scu_Sync_XGSPON   6   //DS_9.95328G  /  US_9.95328G
#define scu_Sync_GPON     7   //DS_2.48832G  /  US_2.48832G

//define_Bit Rate
#define Async_GPON        0   //DS(RX)_2.48832G  /  US(TX)_1.24416G , SCU_WAN_CONF_REG_WAN_SEL_GPON
	    
#define Sync_EPON_1       1   //DS_1.25G      /  US_1.25G                  , SCU_WAN_CONF_REG_WAN_SEL_EPON
#define Sync_EPON_2       8   //DS_1.25G      /  US_1.25G                  , SCU_WAN_CONF_REG_WAN_SEL_XEPON_1G_1G
	    
#define Async_XEPON       6   //DS_10.3125G  /  US_1.25G                  , SCU_WAN_CONF_REG_WAN_SEL_XEPON_10G_1G
	    
#define Sync_XEPON        7   //DS_10.3125G  /  US_10.3125G              , SCU_WAN_CONF_REG_WAN_SEL_XEPON_10G_10G
	    
#define Async_XGPON_1     9  //DS_9.95328G  /  US_2.48832G              , SCU_WAN_CONF_REG_WAN_SEL_XGPON
#define Async_XGPON_2     12  //DS_9.95328G  /  US_2.48832G              , SCU_WAN_CONF_REG_WAN_SEL_NGPON2_10G_2G
	    
#define Sync_XGSPON_1     10   //DS_9.95328G  /  US_9.95328G              , SCU_WAN_CONF_REG_WAN_SEL_XGSPON
#define Sync_XGSPON_2     11  //DS_9.95328G  /  US_9.95328G              , SCU_WAN_CONF_REG_WAN_SEL_NGPON2_10G_10G
	    
#define Sync_GPON         99   //DS_2.48832G  /  US_2.48832G

#define  bist_data         1
#define  r2t_data          2
#define  nor_pma_data      3
 
#define  FIRST_PLUG_IN     1
#define  PLUG_IN           2
#define  PLUG_OUT          3
		 
#define  TX_Async_GPON     1   //DS_2.48832G  /  US_1.24416G   
#define  TX_Sync_EPON      2   //DS_1.25G      /  US_1.25G     
#define  TX_Async_XEPON    3   //DS_10.3125G  /  US_1.25G       
#define  TX_Sync_XEPON     4   //DS_10.3125G  /  US_10.3125G   
#define  TX_Async_XGPON    5   //DS_9.95328G  /  US_2.48832G
#define  TX_Sync_XGSPON    6   //DS_9.95328G  /  US_9.95328G   
#define  TX_Sync_GPON      7   //DS_2.48832G  /  US_2.48832G

#define  EO_Async_GPON     0    //DS(RX)_2.48832G  /  US(TX)_1.24416G
		 
#define  EO_Sync_EPON_1    1    //DS_1.25G       /  US_1.25G
#define  EO_Sync_EPON_2    8    //DS_1.25G       /  US_1.25G
		 
#define  EO_Async_XEPON    6    //DS_10.3125G   /  US_1.25G
		 
#define  EO_Sync_XEPON     7    //DS_10.3125G   /  US_10.3125G
		 
#define  EO_Async_XGPON_1  9   //DS_9.95328G   /  US_2.48832G
#define  EO_Async_XGPON_2  12   //DS_9.95328G   /  US_2.48832G
		 
#define  EO_Sync_XGSPON_1   10    //DS_9.95328G   /  US_9.95328G
#define  EO_Sync_XGSPON_2   11   //DS_9.95328G   /  US_9.95328G
		 
#define  EO_Sync_GPON      99   //DS_2.48832G   /  US_2.48832G

#endif