#ifndef __INC_METIN_II_AFFECT_FLAG_H__
#define __INC_METIN_II_AFFECT_FLAG_H__

#ifndef IS_SET
#define IS_SET(flag, bit)                ((flag) & (bit))
#endif

#ifndef SET_BIT
#define SET_BIT(var, bit)                ((var) |= (bit))
#endif

#ifndef REMOVE_BIT
#define REMOVE_BIT(var, bit)             ((var) &= ~(bit))
#endif

#ifndef TOGGLE_BIT
#define TOGGLE_BIT(var, bit)             ((var) = (var) ^ (bit))
#endif

struct TAffectFlag
{
	uint32_t bits[3];

	inline TAffectFlag() { bits[0] = 0; bits[1] = 0; bits[2] = 0; }
	inline TAffectFlag(uint32_t v1, uint32_t v2 = 0, uint32_t v3 = 0) { bits[0] = v1; bits[1] = v2; bits[2] = v3; }

	inline bool IsSet(int32_t flag) const
	{
		if (AFF_BITS_MAX <= flag || 0 >= flag)
			return false;

		return IS_SET(bits[(flag - 1) >> 5], (((uint32_t)1) << ((flag - 1) & 31)));
	}

	inline void Set(int32_t flag)
	{
		if (AFF_BITS_MAX <= flag || 0 >= flag)
			return;

		SET_BIT(bits[(flag - 1) >> 5], (((uint32_t)1) << ((flag - 1) & 31)));
	}

	inline void Reset(int32_t flag)
	{
		if (AFF_BITS_MAX <= flag || 0 >= flag)
			return;

		REMOVE_BIT(bits[(flag - 1) >> 5], (((uint32_t)1) << ((flag - 1) & 31)));
	}

	inline TAffectFlag& operator = (const TAffectFlag& rhs)
	{
		bits[0] = rhs.bits[0];
		bits[1] = rhs.bits[1];
		bits[2] = rhs.bits[2];
		return *this;
	}
};

inline bool operator == (const TAffectFlag& lhs, const TAffectFlag& rhs)
{
	return lhs.bits[0] == rhs.bits[0] && lhs.bits[1] == rhs.bits[1] && lhs.bits[2] == rhs.bits[2];
}

inline bool operator != (const TAffectFlag& lhs, const TAffectFlag& rhs)
{
	return !(lhs == rhs);
}

#endif
