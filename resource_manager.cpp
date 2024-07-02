#define WIN32_LEAN_AND_MEAN

#include "resource_manager.h"
//#include "tls_client\httplib.h" //lool useless
#include "globals.h"
#include <filesystem>

__forceinline unsigned crc32(void* buffer, size_t size)
{
	auto crc32 = (unsigned)~0;

	for (auto i = 0; i < size; ++i)
		crc32 = (crc32 >> 8) ^ crc32_table[(crc32 ^ ((unsigned char*)buffer)[i]) & UCHAR_MAX];

	return ~crc32;
}

void resource_manager::download(std::string link)
{
    auto path_resources = getenv(crypt_str("APPDATA")) + std::string(crypt_str("\\Legendware\\"));

    if (!std::filesystem::exists(path_resources))
        std::filesystem::create_directories(path_resources);

    if (!std::filesystem::exists(path_resources + crypt_str("resources\\")))
        std::filesystem::create_directories(path_resources + crypt_str("resources\\"));

    if (!std::filesystem::exists(path_resources + crypt_str("resources\\fonts\\")))
        std::filesystem::create_directories(path_resources + crypt_str("resources\\fonts\\"));

    if (!std::filesystem::exists(path_resources + crypt_str("resources\\images\\")))
        std::filesystem::create_directories(path_resources + crypt_str("resources\\images\\"));

    if (std::filesystem::exists(path_resources + link)) { //if not exists then crash ups
        FILE* file = nullptr;
        auto status = fopen_s(&file, (path_resources + link).c_str(), crypt_str("rb"));
        if (file) {
            fseek(file, 0, SEEK_END);
            int size = ftell(file);
            fseek(file, 0, SEEK_SET);
            data_info data_temp;
            data_temp.size = size;
            data_temp.buffer = new uint8_t[size];
            fread(data_temp.buffer, 1, size, file);
            fclose(file);
            m_data[link] = data_temp;
        }
    }
}
void resource_manager::start_download()
{
	static bool started = false;
	if (started)
		return;
	else
		started = true;
	if (m_is_done)
		return;

    //hq skibidi fixB)) by micock 

	for (auto& resource : resources)
		download(resource);
	
	m_is_done = true;
}