#ifndef OESDK_EVENT_H
#define OESDK_EVENT_H
#include <stdbool.h>
#include <stdint.h>
typedef struct OesdkEvent { uint64_t Id; bool Signalled; } OesdkEvent;
void OesdkEventInitialize(OesdkEvent *Event,uint64_t Id); void OesdkEventSignal(OesdkEvent *Event); bool OesdkEventConsume(OesdkEvent *Event);
#endif
