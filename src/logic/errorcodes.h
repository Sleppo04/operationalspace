#ifndef ERRORCODES_H
#define ERRORCODES_H

// Standard codes

#ifndef EBADFD
// File descriptor in bad state
#define EBADFD 77
#endif

#ifndef EINVAL
// Error Invalid Value (or parameter)
#define EINVAL 22
#endif

#ifndef EDESTADDRREQ
// Destination address required
#define EDESTADDRREQ 89
#endif

#ifndef ENOMEM
// Not enough memory
#define ENOMEM 12
#endif

// Custom codes

#ifndef ENOTFOUND
// Object requested could not be found
#define ENOTFOUND 129
#endif

#endif