#ifndef INFRA_VK_H
#define INFRA_VK_H
//An abstraction that attempts to formalize resource allocation and management in Vulkan

namespace infr {
	class InfraVK {
	private:
		virtual void setup() = 0;
		virtual void render() = 0;
		virtual void cleanup() = 0;

	public:
		InfraVK();
		virtual void start();
		virtual ~InfraVK();
	};
}
#endif