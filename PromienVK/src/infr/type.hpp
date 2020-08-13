#ifndef TYPE_H
#define TYPE_H

namespace infr {
	enum class DeviceFunction {
		all,
		compute,
		graphic
	};

	//compute = async compute
	enum class QueueFunction {
		all,
		compute,
		graphic,
		transfer,
		present
	};
}


#endif