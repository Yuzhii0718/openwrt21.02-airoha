#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>

//#define DEBUG
#define VPint			*(volatile unsigned long int *)

#define S_32      0x20
#define S_32K     0x8000
#define S_64K     0x10000
#define S_256K    0x40000
#define S_512K    0x80000
#define S_1M      0x100000
#define S_2M      0x200000

typedef enum {
	ORI_PAT,
	INCR_PAT,
	ANTI_INCR_PAT,
} patType;


int ret = 0;
unsigned char defWByte[] = {1, 2 , 4 , 0};
unsigned char defRByte[] = {1, 2 , 4 , 0};
unsigned char defPat[] = {0x55, 0xaa, 0x5a, 0xa5, 0x00, 0xff};
unsigned char pat;
unsigned int startAddr[] = {0xbfa40000, 0}; /*GDMP SRAM: 0xbfa40000*/
unsigned int testSize = S_32K;
unsigned long size;
int count=0;
unsigned long swRegs[] = {0xbfb00240, 0xbfb00244, 0xbfb00280, 0xbfb00284, 0};

#ifdef TCSUPPORT_LITTLE_ENDIAN
#define SWAP_2B(x)  ( (((x)&0xff)<<8) | (((x)&0xff00)>>8) )
#define SWAP_4B(x)  ( (((x)&0xff)<<24) | (((x)&0xff00)<<8) | (((x)&0xff0000)>>8) | (((x)&0xff000000)>>24) )
#else
#define SWAP_2B(x)  (x)
#define SWAP_4B(x)  (x)
#endif

extern void flush_dcache_range(unsigned long start, unsigned long end);

static int dram_pat_set(void *startAddr, unsigned long size, unsigned long pattern, int patType,unsigned char wByte)
{
	unsigned char *addr1;
	unsigned char pat1;
	unsigned short *addr2;
	unsigned short pat2;
	unsigned long *addr4;
	unsigned long pat4;
	unsigned char antiPat1;
	unsigned char antiPat2;
	unsigned char antiPat4;
	int err=0;

	switch(wByte)
	{
		case 4:
			pat4 = (unsigned long)(pattern & 0xffffffff);
			addr4 = (unsigned long*)startAddr;
			size = size >> 2;
			break;
		case 2:
			pat2 = (unsigned short)(pattern & 0xffff);
			addr2 = (unsigned short*)startAddr;
			size = size >> 1;
			break;
		case 1:
			pat1 = (unsigned char)(pattern & 0xff);
			addr1 = (unsigned char*)startAddr;
			break;
		default:
        #ifdef DEBUG
			printk("dram_pat_set: ERROR! wByte=");
            printk(wByte);
            printk("\n");
        #endif
			err=-1;;
	}


	while(size&&(err!=-1))
	{
		switch(wByte)
		{
			case 4:
				if(patType == ANTI_INCR_PAT)
				{
					antiPat4 = ~pat4;
					*addr4 = antiPat4;
				}
				else
				{
					*addr4 = pat4;
				}

                #ifdef DEBUG
				printk("pat set\naddr:0x");
                printk(addr4,8);
                printk("\n");
				printk("\ndata:0x");
                printk(*addr4,8);
                printk("\n");
			    #endif

			    addr4++;
                
				if((patType == INCR_PAT) || (patType == ANTI_INCR_PAT))
				{
					pat4++;
				}
				break;
			case 2:
				if(patType == ANTI_INCR_PAT)
				{
					antiPat2 = ~pat2;
					*addr2 = antiPat2;
				}
				else
				{
					*addr2 = pat2;
				}
				#ifdef DEBUG
				printk("pat set\naddr:");
				printk(addr2,8);
				printk("\ndata:");
				printk(*addr2,4);
				printk("\n");
				#endif

			    addr2++;

				if((patType == INCR_PAT) || (patType == ANTI_INCR_PAT))
				{
					pat2++;
				}
				break;
			case 1:
				if(patType == ANTI_INCR_PAT)
				{
					antiPat1 = ~pat1;
					*addr1 = antiPat1;
				}
				else
				{
					*addr1 = pat1;
				}

			    addr1++;

				if((patType == INCR_PAT) || (patType == ANTI_INCR_PAT))
				{
					pat1++;
				}
				break;
			default:
            #ifdef DEBUG
				printk("dram_pat_set: ERROR! wByte=");
                printk(wByte);
                printk("\n");
            #endif
				err=-1;
		}

		size--;
	}

	return err;
}

static int dram_pat_cmp(void *startAddr, unsigned long size, unsigned long pattern, unsigned char wByte, unsigned char rByte)
{
	unsigned char *addr1;
	unsigned char pat1;
	unsigned short *addr2;
	unsigned short pat2;
	unsigned long *addr4;
	unsigned long pat4;
	unsigned long wPat;
	unsigned char pNum;
	unsigned long rPat[4] = {0};
	int i, n, err=0;
	

	#ifdef DEBUG
	printk("dram_pat_cmp\n");
	printk(startAddr,8);
	printk("\n");
	printk(size,8);
	printk("\n");
	printk(pattern);
	printk("\n");
	printk(wByte);
	printk("\n");
	printk(rByte);
	printk("\n");
	#endif
	switch(wByte)
	{
		case 4:
			wPat = pattern & 0xffffffff;
			break;
		case 2:
			wPat = pattern & 0xffff;
			break;
		case 1:
			wPat = pattern & 0xff;
			break;
		default:
            #ifdef DEBUG
			printk("dram_pat_cmp: ERROR! wByte=");
            printk(wByte);
            printk("\n");
            #endif
			err=-1;
	}

	switch(rByte)
	{
		case 4:
			addr4 = (unsigned long*)startAddr;
			size = size >> 2;
			break;
		case 2:
			addr2 = (unsigned short*)startAddr;
			size = size >> 1;
			break;
		case 1:
			addr1 = (unsigned char*)startAddr;
			break;
		default:
            #ifdef DEBUG
			printk("dram_pat_cmp: ERROR! read byte=");
            printk(rByte);
            printk("\n");
            #endif
			err=-1;
	}

	if(wByte > rByte)
	{
		pNum = wByte / rByte;
		for(i=pNum-1; i>=0 ;i--)
		{
			switch(rByte)
			{
				case 2:
					rPat[i] = wPat & 0xffff;
					wPat = wPat >> 16;
					break;
				case 1:
					rPat[i] = wPat & 0xff;
					wPat = wPat >> 8;
					break;
				default:
                    #ifdef DEBUG
					printk("dram_pat_cmp: ERROR! read byte=");
                    printk(rByte);
                    printk("\n");
                    #endif
					err=-1;
			}
		}
	}
	else if(wByte < rByte)
	{
		pNum = 1;
		n = rByte / wByte;
		for(i=0; i<(n-1); i++)
		{
			switch(wByte)
			{
				case 2:
					wPat = (wPat << 16) | wPat;
					break;
				case 1:
					wPat = (wPat << 8) | wPat;
					break;
				default:
                    #ifdef DEBUG
					printk("dram_pat_cmp: ERROR! wByte=");
                    printk(wByte);
                    printk("\n");
                    #endif
					err=-1;
			}
		}
		rPat[0] = wPat;
	}
	else
	{
		pNum = wByte / rByte;
		rPat[0] = wPat;
	}

	i = 0;
    
	while(size&&(err!=-1))
	{
		switch(rByte)
		{
			case 4:
				pat4 = (unsigned long) rPat[i] & 0xffffffff;
				if(*addr4 != pat4)
				{
				    #ifdef DEBUG
                    printk("\nerror: rByte:4\n");
                    printk("*addr:0x");
                    printk(*addr4,8);
                    printk(" != pat4:0x");
                    printk(pat4,8);
                    printk(" at addr:0x");
                    printk(addr4,8);
                    printk("\n\n");
                    #endif
                    err=-1;
				}
			    addr4++;
                
				break;
			case 2:
				pat2 = (unsigned short) rPat[i] & 0xffff;
				if(*addr2 != pat2)
				{
                    #ifdef DEBUG
                    printk("\nerror: rByte:2\n");
                    printk("*addr:0x");
                    printk(*addr2,4);
                    printk(" != pat2:0x");
                    printk(pat2,4);
                    printk(" at addr:0x");
                    printk(addr2,8);
                    printk("\n\n");
                    #endif

					err=-1;
				}
			    addr2++;

				break;
			case 1:
				pat1 = (unsigned char) rPat[i] & 0xff;
				if(*addr1 != pat1)
				{
                    #ifdef DEBUG
                    printk("\nerror: rByte:1\n");
                    printk("*addr:0x");
                    printk(*addr1,2);
                    printk(" != pat1:0x");
                    printk(pat1,2);
                    printk(" at addr:0x");
                    printk(addr1,8);
                    printk("\n\n");
                    #endif

					err=-1;
				}
			    addr1++;

				break;
			default:
                #ifdef DEBUG
				printk("dram_pat_cmp: ERROR! read byte=");
                printk(rByte);
                printk("\n");
                #endif
				err=-1;
		}

		i++;
		if(i >= pNum)
		{
			i = 0;
		}
		size--;
	}

    if (err==-1)
       printk("\ndram_pat_cmp: fail\n"); 

	return err;
}


static int dram_incrPat_cmp(void *startAddr, unsigned long size, unsigned long pattern, unsigned char wByte, unsigned char rByte)
{
	unsigned char *addr1;
	unsigned char pat1;
	unsigned short *addr2;
	unsigned short pat2;
	unsigned long *addr4;
	unsigned long pat4;
	int i, n;
	unsigned long rPat[4] = {0};
	unsigned char pNum;
	int patIdx = -1;
	unsigned long tmp;
	unsigned char shiftByte;
	int err=0;
    unsigned short tmp2b;
    unsigned long tmp4b;


	switch(wByte)
	{
		case 4:
			pat4 = (unsigned long)(pattern & 0xffffffff);
			break;
		case 2:
			pat2 = (unsigned short)(pattern & 0xffff);
			break;
		case 1:
			pat1 = (unsigned char)(pattern & 0xff);
			break;
		default:
            #ifdef DEBUG
			printk("dram_incrPat_cmp: ERROR! wByte=");
            printk(wByte);
            printk("\n");
            #endif
			err=-1;
	}
	switch(rByte)
	{
		case 4:
			addr4 = (unsigned long*)startAddr;
			size = size >> 2;
			break;
		case 2:
			addr2 = (unsigned short*)startAddr;
			size = size >> 1;
			break;
		case 1:
			addr1 = (unsigned char*)startAddr;
			break;
		default:
            #ifdef DEBUG
			printk("dram_incrPat_cmp: ERROR! read byte=");
            printk(rByte);
            printk("\n");
            #endif
			err=-1;
	}


	while(size&&(err!=-1))
	{
        #ifdef DEBUG
		printk("\npatIdx:");
        printk(patIdx);
        printk("\n");
        #endif
        
		if(wByte > rByte)
		{
			pNum = wByte / rByte;
			if((patIdx >= pNum-1) || (patIdx == -1))
			{
				switch(wByte)
				{
					case 4:
						tmp = SWAP_4B(pat4);
						pat4++;
						break;
					case 2:
						tmp = SWAP_2B(pat2);
                        #ifdef DEBUG
						printk("\npat2:0x");
                        printk(pat2,4);
                        printk(", tmp:0x");
                        printk(tmp,8);
                        printk("\n");
                        #endif
						pat2++;
						break;
					case 1:
						tmp = pat1;
						pat1++;
						break;
					default:
                        #ifdef DEBUG
						printk("dram_incrPat_cmp: ERROR! wByte=");
                        printk(wByte);
                        printk("\n");
                        #endif
						err=-1;
				}
				for(i=pNum-1; i>=0 ;i--)
				{
					switch(rByte)
					{
						case 2:
							rPat[i] = tmp & 0xffff;
							tmp = tmp >> 16;
							break;
						case 1:
							rPat[i] = tmp & 0xff;
							tmp = tmp >> 8;
                            #ifdef DEBUG
    						printk("\nrPat[i]:0x");
                            printk(rPat[i],8);
                            printk("\n");
                            #endif
							break;
						default:
                            #ifdef DEBUG
							printk("dram_incrPat_cmp: ERROR! read byte=");
                            printk(rByte);
                            printk("\n");
                            #endif
							err=-1;
					}
				}
				patIdx = 0;
			}
			else
			{
				patIdx++;
			}
		}
		else if(wByte < rByte)
		{
			/* take wByte=1 rByte=4 pat1=0x01 for example,
			 * it will get rPat[0]= 0 | ((0x01 <<8) | 0x02) in the first for while,
			 * and then get rPat[0]= (0x0102 << 16 )| ((0x03 <<8) | 0x04) in the second for while.
			 * In this case, n=2 , so the result is rPat[0]=0x01020304 */
			patIdx = 0;
			n = rByte / (wByte * 2);
			rPat[patIdx] = 0;
			shiftByte = 1;

			for(i=0; i<n; i++)
			{
				switch(wByte)
				{
					case 2:
						rPat[patIdx] = (rPat[patIdx] << 16) | ((SWAP_2B(pat2) << 16));
                        pat2++;
                        rPat[patIdx] |= (SWAP_2B(pat2));
						pat2++;
						break;
					case 1:
						rPat[patIdx] = (rPat[patIdx] << (8 * shiftByte)) | ((pat1 << 8) | (++pat1));
						pat1++;
						shiftByte = shiftByte << 1;
						break;
					default:
                        #ifdef DEBUG
						printk("dram_incrPat_cmp: ERROR! wByte=");
                        printk(wByte);
                        printk("\n");
                        #endif
						err=-1;
				}
			}
		}
		else
		{
			patIdx = 0;
			switch(wByte)
			{
				case 4:
					rPat[patIdx] = pat4;
					pat4++;
					break;
				case 2:
					rPat[patIdx] = pat2;
					pat2++;
					break;
				case 1:
					rPat[patIdx] = pat1;;
					pat1++;
					break;
				default:
                    #ifdef DEBUG
					printk("dram_incrPat_cmp: ERROR! wByte=");
                    printk(wByte);
                    printk("\n");
                    #endif
					err=-1;
			}
		}

		switch(rByte)
		{
			case 4:
                if (wByte==1 || wByte==2)
                    tmp4b = SWAP_4B(*addr4);
                else
                    tmp4b = *addr4;
				if(tmp4b != (unsigned long)(rPat[patIdx] & 0xffffffff))
				{
				    #ifdef DEBUG
                    printk("error! rByte:4!");
					printk("\n*addr:0x");
                    printk(tmp4b,8);
					printk(" != rPat:0x");
                    printk(rPat[patIdx],8);
					printk(" at addr:0x");
                    printk(addr4,8);
                    printk("\n\n");
                    #endif

					err=-1;
				}

			    addr4++;
                
				break;
			case 2:
                if (wByte==1 || wByte==4)
                    tmp2b = SWAP_2B(*addr2);
                else
                    tmp2b = *addr2;
				if(tmp2b != (unsigned short)(rPat[patIdx] & 0xffff))
				{
                    #ifdef DEBUG
                    printk("error! rByte:2!");
					printk("\n*addr:0x");
                    printk(tmp2b,4);
					printk(" != rPat:0x");
                    printk(rPat[patIdx],4);
					printk(" at addr:0x");
                    printk(addr2,8);
                    printk("\n\n");
                    #endif

					err=-1;
				}
                
			    addr2++;

				break;
			case 1:
				if(*addr1 != (unsigned char)(rPat[patIdx] & 0xff))
				{
                    #ifdef DEBUG
                    printk("error! rByte:1!");
					printk("\n*addr:0x");
                    printk(*addr1,2);
					printk(" != rPat:0x");
                    printk(rPat[patIdx],8);
					printk(" at addr:0x");
                    printk(addr1,8);
					printk(" patIdx:");
                    printk(patIdx);
                    printk("\n\n");
                    #endif

					err=-1;
				}
                
			    addr1++;

				break;
			default:
                #ifdef DEBUG
				printk("dram_incrPat_cmp: ERROR! read byte=");
                printk(rByte);
                printk("\n");
                #endif
				err=-1;
		}

		size--;
	}

    if (err==-1)
        printk("\ndram_incrPat_cmp: fail\n");

	return err;
}


static int dram_antiIncrPat_cmp(void *startAddr, unsigned long size, unsigned long pattern, unsigned char wByte)
{
	unsigned char *addr1;
	unsigned char pat1;
	unsigned char antiPat1;
	unsigned short *addr2;
	unsigned short pat2;
	unsigned char antiPat2;
	unsigned long *addr4;
	unsigned long pat4;
	unsigned char antiPat4;
	int err=0;


	#ifdef DEBUG
	printk("dram_antiIncrPat_cmp\n");
	printk(startAddr,8);
	printk("\n");
	printk(size,8);
	printk("\n");
	printk(pattern);
	printk("\n");
	printk(wByte);
	printk("\n");
	#endif
	switch(wByte)
	{
		case 4:
			pat4 = (unsigned long)(pattern & 0xffffffff);
			addr4 = (unsigned long*)startAddr; 
			size = size >> 2;
			break;
		case 2:
			pat2 = (unsigned short)(pattern & 0xffff);
			addr2 = (unsigned short*)startAddr;
			size = size >> 1;
			break;
		case 1:
			pat1 = (unsigned char)(pattern & 0xff);
			addr1 = (unsigned char*)startAddr;
			break;
		default:
			//printk("dram_antiIncrPat_cmp: ERROR! wByte=%d \n", wByte);
			err=-1;
	}


	while(size&&(err!=-1))
	{
		switch(wByte)
		{
			case 4:
				antiPat4 = ~pat4;
				if(*addr4 != antiPat4)
				{
				    #ifdef DEBUG
					printk("\nerror! wByte:4!\n");
                    printk("*addr:0x");
                    printk(*addr4,8);
					printk(" != antiPat4:0x");
                    printk(antiPat4,8);
					printk(" at addr:0x");
                    printk(addr4,8);
                    printk("\n\n");
                    #endif

					err=-1;
				}

			    addr4++;
                
				pat4++;
				break;
			case 2:
				antiPat2 = ~pat2;
				if(*addr2 != antiPat2)
				{
                    #ifdef DEBUG
					printk("\nerror! wByte:2!\n");
                    printk("*addr:0x");
                    printk(*addr2,4);
					printk(" != antiPat2:0x");
                    printk(antiPat2,4);
					printk(" at addr:0x");
                    printk(addr2,8);
                    printk("\n\n");
                    #endif

					err=-1;
				}

			    addr2++;

				pat2++;
				break;
			case 1:
				antiPat1 = ~pat1;
				if(*addr1 != antiPat1)
				{
                    #ifdef DEBUG
					printk("\nerror! wByte:1!\n");
                    printk("*addr:0x");
                    printk(*addr1,2);
					printk(" != antiPat1:0x");
                    printk(antiPat1,2);
					printk(" at addr:0x");
                    printk(addr1,8);
                    printk("\n\n");
                    #endif

					err=-1;
				}

			    addr1++;

				pat1++;
				break;
			default:
                #ifdef DEBUG
				printk("dram_antiIncrPat_cmp: ERROR! wByte=");
                printk(wByte);
                printk("\n");
                #endif
				err=-1;
		}

		size--;
	}

    if (err==-1)
        printk("\ndram_antiIncrPat_cmp: fail\n");

	return err;
}

int sramTest(void)
{
    int l, j, k, i;
    int ret = 0;
    unsigned int count=0;

	printk("\nGDMP sramTest start\n\n");

    /* set GDMP sram to be accessed via PBUS (by CPU or GDMA) */
    VPint(0xbfb00954) = 0;

  while(1) {
    printk("\nTest Round:%d\n", ++count);

    for(l=0; startAddr[l]!=0; l++)
    {
		for(j=0; defWByte[j]!=0; j++)
		{
		    #ifdef DEBUG
			printk("\nstartAddr:0x%08x, defWByte:0x%02x\n\n", startAddr[l], defWByte[j]);
            #endif

			/* use the default patterns, incremental pattern */
			dram_pat_set(startAddr[l], testSize, count, INCR_PAT, defWByte[j]);
            
			for(k=0; defRByte[k]!=0; k++)
			{
			    #ifdef DEBUG
                printk("defRByte:0x%02x\n\n", defRByte[k]);
                #endif
				ret += dram_incrPat_cmp(startAddr[l], testSize, count, defWByte[j], defRByte[k]);
                if (ret) 
                    goto sramTest_end;
			}
		}
        
		/* use the default patterns, "anti-incremental patten, 0x5a, 0xa5, 0x0, 0xff" */
	    #ifdef DEBUG
        printk("dram_antiIncrPat test\n\n");
        #endif
		dram_pat_set(startAddr[l], testSize, count, ANTI_INCR_PAT, 1);
		ret += dram_antiIncrPat_cmp(startAddr[l], testSize, count, 1);	
        if (ret) 
            goto sramTest_end;

	    /* test with different patterns */	
		for(i=0; i<sizeof(defPat); i++)
		{
		    #ifdef DEBUG
            printk("defPat:0x%02x\n\n", defPat[i]);
            #endif
			pat = defPat[i];
			dram_pat_set(startAddr[l], testSize, pat, ORI_PAT, 1);
			ret += dram_pat_cmp(startAddr[l], testSize, pat, 1, 1);
            if (ret) 
                goto sramTest_end;
		}
	}
    if ((count & 0x1f) == 0x1f)
        schedule();
  }

    printk("sramTest SUCCESS!\n\n");
    return 0;

sramTest_end:
    if(ret) {
        printk("sramTest FAILED!\n\n");
        return -1;
    }
}

