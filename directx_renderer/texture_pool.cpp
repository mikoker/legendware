//#include "texture_pool.hpp"
//#include "../includes.h"
//#include "../data.h"
//#include "../render.h"
//bool texture_pool::ready_texture_t::is_multithreaded = false;
//std::list<texture_pool::ready_texture_t::data_t> texture_pool::ready_texture_t::load_queue = std::list<texture_pool::ready_texture_t::data_t>();
//std::mutex texture_pool::ready_texture_t::load_lock;
//
//IDirect3DTexture9* texture_pool::get_texture(texture_idx id)
//{
//	if (get_texture_counter++ >= cache_size)
//		return nullptr;
//
//	auto& cell = full_texture_list[id];
//
//	// loading currently
//	if (cell.status == texture_cell_t::texture_status_t::disk)
//	{
//		load_texture(cell);
//	}
//
//	if (cell.status == texture_cell_t::texture_status_t::memory &&
//		!textures_cache.Cached(id))
//	{
//		auto new_texure = ready_texture_t(
//			&cell.texture,
//			cell.data,
//			cell.size,
//			&cell.status
//		);
//		if (new_texure.load(
//			true,
//			false
//		)) //high priority to visible stuff
//		{
//			textures_cache.Put(id, new_texure);
//			new_texure.data.texture = nullptr;
//		}
//	}
//
//	if (cell.status == texture_cell_t::texture_status_t::video)
//	{
//		if (textures_cache.Cached(id))
//		{
//			//touch for lru
//			textures_cache.Get(id);
//		}
//		else
//		{
//			//multithread bug shit
//
//			ready_texture_t texture(
//				&cell.texture,
//				nullptr,
//				0,
//				&cell.status
//			);
//
//			texture.release();
//			return nullptr;
//		}
//
//		return cell.texture;
//	}
//
//
//	return nullptr;
//}
//
//void texture_pool::update_hot_list(filter_fn filter)
//{
//	full_texture_list.shrink_to_fit();
//
//	hot_textures.clear();
//	hot_textures.reserve(full_texture_list.size());
//
//	for (int i = 0, s = full_texture_list.size(); i < s; i++)
//	{
//		if (filter(i))
//		{
//			hot_textures.emplace_back(i);
//		}
//	}
//}
//
//void texture_pool::filter_hot_list(filter_fn filter)
//{
//	for (auto it = hot_textures.begin(); it != hot_textures.end();)
//	{
//		if (filter(*it))
//		{
//			it++;
//		}
//		else
//		{
//			it = hot_textures.erase(it);
//		}
//	}
//}
//
//void texture_pool::on_lost_device()
//{
//	textures_cache.Clear();
//
//}
//
//void texture_pool::on_reset_device()
//{
//	for (auto t : textures_cache)
//		t.second.load();
//}
//
//texture_pool::texture_idx texture_pool::add_texture(std::string path)
//{
//	texture_idx idx = full_texture_list.size();
//
//	texture_cell_t cell;
//
//	cell.status = texture_cell_t::texture_status_t::disk;
//	cell.path = path;
//	cell.data = nullptr;
//	cell.size = 0;
//	cell.texture = nullptr;
//
//	full_texture_list.push_back(cell);
//
//	return idx;
//}
//
//void texture_pool::load_texture(texture_cell_t& texture)
//{
//	static std::mutex load_lock;
//	const auto fs = basefilesys.get();
//	load_lock.lock();
//
//	if (texture.status == texture_cell_t::texture_status_t::disk)
//	{
//		if (auto handle = fs->Open(
//			texture.path.c_str(), crypt_str("r"), crypt_str("GAME")
//		))
//		{
//			size_t size = fs->Size(handle);
//			auto data = new char[size];
//
//			fs->Read(data, size, handle);
//			fs->Close(handle);
//
//			texture.data = data;
//			texture.size = size;
//
//			texture.status = texture_cell_t::texture_status_t::memory;
//
//			//clear memory
//			texture.path.clear();
//			texture.path.shrink_to_fit();
//		}
//	}
//
//	load_lock.unlock();
//}
//
//
//void texture_pool::load_all_textures()
//{
//	for (auto& texture : full_texture_list)
//	{
//		load_texture(texture);
//	}
//}
//
//void texture_pool::ready_texture_t::texture_loader_routine()
//{
//	if (!render->device)
//		return;
//	if (!is_multithreaded)
//	{
//
//		D3DDEVICE_CREATION_PARAMETERS params;
//		render->device->GetCreationParameters(&params);
//
//		is_multithreaded = params.BehaviorFlags & D3DCREATE_MULTITHREADED;
//		return;
//	}
//
//	load_lock.lock();
//
//	if (!load_queue.empty())
//	{
//		auto texture = load_queue.front();
//
//		if (texture.texture)
//			reinterpret_cast<ready_texture_t*>(&texture)->load(false);
//
//		load_queue.pop_front();
//	}
//
//	load_lock.unlock();
//}
//
//bool texture_pool::ready_texture_t::load(bool future, bool high_priority)
//{
//	if (!data.texture || *data.texture)
//		return false;
//
//	if (future && is_multithreaded)
//	{
//		auto lock = std::lock_guard(load_lock);
//
//		if (high_priority)
//		{
//			load_queue.push_front(data);
//		}
//		else
//		{
//			load_queue.push_back(data);
//		}
//	}
//	else
//	{
//		auto result = D3DXCreateTextureFromFileInMemory(
//			render->device.get(),
//			data.data,
//			data.size,
//			data.texture
//		);
//
//		if (result != D3D_OK)
//			return false;
//
//		*data.status = texture_cell_t::texture_status_t::video;
//	}
//
//	return true;
//}
//
//void texture_pool::ready_texture_t::release()
//{
//	if (data.texture && *data.texture)
//	{
//		load_lock.lock();
//
//		for (auto it = load_queue.begin(); it != load_queue.end(); it++)
//		{
//			if (it->data == data.data)
//			{
//				load_queue.erase(it);
//				break;
//			}
//		}
//
//		(*data.texture)->Release();
//		(*data.texture) = nullptr;
//
//		*data.status = texture_cell_t::texture_status_t::memory;
//
//		load_lock.unlock();
//	}
//}
