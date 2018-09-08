#include "data_type.h"

#include <algorithm>


int DataTypeChannel(DataType datatype)
{
	switch (datatype) {
	case Int8:
	case Int16:
	case Int32:
	case Uint8:
	case Uint16:
	case Uint32:
	case Real32:
	case Real64:
		return 1;
	case Int8_2:
	case Int16_2:
	case Int32_2:
	case Uint8_2:
	case Uint16_2:
	case Uint32_2:
	case Real32_2:
	case Real64_2:
		return 2;
	default:
		return 0;
	}
}

int DataTypeSize(DataType datatype)
{
	switch (datatype) {
	case Int8: 
	case Uint8:
		return sizeof(char);
	case Int16:
	case Uint16:
		return sizeof(short);
	case Int32:
	case Uint32:
		return sizeof(int);
	case Real32:
		return sizeof(float);
	case Real64:
		return sizeof(double);
	case Int8_2:
	case Uint8_2:
		return sizeof(char) * 2;
	case Int16_2:
	case Uint16_2:
		return sizeof(short) * 2;
	case Int32_2:
	case Uint32_2:
		return sizeof(int) * 2;
	case Real32_2:
		return sizeof(float) * 2;
	case Real64_2:
		return sizeof(double) * 2;
	default:
		return 0;
	}
}


int DataTypeToReal64(void * input, unsigned int count, DataType datatype, double * output)
{
	int count2 = count * DataTypeChannel(datatype);

	switch (datatype) {
	case DataType::Int8:
	case DataType::Int8_2:
		std::copy((char *)input, (char *)input + count2, output);
		break;

	case DataType::Uint8:
	case DataType::Uint8_2:
		std::copy((unsigned char *)input, (unsigned char *)input + count2, output);
		break;

	case DataType::Int16:
	case DataType::Int16_2:
		std::copy((short *)input, (short *)input + count2, output);
		break;

	case DataType::Uint16:
	case DataType::Uint16_2:
		std::copy((unsigned short *)input, (unsigned short *)input + count2, output);
		break;

	case DataType::Int32:
	case DataType::Int32_2:
		std::copy((int *)input, (int *)input + count2, output);
		break;

	case DataType::Uint32:
	case DataType::Uint32_2:
		std::copy((unsigned int *)input, (unsigned int *)input + count2, output);
		break;

	case DataType::Real32:
	case DataType::Real32_2:
		std::copy((float *)input, (float *)input + count2, output);
		break;

	case DataType::Real64:
	case DataType::Real64_2:
		std::copy((double *)input, (double *)input + count2, output);
		break;

	default:
		return -1;
	}

	return count2;
}