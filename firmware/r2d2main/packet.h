
char statusBuf[5];
char cmdBuf[5];

void prepareStatusBuf(int tofdistance, int radar, int knock, int mic)
{
  int idx=0;

  memcpy(statusBuf+idx,tofdistance,sizeof(int));idx+=2;
  memcpy(statusBuf+idx,radar,sizeof(unsigned char));idx+=1;
  memcpy(statusBuf+idx,knock,sizeof(unsigned char));idx+=1; 
  memcpy(statusBuf+idx,mic,sizeof(unsigned char));idx+=1;
  
}

void parseCmdBuf(int cmdR, int cmdL, int led, int dome)
{
  cmdBuf[0] = cmdR;
  cmdBuf[1] = cmdL;
  cmdBuf[2] = led;
  cmdBuf[3] = dome; 
}
