//#pragma once
//#include <array>
//#include <vector>
//#include <string>
//#include <mutex>
//#include <list>
//#include <functional>
//
//#include <cassert>
//
//#include <d3dx9.h>
//#include <d3d9.h>
//
//#pragma comment (lib, "d3d9.lib")
//#pragma comment (lib, "d3dx9.lib")
//
//#include <cache.hpp>
//#include <lru_cache_policy.hpp>
//
//
//class texture_pool
//{
//public:
//
//	static constexpr size_t cache_size = 128;
//	static constexpr size_t fast_cache_size = 8;
//	static constexpr size_t max_textures_on_screen = 20;
//
//	texture_pool()
//	{
//
//	}
//
//	using texture_idx = int;
//
//	struct texture_cell_t
//	{
//		enum class texture_status_t : int
//		{
//			disk,
//			memory,
//			video
//		}
//		status;
//
//		std::string path;
//
//		void* data;
//		size_t size;
//
//		IDirect3DTexture9* texture;
//	};
//
//	std::vector<texture_cell_t> full_texture_list;
//	std::vector<texture_idx> hot_textures;
//
//	int get_texture_counter = 0;
//	int on_screen_textures_start = 0;
//
//	struct ready_texture_t
//	{
//		struct data_t
//		{
//			mutable IDirect3DTexture9** texture;
//			void* data;
//			size_t size;
//			texture_cell_t::texture_status_t* status;
//		}
//		data;
//
//
//		static bool is_multithreaded;
//		static std::list<data_t> load_queue;
//		static std::mutex load_lock;
//
//		static void texture_loader_routine();
//		bool load(bool future = true, bool high_priority = false);
//		void release();
//
//		ready_texture_t(
//			IDirect3DTexture9** texture,
//			void* data,
//			size_t size,
//			texture_cell_t::texture_status_t* status
//		) :
//			data({ texture, data, size, status })
//		{
//			assert(!!status);
//		}
//
//		ready_texture_t() : data({ 0 })
//		{
//
//		}
//
//		ready_texture_t(const ready_texture_t& e)
//		{
//			this->operator=(e);
//		}
//
//		ready_texture_t& operator=(const ready_texture_t& e)
//		{
//			memcpy(this, &e, sizeof(*this));
//			e.data.texture = nullptr;
//
//			return *this;
//		}
//
//		~ready_texture_t()
//		{
//			this->release();
//		}
//	};
//
//	using cache_t = typename caches::fixed_sized_cache<texture_idx, ready_texture_t, caches::LRUCachePolicy<texture_idx>>;
//
//	cache_t textures_cache = cache_t(cache_size);
//
//
//	texture_idx add_texture(std::string path);
//
//	void begin()
//	{
//		get_texture_counter = 0;
//	}
//	void load_texture(texture_cell_t& texture);
//	void load_all_textures();
//
//	void on_reset_device();
//	void on_lost_device();
//
//	IDirect3DTexture9* get_texture(texture_idx id);
//
//	using filter_fn = std::function<bool(texture_idx id)>;
//
//	void update_hot_list(filter_fn filter);
//	void filter_hot_list(filter_fn filter);
//};
