#include <filesystem>

using namespace std::filesystem;

static path basePath;

void SetBasePathForFile(const char *pFileName)
{
	// Note: nested include files are not supported
	basePath = path(pFileName);
	basePath.remove_filename();
}

void GetFilePath(const char* pFileName, char* pFullName)
{
	path path = basePath;
	path.append(pFileName);

	strcpy(pFullName, path.u8string().c_str());
}
