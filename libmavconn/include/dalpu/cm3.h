#ifndef _DALPU_CM3_H
#define _DALPU_CM3_H    1

#define SPI1_WRITE_DATA             0x61
#define SPI1_READ_DATA              0x71
#define Set_ECC_q                   0x01
#define Set_ECC_a                   0x02
#define Set_ECC_b                   0x03
#define Set_ECC_xG                  0x04
#define Set_ECC_yG                  0x05
#define Set_ECC_n                   0x06
#define Set_Curve                   0x07
//#define SLEEP                     0x08
#define DEEP_SLEEP                  0x09
#define Set_ECDH_PrivateKey         0x11
#define Set_ECDH_PrivateKey_PUF     0x12
#define Create_ECHD_PublicKey       0x13
#define Set_ECDH_PublicKey_X        0x14
#define Set_ECDH_PublicKey_Y        0x15
#define Get_ECDH_PublicKey_X        0x16
#define Get_ECDH_PublicKey_Y        0x17
#define Create_ECHD_KEY             0x18
#define Get_ECDH_KEY_X              0x19
#define SET_EEPROM_BY_KEY           0x1a
#define Get_ECDH_KEY_Y              0x20
#define Set_ECDH_MY_PublicKey_X     0x21
#define Set_ECDH_MY_PublicKey_Y     0x22
#define Get_ECDH_Result             0x23

#define Set_PrivateKEY1             0x23
#define Set_PrivateKEY2             0x24
#define Set_PrivateKEY3             0x25
#define Set_PrivateKEY4             0x26

#define Set_PrivateKEY_K_1          0x27
#define Set_PrivateKEY_K_2          0x28
#define Set_PrivateKEY_K_3          0x29
#define Set_PrivateKEY_K_4          0x2A

#define Set_SHA_Data                0x30
#define Get_SHA_Data                0x35

#define Set_ECDSA_PrivateKey        0x41
#define Set_ECDSA_PrivateKey_PUF    0x42
#define Set_ECDSA_K_RND             0x43
#define Set_ECDSA_K_RND_PUF         0x44
#define Set_ECDSA_Public_Key_Xq     0x45
#define Set_ECDSA_Public_Key_Yq     0x46
#define Create_ECDSA_Public_Key     0x47
#define Get_ECDSA_Public_Key_Xq     0x48
#define Get_ECDSA_Public_Key_Yq     0x49
#define Set_ECDSA_r                 0x50
#define Set_ECDSA_s                 0x51
#define Get_ECDSA_r                 0x52
#define Get_ECDSA_s                 0x53
#define Create_ECDSA_Sign           0x54
#define Set_ECDSA_h                 0x55
#define Get_ECDSA_h                 0x56
#define Get_ECDSA_Result            0x57
#define Do_ECDSA_Verify             0x58

#define EHCO                        0x71

#define Set_RSA_Prime_q             0x81
#define Set_RSA_Prime_p             0x82
#define Set_RSA_PublicExpo          0x83
#define Set_RSA_PrivateKey_d        0x84
#define Set_RSA_PlainText_M         0x85
#define Set_RSA_CipherText_C        0x86
#define Get_RSA_CipherText_C        0x87
#define Set_RSA_Modulus_n           0x88
#define Get_RSA_PlainText_M         0x89
#define Get_RSA_PublicExpo          0x90
#define Get_RSA_PrivateKed_d        0x91
#define Get_RSA_Modulus_n           0x92
#define Create_RSA_Key              0x93

#define Encrypt_RSA                 0x94
#define Decrypt_RSA                 0x95
#define SIZE_EC_ELGAMEL_256         0x96
#define SIZE_ECDH_256               0xA0
#define SIZE_ECDH_521               0xA1
#define SIZE_ECDSA_256              0xA2
#define SIZE_ECDSA_521              0xA3
#define SIZE_RSA_2048               0xA4
#define SAVE_RSA_PARA               0xA5
#define SAVE_KEY                    0xA6
#define MAKE_RAND                   0xA7
#define Get_RAND                    0xA8
#define SET_EC_ELGAMEL_p            0xB0
#define SET_EC_ELGAMEL_n            0xB1
#define SET_EC_ELGAMEL_Gx           0xB2
#define SET_EC_ELGAMEL_Gy           0xB3
#define SET_EC_ELGAMEL_a            0xB4
#define SET_EC_ELGAMEL_b            0xB5
#define SET_EC_ELGAMEL_x            0xB6
#define SET_EC_ELGAMEL_k            0xB7
#define SET_EC_ELGAMEL_SmX          0xB8
#define SET_EC_ELGAMEL_SmY          0xB9
#define SET_EC_ELGAMEL_PmX          0xBA
#define SET_EC_ELGAMEL_PmY          0xBB
#define SET_EC_ELGAMEL_CX           0xBC
#define SET_EC_ELGAMEL_CY           0xBD
#define SET_EC_ELGAMEL_C2X          0xBE
#define SET_EC_ELGAMEL_C2Y          0xBF
#define SET_EC_ELGAMEL_DX           0xC0
#define SET_EC_ELGAMEL_DY           0xC1
#define SET_EC_ELGAMEL_CprimeX      0xC2
#define SET_EC_ELGAMEL_CprimeY      0xC3
#define SET_EC_ELGAMEL_YX           0xC4
#define SET_EC_ELGAMEL_YY           0xC5

#define GET_EC_ELGAMEL_p            0xD0
#define GET_EC_ELGAMEL_n            0xD1
#define GET_EC_ELGAMEL_Gx           0xD2
#define GET_EC_ELGAMEL_Gy           0xD3
#define GET_EC_ELGAMEL_a            0xD4
#define GET_EC_ELGAMEL_b            0xD5
#define GET_EC_ELGAMEL_x            0xD6
#define GET_EC_ELGAMEL_k            0xD7
#define GET_EC_ELGAMEL_PmSrcX       0xD8
#define GET_EC_ELGAMEL_PmSrcY       0xD9
#define GET_EC_ELGAMEL_PmX          0xDA
#define GET_EC_ELGAMEL_PmY          0xDB
#define GET_EC_ELGAMEL_CX           0xDC
#define GET_EC_ELGAMEL_CY           0xDD
#define GET_EC_ELGAMEL_C2X          0xDE
#define GET_EC_ELGAMEL_C2Y          0xDF
#define GET_EC_ELGAMEL_DX           0xE0
#define GET_EC_ELGAMEL_DY           0xE1
#define GET_EC_ELGAMEL_CprimeX      0xE2
#define GET_EC_ELGAMEL_CprimeY      0xE3
#define GET_EC_ELGAMEL_YX           0xEE
#define GET_EC_ELGAMEL_YY           0xE4
#define GET_EC_ELGAMEL_XPrime       0xEF

#define CALC_EC_ELGAMEL_Sm          0xE5
#define CALC_EC_ELGAMEL_C           0xE7
#define CALC_EC_ELGAMEL_C2          0xE8
#define CALC_EC_ELGAMEL_D           0xE9
#define CALC_EC_ELGAMEL_Y           0xEA
#define CALC_EC_ELGAMEL_Xprime      0xEB
#define CALC_EC_ELGAMEL_Cprime      0xEC
#define CALC_EC_ELGAMEL_ORG_PM      0xED
#define CALC_EC_ELGAMEL_Pm          0xEF
#define CALC_EC_ELGAMEL_PmSrc       0xFF

#define SET_KEY_SIZE                0xAB
#define GET_KEY_SIZE                0xAC
#define EEP_KEY_Index0              0xF0
#define EEP_KEY_Index1              0xF1
#define EEP_KEY_Index2              0xF2
#define EEP_KEY_Index3              0xF3
#define EEP_KEY_Index4              0xF4
#define EEP_KEY_Index5              0xF5
#define EEP_KEY_Index6              0xF6
#define EEP_KEY_Index7              0xF7
#define Set_Key_EEPROM              0xFA
#define Get_Key_EEPROM              0xFB
#define Set_KeyC_EEPROM             0xFC

#define Get_TRNG_READ               0xFD

#define SPI1_SPEED                  500*1000

#endif //_DALPU_CM3_H
