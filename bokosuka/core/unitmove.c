/*
 * CLBokosuka Unit move sub.
 * (C) 2013- K.Ohta <whatisthis.sowhat@gmail.com>
 * History:
 *    Jan 16 2013 Initial
 */

#include "unitdef.h"

BOOL CB_isUnitDone(CB_Units *p, int offset) 
{
   if((offset > 32767) || (offset < 0)) return TRUE;
   if(p->units[offset].done != FALSE) return TRUE;
   return FALSE;
}

int16_t CB_SearchUnit(CB_Units *p, int num)
{
   CB_UnitParams *q;
   int16_t i16;
   
   if(p == NULL) return BC_ErrorParams;
   if((num > 32767) || (num < 0)) return BC_ErrorParams;
   if((p->maxunits> 32767) || (p->maxunits < 0)) return BC_ErrorParams;
   if(num >= p->maxunits) return BC_NotFound;
   q = p->units;
   for(i16 = 0; i16 < p->maxunits; i16++){
      if(q->number == num) return i16;
      q++; 
   }
   return BC_NotFound;
}

int16_t CB_SearchUnit_XY(CB_Units *p, int x, int y)
{
   CB_UnitParams *q;
   int16_t i16;
   
   if(p == NULL) return -1;
   
   if((p->maxunits > 32767) || (p->maxunits < 0)) return BC_ErrorParams;
   if(x < -32766) x = -32766;
   if(y < -32766) y = -32766;
   if(x > 32766) x = 32766;
   if(y > 32766) y = 32766;

   q = p->units;
   for(i16 = 0; i16 < p->maxunits; i16++){
      if((q->x == x) && (q->y == y)) return q->number;
      q++; 
   }
   return BC_NotFound;
}


int CB_isMoveUnit(CB_Units *p, int offset, int xaxis, int yaxis, CB_MapData *q)
{
   int x;
   int y;
   int xdst;
   int ydst;
   int16_t query;
   int8_t type, attr;

   if(p == NULL) return FALSE;
   if((offset >= p->maxunits) || (offset < 0) || (p->maxunits <= 0)) return MOVE_FALSE;
   
   x =(int) p->units[offset].x;
   y =(int) p->units[offset].y;
   if(xaxis > 1)  xaxis = 1;
   if(xaxis < -1) xaxis = -1;
   if(yaxis > 1)  yaxis = 1;
   if(yaxis < -1) yaxis = -1;
   
   xdst = x + xaxis;
   ydst = y + yaxis;
   /* Pass1 Check Map */
   if((q->w <= xdst) || (0 > xdst)) {
      p->units[offset].done = TRUE; /* X Overflow, force done. */
      return MOVE_FALSE;
   }
   if((q->h <= ydst) || (0 > ydst)) {
      p->units[offset].done = TRUE; /* Y Overflow, force done. */
      return MOVE_FALSE;
   }
   /* Get map data, will be abstracted */
   type = q->mapdata[xdst + ydst * q->w].type;
   attr = q->mapdata[xdst + ydst * q->w].attribute;
   
   switch(type) {
    case MAP_TYPE_WALL: // WALL; Unable to move
      p->units[offset].done = TRUE;
      return MOVE_FALSE;
      break;
    case MAP_TYPE_BLANK:
    default:
      break;
   }
   
   /* Pass 2 Check friends */
   query = CB_SearchUnit_XY(p, xdst, ydst);
   if(query >= 0) { /* Unit Found */
      if(p->units[query].done == TRUE) {
	 p->units[offset].done = TRUE;
	 return MOVE_FALSE; // Unable to move
      }
      return MOVE_TEMP; // Tempirally move.
   }
   return MOVE_TRUE; // Okay, You can Move.
}

