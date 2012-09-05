#include "fribidi.h"

#define	mk_wcwidth	fribidi_wcwidth
#define	mk_wcswidth	fribidi_wcswidth
#define	mk_wcwidth_cjk	fribidi_wcwidth_cjk
#define	mk_wcswidth_cjk	fribidi_wcswidth_cjk

#define wchar_t		FriBidiChar
#define size_t		FriBidiStrIndex
