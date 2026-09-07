#include <cstdint>
#include <cstring>
#include <print>

#include <nwlib/mem/arena.h>
#include <nwlib/utils/uuid.h>

struct SmallObject
{
	uint8_t Data[0x010] {};
};

struct MediumObject
{
	uint8_t Data[0x080] {};
};

struct BigObject
{
	uint8_t Data[0x400] {};
};

struct TestObject
{
	uint64_t A {};
	uint64_t B {};
	uint64_t C {};

	TestObject(uint64_t a, uint64_t b, uint64_t c)
		: A(a), B(b), C(c) {}
};

void Test_MemoryArena()
{
	namespace mem = nw::mem;

	std::println("Test_MemoryArena");
	
	{ // Basic allocation
		std::println("  Allocate a single object");

		mem::arena arena {};

		mem::arena_box<SmallObject> box(arena);

		for (uint8_t& byte : box->Data)
			byte = 0xAA;

		for (uint8_t byte : box->Data)
		{
			if (byte != 0xAA)
				std::abort();
		}
	}
	
	{ // Multiple allocations + corruption detection
		std::println("  Allocate 10 separate small objects");

		mem::arena arena {};

		mem::arena_box<SmallObject> boxes[] {
			mem::arena_box<SmallObject>(arena),
			mem::arena_box<SmallObject>(arena),
			mem::arena_box<SmallObject>(arena),
			mem::arena_box<SmallObject>(arena),
			mem::arena_box<SmallObject>(arena),
			mem::arena_box<SmallObject>(arena),
			mem::arena_box<SmallObject>(arena),
			mem::arena_box<SmallObject>(arena),
			mem::arena_box<SmallObject>(arena),
			mem::arena_box<SmallObject>(arena)
		};

		for (size_t i = 0; i < 10; ++i)
		{
			for (uint8_t& byte : boxes[i]->Data)
				byte = static_cast<uint8_t>(i);
		}

		// Make sure writing to one allocation didn't fuck with another.
		for (size_t i = 0; i < 10; ++i) {
			for (uint8_t byte : boxes[i]->Data)
			{
				if (byte != static_cast<uint8_t>(i))
					std::abort();
			}
		}
	}
	
	{ // Mixed allocation sizes
		std::println("  Allocate mixed-size objects");

		mem::arena arena {};

		mem::arena_box<SmallObject>  small1(arena);
		mem::arena_box<BigObject>    big1(arena);
		mem::arena_box<MediumObject> medium1(arena);
		mem::arena_box<SmallObject>  small2(arena);
		mem::arena_box<BigObject>    big2(arena);
		mem::arena_box<MediumObject> medium2(arena);

		std::memset(small1->Data, 0x11, sizeof(small1->Data));
		std::memset(big1->Data, 0x22, sizeof(big1->Data));
		std::memset(medium1->Data, 0x33, sizeof(medium1->Data));
		std::memset(small2->Data, 0x44, sizeof(small2->Data));
		std::memset(big2->Data, 0x55, sizeof(big2->Data));
		std::memset(medium2->Data, 0x66, sizeof(medium2->Data));

		for (uint8_t byte : small1->Data)
			if (byte != 0x11) std::abort();

		for (uint8_t byte : big1->Data)
			if (byte != 0x22) std::abort();

		for (uint8_t byte : medium1->Data)
			if (byte != 0x33) std::abort();

		for (uint8_t byte : small2->Data)
			if (byte != 0x44) std::abort();

		for (uint8_t byte : big2->Data)
			if (byte != 0x55) std::abort();

		for (uint8_t byte : medium2->Data)
			if (byte != 0x66) std::abort();
	}
	
	{ // Constructor arguments
		std::println("  Test constructor arguments");

		mem::arena arena {};

		mem::arena_box<TestObject> box(arena, 123, 456, 789);

		if (box->A != 123 || box->B != 456 || box->C != 789)
			std::abort();
	}

	{ // Heavy allocation test
		std::println("  Allocate 10,000 small objects");

		mem::arena arena {};

		mem::arena_box<SmallObject> boxes[10'000];

		for (auto& box : boxes)
			box = mem::arena_box<SmallObject>(arena);

		for (size_t i = 0; i < 10'000; ++i)
		{
			for (uint8_t& byte : boxes[i]->Data)
				byte = static_cast<uint8_t>(i);
		}

		for (size_t i = 0; i < 10'000; ++i)
		{
			for (uint8_t byte : boxes[i]->Data)
			{
				if (byte != static_cast<uint8_t>(i))
					std::abort();
			}
		}
	}

	{ // Alignment test
		std::println("  Test alignment");

		mem::arena arena {};

		mem::arena_box<uint8_t>  a(arena);
		mem::arena_box<uint32_t> b(arena);
		mem::arena_box<uint64_t> c(arena);
		mem::arena_box<double>   d(arena);

		if (reinterpret_cast<uintptr_t>(b.get()) % alignof(uint32_t) != 0)
			std::abort();

		if (reinterpret_cast<uintptr_t>(c.get()) % alignof(uint64_t) != 0)
			std::abort();

		if (reinterpret_cast<uintptr_t>(d.get()) % alignof(double) != 0)
			std::abort();
	}

	std::println("  All arena tests passed.");
}

int main()
{
	Test_MemoryArena();
}
