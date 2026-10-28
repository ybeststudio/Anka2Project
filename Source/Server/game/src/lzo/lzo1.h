#ifndef __LZO1_H
#define __LZO1_H

#ifndef __LZOCONF_H
#include <lzoconf.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define LZO1_MEM_COMPRESS       ((lzo_uint32) (8192L * lzo_sizeof_dict_t))
#define LZO1_MEM_DECOMPRESS     (0)


LZO_EXTERN(int)
lzo1_compress           ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uintp dst_len,
                                lzo_voidp wrkmem );

LZO_EXTERN(int)
lzo1_decompress         ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uintp dst_len,
                                lzo_voidp wrkmem);

#define LZO1_99_MEM_COMPRESS    ((lzo_uint32) (65536L * lzo_sizeof_dict_t))

#if !defined(LZO_99_UNSUPPORTED)
LZO_EXTERN(int)
lzo1_99_compress        ( const lzo_byte *src, lzo_uint  src_len,
                                lzo_byte *dst, lzo_uintp dst_len,
                                lzo_voidp wrkmem );
#endif

#ifdef __cplusplus
}
#endif

#endif
