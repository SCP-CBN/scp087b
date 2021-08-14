#ifndef B_CACHE_H_INCLUDED
#define B_CACHE_H_INCLUDED

#include <PGE/String/String.h>
#include <PGE/String/Key.h>

template <typename T>
class Cache {
	public:
		// Fuck this.
		~Cache() noexcept(
#ifdef DEBUG
			false
#else
			true
#endif
		) {
			if (!entries.empty()) {
#ifdef DEBUG
				PGE::String names;
				for (const auto& it : entries) {
					names += it.second.name;
					names += '\n';
				}
				throw PGE_CREATE_EX(names);
#else
				for (const auto& it : entries) {
					delete it.second.element;
				}
#endif
			}
		}

		T* tryGet(const PGE::String& name) {
			auto it = entries.find(name);
			if (it == entries.end()) {
				return nullptr;
			}
			it->second.count++;
			return it->second.element;
		}

		void add(const PGE::String& name, T* element) {
			entries.emplace(name, Entry(name, element));
		}

		void drop(const PGE::String::Key& key) {
			auto it = entries.find(key);
			PGE_ASSERT(it != entries.end(), "Tried dropping invalid element");
			it->second.count--;
			if (it->second.count <= 0) {
				delete it->second.element;
				entries.erase(it->second.name);
			}
		}

	private:
		struct Entry {
			Entry(const PGE::String& name, T* element) : name(name), element(element) { }
			const PGE::String name;
			T* element;
			int count = 1;
		};

		std::unordered_map<PGE::String::Key, Entry> entries;
};

#endif // B_CACHE_H_INCLUDED
