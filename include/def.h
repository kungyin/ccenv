#ifndef _DEF_H_
#define _DEF_H_

#define _ONDEVICE_

#ifdef _ONDEVICE_
#define FACTORY_DIRPATH "/bin"
#define ENV_DIRPATH "/tmp/factory"
#else
#define FACTORY_DIRPATH "."
#define ENV_DIRPATH "."
#endif

#define FACTORY_EXECUTABLE_FILE FACTORY_DIRPATH"/factory"
#define ENV_FILEPATH ENV_DIRPATH"/env.txt"

#define CMD_SETENV "set"
#define CMD_GETENV "get"
#define CMD_PRINTENV "print"

char default_env[][2][100] = {
    { { 0x40, 0xaf, 0x99, 0xe1, 'b', 'a', 'u', 'd', 'r', 'a', 't', 'e' }, "115200" },
    { "bootargs", "console=ttyS0,115200 earlyprintk loglevel=4" },
    { "bootcmd", "bootr" },
    { "bootdelay", "0" },
    { "ethact", "r8168#0" },
    { "ethaddr", "00:10:20:30:40:50" },
    { "fdt_loadaddr", "1ff2000" },
    { "gatewayip", "192.168.3.254" },
    { "ipaddr", "192.168.3.10" },
    { "kernel_loadaddr", "3000000" },
    { "mtd_part", "mtdparts=rtk_nand:27776k,89600k(/),7168k(etc)249088k" },
    { "netmask", "255.255.255.0" },
    { "rescue_audio", "bluecore.audio" },
    { "rescue_dtb", "rescue.nand.dtb" },
    { "rescue_rootfs", "rescue.root.nand.cpio.gz_pad.img" },
    { "rescue_vmlinux", "nand.uImage" },
    { "rootfs_loadaddr", "0x02200000" },
    { "serverip", "192.168.3.11" }
};

#endif
