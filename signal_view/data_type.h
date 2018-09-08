#ifndef DATA_TYPE_H
#define DATA_TYPE_H

// 数据类型.
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

// 数据类型的通道数量.
int DataTypeChannel(DataType datatype);

// 数据类型的大小（字节数）.
int DataTypeSize(DataType datatype);

// 将数据转换为Real64类型.
int DataTypeToReal64(void * input, unsigned int count, DataType datatype, double * output);


#endif //DATA_TYPE_H
