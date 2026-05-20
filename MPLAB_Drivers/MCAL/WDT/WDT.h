#ifndef WDT_H
#define WDT_H

/* Initializes WDT (if software controlled, though usually hardware config) */
void WDT_Init(void);

/* Pet the dog - call this in your main while(1) loop */
void WDT_Refresh(void);

#endif /* WDT_H */