/*
 * CLBokosuka Unit define file.
 * (C) 2013- K.Ohta <whatisthis.sowhat@gmail.com>
 * History:
 *    Jan 16 2013 Initial
 */

#ifndef __CLBOKOSUKA_UNITDEF_H
#define  __CLBOKOSUKA_UNITDEF_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
/*
 * W-I-P: Will move to typedefs.
 */
typedef int8_t BOOL8;
typedef int32_t BOOL;
enum {
   FALSE = 0,
   TRUE = 1
};
   

enum {
   BC_None = 0,
   BC_Soldier,
   BC_Soldier_SP,
   BC_Spearer,
   BC_Spearer_SP,
   BC_General,
   BC_General_SP,
   BC_Leader
};

enum {
   BC_Dead = 0,
   BC_Live
};

#define BC_NotFound -1;
#define BC_ErrorParams -2;
   
   
typedef struct {
   int16_t number;
   BOOL8   done;
   int8_t  status; 
   uint16_t team;
   uint16_t reserve1;
   int16_t x;
   int16_t y;
   uint16_t  type1; /* General,Spearer,etc.. */
   uint16_t  type2; /* Local Type */
   int16_t  life; /* Life */
   uint16_t  level;  
   int32_t  expeliment;
   int32_t  reserve2[2];
} CB_UnitParams __attribute__((__packed__));

typedef struct {
   int maxunits;
   CB_UnitParams *units;
} CB_Units;
   
   
enum {
   MAP_TYPE_BLANK = 0,
   MAP_TYPE_WALL
};
   
enum {
   MOVE_FALSE = 0,
   MOVE_TRUE = 1,
   MOVE_TEMP = -1
};
   
   
typedef struct {
  uint8_t type;
  uint8_t attribute;
} CB_MapParams __attribute__((__packed__));
   
typedef struct {
  int16_t w;
  int16_t h;
  CB_MapParams *mapdata;
} CB_MapData __attribute__((__packed__));
   
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*  __CLBOKOSUKA_UNITDEF_H */