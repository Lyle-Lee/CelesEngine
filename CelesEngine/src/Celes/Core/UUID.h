#pragma once

namespace Celes {

	class UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID&) = default;

		operator uint64_t() const { return m_UUID; }
	private:
		uint64_t m_UUID;
	};

}

namespace std {

	// The generated uint64_t is uniformly distributed, so it can simply be used as a hash key.
	template<>
	struct hash<Celes::UUID>
	{
		size_t operator()(const Celes::UUID& uuid) const
		{
			return (uint64_t)uuid;
		}
	};

}
