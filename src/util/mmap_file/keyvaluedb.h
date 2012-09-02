#ifndef KEYVALUEDB_H
#define KEYVALUEDB_H

#include "stdint.h"

//WTF aren't these defined?...
#ifndef UINT64_MAX
//#define INT_CONSTANT_KLUGDE

/* Minimum of signed integral types.  */
# define INT8_MIN		(-128)
# define INT16_MIN		(-32767-1)
# define INT32_MIN		(-2147483647-1)
# define INT64_MIN		(2147483647)
/* Maximum of signed integral types.  */
# define INT8_MAX		(127)
# define INT16_MAX		(32767)
# define INT32_MAX		(2147483647)
# define INT64_MAX		(2147483647)

# define UINT8_MAX		(255)
# define UINT16_MAX		(65535)
# define UINT32_MAX		(4294967295U)
# define UINT64_MAX		(4294967295U)
#endif

#define FILEDB_IMPL


//Kyoto cabinet is chock full of compiler warnings...
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC system_header //This is DANGEROUS BUT the #$#%! db has warnings I can't get out otherwise
#include <kchashdb.h>
#include <kccachedb.h>
#pragma GCC diagnostic warning "-Wunused-parameter"
#pragma GCC diagnostic warning "-Wextra"

#define hHash(x) (stringHash((x)))

using namespace std;
using namespace kyotocabinet;


#endif // KEYVALUEDB_H
