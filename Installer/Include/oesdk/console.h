#ifndef OESDK_CONSOLE_H
#define OESDK_CONSOLE_H

void kclear(void);
void kputc(char character);
void kputs(const char *text);
int kprintf(const char *format, ...);

#endif
