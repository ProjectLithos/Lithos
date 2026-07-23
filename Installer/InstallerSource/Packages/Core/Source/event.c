#include <oesdk/event.h>
void OesdkEventInitialize(OesdkEvent*E,uint64_t I){if(E){E->Id=I;E->Signalled=false;}}void OesdkEventSignal(OesdkEvent*E){if(E)E->Signalled=true;}bool OesdkEventConsume(OesdkEvent*E){bool S;if(E==0)return false;S=E->Signalled;E->Signalled=false;return S;}
