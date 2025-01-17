
/*************************************************************************
    > File Name: w25q64.c
    > Author: zhoubo
    > Mail: 2008540876@qq.com
    > Created Time: 2019年03月17日 星期日 22时36分22秒
 ************************************************************************/

#include "w25q64.h"

void w25q264_Init(void)
{
    Spi_Init();
    Spi_Cs_SetPin(0);
    Spi_WriteOneByte(0xff);
    Spi_Cs_SetPin(1);
}

//w2564读取状态寄存器
u8 w25q64_ReadSR(void)
{
    u8 sta = 0;
    Spi_Cs_SetPin(0);                       //使能片选
    Spi_WriteOneByte(w25q64_ReadStatusReg);
    sta = Spi_ReadOneByte();
    Spi_Cs_SetPin(1);                       //取消片选
    return sta;
}

//w25q64写状态寄存器
void w25q64_WriteSR(u8 sta)
{
    Spi_Cs_SetPin(0);
    Spi_WriteOneByte(w25q64_WriteStatusReg);
    Spi_WriteOneByte(sta);
    Spi_Cs_SetPin(1);
}

//w25q64写使能
void w25q64_Write_Enable(void)
{
    Spi_Cs_SetPin(0);
    Spi_WriteOneByte(w25q64_WriteEnable);
    Spi_Cs_SetPin(1);
}

//w25q64禁止写
void w25q64_Write_Disable(void)
{
    Spi_Cs_SetPin(0);
    Spi_WriteOneByte(w25q64_WriteDisable);
    Spi_Cs_SetPin(1);
}

//w25q64等待空闲
void w25q64_Wait_Busy(void)
{
    while ((w25q64_ReadSR() & 0x01) == 0x01);   // 等待busy为清空
}

//读取w25q64的ID
u16 w25q64_ReadID()
{
    u16 Temp = 0;
    Spi_Cs_SetPin(0);
    Spi_WriteOneByte(0x90);                     //发送读取ID命令
    Spi_WriteOneByte(0x00);
    Spi_WriteOneByte(0x00);
    Spi_WriteOneByte(0x00);
    Temp |= Spi_ReadOneByte() << 8;
    Temp |= Spi_ReadOneByte();
    Spi_Cs_SetPin(1);
    return Temp;
}

//w25q64写一页数据
void w25q64_Write_OnePage(u8 *buf, u32 addr, u16 size)
{
    u16 i;
    w25q64_Write_Enable();
    Spi_Cs_SetPin(0);                           //使能器件
    Spi_WriteOneByte(w25q64_PageProgram);       //发送写页命令
    Spi_WriteOneByte((u8)((addr) >> 16));       //发送24bit地址
    Spi_WriteOneByte((u8)((addr) >> 8));
    Spi_WriteOneByte((u8)addr);
    for (i = 0; i < size; i++)
        Spi_WriteOneByte(buf[i]);               //循环写数
    Spi_Cs_SetPin(1);                           //取消片选
    w25q64_Wait_Busy();                         //等待写入结束
}

//无检验写w25q64
void w25q64_Write_NoCheck(u8 *buf, u32 addr, u16 size)
{
    u16 pageremain;
    pageremain = 256 - addr % 256;      //单页剩余的字节数
    if (size <= pageremain)
        pageremain = size;              //不大于256个字节
    while (1)
    {
        w25q64_Write_OnePage(buf, addr, pageremain);
        if (size == pageremain)
            break;                      //写入结束
        else                            //size > pageremain
        {
            buf += pageremain;
            addr += pageremain;

            size -= pageremain;         //减去已经写入了的字节数
            if (size > 256)
                pageremain=256;         //一次可以写入256个字节
            else
                pageremain = size; //不够256个字节
        }
    }
}

//w25q64擦除一个扇区
void w25q64_Erase_Sector(u32 Dst_Addr)
{
    Dst_Addr *= 4096;
    w25q64_Write_Enable();
    w25q64_Wait_Busy();
    Spi_Cs_SetPin(0);
    Spi_WriteOneByte(w25q64_SectorErase);       //发送扇区擦除指令
    Spi_WriteOneByte((u8)((Dst_Addr) >> 16));   //发送24bit地址
    Spi_WriteOneByte((u8)((Dst_Addr) >> 8));
    Spi_WriteOneByte((u8)Dst_Addr);
    Spi_Cs_SetPin(1);
    w25q64_Wait_Busy();                         //等待擦除完成
}

//w25q64整片删除
void w25q64_Erase_Chip(void)
{
    w25q64_Write_Enable();
    w25q64_Wait_Busy();
    Spi_Cs_SetPin(0);
    Spi_WriteOneByte(w25q64_ChipErase);         //发送片擦除命令
    Spi_Cs_SetPin(1);
    w25q64_Wait_Busy();                         //等待芯片擦除结束
}

//w25q64写入数据
u8 w25q64BUF[4096];
void w25q64_Write_Data(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
    u32 secpos;
    u16 secoff;
    u16 secremain;
    u16 i;

    secpos = WriteAddr / 4096;          //扇区地址
    secoff = WriteAddr % 4096;          //在扇区内的偏移
    secremain = 4096 - secoff;          //扇区剩余空间大小

    if (NumByteToWrite <= secremain)
        secremain = NumByteToWrite;     //不大于4096个字节
    while (1)
    {
        w25q64_Read_Data(w25q64BUF, secpos * 4096,4096);    //读出整个扇区的内容
        for (i = 0; i < secremain; i++)                     //校验数据
        {
            if (w25q64BUF[secoff+i] != 0XFF)
                break;                                      //需要擦除
        }
        if (i < secremain)                                  //需要擦除
        {
            w25q64_Erase_Sector(secpos);                    //擦除这个扇区
            for (i = 0; i < secremain; i++)                 //复制
            {
                w25q64BUF[i+secoff] = pBuffer[i];
            }
            w25q64_Write_NoCheck(w25q64BUF, secpos * 4096, 4096);//写入整个扇区

        }
        else
            w25q64_Write_NoCheck(pBuffer,WriteAddr,secremain);//写已经擦除了的,直接写入扇区剩余区间.
        if(NumByteToWrite == secremain)
            break;                                          //写入结束了
        else                                                //写入未结束
        {
            secpos++;                                       //扇区地址增1
            secoff = 0;                                     //偏移位置为0

            pBuffer += secremain;                           //指针偏移
            WriteAddr += secremain;                         //写地址偏移
            NumByteToWrite -= secremain;                    //字节数递减
            if (NumByteToWrite > 4096)
                secremain = 4096;                           //下一个扇区还是写不完
            else
                secremain = NumByteToWrite;                 //下一个扇区可以写完了
        }
    }
}

//w25q64读数据
void w25q64_Read_Data(u8 *buf, u32 addr, u16 size)
{
    u16 i;
    Spi_Cs_SetPin(0);                           //使能器件
    Spi_WriteOneByte(w25q64_ReadData);          //发送读取命令
    Spi_WriteOneByte((u8)((addr) >> 16));       //发送24bit地址
    Spi_WriteOneByte((u8)((addr) >> 8));
    Spi_WriteOneByte((u8)addr);
    for (i = 0; i < size; i++)
    {
        buf[i] = Spi_ReadOneByte();             //循环读数
    }
    Spi_Cs_SetPin(1);                           //取消片选
}

/* 内核驱动部分 */

#define DEVICE_NAME		"w25q64"
static int major = 231;
static int minor = 0;
module_param(major, int, S_IRUGO);
module_param(minor, int, S_IRUGO);

static dev_t devno;
static struct cdev *cdev;
static struct class *w25q64_class;

spi_regs_t *spi_regs;
static int w25q64_open(struct inode *inode, struct file *file)
{
    spi_regs = ioremap(0x56000010, sizeof(spi_regs_t));
    w25q264_Init();
    u16 id = w25q64_ReadID();
    printk(KERN_INFO "w25q64_id = %x \n", id);
    return 0;
}

static int w25q64_read(struct file *file, const char __user *buf, ssize_t count, loff_t *ppos)
{
    unsigned err = 0;
    u8 data[count];

    w25q64_Read_Data(data, 100, count);       //从第100个地址处开始

    err = copy_to_user(buf, data, count);
    if (err)
        printk(KERN_ERR "copy to user error \n");
    return err? -EFAULT:0;
}

static int w25q64_write(struct file *file, const char __user *buf, ssize_t count, loff_t *ppos)
{
    unsigned err = 0;
    u8 data[count];

    err = copy_from_user(data, buf, count);
    if (err)
        printk(KERN_ERR "copy from user error \n");
    w25q64_Write_Data(data, 100, count);
    return err? -EFAULT:0;
}

static struct file_operations w25q64_fops =
{
    .owner = THIS_MODULE,
    .open  = w25q64_open,
    .read  = w25q64_read,
    .write = w25q64_write,
};

static int __init w25q64_init(void)
{
    int ret = 0;
    if (major)			//静态添加设备号
    {
        devno = MKDEV(major, minor);
        ret = register_chrdev_region(devno, minor, DEVICE_NAME);
    }
    else				//动态添加设备号
    {
        ret = alloc_chrdev_region(&devno, minor, 1, DEVICE_NAME);
        major = MAJOR(devno);
    }
    if (ret < 0)
    {
        printk(KERN_ERR "can't add the devno \n");
        return -1;
    }
    cdev = cdev_alloc();
    if (cdev)
    {
        cdev_init(cdev, &w25q64_fops);
        if (cdev_add(cdev, devno, 1))
        {
            printk(KERN_ERR "can't add the w25q64 dvice\n");
            goto err;
        }
    }
    w25q64_class = class_create(THIS_MODULE, "w25q64_class");
    if (IS_ERR(w25q64_class))
    {
        printk(KERN_ERR "Can't add the w25q64 class \n");
        return -1;
    }
    device_create(w25q64_class, NULL, devno, NULL, DEVICE_NAME);
    printk(KERN_INFO "The w25q64 device created completely\n");
    return 0;
err:
    unregister_chrdev_region(devno, 1);
    return -1;
}
static void __exit w25q64_exit(void)
{
    device_destroy(w25q64_class, devno);
    class_destroy(w25q64_class);
    cdev_del(cdev);
    unregister_chrdev_region(devno, 1);
    iounmap(spi_regs);
    printk(KERN_INFO "w25q64 device unregistered completely \n");
}
module_init(w25q64_init);
module_exit(w25q64_exit);
MODULE_LICENSE("GPL");
