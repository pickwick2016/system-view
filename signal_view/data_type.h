#ifndef DATA_TYPE_H
#define DATA_TYPE_H

// ��������.
enum DataType
{
	Unknown = -1,
	Int8,
	Uint8,
	Int16,
	Uint16,
	Int32,
	Uint32,
	Real32,
	Real64,
	Int8_2,
	Uint8_2,
	Int16_2,
	Uint16_2,
	Int32_2,
	Uint32_2,
	Real32_2,
	Real64_2,
};

// �������͵�ͨ������.
int DataTypeChannel(DataType datatype);

// �������͵Ĵ�С���ֽ�����.
int DataTypeSize(DataType datatype);

// ������ת��ΪReal64����.
int DataTypeToReal64(void * input, unsigned int count, DataType datatype, double * output);


#endif //DATA_TYPE_H
