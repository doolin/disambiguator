#ifndef PATENT_MACROS_H
#define PATENT_MACROS_H

// See https://github.com/ndim/freemcan/blob/f6348add31479b17ad7c7eec0acaf1f100843e84/include/compiler.h#L44
/** Rename Unused Parameter */
#define _UP(x) unused_p__ ## x

/** Mark Unused Parameter */
#define UP(x) _UP(x) __attribute__((unused))

/** Rename Unused Variable */
#define _UV(x) unused_v__ ## x

/** Mark Unused Variable */
#define UV(x) _UV(x) __attribute__((unused))


#endif /* PATENT_MACROS_H */
