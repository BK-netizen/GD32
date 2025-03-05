/*!
 * 文件名称： bsp_spi_nor.c
 * 描    述： bsp nor驱动文件
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

#include "bsp_spi_nor.h"

GPIO_DEF(NOR_FLASH_CS,F,6,OUT_PP,SET,NULL);


#define SPI_NOR_UNLOCK()  board_spi_device.spi_device_state_bits.device0=SET
#define SPI_NOR_LOCK()    board_spi_device.spi_device_state_bits.device0=RESET



void bsp_spi_nor_cs_low(void)
{
    bsp_spi_device_busy_wait(&board_spi_device); 
    SPI_NOR_UNLOCK();
    /* select the flash: chip select low */
    driver_gpio_pin_reset(&NOR_FLASH_CS);   
}  

void bsp_spi_nor_cs_high(void)
{
    /* select the flash: chip select high */
    driver_gpio_pin_set(&NOR_FLASH_CS);  
    SPI_NOR_LOCK();   
}  

/*!
    \brief      initialize SPI0 GPIO and parameter
    \param[in]  none
    \param[out] none
    \retval     none
*/
void bsp_spi_nor_init(void)
{
    driver_gpio_general_init(&NOR_FLASH_CS);      
}

/*!
    \brief      erase the specified flash sector
    \param[in]  sector_addr: address of the sector to erase
    \param[out] none
    \retval     none
*/
void bsp_spi_nor_sector_erase(uint32_t sector_addr)
{
    /* send write enable instruction */
    bsp_spi_nor_write_enable();
    /* sector erase */
    /* select the flash: chip select low */
    bsp_spi_nor_cs_low();
    /* send sector erase instruction */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,SE);
    /* send sector_addr high nibble address byte */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,(sector_addr & 0xFF0000) >> 16);
    /* send sector_addr medium nibble address byte */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,(sector_addr & 0xFF00) >> 8);
    /* send sector_addr low nibble address byte */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,sector_addr & 0xFF);
    /* deselect the flash: chip select high */
    bsp_spi_nor_cs_high();

    /* wait the end of flash writing */
    bsp_spi_nor_wait_for_write_end();
}

/*!
    \brief      erase the entire flash
    \param[in]  none
    \param[out] none
    \retval     none
*/
void bsp_spi_nor_bulk_erase(void)
{
    /* send write enable instruction */
    bsp_spi_nor_write_enable();

    /* bulk erase */
    /* select the flash: chip select low */
    bsp_spi_nor_cs_low();
    /* send bulk erase instruction  */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,BE);
    /* deselect the flash: chip select high */
    bsp_spi_nor_cs_high();

    /* wait the end of flash writing */
    bsp_spi_nor_wait_for_write_end();
}

/*!
    \brief      write more than one byte to the flash
    \param[in]  pbuffer: pointer to the buffer
    \param[in]  write_addr: flash's internal address to write
    \param[in]  num_byte_to_write: number of bytes to write to the flash
    \param[out] none
    \retval     none
*/
void bsp_spi_nor_page_write(uint8_t* pbuffer, uint32_t write_addr, uint16_t num_byte_to_write)
{
    /* enable the write access to the flash */
    bsp_spi_nor_write_enable();

    /* select the flash: chip select low */
    bsp_spi_nor_cs_low();

    /* send "write to memory" instruction */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,WRITE);
    /* send write_addr high nibble address byte to write to */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,(write_addr & 0xFF0000) >> 16);
    /* send write_addr medium nibble address byte to write to */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,(write_addr & 0xFF00) >> 8);
    /* send write_addr low nibble address byte to write to */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,write_addr & 0xFF);

    /* while there is data to be written on the flash */
    while(num_byte_to_write--){
        /* send the current byte */
        driver_spi_master_transmit_receive_byte(&BOARD_SPI,*pbuffer);
        /* point on the next byte to be written */
        pbuffer++;
    }

    /* deselect the flash: chip select high */
    bsp_spi_nor_cs_high();

    /* wait the end of flash writing */
    bsp_spi_nor_wait_for_write_end();
}

/*!
    \brief      write block of data to the flash
    \param[in]  pbuffer: pointer to the buffer
    \param[in]  write_addr: flash's internal address to write
    \param[in]  num_byte_to_write: number of bytes to write to the flash
    \param[out] none
    \retval     none
*/
void bsp_spi_nor_buffer_write(uint8_t* pbuffer, uint32_t write_addr, uint16_t num_byte_to_write)
{
    uint8_t num_of_page = 0, num_of_single = 0, addr = 0, count = 0, temp = 0;

    addr          = write_addr % SPI_FLASH_PAGE_SIZE;
    count         = SPI_FLASH_PAGE_SIZE - addr;
    num_of_page   = num_byte_to_write / SPI_FLASH_PAGE_SIZE;
    num_of_single = num_byte_to_write % SPI_FLASH_PAGE_SIZE;

     /* write_addr is SPI_FLASH_PAGE_SIZE aligned  */
    if(0 == addr){
        /* num_byte_to_write < SPI_FLASH_PAGE_SIZE */
        if(0 == num_of_page)
            bsp_spi_nor_page_write(pbuffer,write_addr,num_byte_to_write);
        /* num_byte_to_write > SPI_FLASH_PAGE_SIZE */
        else{
            while(num_of_page--){
                bsp_spi_nor_page_write(pbuffer,write_addr,SPI_FLASH_PAGE_SIZE);
                write_addr += SPI_FLASH_PAGE_SIZE;
                pbuffer += SPI_FLASH_PAGE_SIZE;
            }
            bsp_spi_nor_page_write(pbuffer,write_addr,num_of_single);
        }
    }else{
        /* write_addr is not SPI_FLASH_PAGE_SIZE aligned  */
        if(0 == num_of_page){
            /* (num_byte_to_write + write_addr) > SPI_FLASH_PAGE_SIZE */
            if(num_of_single > count){
                temp = num_of_single - count;
                bsp_spi_nor_page_write(pbuffer,write_addr,count);
                write_addr += count;
                pbuffer += count;
                bsp_spi_nor_page_write(pbuffer,write_addr,temp);
            }else
                bsp_spi_nor_page_write(pbuffer,write_addr,num_byte_to_write);
        }else{
            /* num_byte_to_write > SPI_FLASH_PAGE_SIZE */
            num_byte_to_write -= count;
            num_of_page = num_byte_to_write / SPI_FLASH_PAGE_SIZE;
            num_of_single = num_byte_to_write % SPI_FLASH_PAGE_SIZE;

            bsp_spi_nor_page_write(pbuffer,write_addr, count);
            write_addr += count;
            pbuffer += count;

            while(num_of_page--){
                bsp_spi_nor_page_write(pbuffer,write_addr,SPI_FLASH_PAGE_SIZE);
                write_addr += SPI_FLASH_PAGE_SIZE;
                pbuffer += SPI_FLASH_PAGE_SIZE;
            }

            if(0 != num_of_single)
                bsp_spi_nor_page_write(pbuffer,write_addr,num_of_single);
        }
    }
}

/*!
    \brief      read a block of data from the flash
    \param[in]  pbuffer: pointer to the buffer that receives the data read from the flash
    \param[in]  read_addr: flash's internal address to read from
    \param[in]  num_byte_to_read: number of bytes to read from the flash
    \param[out] none
    \retval     none
*/
void bsp_spi_nor_buffer_read(uint8_t* pbuffer, uint32_t read_addr, uint16_t num_byte_to_read)
{
    /* select the flash: chip slect low */
    bsp_spi_nor_cs_low();

    /* send "read from memory " instruction */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,READ);

    /* send read_addr high nibble address byte to read from */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,(read_addr & 0xFF0000) >> 16);
    /* send read_addr medium nibble address byte to read from */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,(read_addr& 0xFF00) >> 8);
    /* send read_addr low nibble address byte to read from */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,read_addr & 0xFF);

    /* while there is data to be read */
    while(num_byte_to_read--){
        /* read a byte from the flash */
        *pbuffer = driver_spi_master_transmit_receive_byte(&BOARD_SPI,NOR_DUMMY_BYTE);
        /* point to the next location where the byte read will be saved */
        pbuffer++;
    }

    /* deselect the flash: chip select high */
    bsp_spi_nor_cs_high();
}

/*!
    \brief      read flash identification
    \param[in]  none
    \param[out] none
    \retval     flash identification
*/
uint32_t bsp_spi_nor_read_id(void)
{
    uint32_t temp = 0, temp0 = 0, temp1 = 0, temp2 = 0;

    /* select the flash: chip select low */
    bsp_spi_nor_cs_low();

    /* send "RDID " instruction */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,0x9F);

    /* read a byte from the flash */
    temp0 = driver_spi_master_transmit_receive_byte(&BOARD_SPI,NOR_DUMMY_BYTE);

    /* read a byte from the flash */
    temp1 = driver_spi_master_transmit_receive_byte(&BOARD_SPI,NOR_DUMMY_BYTE);

    /* read a byte from the flash */
    temp2 = driver_spi_master_transmit_receive_byte(&BOARD_SPI,NOR_DUMMY_BYTE);

    /* deselect the flash: chip select high */
    bsp_spi_nor_cs_high();

    temp = (temp0 << 16) | (temp1 << 8) | temp2;

    return temp;
}

/*!
    \brief      initiate a read data byte (read) sequence from the flash
    \param[in]  read_addr: flash's internal address to read from
    \param[out] none
    \retval     none
*/
void bsp_spi_nor_start_read_sequence(uint32_t read_addr)
{
    /* select the flash: chip select low */
    bsp_spi_nor_cs_low();

    /* send "read from memory " instruction */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,READ);

    /* send the 24-bit address of the address to read from */
    /* send read_addr high nibble address byte */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,(read_addr & 0xFF0000) >> 16);
    /* send read_addr medium nibble address byte */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,(read_addr& 0xFF00) >> 8);
    /* send read_addr low nibble address byte */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,read_addr & 0xFF);
}

/*!
    \brief      read a byte from the SPI flash
    \param[in]  none
    \param[out] none
    \retval     byte read from the SPI flash
*/
uint8_t bsp_spi_nor_read_byte(void)
{
    return(driver_spi_master_transmit_receive_byte(&BOARD_SPI,NOR_DUMMY_BYTE));
}


/*!
    \brief      enable the write access to the flash
    \param[in]  none
    \param[out] none
    \retval     none
*/
void bsp_spi_nor_write_enable(void)
{
    /* select the flash: chip select low */
    bsp_spi_nor_cs_low();

    /* send "write enable" instruction */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,WREN);

    /* deselect the flash: chip select high */
    bsp_spi_nor_cs_high();
}

/*!
    \brief      poll the status of the write in progress(wip) flag in the flash's status register
    \param[in]  none
    \param[out] none
    \retval     none
*/
void bsp_spi_nor_wait_for_write_end(void)
{
    uint8_t flash_status = 0;

    /* select the flash: chip select low */
    bsp_spi_nor_cs_low();

    /* send "read status register" instruction */
    driver_spi_master_transmit_receive_byte(&BOARD_SPI,RDSR);

    /* loop as long as the memory is busy with a write cycle */
    do{
        /* send a dummy byte to generate the clock needed by the flash
        and put the value of the status register in flash_status variable */
        flash_status = driver_spi_master_transmit_receive_byte(&BOARD_SPI,NOR_DUMMY_BYTE);
    }while((flash_status & WIP_FLAG) == SET);

    /* deselect the flash: chip select high */
    bsp_spi_nor_cs_high();
}

