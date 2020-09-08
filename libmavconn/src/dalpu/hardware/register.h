#ifndef _DALPU_REGISTER_H
#define _DALPU_REGISTER_H   1

//**************************************************************************
//* Register Address
//**************************************************************************

#define ADDR_NOR_W      0x31
#define ADDR_NOR_R      0x21
#define COMM_NOR_W      0x11
#define COMM_NOR_R      0x01
#define ADDR_DUPLEX_W   0x39

#define RG_BT_LOGIC_SEL                 0x0CA1
#define RG_EET_CTRL                     0x06B0
#define RG_EET_OPMODE                   0x06B1
#define RG_EET_BYOB_LEN                 0x06B2
#define RG_EET_BYOB_ADDR_LSB            0x06B3
#define RG_EET_BYOB_ADDR_MSB            0x06B4
#define RG_ST1_RND_OPMODE               0x0608
#define RG_OKA_CTRL                     0x063C
#define RG_ST1_MIDR_OPMODE              0x060B
#define RG_RNDGEN_USER                  0x0700
#define RG_SUPER_WIRE_PW0               0x0660
#define RG_PERM_GET_CTRL                0x0626
#define RG_PERM_GET_CTRL1               0x0627
#define RG_PERM_RELEASE                 0x0628
#define RG_ST1_OKA_OPMODE               0x060A
#define RG_ST3_SYMCIP_RSCREATE_OPMODE   0x061D
#define RG_ST3_SYMCIP_KEYLOAD_OPMODE    0x061F
#define RG_FFFF                         0x0FFF
#define RG_EE_USER_ZONE_SEL             0x061A
#define RG_PERM_GET_EE_RD_PRE_SP        0x0629
#define RG_EE_CFG_RD_RG_EEBUF_ST        0x061C
#define RG_MCUAuthResult                0x0720
#define RG_ST2_SYMCIP_SHAAuth_CMP_DP    0x0721
#define RG_EETEST_BYOB_ADDR_LSB         0x06B3
#define RG_EET_BYOB_ADDR_0              0x06B3 // 17'h1_06B3 // [ 7: 0]
#define RG_EET_BYOB_ADDR_1              0x06B4 // 17'h1_06B4 // [15: 8]
#define RG_EET_BYOB_ADDR_2              0x06B5 // 17'h1_06B5 // [16   ]

#define RG_SHA_CTRL                     0x0638
#define RG_ST1_STDSPI_OPMODE            0x0606

// DORCA4
#define RG_EE_KEY_AES_CTRL              0x0620
#define RG_SPI_LENGTH                   0x0633
#define RG_BUF_CTRL                     0x0634
#define RG_AES_CTRL                     0x0635
#define RG_ST0_OPMODE                   0x0604
#define RG_ST0_OPMODE_TEST              0xA5A5
#define RG_ST1_SYMCIP_OPMODE            0x0609
#define RG_ST2_SYMCIP_OPMODE            0x0619

#define RG_TLS_START_ADDR0              0x0636
#define RG_TLS_START_ADDR1              0x0637
#define RG_TLS_ADDR0                    0x0636
#define RG_TLS_ADDR1                    0x0637
#define RG_TLS_xN_WR                    0x0638
#define RG_TLS_xN_RD                    0x0639
#define RG_TLS_MODE                     0x063A
#define RG_TLS_380_IV                   0x0380
#define RG_TLS_360_KEY                  0x0360

#define RG_SLEEP_REQ                    0x064B
#define RG_SLEEP_CLR                    0x064C
#define RG_WAKEUP_REQ                   0x064D
#define RG_WAKEUP_CLR                   0x064E
#define RG_IRQ_STATUS                   0x064F
#define RG_SLEEP_MODE                   0x0652
#define RG_SLEEP_TIMER                  0x0653

#define RG_EEBUF100                     0x0100
#define RG_EEBUF300                     0x0300
#define RG_EEBUF308                     0x0308
#define RG_EEBUF310                     0x0310
#define RG_EEBUF318                     0x0318
#define RG_EEBUF320                     0x0320
#define RG_EEBUF328                     0x0328
#define RG_EEBUF330                     0x0330
#define RG_EEBUF338                     0x0338
#define RG_EEBUF360                     0x0360
#define RG_EEBUF380                     0x0380
#define RG_EEBUF400                     0x0400
#define RG_EEBUF408                     0x0408
#define RG_EEBUF410                     0x0410
#define RG_EEBUF418                     0x0418
#define RG_EEBUF420                     0x0420
#define RG_EEBUF428                     0x0428
#define RG_EEBUF430                     0x0430
#define RG_EEBUF438                     0x0438
#define RG_EEBUF500                     0x0500
#define RG_EEBUF510                     0x0510

#define RG_SLEEP_TIMER_MSB              0x0650
#define RG_SLEEP_TIMER_LSB              0x0651
#define RG_KL_CTRL                      0x0622
#define RG_RSCREATE_CTRL                0x0623
#define RG_MEM_TEST_OPMODE              0x06B6

#define RG_SHAAUTH_CTRL                 0x0624
#define RG_SOFT_RESET                   0x0600

#define RG_ACCESS                       0x0601

#define A_MIDR                          0xED00
#define A_EE_MEM_BKUP_RSFLAG            0xEF00
#define A_EE_MEM_BKUP_CTRL              0xEF01

#define RG_PON_STATUS                   0x0651

/////////////////////////////////// BIST ///////////////////////////////////////////////////////////
#define RG_BIST_MODE                    0x0722
#define RG_EE_BI_NO                     0x0723
#define RG_MB_ERROR_BIT                 0x0724
#define RG_SRAM_BIST_RESULT             0x0725
#define RG_TRIM                         0x0727
#define RG_TEST                         0x0726
#define RG_ST1_MEM_TEST_OPMODE          0x060E
/////////////////////////////////// BIST ///////////////////////////////////////////////////////////

/////////////////////////////////// DEBUG //////////////////////////////////////////////////////////
#define RG_ST0_CUR                      0x0C00
#define RG_CHK_RSFLAG                   0x0C01
#define RG_ST1_CHK_RSFLAG_CUR           0x0C02
#define RG_ST1_PON_READ_CUR             0x0C03
#define RG_STCM0_CUR                    0x0C04
#define RG_ST1_STDSPI_CUR               0x0C05
#define RG_ST1_EE_CFG_CUR               0x0C06
#define RG_ST1_RND_CUR                  0x0C07
#define RG_ST1_SYMCIP_CUR               0x0C08
#define RG_ST1_OKA_CUR                  0x0C09
#define RG_ST1_MIDR_CUR                 0x0C0A
#define RG_ST1_PERM_GET_CUR             0x0C0B
#define RG_ST1_EEP_OW_CTRL_CUR          0x0C0C
#define RG_ST1_MEM_TEST_CUR             0x0C0D
#define RG_ST2_EEP_OW_CTRL_CUR          0x0C0E
#define RG_ST2_SYMCIP_OPMODE_AES_CUR    0x0C0F
#define RG_ST2_OKA_OKA2_CUR             0x0C10
#define RG_ST2_STDSPI_SHA_CUR           0x0C11
#define RG_ST2_SYMCIP_SHAAuth_CUR       0x0C12
#define RG_ST2_RND_CUR                  0x0C13
#define RG_ST3_RND_CUR                  0x0C14
#define RG_ST3_SYMCIP_AES_CUR           0x0C15
#define RG_ST3_SYMCIP_KEYLOAD_CUR       0x0C16
#define RG_ST3_SYMCIP_RSCREATE_CUR      0x0C17
/////////////////////////////////// DEBUG //////////////////////////////////////////////////////////

typedef enum {
    RG_MODE_ECB = 0,
    RG_MODE_CBC,
    RG_MODE_OFB,
    RG_MODE_CTR,
    RG_MODE_CFB
} rg_mode_t;

typedef enum {
    RG_KEY_256 = 0,
    RG_KEY_128
} rg_key_t;

typedef enum {
    RG_ALG_ARIA = 0,
    RG_ALG_AES
} rg_alg_t;

typedef enum {
    RG_ENC = 0,
    RG_DEC
} rg_encdec_t;

#endif //_DALPU_REGISTER_H
