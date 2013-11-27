void transmission()
{
  transmissionString.begin();
  transmissionString += "\r\nGPS Data: \r\n";
  transmissionString += "Basic Data: \r\n";
  transmissionString += "Fix: ";
  transmissionString += gpsFix;
  transmissionString += " Time: ";
  transmissionString += timeHHMMSS;
  transmissionString += " Millis from start: ";
  transmissionString += millis();
  transmissionString += " Longitude: ";
  transmissionString += longitude;
  transmissionString += " Lattitude: ";
  transmissionString += lattitude;
  transmissionString += " Altitude: ";
  transmissionString += alt;
  transmissionString += " Baro alt: ";
  transmissionString += imu.ZEst;
  transmissionString += "\r\nCourse and speed: \r\n";
  transmissionString += "True Heading: ";
  transmissionString += trueHeading;
  transmissionString += " Magnetic Heading: ";
  transmissionString += magneticHeading;
  transmissionString += " Speed - Knots: ";
  transmissionString += speedKnots;
  transmissionString += " Speed - Km/h: ";
  transmissionString += speedKMH;
  transmissionString += " Weather Data: \r\n";
  transmissionString += "Barometric - inches: ";
  transmissionString += barometricPressureInches;
  transmissionString += " Barometric - bar: ";
  transmissionString += barometricPressureBars;
  transmissionString += " Air temp(c): ";
  transmissionString += airTemp;
  transmissionString += " Relative Humidity: ";
  transmissionString += _relativeHumidity;
  transmissionString += " Dew Point: ";
  transmissionString += dewPoint;
  transmissionString += "\r\nWind Data: ";
  transmissionString +=  "True Wind Dir: ";
  transmissionString += windDirectionTrue;
  transmissionString += " Magnetic Wind Dir: ";
  transmissionString += windDirectionMagnetic;
  transmissionString += " Wind Speed - Knots: ";
  transmissionString += windSpeedKnots;
  transmissionString += " Wind Speed - m/s: ";
  transmissionString += windSpeedMetersPerSecond;
  transmissionString += " Relative Wind Chill: ";
  transmissionString += relativeWindChill;
  transmissionString += " Theoretical Wind Chill: ";
  transmissionString += theoreticalWindChill;
  transmissionString += "\r\nWeather Station Generated AHRS data: \r\n";
  transmissionString += "Pitch: ";
  transmissionString += weatherPitch;
  transmissionString += " Roll: ";
  transmissionString += weatherRoll;
  transmissionString += " Yaw: ";
  transmissionString += weatherYaw;
  transmissionString += "\r\nIMU Generated AHRS data: \r\n";
  transmissionString += "Pitch: ";
  transmissionString += imu.pitch;
  transmissionString += " Roll: ";
  transmissionString += imu.roll;
  transmissionString += " Yaw: ";  
  transmissionString += imu.yaw;
  transmissionString += " Airspeed: ";  
  transmissionString += airSpeedMperS;
  transmissionString += " Humidity from sen: ";  
  transmissionString += relativeHumidity;
   transmissionString += " ~ ";
   
  Serial.print("~");
  Serial.println();
  Serial.println(transmissionString);
  if(SDCardPresent == true)
  {
    File dataFile = SD.open(filename, FILE_WRITE);
    if (dataFile) 
    {
      dataFile.println(transmissionString);
      dataFile.close();
    }
  }
}
