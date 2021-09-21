#ifndef B_ROOMINFO_H_INCLUDED
#define B_ROOMINFO_H_INCLUDED

#include <PGE/String/String.h>

#include "RoomInstance.h"
#include "../../Utilities/Directories.h"

class IRoomInfo {
	public:
		virtual ~IRoomInfo() = default;

		virtual RoomInstance* instantiate(CollisionMeshCollection& cmc, const Room::RenderInfo& info, const PGE::Vector3f& pos) const = 0;

		virtual void load(Resources& res) = 0;
		virtual void unload() = 0;

		virtual int getWeight() const = 0;
		virtual Room& getRoom() const = 0;

	protected:
		IRoomInfo() = default;
};

template <typename RI = RoomInstance>
class RoomInfo : public IRoomInfo {
	static_assert(std::is_base_of<RoomInstance, RI>::value);

	public:
		RoomInfo(const PGE::String& name, int weight) : name(name), weight(weight) { }

		~RoomInfo() {
			delete room;
		}

		RoomInstance* instantiate(CollisionMeshCollection& cmc, const Room::RenderInfo& info, const PGE::Vector3f& pos) const override {
			PGE_ASSERT(room != nullptr, "Room was not loaded!");
			return new RI(*room, cmc, info, pos);
		}

		void load(Resources& res) override {
			PGE_ASSERT(room == nullptr, "Room was already loaded!");
			room = new Room(res, Directories::ROOMS + name);
		}

		void unload() override {
			delete room;
			room = nullptr;
		}

		int getWeight() const override {
			return weight;
		}

		Room& getRoom() const override {
			PGE_ASSERT(room != nullptr, "Room was not loaded");
			return *room;
		}

	private:
		const PGE::String name;
		const int weight;

		Room* room = nullptr;
};

#endif // B_ROOMINFO_H_INCLUDED
