/*
Copyright (c) 2018 Stanislav Brtna

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "sda_time.h"

uint8_t sdaTimeGetSeconds(int32_t sdaTime) {
  return (uint8_t)(sdaTime % 60);
}

uint8_t sdaTimeGetMinutes(int32_t sdaTime) {
  return (uint8_t)((sdaTime / 60) % 60);
}

uint8_t sdaTimeGetHours(int32_t sdaTime) {
  return (uint8_t)((sdaTime / (60 * 60)) % 24);
}

uint8_t sdaTimeGetWeekDay(int32_t sdaTime) {
  return (uint8_t)((sdaTime / (60 * 60 * 24)) % 7) + 1;
}

uint8_t sdaTimeGetDays(int32_t sdaTime) {
  return (uint8_t)sdaTimeGetMonthDayYear(sdaTime, 0);
}

uint8_t sdaTimeGetMonths(int32_t sdaTime) {
  return (uint8_t)sdaTimeGetMonthDayYear(sdaTime, 1);
}

uint16_t sdaTimeGetYears(int32_t sdaTime) {
  return sdaTimeGetMonthDayYear(sdaTime, 2);
}

void sdaTimePrint(int32_t sdaTime) {
  printf("%0u:%0u:%0u %u. %u. %u",
    sdaTimeGetHours(sdaTime),
    sdaTimeGetMinutes(sdaTime),
    sdaTimeGetSeconds(sdaTime),
    sdaTimeGetDays(sdaTime),
    sdaTimeGetMonths(sdaTime),
    sdaTimeGetYears(sdaTime)
  );
}

int32_t sdaTimeGetTimestamp(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec) {

  int32_t sdaTime = 0;
  uint16_t x = 0;

  if (year < 2007) {
    printf("%s: Error: Timestamp only works for years 2007 and above!\n", __FUNCTION__);
    return 0;
  }

  sdaTime += (int32_t)sec;
  sdaTime += (int32_t)min * 60;
  sdaTime += (int32_t)hour * 60 * 60;
  sdaTime += (int32_t)(day - 1) * 60 * 60 * 24;

  do {
    if (month == 1) {
      break;
    }

    sdaTime += 24 * 60 * 60 * 31;

    if (month == 2) {
      break;
    }

    if ((year - 2007 + 3) % 4 != 0) {
      sdaTime += 24 * 60 * 60 * 28;
    } else {
      sdaTime += 24 * 60 * 60 * 29;
    }

    if (month == 3) {
      break;
    }

    sdaTime += 24 * 60 * 60 * 31;

    if (month == 4) {
      break;
    }

    sdaTime += 24 * 60 * 60 * 30;

    if (month == 5) {
      break;
    }

    sdaTime += 24 * 60 * 60 * 31;

    if (month == 6) {
      break;
    }

    sdaTime += 24 * 60 * 60 * 30;

    if (month == 7) {
      break;
    }

    sdaTime += 24 * 60 * 60 * 31;

    if (month == 8) {
      break;
    }

    sdaTime += 24 * 60 * 60 * 31;

    if (month == 9) {
      break;
    }

    sdaTime += 24 * 60 * 60 * 30;

    if (month == 10) {
      break;
    }

    sdaTime += 24 * 60 * 60 * 31;

    if (month == 11) {
      break;
    }

    sdaTime += 24 * 60 * 60 * 30;

    if (month == 12) {
      break;
    }
  } while(0);

  for(x = 0; x < year - 2007; x++) {
    if ((x + 3) % 4 != 0) {
      sdaTime += 24 * 60 * 60 * 365;
    } else {
      sdaTime += 24 * 60 * 60 * 366;
    }
  }

  return sdaTime;
}

uint16_t sdaTimeGetMonthDayYear(int32_t sdaTime, uint8_t select) {
  uint16_t day = 0;
  uint16_t year = 0;
  uint16_t month = 0;

  day = (uint16_t)(sdaTime / (60 * 60 * 24));

  while(day > 365) {
    if ((year + 3) % 4 != 0) {
      day -= 365;
    } else {
      day -= 366;
    }
    year++;
  }

  do {
    if (day <= 30) {
      month = 1;
      break;
    }
    day -= 31;

    if ((year + 3) % 4 != 0) {
      if(day <= 27) {
        month = 2;
        break;
      }
      day -= 28;
    } else {
      if(day <= 28) {
        month = 2;
        break;
      }
      day -= 29;
    }

    if (day <= 30) {
      month = 3;
      break;
    }
    day -= 31;

    if (day <= 29) {
      month = 4;
      break;
    }
    day -= 30;

    if (day <= 30) {
      month = 5;
      break;
    }
    day -= 31;

    if (day <= 29) {
      month = 6;
      break;
    }
    day -= 30;

    if (day <= 30){
      month = 7;
      break;
    }
    day -= 31;

    if (day <= 30) {
      month = 8;
      break;
    }
    day -= 31;

    if (day <= 29) {
      month = 9;
      break;
    }
    day -= 30;

    if (day <= 30) {
      month=10;
      break;
    }
    day -= 31;

    if (day <= 29) {
      month = 11;
      break;
    }
    day -= 30;

    if (day <= 30) {
      month = 12;
    }
  } while(0);

  day += 1;
  year += 2007;

  if (select == 0) {
    return day;
  }

  if (select == 1) {
    return month;
  }

  if (select == 2) {
    return year;
  }

  return 0;
}
