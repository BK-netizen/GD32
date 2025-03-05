/*!
 * 文件名称： bsp_spi_nand.c
 * 描    述： bsp nand驱动文件
 * 版本：     2023-12-03, V1.0
*/

/*
* GD32H757海棠派开发板V1.0
* 聚沃科技官网：https://www.gd32bbs.com/
* B站视频教程：https://space.bilibili.com/475462605
* CSDN文档教程：https://gd32mcu.blog.csdn.net/
* 聚沃开发板购买：https://juwo.taobao.com
* GD技术交流QQ群：859440462
* GD官方开发板、样品购买，咨询Q：87205168  咨询V：13905102619
* GD32 MCU方案移植、方案开发，咨询Q：87205168  咨询V：13905102619
* Copyright@苏州聚沃电子科技有限公司，盗版必究。
*/

#include "bsp_spi_nand.h"
#include "string.h"
#include <stdio.h>


GPIO_DEF(NAND_FLASH_CS,G,13,OUT_PP,SET,NULL);

#define SPI_NAND_UNLOCK()  board_spi_device.spi_device_state_bits.device1=SET
#define SPI_NAND_LOCK()    board_spi_device.spi_device_state_bits.device1=RESET

static uint8_t tem_buffer[SPI_NAND_PAGE_TOTAL_SIZE];   /* the buffer of read page data */

#define true    1
#define false   0

#define TYPE_DBT        0
#define TYPE_RBT        1
#define TYPE_DBT_RBT    1

#define DBT_SIZE    200
#define RBT_SIZE    BLOCK_NUM_FOR_USER
uint8_t dummy=0xA5;

uint16_t DBT[DBT_SIZE]; 
uint16_t RBT[RBT_SIZE]; 

uint16_t g16DBT_Block[2];
uint8_t  g8DBT_CRT_Block_Idx;   //0 or 1
uint8_t  g8DBT_CRT_Page;

uint16_t g16RBT_Block[2];
uint8_t  g8RBT_CRT_Block_Idx;
uint8_t  g8RBT_CRT_Page;

uint16_t g16LastValidBlock;
uint16_t g16BadBlockNum;


/*******************************************************************************
* Function Name  : bsp_spi_nand_init
* Description    : Init NAND Interface
* Input          : None
* Output         : None
* Return         : Status
*******************************************************************************/
void bsp_spi_nand_init(void)
{ 
    driver_gpio_general_init(&NAND_FLASH_CS); 
   
    /* reset NANDFLASH */
    bsp_spi_nandflash_reset();
    /* configure the PROTECTION register*/
    bsp_spi_nandflash_set_register(PROTECTION,0x00);
    /* configure the FEATURE1 register*/
    bsp_spi_nandflash_set_register(FEATURE1,0x11);//0x01→0x11
    /* build bock table: LUT = Look up table */
    //spi_nandflash_read_parameter();
    //erase for debug
    //while(1);    
}


void bsp_spi_nand_cs_low(void)
{
    bsp_spi_device_busy_wait(&board_spi_device); 
    SPI_NAND_UNLOCK();
    /* select the flash: chip select low */
    driver_gpio_pin_reset(&NAND_FLASH_CS);   
}  

void bsp_spi_nand_cs_high(void)
{
    /* select the flash: chip select high */
    driver_gpio_pin_set(&NAND_FLASH_CS);  
    SPI_NAND_LOCK();   
}  


/* detect the nandflash block is or not bad */
//static uint8_t spi_nandflash_badblock_detect(uint32_t blockNo);

//static void load_DBTRBT_from_nand(uint8_t type);
static uint8_t check_whether_in_DBT_array(uint16_t BlockNo);
static uint8_t update_DBTRBT_to_nand(uint8_t type);
static void add_bad_Block_to_DBTRBT_ram(uint16_t BlockNo);
static uint16_t re_mapping_RBT(uint16_t ori_BlockNo,uint16_t old_replace_BlockNo);
//static uint8_t move_page_data(uint16_t des_block_No,uint16_t src_block_No,uint8_t page_No);
static uint16_t get_replace_block_from_ram(uint16_t BlockNo);
static uint8_t update_DBTRBT_array(uint16_t BlockNo);
//static void alloc_DBTRBT_block_addr(void);
//static void init_build_DBTRBT(void);
//static uint8_t rebuild_DBTRBT_array(void);
//static void printfData(uint32_t a32SAddr, uint32_t byteCnt);

/************************ Temporarily not visible to the public **************************/

/* read the data from nandflash */
uint8_t spi_nandflash_read_data(uint8_t *buffer,uint32_t page_No,uint32_t address_in_page,uint32_t byte_cnt);
/* write the data to nandflash */
uint8_t spi_nandflash_write_data(uint8_t *buffer,uint32_t page_No,uint16_t address_page,uint32_t byte_cnt);
/* nandflash internal data move*/
uint8_t spi_nandflash_copy_page(uint32_t source_page,uint32_t target_page);
/* nandflash internal data move and update new data */
uint8_t spi_nandflash_copy_page_update(uint8_t *buffer,uint32_t source_page,uint32_t target_page,uint16_t address_in_page,uint32_t byte_cnt);
/* write the data to spare area */
uint8_t spi_nandflash_write_spare(uint8_t *buffer,uint32_t page_No,uint16_t spare_address,uint16_t byte_cnt);
/* read the data from spare area */
uint8_t spi_nandflash_read_spare(uint8_t *buffer,uint32_t page_No,uint16_t spare_address,uint16_t byte_cnt);

/*!
    \brief      read a byte from the SPI flash
    \param[in]  none
    \param[out] none
    \retval     byte read from the SPI flash
*/
uint8_t spi_nandflash_read_byte(void)
{
    return(driver_spi_master_transmit_receive_byte(&BOARD_SPI,DUMMY_BYTE));
}

/*!
    \brief      reset nandflash
    \param[in]  none
    \param[out] none
    \retval     flash identification
*/

void bsp_spi_nandflash_reset()
{
    /* select the flash: chip select low */
    bsp_spi_nand_cs_low();
    /* send " RESET" command */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,SPI_NAND_RESET);
    /* deselect the flash: chip select high */
    bsp_spi_nand_cs_high();
    
    __IO uint64_t timeout = driver_tick;  
    while(bsp_spi_nandflash_get_status_flag(OIP)==SPI_NAND_BUSY)
    {
        if((timeout+200U) <= driver_tick) {              
            return;
        }     
    }    
}

/*!
    \brief      set nandflash register
    \param[in]  reg: the address of target register
    \param[in]  data: write parameters to target register
    \param[out] none
    \retval     flash identification
*/
void bsp_spi_nandflash_set_register(uint8_t reg,uint8_t data)
{
    /* select the flash: chip select low */
    bsp_spi_nand_cs_low();
    /* send " RESET" command */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,SPI_NAND_SET_FEATURE);
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,reg);
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,data);
    /* deselect the flash: chip select high */
    bsp_spi_nand_cs_high();
    
    __IO uint64_t timeout = driver_tick;  
    while(bsp_spi_nandflash_get_status_flag(OIP)==SPI_NAND_BUSY )
    {
        if((timeout+200U) <= driver_tick) {              
            return;
        }     
    }
}

/*!
    \brief      enable the write access to the flash
    \param[in]  none
    \param[out] none
    \retval     none
*/
void bsp_spi_nandflash_write_enable(void)
{
    /* select the flash: chip select low */
    bsp_spi_nand_cs_low();

    /* send "write enable" instruction */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,SPI_NAND_WREN);

    /* deselect the flash: chip select high */
    bsp_spi_nand_cs_high();
}

/*!
    \brief      poll the status of the write in progress(wip) flag in the flash's status register
    \param[in]  status_reg:the address nandflash status register
    \param[in]  *status: get the byte of status rgister
    \param[out] none
    \retval     none
*/
void bsp_spi_nandflash_get_feature(uint8_t status_reg, uint8_t *status)
{
    /* select the flash: chip select low */
    bsp_spi_nand_cs_low();

    /* send "get feature" command */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,SPI_NAND_GET_FEATURE);
    /* send the address of status register */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,status_reg);
    /* get nandflash status */
    *status = driver_spi_master_transmit_receive_byte(&BOARD_SPI,DUMMY_BYTE);

    /* deselect the flash: chip select high */
    bsp_spi_nand_cs_high();
}
 
/*!
    \brief      get nandflash status register flag bit
    \param[in]  status_flag: the flag of nandflash status register 
    \param[out] none
    \retval     SPI_NAND_BUSY: nandflash is busy
    \retval     SPI_NAND_READY: nandflash is ready
*/
uint8_t bsp_spi_nandflash_get_status_flag(uint8_t status_flag)
{
   uint8_t status;
    /* read nandflash status register*/
    bsp_spi_nandflash_get_feature( STATUS, &status );
    //printf("status %x\n\r",status);
    if( (status & status_flag) == status_flag ){
        return SPI_NAND_BUSY;
    }
    else{
        if(status == E_FAIL)
        {
            return SPI_NAND_BAD;
        }
        if(status == P_FAIL)
        {
            return SPI_NAND_PFAIL;
        }
   
        return SPI_NAND_READY;
    }
}

/*!
    \brief      send the read page command
    \param[in]  page_No: the serial number of nandflash page
    \param[out] none
    \retval     none
*/
void bsp_spi_nandflash_page_read(uint32_t page_No)
{
    /* select the flash: chip select low */
    bsp_spi_nand_cs_low();
    /* send "PAGE READ" command */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,SPI_NAND_PAGE_READ);
    /* send the serial number of page */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,(page_No>>16)&0xFF);
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,(page_No>>8)&0xFF);
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,page_No&0xFF);
    /* deselect the flash: chip select high */
    bsp_spi_nand_cs_high();
}

void bsp_spi_nandflash_parameterpage_read(uint32_t page_No)
{
    /* select the flash: chip select low */
    bsp_spi_nand_cs_low();
    /* send "PAGE READ" command */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,SPI_NAND_PARAMETERPAGE_READ);
    /* send the serial number of page */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,(page_No>>16)&0xFF);
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,(page_No>>8)&0xFF);
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,page_No&0xFF);
    /* deselect the flash: chip select high */
    bsp_spi_nand_cs_high();
}


/*!
    \brief      send the read cache command
    \param[in]  buffer: a pointer to the array
    \param[in]  address_in_page: the address in nandflash page
    \param[in]  byte_cnt: the number of data
    \param[out] none
    \retval     none
*/
void bsp_spi_nandflash_read_cache(uint8_t *buffer,uint16_t address_in_page,uint32_t byte_cnt)
{
    uint32_t i=0;

    /* select the flash: chip select low */
    bsp_spi_nand_cs_low();
#ifdef SPI_NANDFLASH
    /* send "PAGE READ" command */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,SPI_NAND_READ_CACHE);
    //driver_spi_master_transmit_receive_byte(&BOARD_SPI,DUMMY_BYTE);//Q4UC ++ Q5 --
    /* send the address of page */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,(address_in_page>>8)&0xFF);
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,address_in_page&0xFF);
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,DUMMY_BYTE);//Q4UC -- Q5 ++
    
#endif


    
    for(i=0;i<byte_cnt;i++){
        *buffer++=driver_spi_master_transmit_receive_byte(&BOARD_SPI,DUMMY_BYTE);
    }
    
    /* deselect the flash: chip select high */
    bsp_spi_nand_cs_high();
//    qspi_disable(BOARD_SPI.spi_x);
}

/*!
    \brief      send the program load command,write data to cache
    \param[in]  buffer: the data of array
    \param[in]  address_in_page: the address in nandflash page
    \param[in]  byte_cnt: the number of data
    \param[out] none
    \retval     none
*/
void bsp_spi_nandflash_program_load(uint8_t *buffer,uint16_t address_in_page,uint32_t byte_cnt)
{
    uint32_t i=0;

    /* select the flash: chip select low */
    bsp_spi_nand_cs_low();
#ifdef SPI_NANDFLASH
    /* send "PAGE READ" command */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,SPI_NAND_PAGE_LOAD);
    /* send the serial number of page */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,(address_in_page>>8)&0xFF);
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,address_in_page&0xFF);
#endif 
    

    /* deselect the flash: chip select high */

        
    for(i=0;i<byte_cnt;i++){
        driver_spi_master_transmit_receive_byte(&BOARD_SPI,*buffer++);
    }
    //printf("cache program %x %x\n\r",m32record[0],m32record[1]);
        
    bsp_spi_nand_cs_high();
//    qspi_disable(BOARD_SPI.spi_x);
}

/*!
    \brief      send the program excute command
    \param[in]  page_No: the serial number of nandflash page
    \param[out] none
    \retval     none
*/
void bsp_spi_nandflash_program_execute(uint32_t page_No)
{
    /* enable the write access to the flash */
    bsp_spi_nandflash_write_enable();
    /* select the flash: chip select low */
    bsp_spi_nand_cs_low();
    /* send "PAGE READ" command */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,SPI_NAND_PROGRAM_EXEC);
    /* send the serial number of page */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,(page_No>>16)&0xFF);
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,(page_No>>8)&0xFF);
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,page_No&0xFF);
    /* deselect the flash: chip select high */
    bsp_spi_nand_cs_high();
}

/*!
    \brief      send program load random data command
    \param[in]  buffer: the data of array
    \param[in]  address_in_page: the address in nandflash page
    \param[in]  byte_cnt: the number of data
    \param[out] none
    \retval     none
*/
void bsp_spi_nandflash_pl_random_data(uint8_t *buffer,uint16_t address_in_page,uint32_t byte_cnt)
{
    uint32_t i=0;
    /* select the flash: chip select low */
    bsp_spi_nand_cs_low();
#ifdef SPI_NANDFLASH
    /* send "Program Load Random Data" command */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,SPI_NAND_PAGE_RAND_LOAD);
    /* send the address in page */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,(address_in_page>>8)&0xFF);
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,address_in_page&0xFF);
#endif
#ifdef QSPI_NANDFLASH
    /* send "Program Load Random Data" command */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,SPI_NAND_PAGE_RAND_LOAD4);
    /* send the address in page */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,(address_in_page>>8)&0xFF);
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,address_in_page&0xFF);
    qspi_enable(SPIX);
    qspi_write_enable(SPIX);
#endif
    for(i=0;i<byte_cnt;i++){
        driver_spi_master_transmit_receive_byte(&BOARD_SPI,*buffer++);
    }
    /* deselect the flash: chip select high */
    bsp_spi_nand_cs_high();
#ifdef QSPI_NANDFLASH    
    qspi_disable(BOARD_SPI.spi_x);
#endif    
}

///*!
//    \brief      detect the nandflash block is or not bad 
//    \param[in]  blockNo: the serial number of nandlash block
//    \param[out] none
//    \retval     none
//*/
//static uint8_t spi_nandflash_badblock_detect(uint32_t blockNo)
//{
//    uint8_t ucFlag,BakFeature,result;
//    result = 0;

//    bsp_spi_nandflash_get_feature( FEATURE1, &BakFeature );
//#ifdef DEBUG
//    //printf("\BakFeature:%x\n",BakFeature);
//#endif
//    bsp_spi_nandflash_set_register(FEATURE1,BakFeature&0xEF);       //close ECC
//    /* read the bad flag in spare area */
//    spi_nandflash_read_spare(&ucFlag, blockNo * SPI_NAND_BLOCK_SIZE, BI_OFFSET, 1);
//    if (ucFlag != 0xFF){
//#ifdef DEBUG
//        //printf("\rIsBadBlock:%x\n",blockNo);
//#endif
//        result = 1;
//    }
//    /* read the bad flag in spare area(reserve area) */
//    spi_nandflash_read_spare(&ucFlag, blockNo * SPI_NAND_BLOCK_SIZE + 1, BI_OFFSET, 1);
//    if (ucFlag != 0xFF){
//#ifdef DEBUG
//        //printf("\rIsBadBlock:%x\n",blockNo);
//#endif
//        result = 1;
//    }
//    bsp_spi_nandflash_set_register(FEATURE1,BakFeature);

//    return result;
//}





/*!
    \brief      read flash identification
    \param[in]  none
    \param[out] none
    \retval     flash identification
*/
uint32_t bsp_spi_nandflash_read_id(void)
{
    uint32_t temp = 0, temp0 = 0, temp1 = 0, temp2 = 0;

    /* select the flash: chip select low */
    bsp_spi_nand_cs_low();

    /* send "READ_ID " instruction */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,SPI_NAND_READID);

    /* read a byte from the flash */
    temp0 = driver_spi_master_transmit_receive_byte(&BOARD_SPI,DUMMY_BYTE);

    /* read a byte from the flash */
    temp1 = driver_spi_master_transmit_receive_byte(&BOARD_SPI,DUMMY_BYTE);

    /* read a byte from the flash */
    temp2 = driver_spi_master_transmit_receive_byte(&BOARD_SPI,DUMMY_BYTE);

    /* deselect the flash: chip select high */
    bsp_spi_nand_cs_high();

    temp = (temp0 << 16) | (temp1 << 8) | temp2;
    temp = temp & 0xffff;
    //printf("flash id %x\n\r",temp);
    return temp;
}
/*!
    \brief      read flash parameterid
    \param[in]  buffer address_page byte_cnt
    \param[out] none
    \retval     flash identification
*/

void bsp_spi_nandflash_read_cache_parameter(uint8_t *buffer,uint16_t address_page,uint32_t byte_cnt)
{
    uint32_t i=0;
    bsp_spi_nand_cs_low();

    driver_spi_master_transmit_receive_byte(&BOARD_SPI,SPI_NAND_READ_CACHE);
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,(address_page>>8)&0xFF);
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,address_page&0xFF);
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,DUMMY_BYTE);
    

    for(i=0;i<byte_cnt;i++){
        *buffer++=driver_spi_master_transmit_receive_byte(&BOARD_SPI,DUMMY_BYTE);
        
    }
    bsp_spi_nand_cs_high();
//    qspi_disable(BOARD_SPI.spi_x);
}
/*!
    \brief      get ecc level
    \param[in]  none
    \param[out] none
    \retval     err bit
*/

uint8_t bsp_spi_nandflash_get_ecc_level(void)
{
    uint8_t status,m8ecclevel0,m8ecclevel1,m8errbit;
        /* read nandflash status register*/
    bsp_spi_nandflash_get_feature( STATUS, &status );
    m8ecclevel0 = (status>>4)&0x03;
    bsp_spi_nandflash_get_feature( STATUS2, &status );
    m8ecclevel1 = (status>>4)&0x03;
    m8errbit = 0;
    if(m8ecclevel0 != 0)
    {
        printf("ecc level %x\n\r",m8ecclevel0);
    }
    if(m8ecclevel0 < 1)
    {
        m8errbit = 0;
    }
    else if((m8ecclevel0 == 1)&&(m8ecclevel1 < 3))
    {
        m8errbit = 3;
    }
    else if((m8ecclevel0 == 1)&&(m8ecclevel1 == 3))
    {
        m8errbit = 4;
    }
    else if(m8ecclevel0 == 2)
    {
        m8errbit = 5;
    }
    return m8errbit;

}


/*!
    \brief      erase the nandflash blcok
    \param[in]  block_No:the serial number of erase block
    \param[out] none
    \retval     SPI_NAND_FAIL: erase the nandflash block fail
    \retval     SPI_NAND_SUCCESS: erase the nandflash block success
*/
uint8_t bsp_spi_nandflash_block_erase(uint32_t block_No)
{
    uint8_t result = SPI_NAND_SUCCESS;

    block_No<<=6;        //block_No=block_No*64
    bsp_spi_nandflash_write_enable();
    /* select the flash: chip select low */
    bsp_spi_nand_cs_low();
    /* send "ERASE BLOCK" command */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,SPI_NAND_BLOCK_ERASE);
    /* send the address of memory */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,(block_No>>16)&0xFF);
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,(block_No>>8)&0xFF);
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,block_No&0xFF);
    /* deselect the flash: chip select high */
    bsp_spi_nand_cs_high();
    while(bsp_spi_nandflash_get_status_flag(OIP)==SPI_NAND_BUSY);
    /* check program result */        

    return result;
}

/*!
    \brief      read the data from nandflash
    \param[in]  *buffer:the data of array
    \param[in]  page_No: the serial number of nandflash page
    \param[in]  address_in_page: the address in nandflash page
    \param[in]  byte_cnt:the number of data
    \param[out] none
    \retval     SPI_NAND_FAIL,SPI_NAND_SUCCESS 
*/
uint8_t spi_nandflash_read_data(uint8_t *buffer,uint32_t page_No,uint32_t address_in_page,uint32_t byte_cnt)
{
    uint8_t result = SPI_NAND_SUCCESS;
    uint8_t status = 0;
    uint8_t retrycnt = 0;

    /* the capacity of page must be equal or greater than the taotal of address_in_page and byte_cnt */
    if((address_in_page+byte_cnt)>SPI_NAND_PAGE_TOTAL_SIZE){
        return SPI_NAND_FAIL;
    }
ReadRetry:
    /* send the read page command */
    bsp_spi_nandflash_page_read(page_No);
    /* wait for NANDFLASH is ready */
    while(bsp_spi_nandflash_get_status_flag(OIP)==SPI_NAND_BUSY);
    /* read data from cache */
    bsp_spi_nandflash_read_cache(buffer, address_in_page, byte_cnt);
    
    bsp_spi_nandflash_get_feature( STATUS, &status );
    if(( (status & ECCS0) == 0 )&&( (status & ECCS1) == ECCS1 )){    //UECC
        if(retrycnt < 3)
        {
            retrycnt++;
            #ifdef DEBUG
            printf("\rReadretry:%x %x\n",retrycnt,page_No); 
            #endif
            goto ReadRetry;
        }
        else
        {
            #ifdef DEBUG
            printf("\rRead Fail %x\n",page_No);
            #endif
        }      
    }             
    return result;
}

/*!
    \brief      write the data to nandflash
    \param[in]  *buffer:the data of array
    \param[in]  page_No: the serial number of nandflash page
    \param[in]  address_in_page: the address of nandflash page
    \param[in]  byte_cnt:the number of data
    \param[out] none
    \retval     SPI_NAND_FAIL,SPI_NAND_SUCCESS 
*/
uint8_t spi_nandflash_write_data(uint8_t *buffer,uint32_t page_No,uint16_t address_page,uint32_t byte_cnt)
{


    /*sned the program load command,write data to cache*/
    bsp_spi_nandflash_program_load(buffer, address_page, byte_cnt);
    /*sned the program excute command*/
    bsp_spi_nandflash_program_execute(page_No);
    /* Check program result */
    while(bsp_spi_nandflash_get_status_flag(OIP)==SPI_NAND_BUSY);

    
#ifdef WRITE_PAGE_VERIFY_EN
     spi_nandflash_read_data (tem_buffer,page_No, address_page, byte_cnt);
    if (memcmp(tem_buffer, buffer,  byte_cnt) != 0){
        return SUCCESS;
    }
#endif
    return 1;

}

/*!
    \brief      nandflash internal data move 
    \param[in]  source_page: the source page address of data
    \param[in]  target_page: the target page address of data
    \param[out] none
    \retval     SPI_NAND_SUCCESS
*/
uint8_t spi_nandflash_copy_page(uint32_t source_page,uint32_t target_page)
{
    /* read the source page data to cache*/
    bsp_spi_nandflash_page_read(source_page);
    /* wait for nandflash is ready */
    while(bsp_spi_nandflash_get_status_flag(OIP)==SPI_NAND_BUSY);
    /*write the cache data to target page*/
    bsp_spi_nandflash_program_execute(target_page);
    /* wait for nandflash is ready */
    while(bsp_spi_nandflash_get_status_flag(OIP)==SPI_NAND_BUSY);
    return SPI_NAND_SUCCESS;
}

/*!
    \brief      update new data and nandflash internal data move
    \param[in]  buffer:update new data
    \param[in]  source_page: the source page address of data
    \param[in]  target_page: the target page address of data
    \param[in]  address_in_page: the address in page
    \param[in]  byte_cnt: the number of updated data
    \param[out] none
    \retval     SPI_NAND_SUCCESS
*/
uint8_t spi_nandflash_copy_page_update(uint8_t *buffer,uint32_t source_page,uint32_t target_page,uint16_t address_in_page,uint32_t byte_cnt)
{
    /* read the source page data to cache*/
    bsp_spi_nandflash_page_read(source_page);
    /* wait for nandflash is ready */
    while(bsp_spi_nandflash_get_status_flag(OIP)==SPI_NAND_BUSY);
    /* update new data to cache */
    bsp_spi_nandflash_pl_random_data(buffer, address_in_page, byte_cnt);
    /*write the cache data to target page*/
    bsp_spi_nandflash_program_execute(target_page);
    /* wait for nandflash is ready */
    while(bsp_spi_nandflash_get_status_flag(OIP)==SPI_NAND_BUSY);
    return SPI_NAND_SUCCESS;
}

/*!
    \brief      write the data to spare area
    \param[in]  *buffer: the data of array
    \param[in]  page_No: the serial number of nandflash page(0~2048)
    \param[in]  spare_address: the address in nandflash page spare area
    \param[in]  byte_cnt: the number of data
    \param[out] none
    \retval     SPI_NAND_FAIL,SPI_NAND_SUCCESS 
*/
uint8_t spi_nandflash_write_spare(uint8_t *buffer,uint32_t page_No,uint16_t spare_address,uint16_t byte_cnt)
{
#ifdef DEBUG
    printf("\r%s\n",__func__);
#endif
    if (byte_cnt > SPI_NAND_SPARE_AREA_SIZE){
        return SPI_NAND_FAIL;
    }
    return spi_nandflash_write_data(buffer, page_No, SPI_NAND_PAGE_SIZE + spare_address, byte_cnt);
}

/*!
    \brief      read the data from spare area
    \param[in]  *buffer: the data of array
    \param[in]  page_No: the serial number of nandflash page
    \param[in]  spare_address: the address in nandflash page spare area
    \param[in]  byte_cnt: the number of data
    \param[out] none
    \retval     SPI_NAND_FAIL,SPI_NAND_SUCCESS 
*/
uint8_t spi_nandflash_read_spare(uint8_t *buffer,uint32_t page_No,uint16_t spare_address,uint16_t byte_cnt)
{
    if (byte_cnt > SPI_NAND_SPARE_AREA_SIZE){
        return SPI_NAND_FAIL;
    }
    return spi_nandflash_read_data(buffer, page_No, SPI_NAND_PAGE_SIZE + spare_address, byte_cnt);
}

/*!
    \brief      write the page data to nandflash
    \param[in]  buffer: the data of array
    \param[in]  block_No: the address in nandflash block(0~799)
    \param[in]  page_No: the address in nandflash page(0~63) in block_No
    \param[in]  buf_len: the length of array(1~2048)
    \param[out] none
    \retval     SPI_NAND_FAIL,SPI_NAND_SUCCESS 
*/
uint8_t bsp_nandflash_page_program( uint8_t *bufPtr, uint32_t blockNum, uint32_t pageNum, uint8_t bSpareProgram )

{
    return spi_nandflash_write_data(bufPtr,blockNum*SPI_NAND_BLOCK_SIZE+pageNum,0,SPI_NAND_PAGE_SIZE+SPI_NAND_SPARE_AREA_SIZE);
}

/*!
    \brief      read the data from nandflash
    \param[in]  *buffer:the data of array
    \param[in]  block_No: the address in nandflash block(0~799)
    \param[in]  page_No: the serial number of nandflash page(0~63) in block_No
    \param[in]  buf_len: the length of array(1~2048)
    \param[out] none
    \retval     SPI_NAND_FAIL,SPI_NAND_SUCCESS 
*/
uint8_t bsp_nandflash_page_read( uint8_t *bufPtr, uint32_t blockNum, uint32_t pageNum )
{
    if(spi_nandflash_read_data(bufPtr,blockNum*SPI_NAND_BLOCK_SIZE+pageNum,0,SPI_NAND_PAGE_SIZE+SPI_NAND_SPARE_AREA_SIZE) == SUCCESS){
        return 1;
    }
    else{
        return 0;
    }
}
/*!
    \brief      read the data from nandflash
    \param[in]  *buffer:the data of array
    \param[in]  block_No: the address in nandflash block(0~799)
    \param[in]  page_No: the serial number of nandflash page(0~63) in block_No
    \param[in]  buf_len: the length of array(1~2048)
    \param[out] none
    \retval     SPI_NAND_FAIL,SPI_NAND_SUCCESS 
*/

uint8_t bsp_nandflash_page_read_Data( uint8_t *bufPtr, uint32_t blockNum, uint32_t pageNum,uint32_t address_page,uint32_t byte_cnt )
{
    if(spi_nandflash_read_data(bufPtr,blockNum*SPI_NAND_BLOCK_SIZE+pageNum,address_page,byte_cnt) == SUCCESS){
        return 1;
    }
    else{
        return 0;
    }
}

/*!
    \brief      erase the block data of SPI nandflash
    \param[in]  block_No: the address in nandflash block(0~799)
    \param[out] none
    \retval     SPI_NAND_FAIL,SPI_NAND_SUCCESS 
*/
uint8_t bsp_nandflash_block_erase(uint16_t block_No)
{
    uint8_t result;
    uint16_t replace_block;
Start_Erase:
#ifdef DEBUG
    //printf("\rnandflash_page_erase:%x\n",block_No);
#endif
    if(block_No > USER_AREA_END){
#ifdef DEBUG
        printf("\rBlock error:%x\n",block_No);
#endif
        return SPI_NAND_FAIL;
    }    
    
    if(true==check_whether_in_DBT_array(block_No)){
        replace_block = get_replace_block_from_ram(block_No);
    }else{
        replace_block = block_No;
    }
    result = bsp_spi_nandflash_block_erase(replace_block);
    if(result == SPI_NAND_FAIL){     
        update_DBTRBT_array(replace_block);
        re_mapping_RBT(block_No,replace_block);
        update_DBTRBT_to_nand(TYPE_DBT);
        update_DBTRBT_to_nand(TYPE_RBT); 
        goto Start_Erase;
    }
#ifdef DEBUG
    printf("erase result:%x\n",result);
#endif
    return result;
}

///*!
//    \brief      load DBT and RBT block from NAND flash
//    \param[in]  type:need load table type
//    \param[out] none
//    \retval     none
//*/
//static void load_DBTRBT_from_nand(uint8_t type)
//{
//    //printf("\r%s:%x\n",__func__,type);
//    /* load DBT table */
//    if(type == TYPE_DBT){
//        spi_nandflash_read_data(tem_buffer,g16DBT_Block[g8DBT_CRT_Block_Idx]*SPI_NAND_BLOCK_SIZE+g8DBT_CRT_Page,0,SPI_NAND_PAGE_SIZE);
//        memcpy(DBT,tem_buffer,DBT_SIZE*sizeof(DBT[0]));
//    }

//    /* load RBT table */
//    if(type == TYPE_RBT){
//        spi_nandflash_read_data(tem_buffer,g16RBT_Block[g8RBT_CRT_Block_Idx]*SPI_NAND_BLOCK_SIZE+g8RBT_CRT_Page,0,SPI_NAND_PAGE_SIZE);
//        memcpy(RBT,tem_buffer,RBT_SIZE*sizeof(RBT[0]));
//    }
//}


/*!
    \brief      check the block if in DBT table
    \param[in]  BlockNo:block number 
    \param[out] none
    \retval     m8Result: if block in DBT
*/
static uint8_t check_whether_in_DBT_array(uint16_t BlockNo)
{
    uint8_t                 m8Result = false;
    uint16_t                m16OldDB;
    uint16_t                m16ODBNum = DBT[0];
    uint16_t                m16Idx;
#ifdef DEBUG
    //printf("\r%s:%x\n",__func__,BlockNo);
#endif
    for ( m16Idx = 0; m16Idx < m16ODBNum; m16Idx++ ) {
        m16OldDB = DBT[2 + m16Idx];
        if ( m16OldDB == BlockNo ) {
            m8Result = true;
            break;
        }
    }   
    return m8Result;
}


/*!
    \brief      update DBT and RBT block to NAND flash
    \param[in]  type:need load table type
    \param[out] none
    \retval     m8Result: update action if success or not
*/
static uint8_t update_DBTRBT_to_nand(uint8_t type)
{
    uint16_t  BlockNo;
    uint16_t  PageNo;           
    uint8_t   DBT_Erase_Flag,RBT_Erase_Flag;
    uint8_t   result;
#ifdef DEBUG
    printf("\r%s:%x\n",__func__,type);
#endif
    /* update DBT table */
    if(type==TYPE_DBT)
    {       
        if((g8DBT_CRT_Page >= (SPI_NAND_BLOCK_SIZE-1))&&(g8DBT_CRT_Page!=0xFF)){
            g8DBT_CRT_Block_Idx = 1 - g8DBT_CRT_Block_Idx;  //toggle block idx
            g8DBT_CRT_Page = 0;
            DBT_Erase_Flag = true;
        }
        else{
            if(g8DBT_CRT_Page==0xFF){
                g8DBT_CRT_Page=0;
            }
            else{
                g8DBT_CRT_Page++;
            }
            DBT_Erase_Flag = false;
        }   
        BlockNo = g16DBT_Block[g8DBT_CRT_Block_Idx];
        PageNo = g8DBT_CRT_Page;
        memset(tem_buffer,0xFF,(SPI_NAND_PAGE_SIZE+SPI_NAND_SPARE_AREA_SIZE));
        memcpy(tem_buffer,DBT,DBT_SIZE*sizeof(DBT[0]));
#ifdef DEBUG
        //printf("\rSPI_NAND_PAGE_SIZE=%x\n",SPI_NAND_PAGE_SIZE);
#endif
        tem_buffer[SPI_NAND_PAGE_SIZE+4+0]= 'D';    //0x44
        tem_buffer[SPI_NAND_PAGE_SIZE+4+1]= 'B';    //0x42
        tem_buffer[SPI_NAND_PAGE_SIZE+4+2]= 'T';    //0x54
        tem_buffer[SPI_NAND_PAGE_SIZE+4+3]= '!';    //0x21
        tem_buffer[SPI_NAND_PAGE_SIZE+4+4]= (g16DBT_Block[1-g8DBT_CRT_Block_Idx]&0xFF);         //record the pair block No
        tem_buffer[SPI_NAND_PAGE_SIZE+4+5]= (g16DBT_Block[1-g8DBT_CRT_Block_Idx]&0xFF00)>>8;        
        result = spi_nandflash_write_data(tem_buffer,BlockNo*SPI_NAND_BLOCK_SIZE+PageNo,0,(SPI_NAND_PAGE_SIZE+SPI_NAND_SPARE_AREA_SIZE));
        if(result==SPI_NAND_FAIL){
            return result;
        }
        if(DBT_Erase_Flag == true){
            bsp_spi_nandflash_block_erase(g16DBT_Block[1-g8DBT_CRT_Block_Idx]);
        }
    }

    /* update RBT table */
    if(type==TYPE_RBT)
    {
        if((g8RBT_CRT_Page >= (SPI_NAND_BLOCK_SIZE-1))&&(g8RBT_CRT_Page!=0xFF)){
            g8RBT_CRT_Block_Idx = 1 - g8RBT_CRT_Block_Idx;  //toggle block idx
            g8RBT_CRT_Page = 0;
            RBT_Erase_Flag = true;
        }
        else{
            if(g8RBT_CRT_Page==0xFF){
                g8RBT_CRT_Page=0;
            }
            else{
                g8RBT_CRT_Page++;
            }
            RBT_Erase_Flag = false;
        }
        BlockNo = g16RBT_Block[g8RBT_CRT_Block_Idx];
        PageNo = g8RBT_CRT_Page;    
        memset(tem_buffer,0xFF,(SPI_NAND_PAGE_SIZE+SPI_NAND_SPARE_AREA_SIZE));
        memcpy(tem_buffer,RBT,RBT_SIZE*sizeof(RBT[0]));
        tem_buffer[SPI_NAND_PAGE_SIZE+4+0]= 'R';    //0x52
        tem_buffer[SPI_NAND_PAGE_SIZE+4+1]= 'B';    //0x42
        tem_buffer[SPI_NAND_PAGE_SIZE+4+2]= 'T';    //0x54
        tem_buffer[SPI_NAND_PAGE_SIZE+4+3]= '!';    //0x21
        tem_buffer[SPI_NAND_PAGE_SIZE+4+4]= (g16RBT_Block[1-g8RBT_CRT_Block_Idx]&0xFF);         //record the pair block No
        tem_buffer[SPI_NAND_PAGE_SIZE+4+5]= (g16RBT_Block[1-g8RBT_CRT_Block_Idx]&0xFF00)>>8;
        result = spi_nandflash_write_data(tem_buffer,BlockNo*SPI_NAND_BLOCK_SIZE+PageNo,0,(SPI_NAND_PAGE_SIZE+SPI_NAND_SPARE_AREA_SIZE));
        if(result==SPI_NAND_FAIL){
            return result;
        }       
        if(RBT_Erase_Flag == true){
            bsp_spi_nandflash_block_erase( g16RBT_Block[1-g8RBT_CRT_Block_Idx]);
        }
    }
    return result;
}

/*!
    \brief      add bad block to ram screen
    \param[in]  BlockNo:block number
    \param[out] none
    \retval     none
*/
static void add_bad_Block_to_DBTRBT_ram(uint16_t BlockNo)
{
#ifdef DEBUG
    //printf("\r%s:%x\n",__func__,BlockNo);
#endif
//    if((USER_AREA_START<=BlockNo)&&(BlockNo<=USER_AREA_END)){
    if(BlockNo<=USER_AREA_END){
        RBT[BlockNo] = g16LastValidBlock;
        g16LastValidBlock--;
    }
    if(g16LastValidBlock <= ReplaceBlock_AREA_START){
        while(1);
    }
    g16BadBlockNum++; 
    DBT[0] = g16BadBlockNum;        //bad block number
    DBT[1] = g16LastValidBlock;     //record last valid block
    DBT[1+g16BadBlockNum] = BlockNo;
}

/*!
    \brief      re mapping RBT Table when the replace block is bad when running.
    \param[in]  ori_BlockNo:block number before mapping;old replace BlockNo:block number before mapping
    \param[out] none
    \retval     none
*/
static uint16_t re_mapping_RBT(uint16_t ori_BlockNo,uint16_t old_replace_BlockNo)
{
#ifdef DEBUG
    //printf("\r%s\n",__func__);
#endif
    if(0==((ori_BlockNo<=USER_AREA_END)&&(ReplaceBlock_AREA_START<=old_replace_BlockNo)&&(old_replace_BlockNo<=ReplaceBlock_AREA_END))){
        return false;
    }
    if(RBT[ori_BlockNo] != old_replace_BlockNo){
        return false;
    }
	if(ori_BlockNo == old_replace_BlockNo){
        return true;
    }
    RBT[ori_BlockNo] = g16LastValidBlock;
    g16LastValidBlock--;

    if(g16LastValidBlock <= ReplaceBlock_AREA_START){
        /* infinity loop */
        while(1);
    }
    
    DBT[1] = g16LastValidBlock; 
    return true;
}

///*!
//    \brief      move page data(include spare area) from source block to destination block.
//    \param[in]  des_block_No:block number before mapping;old replace BlockNo:block number before mapping
//    \param[out] none
//    \retval     none
//*/
//static uint8_t move_page_data(uint16_t des_block_No,uint16_t src_block_No,uint8_t page_No)
//{
//    uint8_t result;
//    result = spi_nandflash_read_data(tem_buffer,src_block_No*SPI_NAND_BLOCK_SIZE+page_No,0,(SPI_NAND_PAGE_SIZE+SPI_NAND_SPARE_AREA_SIZE));
//    if(SPI_NAND_FAIL == result){
//        return result;
//    }
//    result = spi_nandflash_write_data(tem_buffer,des_block_No*SPI_NAND_BLOCK_SIZE+page_No,0,(SPI_NAND_PAGE_SIZE+SPI_NAND_SPARE_AREA_SIZE));
//    return result;
//}

/*!
    \brief      get replace block from the tabe on the ram
    \param[in]  BlockNo:block number
    \param[out] none
    \retval     none
*/
static uint16_t get_replace_block_from_ram(uint16_t BlockNo)
{
    return RBT[BlockNo];
}

/*!
    \brief      add new block number to DBT array
    \param[in]  BlockNo:block number
    \param[out] none
    \retval     result:update success or not
*/
static uint8_t update_DBTRBT_array(uint16_t BlockNo)
{
    uint8_t   result = true;
#ifdef DEBUG
    //printf("\r%s:%x\n",__func__,BlockNo);
#endif
    result = check_whether_in_DBT_array(BlockNo);
    if ( result==false ) {
        add_bad_Block_to_DBTRBT_ram(BlockNo);
    }
    return result;
}


///*!
//    \brief      allocate the blocks to save bad block mapping table
//    \param[in]  none
//    \param[out] none
//    \retval     none
//*/
//static void alloc_DBTRBT_block_addr(void)
//{
//    uint16_t BlockNo = Table_AREA_END;
//    uint8_t  Idx;
//    g16BadBlockNum = 0;
//    memset(DBT,DBT_SIZE,0);
//    memset(RBT,RBT_SIZE,0);
//#ifdef DEBUG
//    printf("\r%s\n",__func__);
//#endif
//    for(Idx = 0;Idx < DBT_BLOCK_NUM;Idx++){     //alloc DBT address 
//        for(; BlockNo > Table_AREA_START; BlockNo--){   //950~1023
//            if(spi_nandflash_badblock_detect(BlockNo)==0){               
//                g16DBT_Block[Idx]=BlockNo;
//#ifdef DEBUG
//                printf("\rDBT_Block[%x]=%x\n",Idx,g16DBT_Block[Idx]);
//#endif
//                BlockNo--;
//                break;
//            }
//            else{               //bad block
//                g16BadBlockNum ++;          
//                DBT[1+g16BadBlockNum] = BlockNo;
//            }
//        }
//    }
//    g8DBT_CRT_Block_Idx = 0;
//    g8DBT_CRT_Page = 0xFF;

//    for(Idx = 0;Idx < RBT_BLOCK_NUM;Idx++){     //alloc RBT address
//        for(; BlockNo > Table_AREA_START; BlockNo--){
//            if(spi_nandflash_badblock_detect(BlockNo)==0){
//                g16RBT_Block[Idx]=BlockNo;
//#ifdef DEBUG
//                printf("\rRBT_Block[%x]=%x\n",Idx,g16RBT_Block[Idx]);
//#endif
//                BlockNo--;
//                break;
//            }
//            else{
//                g16BadBlockNum ++;
//                DBT[1+g16BadBlockNum] = BlockNo;
//            }
//        }
//    }
//    g8RBT_CRT_Block_Idx = 0;
//    g8RBT_CRT_Page = 0xFF;
//}


/*!
    \brief      init bad block table by scan nand flash
    \param[in]  none
    \param[out] none
    \retval     none
*/
//static void init_build_DBTRBT(void)
//{
//    uint16_t BlockNo = 0;
//#ifdef DEBUG
//    //printf("\r%s\n",__func__);    
//#endif
//    g16LastValidBlock = ReplaceBlock_AREA_END;
//    for(BlockNo = 0;BlockNo <= USER_AREA_END;BlockNo++){
//        if( g16LastValidBlock < ReplaceBlock_AREA_START ){  
//            break;
//        }   
//        if(spi_nandflash_badblock_detect(BlockNo)==1){
//            while(spi_nandflash_badblock_detect(g16LastValidBlock)==1){
//                g16BadBlockNum++;
//                DBT[1+g16BadBlockNum] = g16LastValidBlock;
//                g16LastValidBlock--;                
//            }
//            add_bad_Block_to_DBTRBT_ram(BlockNo);
//            continue;
//        }
//        else{
//            RBT[BlockNo] = BlockNo;
//        }
//    }
//    DBT[0] = g16BadBlockNum;
//    DBT[1] = g16LastValidBlock;
//}


///*!
//    \brief      rebuild bad block mapping table from nand flash
//    \param[in]  none
//    \param[out] none
//    \retval     result: rebuild success or not
//*/
//static uint8_t rebuild_DBTRBT_array(void)
//{
//    uint16_t  BlockNo = SPI_NAND_BLOCK_COUNT - 1;       //1023
//    uint16_t  PageNo = 0;                               //0~63
//    uint8_t   Flag_DBT_Exist = false;
//    uint8_t   Flag_RBT_Exist = false;
//    uint8_t   Spare_data[6];
//    uint16_t  Pair_block_No;
//    uint8_t   Spare_data2[6];
//    uint8_t   result  = true;
//    //check DBT whether exist in nand
//    for(BlockNo = Table_AREA_END; BlockNo >= Table_AREA_START; BlockNo--){                  //1023,1022
//        PageNo = 0;
//        spi_nandflash_read_spare(Spare_data,BlockNo * SPI_NAND_BLOCK_SIZE + PageNo,4,6);
//        if((Spare_data[0]=='D')&&(Spare_data[1]=='B')&&(Spare_data[2]=='T')&&(Spare_data[3]=='!')){
//            Flag_DBT_Exist = true;
//            break;
//        }
//    }

//    if(Flag_DBT_Exist == true){
//        for(PageNo = SPI_NAND_BLOCK_SIZE - 1; (PageNo < SPI_NAND_BLOCK_SIZE); PageNo--){    //0~63
//            spi_nandflash_read_spare(Spare_data,BlockNo * SPI_NAND_BLOCK_SIZE+PageNo,4,6);
//            if((Spare_data[0]=='D')&&(Spare_data[1]=='B')&&(Spare_data[2]=='T')&&(Spare_data[3]=='!')){
//                break;
//            }
//        }
//        g8DBT_CRT_Block_Idx = 0;
//        Pair_block_No = Spare_data[4] + (Spare_data[5]<<8);
//        spi_nandflash_read_spare(Spare_data2,Pair_block_No * SPI_NAND_BLOCK_SIZE+0,4,6);
//        //block的最后一个page，说明最新的Table可能在另一个block的Page0中。
//        if(PageNo == SPI_NAND_BLOCK_SIZE - 1){  //另一个block有内容，说明Erase时掉电了
//            if((Spare_data2[0]=='D')&&(Spare_data2[1]=='B')&&(Spare_data2[2]=='T')&&(Spare_data2[3]=='!')){
//                PageNo = 0;
//                g8DBT_CRT_Block_Idx = 1;                
//                bsp_spi_nandflash_block_erase(BlockNo);     //擦掉(因掉电造成未擦除的)写满的block
//            }
//        }
//        g16DBT_Block[0] = BlockNo;
//        g16DBT_Block[1] = Pair_block_No;
//        g8DBT_CRT_Page = PageNo;
//        load_DBTRBT_from_nand(TYPE_DBT);
//        g16BadBlockNum = DBT[0];        //bad block number
//        g16LastValidBlock = DBT[1];     //record last valid block   
//    }else{
//        //如果没找到就重新全盘扫描，重建DBTRBT
//        alloc_DBTRBT_block_addr();
//        init_build_DBTRBT();
//        update_DBTRBT_to_nand(TYPE_DBT);
//        update_DBTRBT_to_nand(TYPE_RBT);
//        return result;
//    }

//    //check RBT whether exist
//    for(BlockNo = Table_AREA_END; BlockNo >= Table_AREA_START; BlockNo--){                  //1023,1022
//        PageNo = 0;
//        spi_nandflash_read_spare(Spare_data,BlockNo * SPI_NAND_BLOCK_SIZE + PageNo,4,6);
//        if((Spare_data[0]=='R')&&(Spare_data[1]=='B')&&(Spare_data[2]=='T')&&(Spare_data[3]=='!')){
//            Flag_RBT_Exist = true;
//            break;
//        }
//    }
//    
//    if(Flag_RBT_Exist == true){
//        for(PageNo = SPI_NAND_BLOCK_SIZE - 1; (PageNo < SPI_NAND_BLOCK_SIZE); PageNo--){   //0~63
//            spi_nandflash_read_spare(Spare_data,BlockNo * SPI_NAND_BLOCK_SIZE+PageNo,4,6);
//            if((Spare_data[0]=='R')&&(Spare_data[1]=='B')&&(Spare_data[2]=='T')&&(Spare_data[3]=='!')){
//                break;
//            }
//        }
//        g8RBT_CRT_Block_Idx = 0;
//        Pair_block_No = Spare_data[4] + (Spare_data[5]<<8);
//        spi_nandflash_read_spare(Spare_data2,Pair_block_No * SPI_NAND_BLOCK_SIZE+0,4,6);
//        //block的最后一个page，说明最新的Table可能在另一个block的Page0中。
//        if(PageNo == SPI_NAND_BLOCK_SIZE - 1){  //另一个block有内容，说明Erase时掉电了
//            if((Spare_data2[0]=='R')&&(Spare_data2[1]=='B')&&(Spare_data2[2]=='T')&&(Spare_data2[3]=='!')){
//                PageNo = 0;
//                g8RBT_CRT_Block_Idx = 1;
//            }
//        }
//        g16RBT_Block[0] = BlockNo;
//        g16RBT_Block[1] = Pair_block_No;
//        g8RBT_CRT_Page = PageNo;
//        load_DBTRBT_from_nand(TYPE_RBT);
//        return result;
//    }else{
//        result = false;     //DBT exist but RBT not exist
//        return result;
//    }   
//}

/*!
    \brief      for debug
    \param[in]  none
    \param[out] none
    \retval     none
*/
#if 0
static void printfData(uint32_t a32SAddr, uint32_t byteCnt)
{
    uint32_t m32i;
    for( m32i = 0; m32i < byteCnt; m32i++ ){
        if((m32i & 0xF) == 0){
            printf("\r%x: ", m32i);
        }
        if((m32i & 0x7) == 0){
            printf("\t");
        }
        printf("%x ", *(uint8_t *)( a32SAddr + m32i));
        if((m32i & 0xF) == 0xF){
            printf("\n");
        }
    }
}
#endif


