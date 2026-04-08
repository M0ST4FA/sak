#pragma once

void timer_init(volatile unsigned int *timer, int mode, int seconds);
void wait_seconds(int seconds);
