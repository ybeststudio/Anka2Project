#pragma once

#ifdef ENABLE_ITEMSHOP
class TEMP_BUFFER
{
	public:
		TEMP_BUFFER(int Size = 8192, bool ForceDelete = false);
		~TEMP_BUFFER();

		const void* read_peek();
		void write(const void* data, int size);
		int size();
		void reset();
		LPBUFFER getptr() { return buf; }

	protected:
		LPBUFFER buf;
		bool forceDelete;
};
#endif
