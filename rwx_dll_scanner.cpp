#include <windows.h>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#define MINIMAL_SECTION_SIZE 0x64000

bool readFile(const std::string& filename, std::vector<uint8_t>& fileData) {
	std::ifstream file(filename, std::ios::binary);
	if (!file) {
		return false;
	}

	file.seekg(0, std::ios::end);
	std::streampos fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	fileData.resize(fileSize);
	file.read((char*)fileData.data(), fileSize);

	file.close();

	return true;
}

int main() {
	std::vector<uint8_t> dlls = {};
	if (!readFile("dlls.txt", dlls)) {
		std::cerr << "Error opening dlls.txt file!" << std::endl;
	} else {
		std::istringstream iss({ dlls.begin(), dlls.end() });
		std::string dll = {};
		while (std::getline(iss, dll)) {
			if (dll.ends_with('\r')) {
				dll.pop_back();
			}

			HANDLE file = CreateFileA(dll.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (file == INVALID_HANDLE_VALUE) {
				// std::cerr << "Error opening " << dll << " file!" << std::endl;
				continue;
			}

			HANDLE mapping = CreateFileMappingA(file, NULL, PAGE_READONLY, 0, 0, NULL);
			if (mapping == NULL) {
				// std::cerr << "Failed to create file mapping object for " << dll << " file!" << std::endl;
				CloseHandle(file);
				continue;
			}

			PVOID view = MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0);
			if (view == NULL) {
				// std::cerr << "Failed to map view of file for " << dll << " file!" << std::endl;
				CloseHandle(mapping);
				CloseHandle(file);
				continue;
			}

			PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)view;
			if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
				// std::cerr << "Invalid DOS header in " << dll << " file!" << std::endl;
				UnmapViewOfFile(view);
				CloseHandle(mapping);
				CloseHandle(file);
				continue;
			}

			PIMAGE_NT_HEADERS32 ntHeaders = (PIMAGE_NT_HEADERS32)((PBYTE)view + dosHeader->e_lfanew);
			if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) {
				// std::cerr << "Invalid NT header in " << dll << " file!" << std::endl;
				UnmapViewOfFile(view);
				CloseHandle(mapping);
				CloseHandle(file);
				continue;
			}
			
			bool isRWX = false, isX32 = true;

			switch (ntHeaders->OptionalHeader.Magic) {
			case IMAGE_NT_OPTIONAL_HDR32_MAGIC: {
					DWORD securityDirAddr = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY].VirtualAddress,
						securityDirSize = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY].Size;
					if (securityDirAddr == 0 || securityDirSize == 0) {
						UnmapViewOfFile(view);
						CloseHandle(mapping);
						CloseHandle(file);
						continue;
					}

					PIMAGE_SECTION_HEADER sectionHeader = (PIMAGE_SECTION_HEADER)((PBYTE)ntHeaders + sizeof(IMAGE_NT_HEADERS32));
					for (int i = 0; i < ntHeaders->FileHeader.NumberOfSections; ++i, ++sectionHeader) {
						if (sectionHeader->Misc.VirtualSize >= MINIMAL_SECTION_SIZE &&
							sectionHeader->Characteristics & IMAGE_SCN_MEM_READ &&
							sectionHeader->Characteristics & IMAGE_SCN_MEM_WRITE &&
							sectionHeader->Characteristics & IMAGE_SCN_MEM_EXECUTE) {
							isRWX = true;
							std::string sectionName = { sectionHeader->Name, sectionHeader->Name + 8 };
							std::cout << "section" << i << ": \"" << sectionName.c_str() << "\"" << std::endl;
						}
					}
				}
				break;
			case IMAGE_NT_OPTIONAL_HDR64_MAGIC: {
					isX32 = false;
					PIMAGE_NT_HEADERS64 ntHeaders64 = (PIMAGE_NT_HEADERS64)ntHeaders;

					DWORD securityDirAddr = ntHeaders64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY].VirtualAddress,
						securityDirSize = ntHeaders64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY].Size;
					if (securityDirAddr == 0 || securityDirSize == 0) {
						UnmapViewOfFile(view);
						CloseHandle(mapping);
						CloseHandle(file);
						continue;
					}

					PIMAGE_SECTION_HEADER sectionHeader = (PIMAGE_SECTION_HEADER)((PBYTE)ntHeaders64 + sizeof(IMAGE_NT_HEADERS64));
					for (int i = 0; i < ntHeaders64->FileHeader.NumberOfSections; ++i, ++sectionHeader) {
						if (sectionHeader->Misc.VirtualSize >= MINIMAL_SECTION_SIZE &&
							sectionHeader->Characteristics & IMAGE_SCN_MEM_READ &&
							sectionHeader->Characteristics & IMAGE_SCN_MEM_WRITE &&
							sectionHeader->Characteristics & IMAGE_SCN_MEM_EXECUTE) {
							isRWX = true;
							std::string sectionName = { sectionHeader->Name, sectionHeader->Name + 8 };
							std::cout << "section" << i << ": \"" << sectionName.c_str() << "\"" << std::endl;
						}
					}
				}
				break;
			}

			if (!isRWX) {
				UnmapViewOfFile(view);
				CloseHandle(mapping);
				CloseHandle(file);
				continue;
			}

			std::cout << (isX32 ? "32-bit" : "64-bit") << " dll: \"" << dll << "\"" << std::endl << std::endl;
		}
	}
	system("pause");
}
