void getString()
{
  int i;
  char weatherInChar,IMUInChar;
  
  while(Serial3.available())
  {
    IMUInChar=Serial3.read();
    switch (IMUInChar){
    case '$':
      IMUInBuffer.begin();
      //IMUIndex=0;
      IMUInBuffer += IMUInChar;
      break;
    case '*':
      //IMUIndex++;
      IMUInBuffer += IMUInChar;
      IMUParseReady = true;  
      memcpy (IMUParseBuf,IMUInBuf,40);
      break;
    default:
      //IMUIndex++;
      IMUInBuffer += IMUInChar;
      break;
    }
  }
  
  while(Serial1.available())
  {
    weatherInChar=Serial1.read();
    switch (weatherInChar)
    {
    case '$':
      weatherInBuffer.begin();
      weatherInBuffer += weatherInChar;
      break;
    case '*':
      weatherInBuffer += weatherInChar;
      weatherParseReady = true;
      for ( i = 0;i<2;i++)
      {
        delay(1);
        weatherInChar=Serial1.read();
        weatherInBuffer += weatherInChar;
      }
      memcpy (weatherParseBuf,weatherInBuf,90);
     // Serial.println(weatherInBuffer);
      break;
    default:
      weatherInBuffer += weatherInChar;
      break;
    }
  }
}
