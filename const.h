#ifndef __XENON_MINES_CONST_H__
#define __XENON_MINES_CONST_H__

#if defined(_KMINES_MODE)

#define EASY_WIDTH 9
#define EASY_HEIGHT 9
#define EASY_MINES 10

#define MIDDLE_WIDTH 16
#define MIDDLE_HEIGHT 16
#define MIDDLE_MINES 40

#define HARD_WIDTH 30
#define HARD_HEIGHT 16
#define HARD_MINES 99

#else

#define EASY_WIDTH 10
#define EASY_HEIGHT 10
#define EASY_MINES 10

#define MIDDLE_WIDTH 20
#define MIDDLE_HEIGHT 20
#define MIDDLE_MINES 40

#define HARD_WIDTH 40
#define HARD_HEIGHT 40
#define HARD_MINES 160

#endif

#endif // __XENON_MINES_CONST_H__
