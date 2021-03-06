.if !target(__<gcc80.Makefile.inc>__)
__<gcc80.Makefile.inc>__:

.include "../../usr.bin/cc80/Makefile.version"

GCCDIR=		${.CURDIR}${RELUP}/../../../../contrib/gcc-8.0
OCCDIR=		${.OBJDIR}${RELUP}/../../../usr.bin/cc${GCCSHORTVER}
CCDIR=		${.CURDIR}${RELUP}/../../../usr.bin/cc${GCCSHORTVER}

CCVER=gcc${GCCSHORTVER}

TRAMPSIZE=	24

TARGETDIR=		${LIBDIR}/gcc${GCCSHORTVER}
TARGET_LIBDIR=		${TARGETDIR}
TARGET_DEBUGLIBDIR=	${TARGETDIR}/debug
TARGET_PROFLIBDIR=	${TARGETDIR}/profile
TARGET_SHLIBDIR=	${TARGETDIR}

LIBSUPCPP=		${.OBJDIR}/../libsupc++/libsupc++.a

CFLAGS+=	-I${CCDIR}/cc_prep
CFLAGS+=	-I${CCDIR}/cc_prep/config
CFLAGS+=	-I${OCCDIR}/cc_prep
CFLAGS+=	-I${OCCDIR}/cc_tools/tools
CFLAGS+=	-I${GCCDIR}/include
CFLAGS+=	-I${GCCDIR}/gcc
CFLAGS+=	-I${GCCDIR}/gcc/config
CFLAGS+=	-I${GCCDIR}/gcc/config/i386
CFLAGS+=	-I${CCDIR}/../gmp

.endif
