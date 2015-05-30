#ifndef __DATAPACKER__
#define __DATAPACKER__

//负责打包发送数据，也负责处理收到的数据
// ( TMessager 负责的是接收数据的过程，然后交给 DataPacker 处理)

void DataPacker_Pack(void);
void DataPacker_ProcessRecvPack(unsigned char *pack);

#endif
