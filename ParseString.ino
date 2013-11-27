void parseString()
{
  if (strncmp(parseptr, "$GPGGA",6) == 0)
    parseGPGGA();
  if (strncmp(parseptr, "$WIMDA",6) == 0 )
    parseWIMDA();
   if (strncmp(parseptr, "$YXXDR",6) == 0 )
    parseXDR();
    if (strncmp(parseptr, "$YPR",4) == 0 )
    parseYPR();
    if (strncmp(parseptr, "$HCHDG",6) == 0)
    parseHCHDG();
}

void parseYPR()
{
  parseptr = parseptr+5;
  for (int i = 1; i<10;i++){
    if (parseptr[0] != ',') {
      switch (i){
      case 1:
        yaw = atof(parseptr);
        break;
      case 2:
        pitch = atof(parseptr);
        break;
      case 3:
        roll = atof(parseptr);
        break;
      default:
        break;
      }
      parseptr = strchr(parseptr, ',') + 1; 
    }
    else
      parseptr = parseptr +1;
  }
}

void parseGPGGA()
{
  parseptr = parseptr+7;
  float inFloat;
  for (int i = 1; i<10;i++){
    if (parseptr[0] != ',') {
      switch (i){
      case 1:
        inFloat= atof(parseptr);
        timeHHMMSS = (uint32_t)inFloat;
        break;
      case 2:
        lattitude = atof(parseptr);
        break;
      case 4:
        longitude =  atof(parseptr);
      case 6:
        fixQuality = atoi(parseptr);
        if(fixQuality == 0)
          gpsFix = false;
        else
          gpsFix = true;
      case 9:
        alt = atof(parseptr);
      default:
        break;
      }
      parseptr = strchr(parseptr, ',') + 1; 
    }
    else
      parseptr = parseptr +1;
  }
}

void parseWIMDA()
{
  parseptr = parseptr+7;
  for (int i = 1; i<20;i++){
    if (parseptr[0] != ',') {
      switch (i){
      case 1:
        barometricPressureInches = atof(parseptr);
        break;
      case 3:
        barometricPressureBars= atof(parseptr);
        break;  
      case 5: 
        airTemp = atof(parseptr);
        break;
      case 9:
        _relativeHumidity = atof(parseptr);
        break;
      case 11:
        dewPoint = atof(parseptr);
        break;
      case 13:
        windDirectionTrue = atof(parseptr);
        break;
      case 15:
        windDirectionMagnetic = atof(parseptr);
        break;
      case 17:
        windSpeedKnots = atof(parseptr);
        break;
      case 19:
        windSpeedMetersPerSecond = atof(parseptr);
        break;
      default:
        break;
      } 
      parseptr = strchr(parseptr, ',') + 1; 
    }
    else
      parseptr = parseptr +1;
  }
}  

void parseHCHDG(){
  parseptr = parseptr+7;
  for (int i = 1; i<8;i++){
    if (parseptr[0] != ',') {
      switch (i){
      case 1:
        trueHeading = atof(parseptr);
        break;
      case 3:
        magneticHeading = atof(parseptr);
        break;
      case 5:
        speedKnots = atof(parseptr);
        break;
      case 7:
        speedKMH = atof(parseptr);
        break;
      default:
        break;
      } 
      parseptr = strchr(parseptr, ',') + 1; 
    }
    else {
      parseptr = parseptr +1;
    } 
  }
}

void parseXDR()
{
  parseptr = parseptr+7;
  for (int i = 1; i<16;i++){
    if (parseptr[0] != ',') {
      switch (i){
      case 2:
        weatherPitch = atof(parseptr);
        break;
      case 6:
        weatherRoll= atof(parseptr);
        break;  
      default:
        break;
      } 
      parseptr = strchr(parseptr, ',') + 1; 
    }
    else
      parseptr = parseptr +1;
  }  
  
}
