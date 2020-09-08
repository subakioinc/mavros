#ifndef _DALPU_CIPHER_H
#define _DALPU_CIPHER_H     1

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DALPU_CIPHER_ID_AES = 0,
    DALPU_CIPHER_ID_ARIA,
} dalpu_cipher_id_t;

typedef enum {
    DALPU_MODE_ECB = 0,
    DALPU_MODE_CBC,
    DALPU_MODE_OFB,
    DALPU_MODE_CTR,
    DALPU_MODE_CFB,
} dalpu_cipher_mode_t;

typedef enum {
    DALPU_ENCRYPT = 0,
    DALPU_DECRYPT,
} dalpu_operation_t;

#ifdef __cplusplus
}
#endif

#endif /* _DALPU_CIPHER_H */
