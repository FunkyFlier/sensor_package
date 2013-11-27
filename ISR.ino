void _200HzISRConfig(){
  TCCR5A = (1<<COM5A1);
  TCCR5B = (1<<CS51)|(1<<WGM52);
  TIMSK5 = (1<<OCIE5A);
  OCR5A = 10000;
}

ISR(TIMER5_COMPA_vect, ISR_NOBLOCK){
  ReadSerialStreams();
}

void ReadSerialStreams(){
  getString();

  if (handShake == true){
    while(radio.available() > 0){
      newRadio = true;
      inByteRadio = radio.read();
      inBuffer[bufferIndexRadio] = inByteRadio;
      bufferIndexRadio++;
      if (bufferIndexRadio == RADIO_BUF_SIZE){
        bufferIndexRadio = 0;
      }
    }
  }
}













