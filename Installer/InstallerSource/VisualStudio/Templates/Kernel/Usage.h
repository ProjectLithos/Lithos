#ifndef TEMPLATE_USAGE_H
#define TEMPLATE_USAGE_H

#include <oesdk/kernel.h>

void TemplateUsageBoot(const OesdkBootContext *BootContext);
void TemplateUsageCpu(void);
void TemplateUsageMemory(const OesdkBootContext *BootContext);
void TemplateUsageInterruptsAndTimer(void);
void TemplateUsageGraphics(void);
void TemplateUsageObjects(void);

#endif
