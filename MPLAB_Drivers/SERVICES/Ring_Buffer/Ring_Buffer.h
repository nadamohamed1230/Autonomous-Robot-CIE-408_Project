#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include "../STD_TYPES.h"

#define BUFFER_SIZE 16
void RingBuffer_Init(void);
void RingBuffer_Write(u8 data);
u8 RingBuffer_Read(u8 *data);
u8 RingBuffer_IsAvailable(void);

#endif /* RING_BUFFER_H */
