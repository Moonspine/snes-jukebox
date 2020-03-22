#ifndef UTILS_H
#define UTILS_H

word computeElapsedTime(word &lastTimestamp) {
  word currentTimestamp = (word)(millis() & 0xFFFF);
  
  word result;
  if (currentTimestamp >= lastTimestamp) {
    result = currentTimestamp - lastTimestamp;
  } else {
    result = 0xFFFF - lastTimestamp + currentTimestamp;
  }
  
  lastTimestamp = currentTimestamp;
  
  return result;
}

#endif
